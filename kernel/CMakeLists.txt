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
  -fno-pie
  -no-pie
)

add_executable(
  kernel
  boot/entry.S
  boot/start.cpp
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
