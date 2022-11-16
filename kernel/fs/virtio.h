#ifndef ORFOS_KERNEL_FS_VIRTIO_H_
#define ORFOS_KERNEL_FS_VIRTIO_H_

#pragma once

#include <cstdint>
#include "buffer.h"

namespace orfos::kernel::fs {
  constexpr uint64_t VIRTIO_BASE = 0x1000'1000;
  constexpr uint64_t VIRTIO_MMIO_MAGIC_VALUE      = 0x000;
  constexpr uint64_t VIRTIO_MMIO_VERSION          = 0x004;
  constexpr uint64_t VIRTIO_MMIO_DEVICE_ID        = 0x008;
  constexpr uint64_t VIRTIO_MMIO_VENDOR_ID        = 0x00c;
  constexpr uint64_t VIRTIO_MMIO_DEVICE_FEATURES  = 0x010;
  constexpr uint64_t VIRTIO_MMIO_DRIVER_FEATURES  = 0x020;
  constexpr uint64_t VIRTIO_MMIO_QUEUE_SEL        = 0x030;
  constexpr uint64_t VIRTIO_MMIO_QUEUE_NUM_MAX    = 0x034;
  constexpr uint64_t VIRTIO_MMIO_QUEUE_NUM        = 0x038;
  constexpr uint64_t VIRTIO_MMIO_QUEUE_READY      = 0x044;
  constexpr uint64_t VIRTIO_MMIO_QUEUE_NOTIFY     = 0x050;
  constexpr uint64_t VIRTIO_MMIO_INTERRUPT_STATUS = 0x060;
  constexpr uint64_t VIRTIO_MMIO_INTERRUPT_ACK    = 0x064;
  constexpr uint64_t VIRTIO_MMIO_STATUS           = 0x070;
  constexpr uint64_t VIRTIO_MMIO_QUEUE_DESC_LOW   = 0x080;
  constexpr uint64_t VIRTIO_MMIO_QUEUE_DESC_HIGH  = 0x084;
  constexpr uint64_t VIRTIO_MMIO_DRIVER_DESC_LOW  = 0x090;
  constexpr uint64_t VIRTIO_MMIO_DRIVER_DESC_HIGH = 0x094;
  constexpr uint64_t VIRTIO_MMIO_DEVICE_DESC_LOW  = 0x0a0;
  constexpr uint64_t VIRTIO_MMIO_DEVICE_DESC_HIGH = 0x0a4;
  constexpr uint64_t VIRTIO_CONFIG_S_ACKNOWLEDGE  = 1;
  constexpr uint64_t VIRTIO_CONFIG_S_DRIVER       = 2;
  constexpr uint64_t VIRTIO_CONFIG_S_DRIVER_OK    = 4;
  constexpr uint64_t VIRTIO_CONFIG_S_FEATURES_OK  = 8;
  constexpr uint64_t VIRTIO_BLK_F_RO              = 5;
  constexpr uint64_t VIRTIO_BLK_F_SCSI            = 7;
  constexpr uint64_t VIRTIO_BLK_F_CONFIG_WCE      = 11;
  constexpr uint64_t VIRTIO_BLK_F_MQ              = 12;
  constexpr uint64_t VIRTIO_F_ANY_LAYOUT          = 27;
  constexpr uint64_t VIRTIO_RING_F_INDIRECT_DESC  = 28;
  constexpr uint64_t VIRTIO_RING_F_EVENT_IDX      = 29;
  constexpr uint64_t NUM_DESCS                    = 8;
  constexpr uint64_t VRING_DESC_F_NEXT            = 1;
  constexpr uint64_t VRING_DESC_F_WRITE           = 2;
  constexpr uint64_t VIRTIO_BLK_T_IN              = 0;
  constexpr uint64_t VIRTIO_BLK_T_OUT             = 1;

  void initializeVirtio();
  void diskWrite(Buffer* buf);
  void diskRead(Buffer* buf);
  void diskInterrupt();
} // namespace orfos::kernel::fs

#endif // ORFOS_KERNEL_FS_VIRTIO_H_