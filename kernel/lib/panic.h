#ifndef ORFOS_KERNEL_LIB_PANIC_H_
#define ORFOS_KERNEL_LIB_PANIC_H_

namespace orfos::kernel::lib {
  [[noreturn]] void kernelPanic();
} // namespace orfos::kernel::lib

#define panic(fmt, ...) orfos::kernel::lib::kernelPanic()

#define kernelAssert(expr) ((expr) ? (void)0 : panic(#expr))

#endif // ORFOS_KERNEL_LIB_PANIC_H_