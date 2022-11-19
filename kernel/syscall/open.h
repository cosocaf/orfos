#ifndef ORFOS_KERNEL_SYSCALL_OPEN_H_
#define ORFOS_KERNEL_SYSCALL_OPEN_H_

#pragma once

#include <cstdint>

namespace orfos::kernel::syscall {
  uint64_t open();
}

#endif // ORFOS_KERNEL_SYSCALL_OPEN_H_