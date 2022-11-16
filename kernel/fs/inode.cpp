#include "inode.h"

#include <console/console.h>
#include <lib/hash_map.h>
#include <lib/string.h>
#include <memory/copy.h>
#include <mutex/lock_guard.h>
#include <process/cpu.h>

#include <cassert>

#include "log.h"
#include "stat.h"
#include "super_block.h"

namespace orfos::kernel::fs {
  namespace {
    struct Itable {
      mutex::SpinMutex mutex;
      lib::HashMap<uint64_t, Inode> inodes;
    };

    const char* skipElement(const char* path, char* name) {
      while (*path == '/') {
        ++path;
      }
      if (*path == 0) {
        return nullptr;
      }

      auto s = path;
      while (*path != '/' && *path != 0) {
        ++path;
      }

      uint64_t len = path - s;
      if (len >= MAX_DIR_NAME_LENGTH) {
        memmove(name, s, MAX_DIR_NAME_LENGTH);
      } else {
        memmove(name, s, len);
        name[len] = '\0';
      }
      while (*path == '/') {
        ++path;
      }
      return path;
    }

    Itable* itable;
  } // namespace
  Inode* Inode::get(uint32_t device, uint32_t inum) {
    mutex::LockGuard guard(itable->mutex);

    uint64_t key = (static_cast<uint64_t>(device) << 32) | inum;
    if (itable->inodes.contains(key)) {
      ++itable->inodes[key].refCount;
      return &itable->inodes[key];
    }

    itable->inodes[key].device   = device;
    itable->inodes[key].inum     = inum;
    itable->inodes[key].refCount = 1;
    itable->inodes[key].valid    = false;

    return &itable->inodes[key];
  }
  Inode* Inode::dup(Inode* ip) {
    mutex::LockGuard guard(itable->mutex);
    ++ip->refCount;
    return ip;
  }

  void Inode::lock() {
    assert(refCount >= 1);

    mutex.lock();

    if (valid) {
      return;
    }

    auto bp = bufferCache->get(device, iblock(inum, superBlock));
    bp->read();
    auto dip = reinterpret_cast<DiskInode*>(bp->data) + inum % INODES_PER_BLOCK;
    type     = dip->type;
    major    = dip->major;
    minor    = dip->minor;
    numLinks = dip->numLinks;
    size     = dip->size;
    memmove(directBlocks, dip->directBlocks, sizeof(directBlocks));
    indirectBlock = dip->indirectBlock;
    bufferCache->release(bp);
    valid = true;
    assert(type != 0);
  }

  void Inode::unlock() {
    assert(mutex.holding());
    assert(refCount >= 1);

    mutex.unlock();
  }

  void Inode::put() {
    itable->mutex.lock();

    if (refCount == 1 && valid && numLinks == 0) {
      mutex.lock();
      itable->mutex.unlock();

      truncate();
      type = 0;
      update();
      valid = false;

      mutex.unlock();

      itable->mutex.lock();
    }

    --refCount;
    itable->mutex.lock();
  }

  void Inode::truncate() {
    for (auto& direct : directBlocks) {
      if (direct) {
        freeBlock(device, direct);
        direct = 0;
      }
    }
    if (indirectBlock) {
      auto bp = bufferCache->get(device, indirectBlock);
      bp->read();
      auto address = reinterpret_cast<uint32_t*>(bp->data);
      for (uint64_t i = 0; i < NUM_INDIRECT_BLOCKS; ++i) {
        if (address[i]) {
          freeBlock(device, address[i]);
        }
      }
      bufferCache->release(bp);
      freeBlock(device, indirectBlock);
      indirectBlock = 0;
    }

    size = 0;
    update();
  }
  void Inode::update() {
    auto bp = bufferCache->get(device, iblock(inum, superBlock));
    bp->read();

    auto dip = reinterpret_cast<DiskInode*>(bp->data) + inum % INODES_PER_BLOCK;
    dip->type     = type;
    dip->major    = major;
    dip->minor    = minor;
    dip->numLinks = numLinks;
    dip->size     = size;
    memmove(dip->directBlocks, directBlocks, sizeof(directBlocks));
    dip->indirectBlock = indirectBlock;
    writeLog(bp);

    bufferCache->release(bp);
  }

