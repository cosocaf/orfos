#include "console.h"

#include <mutex/lock_guard.h>
#include <mutex/spin_mutex.h>

#include <cstdarg>
#include <cstddef>

#include "uart.h"

namespace orfos::kernel::console {
  namespace {
    constexpr auto digits = "0123456789ABCDEF";
    void print_int(int n, int base, bool sign) {
      char buf[16];
      if (sign && n < 0) {
        sign = true;
        n    = -n;
      } else {
        sign = false;
      }

      int i = 0;
      do {
        buf[i++] = digits[n % base];
      } while (n /= base);

      if (sign) {
        buf[i++] = '-';
      }

      while (--i >= 0) {
        putc(buf[i]);
      }
    }

    void print_ptr(uint64_t ptr) {
      putc('0');
      putc('x');
      for (size_t i = 0; i < sizeof(uint64_t) * 2; i++, ptr <<= 4) {
        putc(digits[ptr >> (sizeof(uint64_t) * 8 - 4)]);
      }
    }

    mutex::SpinMutex mutex;
  } // namespace
  void initialize() {
    initializeUart();
  }
  void putc(char c) {
    uartPutcSync(c);
  }
  char getc() {
    return uartGetc();
  }
  void printf(const char* fmt, ...) {
    mutex::LockGuard guard(mutex);

    va_list ap;
    va_start(ap, fmt);

    for (; *fmt != '\0'; ++fmt) {
      if (*fmt != '%') {
        putc(*fmt);
        continue;
      }

      auto c = *++fmt;
      if (c == '\0') {
        putc('%');
        break;
      }

      switch (c) {
        case 'd':
          print_int(va_arg(ap, int32_t), 10, true);
          break;
        case 'u':
          print_int(va_arg(ap, uint32_t), 10, false);
          break;
        case 'x':
          print_int(va_arg(ap, int32_t), 16, true);
          break;
        case 'c':
          putc(va_arg(ap, int));
          break;
        case 'p':
          print_ptr(va_arg(ap, uint64_t));
          break;
        case 's': {
          const char* str = va_arg(ap, char*);
          if (!str) str = "(null)";
          while (*str) {
            putc(*str);
            ++str;
          }
          break;
        }
        case '%':
          putc('%');
          break;
        default:
          putc('%');
          putc(c);
      }
    }
    va_end(ap);
  }
  void consoleInterrupt(char c) {
    mutex::LockGuard guard(mutex);
    switch (c) {
      case '\b':
      case '\x7f':
        putc('\b');
        putc(' ');
        putc('\b');
        break;
      case '\0':
        break;
      default:
        if (c == '\r') {
          c = '\n';
        }
        putc(c);

        if (c == '\n' || c == '\x04') {
          // wakeup(&cons.r);
        }
    }
  }
} // namespace orfos::kernel::console