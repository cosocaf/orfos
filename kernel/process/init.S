#include "../kernel/syscall/syscall_key.h"

.section .userinit
.global orfos_userEntry
orfos_userEntry:
  la a0, init
  la a1, argv
  li a7, SYS_EXEC
  ecall

exit:
  li a7, SYS_EXIT
  ecall
  jal exit

init:
  .string "/init\0"

.p2align 2
argv:
  .long init - orfos_userEntry
  .long 0
