#include "string.h"

extern "C" {
void *memcpy(void *dest, const void *src, size_t n) {
  auto d = static_cast<char *>(dest);
  auto s = static_cast<const char *>(src);

  while (--n) {
    *d++ = *s++;
  }

  return dest;
}
void *memmove(void *s1, const void *s2, size_t n) {
  char *ss1       = reinterpret_cast<char *>(s1);
  const char *ss2 = reinterpret_cast<const char *>(s2);
  if (n != 0) {
    if (ss1 < ss2) {
      const char *t = ss2 + n;
      do *ss1++ = *ss2++;
      while (ss2 != t);
    } else if (ss1 > ss2) {
      const char *t = ss2;
      ss1 += n;
      ss2 += n;
      do *--ss1 = *--ss2;
      while (ss2 != t);
    }
  }
  return s1;
}
void *memset(void *dest, int ch, size_t n) {
  auto d = static_cast<char *>(dest);
  while (--n) {
    *d++ = static_cast<char>(ch);
  }
  return dest;
}
int strncmp(const char *s1, const char *s2, size_t n) {
  while (*s1 == *s2) {
    if (*s1 == '\0') {
      return 0;
    }
    s1++;
    s2++;
  }
  return (unsigned char)*s1 - (unsigned char)*s2;
}
char *strncpy(char *s, const char *t, size_t n) {
  char *os;

  os = s;
  while (n-- > 0 && (*s++ = *t++) != 0) {
  }
  while (n-- > 0) *s++ = 0;
  return os;
}
size_t strlen(const char *s) {
  const char *ss;
  for (ss = s; *ss != '\0'; ss++) {
  }
  return ss - s;
}
}