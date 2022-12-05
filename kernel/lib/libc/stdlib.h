#ifndef ORFOS_KERNEL_LIB_LIBC_STDLIB_H_
#define ORFOS_KERNEL_LIB_LIBC_STDLIB_H_

#pragma once

extern "C" {
int atoi(const char* str);
[[noreturn]] void abort();
}

#endif // ORFOS_KERNEL_LIB_LIBC_STDLIB_H_