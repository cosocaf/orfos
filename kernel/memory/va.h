#ifndef ORFOS_KERNEL_MEMORY_VA_H_
#define ORFOS_KERNEL_MEMORY_VA_H_

#pragma once

#include <cstdint>

namespace orfos::kernel::memory {
  union VirtualAddress {
    uint64_t address;
    struct {
      uint64_t pageOffset : 12;
      uint64_t vpn0 : 9;
      uint64_t vpn1 : 9;
      uint64_t vpn2 : 9;
    } va;

    VirtualAddress(uint64_t address);

    VirtualAddress roundUp() const;
    VirtualAddress roundDown() const;
  };
  bool operator==(const VirtualAddress& a, const VirtualAddress& b);
  bool operator<(const VirtualAddress& a, const VirtualAddress& b);
  VirtualAddress operator+(const VirtualAddress& a, const VirtualAddress& b);
  VirtualAddress operator-(const VirtualAddress& a, const VirtualAddress& b);
} // namespace orfos::kernel::memory

#endif // ORFOS_KERNEL_MEMORY_VA_H_