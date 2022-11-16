#include "page.h"

#include <arch/config.h>
#include <console/console.h>
#include <lib/panic.h>
#include <lib/string.h>
#include <mutex/lock_guard.h>
#include <mutex/spin_mutex.h>

#include <cassert>

extern "C" {
extern orfos::kernel::memory::Page kernelPages[];
}

namespace orfos::kernel::memory {
  namespace {
    mutex::SpinMutex mutex;
  }

  uint64_t Page::physicalAddressStop = KERNEL_BASE + 128 * 1024 * 1024;
  Page* Page::freeList               = nullptr;
  Page* Page::lastPage               = nullptr;

  void Page::initialize() {
    lastPage = from(physicalAddressStop);
    for (auto page = kernelPages; page < lastPage; ++page) {
      mutex::LockGuard guard(mutex);
      memset(page, 0, PAGE_SIZE);
      page->next = freeList;
      freeList   = page;
    }
  }

  const char* Page::data() const {
    return memory;
  }
  char* Page::data() {
    return memory;
  }
  Page* Page::allocate() {
    Page* page = nullptr;
    {
      mutex::LockGuard guard(mutex);
      page = freeList;
      if (page != nullptr) {
        freeList = page->next;
      }
    }

    if (page != nullptr) {
      memset(page->memory, 0, PAGE_SIZE);
    }

    if (page == nullptr) {
      lib::panic("Page not allocated. Out of memory.");
    }

    return page;
  }
  void Page::free() {
    assert(reinterpret_cast<uint64_t>(this) % PAGE_SIZE == 0);
    assert(this >= kernelPages);
    assert(this < lastPage);

    memset(memory, 0, PAGE_SIZE);

    mutex::LockGuard guard(mutex);
    next     = freeList;
    freeList = this;
  }
} // namespace orfos::kernel::memory