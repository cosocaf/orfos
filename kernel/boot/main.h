#ifndef ORFOS_KERNEL_BOOT_MAIN_H_
#define ORFOS_KERNEL_BOOT_MAIN_H_

#pragma once

namespace orfos::kernel::boot {
  [[noreturn]] void primaryMain();
  [[noreturn]] void secondaryMain();
} // namespace orfos::kernel::boot

#endif // ORFOS_KERNEL_BOOT_MAIN_H_