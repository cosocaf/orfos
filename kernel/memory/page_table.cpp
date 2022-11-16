#include "page_table.h"

#include <arch/config.h>
#include <arch/riscv_macros.h>
#include <console/console.h>
#include <lib/string.h>

#include <cassert>
#include <new>

namespace orfos::kernel::memory {
  PageTable::PageTable() {
    entry.raw = 0;
  }
  bool PageTable::map(VirtualAddress virtualAddress,
                      uint64_t size,
                      uint64_t physicalAddress,
                      uint8_t flags) {
    assert(size != 0);

    auto last = VirtualAddress(virtualAddress.address + size - 1).roundDown();
    virtualAddress = virtualAddress.roundDown();

    while (true) {
      auto pte = walk(virtualAddress, true);
      if (pte == nullptr) {
        return false;
      }
      if (pte->pte.v) {
        int i = 0;
        i += 10;
      }
      assert(!pte->pte.v);
      pte->raw |= flags;
      pte->pte.v = 1;
      pte->raw |= ((physicalAddress >> 12) << 10);

      if (virtualAddress == last) {
        break;
      }
      virtualAddress.address += PAGE_SIZE;
      physicalAddress += PAGE_SIZE;
    }

    return true;
  }
  void PageTable::unmap(VirtualAddress virtualAddress,
                        uint64_t numPages,
                        bool doFree) {
    // TODO: impl
    return;
  }
  PageTableEntry* PageTable::walk(VirtualAddress virtualAddress,
                                  bool allocate) {
    assert(virtualAddress.address < MAX_VIRTUAL_ADDRESS);

    auto pageTable = this;
    for (int level = 2; level > 0; --level) {
      auto pte = pageTable->getEntry(level, virtualAddress);
      if (pte->pte.v) {
        pageTable = Page::from(pte->toPhysicalAddress())->as<PageTable*>();
      } else {
        if (!allocate) {
          return nullptr;
        }
        pageTable = Page::allocate()->as<PageTable*>();
        if (pageTable == nullptr) {
          return nullptr;
        }
        new (pageTable) PageTable();
        *pte = PageTableEntry::fromPhysicalAddress(
          reinterpret_cast<uint64_t>(pageTable));
        pte->pte.v = 1;
      }
    }
    return pageTable->getEntry(0, virtualAddress);
  }
  uint64_t PageTable::walkAddress(VirtualAddress virtualAddress) {
    return 0;
  }

  PageTableEntry* PageTable::getEntry(int level,
                                      VirtualAddress virtualAddress) {
    assert(level >= 0 && level <= 2);

    uint64_t vpn;
    switch (level) {
      case 0:
        vpn = virtualAddress.va.vpn0;
        break;
      case 1:
        vpn = virtualAddress.va.vpn1;
        break;
      case 2:
        vpn = virtualAddress.va.vpn2;
        break;
    }

    return &entry + vpn;
  }

  bool PageTable::copyout(VirtualAddress dst, char* src, uint64_t len) {
    while (len > 0) {
      auto va = dst.roundDown();
      auto pa = walkAddress(va);
      if (pa == 0) {
        return false;
      }
      auto n = PAGE_SIZE - (dst - va).address;
      if (n > len) {
        n = len;
      }
      memmove(reinterpret_cast<void*>(pa + (dst - va).address), src, n);

      len -= n;
      src += n;
      dst = va + PAGE_SIZE;
    }
    return true;
  }
  bool PageTable::copyin(char* dst, VirtualAddress src, uint64_t len) {
    while (len > 0) {
      auto va = src.roundDown();
      auto pa = walkAddress(va);
      if (pa == 0) {
        return false;
      }
      auto n = PAGE_SIZE - (src - va).address;
      if (n > len) {
        n = len;
      }
      memmove(dst, reinterpret_cast<void*>(pa + (src - va).address), n);

      len -= n;
      dst += n;
      src = va + PAGE_SIZE;
    }
    return true;
  }
  bool PageTable::copyinString(char* dst, VirtualAddress src, uint64_t len) {
    bool gotNull = false;
    while (!gotNull && len > 0) {
      auto va = src.roundDown();
      auto pa = walkAddress(va);
      if (pa == 0) {
        return false;
      }
      auto n = PAGE_SIZE - (src - va).address;
      if (n > len) {
        n = len;
      }

      auto ptr = reinterpret_cast<char*>(pa + (src - va).address);
      while (n > 0) {
        if (*ptr == '\0') {
          *dst    = '\0';
          gotNull = true;
          break;
        } else {
          *dst = *ptr;
        }
        --n;
        --len;
        ++ptr;
        ++dst;
      }
      src = va + PAGE_SIZE;
    }

    return gotNull;
  }
  uint64_t PageTable::makeSatp() {
    return SATP_SV39 | (reinterpret_cast<uint64_t>(this) >> 12);
  }
} // namespace orfos::kernel::memory