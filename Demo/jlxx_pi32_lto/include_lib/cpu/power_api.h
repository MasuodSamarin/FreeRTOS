#ifndef _POWER_API_H_
#define _POWER_API_H_

#include "typedef.h"
#include "power_interface.h"
//API
#define AT_VOLATILE_RAM             AT(.volatile_ram)
#define AT_VOLATILE_RAM_CODE        AT(.volatile_ram_code)
#define AT_NON_VOLATILE_RAM         AT(.non_volatile_ram)
#define AT_NON_VOLATILE_RAM_CODE    AT(.non_volatile_ram_code)

#define SLEEP_EN                            BIT(2)
#define SLEEP_SAVE_BEFORE_ENTER_MS          1
#define SLEEP_RECOVER_AFTER_EXIT_MS         1
#define SLEEP_TIMEOUT_MIN_US                ((SLEEP_SAVE_BEFORE_ENTER_MS+SLEEP_RECOVER_AFTER_EXIT_MS)*1000L)  //间隔至少要20slot以上才进入power down

#define DEEP_SLEEP_EN                       BIT(1)
#define DSLEEP_SAVE_BEFORE_ENTER_MS         1
#define DSLEEP_RECOVER_AFTER_EXIT_MS        10
#define DEEP_SLEEP_TIMEOUT_MIN_US           (60*625L)  //间隔至少要60slot以上才进入power off

#define SLEEP_TICKS_UNIT                    (10*1000L) //
#define DEEP_SLEEP_TICKS_UNIT               (20*1000L) //


enum {
    BT_OSC = 0,
    RTC_OSCH,
    RTC_OSCL,
    LRC_32K,
};

enum {
    PWR_NO_CHANGE = 0,
    PWR_LDO33,
    PWR_LDO15,
    PWR_DCDC15,
};

struct low_power_param {
    u8 osc_type;
    u32 osc_hz;
    u8  delay_us;
    u8  config;
    u8  btosc_disable;
};


enum {
    POWER_OSC_INFO,
    POWER_DELAY_US,
    POWER_SET_RESET_MASK,
    POWER_SET_BTOSC_DISABLE,
    POWER_SET_KEEP_OSCI_FLAG,

    POWER_SET_SLEEP_CALLBACK,
    POWER_SET_DEEPSLEEP_CALLBACK,
    POWER_SET_MODE,
    POWER_SET_SOFTOFF,
    POWER_SET_IDLE,
};



void poweroff_recover(void);

void power_on(void *priv);

void power_off(void *priv);

void power_off_lock();

void power_off_unlock();

void *power_get(void *priv, const struct power_operation *ops);

void power_put(void *priv);

void power_init(const struct low_power_param *param);

u8 power_is_poweroff_probe();

u8 power_is_poweroff_post();

void power_set_soft_poweroff();

void power_set_mode(u8 mode);

void power_set_callback(u8 mode, void (*enter)(void), void (*exit)(u32));

u8 power_is_poweroff_post(void);

void power_set_proweroff(void);

#endif

