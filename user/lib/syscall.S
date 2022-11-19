#include <kernel/syscall/syscall_key.h>

.global sys_fork
sys_fork:
  li a7, SYS_FORK
  ecall
  ret

.global sys_exit
sys_exit:
  li a7, SYS_EXIT
  ecall
  ret

.global sys_exec
sys_exec:
  li a7, SYS_EXEC
  ecall
  ret

.global sys_open
sys_open:
  li a7, SYS_OPEN
  ecall
  ret

.global sys_write
sys_write:
  li a7, SYS_WRITE
  ecall
  ret

.global sys_mknod
sys_mknod:
  li a7, SYS_MKNOD
  ecall
  ret
