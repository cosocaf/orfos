#ifndef ORFOS_KERNEL_FS_SUPER_BLOCK_H_
#define ORFOS_KERNEL_FS_SUPER_BLOCK_H_

#pragma once

#include <cstdint>

namespace orfos::kernel::fs {
  constexpr uint64_t FS_MAGIC = 0x10203040;
  constexpr uint64_t FS_SIZE  = 2000;

  struct SuperBlock {
    uint32_t magic;
    uint32_t size;
    uint32_t numBlocks;
    uint32_t numInodes;
    uint32_t numLogs;
    uint32_t logStart;
    uint32_t inodeStart;
    uint32_t bmapStart;

    void read(uint32_t device);
  };

  extern SuperBlock superBlock;
} // namespace orfos::kernel::fs

#endif // ORFOS_KERNEL_FS_SUPER_BLOCK_H_