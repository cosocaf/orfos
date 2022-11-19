#ifndef ORFOS_KERNEL_PROCESS_PROCESS_H_
#define ORFOS_KERNEL_PROCESS_PROCESS_H_

#pragma once

#include <fs/file.h>
#include <fs/inode.h>
#include <memory/page_table.h>
#include <mutex/spin_mutex.h>

#include <cstdint>
#include <vector>

#include "context.h"
#include "trap_frame.h"

namespace orfos::kernel::process {
  enum struct ProcessState {
    Used,
    Sleeping,
    Ready,
    Running,
    Zombie,
  };
  struct Process {
    uint64_t pid;
    Process* parent;
    ProcessState state;
    mutex::SpinMutex mutex;
    Context context;
    TrapFrame* trapFrame;
    memory::PageTable* pageTable;
    char* kernelStackPage;
    uint64_t kernelStack;
    uint64_t memorySize;
    void* chan;
    fs::Inode* cwd;
    std::vector<fs::File*> openFiles;
    char name[16];
    bool killed;
    int exitStatus;

    Process();
    ~Process();

    memory::PageTable* makePageTable();

    int open(const char* path, int mode);
    int close(int fd);

    Process* fork();
    void exit(int status);

  private:
    void reparent();
  };

  void initialize();
} // namespace orfos::kernel::process

#endif // ORFOS_KERNEL_PROCESS_PROCESS_H_