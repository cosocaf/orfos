#ifndef ORFOS_KERNEL_ARCH_RISCV_MACROS_H_
#define ORFOS_KERNEL_ARCH_RISCV_MACROS_H_

#pragma once

// mstatusレジスタのmpp領域のマスク
#define MSTATUS_MPP_MASK (3 << 11)
// mstatusレジスタのmpp領域のs-modeフラグ
#define MSTATUS_MPP_S (1 << 11)
#define MSTATUS_MIE (1 << 3)

#define MIE_MTIE (1 << 7)

#define SIE_SEIE (1 << 9)
#define SIE_STIE (1 << 5)
#define SIE_SSIE (1 << 1)

#define NUM_CPUS (8)

#define CLINT (0x2000000)
#define CLINT_MTIME (CLINT + 0xBFF8)

#endif // ORFOS_KERNEL_ARCH_RISCV_MACROS_H_