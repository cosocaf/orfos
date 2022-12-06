#ifndef ORFOS_KERNEL_DRIVER_INIT_H_
#define ORFOS_KERNEL_DRIVER_INIT_H_

#pragma once

#include <lib/fixed_string.h>
#include <lib/result.h>

#include "device_tree.h"

namespace orfos::kernel::driver {
  lib::Result<DeviceTree, lib::FixedString<>> initialize(void* dth);
}

#endif // ORFOS_KERNEL_DRIVER_INIT_H_