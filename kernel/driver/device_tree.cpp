#include "device_tree.h"

#include <lib/align.h>
#include <lib/endian.h>
#include <lib/format.h>
#include <lib/libc/string.h>

namespace orfos::kernel::driver {
  DeviceTree::DeviceTree(uint64_t fdt)
    : fdtHeader(fdt),
      addressOfMemoryReservationBlock(0),
      addressOfStructureBlock(0),
      addressOfStringsBlock(0) {
    header = *reinterpret_cast<DeviceTreeHeader*>(fdt);
  }

  lib::Result<DeviceTree, lib::FixedString<>> DeviceTree::load(uint64_t fdt) {
    DeviceTree deviceTree(fdt);

    for (size_t i = 0; i < sizeof(deviceTree.header) / sizeof(uint32_t); ++i) {
      auto& member = reinterpret_cast<uint32_t*>(&deviceTree.header)[i];
      member       = lib::fromBigEndian(reinterpret_cast<uint32_t*>(fdt)[i]);
    }

    if (deviceTree.header.magic != MAGIC) {
      return lib::error(
        lib::format<lib::FixedString<>>(
          "Failed to load DTB. The magic number is an unexpected value: {}",
          deviceTree.header.magic)
          .unwrap());
    }

    deviceTree.addressOfMemoryReservationBlock
      = fdt + deviceTree.header.offsetOfMemoryReserveMap;
    deviceTree.addressOfStructureBlock
      = fdt + deviceTree.header.offsetOfDtStruct;
    deviceTree.addressOfStringsBlock
      = fdt + deviceTree.header.offsetOfDtStrings;

    return lib::ok(deviceTree);
  }

  lib::Result<const char*, lib::FixedString<>> DeviceTree::getString(
    uint32_t offset) const {
    if (offset >= header.sizeOfDtStrings) {
      return lib::error("Offset is out of strings block bounds.");
    }
    return lib::ok(
      reinterpret_cast<const char*>(addressOfStringsBlock + offset));
  }

  DeviceTreeStructureBlock DeviceTree::getStructureBlock() {
    return DeviceTreeStructureBlock(
      this,
      addressOfStructureBlock,
      addressOfStructureBlock + header.sizeOfDtStruct);
  }

  DeviceTreeStructureBlock::DeviceTreeStructureBlock(DeviceTree* dt,
                                                     uint64_t begin,
                                                     uint64_t end)
    : dt(dt), blockBegin(begin), blockEnd(end) {}

  DeviceTreeStructureBlock::Iterator::Iterator(
    const DeviceTreeStructureBlock* thiz,
    uint64_t cur,
    uint64_t end)
    : thiz(thiz), cur(cur), end(end) {}

  DeviceTreeStructureVariant DeviceTreeStructureBlock::Iterator::operator*() {
    auto src = reinterpret_cast<DeviceTreeStructure*>(cur);
    DeviceTreeStructureVariant dst;
    const auto token = lib::fromBigEndian(src->token);
    switch (token) {
      case 0x01:
        dst = DeviceTreeBeginNodeStructure{
          .unitName = reinterpret_cast<const char*>(&src->beginNode.unitName)
        };
        break;
      case 0x02:
        dst = DeviceTreeEndNodeStructure{};
        break;
      case 0x03: {
        uint32_t offset = lib::fromBigEndian(src->prop.offsetOfName);

        dst = DeviceTreePropStructure{
          .length       = lib::fromBigEndian(src->prop.length),
          .offsetOfName = offset,
          .name         = thiz->dt->getString(offset).unwrap(),
          .value        = reinterpret_cast<const char*>(&src->prop.value),
        };
      } break;
      case 0x04:
        dst = DeviceTreeNopStructure{};
        break;
      case 0x09:
        dst = DeviceTreeEndStructure{};
        break;
    }
    return dst;
  }
  DeviceTreeStructureBlock::Iterator&
  DeviceTreeStructureBlock::Iterator::operator++() {
    auto src   = reinterpret_cast<DeviceTreeStructure*>(cur);
    auto token = lib::fromBigEndian(src->token);
    switch (token) {
      case 0x01:
        cur += sizeof(token);
        cur += strlen(reinterpret_cast<const char*>(&src->beginNode.unitName));
        cur += 1;
        break;
      case 0x02:
        cur += sizeof(src->endNode);
        break;
      case 0x03:
        cur += sizeof(src->prop.token);
        cur += sizeof(src->prop.length);
        cur += sizeof(src->prop.offsetOfName);
        cur += lib::fromBigEndian(src->prop.length);
        break;
      case 0x04:
        cur += sizeof(src->nop);
        break;
      case 0x09:
        cur += sizeof(src->end);
    }
    cur = lib::align(cur, sizeof(uint32_t));
    if (cur >= end) {
      cur = end;
    }
    return *this;
  }
  bool DeviceTreeStructureBlock::Iterator::operator!=(
    const DeviceTreeStructureBlock::Iterator& other) {
    return cur != other.cur;
  }

  DeviceTreeStructureBlock::Iterator DeviceTreeStructureBlock::begin() const {
    return cbegin();
  }
  DeviceTreeStructureBlock::Iterator DeviceTreeStructureBlock::end() const {
    return cend();
  }
  DeviceTreeStructureBlock::Iterator DeviceTreeStructureBlock::cbegin() const {
    return Iterator(this, blockBegin, blockEnd);
  }
  DeviceTreeStructureBlock::Iterator DeviceTreeStructureBlock::cend() const {
    return Iterator(this, blockEnd, blockEnd);
  }
} // namespace orfos::kernel::driver