#include "init.h"

#include <lib/endian.h>
#include <lib/format.h>
#include <lib/libc/string.h>

#include "device_tree_loader.h"
#include "memory/device.h"
#include "uart/device.h"

namespace orfos::kernel::driver {
  lib::Result<DeviceTree, lib::FixedString<>> initialize(void* dth) {
    DeviceTreeLoader devTreeLoader(dth);
    auto&& loadResult = devTreeLoader.load();
    if (!loadResult) {
      return lib::error(loadResult.unwrapErr());
    }
    auto& deviceTree = loadResult.unwrap();

    auto memoryNode = deviceTree.findNode("/memory");
    if (!memoryNode) {
      return lib::error("Memory device not found.");
    }
    auto memoryPropReg = memoryNode->findProperty("reg");
    if (!memoryPropReg) {
      return lib::error("The 'reg' property does not exist on the memory device.");
    }
    for (size_t i = 0; i < memoryPropReg->getSize(); i += 16) {
      auto base    = memoryPropReg->getValueAsAddress() + i;
      auto address = lib::fromBigEndian(*reinterpret_cast<uint64_t*>(base));
      auto size    = lib::fromBigEndian(*reinterpret_cast<uint64_t*>(base + 8));
      memory::registerMemory(static_cast<uintptr_t>(address), static_cast<size_t>(size));
    }

    auto chosenNode = deviceTree.findNode("/chosen");
    if (!chosenNode) {
      return lib::error("Chosen node not found.");
    }

    auto stdoutPath = chosenNode->findProperty("stdout-path");
    if (!stdoutPath) {
      return lib::error("The 'stdout-path' property does not exist on the chosen node.");
    }
    auto stdoutPathStr = stdoutPath->getValueAsString();
    auto stdoutNode    = deviceTree.findNode(stdoutPathStr);
    if (!stdoutNode) {
      return lib::error(
        lib::format<lib::FixedString<>>("'{}' device not found.", stdoutPathStr).unwrap());
    }

    auto uartInitResult = uart::initialize(*stdoutNode);
    if (!uartInitResult) {
      return lib::error(uartInitResult.unwrapErr());
    }

    return lib::ok(std::move(deviceTree));
  }
} // namespace orfos::kernel::driver