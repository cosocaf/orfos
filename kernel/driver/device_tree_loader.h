/**
 * @file device_tree_loader.h
 * @author cosocaf (cosocaf@gmail.com)
 * @brief デバイスツリーを読み込むための機能を提供する
 * @version 0.1
 * @date 2022-12-05
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef ORFOS_KERNEL_DRIVER_DEVICE_TREE_LOADER_H_
#define ORFOS_KERNEL_DRIVER_DEVICE_TREE_LOADER_H_

#pragma once

#include <lib/fixed_string.h>
#include <lib/result.h>

#include "device_tree.h"

namespace orfos::kernel::driver {
  struct DeviceTreeHeader {
    // ビッグエンディアンで0xD00DFEED固定。
    uint32_t magic;
    // DTBの合計サイズ。構造体のすべてのセクションが含まれる。
    uint32_t totalSize;
    // ヘッダの先頭からの構造ブロックへのオフセット。
    uint32_t offsetOfDtStruct;
    // ヘッダの先頭からの文字列ブロックへのオフセット。
    uint32_t offsetOfDtStrings;
    // ヘッダの先頭からのメモリ予約ブロックへのオフセット。
    uint32_t offsetOfMemoryReservationBlock;
    // DTBのバージョン。
    uint32_t version;
    // 互換性のあるバージョンのうち最も低いバージョン。
    uint32_t lastCompatibleVersion;
    // システムのブートCPUの物理ID。
    uint32_t bootCpuidPhysical;
    // DTBの文字列ブロックセクションの長さ。
    uint32_t sizeOfDtStrings;
    // DTBの構造ブロックセクションの長さ
    uint32_t sizeOfDtStruct;
  };

  /**
   * @brief デバイスツリーを読み込む
   *
   * このクラスはカーネル起動直後に利用されることを想定している。
   * そのため、動的なメモリ確保は行わない。
   *
   */
  class DeviceTreeLoader {
    constexpr static uint32_t MAGIC = 0xD00DFEED;

    const void* deviceTreeHeader;

    DeviceTreeHeader header;

  public:
    /**
     * @brief DeviceTreeLoaderを構築する。
     *
     * orfos::kernel::boot::primaryMainが呼ばれる時点での
     * stvalレジスタにデバイスツリーヘッダへのポインタが格納されている。
     *
     * @param deviceTreeHeader デバイスツリーヘッダへのポインタ
     */
    DeviceTreeLoader(const void* deviceTreeHeader);

    /**
     * @brief デバイスツリーを読み込む
     *
     * @return lib::Result<DeviceTree, lib::FixedString<>>
     * 成功した場合はデバイスツリーを、失敗した場合はその原因を文字列で返す
     */
    lib::Result<DeviceTree, lib::FixedString<>> load();

  private:
    void loadHeader();
  };
} // namespace orfos::kernel::driver

#endif // ORFOS_KERNEL_DRIVER_DEVICE_TREE_LOADER_H_