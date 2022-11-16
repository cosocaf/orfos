#include "sleep_mutex.h"

#include <process/cpu.h>
#include <process/sleep.h>

#include "lock_guard.h"

namespace orfos::kernel::mutex {
  void SleepMutex::lock() {
    LockGuard guard(mutex);
    while (locked) {
      process::sleep(this, this->mutex);
    }
    locked = true;
    pid    = process::Cpu::current().process->pid;
  }
  void SleepMutex::unlock() {
    LockGuard guard(mutex);
    locked = false;
    pid    = 0;
  }
  bool SleepMutex::holding() {
    LockGuard guard(mutex);
    return locked && pid == process::Cpu::current().process->pid;
  }
} // namespace orfos::kernel::mutex