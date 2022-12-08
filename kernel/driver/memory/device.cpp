#include "device.h"

#include <lib/panic.h>

extern "C" {
extern char orfos_entry[];
extern char orfos_endOfKernelSections[];
}

namespace orfos::kernel::driver::memory {
  namespace {
    MemoryBlock* freeList = nullptr;
  }
  void registerMemory(uintptr_t address, size_t size) {
    auto kernelBegin = reinterpret_cast<uintptr_t>(orfos_entry);
    auto kernelEnd   = reinterpret_cast<uintptr_t>(orfos_endOfKernelSections);
    for (size_t i = 0; i < size; i += sizeof(MemoryBlock)) {
      if (kernelBegin <= address + i && address + i < kernelEnd) {
        continue;
      }
      auto memBlock  = reinterpret_cast<MemoryBlock*>(address + i);
      memBlock->next = freeList;
      freeList       = memBlock;
    }
  }
  MemoryBlock* allocate() {
    auto memBlock = freeList;
    if (memBlock != nullptr) {
      freeList = memBlock->next;
    }
    memBlock->next = nullptr;
    return memBlock;
  }
  void free(MemoryBlock* memBlock) {
    kernelAssert(memBlock != nullptr);
    memBlock->next = freeList;
    freeList       = memBlock;
  }
} // namespace orfos::kernel::driver::memory