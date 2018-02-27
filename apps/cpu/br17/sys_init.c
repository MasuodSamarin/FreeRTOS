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
#include "my_malloc.h"
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

#ifdef HAVE_MMU
    mmu_init();
#else
    mem_init();
#endif

#ifdef HAVE_OS
    rtos_init(rtos_ucos_get_instance());
    OSIdleTaskInit(IDLE_TASK_STACK_SIZE);
    __asm__(\
            ".extern sys_main\n\t" \
            "movh sp, 8\n\t" \
            "movl sp, 4096\n\t" \
            /* "j sys_main\n\t" \ */
           );
#else
    __asm__(\
            ".extern _sys_stack\n\t" \
            "movh sp, _sys_stack\n\t" \
            "movl sp, _sys_stack\n\t" \
            /* "j sys_main\n\t" \ */
           );
    rtos_init(rtos_embedded_get_instance());
#endif
    sys_main();
}


static void app_sys_init()
{
    if (!power_is_poweroff_post()) {
        clock_init(APP_CLOCK_IN, APP_OSC_Hz, APP_SYS_Hz);
    }

    close_wdt();
}
sys_initcall(app_sys_init);

static void app_timer_init(void)
{
    /* puts("initcall - app_timer_init\n"); */

    sys_timer_init(APP_TICKS_UNIT);

    __this->sys_timer = periodic_timer_add(APP_TICKS_UNIT, sys_timer_schedule);
}
__initcall(app_timer_init);

static void app_os_time_tick(struct sys_timer *timer)
{
    rtos_time_tick(APP_TICKS_UNIT);

    sys_timer_register(&__this->timeout, APP_TICKS_UNIT, app_os_time_tick);
}

static void sys_init(void)
{
    if (!power_is_poweroff_post()) {
        puts("late_initcall - sys_init\n");
    }

    sys_timer_register(&__this->timeout, APP_TICKS_UNIT, app_os_time_tick);

    irq_global_enable();
}
late_initcall(sys_init);
