#ifndef ORFOS_KERNEL_MEMORY_PAGE_TABLE_H_
#define ORFOS_KERNEL_MEMORY_PAGE_TABLE_H_

#pragma once

#include "page.h"
#include "pte.h"
#include "va.h"

namespace orfos::kernel::memory {
  class PageTable {
    PageTableEntry entry;

    static PageTableEntry* walk(PageTable* pageTable,
                                int level,
                                VirtualAddress virtualAddress,
                                bool allocate);

  public:
    PageTable();
    bool map(VirtualAddress virtualAddress,
             uint64_t size,
             uint64_t physicalAddress,
             uint16_t flags);
    void unmap(VirtualAddress virtualAddress, uint64_t numPages, bool doFree);
    PageTableEntry* walk(VirtualAddress virtualAddress, bool allocate);
    uint64_t walkAddress(VirtualAddress virtualAddress);
    PageTableEntry* getEntry(int level, VirtualAddress virtualAddress);
    bool copyout(VirtualAddress dst, char* src, uint64_t len);
    bool copyin(char* dst, VirtualAddress src, uint64_t len);
    bool copyinString(char* dst, VirtualAddress src, uint64_t len);
    uint64_t makeSatp();
  };
} // namespace orfos::kernel::memory

#endif // ORFOS_KERNEL_MEMORY_PAGE_TABLE_H_