#include "context.h"

#include <console/console.h>

extern "C" {
void orfos_switchContext(const orfos::kernel::process::Context*,
                         const orfos::kernel::process::Context*);
}

namespace orfos::kernel::process {
  void switchContext(const Context& oldContext, const Context& newContext) {
    console::printf("switch %p, %p\n", &oldContext, &newContext);
    orfos_switchContext(&oldContext, &newContext);
  }
} // namespace orfos::kernel::process