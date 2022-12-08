#include "timer.h"

#include <arch/riscv.h>

#include "clint.h"
#include "timer_vector.h"

namespace orfos::kernel::ie {
  void initTimer(Scratch* scratch) {
    scratch->mtimecmp = &mtimecmp;
    scratch->interval = 1'000'000;
    mtimecmp          = mtime + scratch->interval;
    arch::mscratch    = scratch;
    arch::mtvec       = orfos_timerVector;
    arch::mie         = arch::mie | arch::MIE_MTIE;
  }
} // namespace orfos::kernel::ie