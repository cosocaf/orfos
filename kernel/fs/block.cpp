#include "block.h"

#include <lib/string.h>

#include <cassert>

#include "buffer.h"
#include "log.h"
#include "super_block.h"

namespace orfos::kernel::fs {
  namespace {
    inline uint32_t bblock(uint32_t block, const SuperBlock& sb) {
      return block / BITS_PER_BLOCK + sb.bmapStart;
    }
  } // namespace
  uint32_t allocateBlock(uint32_t device) {
    for (uint32_t block = 0; block < superBlock.size; block += BITS_PER_BLOCK) {
      auto bp = bufferCache->get(device, bblock(block, superBlock));
      bp->read();
      for (uint32_t bit = 0;
           bit < BITS_PER_BLOCK && block + bit < superBlock.size;
           ++bit) {
        uint32_t mask = 1 << (bit % 8);
        if ((bp->data[bit / 8] & mask) == 0) {
          bp->data[bit / 8] |= mask;
          writeLog(bp);
          bufferCache->release(bp);
          fillBlock(device, block + bit);
          return block + bit;
        }
      }
      bufferCache->release(bp);
    }
    return 0;
  }
  void freeBlock(uint32_t device, uint32_t block) {
    auto bp = bufferCache->get(device, bblock(block, superBlock));
    bp->read();
    uint32_t bit  = block % BITS_PER_BLOCK;
    uint32_t mask = 1 << (bit % 8);
    assert(bp->data[bit / 8] & mask);
    bp->data[bit / 8] &= ~mask;
    writeLog(bp);
    bufferCache->release(bp);
  }
  void fillBlock(uint32_t device, uint32_t block, int ch) {
    auto bp = bufferCache->get(device, block);
    bp->read();

    memset(bp->data, ch, BLOCK_SIZE);
    writeLog(bp);
    bufferCache->release(bp);
  }
} // namespace orfos::kernel::fs