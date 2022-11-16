#include "cpu.h"

#include <arch/config.h>
#include <arch/riscv.h>

namespace orfos::kernel::process {
  namespace {
    Cpu cpus[NUM_CPUS];
  }
  Cpu& Cpu::current() {
    return cpus[arch::read_tp()];
  }
} // namespace orfos::kernel::process