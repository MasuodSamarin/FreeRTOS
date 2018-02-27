/***********************************Jieli tech************************************************
  File : startup.S
  By   : Juntham
  date : 2015-08-21 09:06
********************************************************************************************/

.section .stack, "a"
         .space 0x400
         .global _stack
_stack:

#define hi(x) ((x)>>16)
#define lo(x) ((x)&0xffff)
.section .text
    .align 2
    .global _start
    .extern memset
    .type _start,@function
    .org 0

    .extern sys_arch_init
_start:
    mov  r7,r0
    mov  r5,r1
    mov  r6,r2
    movh ssp, _stack
    movl ssp, _stack
    movh sp, _stack
    movl sp, _stack
    /* call pll_init */


    /* call bt_power_is_poweroff_probe */
    mov  r0 ,0
    cmp  r0 ,0
    jne .L7

    movh  r0, ram1_addr
    movl  r0, ram1_addr
    movh  r1, ram1_begin
    movl  r1, ram1_begin
    movh  r2, ram1_size
    movl  r2, ram1_size

    jz    r2, .L7
.L5:
    cmp r2, 32
    ifcs 2, 2
    movs r4, 0
    sub r2, r2, 32
    mov r4, r2
    movs r2, 0
    rep .L6, r4
    lbzadd r3, r1, 1
    sbadd r3, r0, 1
.L6:
    jnz  r2, .L5
.L7:


    movh  r0, bss_begin
    movl  r0, bss_begin
    mov   r1,0
    movh  r2, bss_size
    movl  r2, bss_size

    jz    r2, .L_2
.L_1:
    cmp r2, 32
    ifcs 2, 2
    movs r3, 0
    sub r2, r2, 32
    mov r3, r2
    movs r2, 0
    rep .L_01, r3
    sbadd r1, r0, 1
.L_01:
    jnz   r2, .L_1
.L_2:

    movh  r0, data_addr
    movl  r0, data_addr
    movh  r1, data_begin
    movl  r1, data_begin
    movh  r2, data1_size
    movl  r2, data1_size

    jz    r2, .L4
.L2:
    cmp r2, 32
    ifcs 2, 2
    movs r4, 0
    sub r2, r2, 32
    mov r4, r2
    movs r2, 0
    rep .L3, r4
    lbzadd r3, r1, 1
    sbadd r3, r0, 1
.L3:
    jnz  r2, .L2
.L4:

    mov  r0,r7
    mov  r1,r5
    mov  r2,r6
    j  sys_arch_init

