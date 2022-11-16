#include "buffer.h"

#include <mutex/lock_guard.h>

#include <cassert>

#include "virtio.h"

namespace orfos::kernel::fs {
  BufferCache* bufferCache = nullptr;
  void initializeBufferCache() {
    bufferCache = new BufferCache();
  }
  Buffer* BufferCache::get(uint32_t device, uint32_t block) {
    mutex.lock();

    uint64_t key = (static_cast<uint64_t>(device) << 32) | block;

    Buffer* buffer;
    if (bufferTable.contains(key)) {
      buffer = bufferTable[key];
    } else {
      buffer           = new Buffer();
      buffer->device   = device;
      buffer->block    = block;
      buffer->valid    = false;
      bufferTable[key] = buffer;
    }
    ++buffer->refCount;

    mutex.unlock();
    buffer->mutex.lock();

    return buffer;
  }
  void BufferCache::release(Buffer* buffer) {
    assert(buffer->mutex.holding());

    buffer->mutex.unlock();

    mutex::LockGuard guard(mutex);
    --buffer->refCount;
    if (buffer->refCount == 0) {
      uint64_t key
        = (static_cast<uint64_t>(buffer->device) << 32) | buffer->block;
      bufferTable.erase(key);
      delete buffer;
    }
  }
  void BufferCache::pin(Buffer* buffer) {
    mutex::LockGuard guard(mutex);
    buffer->refCount++;
  }
  void BufferCache::unpin(Buffer* buffer) {
    assert(buffer->refCount >= 1);
    buffer->refCount--;
  }

  void Buffer::read() {
    if (!valid) {
      diskRead(this);
      valid = true;
    }
  }
  void Buffer::write() {
    assert(mutex.holding());
    diskWrite(this);
  }
} // namespace orfos::kernel::fs