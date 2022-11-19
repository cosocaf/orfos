#ifndef ORFOS_KERNEL_CONSOLE_CONSOLE_H_
#define ORFOS_KERNEL_CONSOLE_CONSOLE_H_

#pragma once

#include <cstddef>

namespace orfos::kernel::console {
  void initialize();
  void printf(const char* fmt, ...);
  void putc(char c);
  char readline(bool user, char* buf, size_t len);
  void consoleInterrupt(char c);
} // namespace orfos::kernel::console

#endif // ORFOS_KERNEL_CONSOLE_CONSOLE_H_