#ifndef ORFOS_KERNEL_SYSCALL_SYSCALL_H_
#define ORFOS_KERNEL_SYSCALL_SYSCALL_H_

#pragma once

#include <cstdint>

#include "syscall_key.h"

namespace orfos::kernel::syscall {
  constexpr uint64_t EXIT = SYS_EXIT;
  constexpr uint64_t EXEC = SYS_EXEC;
  void syscall();
  void initialize();
} // namespace orfos::kernel::syscall

#endif // ORFOS_KERNEL_SYSCALL_SYSCALL_H_