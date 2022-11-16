#include <console/console.h>

#include "panic.h"

namespace std {
  void __throw_length_error(char const* msg) {
    orfos::kernel::console::printf("length_error\n");
    orfos::kernel::lib::panic(msg);
  }
  void __throw_bad_alloc() {
    orfos::kernel::lib::panic("bad_alloc");
  }
  void __throw_bad_array_new_length() {
    orfos::kernel::lib::panic("bad_array_new_length");
  }
} // namespace std