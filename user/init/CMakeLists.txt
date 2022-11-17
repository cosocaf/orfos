cmake_minimum_required(VERSION 3.24)

add_executable(
  init
  main.cpp
)

target_include_directories(
  init PRIVATE
  ${USER_ROOT}
)
