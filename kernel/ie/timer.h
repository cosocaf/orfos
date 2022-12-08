#ifndef ORFOS_KERNEL_IE_TIMER_H_
#define ORFOS_KERNEL_IE_TIMER_H_

#pragma once

#include <cstdint>

namespace orfos::kernel::ie {
  struct Scratch {
    /**
     * @brief a1レジスタ保存用
     *
     * タイマ割り込み時にa1レジスタを保存するために使用される領域。
     */
    uint64_t a1;
    /**
     * @brief a2レジスタ保存用
     *
     * タイマ割り込み時にa2レジスタを保存するために使用される領域。
     */
    uint64_t a2;
    /**
     * @brief a3レジスタ保存用
     *
     * タイマ割り込み時にa3レジスタを保存するために使用される領域。
     */
    uint64_t a3;
    /**
     * @brief mtimecmpレジスタ
     *
     * hartごとに使用されるmtimecmpレジスタのアドレス。
     */
    uint64_t* mtimecmp;
    /**
     * @brief タイマ割り込みの間隔
     *
     * タイマ割り込みが発生する間隔。
     * 単位は100ns。
     */
    uint64_t interval;
  };
  void initTimer(Scratch* scratch);
} // namespace orfos::kernel::ie

#endif // ORFOS_KERNEL_IE_TIMER_H_