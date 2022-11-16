#ifndef ORFOS_KERNEL_LIB_PANIC_H_
#define ORFOS_KERNEL_LIB_PANIC_H_

#pragma once

namespace orfos::kernel::lib {
  extern bool panicked;
  [[noreturn]] void panic(const char* msg);
} // namespace orfos::kernel::lib

#endif // ORFOS_KERNEL_LIB_PANIC_H_