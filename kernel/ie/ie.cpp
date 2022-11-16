#include "ie.h"

#include <arch/riscv.h>

#include "kernel_vector.h"
#include "plic.h"

namespace orfos::kernel::ie {
  void initializeHart() {
    arch::write_stvec(reinterpret_cast<uint64_t>(orfos_kernelVector));
  }
} // namespace orfos::kernel::ie