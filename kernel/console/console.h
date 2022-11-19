#ifndef ORFOS_KERNEL_CONSOLE_CONSOLE_H_
#define ORFOS_KERNEL_CONSOLE_CONSOLE_H_

#pragma once

namespace orfos::kernel::console {
  void initialize();
  void printf(const char* fmt, ...);
  void putc(char c);
  char getc();
  void consoleInterrupt(char c);
}

#endif // ORFOS_KERNEL_CONSOLE_CONSOLE_H_