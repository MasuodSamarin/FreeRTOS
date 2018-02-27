/*********************************************************************************************
    *   Filename        : clock_driver.c

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2017-09-07 10:05

    *   Copyright:(c)JIELI  2011-2017  @ , All Rights Reserved.
*********************************************************************************************/
#include "clock_hw.h"
#include "clock_interface.h"
/* #include "rtc_driver.h" */
#include "uart.h"


/* #define CLOCK_DEBUG */

#ifdef CLOCK_DEBUG
#define clock_puts              puts
#define clock_printf            printf
#define clock_put8              put_u8hex0
#define clock_put16             put_u16hex
#define clock_put32             put_u32hex
#else
#define clock_puts(...)
#define clock_printf(...)
#define clock_put8
#define clock_put16
#define clock_put32
#endif/*CLOCK_DEBUG*/


/********************************************************************************/
/*
 *          --------时钟工作模式配置
 *
 *                  P.S 仅内部开发人员配置
 */
///<PLL 差分输入使能
#define PLL_DIFF_ENABLE

///<PLL 分频模式选择(PLL_DIVIDER_INTE/PLL_DIVIDER_FRAC)
#define PLL_DIVIDER     PLL_DIVIDER_INTE

///<PLL 后分频APC 固定时钟频率(~-96MHz)
#define APC_Hz		    96000000L	            //<-根据运算能力可调整

///<PLL 后分频ALNK 固定时钟频率(12.288/11.2896MHz)
#define ALNK_Hz		    ((192 * 1000000L) / 17)  //<-根据运算能力可调整

///<配置外设USB时钟(USB_CLOCK_IN_DISABLE/USB_CLOCK_IN_PLL48M/USB_CLOCK_IN_LSB)
#define USB_SRC_CLK     USB_CLOCK_IN_PLL48M

///<配置外设DAC时钟(DAC_CLOCK_IN_DISABLE/DAC_CLOCK_IN_PLL24M/DAC_CLOCK_IN_LSB)
#define DAC_SRC_CLK     DAC_CLOCK_IN_PLL24M

///<配置外设APC时钟(APC_CLOCK_IN_DISABLE/APC_CLOCK_IN_PLL64M/APC_CLOCK_IN_PLLAPC/APC_CLOCK_IN_LSB)
#define APC_SRC_CLK     APC_CLOCK_IN_PLLAPC

///<配置外设UART时钟(UART_CLOCK_IN_DISABLE/UART_CLOCK_IN_OSC/UART_CLOCK_IN_PLL48M/UART_CLOCK_IN_LSB)
#define UART_SRC_CLK    UART_CLOCK_IN_OSC

///<配置外设BT时钟(BT_CLOCK_IN_DISABLE/BT_CLOCK_IN_PLL64M/BT_CLOCK_IN_LSB)
#define BT_SRC_CLK      BT_CLOCK_IN_PLL64M

///<配置SFR 时钟(SFR_CLOCK_IDLE/SFR_CLOCK_ALWAYS_ON)
#define SFR_CLK_MODE    SFR_CLOCK_IDLE

///<配置Memory 时钟(MEM_CLOCK_IDLE/MEM_CLOCK_ALWAYS_ON)
#define MEM_CLK_MODE    MEM_CLOCK_IDLE

///<配置数字部分电压(DVDD) 0~7
#define DVDD_LEVEL_SET  3

/********************************************************************************/

#define DVDD_LEVEL(x)  SFR(JL_SYSTEM->LDO_CON0, 12, 3, x); \
                        SFR(JL_SYSTEM->LDO_CON0, 21, 3, x);

#define MHz_UNIT	1000000L
#define KHz_UNIT	1000L

#define IO_DEBUG_OUT(i,x)       {JL_PORT##i->HD &= ~BIT(x), JL_PORT##i->DIR &= ~BIT(x);}

#define FREQ_BY_DIV(freq, div)    (freq/(div+1))

struct __clock_hdl {
    u8  pll_sys_table;
    u8  pll_sys_div;
    u32 pll_sys_clk;

    u8  apc_table;
    u8  apc_div;
    u32 apc_clk;

    u8  alnk_div;
    u32 alnk_clk;
    u32 osc_clk;

    u8  iosc_port;
    u32 iosc_clk;

    u16 hsb_div;
    u32 hsb_clk;    //cpu clk

    u8  lsb_div;
    u32 lsb_clk;

    u8  sfc_div;
    u32 sfc_clk;

    u8  otp_div;
    u32 otp_clk;
};

static struct __clock_hdl hdl;

#define __this      (&hdl)


//define
enum {
    DVDD_VOLTAGE_10V = 0,
    DVDD_VOLTAGE_11V,
    DVDD_VOLTAGE_12V,
};

enum {
    LIMIT_SFC_CLK = 0,
    LIMIT_HSB_CLK,
    LIMIT_LSB_CLK,
    LIMIT_OTP_CLK,
};

const u32 clock_limit[][4] = {
    //SFC          |  HSB           | LSB          | OTP
    {96 * 1000000L,   96 * 1000000L, 32 * 1000000L, 24 * 1000000L},
    {160 * 1000000L, 160 * 1000000L, 32 * 1000000L, 30 * 1000000L},
    {192 * 1000000L, 192 * 1000000L, 32 * 1000000L, 30 * 1000000L},
};

