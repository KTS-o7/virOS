[BITS 32]

global _start
extern c_start
extern viros_exit

section .asm

_start:
    call c_start
    call viros_exit
    ret