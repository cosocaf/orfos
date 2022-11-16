#ifndef ORFOS_KERNEL_ARCH_RISCV_H_
#define ORFOS_KERNEL_ARCH_RISCV_H_

#pragma once

#include <cstdint>

namespace orfos::kernel::arch {
  inline uint64_t read_mhartid() {
    uint64_t mhartid;
    asm volatile("csrr %0, mhartid" : "=r"(mhartid));
    return mhartid;
  }
  inline uint64_t read_mstatus() {
    uint64_t mstatus;
    asm volatile("csrr %0, mstatus" : "=r"(mstatus));
    return mstatus;
  }
  inline void write_mstatus(uint64_t mstatus) {
    asm volatile("csrw mstatus, %0" : : "r"(mstatus));
  }
  inline void write_mepc(uint64_t mepc) {
    asm volatile("csrw mepc, %0" : : "r"(mepc));
  }
  inline uint64_t read_mie() {
    uint64_t mie;
    asm volatile("csrr %0, mie" : "=r"(mie));
    return mie;
  }
  inline void write_mie(uint64_t mie) {
    asm volatile("csrw mie, %0" : : "r"(mie));
  }
  inline uint64_t read_medeleg() {
    uint64_t medeleg;
    asm volatile("csrr %0, medeleg" : "=r"(medeleg));
    return medeleg;
  }
  inline void write_medeleg(uint64_t medeleg) {
    asm volatile("csrw medeleg, %0" : : "r"(medeleg));
  }
  inline uint64_t read_mideleg() {
    uint64_t mideleg;
    asm volatile("csrr %0, mideleg" : "=r"(mideleg));
    return mideleg;
  }
  inline void write_mideleg(uint64_t mideleg) {
    asm volatile("csrw mideleg, %0" : : "r"(mideleg));
  }
  inline void write_mtvec(uint64_t mtvec) {
    asm volatile("csrw mtvec, %0" : : "r"(mtvec));
  }
  inline void write_mscratch(uint64_t mscratch) {
    asm volatile("csrw mscratch, %0" : : "r"(mscratch));
  }
  inline uint64_t read_mcounteren() {
    uint64_t mcounteren;
    asm volatile("csrr %0, mcounteren" : "=r"(mcounteren));
    return mcounteren;
  }
  inline void write_mcounteren(uint64_t mcounteren) {
    asm volatile("csrw mcounteren, %0" : : "r"(mcounteren));
  }
  inline uint64_t read_time() {
    uint64_t time;
    asm volatile("csrr %0, time" : "=r"(time));
    return time;
  }

  inline void write_pmpcfg0(uint64_t pmpcfg0) {
    asm volatile("csrw pmpcfg0, %0" : : "r"(pmpcfg0));
  }
  inline void write_pmpaddr0(uint64_t pmpaddr0) {
    asm volatile("csrw pmpaddr0, %0" : : "r"(pmpaddr0));
  }

  inline uint64_t read_sstatus() {
    uint64_t sstatus;
    asm volatile("csrr %0, sstatus" : "=r"(sstatus));
    return sstatus;
  }
  inline void write_sstatus(uint64_t sstatus) {
    asm volatile("csrw sstatus, %0" : : "r"(sstatus));
  }
  inline uint64_t read_sip() {
    uint64_t sip;
    asm volatile("csrr %0, sip" : "=r"(sip));
    return sip;
  }
  inline void write_sip(uint64_t sip) {
    asm volatile("csrw sip, %0" : : "r"(sip));
  }
  inline uint64_t read_sie() {
    uint64_t sie;
    asm volatile("csrr %0, sie" : "=r"(sie));
    return sie;
  }
  inline void write_sie(uint64_t sie) {
    asm volatile("csrw sie, %0" : : "r"(sie));
  }
  inline uint64_t read_sepc() {
    uint64_t sepc;
    asm volatile("csrr %0, sepc" : "=r"(sepc));
    return sepc;
  }
  inline void write_sepc(uint64_t sepc) {
    asm volatile("csrw sepc, %0" : : "r"(sepc));
  }
  inline uint64_t read_stvec() {
    uint64_t stvec;
    asm volatile("csrr %0, stvec" : "=r"(stvec));
    return stvec;
  }
  inline void write_stvec(uint64_t stvec) {
    asm volatile("csrw stvec, %0" : : "r"(stvec));
  }
  inline uint64_t read_satp() {
    uint64_t satp;
    asm volatile("csrr %0, satp" : "=r"(satp));
    return satp;
  }
  inline void write_satp(uint64_t satp) {
    asm volatile("csrw satp, %0" : : "r"(satp));
  }
  inline uint64_t read_scause() {
    uint64_t scause;
    asm volatile("csrr %0, scause" : "=r"(scause));
    return scause;
  }
  inline uint64_t read_stval() {
    uint64_t stval;
    asm volatile("csrr %0, stval" : "=r"(stval));
    return stval;
  }

  inline uint64_t read_sp() {
    uint64_t sp;
    asm volatile("mv %0, sp" : "=r"(sp));
    return sp;
  }
  inline uint64_t read_tp() {
    uint64_t tp;
    asm volatile("mv %0, tp" : "=r"(tp));
    return tp;
  }
  inline void write_tp(uint64_t tp) {
    asm volatile("mv tp, %0" : : "r"(tp));
  }
  inline uint64_t read_ra() {
    uint64_t ra;
    asm volatile("mv %0, ra" : : "r"(ra));
    return ra;
  }

  inline void sfence_vma() {
    asm volatile("sfence.vma zero, zero");
  }

  inline void mret() {
    asm volatile("mret");
  }
  inline void wfi() {
    asm volatile("wfi");
  }
} // namespace orfos::kernel::arch

#endif // ORFOS_KERNEL_ARCH_RISCV_H_