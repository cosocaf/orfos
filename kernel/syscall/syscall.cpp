#include "syscall.h"

#include <console/console.h>
#include <lib/hash_map.h>
#include <process/cpu.h>

#include "close.h"
#include "exec.h"
#include "exit.h"
#include "fork.h"
#include "mknod.h"
#include "open.h"
#include "read.h"
#include "write.h"

namespace orfos::kernel::syscall {
  namespace {
    using SyscallFunc = uint64_t (*)();
    lib::HashMap<uint64_t, SyscallFunc>* syscallMap;
  } // namespace
  void syscall() {
    auto proc = process::Cpu::current().process;
    if (!proc) return;

    auto code = proc->trapFrame->a7;
    if (syscallMap->contains(code)) {
      auto& fn            = (*syscallMap)[code];
      proc->trapFrame->a0 = fn();
    } else {
      console::printf(
        "pid=%d %s: Unknown system call %d\n", proc->pid, proc->name, code);
      proc->trapFrame->a0 = -1;
    }
  }
  void initialize() {
    syscallMap           = new lib::HashMap<uint64_t, SyscallFunc>();
    (*syscallMap)[FORK]  = fork;
    (*syscallMap)[EXIT]  = exit;
    (*syscallMap)[READ]  = read;
    (*syscallMap)[EXEC]  = exec;
    (*syscallMap)[OPEN]  = open;
    (*syscallMap)[WRITE] = write;
    (*syscallMap)[MKNOD] = mknod;
    (*syscallMap)[CLOSE] = close;
  }
} // namespace orfos::kernel::syscall