#ifndef ORFOS_KERNEL_FS_LOG_H_
#define ORFOS_KERNEL_FS_LOG_H_

#pragma once

#include <mutex/spin_mutex.h>

#include <cstdint>

#include "block.h"
#include "buffer.h"
#include "super_block.h"

namespace orfos::kernel::fs {
  constexpr uint64_t MAX_OP_BLOCKS = 10;
  constexpr uint64_t LOG_SIZE      = MAX_OP_BLOCKS * 3;

  struct LogHeader {
    uint64_t n;
    uint32_t block[LOG_SIZE];
  };
  static_assert(sizeof(LogHeader) < BLOCK_SIZE);

  struct Log {
    mutex::SpinMutex mutex;
    int start;
    uint64_t size;
    int outstanding;
    int committing;
    uint32_t device;
    LogHeader header;
  };

  void initializeLog(uint32_t device, const SuperBlock& sb);
  void beginOp();
  void endOp();
  void writeLog(Buffer* buf);
} // namespace orfos::kernel::fs

#endif // ORFOS_KERNEL_FS_LOG_H_