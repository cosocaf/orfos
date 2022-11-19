#ifndef ORFOS_KERNEL_FS_INODE_H_
#define ORFOS_KERNEL_FS_INODE_H_

#pragma once

#include <mutex/sleep_mutex.h>

#include <cstdint>

#include "block.h"
#include "buffer.h"
#include "super_block.h"

namespace orfos::kernel::fs {
  constexpr uint64_t NUM_DIRECT_BLOCKS   = 12;
  constexpr uint64_t NUM_INDIRECT_BLOCKS = BLOCK_SIZE / sizeof(uint32_t);
  constexpr uint64_t MAX_FILE    = NUM_DIRECT_BLOCKS + NUM_INDIRECT_BLOCKS;
  constexpr uint64_t ROOT_DEVICE = 1;
  constexpr uint64_t ROOT_INUM   = 1;
  constexpr uint64_t NUM_INODES  = 200;

  struct Inode {
    uint32_t device;
    uint32_t inum;
    uint32_t refCount;
    mutex::SleepMutex mutex;
    bool valid;
    int16_t type;
    int16_t major;
    int16_t minor;
    int16_t numLinks;
    uint32_t size;
    uint32_t directBlocks[NUM_DIRECT_BLOCKS];
    uint32_t indirectBlock;

  private:
    static Inode* allocate(uint32_t device, int16_t type);
    static Inode* get(uint32_t device, uint32_t inum);
    void update();

    uint32_t bmap(uint32_t bn);

    static Inode* open(const char* path, bool parent, char* name);

  public:
    static Inode* open(const char* path);
    static Inode* open(const char* path, char* name);
    static Inode* create(const char* path,
                         int16_t type,
                         int16_t major,
                         int16_t minor);

    Inode* dup();
    void truncate();

    void lock();
    void unlock();
    void put();

    Inode* dirLookup(const char* name, uint32_t* poffset);
    bool dirLink(const char* name, uint32_t inum);
    int read(bool userDst, uint64_t dst, uint32_t offset, uint32_t length);
    int write(bool userSrc, uint64_t src, uint32_t offset, uint32_t length);
  };

  struct DiskInode {
    int16_t type;
    int16_t major;
    int16_t minor;
    int16_t numLinks;
    uint32_t size;
    uint32_t directBlocks[NUM_DIRECT_BLOCKS];
    uint32_t indirectBlock;
  };

  constexpr uint64_t INODES_PER_BLOCK    = BLOCK_SIZE / sizeof(DiskInode);
  constexpr uint64_t MAX_DIR_NAME_LENGTH = 14;

  struct DirectoryEntry {
    uint16_t inum;
    char name[MAX_DIR_NAME_LENGTH];
  };

  void initializeInode();
  constexpr inline uint32_t iblock(uint32_t i, const SuperBlock& sb) {
    return i / INODES_PER_BLOCK + sb.inodeStart;
  }
} // namespace orfos::kernel::fs

#endif // ORFOS_KERNEL_FS_INODE_H_