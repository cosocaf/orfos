#ifndef ORFOS_KERNEL_LIB_LIBC_BITS_H_
#define ORFOS_KERNEL_LIB_LIBC_BITS_H_

#pragma once

extern "C" {
int __clzsi2(unsigned int a);
int __clzdi2(unsigned long a);
int __clzti2(unsigned long long a);

int __ctzsi2(unsigned int a);
int __ctzdi2(unsigned long a);
int __ctzti2(unsigned long long a);
}

#endif // ORFOS_KERNEL_LIB_LIBC_BITS_H_