#ifndef ORFOS_USER_LIB_STDIO_H_
#define ORFOS_USER_LIB_STDIO_H_

#pragma once

#include <cstdarg>

void putc(int fd, char c);
void printf(const char* fmt, ...);
void fprintf(int fd, const char* fmt, ...);
void vprintf(int fd, const char* fmt, va_list ap);

#endif // ORFOS_USER_LIB_STDIO_H_