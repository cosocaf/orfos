#include <arch/riscv.h>
#include <driver/init.h>
#include <driver/uart/device.h>
#include <lib/panic.h>

namespace orfos::kernel::boot {
  [[noreturn]] void primaryMain() {
    auto driverInitResult = driver::initialize(static_cast<void*>(arch::stval));
    if (!driverInitResult) {
      panic(driverInitResult.unwrapErr().c_str());
    }

    for (auto c : "Hello, World!\n") {
      driver::uart::putc(c);
    }

    while (true) {
    }
  }
  [[noreturn]] void secondaryMain() {
    while (true) {
    }
  }
} // namespace orfos::kernel::boot