#ifndef ORFOS_KERNEL_MEMORY_KVM_H_
#define ORFOS_KERNEL_MEMORY_KVM_H_

#pragma once

#include "page_table.h"

namespace orfos::kernel::memory {
  extern PageTable* kernelPageTable;
  void initializeKvm();
  void initializeKvmHart();
} // namespace orfos::kernel::memory

#endif // ORFOS_KERNEL_MEMORY_KVM_H_