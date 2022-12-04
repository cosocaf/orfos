#include "string.h"

#include <cstdint>

extern "C" {
void* memcpy(void* dst, void* src, size_t n) {
  auto s = reinterpret_cast<const uint8_t*>(src);
  auto d = reinterpret_cast<uint8_t*>(dst);
  while (n--) {
    *d = *s;
    ++d;
    ++s;
  }
  return src;
}
size_t strlen(const char* str) {
  size_t len = 0;
  while (str[len] != '\0') {
    ++len;
  }
  return len;
}
}