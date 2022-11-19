cmake_minimum_required(VERSION 3.16)

add_library(
  lib STATIC
  syscall.cpp
  syscall.S
  file.cpp
  stdio.cpp
  string.cpp
)

target_include_directories(
  lib PRIVATE
  ${ROOT_DIR}
  ${USER_ROOT}
)
