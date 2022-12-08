#include "string.h"

#include <cstdint>

extern "C" {
void* memset(void* dst, int ch, size_t n) {
  auto d = reinterpret_cast<uint8_t*>(dst);
  while (n--) {
    *d = ch;
    ++d;
  }
  return dst;
}
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
int memcmp(const void* s, const void* t, size_t n) {
  auto p1 = reinterpret_cast<const unsigned char*>(s);
  auto p2 = reinterpret_cast<const unsigned char*>(t);
  while (n--) {
    if (*p1 != *p2) {
      return *p1 - *p2;
    }
    ++p1;
    ++p2;
  }
  return 0;
}
size_t strlen(const char* str) {
  size_t len = 0;
  while (str[len] != '\0') {
    ++len;
  }
  return len;
}
int strcmp(const char* s, const char* t) {
  while (*s != '\0' && *s == *t) {
    ++s;
    ++t;
  }
  return static_cast<unsigned char>(*s) - static_cast<unsigned char>(*t);
}
}