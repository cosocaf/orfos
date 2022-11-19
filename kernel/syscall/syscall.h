#ifndef ORFOS_KERNEL_SYSCALL_SYSCALL_H_
#define ORFOS_KERNEL_SYSCALL_SYSCALL_H_

#pragma once

#include <cstdint>

#include "syscall_key.h"

namespace orfos::kernel::syscall {
  constexpr uint64_t FORK  = SYS_FORK;
  constexpr uint64_t EXIT  = SYS_EXIT;
  constexpr uint64_t WAIT  = SYS_WAIT;
  constexpr uint64_t READ  = SYS_READ;
  constexpr uint64_t EXEC  = SYS_EXEC;
  constexpr uint64_t OPEN  = SYS_OPEN;
  constexpr uint64_t WRITE = SYS_WRITE;
  constexpr uint64_t MKNOD = SYS_MKNOD;
  constexpr uint64_t CLOSE = SYS_CLOSE;

  void syscall();
  void initialize();
} // namespace orfos::kernel::syscall

#endif // ORFOS_KERNEL_SYSCALL_SYSCALL_H_