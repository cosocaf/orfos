#ifndef ORFOS_KERNEL_LIB_STRING_H_
#define ORFOS_KERNEL_LIB_STRING_H_

#pragma once

#include <cstddef>

extern "C" {
void *memcpy(void *dest, const void *src, size_t n);
void *memmove(void *dest, const void *src, size_t n);
void *memset(void *dest, int ch, size_t n);
int strncmp(const char *s1, const char *s2, size_t n);
char *strncpy(char *s, const char *t, int n);
size_t strlen(const char *s);
}

#endif // ORFOS_KERNEL_LIB_STRING_H_