#include "stdlib.h"

#include <charconv>

#include "../panic.h"
#include "string.h"

extern "C" {
int atoi(const char* str) {
  int result;
  if (auto [ptr, ec] = std::from_chars(str, str + strlen(str), result); ec == std::errc{}) {
    return result;
  } else {
    panic("atoi failed. str: %s", str);
    return 0;
  }
}

[[noreturn]] void abort() {
  panic("Aborted");
}
}