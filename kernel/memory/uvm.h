#ifndef ORFOS_KERNEL_MEMORY_UVM_H_
#define ORFOS_KERNEL_MEMORY_UVM_H_

#pragma once

#include "page_table.h"

namespace orfos::kernel::memory {
  uint64_t allocateUserVirtualMemory(PageTable* pageTable,
                                     uint64_t oldSize,
                                     uint64_t newSize,
                                     int xperm);
  uint64_t deallocateUserVirtualMemory(PageTable* pageTable,
                                       uint64_t oldsz,
                                       uint64_t newsz);
  void clearUserVirtualMemory(PageTable* pageTable, uint64_t va);
  void copyUserVirtualMemory(PageTable* oldTable, PageTable* newTable, uint64_t size);
} // namespace orfos::kernel::memory

#endif // ORFOS_KERNEL_MEMORY_UVM_H_