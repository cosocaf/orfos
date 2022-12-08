#ifndef ORFOS_KERNEL_ARCH_RISCV_H_
#define ORFOS_KERNEL_ARCH_RISCV_H_

#pragma once

#include <cstdint>

namespace orfos::kernel::arch {
#define ORFOS_KERNEL_ARCH_MAKE_REGISTER_CONTROL(w, r, name) \
  struct Register_##name {                                  \
    template <typename T>                                   \
    inline Register_##name& operator=(const T& value) {     \
      asm volatile(#w " " #name ", %0" : : "r"(value));     \
      return *this;                                         \
    }                                                       \
    template <typename T>                                   \
    inline operator T() {                                   \
      T value;                                              \
      asm volatile(#r " %0, " #name : "=r"(value));         \
      return value;                                         \
    }                                                       \
    template <typename T>                                   \
    inline bool operator==(const T& value) {                \
      return (static_cast<T>(*this)) == value;              \
    }                                                       \
  };                                                        \
  extern Register_##name name

#define ORFOS_KERNEL_ARCH_MAKE_CSR_CONTROL(name) \
  ORFOS_KERNEL_ARCH_MAKE_REGISTER_CONTROL(csrw, csrr, name)

#define ORFOS_KERNEL_ARCH_MAKE_MV_CONTROL(name) \
  ORFOS_KERNEL_ARCH_MAKE_REGISTER_CONTROL(mv, mv, name)

#define ORFOS_KERNEL_ARCH_MAKE_CALLABLE_CONTROL(name, expr) \
  struct Register_##name {                                  \
    inline void operator()() {                              \
      asm volatile(expr);                                   \
    }                                                       \
  };                                                        \
  extern Register_##name name

  ORFOS_KERNEL_ARCH_MAKE_CSR_CONTROL(mhartid);
  ORFOS_KERNEL_ARCH_MAKE_CSR_CONTROL(mstatus);
  ORFOS_KERNEL_ARCH_MAKE_CSR_CONTROL(mepc);
  ORFOS_KERNEL_ARCH_MAKE_CSR_CONTROL(mie);
  ORFOS_KERNEL_ARCH_MAKE_CSR_CONTROL(medeleg);
  ORFOS_KERNEL_ARCH_MAKE_CSR_CONTROL(mideleg);
  ORFOS_KERNEL_ARCH_MAKE_CSR_CONTROL(mtvec);
  ORFOS_KERNEL_ARCH_MAKE_CSR_CONTROL(mscratch);
  ORFOS_KERNEL_ARCH_MAKE_CSR_CONTROL(mcounteren);
  ORFOS_KERNEL_ARCH_MAKE_CSR_CONTROL(time);
  ORFOS_KERNEL_ARCH_MAKE_CSR_CONTROL(pmpcfg0);
  ORFOS_KERNEL_ARCH_MAKE_CSR_CONTROL(pmpaddr0);
  ORFOS_KERNEL_ARCH_MAKE_CSR_CONTROL(sstatus);
  ORFOS_KERNEL_ARCH_MAKE_CSR_CONTROL(sip);
  ORFOS_KERNEL_ARCH_MAKE_CSR_CONTROL(sie);
  ORFOS_KERNEL_ARCH_MAKE_CSR_CONTROL(sepc);
  ORFOS_KERNEL_ARCH_MAKE_CSR_CONTROL(stvec);
  ORFOS_KERNEL_ARCH_MAKE_CSR_CONTROL(satp);
  ORFOS_KERNEL_ARCH_MAKE_CSR_CONTROL(scause);
  ORFOS_KERNEL_ARCH_MAKE_CSR_CONTROL(stval);

  ORFOS_KERNEL_ARCH_MAKE_MV_CONTROL(sp);
  ORFOS_KERNEL_ARCH_MAKE_MV_CONTROL(tp);
  ORFOS_KERNEL_ARCH_MAKE_MV_CONTROL(ra);

  ORFOS_KERNEL_ARCH_MAKE_CALLABLE_CONTROL(sfence_vma, "sfence.vma zero, zero");
  ORFOS_KERNEL_ARCH_MAKE_CALLABLE_CONTROL(mret, "mret");
  ORFOS_KERNEL_ARCH_MAKE_CALLABLE_CONTROL(wfi, "wfi");

  constexpr uint64_t MSTATUS_MPP_MASK = 3 << 11;
  constexpr uint64_t MSTATUS_MPP_S    = 1 << 11;
  constexpr uint64_t MSTATUS_MIE      = 1 << 3;

  constexpr uint64_t MIE_MEIE = 1 << 11;
  constexpr uint64_t MIE_MTIE = 1 << 7;
  constexpr uint64_t MIE_MSIE = 1 << 3;

  constexpr uint64_t SSTATUS_SIE  = 1 << 1;
  constexpr uint64_t SSTATUS_SPP  = 1 << 8;
  constexpr uint64_t SSTATUS_SPIE = 1 << 5;

  constexpr uint64_t SIE_SEIE = 1 << 9;
  constexpr uint64_t SIE_STIE = 1 << 5;
  constexpr uint64_t SIE_SSIE = 1 << 1;

  constexpr uint64_t SATP_SV39 = 8ull << 60;
} // namespace orfos::kernel::arch

#endif // ORFOS_KERNEL_ARCH_RISCV_H_