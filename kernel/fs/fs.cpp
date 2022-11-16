#include "fs.h"

#include <cassert>

#include "log.h"
#include "super_block.h"

namespace orfos::kernel::fs {
  void initializeFs(uint32_t device) {
    superBlock.read(device);
    assert(superBlock.magic == FS_MAGIC);
    initializeLog(device, superBlock);
  }
} // namespace orfos::kernel::fs