const u32 pll_clk480_tab[] = {
    (480 * 1000000L) / 1 / 1,
    (480 * 1000000L) / 1 / 3,
    (480 * 1000000L) / 1 / 5,
    (480 * 1000000L) / 1 / 7,

    (480 * 1000000L) / 2 / 1,
    (480 * 1000000L) / 2 / 3,
    (480 * 1000000L) / 2 / 5,
    (480 * 1000000L) / 2 / 7,

    (480 * 1000000L) / 4 / 1,
    (480 * 1000000L) / 4 / 3,
    (480 * 1000000L) / 4 / 5,
    (480 * 1000000L) / 4 / 7,

    (480 * 1000000L) / 8 / 1,
    (480 * 1000000L) / 8 / 3,
    (480 * 1000000L) / 8 / 5,
    (480 * 1000000L) / 8 / 7,
};

const u32 pll_clk192_tab[] = {
    (192 * 1000000L) / 1 / 1,
    (192 * 1000000L) / 1 / 3,
    (192 * 1000000L) / 1 / 5,
    (192 * 1000000L) / 1 / 7,

    (192 * 1000000L) / 2 / 1,
    (192 * 1000000L) / 2 / 3,
    (192 * 1000000L) / 2 / 5,
    (192 * 1000000L) / 2 / 7,

    (192 * 1000000L) / 4 / 1,
    (192 * 1000000L) / 4 / 3,
    (192 * 1000000L) / 4 / 5,
    (192 * 1000000L) / 4 / 7,

    (192 * 1000000L) / 8 / 1,
    (192 * 1000000L) / 8 / 3,
    (192 * 1000000L) / 8 / 5,
    (192 * 1000000L) / 8 / 7,
};

const u32 pll_clk137_tab[] = {
    (137 * 1000000L) / 1 / 1,
    (137 * 1000000L) / 1 / 3,
    (137 * 1000000L) / 1 / 5,
    (137 * 1000000L) / 1 / 7,

    (137 * 1000000L) / 2 / 1,
    (137 * 1000000L) / 2 / 3,
    (137 * 1000000L) / 2 / 5,
    (137 * 1000000L) / 2 / 7,

    (137 * 1000000L) / 4 / 1,
    (137 * 1000000L) / 4 / 3,
    (137 * 1000000L) / 4 / 5,
    (137 * 1000000L) / 4 / 7,

    (137 * 1000000L) / 8 / 1,
    (137 * 1000000L) / 8 / 3,
    (137 * 1000000L) / 8 / 5,
    (137 * 1000000L) / 8 / 7,
};


const u32 alnk_clk_tab[] = {
    (192 * 1000000L) / 17,
    (480 * 1000000L) / 39,
};


//global var
extern void delay(u32 d);

/********************************************************************************/
/*
 *                   HW Sfr Layer
 */
static void __hw_bt_osc_enable(void)
{
    /*-TODO-*/

}

static void __hw_bt_osc_disable()
{
    /*-TODO-*/

}

static void __hw_rtoscl_enable(void)
{
    /*-TODO-*/

}

static void __hw_rtoscl_disable(void)
{
    /*-TODO-*/

}

static void __hw_rtosch_enable(void)
{
    /*-TODO-*/

}

static void __hw_rtosh_disable(void)
{
    /*-TODO-*/

}


static void __hw_rc_enable(void)
{
    RC_EN(1); //RC en
    delay(100);
}

static void __hw_rc_disable(void)
{
    RC_EN(0);
}

static void __hw_lrc_enable(void)
{
    /*-TODO-*/

}


static void __hw_pll_enable()
{
    //reset
    clock_puts("HAL - pll_enable\n");
    PLL_EN(1);
    delay(1);	    //>10us
    PLL_REST(1);    //复位释放(需在PLL EN使能10us后才能释放)
    delay(10 + 10); //>300us
}

static void __hw_pll_disable(void)
{
    clock_puts("HAL - pll_disable\n");
    PLL_EN(0);
    PLL_REST(0); //复位
}

static u8 get_dvdd(void)
{
    u8 dvdd;

    dvdd = (JL_SYSTEM->LDO_CON0 & (BIT(12) | BIT(13) | BIT(14))) >> 12;
    //
    if (dvdd <= 3) {
        dvdd = DVDD_VOLTAGE_12V;
    } else if (dvdd <= 5) {
        dvdd = DVDD_VOLTAGE_11V;
    } else {
        dvdd = DVDD_VOLTAGE_10V;
    }

    return dvdd;
}

static u16 __hw_hsb_clk_limit(u32 frequency)
{
    u16 div;

    u32 hsb_clk_limit = clock_limit[get_dvdd()][LIMIT_HSB_CLK];

    clock_printf("--func = %s\n", __FUNCTION__);
    for (div = 0; div < 256; div++) {
        if ((frequency / (div + 1)) <= hsb_clk_limit) {
            break;
        }
    }

    return div;
}


static u8 __hw_lsb_clk_limit(u32 frequency)
{
    u8 div;

    u32 lsb_clk_limit = clock_limit[get_dvdd()][LIMIT_LSB_CLK];

    clock_printf("--func = %s\n", __FUNCTION__);
    for (div = 0; div < 8; div++) {
        if ((frequency / (div + 1)) <= lsb_clk_limit) {
            break;
        }
    }

    return div;
}

__attribute__((noinline))
AT(.volatile_ram_code)
static void __hw_sfc_clk_div(u8 baud)
{
    OS_ENTER_CRITICAL();

    while (!(JL_DSP->CON & BIT(5)));
    JL_SFC->CON &= ~BIT(0);
    JL_SFC->BAUD = baud;
    JL_SFC->CON |= BIT(0);

    OS_EXIT_CRITICAL();
}

