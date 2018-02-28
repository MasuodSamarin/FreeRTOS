/*********************************************************************************************
    *   Filename        : debug_init.c

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2016-07-12 11:39

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/
#include "typedef.h"
#include "irq_interface.h"
#include "interrupt.h"
#include "printf.h"
#include "uart.h"
#include "init.h"
#include "clock_interface.h"
#include "os/rtos.h"
#include "debug_interface.h"
#include "task_schedule.h"
#include "clock_api.h"
#include "timer_interface.h"
#include "uart_interface.h"
#include "sdk_cfg.h"
#include "sys_timer.h"
#include "os/rtos.h"

#pragma str_literal_override("debug_init.rodata")

struct debug_hdl {
    //periodic timer
    void *timer[5];

    //system timer
    u32 period_ms;
    struct sys_timer timeout;

    struct sys_timer timeout_500ms;
    struct sys_timer timeout_ms;
};
static struct debug_hdl hdl;

#define __this  (&hdl)

/*代码运行保护符号使用说明，refer ram.ld*/
extern u32 mpu_prp_start;
extern u32 mpu_prp_end;
extern u32 mpu_pc_start;
extern u32 mpu_pc_end;
extern u32 mpu_pc_ex_start;
extern u32 mpu_pc_ex_end;

const char exception[] = "\r\n*********Exception Isr " __DATE__ " "__TIME__ "******\r\n";
static void exception_isr(u32 rets, u32 reti)
/* static void exception_isr(void) */
{
    /* u32 rets, reti; */

    /* __asm__ volatile("mov %0,RETS" : "=r"(rets)); */
    /* __asm__ volatile("mov %0,RETI" : "=r"(reti)); */

    puts("\n\n---------------------------SDK exception_isr---------------------\n\n");
    put_u32hex(JL_DEBUG->MSG);

    if (JL_DEBUG->MSG & BIT(2)) {
        puts("Peripheral ex limit err\n");
    }
    if (JL_DEBUG->MSG & BIT(8)) {
        puts("DSP MMU err,DSP write MMU All 1\n");
    }
    if (JL_DEBUG->MSG & BIT(9)) {
        puts("PRP MMU err,PRP write MMU 1\n");
    }
    if (JL_DEBUG->MSG & BIT(16)) {
        puts("PC err,PC over limit\n");
    }
    if (JL_DEBUG->MSG & BIT(17)) {
        puts("DSP ex err,DSP store over limit\n");
    }
    if (JL_DEBUG->MSG & BIT(18)) {
        puts("DSP illegal,DSP instruction\n");
    }
    if (JL_DEBUG->MSG & BIT(19)) {
        puts("DSP misaligned err\n");
    }

    u32 tmp;

    printf("RETS = %08x \n", rets);
    printf("RETI = %08x \n", reti);
    __asm__ volatile("mov %0,sp" : "=r"(tmp));
    printf("SP = %08x \n", tmp);
    __asm__ volatile("mov %0,usp" : "=r"(tmp));
    printf("USP = %08x \n", tmp);
    __asm__ volatile("mov %0,ssp" : "=r"(tmp));
    printf("SSP = %08x \n", tmp);

#if (APP_UART_DEBUG == 1)
    while (1);
#else
    JL_POWER->CON |= BIT(4); //异常后软复位
#endif // __DEBUG
}
/* IRQ_REGISTER(IRQ_EXCEPTION_IDX, exception_isr); */


_NOINLINE_
void exception_misaligned(void)
{
    /* puts("exception_misaligned\n"); */
    u32 tmp;

    ///printf rets & reti
    u32 *ptr = (u32 *)0x55;

    *ptr = 0x1;
    puts("exception_misaligned\n");
}

