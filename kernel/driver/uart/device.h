#ifndef ORFOS_KERNEL_DEVICE_UART_DEVICE_H_
#define ORFOS_KERNEL_DEVICE_UART_DEVICE_H_

#pragma once

#include <lib/fixed_string.h>
#include <lib/result.h>

#include "../device_tree.h"

namespace orfos::kernel::driver::uart {
  void putc(char c);
  void putcSync(char c);
  char getc();

  lib::Result<lib::_, lib::FixedString<>> initialize(const DeviceTreeNode& stdoutNode);
} // namespace orfos::kernel::driver::uart

#endif // ORFOS_KERNEL_DEVICE_UART_DEVICE_H_