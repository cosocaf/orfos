#include "plic.h"

#include <arch/config.h>
#include <arch/riscv.h>
#include <console/console.h>

#include <cstdint>

namespace orfos::kernel::ie {
  namespace {
    inline uint32_t* senable(uint64_t hart) {
      return reinterpret_cast<uint32_t*>(PLIC + 0x2080 + hart * 0x100);
    }
    inline uint32_t* spriority(uint64_t hart) {
      return reinterpret_cast<uint32_t*>(PLIC + 0x201000 + hart * 0x2000);
    }
  } // namespace
  void initializePlic() {
    *reinterpret_cast<uint32_t*>(PLIC + UART0_IRQ * 4)   = 1;
    *reinterpret_cast<uint32_t*>(PLIC + VIRTIO0_IRQ * 4) = 1;
  }
  void initializePlicHart() {
    auto hartid        = arch::read_tp();
    *senable(hartid)   = (1 << UART0_IRQ) | (1 << VIRTIO0_IRQ);
    *spriority(hartid) = 0;
  }

  uint32_t plicClaim() {
    auto hartid  = arch::read_tp();
    auto address = PLIC + 0x201004 + hartid * 0x2000;
    return *reinterpret_cast<uint32_t*>(address);
  }
  void plicComplete(uint32_t irq) {
    auto hartid                           = arch::read_tp();
    auto address                          = PLIC + 0x201004 + hartid * 0x2000;
    *reinterpret_cast<uint32_t*>(address) = irq;
  }
} // namespace orfos::kernel::ie