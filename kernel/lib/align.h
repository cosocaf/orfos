#ifndef ORFOS_KERNEL_LIB_ALIGN_H_
#define ORFOS_KERNEL_LIB_ALIGN_H_

#pragma once

#include <cstddef>

namespace orfos::kernel::lib {
  template <typename T>
  constexpr T align(T n, size_t alignAs) {
    return (n + alignAs - 1) / alignAs * alignAs;
  }
} // namespace orfos::kernel::lib

#endif // ORFOS_KERNEL_LIB_ALIGN_H_