cmake_minimum_required(VERSION 3.24)

set(CMAKE_CROSSCOMPILING TRUE)
SET(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR riscv64)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_C_FLAGS_INIT " ")
set(CMAKE_CXX_FLAGS_INIT " ")
set(CMAKE_BUILD_TYPE Debug)

set(TOOLCHAIN_PREFIX "riscv64-unknown-elf-")

set(CXX ${TOOLCHAIN_PREFIX}g++)
set(AS ${TOOLCHAIN_PREFIX}g++)
set(LD ${TOOLCHAIN_PREFIX}ld)
set(OBJCOPY ${TOOLCHAIN_PREFIX}objcopy)

set(CMAKE_CXX_COMPILER ${CXX})
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_ASM_COMPILER ${AS})

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
  -mno-relax
  -fno-stack-protector
  -fno-exceptions
  -fno-pie
  -no-pie
)

project(
  orfos
  LANGUAGES ASM CXX
)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/kernel)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/disk)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/user)
