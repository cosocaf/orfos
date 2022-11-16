#include "kvm.h"

#include <arch/config.h>
#include <arch/riscv.h>
#include <arch/riscv_macros.h>
#include <console/console.h>
#include <console/uart.h>
#include <fs/virtio.h>
#include <ie/user_vector.h>

#include <new>

#include "page_table.h"

extern "C" {
extern char endOfText[];
}

namespace orfos::kernel::memory {
  PageTable* kernelPageTable = nullptr;
  void initializeKvm() {
    kernelPageTable = Page::allocate()->as<PageTable*>();
    new (kernelPageTable) PageTable();

    auto eot = reinterpret_cast<uint64_t>(endOfText);

    kernelPageTable->map(
      console::UART_BASE, PAGE_SIZE, console::UART_BASE, PTE_R | PTE_W);

    kernelPageTable->map(
      fs::VIRTIO_BASE, PAGE_SIZE, fs::VIRTIO_BASE, PTE_R | PTE_W);

    kernelPageTable->map(PLIC, 0x400000, PLIC, PTE_R | PTE_W);

    kernelPageTable->map(
      KERNEL_BASE, eot - KERNEL_BASE, KERNEL_BASE, PTE_R | PTE_X);

    kernelPageTable->map(
      eot, Page::physicalAddressStop - eot, eot, PTE_R | PTE_W);

    kernelPageTable->map(TRAMPOLINE,
                         PAGE_SIZE,
                         reinterpret_cast<uint64_t>(orfos_trampoline),
                         PTE_R | PTE_X);
  }
  void initializeKvmHart() {
    arch::sfence_vma();
    uint64_t satp = kernelPageTable->makeSatp();
    arch::write_satp(satp);
    arch::sfence_vma();
  }
} // namespace orfos::kernel::memory