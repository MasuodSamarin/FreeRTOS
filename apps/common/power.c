#include "power_interface.h"
#include "sdk_cfg.h"
#include "init.h"
#include "irtc_interface.h"
#include "debug.h"


/**
 * @brief System Low Power setting
 */
const struct low_power_param power_param = {
    .config         = APP_LOWPOWER_SELECT,
    .osc_type       = APP_LOWPOWER_OSC_TYPE,
    .osc_hz         = APP_LOWPOWER_OSC_HZ,
    .delay_us       = APP_SYS_Hz,
    .btosc_disable  = APP_LOWPOWER_BTOSC_DISABLE,
};

static void __user_set_sleep_enter(void)
{
    if (JL_IOMAP->CON0 & BIT(5)) {  //boot B
        /* putchar('B'); */
        JL_PORTA->DIR |=  0xffff;
        JL_PORTA->PU &= ~0xffff;
        JL_PORTA->PD &= ~0xffff;
        JL_PORTA->DIE &= ~0xffff;

        /*注意spi flash的几个脚不能关*/
        JL_PORTB->DIR |=  0xffb8;
        JL_PORTB->PU &= ~0xffb8;
        JL_PORTB->PD &= ~0xffb8;
        JL_PORTB->DIE &= ~0xffb8;

        JL_PORTC->DIR |=  0xffff;
        JL_PORTC->PU &= ~0xffff;
        JL_PORTC->PD &= ~0xffff;
        JL_PORTC->DIE &= ~0xffff;

        JL_PORTD->DIR |= 0xffff;
        JL_PORTD->PU  &= ~0xffff;
        JL_PORTD->PD  &= ~0xffff;
        JL_PORTD->DIE &= ~0xffff;
    } else {                        //boot A
        /* putchar('A'); */
        JL_PORTA->DIR |=  0xffff;
        JL_PORTA->PU &= ~0xffff;
        JL_PORTA->PD &= ~0xffff;
        JL_PORTA->DIE &= ~0xffff;

        JL_PORTB->DIR |=  0xffff;
        JL_PORTB->PU &= ~0xffff;
        JL_PORTB->PD &= ~0xffff;
        JL_PORTB->DIE &= ~0xffff;

        JL_PORTC->DIR |=  0xffff;
        JL_PORTC->PU &= ~0xffff;
        JL_PORTC->PD &= ~0xffff;
        JL_PORTC->DIE &= ~0xffff;

        /*注意spi flash的几个脚不能关*/
        JL_PORTD->DIR |= 0xfff0;
        JL_PORTD->PU  &= ~0xfff0;
        JL_PORTD->PD  &= ~0xfff0;
        JL_PORTD->DIE &= ~0xfff0;
    }
}

static void sleep_enter_callback(void)
{
    /* IO_DEBUG_0(A, 0); */

    //Store RTC setting
    rtc_port_store();
    rtc_port_close();
    __user_set_sleep_enter();
}

static void sleep_exit_callback(u32 usec)
{
    /* IO_DEBUG_1(A, 0); */

    //Restore RTC setting
    rtc_port_restore();
    /* log_info("sleep exit\n"); */
}


void app_power_init()
{
    power_init(&power_param);

    power_set_mode(APP_SYS_POWER_SELECT);

    if (power_is_poweroff_post()) {
        poweroff_recover();
    } else {
#if (APP_LOWPOWER_SELECT == SLEEP_EN)
        power_set_callback(APP_LOWPOWER_SELECT, sleep_enter_callback, sleep_exit_callback);
#endif
    }
}
sys_initcall(app_power_init);

/**
 * @brief app_power_set_idle : power consumption : 300uA
 */
/* void app_power_set_idle(void) */
/* { */
/*     log_info("\napp_power_set_idle\n"); */
/*  */
/*     OS_ENTER_CRITICAL(); */
/*  */
/*     rtc_low_power(); */
/*  */
/*     //set wakeup I/O */
/*     rtc_set_wakeup_io(2, 0); */
/*  */
/*     power_set_idle(); */
/* } */

/**
 * @brief app_power_set_softoff : power consumption : 2uA
 */
void app_power_set_softoff(void)
{
    log_info("\napp_power_set_softoff\n");

    OS_ENTER_CRITICAL();

    rtc_low_power();
    //set wakeup I/O
    rtc_set_wakeup_io(2, 0);
    /* __rtc_debug(); */

    power_set_soft_poweroff();
}

