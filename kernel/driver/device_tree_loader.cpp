/**
 * @file device_tree_loader.cpp
 * @author cosocaf (cosocaf@gmail.com)
 * @brief device_tree_loader.hの実装
 * @version 0.1
 * @date 2022-12-05
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "device_tree_loader.h"

#include <lib/endian.h>
#include <lib/format.h>

namespace orfos::kernel::driver {
  DeviceTreeLoader::DeviceTreeLoader(const void* deviceTreeHeader)
    : deviceTreeHeader(deviceTreeHeader) {}

  lib::Result<DeviceTree, lib::FixedString<>> DeviceTreeLoader::load() {
    loadHeader();

    if (header.magic != MAGIC) {
      return lib::error(
        lib::format<lib::FixedString<>>(
          "Failed to load DTB. The magic number is an unexpected value: {}", header.magic)
          .unwrap());
    }

    const auto base                            = reinterpret_cast<uintptr_t>(deviceTreeHeader);
    const auto addressOfMemoryReservationBlock = base + header.offsetOfMemoryReservationBlock;
    const auto addressOfStructureBlock         = base + header.offsetOfDtStruct;
    const auto addressOfStringsBlock           = base + header.offsetOfDtStrings;

    auto endOfMemoryReservationBlock
      = reinterpret_cast<DeviceTreeReserveEntry*>(addressOfMemoryReservationBlock);
    while (endOfMemoryReservationBlock->address != 0 || endOfMemoryReservationBlock->size != 0) {
      ++endOfMemoryReservationBlock;
    }

    return lib::ok(DeviceTree(
      std::span(reinterpret_cast<DeviceTreeReserveEntry*>(addressOfMemoryReservationBlock),
                reinterpret_cast<DeviceTreeReserveEntry*>(endOfMemoryReservationBlock)),
      std::span(
        reinterpret_cast<DeviceTreeStructure*>(addressOfStructureBlock),
        reinterpret_cast<DeviceTreeStructure*>(addressOfStructureBlock + header.sizeOfDtStruct)),
      std::span(reinterpret_cast<char*>(addressOfStringsBlock),
                reinterpret_cast<char*>(addressOfStringsBlock + header.sizeOfDtStrings))));
  }

  void DeviceTreeLoader::loadHeader() {
    auto dth = static_cast<const DeviceTreeHeader*>(deviceTreeHeader);

    header.magic                          = lib::fromBigEndian(dth->magic);
    header.totalSize                      = lib::fromBigEndian(dth->totalSize);
    header.offsetOfDtStruct               = lib::fromBigEndian(dth->offsetOfDtStruct);
    header.offsetOfDtStrings              = lib::fromBigEndian(dth->offsetOfDtStrings);
    header.offsetOfMemoryReservationBlock = lib::fromBigEndian(dth->offsetOfMemoryReservationBlock);
    header.version                        = lib::fromBigEndian(dth->version);
    header.lastCompatibleVersion          = lib::fromBigEndian(dth->lastCompatibleVersion);
    header.bootCpuidPhysical              = lib::fromBigEndian(dth->bootCpuidPhysical);
    header.sizeOfDtStrings                = lib::fromBigEndian(dth->sizeOfDtStrings);
    header.sizeOfDtStruct                 = lib::fromBigEndian(dth->sizeOfDtStruct);
  }
} // namespace orfos::kernel::driver