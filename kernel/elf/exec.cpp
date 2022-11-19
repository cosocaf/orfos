#include "exec.h"

#include <fs/inode.h>
#include <fs/log.h>
#include <lib/string.h>
#include <memory/page_table.h>
#include <memory/uvm.h>
#include <process/cpu.h>
#include <process/process.h>
#include <process/scheduler.h>

#include <cassert>

#include "elf.h"

namespace orfos::kernel::elf {
  namespace {
    int flagsToPerm(int flags) {
      int perm = 0;
      if (flags & 0x1) perm = memory::PTE_X;
      if (flags & 0x2) perm |= memory::PTE_W;
      return perm;
    }
    bool loadSegment(memory::PageTable* pageTable,
                     uint64_t va,
                     fs::Inode* ip,
                     uint64_t offset,
                     uint64_t size) {
      for (uint64_t i = 0; i < size; i += PAGE_SIZE) {
        auto pa = pageTable->walkAddress(va + i);
        assert(pa != 0);
        uint64_t n;
        if (size - i < PAGE_SIZE) {
          n = size - i;
        } else {
          n = PAGE_SIZE;
        }
        uint64_t len = ip->read(false, pa, offset + i, n);
        if (len != n) {
          return false;
        }
      }
      return true;
    }
  } // namespace
  int exec(char* path, char** argv) {
    fs::beginOp();
    auto ip = fs::Inode::open(path);
    if (ip == nullptr) {
      fs::endOp();
      return -1;
    }
    ip->lock();

    ElfHeader elfHeader;
    ProgramHeader progHeader;
    auto proc                    = process::Cpu::current().process;
    memory::PageTable* pageTable = proc->makePageTable();
    uint64_t size                = 0;

    const auto bad = [&]() {
      delete pageTable;
      if (ip) {
        ip->unlock();
        ip->put();
        fs::endOp();
      }
      return -1;
    };

    auto len = ip->read(
      false, reinterpret_cast<uint64_t>(&elfHeader), 0, sizeof(elfHeader));
    if (len != sizeof(elfHeader)) {
      return bad();
    }
    if (elfHeader.magic != ELF_MAGIC) {
      return bad();
    }
    for (uint64_t i = 0, offset = elfHeader.phoff; i < elfHeader.phnum;
         ++i, offset += sizeof(progHeader)) {
      len = ip->read(false,
                     reinterpret_cast<uint64_t>(&progHeader),
                     offset,
                     sizeof(progHeader));
      if (len != sizeof(progHeader)) {
        return bad();
      }
      if (progHeader.type != ELF_PROG_LOAD) {
        continue;
      }
      if (progHeader.memsz < progHeader.filesz) {
        return bad();
      }
      if (progHeader.vaddr + progHeader.memsz < progHeader.vaddr) {
        return bad();
      }
      if (progHeader.vaddr % PAGE_SIZE != 0) {
        return bad();
      }

      size
        = memory::allocateUserVirtualMemory(pageTable,
                                            size,
                                            progHeader.vaddr + progHeader.memsz,
                                            flagsToPerm(progHeader.flags));
      if (size == 0) {
        return bad();
      }

      auto res = loadSegment(
        pageTable, progHeader.vaddr, ip, progHeader.off, progHeader.filesz);
      if (!res) {
        return bad();
      }
    }
    ip->unlock();
    ip->put();
    fs::endOp();
    ip = nullptr;

    size               = memory::VirtualAddress(size).roundUp().address;
    uint64_t allocSize = memory::allocateUserVirtualMemory(
      pageTable, size, size + PAGE_SIZE * 2, memory::PTE_W);
    if (allocSize == 0) {
      return bad();
    }

    size = allocSize;
    memory::clearUserVirtualMemory(pageTable, size - PAGE_SIZE * 2);

    auto sp        = size;
    auto stackBase = sp - PAGE_SIZE;
    uint64_t ustack[MAX_ARGS];
    uint64_t argc;

    for (argc = 0; argv[argc]; ++argc) {
      if (argc >= MAX_ARGS) {
        return bad();
      }
      sp -= strlen(argv[argc]) + 1;
      sp -= sp % 16;
      if (sp < stackBase) {
        return bad();
      }
      if (!pageTable->copyout(sp, argv[argc], strlen(argv[argc]) + 1)) {
        return bad();
      }
      ustack[argc] = sp;
    }
    ustack[argc] = 0;

    sp -= (argc + 1) * sizeof(uint64_t);
    sp -= sp % 16;
    if (sp < stackBase) {
      return bad();
    }
    if (!pageTable->copyout(
          sp, reinterpret_cast<char*>(ustack), (argc + 1) * sizeof(uint64_t))) {
      return bad();
    }

    proc->trapFrame->a1 = sp;

    char* last;
    char* s;
    for (last = s = path; *s; ++s) {
      if (*s == '/') {
        last = s + 1;
      }
    }
    strncpy(proc->name, last, sizeof(proc->name));

    auto oldPageTable    = proc->pageTable;
    proc->pageTable      = pageTable;
    proc->memorySize     = size;
    proc->trapFrame->epc = elfHeader.entry;
    proc->trapFrame->sp  = sp;

    delete oldPageTable;

    return argc;
  }
} // namespace orfos::kernel::elf