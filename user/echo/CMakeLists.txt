cmake_minimum_required(VERSION 3.16)

add_executable(
  echo
  main.cpp
)

target_include_directories(
  echo PRIVATE
  ${USER_ROOT}
)

target_link_libraries(echo PRIVATE lib)

target_link_options(
  echo PRIVATE
  -z max-page-size=4096
  -nostdlib
  -T ${USER_ROOT}/linker.ld
)

set_target_properties(
  echo
  PROPERTIES
  LINK_DEPENDS ${USER_ROOT}/linker.ld
)
