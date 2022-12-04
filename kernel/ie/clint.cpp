#include "clint.h"

namespace orfos::kernel::ie {
  RegisterMsip msip;
  RegisterMtimeCmp mtimecmp;
  RegisterMtime mtime;

  void RegisterMsip::fire(uint64_t hartid) {
    uint64_t address = BASE + 4 * hartid;
    uint32_t* msip   = reinterpret_cast<uint32_t*>(address);
    *msip            = 1;
  }
} // namespace orfos::kernel::ie