static u8 __hw_sfc_clk_limit(u32 frequency)
{
    tu8 div;

    u32 sfc_clk_limit = clock_limit[get_dvdd()][LIMIT_SFC_CLK];

    clock_printf("--func = %s\n", __FUNCTION__);
    for (div = 0; div < 256; div++) {
        if ((frequency / (div + 1)) <= sfc_clk_limit) {
            break;
        }
    }


    //__hw_sfc_clk_div(3);

    return div;
}

static u8 __hw_otp_clk_limit(u32 frequency)
{
    tu8 div;

    u32 otp_clk_limit = clock_limit[get_dvdd()][LIMIT_OTP_CLK];

    clock_printf("--func = %s\n", __FUNCTION__);
    for (div = 0; div < 256; div++) {
        if ((frequency / (div + 1)) <= otp_clk_limit) {
            break;
        }
    }

    while (!(JL_DSP->CON & BIT(5)));
    //delay = cnt / 24MHz > 10us
    JL_OTP->OPENCNT = 240;
    JL_OTP->CLOSECNT = 240;
    JL_CRC->REG = 0x7ea4;
    JL_OTP->CMD2 = 0;
    JL_CRC->REG = 0;

    return div;
}

//source pll clk ->/ [div] /->sfc clk ->/ [div] /->hsb clk ->/ [div] /-> lsb clk
//                                                         |
//                                                         ->/ [div] /-> otp clk
static void clock_all_limit_pre(u32 frequency)
{
    u8 div;

    ASSERT(frequency, "%s\n", __func__);

    div = __hw_sfc_clk_limit(frequency);
    __this->sfc_div = div;
    __this->sfc_clk = FREQ_BY_DIV(frequency, div);

    frequency = __this->sfc_clk;
    div = __hw_hsb_clk_limit(frequency);
    __this->hsb_div = div;
    __this->hsb_clk = FREQ_BY_DIV(frequency, div);

    frequency = __this->hsb_clk;
    div = __hw_lsb_clk_limit(frequency);
    __this->lsb_div = div;
    __this->lsb_clk = FREQ_BY_DIV(frequency, div);

    div = __hw_otp_clk_limit(frequency);
    __this->otp_div = div;
    __this->otp_clk = FREQ_BY_DIV(frequency, div);
}

static void clock_all_limit_post()
{
    if ((__this->pll_sys_clk != -1)
        || (__this->osc_clk != 1)) {

        __hw_sfc_clk_div(__this->sfc_div);
        HSB_CLK_DIV(__this->hsb_div);
        LSB_CLK_DIV(__this->lsb_div);
        OTP_CLK_DIV(__this->otp_div);
    }
}


static void __hw_clock_debug_PB0(u8 out)
{
    if (out == PB0_CLOCK_OUTPUT) {
        PB0_CLOCK_OUT(PB0_CLOCK_OUTPUT);
    } else {
        PB0_CLOCK_OUT(out);
        IO_DEBUG_OUT(B, 0);
    }
}

static void __hw_clock_debug_PA2(u8 out)
{
    if (out == PA2_CLOCK_OUTPUT) {
        PA2_CLOCK_OUT(PA2_CLOCK_OUTPUT);
    } else {
        PA2_CLOCK_OUT(out);
        IO_DEBUG_OUT(A, 2);
    }
}


static bool __hw_pll_sys_clk_out_pre(u32 frequency)
{
    u8 index;

    clock_printf("--func = %s\n", __FUNCTION__);
    clock_printf("pll sys frequency : 0x%d\n", frequency);

    for (index = 0; index < ARRAY_SIZE(pll_clk137_tab); index++) {
        if (frequency == pll_clk137_tab[index]) {
            __this->pll_sys_table = PLL_SYS_SEL_PLL137M;
            __this->pll_sys_div = index;
            clock_printf("pll_clk137_tab : 0x%d\n", index);
            __this->pll_sys_clk = pll_clk137_tab[index];
            return TRUE;
        }
    }
    for (index = 0; index < ARRAY_SIZE(pll_clk192_tab); index++) {
        if (frequency == pll_clk192_tab[index]) {
            __this->pll_sys_table = PLL_SYS_SEL_PLL192M;
            __this->pll_sys_div = index;
            clock_printf("pll_clk192_tab : 0x%d\n", index);
            __this->pll_sys_clk = pll_clk192_tab[index];
            return TRUE;
        }
    }
    for (index = 0; index < ARRAY_SIZE(pll_clk480_tab); index++) {
        if (frequency == pll_clk480_tab[index]) {
            __this->pll_sys_table = PLL_SYS_SEL_PLL480M;
            __this->pll_sys_div = index;
            clock_printf("pll_clk480_tab : 0x%d\n", index);
            __this->pll_sys_clk = pll_clk480_tab[index];
            return TRUE;
        }
    }

    __this->pll_sys_clk = -1;
    return FALSE;
}

static void __hw_pll_sys_clk_out_post(void)
{
    if (__this->pll_sys_clk == -1) {
        return;
    }

    PLL_SYS_SEL(__this->pll_sys_table);
    PLL_SYS_DIV(__this->pll_sys_div);
}

static bool __hw_alnk_clk_out_pre(u32 frequency)
{
    u8 index;

    clock_printf("--func = %s\n", __FUNCTION__);
    clock_printf("\n alnk frequency : 0x%d\n", frequency);

    for (index = 0; index < ARRAY_SIZE(alnk_clk_tab); index++) {
        if (frequency == alnk_clk_tab[index]) {
            __this->alnk_div = index;
            clock_printf("\n alnk_clk_tab : 0x%d\n", index);
            __this->alnk_clk = alnk_clk_tab[index];
            return TRUE;
        }
    }

    __this->alnk_clk = -1;
    return FALSE;
}

