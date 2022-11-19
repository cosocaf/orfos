#include "uvm.h"

#include <lib/string.h>

#include <cassert>

#include "page.h"
#include "va.h"

namespace orfos::kernel::memory {
  uint64_t allocateUserVirtualMemory(PageTable* pageTable,
                                     uint64_t oldSize,
                                     uint64_t newSize,
                                     int xperm) {
    if (newSize < oldSize) {
      return oldSize;
    }

    oldSize = VirtualAddress(oldSize).roundDown().address;
    for (uint64_t addr = oldSize; addr < newSize; addr += PAGE_SIZE) {
      auto mem = Page::allocate();
      if (mem == nullptr) {
        deallocateUserVirtualMemory(pageTable, addr, oldSize);
        return 0;
      }
      auto res = pageTable->map(
        addr, PAGE_SIZE, mem->as<uint64_t>(), PTE_R | PTE_U | xperm);
      if (!res) {
        mem->free();
        deallocateUserVirtualMemory(pageTable, addr, oldSize);
        return 0;
      }
    }
    return newSize;
  }

  uint64_t deallocateUserVirtualMemory(PageTable* pageTable,
                                       uint64_t oldSize,
                                       uint64_t newSize) {
    if (newSize >= oldSize) {
      return oldSize;
    }

    auto newsz = VirtualAddress(newSize);
    auto oldsz = VirtualAddress(oldSize);

    if (newsz.roundUp() < oldsz.roundUp()) {
      auto numPages = (oldsz.roundUp() - newsz.roundUp()).address / PAGE_SIZE;
      pageTable->unmap(newsz.roundUp(), numPages, true);
    }

    return newSize;
  }
  void clearUserVirtualMemory(PageTable* pageTable, uint64_t va) {
    auto pte = pageTable->walk(va, false);
    assert(pte != nullptr);
    pte->pte.u = 0;
  }
  void copyUserVirtualMemory(PageTable* oldTable,
                             PageTable* newTable,
                             uint64_t size) {
    for (size_t i = 0; i < size; i += PAGE_SIZE) {
      auto pte = oldTable->walk(i, false);
      assert(pte != nullptr);
      assert(pte->pte.v);
      auto pa    = pte->toPhysicalAddress();
      auto flags = pte->flags();
      auto mem   = new char[PAGE_SIZE];
      memmove(mem, reinterpret_cast<char*>(pa), PAGE_SIZE);
      newTable->map(i, PAGE_SIZE, reinterpret_cast<uint64_t>(mem), flags);
    }
  }
} // namespace orfos::kernel::memory