#ifndef ORFOS_KERNEL_SYSCALL_ARGS_H_
#define ORFOS_KERNEL_SYSCALL_ARGS_H_

#pragma once

#include <fs/file.h>

#include <cstdint>

namespace orfos::kernel::syscall {
  bool fetchAddress(uint64_t address, uint64_t* ip);
  int fetchString(uint64_t address, char* buf, uint64_t length);
  void getArgInt(int n, int* ip);
  void getArgAddress(int n, uint64_t* ip);
  int getArgString(int n, char* buf, uint64_t length);
  bool getArgFd(int n, int* pfd, fs::File** file);
} // namespace orfos::kernel::syscall

#endif // ORFOS_KERNEL_SYSCALL_ARGS_H_