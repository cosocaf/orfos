#include "virtio.h"

#include <mutex/lock_guard.h>
#include <process/sleep.h>

#include <cassert>

#include "block.h"

namespace orfos::kernel::fs {
  namespace {
    uint32_t* reg(uint64_t r) {
      return reinterpret_cast<uint32_t*>(VIRTIO_BASE + r);
    }

    bool findVirtioDisk() {
      if (*reg(VIRTIO_MMIO_MAGIC_VALUE) != 0x74726976) {
        return false;
      }
      if (*reg(VIRTIO_MMIO_VERSION) != 2) {
        return false;
      }
      if (*reg(VIRTIO_MMIO_DEVICE_ID) != 2) {
        return false;
      }
      if (*reg(VIRTIO_MMIO_VENDOR_ID) != 0x554d4551) {
        return false;
      }
      return true;
    }

    template <typename T>
    uint32_t low(T address) {
      return static_cast<uint32_t>(reinterpret_cast<uint64_t>(address));
    }
    template <typename T>
    uint32_t high(T address) {
      return static_cast<uint32_t>(reinterpret_cast<uint64_t>(address) >> 32);
    }

    struct Descriptor {
      uint64_t address;
      uint32_t length;
      uint16_t flags;
      uint16_t next;
    };
    struct Avail {
      uint16_t flags;
      uint16_t index;
      uint16_t ring[NUM_DESCS];
      uint16_t unused;
    };
    struct UsedElement {
      uint32_t id;
      uint32_t length;
    };
    struct Used {
      uint16_t flags;
      uint16_t index;
      UsedElement ring[NUM_DESCS];
    };
    struct BlockRequest {
      uint32_t type;
      uint32_t reserved;
      uint64_t sector;
    };
    struct Disk {
      Descriptor* descriptor;
      Avail* avail;
      Used* used;
      char free[NUM_DESCS];
      uint16_t usedIndex;
      struct {
        Buffer* buffer;
        char status;
      } info[NUM_DESCS];
      BlockRequest ops[NUM_DESCS];
      mutex::SpinMutex mutex;
    };

    Disk disk;

    int allocateDescriptor() {
      for (uint64_t i = 0; i < NUM_DESCS; i++) {
        if (disk.free[i]) {
          disk.free[i] = 0;
          return i;
        }
      }
      return -1;
    }
    void freeDescriptor(int i) {
      assert(static_cast<uint64_t>(i) < NUM_DESCS);
      assert(!disk.free[i]);
      disk.descriptor[i].address = 0;
      disk.descriptor[i].length  = 0;
      disk.descriptor[i].flags   = 0;
      disk.descriptor[i].next    = 0;
      disk.free[i]               = 1;
      process::wakeup(disk.free);
    }
    void freeChain(int i) {
      while (true) {
        auto flag = disk.descriptor[i].flags;
        auto next = disk.descriptor[i].next;
        freeDescriptor(i);
        if (flag & VRING_DESC_F_NEXT) {
          i = next;
        } else {
          break;
        }
      }
    }

