#include "args.h"

#include <lib/string.h>
#include <process/cpu.h>

#include <cassert>
namespace orfos::kernel::syscall {
  namespace {
    uint64_t getArg(int n) {
      auto proc = process::Cpu::current().process;
      assert(n >= 0 && n <= 5);
      switch (n) {
        case 0:
          return proc->trapFrame->a0;
        case 1:
          return proc->trapFrame->a1;
        case 2:
          return proc->trapFrame->a2;
        case 3:
          return proc->trapFrame->a3;
        case 4:
          return proc->trapFrame->a4;
        case 5:
          return proc->trapFrame->a5;
      }
      return -1;
    }
  } // namespace
  bool fetchAddress(uint64_t address, uint64_t* ip) {
    auto proc = process::Cpu::current().process;
    if (address >= proc->memorySize) {
      return false;
    }
    if (address + sizeof(uint64_t) > proc->memorySize) {
      return false;
    }

    return proc->pageTable->copyin(
      reinterpret_cast<char*>(ip), address, sizeof(*ip));
  }
  int fetchString(uint64_t address, char* buf, uint64_t length) {
    auto proc = process::Cpu::current().process;

    if (!proc->pageTable->copyinString(buf, address, length)) {
      return -1;
    }
    return strlen(buf);
  }
  void getArgInt(int n, int* ip) {
    *ip = getArg(n);
  }
  void getArgAddress(int n, uint64_t* ip) {
    *ip = getArg(n);
  }
  int getArgString(int n, char* buf, uint64_t length) {
    uint64_t address;
    getArgAddress(n, &address);
    return fetchString(address, buf, length);
  }
  bool getArgFd(int n, int* pfd, fs::File** pfile) {
    auto proc = process::Cpu::current().process;

    int fd;
    getArgInt(n, &fd);
    if (fd < 0) {
      return false;
    }
    if (static_cast<size_t>(fd) >= proc->openFiles.size()) {
      return false;
    }
    if (proc->openFiles[fd] == nullptr) {
      return false;
    }

    if (pfd) {
      *pfd = fd;
    }
    if (pfile) {
      *pfile = proc->openFiles[fd];
    }

    return true;
  }
} // namespace orfos::kernel::syscall