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

void OSIdleTaskInit(u32 stack_size);
void sys_main(void);

#define WD_1MS			0x00
#define WD_2MS			0x01
#define WD_4MS			0x02
#define WD_8MS			0x03
#define WD_16MS			0x04
#define WD_32MS			0x05
#define WD_64MS			0x06
#define WD_128MS		0x07
#define WD_256MS		0x08
#define WD_512MS		0x09
#define WD_1S			0x0A
#define WD_2S			0x0B
#define WD_4S			0x0C
#define WD_8S			0x0D
#define WD_16S			0x0E
#define WD_32S			0x0F

static void __watchdog_isr(void)
{
    u32 tmp;
    u32 rets, reti;

    __asm__ volatile("mov %0,RETS" : "=r"(rets));
    __asm__ volatile("mov %0,RETI" : "=r"(reti));

    puts("\n\n---------------------------SDK watchdog---------------------\n\n");
    printf("RETS = %08x \n", rets);
    printf("RETI = %08x \n", reti);

    __asm__ volatile("mov %0,sp" : "=r"(tmp));
    printf("SP = %08x \n", tmp);
    __asm__ volatile("mov %0,usp" : "=r"(tmp));
    printf("USP = %08x \n", tmp);
    __asm__ volatile("mov %0,ssp" : "=r"(tmp));
    printf("SSP = %08x \n", tmp);

    while (1);
}
IRQ_REGISTER(IRQ_WD_IDX, __watchdog_isr);

static void close_wdt(void)
{
    CPU_SR_ALLOC();
    OS_ENTER_CRITICAL();
    JL_CRC->REG = 0x6ea5;
    JL_SYSTEM->WDT_CON &= ~BIT(4);
    JL_CRC->REG = 0;
    OS_EXIT_CRITICAL();
}
static void init_wdt(u8 cfg)
{
    CPU_SR_ALLOC();
    OS_ENTER_CRITICAL();
    //puts("open_wdt\n");
    JL_CRC->REG  = 0X6EA5;
    JL_SYSTEM->WDT_CON  = cfg & 0x0f;
    /* JL_SYSTEM->WDT_CON |= BIT(5);	//ie enable */
    JL_SYSTEM->WDT_CON &= ~BIT(5);	//ie disenable
    JL_SYSTEM->WDT_CON |=  BIT(4);
    JL_CRC->REG  = 0XAAAA;
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

    /* init_wdt(WD_4S); */
    /* IRQ_REQUEST(IRQ_WD_IDX, __watchdog_isr, 3); */
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
