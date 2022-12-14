#include "stdio.h"

#include "file.h"
#include "syscall.h"

namespace {
  auto digits = "0123456789ABCDEF";
  void printint(int fd, int xx, int base, int sgn) {
    char buf[16];
    int i, neg;
    uint32_t x;

    neg = 0;
    if (sgn && xx < 0) {
      neg = 1;
      x   = -xx;
    } else {
      x = xx;
    }

    i = 0;
    do {
      buf[i++] = digits[x % base];
    } while ((x /= base) != 0);
    if (neg) buf[i++] = '-';

    while (--i >= 0) putc(fd, buf[i]);
  }

  void printptr(int fd, uint64_t x) {
    int i;
    putc(fd, '0');
    putc(fd, 'x');
    for (i = 0; i < (sizeof(uint64_t) * 2); i++, x <<= 4)
      putc(fd, digits[x >> (sizeof(uint64_t) * 8 - 4)]);
  }
} // namespace

char getc(int fd) {
  char c;
  read(fd, &c, 1);
  return c;
}
char* getline(char* buf, int n) {
  int i;
  for (i = 0; i < n - 1;) {
    auto c = getc(0);
    if (c < 1) {
      break;
    }
    buf[i] = c;
    ++i;
    if (c == '\n' || c == '\r') {
      break;
    }
  }
  buf[i] = '\0';
  return buf;
}

void putc(int fd, char c) {
  write(fd, &c, 1);
}
void printf(const char* fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  vprintf(0, fmt, ap);
}
void fprintf(int fd, const char* fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  vprintf(fd, fmt, ap);
}
void vprintf(int fd, const char* fmt, va_list ap) {
  const char* s;
  int c, i, state;

  state = 0;
  for (i = 0; fmt[i]; i++) {
    c = fmt[i] & 0xff;
    if (state == 0) {
      if (c == '%') {
        state = '%';
      } else {
        putc(fd, c);
      }
    } else if (state == '%') {
      if (c == 'd') {
        printint(fd, va_arg(ap, int), 10, 1);
      } else if (c == 'l') {
        printint(fd, va_arg(ap, uint64_t), 10, 0);
      } else if (c == 'x') {
        printint(fd, va_arg(ap, int), 16, 0);
      } else if (c == 'p') {
        printptr(fd, va_arg(ap, uint64_t));
      } else if (c == 's') {
        s = va_arg(ap, char*);
        if (s == 0) s = "(null)";
        while (*s != 0) {
          putc(fd, *s);
          s++;
        }
      } else if (c == 'c') {
        putc(fd, va_arg(ap, uint32_t));
      } else if (c == '%') {
        putc(fd, c);
      } else {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c);
      }
      state = 0;
    }
  }
} // namespace lib