static void __hw_alnk_clk_out_post(void)
{
    if (__this->alnk_clk == -1) {
        return;
    }

    PLL_ALNK_SEL(__this->alnk_div);
}


static bool __hw_apc_clk_out_pre(u32 frequency)
{
    u8 index;

    clock_printf("--func = %s\n", __FUNCTION__);
    clock_printf("\n apc frequency : 0x%d\n", frequency);

    for (index = 0; index < ARRAY_SIZE(pll_clk137_tab); index++) {
        if (frequency == pll_clk137_tab[index]) {
            __this->apc_table = PLL_APC_SEL_PLL137M;
            __this->apc_div = index;
            clock_printf("pll_clk137_tab: 0x%d\n", index);
            __this->apc_clk = pll_clk137_tab[index];
            return TRUE;
        }
    }
    for (index = 0; index < ARRAY_SIZE(pll_clk192_tab); index++) {
        if (frequency == pll_clk192_tab[index]) {
            __this->apc_table = PLL_APC_SEL_PLL192M;
            __this->apc_div = index;
            clock_printf("pll_clk192_tab : 0x%d\n", index);
            __this->apc_clk = pll_clk192_tab[index];
            return TRUE;
        }
    }

    __this->apc_clk = -1;
    return FALSE;
}

static void __hw_apc_clk_out_post(void)
{
    if (__this->apc_clk == -1) {
        return;
    }

    PLL_APC_SEL(__this->apc_table);
    PLL_APC_DIV(__this->apc_div);
}

static void __hw_pll_all_oe(void)
{
    bool pll_clk480_oe = 0;

    pll_clk480_oe |= ((JL_CLOCK->CLK_CON2 & (BIT(0) | BIT(1))) == PLL_SYS_SEL_PLL480M) ? 1 : 0;
    pll_clk480_oe |= ((JL_CLOCK->CLK_CON2 & (BIT(30)) >> 30) == PLL_ALNK_480M_DIV39) ? 1 : 0;

    bool pll_clk137_oe = 0;

    pll_clk137_oe |= ((JL_CLOCK->CLK_CON2 & (BIT(0) | BIT(1))) == PLL_SYS_SEL_PLL137M) ? 1 : 0;
    pll_clk137_oe |= ((JL_CLOCK->CLK_CON2 & (BIT(18) | BIT(19)) >> 18) == PLL_APC_SEL_PLL137M) ? 1 : 0;

    PLL_CLK320M_OE(0);
    PLL_CLK107M_OE(0);
    PLL_CLK192M_OE(1);
    PLL_CLK480M_OE(pll_clk480_oe);
    PLL_CLK137M_OE(pll_clk137_oe);
}

static void __hw_pll_run_pre(u32 input_freq, u32 out_freq)
{
    __hw_pll_sys_clk_out_pre(out_freq);

    __hw_apc_clk_out_pre(APC_Hz);

    __hw_alnk_clk_out_pre(ALNK_Hz);
}

static void __hw_pll_run_post(u32 input_freq)
{
    u32 frequency;

    clock_printf("--func = %s\n", __FUNCTION__);
    clock_printf("PLL in freq : 0x%d\n", input_freq);

    /****************Analog**********************/
    PLL_PFD(1);
    PLL_ICP(2);
    PLL_LPFR2(0);
    PLL_LPFR3(0);

    PLL_LDO_BYPASS(0);
    PLL_LD012A(1);
    PLL_LDO12D(1);
    PLL_IVCO(3);
    /****************Analog end**********************/

    //<关闭 PLL
    __hw_pll_disable();

    //<设置 PLL 参考源和参考频率
    PLL_DIVSEL(PLL_DIVIDER);
#if (PLL_DIVIDER == PLL_DIVIDER_INTE)
    //Fout = Fref / DIVn * DIVm
    ASSERT((input_freq == 12 * MHz_UNIT) || (input_freq == 24 * MHz_UNIT), "%s\n, __func__");
    PLL_DIVn_EN(0);
    PLL_DIVn(0);    //DIVn
    PLL_DIVm((480 * MHz_UNIT / input_freq) - 2); //DIVm
#elif (PLL_DIVIDER == PLL_DIVIDER_FRAC)
    /*-TODO-*/
    PLL_FRAC(20 << 24 + 5592405);
    PLL_DMAX(3355443);
    PLL_DMIN(-3355443);
    PLL_DSTP(33554);
    PLL_DIVn_EN(0);
    PLL_DSMS(0);
    PLL_DSM_TSEL(0b1110);
    PLL_DSM_RSEL(0b00);
    PLL_DSM_MSEL(0b00);
#endif

    //<设置 PLL 后分频系数
    __hw_pll_sys_clk_out_post();

    __hw_apc_clk_out_post();

    __hw_alnk_clk_out_post();

    __hw_pll_all_oe();

    //<使能 PLL
    __hw_pll_enable();
}



/********************************************************************************/
/*
 *                   HW Abstract Layer
 */

static void sys_internal_clock_by_osc(SYS_ICLOCK_INPUT input, u32 input_freq)
{
    switch (input) {
    case SYS_ICLOCK_INPUT_BTOSC:
        __hw_bt_osc_enable();
        OSC_CLOCK_IN(OSC_CLOCK_IN_BT_OSC);
        break;
    case SYS_ICLOCK_INPUT_RTOSCH:
        __hw_rtosch_enable();
        OSC_CLOCK_IN(OSC_CLOCK_IN_RTOSC_H);
        break;
    case SYS_ICLOCK_INPUT_RTOSCL:
        __hw_rtoscl_enable();
        OSC_CLOCK_IN(OSC_CLOCK_IN_RTOSC_L);
        break;
    case SYS_ICLOCK_INPUT_PAT:
        OSC_CLOCK_IN(OSC_CLOCK_IN_PAT);
        break;
    }

    __this->iosc_port = input;
    __this->iosc_clk = input_freq;
}

