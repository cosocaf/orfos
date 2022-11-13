#include "uart.h"

#include <mutex/lock_guard.h>
#include <mutex/spin_mutex.h>

namespace orfos::kernel::console {
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
    constexpr inline uint8_t* reg(uint64_t reg) {
      return reinterpret_cast<uint8_t*>(UART_BASE + reg);
    }
    constexpr inline uint8_t read_reg(uint64_t r) {
      return *reg(r);
    }
    constexpr inline void write_reg(uint64_t r, uint8_t val) {
      *reg(r) = val;
    }

    mutex::SpinMutex mutex;
  } // namespace
  void initialize_uart() {
    write_reg(IER, 0x00);
    write_reg(LCR, LCR_BAUD_LATCH);
    write_reg(0, 0x03);
    write_reg(1, 0x00);
    write_reg(LCR, LCR_EIGHT_BITS);
    write_reg(FCR, FCR_FIFO_ENABLE | FCR_FIFO_CLEAR);
    write_reg(IER, IER_TX_ENABLE | IER_RX_ENABLE);
  }
  void uart_putc(char c) {
    mutex::LockGuard guard(mutex);
    if (read_reg(LSR) & LSR_TX_IDLE) {
      write_reg(THR, c);
    }
  }
  void uart_putc_sync(char c) {
    mutex.push();
    while (!(read_reg(LSR) & LSR_TX_IDLE)) {
      // Wait
    }
    write_reg(THR, c);
    mutex.pop();
  }
  char uart_getc() {
    if (read_reg(LSR) & 0x01) {
      return read_reg(RHR);
    }
    return '\0';
  }
} // namespace orfos::kernel::console