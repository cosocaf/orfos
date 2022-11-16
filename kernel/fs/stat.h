#ifndef ORFOS_KERNEL_FS_STAT_H_
#define ORFOS_KERNEL_FS_STAT_H_

#pragma once

#include <cstdint>

namespace orfos::kernel::fs {
  constexpr uint32_t T_DIR    = 1;
  constexpr uint32_t T_FILE   = 2;
  constexpr uint32_t T_DEVICE = 3;

  struct Stat {
    uint32_t device;
    uint32_t inum;
    int16_t type;
    int16_t numLinks;
    uint64_t size;
  };
} // namespace orfos::kernel::fs

#endif // ORFOS_KERNEL_FS_STAT_H_