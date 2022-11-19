#ifndef ORFOS_KERNEL_PROCESS_SCHEDULER_H_
#define ORFOS_KERNEL_PROCESS_SCHEDULER_H_

#pragma once

#include <lib/hash_map.h>
#include <lib/hash_set.h>
#include <lib/queue.h>
#include <mutex/spin_mutex.h>

#include <vector>

#include "process.h"

namespace orfos::kernel::process {
  class Scheduler {
    mutex::SpinMutex mutex;
    lib::Queue<Process*> readyQueue;
    lib::HashMap<void*, std::vector<Process*>> sleepTable;

    void killZombie(Process* process);

  public:
    void registerProcess(Process* process);
    void unregisterProcess(Process* process);
    void reschedule();
    void wakeup(void* chan);
    [[noreturn]] void run();
  };
  extern Scheduler* scheduler;
  void initializeScheduler();
} // namespace orfos::kernel::process

#endif // ORFOS_KERNEL_PROCESS_SCHEDULER_H_