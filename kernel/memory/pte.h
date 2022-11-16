#ifndef ORFOS_KERNEL_MEMORY_PTE_H_
#define ORFOS_KERNEL_MEMORY_PTE_H_

#pragma once

#include <cstdint>

namespace orfos::kernel::memory {
  constexpr uint64_t PTE_V = 1 << 0;
  constexpr uint64_t PTE_R = 1 << 1;
  constexpr uint64_t PTE_W = 1 << 2;
  constexpr uint64_t PTE_X = 1 << 3;
  constexpr uint64_t PTE_U = 1 << 4;
  union PageTableEntry {
    uint64_t raw;
    struct {
      uint64_t v : 1;
      uint64_t r : 1;
      uint64_t w : 1;
      uint64_t x : 1;
      uint64_t u : 1;
      [[maybe_unused]] uint64_t g : 1;
      [[maybe_unused]] uint64_t a : 1;
      [[maybe_unused]] uint64_t d : 1;
      [[maybe_unused]] uint64_t rsw : 2;
      uint64_t ppn0 : 9;
      uint64_t ppn1 : 9;
      uint64_t ppn2 : 26;
      [[maybe_unused]] uint64_t reserved : 10;
    } pte;

    PageTableEntry();
    uint64_t toPhysicalAddress() const;
    static PageTableEntry fromPhysicalAddress(uint64_t address);
  };
  static_assert(sizeof(PageTableEntry) == sizeof(uint64_t));
} // namespace orfos::kernel::memory

#endif // ORFOS_KERNEL_MEMORY_PTE_H_