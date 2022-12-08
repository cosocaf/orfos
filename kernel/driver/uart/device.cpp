#include "device.h"

#include <lib/format.h>

namespace orfos::kernel::driver::uart {
  namespace {
    constexpr uint64_t RHR             = 0;
    constexpr uint64_t THR             = 0;
    constexpr uint64_t IER             = 1;
    constexpr uint64_t IER_RX_ENABLE   = 1 << 0;
    constexpr uint64_t IER_TX_ENABLE   = 1 << 1;
    constexpr uint64_t FCR             = 2;
    constexpr uint64_t FCR_FIFO_ENABLE = 1 << 0;
    constexpr uint64_t FCR_FIFO_CLEAR  = 3 << 1;
    constexpr uint64_t ISR             = 2;
    constexpr uint64_t LCR             = 3;
    constexpr uint64_t LCR_EIGHT_BITS  = 3 << 0;
    constexpr uint64_t LCR_BAUD_LATCH  = 1 << 7;
    constexpr uint64_t LSR             = 5;
    constexpr uint64_t LSR_RX_READY    = 1 << 0;
    constexpr uint64_t LSR_TX_IDLE     = 1 << 5;

    uintptr_t uartBase;

    inline uint8_t* reg(uint64_t reg) {
      return reinterpret_cast<uint8_t*>(uartBase + reg);
    }
    inline uint8_t readReg(uint64_t r) {
      return *reg(r);
    }
    inline void writeReg(uint64_t r, uint8_t val) {
      *reg(r) = val;
    }

    void init(uintptr_t base) {
      uartBase = base;
      writeReg(IER, 0x00);
      writeReg(LCR, LCR_BAUD_LATCH);
      writeReg(0, 0x03);
      writeReg(1, 0x00);
      writeReg(LCR, LCR_EIGHT_BITS);
      writeReg(FCR, FCR_FIFO_ENABLE | FCR_FIFO_CLEAR);
      writeReg(IER, IER_TX_ENABLE | IER_RX_ENABLE);
    }
  } // namespace

  void putc(char c) {
    if (!(readReg(LSR) & LSR_TX_IDLE)) {
      return;
    }
    writeReg(THR, c);
  }
  void putcSync(char c) {
    while (!(readReg(LSR) & LSR_TX_IDLE)) {
      // Wait
    }
    writeReg(THR, c);
  }
  char getc() {
    if (readReg(LSR) & 0x01) {
      return readReg(RHR);
    }
    return '\0';
  }

  lib::Result<lib::_, lib::FixedString<>> initialize(const DeviceTreeNode& stdoutNode) {
    auto stdoutPropReg = stdoutNode.findProperty("reg");
    if (!stdoutPropReg) {
      return lib::error(
        lib::format<lib::FixedString<>>("The 'reg' property does not exist on '{}' device.",
                                        stdoutNode.getName().c_str())
          .unwrap());
    }

    auto stdoutPropCompatible = stdoutNode.findProperty("compatible");
    if (!stdoutPropCompatible) {
      return lib::error(
        lib::format<lib::FixedString<>>("The 'compatible' property does not exist on '{}' device.",
                                        stdoutNode.getName().c_str())
          .unwrap());
    }

    auto stdoutCompatible = stdoutPropCompatible->getValueAsString();
    if (strcmp(stdoutCompatible, "ns16550") == 0 || strcmp(stdoutCompatible, "ns16550a") == 0) {
      init(stdoutPropReg->getValueAsU64());
    } else {
      return lib::error(
        lib::format<lib::FixedString<>>("'{}' is not supported.", stdoutCompatible).unwrap());
    }

    return lib::ok();
  }
} // namespace orfos::kernel::driver::uart