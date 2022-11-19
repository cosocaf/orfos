#include "timer.h"

#include <arch/config.h>
#include <arch/riscv.h>
#include <arch/riscv_macros.h>
#include <mutex/lock_guard.h>
#include <mutex/spin_mutex.h>
#include <process/sleep.h>

#include "kernel_vector.h"

namespace orfos::kernel::ie {
  namespace {
    uint64_t timerScrach[NUM_CPUS][5];
    // sleepシステムコールで使用する
    uint64_t ticks;
    mutex::SpinMutex mutex;

    inline uint64_t* clintMtimeCmp(uint64_t hartid) {
      return reinterpret_cast<uint64_t*>(CLINT + 0x4000 + 8 * hartid);
    }
  } // namespace
  void initializeTimer() {
    auto hartid = arch::read_mhartid();

    // 単位は100ns -> 100msに一回の割り込み
    constexpr uint64_t interval = 1'000'000;
    auto mtimecmp               = clintMtimeCmp(hartid);
    *mtimecmp = *reinterpret_cast<uint64_t*>(CLINT_MTIME) + interval;

    auto& scratch = timerScrach[hartid];
    scratch[3]    = reinterpret_cast<uint64_t>(clintMtimeCmp(hartid));
    scratch[4]    = interval;
    arch::write_mscratch(reinterpret_cast<uint64_t>(scratch));

    arch::write_mtvec(reinterpret_cast<uint64_t>(orfos_timerVector));

    auto mstatus = arch::read_mstatus();
    arch::write_mstatus(mstatus | MSTATUS_MIE);

    auto mie = arch::read_mie();
    arch::write_mie(mie | MIE_MTIE);
  }
  void clockInterrupt() {
    mutex::LockGuard guard(mutex);
    ++ticks;
    process::wakeup(&ticks);
  }
} // namespace orfos::kernel::ie