#include "../arch/config.h"

.section .text
.global _entry
_entry:
  # hartごとにスタックを割り振る
  la sp, stack
  li a0, 4096
  csrr a1, mhartid
  addi a1, a1, 1
  mul a0, a0, a1
  add sp, sp, a0
  # C++側に実行を移す
  call orfos_start
spin:
  wfi
  j spin

.section .data
.align 16
stack:
  .space 4096 * NUM_CPUS
