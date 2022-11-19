#include "syscall.h"

#include <kernel/syscall/close.h>
#include <kernel/syscall/exec.h>
#include <kernel/syscall/exit.h>
#include <kernel/syscall/fork.h>
#include <kernel/syscall/mknod.h>
#include <kernel/syscall/open.h>
#include <kernel/syscall/write.h>

extern "C" {
uint64_t sys_fork();
[[noreturn]] uint64_t sys_exit(int);
uint64_t sys_read(int, char*, int);
uint64_t sys_exec(const char*, const char**);
uint64_t sys_open(const char*, int);
uint64_t sys_write(int, const char*, int);
uint64_t sys_mknod(const char*, int16_t, int16_t);
uint64_t sys_close(int);
}

using namespace orfos::kernel;
uint64_t fork() {
  return sys_fork();
}
[[noreturn]] void exit(int status) {
  sys_exit(status);
}
int read(int fd, char* buf, int len) {
  return sys_read(fd, buf, len);
}
void exec(const char* path, const char** argv) {
  sys_exec(path, argv);
}
int open(const char* path, int mode) {
  return sys_open(path, mode);
}
int write(int fd, const char* buf, int len) {
  return sys_write(fd, buf, len);
}
bool mknod(const char* path, int16_t major, int16_t minor) {
  auto ret = sys_mknod(path, major, minor);
  return ret == 0;
}
void close(int fd) {
  sys_close(fd);
}
