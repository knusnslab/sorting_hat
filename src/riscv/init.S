.section .text
.global _instr_start
.global _instr_end

_instr_start:

    lui fp, 0xdead
    srli fp, fp, 4
    ori fp, fp, 0xbe
    slli fp, fp, 8
    ori fp, fp, 0xaf
    lui   x15, 0x6796
    srli x15, x15, 4
    ori x15, x15, 0x4c
    slli x15, x15, 8
    ori x15, x15, 0x91
    slli  x15, x15, 8
    ori x15, x15, 0x3c
    slli  x15, x15, 8
    ori x15, x15, 0x7c
    slli  x15, x15, 8
    ori x15, x15, 0xf8
    slli  x15, x15, 8
    ori x15, x15, 0xb9
_instr_end:             
