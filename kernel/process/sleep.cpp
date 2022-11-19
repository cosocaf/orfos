#include "sleep.h"

#include <mutex/lock_guard.h>

#include "cpu.h"
#include "scheduler.h"

namespace orfos::kernel::process {
  void yield() {
    auto proc = Cpu::current().process;
    if (!proc) {
      return;
    }
    mutex::LockGuard guard(proc->mutex);
    scheduler->unregisterProcess(proc);
    proc->state = ProcessState::Ready;
    scheduler->reschedule();
  }
  void sleep(void* chan, mutex::SpinMutex& mutex) {
    auto proc = Cpu::current().process;
    proc->mutex.lock();
    mutex.unlock();

    proc->chan  = chan;
    proc->state = ProcessState::Sleeping;

    scheduler->reschedule();

    proc->chan = nullptr;

    proc->mutex.unlock();
    mutex.lock();
  }
  void wakeup(void* chan) {
    scheduler->wakeup(chan);
  }
} // namespace orfos::kernel::process