#ifndef ORFOS_KERNEL_MUTEX_SPIN_MUTEX_H_
#define ORFOS_KERNEL_MUTEX_SPIN_MUTEX_H_

#pragma once

#include <arch/riscv_macros.h>

#include <cstddef>
#include <cstdint>

namespace orfos::kernel::mutex {
  class SpinMutex {
    bool locked;
    uint64_t hartid;

  public:
    SpinMutex();
    void lock();
    void unlock();
    void push();
    void pop();
    bool holding();
  };
} // namespace orfos::kernel::mutex

#endif // ORFOS_KERNEL_MUTEX_SPIN_MUTEX_H_