#include "super_block.h"

#include <cstring>

#include "buffer.h"

namespace orfos::kernel::fs {
  SuperBlock superBlock;
  void SuperBlock::read(uint32_t device) {
    auto buf = bufferCache->get(device, 1);
    buf->read();
    memmove(this, buf->data, sizeof(*this));
    bufferCache->release(buf);
  }
} // namespace orfos::kernel::fs