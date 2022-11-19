#include "user_init_proc.h"

#include <console/console.h>
#include <lib/string.h>
#include <memory/kvm.h>
#include <syscall/syscall.h>

#include "process.h"
#include "scheduler.h"

extern "C" {
extern char beginUserInitCode[];
extern char endUserInitCode[];
}

namespace orfos::kernel::process {
  Process* initProc = nullptr;
  void startUserInitProcess() {
    auto proc = new Process();

    auto mem = new char[PAGE_SIZE];
    memset(mem, 0, PAGE_SIZE);
    proc->pageTable->map(
      0,
      PAGE_SIZE,
      reinterpret_cast<uint64_t>(mem),
      memory::PTE_W | memory::PTE_R | memory::PTE_X | memory::PTE_U);

    memmove(mem, beginUserInitCode, endUserInitCode - beginUserInitCode);

    proc->memorySize = PAGE_SIZE;
    // ユーザプロセスのエントリポイントは0番地
    proc->trapFrame->epc = 0;
    proc->trapFrame->sp  = PAGE_SIZE;
    strncpy(proc->name, "initcode", sizeof("initcode"));
    proc->cwd   = fs::Inode::open("/");
    proc->state = ProcessState::Ready;

    initProc = proc;
    console::printf("User init process initialised.\n");
    scheduler->registerProcess(proc);
  }
} // namespace orfos::kernel::process