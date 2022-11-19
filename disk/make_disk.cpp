#include <kernel/fs/block.h>
#include <kernel/fs/inode.h>
#include <kernel/fs/log.h>
#include <kernel/fs/stat.h>

#include <cassert>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>

using namespace orfos::kernel;

static_assert(sizeof(int) == 4);
static_assert((fs::BLOCK_SIZE % sizeof(fs::DiskInode)) == 0);
static_assert((fs::BLOCK_SIZE % sizeof(fs::DirectoryEntry)) == 0);

std::fstream fsimg;
alignas(16) char zero[fs::BLOCK_SIZE];
fs::SuperBlock sb;
uint32_t freeInode = 1;
uint32_t freeBlock;

template <typename Int>
constexpr Int convOrder(Int n) {
  Int res;
  auto buf = reinterpret_cast<uint8_t *>(&res);
  for (size_t i = 0; i < sizeof(Int); ++i) {
    buf[i] = n >> (i * 8);
  }
  return res;
}

void readSector(uint32_t sector, void *buf) {
  fsimg.seekp(sector * fs::BLOCK_SIZE, std::ios::beg);
  fsimg.read(static_cast<char *>(buf), fs::BLOCK_SIZE);
}
void writeSector(uint32_t sector, void *buf) {
  fsimg.seekp(sector * fs::BLOCK_SIZE, std::ios::beg);
  fsimg.write(static_cast<char *>(buf), fs::BLOCK_SIZE);
}

void readInode(uint32_t inum, fs::DiskInode &ip) {
  char buf[fs::BLOCK_SIZE];
  auto block = fs::iblock(inum, sb);
  readSector(block, buf);
  auto dip
    = reinterpret_cast<fs::DiskInode *>(buf) + (inum % fs::INODES_PER_BLOCK);
  ip = *dip;
}
void writeInode(uint32_t inum, const fs::DiskInode &ip) {
  char buf[fs::BLOCK_SIZE];
  auto block = fs::iblock(inum, sb);
  readSector(block, buf);
  auto dip
    = reinterpret_cast<fs::DiskInode *>(buf) + (inum % fs::INODES_PER_BLOCK);
  *dip = ip;
  writeSector(block, buf);
}

uint32_t allocateInode(uint16_t type) {
  auto inum = freeInode++;
  fs::DiskInode dinode;

  bzero(&dinode, sizeof(dinode));
  dinode.type     = convOrder<int16_t>(type);
  dinode.numLinks = convOrder<int16_t>(1);
  dinode.size     = convOrder<uint32_t>(0);
  writeInode(inum, dinode);
  return inum;
}
void appendInode(uint32_t inum, void *xp, size_t n) {
  fs::DiskInode dinode;
  readInode(inum, dinode);
  auto offset = convOrder<uint32_t>(dinode.size);
  uint32_t indirect[fs::NUM_INDIRECT_BLOCKS];

  auto p = static_cast<char *>(xp);

  uint32_t x;
  while (n > 0) {
    auto fbn = offset / fs::BLOCK_SIZE;
    if (fbn < fs::NUM_DIRECT_BLOCKS) {
      if (convOrder<uint32_t>(dinode.directBlocks[fbn]) == 0) {
        dinode.directBlocks[fbn] = convOrder<uint32_t>(freeBlock++);
      }
      x = convOrder<uint32_t>(dinode.directBlocks[fbn]);
    } else {
      if (convOrder<uint32_t>(dinode.indirectBlock) == 0) {
        dinode.indirectBlock = convOrder<uint32_t>(freeBlock++);
      }
      readSector(convOrder<uint32_t>(dinode.indirectBlock), indirect);
      if (indirect[fbn - fs::NUM_DIRECT_BLOCKS] == 0) {
        indirect[fbn - fs::NUM_DIRECT_BLOCKS]
          = convOrder<uint32_t>(freeBlock++);
        writeSector(convOrder<uint32_t>(dinode.indirectBlock), indirect);
      }
      x = convOrder<uint32_t>(indirect[fbn - fs::NUM_DIRECT_BLOCKS]);
    }
    auto len = std::min<size_t>(n, (fbn + 1) * fs::BLOCK_SIZE - offset);
    char buf[fs::BLOCK_SIZE];
    readSector(x, buf);
    bcopy(p, buf + offset - (fbn * fs::BLOCK_SIZE), len);
    writeSector(x, buf);
    n -= len;
    offset += len;
    p += len;
  }

  dinode.size = convOrder<uint32_t>(offset);
  writeInode(inum, dinode);
}

