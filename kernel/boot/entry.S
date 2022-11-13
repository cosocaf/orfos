#include "../arch/riscv_macros.h"

.section .text
.global _entry
_entry:
  la sp, stack
  li a0, 4096
  csrr a1, mhartid
  addi a1, a1, 1
  mul a0, a0, a1
  add sp, sp, a0
  call orfos_start
spin:
  wfi
  j spin

.section .data
.align 16
stack:
  .space 4096 * NUM_CPUS
