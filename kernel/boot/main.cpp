#include <arch/riscv.h>
#include <driver/device_tree.h>
#include <lib/panic.h>

#include <string_view>

namespace orfos::kernel::boot {
  [[noreturn]] void primaryMain() {
    // DTよりも先にKVM作った方がいいかもしれない
    // KVMパニックすることあるっけ?
    // パニックしないなら先にKVM作ってしまおう。
    // あ、でもメモリ容量取得にDTいる気がする。。。

    auto loadResult = driver::DeviceTree::load(arch::stval);
    if (!loadResult) {
      // デバイスツリーのロードに失敗したということはUARTへの出力もできないわけだから
      // パニックしたところでその旨を表示する術はなさそう?
      panic("Failed to load device tree: %s", loadResult.unwrapErr().c_str());
    }

    auto& deviceTree = loadResult.unwrap();
    for (const auto& block : deviceTree.getStructureBlock()) {
      if (std::holds_alternative<driver::DeviceTreeBeginNodeStructure>(block)) {
        auto& beginNode = std::get<driver::DeviceTreeBeginNodeStructure>(block);
        auto& n         = beginNode;
        (void)n;
      } else if (std::holds_alternative<driver::DeviceTreeEndNodeStructure>(
                   block)) {
        auto& endNode = std::get<driver::DeviceTreeEndNodeStructure>(block);
        auto& n       = endNode;
        (void)n;
      } else if (std::holds_alternative<driver::DeviceTreePropStructure>(
                   block)) {
        auto& prop = std::get<driver::DeviceTreePropStructure>(block);
        auto& n    = prop;
        (void)n;
      } else if (std::holds_alternative<driver::DeviceTreeNopStructure>(
                   block)) {
        auto& nop = std::get<driver::DeviceTreeNopStructure>(block);
        auto& n   = nop;
        (void)n;
      } else if (std::holds_alternative<driver::DeviceTreeEndStructure>(
                   block)) {
        auto& end = std::get<driver::DeviceTreeEndStructure>(block);
        auto& n   = end;
        (void)n;
      }
    }

    while (true) {
    }
  }
  [[noreturn]] void secondaryMain() {
    while (true) {
    }
  }
} // namespace orfos::kernel::boot