static void sys_clock_by_osc_pre(SYS_CLOCK_INPUT input, u32 input_freq, u32 out_freq)
{
    u16 div;

    div = input_freq % out_freq;
    if (!div) {
        div = (input_freq / out_freq) - 1;
        if (div <= 256) {
            __this->osc_clk = out_freq;
            clock_all_limit_pre(__this->osc_clk);
            /* __this->hsb_div = div; */
            /* __this->hsb_clk = FREQ_BY_DIV(out_freq, div); */
            return;
        }
    }

    __this->osc_clk = -1;

    return;
}


static void sys_clock_by_osc_post(SYS_CLOCK_INPUT input, u32 input_freq, u32 out_freq)
{
    clock_all_limit_post();

    switch (input) {
    case SYS_CLOCK_INPUT_BT_OSC:
        __hw_bt_osc_enable();
        MAIN_CLOCK_SEL(MAIN_CLOCK_IN_BTOSC);
        break;
    case SYS_CLOCK_INPUT_RTOSCL:
        __hw_rtoscl_enable();
        MAIN_CLOCK_SEL(MAIN_CLOCK_IN_RTOSC_L);
        break;
    case SYS_CLOCK_INPUT_RTOSCH:
        __hw_rtosch_enable();
        MAIN_CLOCK_SEL(MAIN_CLOCK_IN_RTOSC_H);
        break;
    case SYS_CLOCK_INPUT_PAT:
        TEST_SEL(1);
        break;
    default:
        break;
    }
}

static void sys_clock_by_pll_pre(SYS_CLOCK_INPUT input, u32 input_freq, u32 out_freq)
{
    __hw_pll_run_pre(input_freq, out_freq);

    if (__this->pll_sys_clk == -1) {
        return;
    }

    clock_all_limit_pre(__this->pll_sys_clk);
}

static void sys_clock_by_pll_post(SYS_CLOCK_INPUT input, u32 input_freq, u32 out_freq)
{
    switch (input) {
    case SYS_CLOCK_INPUT_PLL_RTOSCH:
        __hw_rtosch_enable();
#ifdef PLL_DIFF_ENABLE
        PLL_RSEL(PLL_RSEL_RTOSC_DIFF);
#else
        PLL_RSEL(PLL_RSEL_PLL_REF_SEL);
        PLL_REF_SEL(PLL_REF_SEL_RTOSC);
#endif
        break;
    case SYS_CLOCK_INPUT_PLL_BT_OSC:
        __hw_bt_osc_enable();
#ifdef PLL_DIFF_ENABLE
        PLL_RSEL(PLL_RSEL_BTOSC_DIFF);
#else
        PLL_RSEL(PLL_RSEL_PLL_REF_SEL);
        PLL_REF_SEL(PLL_REF_SEL_BTOSC);
#endif
        break;
    case SYS_CLOCK_INPUT_PLL_PAT:
        PLL_RSEL(PLL_RSEL_PLL_REF_SEL);
        PLL_REF_SEL(PLL_REF_SEL_PAT);
        break;
    default:
        break;
    }

    __hw_pll_run_post(input_freq);

    clock_all_limit_post();

    MAIN_CLOCK_SEL(MAIN_CLOCK_IN_PLL);
}

static void sys_clock_by_pll_re_run(u32 frequency)
{
    __hw_pll_sys_clk_out_pre(frequency);

    if (__this->pll_sys_clk == -1) {
        return;
    }

    clock_all_limit_pre(__this->pll_sys_clk);

    OS_ENTER_CRITICAL();

    TEST_SEL(0);

    switch (__this->iosc_port) {
    case SYS_ICLOCK_INPUT_BTOSC:
        MAIN_CLOCK_SEL(MAIN_CLOCK_IN_BTOSC);
        break;
    case SYS_ICLOCK_INPUT_RTOSCH:
        MAIN_CLOCK_SEL(MAIN_CLOCK_IN_RTOSC_H);
        break;
    }

    __hw_pll_sys_clk_out_post();

    __hw_pll_all_oe();

    clock_all_limit_post();

    MAIN_CLOCK_SEL(MAIN_CLOCK_IN_PLL);

    OS_EXIT_CRITICAL();
}

