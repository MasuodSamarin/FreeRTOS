#include "sdk_cfg.h"
#include "irq_interface.h"
#include "interrupt.h"
#include "printf.h"
#include "uart.h"
#include "init.h"
#include "clock_interface.h"
#include "os/rtos.h"
#include "sys_timer.h"
#include "async_timer.h"
#include "clock_api.h"
#include "malloc.h"
#include "uart_interface.h"
#include "timer_interface.h"


struct sys_hdl {
    void *sys_timer;
    void *os_ptimer;
    void *async_ptimer;
    u8 pad;

    //system timer
    struct sys_timer timeout;
};
static struct sys_hdl hdl;

#define __this  (&hdl)

u32 args[3];

u32 vmm_init(void);
void OSIdleTaskInit(u32 stack_size);
void sys_main(void);

static void close_wdt(void)
{
    CPU_SR_ALLOC();
    OS_ENTER_CRITICAL();
    JL_CRC->REG = 0x6ea5;
    JL_SYSTEM->WDT_CON &= ~BIT(4);
    JL_CRC->REG = 0;
    OS_EXIT_CRITICAL();
}


void sys_arch_init(u32 cfg_addr, u32 addr, u32 res)
{
    args[0] = cfg_addr;
    args[1] = addr;
    args[2] = res;


    irq_init();

    vmm_init();

    os_init();

    OSIdleTaskInit(IDLE_TASK_STACK_SIZE);

    __asm__(\
            ".extern sys_main\n\t" \
            "movh sp, 8\n\t" \
            "movl sp, 4096\n\t" \
            /* "j sys_main\n\t" \ */
           );
    sys_main();
}


static void app_sys_init()
{
    if (!power_is_poweroff_post()) {
        /* clock_init(APP_CLOCK_IN, APP_OSC_Hz, APP_SYS_Hz); */
    }

    close_wdt();
}
sys_initcall(app_sys_init);

static void app_timer_init(void)
{
    /* puts("initcall - app_timer_init\n"); */

    /* periodic_async_timer_init(); */
    /* __this->async_ptimer = periodic_timer_add(2, async_timer_schedule_probe); */

    sys_timer_init();
    /* __this->sys_timer = periodic_async_timer_add(10, sys_timer_schedule); */
#if (APP_LOWPOWER_SELECT == 0)
    //if no sleep mode , sys timer relay on timer_hw
    __this->sys_timer = periodic_timer_add(10, sys_timer_schedule);
#endif
}
__initcall(app_timer_init);

static void app_os_time_tick(struct sys_timer *timer)
{
    os_time_tick();
    sys_timer_register(&__this->timeout, 10, app_os_time_tick);
}

static void sys_init(void)
{
    if (!power_is_poweroff_post()) {
        puts("late_initcall - sys_init\n");
    }

    os_start();

    /* __this->os_ptimer = periodic_timer_add(10, os_time_tick); */
    /* __this->os_ptimer = periodic_async_timer_add(10, os_time_tick); */

    sys_timer_register(&__this->timeout, 10, app_os_time_tick);

    irq_global_enable();
}
late_initcall(sys_init);
