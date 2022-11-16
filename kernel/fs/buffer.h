#ifndef ORFOS_KERNEL_FS_BUFFER_H_
#define ORFOS_KERNEL_FS_BUFFER_H_

#pragma once

#include <lib/hash_map.h>
#include <mutex/sleep_mutex.h>
#include <mutex/spin_mutex.h>

#include <cstdint>

#include "block.h"

namespace orfos::kernel::fs {
  struct Buffer {
    int valid;
    int disk;
    uint32_t device;
    uint32_t block;
    uint32_t refCount;
    mutex::SleepMutex mutex;
    char data[BLOCK_SIZE];

    void read();
    void write();
  };
  class BufferCache {
    mutex::SpinMutex mutex;
    lib::HashMap<uint64_t, Buffer*> bufferTable;

  public:
    Buffer* get(uint32_t device, uint32_t block);
    void release(Buffer* buffer);
    void pin(Buffer* buffer);
    void unpin(Buffer* buffer);
  };
  extern BufferCache* bufferCache;
  void initializeBufferCache();
} // namespace orfos::kernel::fs

#endif // ORFOS_KERNEL_FS_BUFFER_H_