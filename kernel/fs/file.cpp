#include "file.h"

#include <mutex/lock_guard.h>
#include <mutex/spin_mutex.h>

#include <cassert>

#include "device.h"
#include "inode.h"
#include "log.h"

namespace orfos::kernel::fs {
  namespace {
    constexpr size_t NUM_FILES = 200;
    struct {
      mutex::SpinMutex mutex;
      File files[NUM_FILES];
    } ftable;
  } // namespace
  File* File::allocate() {
    mutex::LockGuard guard(ftable.mutex);

    for (auto& file : ftable.files) {
      if (file.refCount == 0) {
        ++file.refCount;
        return &file;
      }
    }
    return nullptr;
  }
  File* File::dup() {
    mutex::LockGuard guard(ftable.mutex);
    assert(refCount >= 1);
    ++refCount;
    return this;
  }

  int File::write(uint64_t address, int n) {
    if (!writable) {
      return -1;
    }

    switch (type) {
      case FileType::Pipe:
        // TODO: implement
        return 0;
      case FileType::Device: {
        auto dev = getDevice(device.major);
        if (dev == nullptr) {
          return -1;
        }
        return dev->write(true, address, n);
      }
      case FileType::Inode: {
        int max = ((MAX_OP_BLOCKS - 1 - 1 - 2) / 2) * BLOCK_SIZE;
        int i   = 0;
        while (i < n) {
          auto len = n - i;
          if (len > max) {
            len = max;
          }
          beginOp();
          inode.ip->lock();
          auto res = inode.ip->write(true, address + i, inode.offset, len);
          if (res > 0) {
            inode.offset += res;
          }
          inode.ip->unlock();
          endOp();
          if (res != len) {
            break;
          }
          i += res;
        }
        return (i == n) ? n : -1;
      }
      default:
        assert(false);
    }
    return -1;
  }

  void File::close() {
    File file;
    {
      mutex::LockGuard guard(ftable.mutex);
      assert(refCount >= 1);
      if (--refCount > 0) {
        return;
      }
      file     = *this;
      refCount = 0;
      type     = FileType::None;
    }
    switch (file.type) {
      case FileType::None:
        // Do nothing.
        break;
      case FileType::Pipe:
        // TODO: implement
        break;
      case FileType::Inode:
        beginOp();
        file.inode.ip->put();
        endOp();
        break;
      case FileType::Device:
        beginOp();
        file.device.ip->put();
        endOp();
        break;
    }
  }
} // namespace orfos::kernel::fs