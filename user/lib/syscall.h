#ifndef ORFOS_USER_LIB_SYSCALL_H_
#define ORFOS_USER_LIB_SYSCALL_H_

#pragma once

#include <cstdint>

uint64_t fork();
[[noreturn]] void exit(int status);
void exec(const char* path, const char** argv);
int open(const char* path, int mode);
int write(int fd, const char* buf, int len);
bool mknod(const char* path, int16_t major, int16_t minor);

#endif // ORFOS_USER_LIB_SYSCALL_H_