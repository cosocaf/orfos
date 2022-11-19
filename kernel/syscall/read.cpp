#include "read.h"

#include <fs/file.h>

#include "args.h"

namespace orfos::kernel::syscall {
  uint64_t read() {
    fs::File* file;
    uint64_t buf;
    int n;
    if (!getArgFd(0, nullptr, &file)) {
      return -1;
    }
    getArgAddress(1, &buf);
    getArgInt(2, &n);
    return file->read(buf, n);
  }
} // namespace orfos::kernel::syscall