static void clock_run(SYS_CLOCK_INPUT input, u32 input_freq, u32 out_freq)
{
    /* ASSERT(input_freq <= out_freq, "%s\n", __func__); */

    //pre
    switch (input) {
    //<- (原生时钟源)
    case SYS_CLOCK_INPUT_BT_OSC:
    case SYS_CLOCK_INPUT_RTOSCL:
    case SYS_CLOCK_INPUT_RTOSCH:
    case SYS_CLOCK_INPUT_PAT:
        sys_clock_by_osc_pre(input, input_freq, out_freq);
        break;
    //<- (衍生时钟源)
    case SYS_CLOCK_INPUT_PLL_RTOSCH:
    case SYS_CLOCK_INPUT_PLL_BT_OSC:
    case SYS_CLOCK_INPUT_PLL_PAT:
        sys_clock_by_pll_pre(input, input_freq, out_freq);
        break;
    default:
        break;
    }

    OS_ENTER_CRITICAL();

    TEST_SEL(0);
    //current using OSC / PLL for system clock
    __hw_rc_enable();

    MAIN_CLOCK_SEL(MAIN_CLOCK_IN_RC);

    DVDD_LEVEL(DVDD_LEVEL_SET);

    //post
    switch (input) {
    //<- (原生时钟源)
    case SYS_CLOCK_INPUT_BT_OSC:
    case SYS_CLOCK_INPUT_RTOSCL:
    case SYS_CLOCK_INPUT_RTOSCH:
    case SYS_CLOCK_INPUT_PAT:
        sys_clock_by_osc_post(input, input_freq, out_freq);
        break;
    //<- (衍生时钟源)
    case SYS_CLOCK_INPUT_PLL_RTOSCH:
    case SYS_CLOCK_INPUT_PLL_BT_OSC:
    case SYS_CLOCK_INPUT_PLL_PAT:
        sys_clock_by_pll_post(input, input_freq, out_freq);
        break;
    default:
        ASSERT((input == SYS_CLOCK_INPUT_RC), "%s\n", __func__);
        break;
    }

    OS_EXIT_CRITICAL();
}

/********************************************************************************/
/*
 *                   API Layer
 */
void hw_clock_switching(u32 out_freq)
{
    if (__this->pll_sys_clk) {
        sys_clock_by_pll_re_run(out_freq);
    }
}

void hw_clock_init(u8 sys_in, u32 input_freq, u32 out_freq)
{
    memset(__this, 0, sizeof(struct __clock_hdl)); //clear var

    USB_CLOCK_IN(USB_CLOCK_IN_DISABLE);
    DAC_CLOCK_IN(DAC_CLOCK_IN_DISABLE);
    APC_CLOCK_IN(APC_CLOCK_IN_DISABLE);
    UART_CLOCK_IN(UART_CLOCK_IN_DISABLE);
    BT_CLOCK_IN(BT_CLOCK_IN_DISABLE);

    clock_run(sys_in, input_freq, out_freq);   //run clock

    switch (sys_in) {
    //<- (原生时钟源)
    case SYS_CLOCK_INPUT_RTOSCL:
        sys_internal_clock_by_osc(SYS_ICLOCK_INPUT_RTOSCL, input_freq);
        break;
    case SYS_CLOCK_INPUT_RTOSCH:
    case SYS_CLOCK_INPUT_PLL_RTOSCH:
        sys_internal_clock_by_osc(SYS_ICLOCK_INPUT_RTOSCH, input_freq);
        break;
    //<- (衍生时钟源)
    case SYS_CLOCK_INPUT_BT_OSC:
    case SYS_CLOCK_INPUT_PLL_BT_OSC:
        sys_internal_clock_by_osc(SYS_ICLOCK_INPUT_BTOSC, input_freq);
        break;
    case SYS_CLOCK_INPUT_PAT:
    case SYS_CLOCK_INPUT_PLL_PAT:
        sys_internal_clock_by_osc(SYS_ICLOCK_INPUT_PAT, input_freq);
        break;
    default:
        break;
    }
    //Additional - Default
    USB_CLOCK_IN(USB_SRC_CLK);
    DAC_CLOCK_IN(DAC_SRC_CLK);
    APC_CLOCK_IN(APC_SRC_CLK);
    UART_CLOCK_IN(UART_SRC_CLK);
    BT_CLOCK_IN(BT_SRC_CLK);

    SFR_MODE(SFR_CLK_MODE);
    MEM_CLOCK_STATUS(MEM_CLK_MODE);
}

u32 clock_get_usb_freq(void)
{
    u32 frequency = 0;

    switch (USB_SRC_CLK) {
    case USB_CLOCK_IN_PLL48M:
        frequency = (__this->pll_sys_clk == 0) ? 0 : 48 * MHz_UNIT;
        break;
    case USB_CLOCK_IN_LSB:
        frequency = __this->lsb_clk;
        break;
    }

    return frequency;
}

u32 clock_get_dac_freq(void)
{
    u32 frequency = 0;

    switch (DAC_SRC_CLK) {
    case DAC_CLOCK_IN_PLL24M:
        frequency = (__this->pll_sys_clk == 0) ? 0 : 24 * MHz_UNIT;
        break;
    case DAC_CLOCK_IN_LSB:
        frequency = __this->lsb_clk;
        break;
    }

    return frequency;
}

u32 clock_get_apc_freq(void)
{
    u32 frequency = 0;

    switch (APC_SRC_CLK) {
    case APC_CLOCK_IN_PLL64M:
        frequency = (__this->pll_sys_clk == 0) ? 0 : 64 * MHz_UNIT;
        break;
    case APC_CLOCK_IN_PLLAPC:
        frequency = __this->apc_clk;
        break;
    case APC_CLOCK_IN_LSB:
        frequency = __this->lsb_clk;
        break;
    }

    return frequency;
}

u32 clock_get_uart_freq(void)
{
    u32 frequency = 0;

    switch (UART_SRC_CLK) {
    case UART_CLOCK_IN_OSC:
        frequency = __this->iosc_clk;
        break;
    case UART_CLOCK_IN_PLL48M:
        frequency = (__this->pll_sys_clk == 0) ? 0 : 48 * MHz_UNIT;
        break;
    case UART_CLOCK_IN_LSB:
        frequency = __this->lsb_clk;
        break;
    }

    return frequency;
}

u32 clock_get_bt_freq(void)
{
    u32 frequency = 0;

    switch (BT_SRC_CLK) {
    case BT_CLOCK_IN_PLL64M:
        frequency = (__this->pll_sys_clk == 0) ? 0 : 64 * MHz_UNIT;
        break;
    case BT_CLOCK_IN_LSB:
        frequency = __this->lsb_clk;
        break;
    }

    return frequency;
}

