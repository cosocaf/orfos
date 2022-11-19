#include <arch/riscv.h>
#include <arch/riscv_macros.h>
#include <console/console.h>
#include <lib/panic.h>
#include <process/cpu.h>
#include <process/sleep.h>

#include <cassert>

#include "cause.h"

namespace orfos::kernel::ie {
  void kernelTrap() {
    auto sepc    = arch::read_sepc();
    auto sstatus = arch::read_sstatus();
    auto scause  = arch::read_scause();

    assert(sstatus & SSTATUS_SPP);
    assert(!(sstatus & SSTATUS_SIE));

    auto cause = which(scause);
    if (cause == Cause::Unexpected) {
      auto stval = arch::read_stval();
      console::printf("Kernel trap: Unexpected interrupt/exception\n");
      console::printf("scause %p, sepc %p, stval %p\n", scause, sepc, stval);
      lib::panic("unexpected ie");
    }

    if (cause == Cause::Timer) {
      process::yield();
    }

    arch::write_sepc(sepc);
    arch::write_sstatus(sstatus);
  }
} // namespace orfos::kernel::ie

extern "C" {
void orfos_kernelTrap() {
  orfos::kernel::ie::kernelTrap();
}
}