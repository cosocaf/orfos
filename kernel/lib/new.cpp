#include <memory/kvm.h>
#include <memory/page.h>
#include <memory/page_table.h>
#include <mutex/lock_guard.h>
#include <mutex/spin_mutex.h>

#include <algorithm>
#include <cassert>
#include <cstddef>

namespace {
  orfos::kernel::mutex::SpinMutex mutex;

  void* allocate(size_t size) {
    using namespace orfos::kernel;

    assert(size > 0);

    mutex::LockGuard guard(::mutex);
    if (size <= PAGE_SIZE) {
      return memory::Page::allocate()->as<void*>();
    } else {
      auto first   = memory::Page::allocate()->as<void*>();
      // auto address = reinterpret_cast<uint64_t>(first);
      // memory::kernelPageTable->map(address,
      //                              PAGE_SIZE,
      //                              reinterpret_cast<uint64_t>(first),
      //                              memory::PTE_R | memory::PTE_W);
      for (size_t offset = PAGE_SIZE;
           offset < (size + PAGE_SIZE - 1) / PAGE_SIZE * PAGE_SIZE;
           offset += PAGE_SIZE) {
        /*auto page = */memory::Page::allocate()->as<void*>();
        // memory::kernelPageTable->map(address + offset,
        //                              PAGE_SIZE,
        //                              reinterpret_cast<uint64_t>(page),
        //                              memory::PTE_R | memory::PTE_W);
      }
      return first;
    }
    // auto header = memory::Page::allocate()->as<Header*>();
    // memory::kernelPageTable->map(
    //   address, PAGE_SIZE, reinterpret_cast<uint64_t>(header), memory::PTE_R);

    // auto headerAddress = address;
    // while (true) {
    //   address += PAGE_SIZE;
    //   auto page = memory::Page::allocate();
    //   memory::kernelPageTable->map(
    //     address,
    //     std::min(size, static_cast<size_t>(PAGE_SIZE)),
    //     reinterpret_cast<uint64_t>(page),
    //     memory::PTE_R | memory::PTE_W);
    //   if (size <= PAGE_SIZE) {
    //     break;
    //   }
    //   size -= PAGE_SIZE;
    // }

    // address += PAGE_SIZE;
    // return reinterpret_cast<void*>(headerAddress + PAGE_SIZE);
  }
} // namespace

[[nodiscard]] void* operator new(size_t size) {
  return allocate(size);
}

[[nodiscard]] void* operator new[](std::size_t size) {
  return allocate(size);
}

void operator delete(void* ptr) {
  // TODO
}
void operator delete(void* ptr, size_t size) {
  // TODO
}

void operator delete[](void* ptr) noexcept {
  // TODO
}