#include "close.h"

#include <fs/file.h>
#include <process/cpu.h>

#include "args.h"

namespace orfos::kernel::syscall {
  uint64_t close() {
    auto proc = process::Cpu::current().process;
    int fd;
    if (!getArgFd(0, &fd, nullptr)) {
      return -1;
    }
    return proc->close(fd);
  }
} // namespace orfos::kernel::syscall