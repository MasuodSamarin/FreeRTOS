/*********************************************************************************************
    *   Filename        : key_drv_ad.c

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2017-02-16 10:57

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/
#include "adc_interface.h"
#include "init.h"
#include "key_drv_ad.h"
#include "stdarg.h"
#include "sdk_cfg.h"
#include "timer_interface.h"

//#define KEY_UART_DEBUG
#ifdef KEY_UART_DEBUG
#define key_puts           puts
#define key_printf         printf
#define key_buf            put_buf
#else
#define key_puts(...)
#define key_printf(...)
#define key_buf(...)
#endif/*KEY_UART_DEBUG*/

#define ADC10_33        (0x3dfL)
#define ADC10_30        (0x3dfL*30/33)
#define ADC10_27        (0x3dfL*27/33)
#define ADC10_23        (0x3dfL*23/33)
#define ADC10_20        (0x3dfL*20/33)
#define ADC10_17        (0x3dfL*17/33)
#define ADC10_13        (0x3dfL*13/33)
#define ADC10_10        (0x3dfL*10/33)
#define ADC10_07        (0x3dfL*07/33)
#define ADC10_04        (0x3dfL*04/33)
#define ADC10_00        (0)

#define AD_NOKEY        ((ADC10_33 + ADC10_30)/2)
#define ADKEY1_0		((ADC10_30 + ADC10_27)/2)
#define ADKEY1_1		((ADC10_27 + ADC10_23)/2)
#define ADKEY1_2		((ADC10_23 + ADC10_20)/2)
#define ADKEY1_3		((ADC10_20 + ADC10_17)/2)
#define ADKEY1_4		((ADC10_17 + ADC10_13)/2)
#define ADKEY1_5		((ADC10_13 + ADC10_10)/2)
#define ADKEY1_6		((ADC10_10 + ADC10_07)/2)
#define ADKEY1_7		((ADC10_07 + ADC10_04)/2)
#define ADKEY1_8		((ADC10_04 + ADC10_00)/2)

#define SET_AD_IO(i,x)       {JL_PORT##i->PD &= ~BIT(x), JL_PORT##i->PU &= ~BIT(x), JL_PORT##i->DIR |= BIT(x), JL_PORT##i->DIE &= ~BIT(x);}


static const u16 ad_key_table[] = {
    ADKEY1_0, ADKEY1_1, ADKEY1_2, ADKEY1_3, ADKEY1_4,
    ADKEY1_5, ADKEY1_6, ADKEY1_7, ADKEY1_8
};


static const u8 ad_roundbin_table[] = {
    AD_CHANNEL_SEL_PA3,
    AD_CHANNEL_SEL_LDOIN,
    AD_CHANNEL_SEL_LDOREF,
};

struct adc_handle {

    u8 next_channel;

    u16 value[ARRAY_SIZE(ad_roundbin_table)];
};

static struct adc_handle hdl;

#define __this  (&hdl)

/**
 * @brief __key_ad_init
 */

static void ad_key_init(void)
{
    s32 ret;
    key_puts("app initcall - ad key init\n");

    //IO setting
    SET_AD_IO(A, 3);

    //ADC setting
    __adc_drv->ioctrl(SET_ADC_LVD_EN, 1);
    __adc_drv->ioctrl(SET_ADC_LVD_CH, 1);
    SFR(JL_AUDIO->ADA_CON0, 22, 1, 0b1);
    SFR(JL_AUDIO->ADA_CON0, 23, 3, 0b110);
    __adc_drv->on(ad_roundbin_table[0]);
}

static void __adc_roundrobin(void)
{
    static u8 channel = 0;

    ad_key_init();

    if (channel == 0) {
        /* printf("ADC res %x \n", __this->value[channel]); */
    }
    __adc_drv->ioctrl(GET_ADC_VALUE, &__this->value[channel]);

    channel = (channel >= ARRAY_SIZE(ad_roundbin_table) - 1) ? 0 : channel + 1;

    __adc_drv->ioctrl(SET_ADC_CHANNEL, ad_roundbin_table[channel]);

}

/**
 * @brief ad_key_get_number
 *
 * @return
 */
static u8 ad_key_get_number(void)
{
    u8 idx;
    u32 key_value;

    __adc_roundrobin();

    key_value = __this->value[0];

    if (key_value > AD_NOKEY) {
        return KEY_NUMBER_INVALID;
    }

    for (idx = 0; idx < ARRAY_SIZE(ad_key_table); idx++) {
        if (key_value > ad_key_table[idx]) {
            break;
        }
    }

    /* printf("adkey_value:%d   key_num:0x%x\n",key_value, idx); */

    return idx;
}

/**
 * @brief get_battery_level
 *
 * @return
 */
static u16 get_battery_level(void)
{
    u16 battery_value, LDOIN_12, LDO_ref;
    LDOIN_12 = __this->value[1];
    LDO_ref  = __this->value[2];//0x181,1.2v

    battery_value = (((u16)LDOIN_12 * 374) / 10) / ((u16)LDO_ref); //针对AC69 VDDIO3.3V偏高设定
    /* battery_value = (((u16)LDOIN_12*368)/10)/((u16)LDO_ref); */
    /* printf("battery_value:0x%x    0x%x   %d\n",LDOIN_12,LDO_ref,battery_value); */
    return battery_value;
}

static void ad_key_ctrl(int ctrl, ...)
{
    va_list argptr;
    va_start(argptr, ctrl);

    switch (ctrl) {
    case GET_SYS_VOLTAGE:
        *((u16 *)va_arg(argptr, int)) = get_battery_level();
        break;
    default:
        break;
    }

    va_end(argptr);
}



static const struct key_driver key_drv_ad = {
    .init               = ad_key_init,

    .get_key_number     = ad_key_get_number,

    .ioctrl             = ad_key_ctrl,
};
REGISTER_KEY_ADC_DRIVER(key_drv_ad);

