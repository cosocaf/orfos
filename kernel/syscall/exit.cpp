#include "exit.h"

#include <console/console.h>
#include <process/cpu.h>

#include "args.h"

namespace orfos::kernel::syscall {
  uint64_t exit() {
    int status;
    getArgInt(0, &status);
    auto proc = process::Cpu::current().process;
    console::printf(
      "[EXIT] %s status=0x%x pid=%d\n", proc->name, status, proc->pid);
    proc->exit(status);
    return 0;
  }
} // namespace orfos::kernel::syscall