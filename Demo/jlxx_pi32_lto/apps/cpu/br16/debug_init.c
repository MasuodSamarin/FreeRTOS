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


struct debug_hdl {
    void *timer[5];
};
static struct debug_hdl hdl;

#define __this  (&hdl)

const char exception[] = "\r\n*********Exception Isr " __DATE__ " "__TIME__ "******\r\n";
static void exception_isr(void)
{
    u32 tmp;

    ///printf rets & reti
    asm("mov %0, RETS " : "=r"(tmp));

    printf("\nRETS=0x%x\n", tmp);

    printf("DEBUG_MSG = 0x%x\n", DEBUG_MSG);

    asm("mov %0, RETI " : "=r"(tmp));

    printf("\nRETI=0x%x\n", tmp);

    puts(exception);
    while (1);
#ifdef __DEBUG
    PWR_CON |= BIT(4); //异常后软复位
#else
    PWR_CON |= BIT(4); //异常后软复位
#endif // __DEBUG
}
/* IRQ_REGISTER(IRQ_EXCEPTION_IDX, exception_isr); */


_NOINLINE_
void exception_misaligned(void)
{
    /* puts("exception_misaligned\n"); */
    u32 tmp;

    ///printf rets & reti
    u32 *ptr = 0x55;

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

        u32 *ptr = 0x100;

        *ptr = 0x1;
    }
    break;
    case 1: {
        //range outside
        /* __cpu_protect_ram(0x101, 0x110, 0); */
        __debug_drv->ioctrl(PC_WR_RAM, 0x101, 0x110, 0);

        u32 *ptr = 0x100;

        *ptr = 0x1;
    }
    break;
    }

}

static exception_dsp_pc_err(void)
{
    //range outside
    /* __pc_protect_ram(0x101, 0x110); */
    /* __debug_drv->ioctrl(PC_RUN_RAM, 0x101, 0x110, 0); */
    DEBUG_PC_LIMIT(0x101, 0x110);

    void (*foo)(void);
    foo = 0x100;
    foo();
}

const char pubDate[] = "\r\n*********SDK Main" __DATE__ " "__TIME__ "******\r\n";

static void debug_sys_tick_500ms(void)
{
    puts(".");
}

static void debug_sys_tick_100ms(void)
{
    puts("*");
}

static void debug_os_tick_1s(void)
{
    printf("os tick %d\n", OSTimeGet());

    os_taskq_post(TASK_NAME_UI, 1, OSTimeGet() % 1000);
    puts("send\n");
}

static void app_debug_init(void)
{
    puts(pubDate);
    puts("app_initcall - app_debug_init\n");
    /* uart_init(460800); */
    printf("--Reset Source : 0x%x\n", PWR_CON >> 5); //0:PWR_ON 1:VCM 2:IO 3:LVD 4:WDT 5:SRST
    /* IRQ_REQUEST(IRQ_EXCEPTION_IDX, exception_isr, 0); //exception isr always in highest IP,can not set IP */
    irq_handler_register(0, exception_isr, 3);

    /* exception_dsp_ex_err(0); */
    /* exception_dsp_ex_err(1); */
    /* exception_dsp_pc_err(); */
    /* exception_misaligned(); */

    __this->timer[0] = periodic_timer_add(500, debug_sys_tick_500ms);
    /* periodic_timer_remove(__this->timer[0]); */
    /* __this->timer[0] = periodic_async_timer_add(500, debug_sys_tick_500ms); */

    __this->timer[1] = periodic_timer_add(1000, debug_os_tick_1s);
    /* periodic_timer_remove(__this->timer[1]); */
    /* __this->timer[1] = periodic_async_timer_add(1000, debug_os_tick_1s); */

}
app_initcall(app_debug_init);

