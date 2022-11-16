#ifndef ORFOS_KERNEL_PROCESS_SLEEP_H_
#define ORFOS_KERNEL_PROCESS_SLEEP_H_

#pragma once

#include <mutex/spin_mutex.h>

namespace orfos::kernel::process {
  void yield();
  void sleep(void* chan, mutex::SpinMutex& mutex);
  void wakeup(void* chan);
} // namespace orfos::kernel::process

#endif // ORFOS_KERNEL_PROCESS_SLEEP_H_