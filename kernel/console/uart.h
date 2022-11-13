#ifndef ORFOS_KERNEL_CONSOLE_UART_H_
#define ORFOS_KERNEL_CONSOLE_UART_H_

#pragma once

#include <cstdint>

namespace orfos::kernel::console {
  constexpr uint64_t UART_BASE = 0x1000'0000;
  void initialize_uart();
  void uart_putc(char c);
  void uart_putc_sync(char c);
  char uart_getc();
}

#endif // ORFOS_KERNEL_CONSOLE_UART_H_