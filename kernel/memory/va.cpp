#include "va.h"

#include "page.h"

namespace orfos::kernel::memory {
  VirtualAddress::VirtualAddress(uint64_t address) : address(address) {}
  VirtualAddress VirtualAddress::roundUp() const {
    return (address + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
  }
  VirtualAddress VirtualAddress::roundDown() const {
    return address & ~(PAGE_SIZE - 1);
  }
  bool operator==(const VirtualAddress& a, const VirtualAddress& b) {
    return a.address == b.address;
  }
  bool operator<(const VirtualAddress& a, const VirtualAddress& b) {
    return a.address < b.address;
  }
  VirtualAddress operator+(const VirtualAddress& a, const VirtualAddress& b) {
    return a.address + b.address;
  }
  VirtualAddress operator-(const VirtualAddress& a, const VirtualAddress& b) {
    return a.address - b.address;
  }
} // namespace orfos::kernel::memory