#ifndef ORFOS_KERNEL_SYSCALL_MKNOD_H_
#define ORFOS_KERNEL_SYSCALL_MKNOD_H_

#pragma once

#include <cstdint>

namespace orfos::kernel::syscall {
  uint64_t mknod();
}

#endif // ORFOS_KERNEL_SYSCALL_MKNOD_H_