#include <kernel/syscall/syscall_key.h>

.global sys_fork
sys_fork:
  li a7, SYS_FORK
  ecall
  ret

.global sys_read
sys_read:
  li a7, SYS_READ
  ecall
  ret

.global sys_wait
sys_wait:
  li a7, SYS_WAIT
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

.global sys_close
sys_close:
  li a7, SYS_CLOSE
  ecall
  ret
