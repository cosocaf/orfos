#ifndef ORFOS_KERNEL_SYSCALL_WAIT_H_
#define ORFOS_KERNEL_SYSCALL_WAIT_H_

#pragma once

#include <cstdint>

namespace orfos::kernel::syscall { 
  uint64_t wait();
}

#endif // ORFOS_KERNEL_SYSCALL_WAIT_H_