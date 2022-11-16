#ifndef ORFOS_KERNEL_MUTEX_SLEEP_MUTEX_H_
#define ORFOS_KERNEL_MUTEX_SLEEP_MUTEX_H_

#pragma once

#include "spin_mutex.h"

namespace orfos::kernel::mutex {
  class SleepMutex {
    SpinMutex mutex;
    bool locked;
    uint64_t pid;

  public:
    void lock();
    void unlock();
    bool holding();
  };
} // namespace orfos::kernel::mutex

#endif // ORFOS_KERNEL_MUTEX_SLEEP_MUTEX_H_