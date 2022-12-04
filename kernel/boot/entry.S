.global orfos_entry
.global orfos_primaryStart
.global orfos_secondaryStart
.global orfos_startedCoreCount
.global orfos_nextStackPointer

.section .entry
# カーネルエントリポイント
# 0x80000000に配置される
# ブートローダによって
# a0: hartid
# a1: dtb
# がそれぞれ配置されているっぽい?
orfos_entry:
  beq a0, zero, main_boot
  j sub_boot

# メインコア用の初期化ルーチン
# hartid = 0の時に実行される
main_boot:
  la sp, primary_stack_begin
  call orfos_primaryStart
  j spin

# サブコア用の初期化ルーチン
# hartid != 0の時に実行される
sub_boot:
  csrr a0, mhartid
  add a0, a0, -1
  la a1, orfos_startedCoreCount
  bne a0, a1, sub_boot
  ld sp, orfos_nextStackPointer
  call orfos_secondaryStart
  j spin

spin:
  wfi
  j spin

.section .data
.align 16
primary_stack:
  .space 4096
primary_stack_begin:
orfos_startedCoreCount:
  .8byte 0
orfos_nextStackPointer:
  .8byte 0
