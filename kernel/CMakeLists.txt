cmake_minimum_required(VERSION 3.24)

add_executable(
  kernel.elf
  boot/entry.S
  boot/start.cpp
  boot/main.cpp
  console/console.cpp
  console/uart.cpp
  mutex/spin_mutex.cpp
  mutex/sleep_mutex.cpp
  memory/page.cpp
  memory/pte.cpp
  memory/va.cpp
  memory/page_table.cpp
  memory/kvm.cpp
  memory/uvm.cpp
  memory/copy.cpp
  process/context.S
  process/context.cpp
  process/process.cpp
  process/cpu.cpp
  process/scheduler.cpp
  process/sleep.cpp
  process/user_init_proc.cpp
  process/init.S
  ie/ie.cpp
  ie/user_vector.S
  ie/user_trap.cpp
  ie/kernel_vector.S
  ie/kernel_trap.cpp
  ie/timer.cpp
  ie/cause.cpp
  ie/plic.cpp
  fs/fs.cpp
  fs/file.cpp
  fs/device.cpp
  fs/buffer.cpp
  fs/block.cpp
  fs/super_block.cpp
  fs/inode.cpp
  fs/log.cpp
  fs/virtio.cpp
  syscall/syscall.cpp
  syscall/args.cpp
  syscall/fork.cpp
  syscall/exit.cpp
  syscall/exec.cpp
  syscall/open.cpp
  syscall/write.cpp
  syscall/mknod.cpp
  elf/exec.cpp
  lib/assert.cpp
  lib/stdexcept.cpp
  lib/string.cpp
  lib/new.cpp
  lib/panic.cpp
)

target_include_directories(
  kernel.elf PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_options(
  kernel.elf PRIVATE
  $<$<COMPILE_LANGUAGE:CXX>:${COMMON_CXX_OPTIONS}>
)

target_link_options(
  kernel.elf PRIVATE
  -z max-page-size=4096
  -nostdlib
  -T ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld
)

set_target_properties(
  kernel.elf
  PROPERTIES
  LINK_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld
)
