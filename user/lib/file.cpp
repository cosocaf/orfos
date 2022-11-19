#include "file.h"

#include <kernel/fs/device.h>
#include <kernel/fs/file.h>

const int O_RDONLY         = orfos::kernel::fs::O_RDONLY;
const int O_WRONLY         = orfos::kernel::fs::O_WRONLY;
const int O_RDWR           = orfos::kernel::fs::O_RDWR;
const int O_CREATE         = orfos::kernel::fs::O_CREATE;
const int O_TRUNC          = orfos::kernel::fs::O_TRUNC;
const uint32_t DEV_CONSOLE = orfos::kernel::fs::CONSOLE;
