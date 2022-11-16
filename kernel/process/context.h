#ifndef ORFOS_KERNEL_PROCESS_CONTEXT_H_
#define ORFOS_KERNEL_PROCESS_CONTEXT_H_

#pragma once

#include <cstdint>

namespace orfos::kernel::process {
  struct Context {
    uint64_t ra;
    uint64_t sp;
    uint64_t s0;
    uint64_t s1;
    uint64_t s2;
    uint64_t s3;
    uint64_t s4;
    uint64_t s5;
    uint64_t s6;
    uint64_t s7;
    uint64_t s8;
    uint64_t s9;
    uint64_t s10;
    uint64_t s11;
  };
  void switchContext(const Context& oldContext, const Context& newContext);
}

#endif // ORFOS_KERNEL_PROCESS_CONTEXT_H_