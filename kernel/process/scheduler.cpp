#include "scheduler.h"

#include <arch/riscv.h>
#include <arch/riscv_macros.h>
#include <console/console.h>
#include <mutex/lock_guard.h>

#include <cassert>

#include "cpu.h"

namespace orfos::kernel::process {
  Scheduler* scheduler;
  void initializeScheduler() {
    scheduler = new Scheduler();
  }

  void Scheduler::killZombie(Process* process) {
    assert(process->state == ProcessState::Zombie);
    delete process;
    allProc.erase(process);
  }

  const lib::HashSet<Process*>& Scheduler::allProcesses() const {
    return allProc;
  }

  void Scheduler::registerProcess(Process* process) {
    assert(process != nullptr);

    mutex::LockGuard guard(mutex);
    switch (process->state) {
      case ProcessState::Used:
      case ProcessState::Running:
        allProc.insert(process);
        break;
      case ProcessState::Ready:
        readyQueue.push(process);
        allProc.insert(process);
        break;
      case ProcessState::Sleeping:
        sleepTable[process->chan].push_back(process);
        allProc.insert(process);
        break;
      case ProcessState::Zombie:
        // Do nothing.
        break;
    }
  }
  void Scheduler::unregisterProcess(Process* process) {
    assert(process != nullptr);

    mutex::LockGuard guard(mutex);

    decltype(readyQueue) que;
    while (!readyQueue.empty()) {
      auto proc = readyQueue.front();
      readyQueue.pop();
      if (proc != process) {
        que.push(proc);
      }
    }
    readyQueue = que;
  }

  void Scheduler::reschedule() {
    auto& cpu = Cpu::current();
    auto proc = cpu.process;

    assert(proc->mutex.holding());
    assert(cpu.numLocks == 1);
    assert(proc->state != ProcessState::Running);
    assert((arch::read_sstatus() & SSTATUS_SIE) == 0);

    auto interruptEnabled = cpu.interruptEnabled;
    registerProcess(proc);
    switchContext(proc->context, cpu.context);
    cpu.interruptEnabled = interruptEnabled;
  }
  void Scheduler::wakeup(void* chan) {
    mutex::LockGuard guard(mutex);
    for (auto proc : sleepTable[chan]) {
      mutex::LockGuard procGuard(proc->mutex);
      proc->state = ProcessState::Ready;
      mutex.unlock();
      registerProcess(proc);
      mutex.lock();
    }
    sleepTable.erase(chan);
  }
  [[noreturn]] void Scheduler::run() {
    while (true) {
      // 割り込み有効化
      auto sstatus = arch::read_sstatus();
      arch::write_sstatus(sstatus | SSTATUS_SIE);

      while (true) {
        Process* proc;
        {
          mutex::LockGuard guard(mutex);
          if (readyQueue.empty()) {
            break;
          }
          proc = readyQueue.front();
          readyQueue.pop();
        }

        if (proc->state != ProcessState::Ready) {
          if (proc->state == ProcessState::Zombie) {
            killZombie(proc);
          } else {
            registerProcess(proc);
          }
          break;
        }

        {
          mutex::LockGuard guard(proc->mutex);
          proc->state = ProcessState::Running;
          auto& cpu   = Cpu::current();
          cpu.process = proc;
          switchContext(cpu.context, proc->context);
          cpu.process = nullptr;
        }

        registerProcess(proc);
      }
    }
  }
} // namespace orfos::kernel::process