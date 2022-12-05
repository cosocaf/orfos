#include <arch/riscv.h>
#include <driver/device_tree.h>
#include <driver/device_tree_loader.h>

namespace orfos::kernel::boot {
  [[noreturn]] void primaryMain() {
    driver::DeviceTreeLoader devTreeLoader(static_cast<void*>(arch::stval));
    auto& devTree = devTreeLoader.load().unwrap();

    auto stdoutPath = devTree.findNode("/chosen")->findProperty("stdout-path")->getValueAsString();

    auto uart                  = *devTree.findNode(stdoutPath);
    [[maybe_unused]] auto name = uart.getName();
    [[maybe_unused]] auto addr = uart.getUnitAddress().value_or(-1);

    while (true) {
    }
  }
  [[noreturn]] void secondaryMain() {
    while (true) {
    }
  }
} // namespace orfos::kernel::boot