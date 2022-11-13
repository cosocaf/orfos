#include "main.h"

#include <arch/riscv.h>
#include <console/console.h>

namespace orfos::kernel::boot {
  namespace {
    // hartid=0の初期化が完了したかどうかのフラグ
    volatile bool started = false;
  }
  [[noreturn]] void main() {
    auto hartid = arch::read_tp();
    if (hartid == 0) {
      console::initialize();
      console::printf("ORFOS is booting...");
      __sync_synchronize();
      started = true;
    } else {
      while (!started) {
        // Wait
      }
      __sync_synchronize();
      console::printf("Hart %d is starting...", hartid);
    }
    while (true) {
    }
  }
} // namespace orfos::kernel::boot