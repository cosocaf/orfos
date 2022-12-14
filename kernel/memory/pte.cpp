#include "pte.h"

namespace orfos::kernel::memory {
  PageTableEntry::PageTableEntry() : raw(0) {}
  uint64_t PageTableEntry::toPhysicalAddress() const {
    return (raw >> 10) << 12;
  }
  PageTableEntry PageTableEntry::fromPhysicalAddress(uint64_t address) {
    PageTableEntry pte{};
    pte.raw = (address >> 12) << 10;
    return pte;
  }
  uint16_t PageTableEntry::flags() const {
    return static_cast<uint16_t>(raw & 0x3FF);
  }
} // namespace orfos::kernel::memory