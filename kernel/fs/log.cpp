#include "log.h"

#include <lib/string.h>
#include <mutex/lock_guard.h>
#include <process/sleep.h>

#include <cassert>

#include "buffer.h"

namespace orfos::kernel::fs {
  namespace {
    Log log;

    void readHead() {
      auto buf = bufferCache->get(log.device, log.start);
      buf->read();
      auto header  = reinterpret_cast<LogHeader*>(buf->data);
      log.header.n = header->n;
      for (uint64_t i = 0; i < log.header.n; i++) {
        log.header.block[i] = header->block[i];
      }
      bufferCache->release(buf);
    }
    void writeHead() {
      auto buf = bufferCache->get(log.device, log.start);
      buf->read();
      auto header = reinterpret_cast<LogHeader*>(buf->data);
      header->n   = log.header.n;
      for (uint64_t i = 0; i < log.header.n; ++i) {
        header->block[i] = log.header.block[i];
      }
      buf->write();
      bufferCache->release(buf);
    }
    void installTransaction(bool recover) {
      for (uint64_t tail = 0; tail < log.header.n; ++tail) {
        auto lbuf = bufferCache->get(log.device, log.start + tail + 1);
        auto dbuf = bufferCache->get(log.device, log.header.block[tail]);
        lbuf->read();
        dbuf->read();

        memmove(dbuf->data, lbuf->data, BLOCK_SIZE);
        dbuf->write();
        if (!recover) {
          bufferCache->unpin(dbuf);
        }
        bufferCache->release(lbuf);
        bufferCache->release(dbuf);
      }
    }
    void recover() {
      readHead();
      installTransaction(true);
      log.header.n = 0;
      writeHead();
    }

    void writeLog() {
      for (uint64_t tail = 0; tail < log.header.n; ++tail) {
        auto to   = bufferCache->get(log.device, log.start + tail + 1);
        auto from = bufferCache->get(log.device, log.header.block[tail]);
        to->read();
        from->read();

        memmove(to->data, from->data, BLOCK_SIZE);
        to->write();
        bufferCache->release(from);
        bufferCache->release(to);
      }
    }
    void commit() {
      if (log.header.n > 0) {
        writeLog();
        writeHead();
        installTransaction(false);
        log.header.n = 0;
        writeHead();
      }
    }
  } // namespace
  void initializeLog(uint32_t device, const SuperBlock& sb) {
    log.start  = sb.logStart;
    log.size   = sb.numLogs;
    log.device = device;
    recover();
  }
  void beginOp() {
    mutex::LockGuard guard(log.mutex);
    while (true) {
      if (log.committing) {
        process::sleep(&log, log.mutex);
      } else if (log.header.n + (log.outstanding + 1) * MAX_OP_BLOCKS
                 > LOG_SIZE) {
        process::sleep(&log, log.mutex);
      } else {
        ++log.outstanding;
        break;
      }
    }
  }
  void endOp() {
    bool doCommit = false;

    {
      mutex::LockGuard guard(log.mutex);
      assert(!log.committing);
      --log.outstanding;

      if (log.outstanding == 0) {
        doCommit       = true;
        log.committing = true;
      } else {
        process::wakeup(&log);
      }
    }

    if (doCommit) {
      commit();
      mutex::LockGuard guard(log.mutex);
      log.committing = false;
      process::wakeup(&log);
    }
  }
  void writeLog(Buffer* buf) {
    mutex::LockGuard guard(log.mutex);
    assert(log.header.n < LOG_SIZE);
    assert(log.header.n < log.size - 1);
    assert(log.outstanding >= 1);

    uint64_t i;
    for (i = 0; i < log.header.n; ++i) {
      if (log.header.block[i] == buf->block) {
        break;
      }
    }

    log.header.block[i] = buf->block;
    if (i == log.header.n) {
      bufferCache->pin(buf);
      ++log.header.n;
    }
  }
} // namespace orfos::kernel::fs