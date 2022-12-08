OUTPUT_ARCH( "riscv" )
ENTRY( orfos_entry )

SECTIONS {
  . = 0x80000000;

  .text : {
    *(.entry)
    *(.text .text.*)
    . = ALIGN(0x1000);
    _trampoline = .;
    *(trampsec)
    . = ALIGN(0x1000);
    /* ASSERT(. - _trampoline == 0x1000, "error: trampoline larger than one page"); */
    PROVIDE(endOfText = .);
  }

  .rodata : {
    . = ALIGN(16);
    *(.srodata .srodata.*)
    . = ALIGN(16);
    PROVIDE(beginUserInitCode = .);
    *(.userinit)
    PROVIDE(endUserInitCode = .);
    . = ALIGN(16);
    *(.rodata .rodata.*)
  }

  .data : {
    . = ALIGN(16);
    *(.sdata .sdata.*)
    . = ALIGN(16);
    *(.data .data.*)
  }

  .bss : {
    . = ALIGN(16);
    *(.sbss .sbss.*)
    . = ALIGN(16);
    *(.bss .bss.*)
  }

  . = ALIGN(4096);
  PROVIDE(orfos_endOfKernelSections = .);
}