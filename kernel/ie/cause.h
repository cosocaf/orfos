#ifndef ORFOS_KERNEL_IE_CAUSE_H_
#define ORFOS_KERNEL_IE_CAUSE_H_

#pragma once

#include <cstdint>

namespace orfos::kernel::ie {
  enum struct Cause {
    Unexpected,
    Plic,
    Timer,
  };

  Cause which(uint64_t scause);
}

#endif // ORFOS_KERNEL_IE_CAUSE_H_