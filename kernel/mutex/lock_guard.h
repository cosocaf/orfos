#ifndef ORFOS_KERNEL_MUTEX_LOCK_GUARD_H_
#define ORFOS_KERNEL_MUTEX_LOCK_GUARD_H_

#pragma once

namespace orfos::kernel::mutex {
  template <typename Mutex>
  class LockGuard {
    Mutex& mutex;

  public:
    LockGuard(Mutex& mutex) : mutex(mutex) {
      mutex.lock();
    }
    ~LockGuard() {
      mutex.unlock();
    }
  };
} // namespace orfos::kernel::mutex

#endif // ORFOS_KERNEL_MUTEX_LOCK_GUARD_H_