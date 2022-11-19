cmake_minimum_required(VERSION 3.16)

add_executable(
  init
  main.cpp
)

target_include_directories(
  init PRIVATE
  ${USER_ROOT}
)

target_link_libraries(init PRIVATE lib)

target_link_options(
  init PRIVATE
  -z max-page-size=4096
  -nostdlib
  -T ${USER_ROOT}/linker.ld
)

set_target_properties(
  init
  PROPERTIES
  LINK_DEPENDS ${USER_ROOT}/linker.ld
)
