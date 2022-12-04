#ifndef ORFOS_KERNEL_LIB_ENDIAN_H_
#define ORFOS_KERNEL_LIB_ENDIAN_H_

#include <bit>
#include <cstddef>
#include <cstdint>

namespace orfos::kernel::lib {
  template <typename T>
  constexpr T swapEndian(T value) {
    union {
      T value;
      uint8_t bytes[sizeof(T)];
    } src, dst;
    src.value = value;
    for (size_t i = 0; i < sizeof(T); ++i) {
      dst.bytes[i] = src.bytes[sizeof(T) - i - 1];
    }
    return dst.value;
  }
  template <typename T>
  constexpr T fromBigEndian(T value) {
    if constexpr (std::endian::native == std::endian::little) {
      return swapEndian(value);
    } else {
      return value;
    }
  }
  template <typename T>
  constexpr T fromLittleEndian(T value) {
    if constexpr (std::endian::native == std::endian::big) {
      return swapEndian(value);
    } else {
      return value;
    }
  }
} // namespace orfos::kernel::lib

#endif // ORFOS_KERNEL_LIB_ENDIAN_H_