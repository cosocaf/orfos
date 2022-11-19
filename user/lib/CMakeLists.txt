cmake_minimum_required(VERSION 3.24)

add_library(
  lib STATIC
  syscall.cpp
  syscall.S
  file.cpp
  stdio.cpp
)

target_include_directories(
  lib PRIVATE
  ${ROOT_DIR}
  ${USER_ROOT}
)