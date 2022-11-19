#ifndef ORFOS_KERNEL_SYSCALL_CLOSE_H_
#define ORFOS_KERNEL_SYSCALL_CLOSE_H_

#pragma once

#include <cstdint>

namespace orfos::kernel::syscall {
  uint64_t close();
}

#endif // ORFOS_KERNEL_SYSCALL_CLOSE_H_