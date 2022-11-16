#ifndef ORFOS_KERNEL_FS_BLOCK_H_
#define ORFOS_KERNEL_FS_BLOCK_H_

#pragma once

#include <cstdint>

namespace orfos::kernel::fs {
  constexpr uint64_t BLOCK_SIZE     = 1024;
  constexpr uint64_t BITS_PER_BLOCK = BLOCK_SIZE * 8;
  uint32_t allocateBlock(uint32_t device);
  void freeBlock(uint32_t device, uint32_t block);
  void fillBlock(uint32_t device, uint32_t block, int ch = 0);
} // namespace orfos::kernel::fs

#endif // ORFOS_KERNEL_FS_BLOCK_H_