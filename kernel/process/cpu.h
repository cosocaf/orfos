#ifndef ORFOS_KERNEL_PROCESS_CPU_H_
#define ORFOS_KERNEL_PROCESS_CPU_H_

#pragma once

#include <mutex/spin_mutex.h>

#include "context.h"
#include "process.h"

namespace orfos::kernel::process {
  struct Cpu {
    Context context;
    Process* process;
    size_t numLocks;
    bool interruptEnabled;
    static Cpu& current();
  };
} // namespace orfos::kernel::process

#endif // ORFOS_KERNEL_PROCESS_CPU_H_