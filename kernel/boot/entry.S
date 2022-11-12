.section .text
.global _entry
_entry:
spin:
  wfi
  j spin
