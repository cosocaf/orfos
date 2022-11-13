#include "spin_mutex.h"

namespace orfos::kernel::mutex {
  void SpinMutex::lock() {}
  void SpinMutex::unlock() {}
  void SpinMutex::push() {}
  void SpinMutex::pop() {}
  bool SpinMutex::holding() {
    return false;
  }
} // namespace orfos::kernel::mutex