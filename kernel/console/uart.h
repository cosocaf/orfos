#ifndef ORFOS_KERNEL_CONSOLE_UART_H_
#define ORFOS_KERNEL_CONSOLE_UART_H_

#pragma once

#include <cstdint>

namespace orfos::kernel::console {
  constexpr uint64_t UART_BASE = 0x1000'0000;
  void initializeUart();
  void uartPutc(char c);
  void uartPutcSync(char c);
  char uartGetc();
  void uartInterrupt();
} // namespace orfos::kernel::console

#endif // ORFOS_KERNEL_CONSOLE_UART_H_