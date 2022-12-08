#ifndef ORFOS_KERNEL_LIB_LIBC_STRING_H_
#define ORFOS_KERNEL_LIB_LIBC_STRING_H_

#include <cstddef>

extern "C" {
void* memset(void* dst, int ch, size_t n);
void* memcpy(void* dst, void* src, size_t n);
int memcmp(const void* s, const void* t, size_t n);
size_t strlen(const char* str);
int strcmp(const char* s, const char* t);
}

#endif // ORFOS_KERNEL_LIB_LIBC_STRING_H_