#include "copy.h"

#include <lib/string.h>
#include <process/cpu.h>

namespace orfos::kernel::memory {
  bool eitherCopyout(bool userDst, uint64_t dst, void* src, uint64_t len) {
    if (userDst) {
      return process::Cpu::current().process->pageTable->copyout(
        dst, static_cast<char*>(src), len);
    } else {
      memmove(reinterpret_cast<void*>(dst), src, len);
      return true;
    }
  }
  bool eitherCopyin(void* dst, bool userSrc, uint64_t src, uint64_t len) {
    if (userSrc) {
      return process::Cpu::current().process->pageTable->copyin(
        static_cast<char*>(dst), src, len);
    } else {
      memmove(dst, reinterpret_cast<void*>(src), len);
      return true;
    }
  }
} // namespace orfos::kernel::memory