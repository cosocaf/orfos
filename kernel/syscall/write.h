#ifndef ORFOS_KERNEL_SYSCALL_WRITE_H_
#define ORFOS_KERNEL_SYSCALL_WRITE_H_

#pragma once

#include <cstdint>

namespace orfos::kernel::syscall {
  uint64_t write();
}

#endif // ORFOS_KERNEL_SYSCALL_WRITE_H_