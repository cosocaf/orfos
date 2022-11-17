#ifndef ORFOS_KERNEL_ARCH_CONFIG_H_
#define ORFOS_KERNEL_ARCH_CONFIG_H_

#pragma once

// カーネルのエントリポイント
#define KERNEL_BASE 0x80000000ull
#define MAX_VIRTUAL_ADDRESS (1ull << (9 + 9 + 9 + 12 - 1))

#define CLINT (0x2000000)
#define CLINT_MTIME (CLINT + 0xBFF8)

#define PLIC 0x0c000000L
#define UART0_IRQ 10
#define VIRTIO0_IRQ 1

// hartの数
#define NUM_CPUS 8

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif // PAGE_SIZE

#define TRAMPOLINE (MAX_VIRTUAL_ADDRESS - PAGE_SIZE)

#define TRAPFRAME (TRAMPOLINE - PAGE_SIZE)

#endif // ORFOS_KERNEL_ARCH_CONFIG_H_