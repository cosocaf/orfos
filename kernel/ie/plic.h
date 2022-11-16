#ifndef ORFOS_KERNEL_IE_PLIC_H_
#define ORFOS_KERNEL_IE_PLIC_H_

#pragma once

#include <cstdint>

namespace orfos::kernel::ie {
  void initializePlic();
  void initializePlicHart();
  uint32_t plicClaim();
  void plicComplete(uint32_t irq);
}

#endif // ORFOS_KERNEL_IE_PLIC_H_