#include "user_trap.h"

#include <arch/config.h>
#include <arch/riscv.h>
#include <lib/panic.h>
#include <process/cpu.h>
#include <process/sleep.h>
#include <syscall/syscall.h>

#include <cassert>

#include "cause.h"
#include "kernel_vector.h"
#include "user_vector.h"

namespace orfos::kernel::ie {
  namespace {
    void userTrap() {
      assert((arch::read_sstatus() & SSTATUS_SPP) == 0);

      arch::write_stvec(reinterpret_cast<uint64_t>(orfos_kernelVector));

      auto proc            = process::Cpu::current().process;
      proc->trapFrame->epc = arch::read_sepc();

      auto scause = arch::read_scause();
      // syscall
      if (scause == 8) {
        proc->trapFrame->epc += 4;
        arch::write_sstatus(arch::read_sstatus() | SSTATUS_SIE);
        syscall::syscall();
      } else if (auto cause = which(scause); cause == Cause::Unexpected) {
        console::printf("Unexpected interrupt/exception\n");
        console::printf("scause %p, sepc %p, stval %p, pid %d\n",
                        scause,
                        arch::read_sepc(),
                        arch::read_stval(),
                        proc->pid);
        lib::panic("unexpected ie");
      } else if (cause == Cause::Timer) {
        process::yield();
      }

      returnUserTrap();
    }
  } // namespace
  void returnUserTrap() {
    auto proc = process::Cpu::current().process;

    arch::write_sstatus(arch::read_sstatus() & ~SSTATUS_SIE);

    uint64_t trampolineUserVector
      = TRAMPOLINE
        + (reinterpret_cast<uint64_t>(orfos_userVector)
           - reinterpret_cast<uint64_t>(orfos_trampoline));
    arch::write_stvec(trampolineUserVector);

    proc->trapFrame->kernel_satp   = arch::read_satp();
    proc->trapFrame->kernel_sp     = proc->kernelStack + PAGE_SIZE;
    proc->trapFrame->kernel_trap   = reinterpret_cast<uint64_t>(userTrap);
    proc->trapFrame->kernel_hartid = arch::read_tp();

    auto sstatus = arch::read_sstatus();
    sstatus &= ~SSTATUS_SPP;
    sstatus |= SSTATUS_SPIE;
    arch::write_sstatus(sstatus);

    arch::write_sepc(proc->trapFrame->epc);

    auto satp = proc->pageTable->makeSatp();

    uint64_t trampolineReturnUser
      = TRAMPOLINE
        + (reinterpret_cast<uint64_t>(orfos_returnUser)
           - reinterpret_cast<uint64_t>(orfos_trampoline));

    reinterpret_cast<void (*)(uint64_t)>(trampolineReturnUser)(satp);
  }
} // namespace orfos::kernel::ie