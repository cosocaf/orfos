#include "device.h"

#include <console/console.h>
#include <memory/copy.h>

#include <algorithm>

namespace orfos::kernel::fs {
  namespace {
    Device deviceMap[NUM_DEVICES];
    int consoleRead(bool user, uint64_t dst, int n) {
      return console::readline(user, reinterpret_cast<char*>(dst), n);
    }
    int consoleWrite(bool user, uint64_t src, int n) {
      char buf[1024];
      int offset = 0;
      while (n > 0) {
        auto len = std::min<size_t>(n, sizeof(buf));
        if (!memory::eitherCopyin(buf, user, src + offset, len)) {
          break;
        }
        for (size_t i = 0; i < len; ++i) {
          console::putc(buf[i]);
        }
        offset += len;
        n -= len;
      }
      return offset;
    }
  } // namespace
  void initializeDevices() {
    deviceMap[CONSOLE].read  = consoleRead;
    deviceMap[CONSOLE].write = consoleWrite;
  }
  Device* getDevice(int id) {
    if (id < 0 || id >= NUM_DEVICES) {
      return nullptr;
    }
    return &deviceMap[id];
  }
} // namespace orfos::kernel::fs