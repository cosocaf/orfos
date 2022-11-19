cmake_minimum_required(VERSION 3.16)

# add_executable(
# init.elf
# init.S
# )

# target_compile_options(
# init.elf PRIVATE
# ${COMMON_CXX_OPTIONS}
# -march=rv64g
# -nostdinc
# )

# target_link_options(
# init.elf PRIVATE
# -nostdlib
# -z max-page-size=4096
# -N
# -e start
# -Ttext 0
# )

# add_custom_command(
# TARGET init.elf POST_BUILD
# COMMAND ${OBJCOPY} -S -O binary $<TARGET_FILE:init.elf> init
# )
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_SYSTEM_PROCESSOR aarch64)

add_executable(
  make_disk
  make_disk.cpp
)

target_include_directories(
  make_disk PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/..
  ${CMAKE_CURRENT_SOURCE_DIR}/../kernel
)

set(
  MAKE_DISK_COMMAND
  $<TARGET_FILE:make_disk>
  ${CMAKE_CURRENT_SOURCE_DIR}/../fs.img
  ${CMAKE_CURRENT_SOURCE_DIR}/../README.md
  $<TARGET_FILE:init>
  $<TARGET_FILE:sh>
)

add_custom_target(
  run_make_disk ALL
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../
  COMMAND ${MAKE_DISK_COMMAND}
)
