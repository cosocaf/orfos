#include "process.h"

#include <arch/config.h>
#include <fs/device.h>
#include <fs/file.h>
#include <fs/fs.h>
#include <fs/inode.h>
#include <fs/log.h>
#include <fs/stat.h>
#include <ie/user_trap.h>
#include <ie/user_vector.h>
#include <lib/string.h>
#include <memory/kvm.h>
#include <memory/uvm.h>
#include <mutex/lock_guard.h>

#include <cassert>

#include "cpu.h"
#include "scheduler.h"
#include "sleep.h"
#include "user_init_proc.h"

namespace orfos::kernel::process {
  namespace {
    uint64_t nextPid;
    mutex::SpinMutex pidMutex;
    mutex::SpinMutex waitMutex;
    bool first = true;

    void returnFork() {
      Cpu::current().process->mutex.unlock();
      if (first) {
        first = false;
        fs::initializeFs(fs::ROOT_DEVICE);
      }

      ie::returnUserTrap();
    }
  } // namespace
  void initialize() {
    nextPid = 1;
  }

  Process::Process() {
    {
      mutex::LockGuard guard(pidMutex);
      pid = nextPid;
      ++nextPid;
    }
    state           = ProcessState::Used;
    trapFrame       = new TrapFrame();
    kernelStackPage = new char[PAGE_SIZE * 2];
    kernelStack     = TRAMPOLINE - pid * 2 * PAGE_SIZE;
    pageTable       = makePageTable();

    memory::kernelPageTable->map(kernelStack,
                                 PAGE_SIZE,
                                 reinterpret_cast<uint64_t>(kernelStackPage),
                                 memory::PTE_R | memory::PTE_W);

    memset(&context, 0, sizeof(context));
    context.ra = reinterpret_cast<uint64_t>(returnFork);
    context.sp = kernelStack + PAGE_SIZE;
  }

  Process::~Process() {
    if (trapFrame) {
      delete trapFrame;
      trapFrame = nullptr;
    }
    if (kernelStackPage) {
      delete[] kernelStackPage;
      kernelStackPage = nullptr;
    }
  }

  memory::PageTable* Process::makePageTable() {
    auto pt = new memory::PageTable();

    pt->map(TRAMPOLINE,
            PAGE_SIZE,
            reinterpret_cast<uint64_t>(orfos_trampoline),
            memory::PTE_R | memory::PTE_X);

    pt->map(TRAPFRAME,
            PAGE_SIZE,
            reinterpret_cast<uint64_t>(trapFrame),
            memory::PTE_R | memory::PTE_W);

    return pt;
  }

  int Process::open(const char* path, int mode) {
    fs::Inode* ip;
    fs::beginOp();
    if (mode & fs::O_CREATE) {
      ip = fs::Inode::create(path, fs::T_FILE, 0, 0);
      if (ip == nullptr) {
        fs::endOp();
        return -1;
      }
    } else {
      ip = fs::Inode::open(path);
      if (ip == nullptr) {
        fs::endOp();
        return -1;
      }
      ip->lock();
      if (ip->type == fs::T_DIR && mode != fs::O_RDONLY) {
        ip->unlock();
        ip->put();
        fs::endOp();
        return -1;
      }
    }

    if (ip->type == fs::T_DEVICE
        && (ip->major < 0 || ip->major >= fs::NUM_DEVICES)) {
      ip->unlock();
      ip->put();
      return -1;
    }

    auto file = fs::File::allocate();
    if (file == nullptr) {
      ip->unlock();
      ip->put();
      fs::endOp();
      return -1;
    }

    auto fd = openFiles.size();
    openFiles.push_back(file);

    if (ip->type == fs::T_DEVICE) {
      file->type         = fs::FileType::Device;
      file->device.ip    = ip;
      file->device.major = ip->major;
    } else {
      file->type         = fs::FileType::Inode;
      file->inode.ip     = ip;
      file->inode.offset = 0;
    }
    file->readable = !(mode & fs::O_WRONLY);
    file->writable = !(mode & fs::O_WRONLY) || (mode & fs::O_RDWR);

    if ((mode & fs::O_TRUNC) && ip->type == fs::T_FILE) {
      ip->truncate();
    }

    ip->unlock();
    fs::endOp();

    return fd;
  }

  int Process::close(int fd) {
    if (fd < 0 || static_cast<size_t>(fd) >= openFiles.size()) {
      return -1;
    }
    openFiles[fd]->close();
    openFiles[fd] = nullptr;
    return 0;
  }

  Process* Process::fork() {
    auto newProc = new Process();
    memory::copyUserVirtualMemory(pageTable, newProc->pageTable, memorySize);
    newProc->memorySize = memorySize;

    *(newProc->trapFrame) = *trapFrame;
    // fork()の戻り値は子プロセスの場合0
    newProc->trapFrame->a0 = 0;
    for (const auto& file : openFiles) {
      if (file) {
        newProc->openFiles.push_back(file->dup());
      } else {
        newProc->openFiles.push_back(nullptr);
      }
    }
    newProc->cwd = cwd->dup();

    strncpy(newProc->name, name, sizeof(name));

    {
      mutex::LockGuard guard(waitMutex);
      newProc->parent = this;
      children.push_back(newProc);
    }

    {
      mutex::LockGuard guard(newProc->mutex);
      newProc->state = ProcessState::Ready;
    }

    process::scheduler->registerProcess(newProc);

    return newProc;
  }

  void Process::exit(int status) {
    for (auto& file : openFiles) {
      if (file) {
        file->close();
        file = nullptr;
      }
    }
    openFiles.clear();

    fs::beginOp();
    cwd->put();
    fs::endOp();
    cwd = nullptr;

    {
      mutex::LockGuard guard(waitMutex);
      reparent();
      scheduler->wakeup(parent);
      mutex.lock();
      exitStatus = status;
      state      = ProcessState::Zombie;
    }

    scheduler->reschedule();
    // ここにはもう帰ってこない
    assert(false);
  }

  void Process::growMemory(size_t size) {
    if (size > memorySize) {
      memory::allocateUserVirtualMemory(
        pageTable, memorySize, size, memory::PTE_W);
    } else if (size < memorySize) {
      memory::deallocateUserVirtualMemory(pageTable, size, memorySize);
    }
    memorySize = size;
  }

  bool Process::wait(uint64_t address) {
    mutex::LockGuard guard(waitMutex);
    while (true) {
      if (children.empty()) {
        return -1;
      }
      for (size_t i = 0; i < children.size(); ++i) {
        auto& child = children[i];
        mutex::LockGuard guard(child->mutex);
        if (child->state == ProcessState::Zombie) {
          if (address != 0
              && !pageTable->copyout(
                address,
                reinterpret_cast<char*>(&child->exitStatus),
                sizeof(child->exitStatus))) {
            return -1;
          }
          children.erase(children.begin() + i);
          return child->pid;
        }
      }
      sleep(this, waitMutex);
    }
  }

  void Process::reparent() {
    for (auto& child : children) {
      mutex::LockGuard guard(child->mutex);
      child->parent = initProc;
      scheduler->wakeup(initProc);
    }
  }
} // namespace orfos::kernel::process