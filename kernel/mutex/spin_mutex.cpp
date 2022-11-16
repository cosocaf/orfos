#include "spin_mutex.h"

#include <arch/riscv.h>
#include <arch/riscv_macros.h>
#include <process/cpu.h>

#include <cassert>

namespace orfos::kernel::mutex {
  SpinMutex::SpinMutex() : locked(false), hartid(-1) {}
  void SpinMutex::lock() {
    push();
    assert(!holding());
    while (__sync_lock_test_and_set(&locked, true)) {
      // spin lock
    }

    __sync_synchronize();
    hartid = arch::read_tp();
  }
  void SpinMutex::unlock() {
    assert(holding());

    hartid = -1;

    __sync_synchronize();
    __sync_lock_release(&locked);

    pop();
  }
  void SpinMutex::push() {
    // 割り込みが有効化されているかを確認
    auto sstatus = arch::read_sstatus();
    bool old     = (sstatus & SSTATUS_SIE) != 0;

    // 割り込みを無効化
    arch::write_sstatus(sstatus & ~SSTATUS_SIE);

    auto& cpu = process::Cpu::current();
    if (cpu.numLocks == 0) {
      cpu.interruptEnabled = old;
    }
    ++cpu.numLocks;
  }
  void SpinMutex::pop() {
    // 割り込みが有効化されていないことを確認
    auto sstatus = arch::read_sstatus();
    assert((sstatus & SSTATUS_SIE) == 0);

    auto& cpu = process::Cpu::current();
    assert(cpu.numLocks > 0);
    --cpu.numLocks;

    if (cpu.numLocks == 0 && cpu.interruptEnabled) {
      sstatus = arch::read_sstatus();
      arch::write_sstatus(sstatus | SSTATUS_SIE);
    }
  }
  bool SpinMutex::holding() {
    return locked && hartid == arch::read_tp();
  }
} // namespace orfos::kernel::mutex