#ifndef ORFOS_KERNEL_DRIVER_DEVICE_TREE_H_
#define ORFOS_KERNEL_DRIVER_DEVICE_TREE_H_

#pragma once

#include <lib/fixed_string.h>
#include <lib/result.h>

#include <variant>

namespace orfos::kernel::driver {
  class DeviceTree;

  struct DeviceTreeHeader {
    /**
     * @brief FDTのマジックナンバー
     *
     * ビッグエンディアンで0xD00DFEED固定。
     */
    uint32_t magic;
    /**
     * @brief DTBの合計サイズ
     *
     * 構造体のすべてのセクションが含まれる。
     */
    uint32_t totalSize;
    /**
     * @brief ヘッダの先頭からの構造ブロックへのオフセット
     *
     */
    uint32_t offsetOfDtStruct;
    /**
     * @brief ヘッダの先頭からの文字列ブロックへのオフセット
     *
     */
    uint32_t offsetOfDtStrings;
    /**
     * @brief ヘッダの先頭からのメモリ予約ブロックへのオフセット
     *
     */
    uint32_t offsetOfMemoryReserveMap;
    /**
     * @brief DTBのバージョン
     *
     */
    uint32_t version;
    /**
     * @brief 下位互換性のあるバージョンのうち最も低いバージョン
     *
     */
    uint32_t lastCompatibleVersion;
    /**
     * @brief システムのブートCPUの物理ID
     *
     */
    uint32_t bootCpuidPhysical;
    /**
     * @brief DTBの文字列ブロックセクションの長さ
     *
     */
    uint32_t sizeOfDtStrings;
    /**
     * @brief DTBの構造ブロックセクションの長さ
     *
     */
    uint32_t sizeOfDtStruct;
  };

  struct DeviceTreeMemoryEntry {
    uint64_t address;
    uint64_t size;
  };
  union DeviceTreeStructure {
    uint32_t token;
    struct {
      uint32_t token;
      uint32_t unitName;
    } beginNode;
    struct {
      uint32_t token;
    } endNode;
    struct {
      uint32_t token;
      uint32_t length;
      uint32_t offsetOfName;
      uint32_t value;
    } prop;
    struct {
      uint32_t token;
    } nop;
    struct {
      uint32_t token;
    } end;
  };

  struct DeviceTreeBeginNodeStructure {
    const char* unitName;
  };
  struct DeviceTreeEndNodeStructure {};
  struct DeviceTreePropStructure {
    uint32_t length;
    uint32_t offsetOfName;
    const char* name;
    const char* value;
    uint32_t u32() const;
    uint64_t u64() const;
    char* str() const;
  };
  struct DeviceTreeNopStructure {};
  struct DeviceTreeEndStructure {};

  using DeviceTreeStructureVariant = std::variant<DeviceTreeBeginNodeStructure,
                                                  DeviceTreeEndNodeStructure,
                                                  DeviceTreePropStructure,
                                                  DeviceTreeNopStructure,
                                                  DeviceTreeEndStructure>;

  class DeviceTreeStructureBlock {
    friend class DeviceTree;

    class Iterator {
      const DeviceTreeStructureBlock* thiz;
      uint64_t cur;
      uint64_t end;

    public:
      Iterator(const DeviceTreeStructureBlock* thiz,
               uint64_t cur,
               uint64_t end);
      DeviceTreeStructureVariant operator*();
      Iterator& operator++();
      bool operator!=(const Iterator& other);
    };

    DeviceTree* dt;
    uint64_t blockBegin;
    uint64_t blockEnd;

    DeviceTreeStructureBlock(DeviceTree* dt, uint64_t begin, uint64_t end);

  public:
    Iterator begin() const;
    Iterator end() const;
    Iterator cbegin() const;
    Iterator cend() const;
  };

  class DeviceTree {
    constexpr static uint32_t MAGIC = 0xd00dfeed;

    uint64_t fdtHeader;
    uint64_t addressOfMemoryReservationBlock;
    uint64_t addressOfStructureBlock;
    uint64_t addressOfStringsBlock;

    DeviceTreeHeader header;

    DeviceTree(uint64_t fdt);

  public:
    static lib::Result<DeviceTree, lib::FixedString<>> load(uint64_t fdt);
    lib::Result<const char*, lib::FixedString<>> getString(
      uint32_t offset) const;

    DeviceTreeStructureBlock getStructureBlock();
  };
} // namespace orfos::kernel::driver

#endif // ORFOS_KERNEL_DRIVER_DEVICE_TREE_H_