#ifndef ORFOS_KERNEL_MEMORY_PAGE_H_
#define ORFOS_KERNEL_MEMORY_PAGE_H_

#pragma once

#include <cassert>
#include <cstdint>

#define PAGE_SIZE 4096

namespace orfos::kernel::memory {
  union Page {
  private:
    static Page* freeList;
    static Page* lastPage;

    Page* next;
    char memory[PAGE_SIZE];

  public:
    static uint64_t physicalAddressStop;

    static void initialize();
    static Page* allocate();
    template <typename T>
    static Page* from(T address) noexcept {
      auto addr = reinterpret_cast<uint64_t>(address);
      assert(addr % PAGE_SIZE == 0);
      return reinterpret_cast<Page*>(addr);
    }

    template <typename T>
    const T as() const {
      return reinterpret_cast<const T>(reinterpret_cast<const void*>(this));
    }

    template <typename T>
    T as() {
      return reinterpret_cast<T>(reinterpret_cast<void*>(this));
    }

    const char* data() const;
    char* data();
    void free();
  };
} // namespace orfos::kernel::memory

#endif // ORFOS_KERNEL_MEMORY_PAGE_H_