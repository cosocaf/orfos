#ifndef ORFOS_KERNEL_SYSCALL_FORK_H_
#define ORFOS_KERNEL_SYSCALL_FORK_H_

#pragma once

#include <cstdint>

namespace orfos::kernel::syscall {
  uint64_t fork();
}

#endif // ORFOS_KERNEL_SYSCALL_FORK_H_