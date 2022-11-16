#include "cause.h"

#include <arch/config.h>
#include <arch/riscv.h>
#include <console/console.h>
#include <console/uart.h>
#include <fs/virtio.h>

#include "plic.h"
#include "timer.h"

namespace orfos::kernel::ie {
  Cause which(uint64_t scause) {
    if (scause & 0x8000000000000000ull && (scause & 0xff) == 9) {
      auto irq = plicClaim();
      if (irq == UART0_IRQ) {
        console::uartInterrupt();
      } else if (irq == VIRTIO0_IRQ) {
        fs::diskInterrupt();
      } else {
        console::printf("Unexpected IRQ %d\n", irq);
      }
      if (irq) {
        plicComplete(irq);
      }
      return Cause::Plic;
    } else if (scause == 0x8000000000000001ull) {
      if (arch::read_tp() == 0) {
        clockInterrupt();
      }
      auto sip = arch::read_sip();
      arch::write_sip(sip & ~2);
      return Cause::Timer;
    } else {
      return Cause::Unexpected;
    }
  }
} // namespace orfos::kernel::ie