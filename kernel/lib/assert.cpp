#include <console/console.h>

#include "panic.h"
extern "C" {
[[noreturn]] void __assert_func(char* file, int line, char* fn, char* msg) {
  using namespace orfos::kernel;
  console::printf("Assertion failed: %s\n", msg);
  console::printf("\tat %s %s:%d\n", fn, file, line);
  lib::panic("assert");
}
}