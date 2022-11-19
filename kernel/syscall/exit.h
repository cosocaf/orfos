#ifndef ORFOS_KERNEL_SYSCALL_EXIT_H_
#define ORFOS_KERNEL_SYSCALL_EXIT_H_

#pragma once

#include <cstdint>

namespace orfos::kernel::syscall {
  uint64_t exit();
}

#endif // ORFOS_KERNEL_SYSCALL_EXIT_H_