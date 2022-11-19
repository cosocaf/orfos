#ifndef ORFOS_KERNEL_PROCESS_USER_INIT_PROC_H_
#define ORFOS_KERNEL_PROCESS_USER_INIT_PROC_H_

#pragma once

#include "process.h"

namespace orfos::kernel::process {
  extern Process* initProc;
  void startUserInitProcess();
}

#endif // ORFOS_KERNEL_PROCESS_USER_INIT_PROC_H_