cmake_minimum_required(VERSION 3.24)

add_executable(
  sh
  main.cpp
)

target_include_directories(
  sh PRIVATE
  ${USER_ROOT}
)

target_link_libraries(sh PRIVATE lib)

target_link_options(
  sh PRIVATE
  -z max-page-size=4096
  -nostdlib
  -T ${USER_ROOT}/linker.ld
)

set_target_properties(
  sh
  PROPERTIES
  LINK_DEPENDS ${USER_ROOT}/linker.ld
)