    void diskrw(Buffer* buf, bool write) {
      uint64_t sector = buf->block * (BLOCK_SIZE / 512);

      mutex::LockGuard guard(disk.mutex);

      int index[3];
      while (true) {
        bool ok = true;
        for (int i = 0; i < 3; ++i) {
          index[i] = allocateDescriptor();
          if (index[i] < 0) {
            for (int j = 0; j < i; ++j) {
              freeDescriptor(index[j]);
              ok = false;
            }
            break;
          }
        }
        if (ok) {
          break;
        }
        process::sleep(disk.free, disk.mutex);
      }

      auto req = &disk.ops[index[0]];
      if (write) {
        req->type = VIRTIO_BLK_T_OUT;
      } else {
        req->type = VIRTIO_BLK_T_IN;
      }
      req->reserved = 0;
      req->sector   = sector;

      disk.descriptor[index[0]].address = reinterpret_cast<uint64_t>(req);
      disk.descriptor[index[0]].length  = sizeof(*req);
      disk.descriptor[index[0]].flags   = VRING_DESC_F_NEXT;
      disk.descriptor[index[0]].next    = index[1];

      disk.descriptor[index[1]].address = reinterpret_cast<uint64_t>(buf->data);
      disk.descriptor[index[1]].length  = BLOCK_SIZE;
      if (write) {
        disk.descriptor[index[1]].flags = 0;
      } else {
        disk.descriptor[index[1]].flags = VRING_DESC_F_WRITE;
      }
      disk.descriptor[index[1]].flags |= VRING_DESC_F_NEXT;
      disk.descriptor[index[1]].next = index[2];

      disk.info[index[0]].status = 0xff;
      disk.descriptor[index[2]].address
        = reinterpret_cast<uint64_t>(&disk.info[index[0]].status);
      disk.descriptor[index[2]].length = 1;
      disk.descriptor[index[2]].flags  = VRING_DESC_F_WRITE;
      disk.descriptor[index[2]].next   = 0;

      buf->disk                                       = true;
      disk.info[index[0]].buffer                      = buf;
      disk.avail->ring[disk.avail->index % NUM_DESCS] = index[0];

      __sync_synchronize();

      ++disk.avail->index;

      __sync_synchronize();

      *reg(VIRTIO_MMIO_QUEUE_NOTIFY) = 0;

      while (buf->disk) {
        process::sleep(buf, disk.mutex);
      }

      disk.info[index[0]].buffer = nullptr;
      freeChain(index[0]);
    }
  } // namespace
  void initializeVirtio() {
    assert(findVirtioDisk());

    uint32_t status          = 0;
    *reg(VIRTIO_MMIO_STATUS) = status;

    status |= VIRTIO_CONFIG_S_ACKNOWLEDGE;
    *reg(VIRTIO_MMIO_STATUS) = status;

    status |= VIRTIO_CONFIG_S_DRIVER;
    *reg(VIRTIO_MMIO_STATUS) = status;

    auto features = *reg(VIRTIO_MMIO_DEVICE_FEATURES);
    features &= ~(1 << VIRTIO_BLK_F_RO);
    features &= ~(1 << VIRTIO_BLK_F_SCSI);
    features &= ~(1 << VIRTIO_BLK_F_CONFIG_WCE);
    features &= ~(1 << VIRTIO_BLK_F_MQ);
    features &= ~(1 << VIRTIO_F_ANY_LAYOUT);
    features &= ~(1 << VIRTIO_RING_F_EVENT_IDX);
    features &= ~(1 << VIRTIO_RING_F_INDIRECT_DESC);
    *reg(VIRTIO_MMIO_DRIVER_FEATURES) = features;

    status |= VIRTIO_CONFIG_S_FEATURES_OK;
    *reg(VIRTIO_MMIO_STATUS) = status;

    status = *reg(VIRTIO_MMIO_STATUS);
    assert(status & VIRTIO_CONFIG_S_FEATURES_OK);

    *reg(VIRTIO_MMIO_QUEUE_SEL) = 0;
    assert(!*reg(VIRTIO_MMIO_QUEUE_READY));

    auto max = *reg(VIRTIO_MMIO_QUEUE_NUM_MAX);
    assert(max != 0);
    assert(max >= NUM_DESCS);

    disk.descriptor = new Descriptor();
    disk.avail      = new Avail();
    disk.used       = new Used();

    *reg(VIRTIO_MMIO_QUEUE_NUM) = NUM_DESCS;

    *reg(VIRTIO_MMIO_QUEUE_DESC_LOW)   = low(disk.descriptor);
    *reg(VIRTIO_MMIO_QUEUE_DESC_HIGH)  = high(disk.descriptor);
    *reg(VIRTIO_MMIO_DRIVER_DESC_LOW)  = low(disk.avail);
    *reg(VIRTIO_MMIO_DRIVER_DESC_HIGH) = high(disk.avail);
    *reg(VIRTIO_MMIO_DEVICE_DESC_LOW)  = low(disk.used);
    *reg(VIRTIO_MMIO_DEVICE_DESC_HIGH) = high(disk.used);

    *reg(VIRTIO_MMIO_QUEUE_READY) = 0x1;

    for (auto& free : disk.free) {
      free = 1;
    }

    status |= VIRTIO_CONFIG_S_DRIVER_OK;
    *reg(VIRTIO_MMIO_STATUS) = status;
  }

  void diskWrite(Buffer* buf) {
    diskrw(buf, true);
  }
  void diskRead(Buffer* buf) {
    diskrw(buf, false);
  }
  void diskInterrupt() {
    mutex::LockGuard guard(disk.mutex);
    *reg(VIRTIO_MMIO_INTERRUPT_ACK) = *reg(VIRTIO_MMIO_INTERRUPT_STATUS) & 0b11;

    __sync_synchronize();

    while (disk.usedIndex != disk.used->index) {
      __sync_synchronize();
      auto id = disk.used->ring[disk.usedIndex % NUM_DESCS].id;

      assert(disk.info[id].status == 0);

      auto buf  = disk.info[id].buffer;
      buf->disk = false;
      process::wakeup(buf);

      ++disk.usedIndex;
    }
  }
} // namespace orfos::kernel::fs