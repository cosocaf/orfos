#ifndef ORFOS_KERNEL_LIB_LIBC_STRING_H_
#define ORFOS_KERNEL_LIB_LIBC_STRING_H_

#include <cstddef>

extern "C" {
void* memcpy(void* dst, void* src, size_t n);
size_t strlen(const char* str);
}

#endif // ORFOS_KERNEL_LIB_LIBC_STRING_H_