u32 clock_get_sfc_freq(void)
{
    return __this->sfc_clk;
}

u32 clock_get_hsb_freq(void)
{
    return __this->hsb_clk;
}

u32 clock_get_lsb_freq(void)
{
    return __this->lsb_clk;
}

u32 clock_get_otp_freq(void)
{
    return __this->otp_clk;
}

u32 clock_get_iosc_freq(void)
{
    return __this->iosc_clk;
}

u32 clock_get_osc_freq(void)
{
    return __this->osc_clk;
}

void clock_dump(void)
{
    puts("[Clock Dump]\n");
    printf("Internal OSC CLK : %d\n", __this->iosc_clk);
    printf("OSC CLK : %d\n",        __this->osc_clk);

    printf("PLL SYS CLK : %d\n",    __this->pll_sys_clk);
    printf("PLL APC CLK : %d\n",    __this->apc_clk);
    printf("PLL ALNK CLK : %d\n",   __this->alnk_clk);

    printf("-SFC CLK : %d\n",        __this->sfc_clk);
    printf("-HSB CLK : %d\n",        __this->hsb_clk);
    printf("-LSB CLK : %d\n",        __this->lsb_clk);
    printf("-OTP CLK : %d\n",        __this->otp_clk);

    printf("USB CLK : %d\n",        clock_get_usb_freq());
    printf("DAC CLK : %d\n",        clock_get_dac_freq());
    printf("APC CLK : %d\n",        clock_get_apc_freq());
    printf("UART CLK : %d\n",       clock_get_uart_freq());
    printf("BT CLK : %d\n",         clock_get_bt_freq());
}

