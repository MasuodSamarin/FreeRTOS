/*********************************************************************************************
    *   Filename        : startup.S

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2017-01-11 11:48

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/

.section .stack, "a"
         .space 0x800
         .global _stack
_stack:

; .section .ustack, "a"
         ; .space 0x800
         ; .global _sys_stack
; _sys_stack:

#define hi(x) ((x)>>16)
#define lo(x) ((x)&0xffff)
.section .text
    .align 2
    .global _start
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

    movh  r0, data1_addr
    movl  r0, data1_addr
    movh  r1, data1_begin
    movl  r1, data1_begin
    movh  r2, data1_size
    movl  r2, data1_size

    jz    r2, .clean_bss
.copy_nvdata:
    cmp r2, 32
    ifcs 2, 2
    movs r4, 0
    sub r2, r2, 32
    mov r4, r2
    movs r2, 0
    rep .copy_nvdata2, r4
    lbzadd r3, r1, 1
    sbadd r3, r0, 1
.copy_nvdata2:
    jnz  r2, .copy_nvdata

.clean_bss:
    movh  r0, bss_begin
    movl  r0, bss_begin
    mov   r1, 0
    movh  r2, bss_size
    movl  r2, bss_size

    jz    r2, .clean_nvbss
.clean_bss1:
    cmp r2, 32
    ifcs 2, 2
    movs r3, 0
    sub r2, r2, 32
    mov r3, r2
    movs r2, 0
    rep .clean_bss2, r3
    sbadd r1, r0, 1
.clean_bss2:
    jnz   r2, .clean_bss1

.clean_nvbss:
    movh  r0, nvbss_begin
    movl  r0, nvbss_begin
    mov   r1, 0
    movh  r2, nvbss_size
    movl  r2, nvbss_size

    jz    r2, .copy_data
.clean_nvbss1:
    cmp r2, 32
    ifcs 2, 2
    movs r3, 0
    sub r2, r2, 32
    mov r3, r2
    movs r2, 0
    rep .clean_nvbss2, r3
    sbadd r1, r0, 1
.clean_nvbss2:
    jnz   r2, .clean_nvbss1

.copy_data:
    movh  r0, data_addr
    movl  r0, data_addr
    movh  r1, data_begin
    movl  r1, data_begin
    movh  r2, data_size
    movl  r2, data_size

    jz    r2, .done
.copy_data1:
    cmp r2, 32
    ifcs 2, 2
    movs r4, 0
    sub r2, r2, 32
    mov r4, r2
    movs r2, 0
    rep .copy_data2, r4
    lbzadd r3, r1, 1
    sbadd r3, r0, 1
.copy_data2:
    jnz  r2, .copy_data1

.done:
    mov  r0,r7
    mov  r1,r5
    mov  r2,r6

    j  sys_arch_init

