#include "bits.h"

extern "C" {
int __clzdi2(unsigned long a) {
  int cnt = 0;
  for (int i = static_cast<int>(sizeof(a) * 8 - 1); i >= 0; --i) {
    if (a & (1 << i)) {
      break;
    }
    ++cnt;
  }
  return cnt;
}
}