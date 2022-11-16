#ifndef ORFOS_KERNEL_FS_FS_H_
#define ORFOS_KERNEL_FS_FS_H_

#pragma once

#include <cstdint>

namespace orfos::kernel::fs {
  void initializeFs(uint32_t device);
}

#endif // ORFOS_KERNEL_FS_FS_H_