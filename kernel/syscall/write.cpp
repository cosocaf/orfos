#include "write.h"

#include <console/console.h>

#include "args.h"

namespace orfos::kernel::syscall {
  uint64_t write() {
    uint64_t address;
    int len;
    fs::File* file;
    getArgAddress(1, &address);
    getArgInt(2, &len);
    if (!getArgFd(0, nullptr, &file)) {
      return -1;
    }

    return file->write(address, len);
  }
} // namespace orfos::kernel::syscall