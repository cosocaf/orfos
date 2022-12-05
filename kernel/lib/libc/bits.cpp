#include "bits.h"

namespace {
  template <typename T>
  int clz(T a) {
    int count = 0;
    for (T i = static_cast<T>(sizeof(T) * 8 - 1); i >= 0; --i) {
      if (a & (1 << i)) {
        break;
      }
      ++count;
    }
    return count;
  }
  template <typename T>
  int ctz(T a) {
    int count = 0;
    for (T i = 0; i < static_cast<T>(sizeof(T) * 8); ++i) {
      if (a & (1 << i)) {
        break;
      }
      ++count;
    }
    return count;
  }
} // namespace

extern "C" {
int __clzsi2(unsigned int a) {
  return clz(a);
}
int __clzdi2(unsigned long a) {
  return clz(a);
}
int __clzti2(unsigned long long a) {
  return clz(a);
}

int __ctzsi2(unsigned int a) {
  return ctz(a);
}
int __ctzdi2(unsigned long a) {
  return ctz(a);
}
int __ctzti2(unsigned long long a) {
  return ctz(a);
}
}