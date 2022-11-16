#include "main.h"

#include <arch/riscv.h>
#include <console/console.h>
#include <fs/buffer.h>
#include <fs/inode.h>
#include <fs/virtio.h>
#include <ie/ie.h>
#include <ie/plic.h>
#include <memory/kvm.h>
#include <memory/page.h>
#include <process/process.h>
#include <process/scheduler.h>
#include <process/user_init_proc.h>
#include <syscall/syscall.h>

namespace orfos::kernel::boot {
  namespace {
    // hartid=0の初期化が完了したかどうかのフラグ
    volatile bool started = false;
  } // namespace
  [[noreturn]] void main() {
    auto hartid = arch::read_tp();
    if (hartid == 0) {
      console::initialize();
      console::printf("ORFOS is booting...\n");

      memory::Page::initialize();
      memory::initializeKvm();
      memory::initializeKvmHart();

      process::initialize();
      process::initializeScheduler();

      ie::initializeHart();
      ie::initializePlic();
      ie::initializePlicHart();

      fs::initializeBufferCache();
      fs::initializeInode();
      fs::initializeVirtio();

      syscall::initialize();

      process::startUserInitProcess();

      __sync_synchronize();
      started = true;
    } else {
      while (!started) {
        // Wait
      }
      __sync_synchronize();
      console::printf("Hart %d is starting...", hartid);
      memory::initializeKvmHart();
      ie::initializeHart();
      ie::initializePlicHart();
    }

    process::scheduler->run();
  }
} // namespace orfos::kernel::boot