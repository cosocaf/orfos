cmake_minimum_required(VERSION 3.24)

set(
  COMMON_CXX_OPTIONS
  -Wall
  -Werror
  -O0
  -fno-omit-frame-pointer
  -ggdb
  -gdwarf-2
  -MD
  -mcmodel=medany
  -ffreestanding
  -fno-common
  -nostdlib
  -mno-relax
  -fno-stack-protector
  -fno-exceptions
  -fno-pie
  -no-pie
)

add_executable(
  kernel.elf
  boot/entry.S
  boot/start.cpp
  boot/main.cpp
  console/console.cpp
  console/uart.cpp
  mutex/spin_mutex.cpp
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
