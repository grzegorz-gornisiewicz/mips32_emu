# mips32_emu
MIPS32 emulator

## implemented instructions

- ADD, ADDIU
- SLL, SRL
- JR, J, BEQ, BNE, BLTZ, BGEZ, SRL
- SYSCALL (prints integer and asciiz string)
- NOP (in fact it is sll $zero, $zero, 0)
