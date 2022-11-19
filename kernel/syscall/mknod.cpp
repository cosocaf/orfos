#include "mknod.h"

#include <console/console.h>
#include <fs/file.h>
#include <fs/inode.h>
#include <fs/log.h>
#include <fs/stat.h>

#include "args.h"

namespace orfos::kernel::syscall {
  uint64_t mknod() {
    char path[fs::MAX_PATH_LENGTH];

    fs::beginOp();
    int major, minor;
    getArgInt(1, &major);
    getArgInt(2, &minor);
    if (getArgString(0, path, sizeof(path)) < 0) {
      fs::endOp();
      return -1;
    }

    auto ip = fs::Inode::create(path, fs::T_DEVICE, major, minor);
    if (ip == nullptr) {
      fs::endOp();
      return -1;
    }

    ip->unlock();
    ip->put();
    fs::endOp();
    return 0;
  }
} // namespace orfos::kernel::syscall