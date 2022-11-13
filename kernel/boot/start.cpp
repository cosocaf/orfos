#include <arch/riscv.h>
#include <arch/riscv_macros.h>

#include "main.h"

extern "C" {
void orfos_start() {
  using namespace orfos::kernel::arch;
  using namespace orfos::kernel::boot;

  // M-mode Previous Privilegeを01(=S-mode)に変更する
  // mret後にS-modeへ移行する
  auto mstatus = read_mstatus();
  mstatus &= ~MSTATUS_MPP_MASK;
  mstatus |= MSTATUS_MPP_S;
  write_mstatus(mstatus);

  // mret後にmainへ移動する
  write_mepc(reinterpret_cast<uint64_t>(main));

  // satpを0(=Bare)に設定
  // 仮想アドレス無効化
  write_satp(0);

  // 割り込みと例外処理をS-modeに委譲する
  // ただし、タイマ割り込みはM-modeで処理する
  write_medeleg(0xffff);
  write_mideleg(0xffff);
  auto sie = read_sie();
  sie |= SIE_SEIE;
  sie |= SIE_STIE;
  sie |= SIE_SSIE;
  write_sie(sie);

  // L=0  <- ロックしない
  // A=01 <- TOR
  // X=1  <- 実行可能
  // W=1  <- 読み取り可能
  // R=1  <- 書き込み可能
  write_pmpcfg0(0xf);
  // 0 < addr < 0x3fffffffffffffのアドレス空間に適用
  write_pmpaddr0(0x3fffffffffffff);

  // TODO: timer

  // tpをhartidに変更
  auto mhartid = read_mhartid();
  write_tp(mhartid);

  // S-modeに切り替えてmainを呼ぶ
  mret();
};
}