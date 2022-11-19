#ifndef ORFOS_KERNEL_FS_FILE_H_
#define ORFOS_KERNEL_FS_FILE_H_

#pragma once

#include <cstdint>

namespace orfos::kernel::fs {
  constexpr uint64_t MAX_PATH_LENGTH = 128;
  constexpr uint64_t O_RDONLY        = 0x000;
  constexpr uint64_t O_WRONLY        = 0x001;
  constexpr uint64_t O_RDWR          = 0x002;
  constexpr uint64_t O_CREATE        = 0x200;
  constexpr uint64_t O_TRUNC         = 0x400;

  struct Inode;
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

    static File* allocate();
    File* dup();
    int write(uint64_t address, int n);
    void close();
  };
} // namespace orfos::kernel::fs

#endif // ORFOS_KERNEL_FS_FILE_H_