#include "fork.h"

#include <console/console.h>
#include <process/cpu.h>

namespace orfos::kernel::syscall {
  uint64_t fork() {
    auto proc    = process::Cpu::current().process;
    auto newProc = proc->fork();
    if (newProc == nullptr) {
      return -1;
    }

    return newProc->pid;
  }
} // namespace orfos::kernel::syscall