void allocateBlock(uint32_t used) {
  assert(used < fs::BLOCK_SIZE * 8);
  char buf[fs::BLOCK_SIZE];
  bzero(buf, fs::BLOCK_SIZE);
  for (uint32_t i = 0; i < used; ++i) {
    buf[i / 8] |= (1 << (i % 8));
  }
  writeSector(sb.bmapStart, buf);
}

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " fs.img files..." << std::endl;
    return 1;
  }

  fsimg.open(argv[1],
             std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);
  if (!fsimg) {
    std::cerr << "Error: cannot open " << argv[1] << std::endl;
    return 2;
  }

  char buf[fs::BLOCK_SIZE];

  uint64_t numInodeBlocks = fs::NUM_INODES / fs::INODES_PER_BLOCK + 1;
  uint64_t numMeta        = 2;
  numMeta += fs::LOG_SIZE;
  numMeta += numInodeBlocks;
  numMeta += fs::FS_SIZE / (fs::BLOCK_SIZE * 8) + 1;

  uint32_t numBlocks = fs::FS_SIZE - numMeta;

  sb.magic      = fs::FS_MAGIC;
  sb.size       = convOrder<uint32_t>(fs::FS_SIZE);
  sb.numBlocks  = convOrder<uint32_t>(numBlocks);
  sb.numInodes  = convOrder<uint32_t>(fs::NUM_INODES);
  sb.numLogs    = convOrder<uint32_t>(fs::LOG_SIZE);
  sb.logStart   = convOrder<uint32_t>(2);
  sb.inodeStart = convOrder<uint32_t>(2 + fs::LOG_SIZE);
  sb.bmapStart  = convOrder<uint32_t>(2 + fs::LOG_SIZE + numInodeBlocks);

  freeBlock = numMeta;

  for (size_t i = 0; i < fs::FS_SIZE; i += fs::BLOCK_SIZE) {
    writeSector(i, zero);
  }

  memset(buf, 0, sizeof(buf));
  memmove(buf, &sb, sizeof(sb));
  writeSector(1, buf);

  auto root = allocateInode(fs::T_DIR);

  fs::DirectoryEntry entry;
  bzero(&entry, sizeof(entry));
  entry.inum = convOrder<uint16_t>(root);
  strcpy(entry.name, ".");
  appendInode(root, &entry, sizeof(entry));

  bzero(&entry, sizeof(entry));
  entry.inum = convOrder<uint16_t>(root);
  strcpy(entry.name, "..");
  appendInode(root, &entry, sizeof(entry));

  for (int i = 2; i < argc; ++i) {
    std::ifstream istream(argv[i], std::ios::binary);
    auto inum = allocateInode(fs::T_FILE);
    bzero(&entry, sizeof(entry));
    entry.inum = convOrder<uint16_t>(inum);

    std::filesystem::path path(argv[i]);

    strncpy(entry.name, path.filename().c_str(), fs::MAX_DIR_NAME_LENGTH);
    appendInode(root, &entry, sizeof(entry));

    while (!istream.eof()) {
      auto len = istream.readsome(buf, sizeof(buf));
      if (len == 0) break;
      appendInode(inum, buf, len);
    }
  }

  fs::DiskInode dinode;
  readInode(root, dinode);
  auto offset = convOrder<uint32_t>(dinode.size);
  offset      = (offset / fs::BLOCK_SIZE + 1) * fs::BLOCK_SIZE;
  dinode.size = convOrder<uint32_t>(offset);
  writeInode(root, dinode);

  allocateBlock(freeBlock);
}