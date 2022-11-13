#ifndef ORFOS_KERNEL_MUTEX_SPIN_MUTEX_H_
#define ORFOS_KERNEL_MUTEX_SPIN_MUTEX_H_

#pragma once

namespace orfos::kernel::mutex {
  class SpinMutex {
    public:
    void lock();
    void unlock();
    void push();
    void pop();
    bool holding();
  };
}

#endif // ORFOS_KERNEL_MUTEX_SPIN_MUTEX_H_