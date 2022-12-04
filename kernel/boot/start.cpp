#include <arch/riscv.h>
#include <ie/clint.h>
#include <ie/timer.h>

#include "main.h"

namespace orfos::kernel::boot {
  namespace {
    // ie::Scratch mainHartScratch;
  }

  void start(bool primary, void* dtb) {
    // M-mode Previous Privilegeを01(=S-mode)に変更する
    // mret後にS-modeへ移行する
    uint64_t mstatus = arch::mstatus;
    mstatus &= ~arch::MSTATUS_MPP_MASK;
    mstatus |= arch::MSTATUS_MPP_S;
    arch::mstatus = mstatus;

    // mret後にmainへ移動する
    if (primary) {
      arch::mepc = primaryMain;
    } else {
      arch::mepc = secondaryMain;
    }

    // satpを0(=Bare)に設定
    // 仮想アドレス無効化
    arch::satp = 0;

    // 割り込みと例外処理をS-modeに委譲する
    // ただし、タイマ割り込みはM-modeで処理する
    arch::medeleg = 0xffff;
    arch::mideleg = 0xffff;
    uint64_t sie  = arch::sie;
    sie |= arch::SIE_SEIE;
    sie |= arch::SIE_STIE;
    sie |= arch::SIE_SSIE;
    arch::sie = sie;

    // L=0  <- ロックしない
    // A=01 <- TOR
    // X=1  <- 実行可能
    // W=1  <- 読み取り可能
    // R=1  <- 書き込み可能
    arch::pmpcfg0 = 0xf;
    // 0 < addr < 0x3fffffffffffffのアドレス空間に適用
    arch::pmpaddr0 = 0x3f'ffff'ffff'ffff;

    // タイマを初期化する
    if (primary) {
      // ie::initTimer(&mainHartScratch);
    } else {
      // ie::msip.fire(0);
    }

    // tpをhartidに変更
    uint64_t mhartid = arch::mhartid;
    arch::tp         = mhartid;

    // S-modeに切り替えてmainを呼ぶ
    arch::stval = dtb;
    arch::mret();
  }
} // namespace orfos::kernel::boot

extern "C" {
void orfos_primaryStart([[maybe_unused]] uint64_t hartid, void* dtb) {
  orfos::kernel::boot::start(true, dtb);
}
void orfos_secondaryStart() {
  orfos::kernel::boot::start(false, nullptr);
}
}