#ifndef ORFOS_KERNEL_ELF_EXEC_H_
#define ORFOS_KERNEL_ELF_EXEC_H_

#pragma once

#include <cstdint>

namespace orfos::kernel::elf {
  constexpr uint64_t MAX_ARGS = 32;
  int exec(char* path, char** argv);
} // namespace orfos::kernel::elf

#endif // ORFOS_KERNEL_ELF_EXEC_H_