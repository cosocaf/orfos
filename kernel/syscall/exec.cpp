#include "exec.h"

#include <arch/config.h>
#include <elf/exec.h>
#include <fs/file.h>
#include <lib/string.h>

#include "args.h"

namespace orfos::kernel::syscall {
  uint64_t exec() {
    char path[fs::MAX_PATH_LENGTH];
    char* argv[elf::MAX_ARGS];

    if (getArgString(0, path, sizeof(path)) < 0) {
      return -1;
    }

    uint64_t uargv, uarg;
    getArgAddress(1, &uargv);

    memset(argv, 0, sizeof(argv));

    const auto free = [&]() {
      for (uint64_t i = 0; i < elf::MAX_ARGS && argv[i] != 0; ++i) {
        delete argv[i];
      }
    };
    for (uint64_t i = 0;; ++i) {
      if (i >= elf::MAX_ARGS) {
        free();
        return -1;
      }
      if (!fetchAddress(uargv + sizeof(uint64_t) * i, &uarg)) {
        free();
        return -1;
      }
      if (uarg == 0) {
        argv[i] = nullptr;
        break;
      }
      argv[i] = new char[PAGE_SIZE];
      if (argv[i] == nullptr) {
        free();
        return -1;
      }
      if (fetchString(uarg, argv[i], PAGE_SIZE) < 0) {
        free();
        return -1;
      }
    }

    auto result = elf::exec(path, argv);
    free();
    return result;
  }
} // namespace orfos::kernel::syscall