#ifndef ORFOS_KERNEL_IE_USER_VECTOR_H_
#define ORFOS_KERNEL_IE_USER_VECTOR_H_

#pragma once

extern "C" {
extern char orfos_trampoline[];
void orfos_userVector();
void orfos_returnUser();
}

#endif // ORFOS_KERNEL_IE_USER_VECTOR_H_