void clock_debug(void)
{
    puts("---Source BTOSC 24M\n");
    hw_clock_init(SYS_CLOCK_INPUT_BT_OSC, 24000000L, 24000000L);
    clock_dump();

    hw_clock_init(SYS_CLOCK_INPUT_PLL_BT_OSC, 24000000L, 96000000L);

    hw_clock_switching(48000000L);
    puts("---Switching 48M\n");
    clock_dump();
    hw_clock_switching(80000000L);
    puts("---Switching 80M\n");
    clock_dump();

    hw_clock_init(SYS_CLOCK_INPUT_PLL_BT_OSC, 24000000L, 480000000L / 3);
    puts("---480M/3\n");
    clock_dump();
    hw_clock_init(SYS_CLOCK_INPUT_PLL_BT_OSC, 24000000L, 480000000L / 5);
    puts("---480M/5\n");
    clock_dump();
    hw_clock_init(SYS_CLOCK_INPUT_PLL_BT_OSC, 24000000L, 480000000L / 7);
    puts("---480M/7\n");
    clock_dump();
    hw_clock_init(SYS_CLOCK_INPUT_PLL_BT_OSC, 24000000L, 480000000L / 2 / 3);
    puts("---480M/2/3\n");
    clock_dump();
    hw_clock_init(SYS_CLOCK_INPUT_PLL_BT_OSC, 24000000L, 480000000L / 2 / 5);
    puts("---480M/2/5\n");
    clock_dump();
    hw_clock_init(SYS_CLOCK_INPUT_PLL_BT_OSC, 24000000L, 480000000L / 2 / 7);
    puts("---480M/2/7\n");
    clock_dump();
    hw_clock_init(SYS_CLOCK_INPUT_PLL_BT_OSC, 24000000L, 480000000L / 4 / 1);
    puts("---480M/4/1\n");
    clock_dump();
    hw_clock_init(SYS_CLOCK_INPUT_PLL_BT_OSC, 24000000L, 480000000L / 4 / 3);
    puts("---480M/4/3\n");
    clock_dump();
    hw_clock_init(SYS_CLOCK_INPUT_PLL_BT_OSC, 24000000L, 480000000L / 4 / 5);
    puts("---480M/4/5\n");
    clock_dump();
    /* hw_clock_init(SYS_CLOCK_INPUT_PLL_BT_OSC, 24000000L, 480000000L/4/7); */
    puts("---480M/4/7\n");
    /* clock_dump(); */
    hw_clock_init(SYS_CLOCK_INPUT_PLL_BT_OSC, 24000000L, 480000000L / 8 / 1);
    puts("---480M/8/1\n");
    clock_dump();
    /* hw_clock_init(SYS_CLOCK_INPUT_PLL_BT_OSC, 24000000L, 480000000L/8/3); */
    puts("---480M/8/3\n");
    /* clock_dump(); */
    /* hw_clock_init(SYS_CLOCK_INPUT_PLL_BT_OSC, 24000000L, 480000000L/8/5); */
    puts("---480M/8/5\n");
    /* clock_dump(); */
    /* hw_clock_init(SYS_CLOCK_INPUT_PLL_BT_OSC, 24000000L, 480000000L/8/7); */
    puts("---480M/8/7\n");
    /* clock_dump(); */

    hw_clock_init(SYS_CLOCK_INPUT_PLL_BT_OSC, 24000000L, 192000000L / 3);
    puts("---192M/3\n");
    clock_dump();
    hw_clock_init(SYS_CLOCK_INPUT_PLL_BT_OSC, 24000000L, 192000000L / 5);
    puts("---192M/5\n");
    clock_dump();
    hw_clock_init(SYS_CLOCK_INPUT_PLL_BT_OSC, 24000000L, 192000000L / 7);
    puts("---192M/7\n");
    clock_dump();
    hw_clock_init(SYS_CLOCK_INPUT_PLL_BT_OSC, 24000000L, 192000000L / 2 / 1);
    puts("---192M/2/1\n");
    clock_dump();
    hw_clock_init(SYS_CLOCK_INPUT_PLL_BT_OSC, 24000000L, 192000000L / 2 / 3);
    puts("---192M/2/3\n");
    clock_dump();
    /* hw_clock_init(SYS_CLOCK_INPUT_PLL_BT_OSC, 24000000L, 192000000L/2/5); */
    puts("---192M/2/5\n");
    /* clock_dump(); */
    /* hw_clock_init(SYS_CLOCK_INPUT_PLL_BT_OSC, 24000000L, 192000000L/2/7); */
    puts("---192M/2/7\n");
    /* clock_dump(); */
    hw_clock_init(SYS_CLOCK_INPUT_PLL_BT_OSC, 24000000L, 192000000L / 4 / 1);
    puts("---192M/4/1\n");
    clock_dump();
    /* hw_clock_init(SYS_CLOCK_INPUT_PLL_BT_OSC, 24000000L, 192000000L/4/3); */
    puts("---192M/4/3\n");
    /* clock_dump(); */
    /* hw_clock_init(SYS_CLOCK_INPUT_PLL_BT_OSC, 24000000L, 192000000L/4/5); */
    puts("---192M/4/5\n");
    /* clock_dump(); */
    /* hw_clock_init(SYS_CLOCK_INPUT_PLL_BT_OSC, 24000000L, 192000000L/4/7); */
    puts("---192M/4/7\n");
    /* clock_dump(); */
    hw_clock_init(SYS_CLOCK_INPUT_PLL_BT_OSC, 24000000L, 192000000L / 8 / 1);
    puts("---192M/8/1\n");
    clock_dump();
    /* hw_clock_init(SYS_CLOCK_INPUT_PLL_BT_OSC, 24000000L, 192000000L/8/3); */
    puts("---192M/8/3\n");
    /* clock_dump(); */
    /* hw_clock_init(SYS_CLOCK_INPUT_PLL_BT_OSC, 24000000L, 192000000L/8/5); */
    puts("---192M/8/5\n");
    /* clock_dump(); */
    /* hw_clock_init(SYS_CLOCK_INPUT_PLL_BT_OSC, 24000000L, 192000000L/8/7); */
    puts("---192M/8/7\n");
    /* clock_dump(); */

    hw_clock_init(SYS_CLOCK_INPUT_PLL_BT_OSC, 24000000L, 137000000L / 3);
    puts("---137M/3\n");
    clock_dump();
    hw_clock_init(SYS_CLOCK_INPUT_PLL_BT_OSC, 24000000L, 137000000L / 5);
    puts("---137M/5\n");
    clock_dump();
    /* hw_clock_init(SYS_CLOCK_INPUT_PLL_BT_OSC, 24000000L, 137000000L/7); */
    puts("---137M/7\n");
    /* clock_dump(); */
    hw_clock_init(SYS_CLOCK_INPUT_PLL_BT_OSC, 24000000L, 137000000L / 2 / 1);
    puts("---137M/2/1\n");
    clock_dump();
    /* hw_clock_init(SYS_CLOCK_INPUT_PLL_BT_OSC, 24000000L, 137000000L/2/3); */
    puts("---137M/2/3\n");
    /* clock_dump(); */
    /* hw_clock_init(SYS_CLOCK_INPUT_PLL_BT_OSC, 24000000L, 137000000L/2/5); */
    puts("---137M/2/5\n");
    /* clock_dump(); */
    /* hw_clock_init(SYS_CLOCK_INPUT_PLL_BT_OSC, 24000000L, 137000000L/2/7); */
    puts("---137M/2/7\n");
    /* clock_dump(); */
    hw_clock_init(SYS_CLOCK_INPUT_PLL_BT_OSC, 24000000L, 137000000L / 4 / 1);
    puts("---137M/4/1\n");
    clock_dump();
    /* hw_clock_init(SYS_CLOCK_INPUT_PLL_BT_OSC, 24000000L, 137000000L/4/3); */
    puts("---137M/4/3\n");
    /* clock_dump(); */
    /* hw_clock_init(SYS_CLOCK_INPUT_PLL_BT_OSC, 24000000L, 137000000L/4/5); */
    puts("---137M/4/5\n");
    /* clock_dump(); */
    /* hw_clock_init(SYS_CLOCK_INPUT_PLL_BT_OSC, 24000000L, 137000000L/4/7); */
    puts("---137M/4/7\n");
    /* clock_dump(); */
    /* hw_clock_init(SYS_CLOCK_INPUT_PLL_BT_OSC, 24000000L, 137000000L/8/1); */
    puts("---137M/8/1\n");
    /* clock_dump(); */
    /* hw_clock_init(SYS_CLOCK_INPUT_PLL_BT_OSC, 24000000L, 137000000L/8/3); */
    puts("---137M/8/3\n");
    /* clock_dump(); */
    /* hw_clock_init(SYS_CLOCK_INPUT_PLL_BT_OSC, 24000000L, 137000000L/8/5); */
    puts("---137M/8/5\n");
    /* clock_dump(); */
    /* hw_clock_init(SYS_CLOCK_INPUT_PLL_BT_OSC, 24000000L, 137000000L/8/7); */
    puts("---137M/8/7\n");
    /* clock_dump(); */
}

static struct clock_driver clock_drv_ins = {
    .on             = hw_clock_init,
    .off            = NULL,
    .switching      = hw_clock_switching,
};
REGISTER_CLOCK_DRIVER(clock_drv_ins);