  uint32_t Inode::bmap(uint32_t bn) {
    if (bn < NUM_DIRECT_BLOCKS) {
      if (directBlocks[bn] == 0) {
        directBlocks[bn] = allocateBlock(device);
      }
      return directBlocks[bn];
    }

    bn -= NUM_DIRECT_BLOCKS;
    assert(bn < NUM_INDIRECT_BLOCKS);

    if (indirectBlock == 0) {
      indirectBlock = allocateBlock(device);
      if (indirectBlock == 0) {
        return 0;
      }
    }
    auto bp      = bufferCache->get(device, indirectBlock);
    auto address = reinterpret_cast<uint32_t*>(bp->data);
    if (address[bn] == 0) {
      address[bn] = allocateBlock(device);
      if (address[bn]) {
        writeLog(bp);
      }
    }
    bufferCache->release(bp);
    return address[bn];
  }

  Inode* Inode::open(const char* path, bool parent, char* name) {
    Inode* ip;
    if (*path == '/') {
      ip = get(ROOT_DEVICE, ROOT_INUM);
    } else {
      ip = dup(process::Cpu::current().process->cwd);
    }

    while ((path = skipElement(path, name)) != 0) {
      ip->lock();
      if (ip->type != T_DIR) {
        ip->unlock();
        ip->put();
        return nullptr;
      }
      if (parent && *path == '\0') {
        ip->unlock();
        return ip;
      }
      auto next = ip->dirLookup(name, nullptr);
      if (next == nullptr) {
        ip->unlock();
        ip->put();
        return nullptr;
      }
      ip->unlock();
      ip->put();
      ip = next;
    }

    if (parent) {
      ip->put();
      return nullptr;
    }

    return ip;
  }
  Inode* Inode::open(const char* path) {
    char name[MAX_DIR_NAME_LENGTH];
    return open(path, false, name);
  }
  Inode* Inode::open(const char* path, char* name) {
    return open(path, true, name);
  }

  Inode* Inode::dirLookup(char* name, uint32_t* poffset) {
    assert(type == T_DIR);

    DirectoryEntry entry;
    for (uint32_t offset = 0; offset < size; offset += sizeof(entry)) {
      auto len = read(
        false, reinterpret_cast<uint64_t>(&entry), offset, sizeof(entry));
      assert(len == sizeof(entry));

      if (entry.inum == 0) {
        continue;
      }

      if (strncmp(name, entry.name, MAX_DIR_NAME_LENGTH) == 0) {
        if (poffset) {
          *poffset = offset;
        }
        inum = entry.inum;
        return get(device, inum);
      }
    }

    return nullptr;
  }

  int Inode::read(bool userDst,
                  uint64_t dst,
                  uint32_t offset,
                  uint32_t length) {
    if (offset > size || offset + length < offset) {
      return 0;
    }
    if (offset + length > size) {
      length = size - offset;
    }

    uint32_t tot, len;
    for (tot = 0; tot < length; tot += len, offset += len, dst += len) {
      uint32_t address = bmap(offset / BLOCK_SIZE);
      if (address == 0) {
        break;
      }

      auto bp = bufferCache->get(device, address);
      bp->read();
      len = std::min<uint64_t>(-tot, BLOCK_SIZE - offset % BLOCK_SIZE);
      if (!memory::eitherCopyout(
            userDst, dst, bp->data + (offset % BLOCK_SIZE), len)) {
        bufferCache->release(bp);
        return -1;
      }
      bufferCache->release(bp);
    }

    return static_cast<int>(tot);
  }

  void initializeInode() {
    itable = new Itable();
  }
} // namespace orfos::kernel::fs