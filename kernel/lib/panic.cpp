#include "panic.h"

#include <arch/riscv.h>
#include <console/console.h>

namespace orfos::kernel::lib {
  bool panicked = false;
  [[noreturn]] void panic(const char* msg) {
    panicked = true;
    console::printf("kernel panic: %s\n", msg);
    while (true) {
      arch::wfi();
    }
  }
} // namespace orfos::kernel::lib