void exception_dsp_ex_err(u8 cmd)
{
    switch (cmd) {
    case 0: {
        //range inside
        /* __cpu_protect_ram(0x100, 0x110, 1); */
        __debug_drv->ioctrl(PC_WR_RAM, 0x100, 0x110, 1);

        u32 *ptr = (u32 *)0x104;

        *ptr = 0x1;
    }
    break;
    case 1: {
        //range outside
        /* __cpu_protect_ram(0x101, 0x110, 0); */
        __debug_drv->ioctrl(PC_WR_RAM, 0x101, 0x110, 0);

        u32 *ptr = (u32 *)0x104;

        *ptr = 0x1;
    }
    break;
    }

}

static void exception_dsp_pc_err(void)
{
    //range outside
    /* __pc_protect_ram(0x101, 0x110); */
    /* __debug_drv->ioctrl(PC_RUN_RAM, 0x101, 0x110, 0); */
    DEBUG_PC_LIMIT(0x101, 0x110);

    void (*foo)(void);
    foo = (void (*)(void))0x100;
    foo();
}

static void examples(void)
{
    exception_dsp_ex_err(0);
    exception_dsp_ex_err(1);
    exception_dsp_pc_err();
    exception_misaligned();

}

static void __mpu_enable(void)
{
    /* __debug_drv->ioctrl(PC_WR_RAM, &mpu_pc_start, &mpu_pc_end, 1); */

    DEBUG_PC_LIMIT(&mpu_pc_ex_start, &mpu_pc_ex_end);
}


static void sys_tick_500ms(void)
{
    puts(".");
    /* JL_SYSTEM->WDT_CON |= BIT(6); */
    /* printf("LSB clock : %d\n", TIMER_CLK); */
}

static void app_sys_tick_500ms(struct sys_timer *timer)
{
    sys_tick_500ms();
    sys_timer_register(&__this->timeout_500ms, 500, app_sys_tick_500ms);
}

static void sys_tick_nms(void)
{
    printf("os tick %d\n", rtos_time_get());
    /* puts("send\n"); */
}

static void app_sys_tick_nms(struct sys_timer *timer)
{
    sys_tick_nms();
    sys_timer_register(&__this->timeout_ms, 1000L, app_sys_tick_nms);
}

static void timeout_handler(struct sys_timer *timer)
{
    puts("1s timeout handler\n");
    sys_timer_register(&__this->timeout, __this->period_ms, timeout_handler);
}

static void timeout_start(u32 timeout_ms)
{
    puts("timeout start\n");
    __this->period_ms = timeout_ms;
    sys_timer_register(&__this->timeout, timeout_ms, timeout_handler);
}

static void debug_reset_source(void)
{
    u32 sfr;

    sfr = JL_SYSTEM->RST_SRC;
    printf("--Reset Source : 0x%x\n", sfr);

    if (sfr & BIT(0)) {
        puts("SPOR\n");
    }
    if (sfr & BIT(1)) {
        puts("SLVD\n");
    }
    if (sfr & BIT(2)) {
        puts("SWDT\n");
    }
    if (sfr & BIT(3)) {
        puts("SOFTR\n");
    }
    if (sfr & BIT(4)) {
        puts("PLPM\n");
    }
}

const char pubDate[] = "\r\n[FreeRTOS SDK Main "__DATE__ " "__TIME__ "]\r\n";
extern u32 args[3];

static void app_debug_init(void)
{
    uart_init(APP_UART_BAUD);

    if (!power_is_poweroff_post()) {
        puts((char *)pubDate);
        if (app_debug_init > 0x1ff0000) {
            puts("\r\n-----------OTP-------------\n");
        } else {
            puts("\r\n============Flash============\r\n");
        }

        puts("app_initcall - app_debug_init\n");

        /* clock_debug(); */
        debug_reset_source();
    }

    /* printf("exception_isr_hook : %x\n", *((u32 *)0x100)); */
    irq_handler_register(IRQ_EXCEPTION_IDX, exception_isr, 3);
    /* __mpu_enable(); */

    sys_timer_register(&__this->timeout_500ms, 500, app_sys_tick_500ms);

    /* sys_timer_register(&__this->timeout_ms, 1000L, app_sys_tick_nms); */

    /* timeout_start(1000); */
}
app_initcall(app_debug_init);

