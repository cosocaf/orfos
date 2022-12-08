cmake_minimum_required(VERSION 3.16)

add_executable(
  kernel
  arch/riscv.cpp
  boot/entry.s
  boot/start.cpp
  boot/main.cpp
  driver/device_tree.cpp
  driver/device_tree_loader.cpp
  driver/init.cpp
  driver/memory/device.cpp
  driver/uart/device.cpp
  lib/panic.cpp
  lib/libc/string.cpp
  lib/libc/stdlib.cpp
  lib/libc/bits.cpp
)

target_include_directories(
  kernel PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_options(
  kernel PRIVATE
  $<$<COMPILE_LANGUAGE:CXX>:${COMMON_CXX_OPTIONS}>
)

target_link_options(
  kernel PRIVATE
  -z max-page-size=4096
  -nostdlib
  -T ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld
)

set_target_properties(
  kernel
  PROPERTIES
  LINK_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld
)
