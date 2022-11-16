#include "process.h"

#include <arch/config.h>
#include <fs/fs.h>
#include <fs/inode.h>
#include <ie/user_trap.h>
#include <ie/user_vector.h>
#include <lib/string.h>
#include <memory/kvm.h>
#include <mutex/lock_guard.h>

#include <cassert>

#include "cpu.h"
#include "scheduler.h"

namespace orfos::kernel::process {
  namespace {
    uint64_t nextPid;
    mutex::SpinMutex pidMutex;
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
    kernelStack     = TRAMPOLINE - 2 * PAGE_SIZE;
    pageTable       = new memory::PageTable();

    pageTable->map(TRAMPOLINE,
                   PAGE_SIZE,
                   reinterpret_cast<uint64_t>(orfos_trampoline),
                   memory::PTE_R | memory::PTE_X);

    pageTable->map(TRAPFRAME,
                   PAGE_SIZE,
                   reinterpret_cast<uint64_t>(trapFrame),
                   memory::PTE_R | memory::PTE_W);

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
} // namespace orfos::kernel::process