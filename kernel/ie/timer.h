#ifndef ORFOS_KERNEL_IE_TIMER_H_
#define ORFOS_KERNEL_IE_TIMER_H_

#pragma once

namespace orfos::kernel::ie {
  void initializeTimer();
  void clockInterrupt();
}

#endif // ORFOS_KERNEL_IE_TIMER_H_