#ifndef ORFOS_KERNEL_IE_CLINT_H_
#define ORFOS_KERNEL_IE_CLINT_H_

#include <arch/riscv.h>

#include <cstdint>

namespace orfos::kernel::ie {
  constexpr uint64_t CLINT_BASE = 0x0200'0000;
  struct RegisterMsip {
    constexpr static uint64_t BASE = CLINT_BASE + 0x0000;
    void fire(uint64_t hartid);
  };
  struct RegisterMtimeCmp {
    constexpr static uint64_t BASE = CLINT_BASE + 0x4000;
    template <typename T>
    RegisterMtimeCmp& operator=(const T& value) {
      uint64_t hartid    = arch::mhartid;
      uint64_t address   = BASE + 8 * hartid;
      uint64_t* mtimecmp = reinterpret_cast<uint64_t*>(address);
      *mtimecmp          = reinterpret_cast<uint64_t>(value);
      return *this;
    }
    uint64_t* operator&() {
      uint64_t hartid    = arch::mhartid;
      uint64_t address   = BASE + 8 * hartid;
      uint64_t* mtimecmp = reinterpret_cast<uint64_t*>(address);
      return mtimecmp;
    }
  };
  struct RegisterMtime {
    constexpr static uint64_t BASE = CLINT_BASE + 0xBFF8;
    template <typename T>
    operator T() {
      uint64_t* mtime = reinterpret_cast<uint64_t*>(BASE);
      return *mtime;
    }
  };

  extern RegisterMsip msip;
  extern RegisterMtimeCmp mtimecmp;
  extern RegisterMtime mtime;
} // namespace orfos::kernel::ie

#endif // ORFOS_KERNEL_IE_CLINT_H_