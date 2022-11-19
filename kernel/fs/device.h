#ifndef ORFOS_KERNEL_FS_DEVICE_H_
#define ORFOS_KERNEL_FS_DEVICE_H_

#pragma once

#include <cstdint>

namespace orfos::kernel::fs {
  constexpr uint16_t NUM_DEVICES = 10;
  constexpr uint32_t CONSOLE       = 1;

  struct Device {
    int (*read)(bool userDst, uint64_t address, int n);
    int (*write)(bool userSrc, uint64_t address, int n);
  };

  void initializeDevices();
  Device* getDevice(int id);
} // namespace orfos::kernel::fs

#endif // ORFOS_KERNEL_FS_DEVICE_H_