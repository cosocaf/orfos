#include "wait.h"

#include <process/cpu.h>

#include "args.h"

namespace orfos::kernel::syscall {
  uint64_t wait() {
    auto proc = process::Cpu::current().process;
    uint64_t ptr;
    getArgAddress(0, &ptr);
    return proc->wait(ptr) ? 0 : -1;
  }
} // namespace orfos::kernel::syscall