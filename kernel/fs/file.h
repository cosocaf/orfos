#ifndef ORFOS_KERNEL_FS_FILE_H_
#define ORFOS_KERNEL_FS_FILE_H_

#pragma once

#include <cstdint>

#include "inode.h"

namespace orfos::kernel::fs {
  constexpr uint64_t MAX_PATH_LENGTH = 128;
  enum struct FileType {
    None,
    Pipe,
    Inode,
    Device,
  };
  struct File {
    FileType type;
    uint32_t refCount;
    bool readable;
    bool writable;
    union {
      struct {
        Inode* ip;
        uint32_t offset;
      } inode;
      struct {
        Inode* ip;
        int16_t major;
      } device;
    };
  };
} // namespace orfos::kernel::fs

#endif // ORFOS_KERNEL_FS_FILE_H_