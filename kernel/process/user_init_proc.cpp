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
  void startUserInitProcess() {
    auto proc = new Process();

    auto mem = new char[PAGE_SIZE];
    memset(mem, 0, PAGE_SIZE);
    console::printf(
      "pre-pagetable: %p, %p\n", proc->pageTable, *proc->pageTable);
    proc->pageTable->map(
      0,
      PAGE_SIZE,
      reinterpret_cast<uint64_t>(mem),
      memory::PTE_W | memory::PTE_R | memory::PTE_X | memory::PTE_U);
    console::printf("pagetable: %p, %p\n", proc->pageTable, *proc->pageTable);

    for (auto code = beginUserInitCode; code <= endUserInitCode; ++code) {
      console::printf("%x ", *code);
    }
    console::printf("\n");

    memmove(mem, beginUserInitCode, endUserInitCode - beginUserInitCode);

    console::printf("*********************\n");

    for (long i = 0; i < endUserInitCode - beginUserInitCode; ++i) {
      console::printf("%x ", mem[i]);
    }
    console::printf("\n");

    proc->memorySize = PAGE_SIZE;
    // ユーザプロセスのエントリポイントは0番地
    proc->trapFrame->epc = 0;
    proc->trapFrame->sp  = PAGE_SIZE;
    strncpy(proc->name, "initcode", sizeof("initcode"));
    proc->cwd   = fs::Inode::open("/");
    proc->state = ProcessState::Ready;

    scheduler->registerProcess(proc);
  }
} // namespace orfos::kernel::process