#ifndef ORFOS_KERNEL_DRIVER_DEVICE_TREE_H_
#define ORFOS_KERNEL_DRIVER_DEVICE_TREE_H_

#pragma once

#include <lib/fixed_string.h>

#include <optional>
#include <span>

namespace orfos::kernel::driver {
  using phandle_t = uint32_t;

  struct DeviceTreeReserveEntry {
    uint64_t address;
    uint64_t size;
  };

  enum struct DeviceTreeStructureToken : uint32_t {
    BeginNode = 0x01,
    EndNode   = 0x02,
    Prop      = 0x03,
    Nop       = 0x04,
    End       = 0x09,
  };
  union DeviceTreeStructure {
    DeviceTreeStructureToken token;
    struct {
      DeviceTreeStructureToken token;
      char unitName[];
    } beginNode;
    struct {
      DeviceTreeStructureToken token;
    } endNode;
    struct {
      DeviceTreeStructureToken token;
      uint32_t length;
      uint32_t offsetOfName;
      char value[];
    } prop;
    struct {
      DeviceTreeStructureToken token;
    } nop;
    struct {
      DeviceTreeStructureToken token;
    } end;
  };

  class StringList {
    const char* const begin;
    const char* strs;
    size_t length;

  public:
    StringList(const char* strs, size_t length);
    const char* next();
  };

  class DeviceTreeProperty {
    lib::FixedString<31> name;
    uint32_t length;
    uintptr_t value;

  public:
    DeviceTreeProperty(const char* name, uint32_t length, uintptr_t value);

    const decltype(name)& getName() const;
    uint32_t getValueAsU32() const;
    uint64_t getValueAsU64() const;
    const char* getValueAsString() const;
    phandle_t getValueAsPhandle() const;
    StringList getValueAsStringList() const;
    void* getValueAsPointer() const;
  };
  class DeviceTreeNode {
    class Iterator {
      const DeviceTreeStructure* beginStruct;
      std::span<char> stringsBlock;

    public:
      Iterator(const DeviceTreeStructure* beginStruct, std::span<char> stringsBlock);
      Iterator& operator++();
      DeviceTreeNode operator*() const;
      bool operator==(const Iterator& other) const;
    };

    const DeviceTreeStructure* beginStruct;
    std::span<char> stringsBlock;
    lib::FixedString<31> name;
    std::optional<uintptr_t> unitAddress;

  public:
    DeviceTreeNode(const DeviceTreeStructure* beginStruct, std::span<char> stringsBlock);

    const decltype(name)& getName() const;
    std::optional<uintptr_t> getUnitAddress() const;

    std::optional<DeviceTreeNode> findNode(const char* name, bool full) const;
    std::optional<DeviceTreeNode> findNode(phandle_t phandle) const;

    std::optional<DeviceTreeProperty> findProperty(const char* name) const;

    Iterator begin() const;
    Iterator end() const;

  private:
    static const DeviceTreeStructure* next(const DeviceTreeStructure* cur);
  };
  class DeviceTree {
    std::span<DeviceTreeReserveEntry> memoryReservationBlock;
    std::span<DeviceTreeStructure> structureBlock;
    std::span<char> stringsBlock;

    DeviceTreeNode rootNode;

  public:
    DeviceTree(std::span<DeviceTreeReserveEntry> memoryReservationBlock,
               std::span<DeviceTreeStructure> structureBlock,
               std::span<char> stringsBlock);

    std::optional<DeviceTreeNode> findNode(const char* path) const;
    std::optional<DeviceTreeNode> findNode(const char* name, bool full) const;
    std::optional<DeviceTreeNode> findNode(phandle_t phandle) const;
  };
} // namespace orfos::kernel::driver

#endif // ORFOS_KERNEL_DRIVER_DEVICE_TREE_H_