#include "typedef.h"
#include "irq_interface.h"
#include "interrupt.h"
#include "printf.h"
#include "uart.h"
#include "init.h"
#include "clock_interface.h"
#include "os/rtos.h"
#include "sys_timer.h"
#include "async_timer.h"

struct sys_hdl {
    void *sys_timer;
    void *os_ptimer;
    void *async_ptimer;
    u8 pad;
};
static struct sys_hdl hdl;

#define __this  (&hdl)

static u32 args[3];

void sys_arch_init(u32 cfg_addr, u32 addr, u32 res)
{
    args[0] = cfg_addr;
    args[1] = addr;
    args[2] = res;

    irq_init();

    vmm_init();

    OSInit();

    OSIdleTaskInit(IDLE_TASK_STACK_SIZE);
    __asm__(\
            "movh sp, 8\n\t" \
            "movl sp, 4096\n\t" \
           );
    sys_main();
}

static void close_wdt(void)
{
    CPU_SR_ALLOC();
    OS_ENTER_CRITICAL();
    CRC_REG = 0x6ea5;
    WDT_CON &= ~BIT(4);
    CRC_REG = 0;
    OS_EXIT_CRITICAL();
}

static void app_sys_init()
{
    clock_init(SYS_CLOCK_IN, OSC_Hz, SYS_Hz);
    close_wdt();
    uart_init(460800);
}
sys_initcall(app_sys_init);

static void app_timer_init(void)
{
    puts("initcall - app_timer_init\n");

    sys_timer_init();
    __this->sys_timer = periodic_timer_add(10, sys_timer_schedule);

    /* periodic_async_timer_init(); */
    /* __this->async_ptimer = periodic_timer_add(2, async_timer_schedule_probe); */
}
__initcall(app_timer_init);


static void sys_init(void)
{
    puts("late_initcall - sys_init\n");

    puts("-1-\n");
    OSStart();
    __this->os_ptimer = periodic_timer_add(10, OSTimeTick);
    /* __this->os_ptimer = periodic_async_timer_add(10, OSTimeTick); */

    u32 tmp;
    __asm__ volatile("mov %0,ie0" : "=r"(tmp));
    printf("ie0 =%08x \n", tmp);
    __asm__ volatile("mov %0,ie1" : "=r"(tmp));
    printf("ie1 =%08x \n", tmp);
    __asm__ volatile("mov %0,icfg" : "=r"(tmp));
    printf("icfg =%08x \n", tmp);

    irq_global_enable();
}
late_initcall(sys_init);
