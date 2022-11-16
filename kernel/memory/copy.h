#ifndef ORFOS_KERNEL_MEMORY_COPY_H_
#define ORFOS_KERNEL_MEMORY_COPY_H_

#pragma once

#include <cstdint>

namespace orfos::kernel::memory {
  bool eitherCopyout(bool userDst, uint64_t dst, void* src, uint64_t len);
  bool eitherCopyin(void* dst, bool userSrc, uint64_t src, uint64_t len);
} // namespace orfos::kernel::memory

#endif // ORFOS_KERNEL_MEMORY_COPY_H_