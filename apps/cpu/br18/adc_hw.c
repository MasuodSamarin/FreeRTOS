#include "stdarg.h"
#include "adc_interface.h"
#include "sdk_cfg.h"

static const u32 adc_clk_table[] = {
    128000000L,
    96000000L,
    72000000L,
    48000000L,
    24000000L,
    12000000L,
    6000000L,
    1000000L,
};

static u8 __set_adc_clk(u32 adc_clk_src)
{
    u8 idx;

    //ADC clk config
    for (idx = 0; idx < ARRAY_SIZE(adc_clk_table); idx++) {
        if (adc_clk_src > adc_clk_table[idx]) {
            return (ARRAY_SIZE(adc_clk_table) - idx);
        }
    }

    return (ARRAY_SIZE(adc_clk_table) - 1);
    //printf("ADC_DIV = %d\n",adc_clk_idx);
}

static void __adc_on(u8 channel)
{
    JL_ADC->CON = (0xf << 12);  //Launch Delay Control
    /* JL_ADC->CON |= (channel<<8);       */
    SFR(JL_ADC->CON, 8, 4, channel); //ADC Channel Select
    SFR(JL_ADC->CON, 0, 3, __set_adc_clk(ADC_CLK)); //ADC Channel Select
    JL_ADC->CON |= BIT(3);       //ADC Analog Module Enable
    JL_ADC->CON |= BIT(4);       //ADC Controller Enable
    JL_ADC->CON |= BIT(6);       //Launch ADC
}

static void __adc_off()
{
    JL_ADC->CON = 0;
}



static void __adc_ctrl(u32 ctrl, ...)
{
    va_list argptr;
    va_start(argptr, ctrl);

    switch (ctrl) {
    case SET_ADC_LVD_EN:
        if (va_arg(argptr, int)) {
            SFR(JL_SYSTEM->LDO_CON, 0, 2, 0b11);
        } else {
            SFR(JL_SYSTEM->LDO_CON, 0, 2, 0);//[0]ADC_VBGB_EN [1]ADC_V50_D_EN
        }
        break;
    case SET_ADC_LVD_CH:
        if (va_arg(argptr, int)) {
            SFR(JL_SYSTEM->LDO_CON, 30, 2, 0b10);
        } else {
            SFR(JL_SYSTEM->LDO_CON, 30, 2, 0b01);
        }
        break;
    case SET_ADC_CHANNEL:
        SFR(JL_ADC->CON, 8, 4, va_arg(argptr, int)); //ADC Channel Select
        JL_ADC->CON |= BIT(6);       //Launch ADC
        break;
    case GET_ADC_VALUE:
        while (!(BIT(7) & JL_ADC->CON));	  //wait pending
        /* printf("GET ADC VALUE %x\n", ((u16 *)va_arg(argptr, int))); */
        *((u16 *)va_arg(argptr, int)) = JL_ADC->RES;
        break;
    default:
        break;
    }

    va_end(argptr);
}

static const struct adc_driver adc_drv_ins = {
    .on            = __adc_on,
    .off           = __adc_off,
    .ioctrl        = __adc_ctrl,
};
REGISTER_ADC_DRIVER(adc_drv_ins);
