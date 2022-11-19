#include "open.h"

#include <fs/file.h>
#include <process/cpu.h>
#include <console/console.h>

#include "args.h"

namespace orfos::kernel::syscall {
  uint64_t open() {
    char path[fs::MAX_PATH_LENGTH];

    int openMode;
    getArgInt(1, &openMode);
    auto length = getArgString(0, path, sizeof(path));
    if (length < 0) {
      return -1;
    }

    auto proc = process::Cpu::current().process;
    return proc->open(path, openMode);
  }
} // namespace orfos::kernel::syscall