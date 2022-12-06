#ifndef ORFOS_KERNEL_DRIVER_MEMORY_DEVICE_H_
#define ORFOS_KERNEL_DRIVER_MEMORY_DEVICE_H_

#pragma once

#include <optional>

#include "../device_tree.h"

namespace orfos::kernel::driver::memory {
  union MemoryBlock {
    uint8_t data[4096];
    MemoryBlock* next;
  };
  void registerMemory(uintptr_t address, size_t size);
  MemoryBlock* allocate();
  void free(MemoryBlock* memBlock);
} // namespace orfos::kernel::driver::memory

#endif // ORFOS_KERNEL_DRIVER_MEMORY_DEVICE_H_