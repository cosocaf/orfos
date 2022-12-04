#include "stdlib.h"

#include "../panic.h"

extern "C" {
[[noreturn]] void abort() {
  panic("Aborted");
}
}