#include "clock_hw.h"
#include "clock_interface.h"
/* #include "rtc_driver.h" */

//#define CLOCK_DEBUG
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

typedef struct _SYS_CLK_SET {
    void (*rc_protect)(void);
    void (*input)(void);
    void (*io_enable)(bool enable, u32 in_freq);
    u8(*hsb_limit)(u32 out_freq);
    u8(*lsb_limit)(u32 out_freq);
    u32(*run)(u32 in_freq, u32 out_freq);
} SYS_CLK_SET;

typedef struct _CLK_INFO {
    u8 hsb_clk; //<HSB CLK(48 mean 48M)
    u8 lsb_clk; //<LSB CLK(48 mean 48M)
    u8 fm_clk;
    u8 align;
} CLK_INFO;

static CLK_INFO g_clk_info;

#define __this (&g_clk_info)

//define
#define MHZ_UNIT	1000000L
#define KHZ_UNIT	1000L
/********************************************************************************/
/*
 *                   HW Sfr Layer
 */

//define
#define HSB_CLK_MAX		(120 * 1000000L) //120M
#define LSB_CLK_MAX		(60 *  1000000L) //60M

#define PLL_REF_2M		(2 *  1000000L) //2M
#define PLL_REF_12M		(12 * 1000000L) //12M
//global var

extern void delay(u32 d);

/*
 * Clock delay
 */
void osc_delay(u32 in_freq)
{
    //osc delay, under RC
    if (in_freq < 1000000L) {
        delay(30000 + 30000);    //1s + 100%
    } else if (in_freq <= 12000000L) {
        delay(150 + 150);    //5ms + 100%
    } else if (in_freq <= 24000000L) {
        delay(60 + 60);    //2ms + 100%
    } else {
        delay(300 + 300);    //10ms + 100%
    }
}
#define    BT_SFR_ADR              0XEFC00
#define    BTA_CON13               (*(volatile unsigned long *)(BT_SFR_ADR + 0x3d*4))
#define    BTA_CON14               (*(volatile unsigned long *)(BT_SFR_ADR + 0x3e*4))

static void clock_moudle_bt_osc_init(bool enable, u32 in_freq)
{
    clock_printf("--func = %s\n", __FUNCTION__);

#ifndef CLOCK_HAL_DEBUG
    if (enable == TRUE) {
        BT_CLOCK_IN(BT_CLOCK_IN_SYS); //BT_CLK SEL SYS

        osc_delay(in_freq);
        RFI_CON &= ~BIT(1);      //RST BT
        RFI_CON |= BIT(1);
        osc_delay(in_freq);
//	    SFR(BTA_CON14, 1,  3, 2); //BT_OSC_HCS
//      SFR(BTA_CON14, 5, 1,  1); // BTOSC_FM_OE
//		SFR(BTA_CON13, 5, 10, 0); //BT_OSC_CAP
//		BTA_CON13 |= BIT(15);    //BT_OSC_EN
//		BTA_CON14 |= BIT(6);     //BT_OSC_TO_SYS

        SFR(BTA_CON14, 1, 4, 0x8);
        SFR(BTA_CON13, 5, 5, 0x10);
        SFR(BTA_CON13, 10, 5, 0x10);
        SFR(BTA_CON14, 7, 1, 0x1);
        SFR(BTA_CON13, 15, 1, 0x1); //osc en ; no connect should close


        BT_CLOCK_IN(BT_CLOCK_IN_DISABLE_0); //bt clk close for less power
    } else {
        BT_CLOCK_IN(BT_CLOCK_IN_SYS); //BT_CLK SEL SYS

        BTA_CON13 &= ~BIT(15);     //disable bt_xosc
        RFI_CON &= ~BIT(1);        //RST BT

        BT_CLOCK_IN(BT_CLOCK_IN_DISABLE_0); //bt clk close for less power
    }
#endif
}

static void clock_moudle_main_clk_input(tu8 ckmux, tu8 input)
{
    clock_printf("--func = %s\n", __FUNCTION__);
    switch (ckmux) {
    case MAIN_CLOCK_IN_MUX0:
        clock_puts("MAIN_CLOCK_IN_MUX0\n");
        CLOCK_IN_MUX0(input);
        break;
    case MAIN_CLOCK_IN_MUX1:
        clock_puts("MAIN_CLOCK_IN_MUX1\n");
        CLOCK_IN_MUX1(input);
        break;
    default:
        clock_puts("MAIN_CLOCK_IN_MUX_err\n");
        break;
    }

    CLOCK_SEL(ckmux);
}

static void clock_moudle_rc_init(bool enable)
{
    clock_printf("--func = %s\n", __FUNCTION__);

    if (enable == TRUE) {
        RC_EN(1); //RC en
        delay(100);
    } else {
        RC_EN(0);
    }
}

static void rc_clock_protect(void)
{
    clock_printf("--func = %s\n", __FUNCTION__);

    clock_moudle_rc_init(TRUE);
    clock_moudle_main_clk_input(MAIN_CLOCK_IN_MUX0, CLOCK_IN_MUX0_RC_0);
}

static void clock_moudle_rtosc_1pin_init(bool enable, u32 in_freq)
{
#if 0
    clock_printf("--func = %s\n", __FUNCTION__);
    if (enable == TRUE) {
#ifndef CLOCK_HAL_DEBUG
        /*RTOSC port in*/

        __rtc_drv->portr_control(PORTR_CTL_PD, BIT(3), 0);
        __rtc_drv->portr_control(PORTR_CTL_PU, BIT(3), 0);
        __rtc_drv->portr_control(PORTR_CTL_DIR, BIT(3), 1);
        __rtc_drv->portr_control(PORTR_CTL_DIE, BIT(3), 0);
        //	PORTR_PD (PORTR3, 0); //close PR3 PD
        //	PORTR_PU (PORTR3, 0); //close PR3 PU
        //	PORTR_DIR(PORTR3, 1); //PR3 set as input
        //	PORTR_DIE(PORTR3, 0); //PR3 set as analog input
#endif
        if (in_freq == 32768L) {
            /*RTOSC X32K1P enable*/
            RTC_CON_w(0, 2, 3);

            /*RTOSC X32K1P High Current*/
        } else {
            //g_clock_moudle_err = 0x1;
            return;
        }
        osc_delay(in_freq);
    } else {
    }
#endif
}


static void clock_moudle_rtosc_2pin_init(bool enable, u32 in_freq)
{
#if 0
    clock_printf("--func = %s\n", __FUNCTION__);

    if (enable == TRUE) {
#ifndef CLOCK_HAL_DEBUG
        /*RTOSC port in*/
        __rtc_drv->portr_control(PORTR_CTL_PD, BIT(2) | BIT(3), 0);
        __rtc_drv->portr_control(PORTR_CTL_PU, BIT(2) | BIT(3), 0);
        __rtc_drv->portr_control(PORTR_CTL_DIR, BIT(2) | BIT(3), 1);
        __rtc_drv->portr_control(PORTR_CTL_DIE, BIT(2) | BIT(3), 0);
        //	PORTR_PD (PORTR2|PORTR3, 0); //close PR2/3 PD
        //	PORTR_PU (PORTR2|PORTR3, 0); //close PR2/3 PU
        //	PORTR_DIR(PORTR2|PORTR3, 1); //PR2/3 set as input
        //	PORTR_DIE(PORTR2|PORTR3, 0); //PR2/3 set as analog input
#endif
        if (in_freq == 32768L) {
            /*RTOSC X32K2P enable*/
            LDO_CON_w(0, 1, 1);

            /*RTOSC X32K2P High Current*/
            LDO_CON_w(1, 1, 1);
        } else if ((in_freq >= 1000000L)
                   && (in_freq <= 26000000L)) {
            LDO_CON_w(0, 1, 0); //close X32K2P
            if (in_freq > 12000000L) {
                /*RTOSC X24M2P High Current*/
                OSC_CON_w(4, 3, 1);
            } else {
                /*RTOSC X24M2P High Current*/
                OSC_CON_w(4, 3, 0);
            }
            LDO_CON_w(6, 2, 3); //enable X24M2P
        } else {
            //g_clock_moudle_err = 0x2;
            return;
        }
    } else {
    }
#endif
}


#define PLL_OUT_480M    (480 * 1000000L) //480M
#define PLL_OUT_192M    (192 * 1000000L) //192M
const u32 pll_clk480_tab[] = {
    PLL_OUT_480M / 1 / 1,
    PLL_OUT_480M / 1 / 3,
    PLL_OUT_480M / 1 / 5,
    PLL_OUT_480M / 1 / 7,

    PLL_OUT_480M / 2 / 1,
    PLL_OUT_480M / 2 / 3,
    PLL_OUT_480M / 2 / 5,
    PLL_OUT_480M / 2 / 7,

    PLL_OUT_480M / 4 / 1,
    PLL_OUT_480M / 4 / 3,
    PLL_OUT_480M / 4 / 5,
    PLL_OUT_480M / 4 / 7,

    PLL_OUT_480M / 8 / 1,
    PLL_OUT_480M / 8 / 3,
    PLL_OUT_480M / 8 / 5,
    PLL_OUT_480M / 8 / 7,
};

const u32 pll_clk192_tab[] = {
    PLL_OUT_192M / 1 / 1,
    PLL_OUT_192M / 1 / 3,
    PLL_OUT_192M / 1 / 5,
    PLL_OUT_192M / 1 / 7,

    PLL_OUT_192M / 2 / 1,
    PLL_OUT_192M / 2 / 3,
    PLL_OUT_192M / 2 / 5,
    PLL_OUT_192M / 2 / 7,

    PLL_OUT_192M / 4 / 1,
    PLL_OUT_192M / 4 / 3,
    PLL_OUT_192M / 4 / 5,
    PLL_OUT_192M / 4 / 7,

    PLL_OUT_192M / 8 / 1,
    PLL_OUT_192M / 8 / 3,
    PLL_OUT_192M / 8 / 5,
    PLL_OUT_192M / 8 / 7,
};

#define PLL_CLK_480M_TAB_SIZE	sizeof(pll_clk480_tab)/sizeof(u32)
#define PLL_CLK_192M_TAB_SIZE	sizeof(pll_clk192_tab)/sizeof(u32)

/*
 * PLL 后分频 SYS CLK
 */
static void clock_moudle_pll_sys_clk(tu8 src, tu8 div)
{
    PLL_SYS_SEL(src);
    PLL_SYS_DIV(div);
}

const u32 alnk_clk_tab[] = {
    PLL_OUT_192M / 17,
    PLL_OUT_480M / 39,
};

#define ALNK_CLK_TAB_SIZE	sizeof(alnk_clk_tab)/sizeof(u32)

static void clock_moudle_pll_alnk_clk(tu8 src, tu8 div)
{
    PLL_ALNK_DIV(div);
}

static void clock_moudle_pll_rfi_clk(tu8 src, tu8 div)
{
    PLL_RFI_SEL(src);
    PLL_RFI_DIV(div);
}

static void clock_moudle_pll_apc_clk(tu8 src, tu8 div)
{
    PLL_APC_SEL(src);
    PLL_APC_DIV(div);
}

static void pll_input_rtosc(void)
{
    clock_printf("--func = %s\n", __FUNCTION__);
    PLL_RESOURCE(PLL_RESOURCE_RTOSC);
}

static void pll_input_bt_osc(void)
{
    clock_printf("--func = %s\n", __FUNCTION__);
    PLL_RESOURCE(PLL_RESOURCE_BT_OSC);
}

static void pll_input_rc(void)
{
    clock_printf("--func = %s\n", __FUNCTION__);
    PLL_RESOURCE(PLL_RESOURCE_RC);
}

static void pll_input_pat(void)
{
    clock_printf("--func = %s\n", __FUNCTION__);
    PLL_RESOURCE(PLL_RESOURCE_PAT);
}

static void clock_moudle_pll_ref_clk(tu32 input_freq)
{
    tu32 ref_freq;
    tu8  ref_div;
    tu8  ref_clk;

    clock_printf("--func = %s\n", __FUNCTION__);

    PLL_REF_SEL(PLL_REF_SEL_PLL_REF); //PLL 参考时钟源选择

    if (input_freq < PLL_REF_12M) {
        ref_clk = PLL_REF_CLK_2M;
        ref_freq = PLL_REF_2M;
    } else {
        ref_clk = PLL_REF_CLK_12M;
        ref_freq = PLL_REF_12M;
    }
    clock_printf("ref clk : 0x%x\n", ref_clk);
    clock_printf("ref freq : 0x%x\n", ref_freq);

    ref_div = input_freq / ref_freq;
    clock_printf("ref div : 0x%x\n", ref_div);
    if ((input_freq == PLL_REF_2M)
        || (input_freq == PLL_REF_12M)) {
        PLL_REFDS(0);
        PLL_REFDS_EN(0);
        clock_puts("ref 0 \n");
    } else {
        PLL_REFDS(ref_div - 2);
        PLL_REFDS_EN(1);
        clock_puts("ref -2 \n");
    }

    PLL_REF_CLK(ref_clk); //鉴相器选择(2M or 12M)
}

static void clock_moudle_pll_enable(bool enable)
{
    clock_printf("--func = %s\n", __FUNCTION__);
    if (enable == TRUE) {
        clock_puts("HAL - pll_enable\n");
        PLL_EN(1);
        delay(1);	    //>10us
        PLL_REST(1);    //复位释放(需在PLL EN使能10us后才能释放)
        delay(10 + 10); //>300us
    } else {
        clock_puts("HAL - pll_disable\n");
        PLL_EN(0);
        PLL_REST(0); //复位
    }
}

static void clock_moudle_hsb_clk_div(tu8 div)
{
    HSB_CLK_DIV(div);
}

static u8 hsb_limit(u32 out_freq)
{
    tu8 div;

    clock_printf("--func = %s\n", __FUNCTION__);
    if (out_freq > HSB_CLK_MAX) {
        div = (SYS_DIV & 0xFF) + 1; //get hsb div num

        if ((out_freq / div) > HSB_CLK_MAX) {
            for (div = 0; div < 256; div++) {
                if ((out_freq / (div + 1)) <= HSB_CLK_MAX) {
                    break;
                }
            }
            clock_moudle_hsb_clk_div(div);
        }
    }

    div = SYS_DIV & 0xFF;
    clock_printf("hsb_div=0x%x\n", div);

    return div;
}

static void clock_moudle_lsb_clk_div(tu8 div)
{
    LSB_CLK_DIV(div);
}

static u8 lsb_limit(u32 out_freq)
{
    tu8 div;

    clock_printf("--func = %s\n", __FUNCTION__);
    if (out_freq > LSB_CLK_MAX) {
        div = ((SYS_DIV & (0x7 << 8)) >> 8) + 1; //get lsb div num

        if ((out_freq / div) > LSB_CLK_MAX) {
            for (div = 0; div < 8; div++) {
                if ((out_freq / (div + 1)) <= LSB_CLK_MAX) {
                    break;
                }
            }
            clock_moudle_lsb_clk_div(div);
        }
    }

    div = (SYS_DIV & (0x7 << 8)) >> 8;
    clock_printf("lsb_div=0x%x\n", div);

    return div;
}
/*
 *		--- Clock I/O debug ---
 */
static void clock_out_PA4(tu8 out)
{
    if (out == PA4_CLOCK_OUTPUT) {
        PA4_CLOCK_OUT(PA4_CLOCK_OUTPUT);
    } else {
        PA4_CLOCK_OUT(out);
        PORTA_HD |= BIT(4);
        PORTA_DIR &= ~BIT(4);
    }
}

static void clock_out_PA5(tu8 out)
{
    if (out == PA5_CLOCK_OUTPUT) {
        PA5_CLOCK_OUT(PA5_CLOCK_OUTPUT);
    } else {
        PA5_CLOCK_OUT(out);
        PORTA_HD |= BIT(5);
        PORTA_DIR &= ~BIT(5);
    }
}
/*
 * LSB Clock
 */
static u32 get_lsb_div(void)
{
    return (1 + ((SYS_DIV & (0x7 << 8)) >> 8));
}

static void set_lsb_div(u8 div)
{
    div &= 0x07;
    if (div == NULL) {
        return;
    }

    SFR(SYS_DIV, 8, 3, div - 1);
    g_clk_info.lsb_clk = g_clk_info.hsb_clk / div;
}
/*
 * HSB Clock
 */
static u32 get_hsb_div(void)
{
    return (1 + (SYS_DIV & 0xFF));
}

static void set_sys_div(u8 div)
{
    if (div == NULL) {
        return;
    }

    SFR(SYS_DIV, 0, 8, div - 1);
    g_clk_info.hsb_clk /= div;
    g_clk_info.lsb_clk /= div;
}
/********************************************************************************/
/*
 *                   HW API Layer
 */

static void rtosc_clk_1pin_io_enable(bool enable, u32 in_freq)
{
    clock_printf("--func = %s\n", __FUNCTION__);
    clock_moudle_rtosc_1pin_init(enable, in_freq);
}

static void rtosc_clk_2pin_io_enable(bool enable, u32 in_freq)
{
    clock_printf("--func = %s\n", __FUNCTION__);
    clock_moudle_rtosc_2pin_init(enable, in_freq);
}

static void btosc_clk_io_enable(bool enable, u32 in_freq)
{
    clock_printf("--func = %s\n", __FUNCTION__);
    clock_moudle_bt_osc_init(enable, in_freq);
}

static void rc_clk_io_enable(bool enable, u32 in_freq)
{
    clock_printf("--func = %s\n", __FUNCTION__);
    clock_moudle_rc_init(enable);
}


static u32 rtosc_clock_run(u32 input_freq, u32 out_freq)
{
    clock_printf("--func = %s\n", __FUNCTION__);
    clock_moudle_main_clk_input(MAIN_CLOCK_IN_MUX0, CLOCK_IN_MUX0_RTOSC); //2 choice 1 all right
    //clock_moudle_main_clk_input(MAIN_CLOCK_IN_MUX1, CLOCK_IN_MUX1_RTOSC); //2 choice 1 all right

    return input_freq;
}
static u32 bt_osc_clock_run(u32 input_freq, u32 out_freq)
{
    clock_printf("--func = %s\n", __FUNCTION__);
    clock_moudle_main_clk_input(MAIN_CLOCK_IN_MUX1, CLOCK_IN_MUX1_BT_OSC); //2 choice 1 all right
    //clock_moudle_main_clk_input(MAIN_CLOCK_IN_MUX0, CLOCK_IN_MUX0_BT_OSC); //2 choice 1 all right

    return input_freq;
}
static u32 rc_clock_run(u32 input_freq, u32 out_freq)
{
    clock_printf("--func = %s\n", __FUNCTION__);
    clock_moudle_main_clk_input(MAIN_CLOCK_IN_MUX0, CLOCK_IN_MUX0_RC_0);

    return input_freq;
}

static u32 clock_moudle_pll_sys_freq(u32 out_freq)
{
    tu8 index;

    clock_printf("--func = %s\n", __FUNCTION__);
    clock_printf("sys out_freq : 0x%x\n", out_freq);

    for (index = 0; index < PLL_CLK_480M_TAB_SIZE; index++) {
        if (out_freq == pll_clk480_tab[index]) {
            clock_moudle_pll_sys_clk(PLL_SYS_SEL_PLL480M, index);
            clock_printf("pll_clk480_tab : 0x%x\n", index);
            return pll_clk480_tab[index];
        }
    }
    for (index = 0; index < PLL_CLK_192M_TAB_SIZE; index++) {
        if (out_freq == pll_clk192_tab[index]) {
            clock_moudle_pll_sys_clk(PLL_SYS_SEL_PLL192M, index);
            clock_printf("pll_clk192_tab : 0x%x\n", index);
            return pll_clk192_tab[index];
        }
    }
    return 0;
}

static void clock_moudle_pll_alnk_freq(u32 out_freq)
{
    tu8 index;

    clock_printf("--func = %s\n", __FUNCTION__);
    clock_printf("\n alnk out_freq : 0x%x\n", out_freq);

    for (index = 0; index < ALNK_CLK_TAB_SIZE; index++) {
        if (out_freq == alnk_clk_tab[index]) {
            clock_moudle_pll_alnk_clk(NULL, index);
            clock_printf("\n alnk_clk_tab : 0x%x\n", index);
            return;
        }
    }
}

static void clock_moudle_pll_rfi_freq(u32 out_freq)
{
    tu8 index;

    clock_printf("--func = %s\n", __FUNCTION__);
    clock_printf("\n rfi out_freq : 0x%x\n", out_freq);

    for (index = 0; index < PLL_CLK_192M_TAB_SIZE; index++) {
        if (out_freq == pll_clk192_tab[index]) {
            clock_moudle_pll_rfi_clk(PLL_RFI_SEL_PLL192M, index);
            clock_printf("pll_clk192_tab : 0x%x\n", index);
            return;
        }
    }
    for (index = 0; index < PLL_CLK_480M_TAB_SIZE; index++) {
        if (out_freq == pll_clk480_tab[index]) {
            clock_moudle_pll_rfi_clk(PLL_RFI_SEL_PLL480M, index);
            clock_printf("pll_clk480_tab : 0x%x\n", index);
            return;
        }
    }
}

static void clock_moudle_pll_apc_freq(u32 out_freq)
{
    tu8 index;

    clock_printf("--func = %s\n", __FUNCTION__);
    clock_printf("\n apc out_freq : 0x%x\n", out_freq);

    for (index = 0; index < PLL_CLK_192M_TAB_SIZE; index++) {
        if (out_freq == pll_clk192_tab[index]) {
            clock_moudle_pll_apc_clk(PLL_APC_SEL_PLL192M, index);
            clock_printf("pll_clk192_tab : 0x%x\n", index);
            return;
        }
    }
    for (index = 0; index < PLL_CLK_480M_TAB_SIZE; index++) {
        if (out_freq == pll_clk480_tab[index]) {
            clock_moudle_pll_apc_clk(PLL_APC_SEL_PLL480M, index);
            clock_printf("pll_clk480_tab : 0x%x\n", index);
            return;
        }
    }
}

static u32 pll_moudle_run(u32 input_freq, u32 out_freq)
{
    u32 really_out_freq;

    clock_printf("--func = %s\n", __FUNCTION__);
    clock_printf("PLL in freq : 0x%x\n", input_freq);

    clock_moudle_pll_enable(FALSE);         //<关闭 PLL
    clock_moudle_pll_ref_clk(input_freq);   //<设置 PLL 参考源和参考频率
    really_out_freq = clock_moudle_pll_sys_freq(out_freq);    //<设置 PLL 后分频系数
    clock_moudle_pll_enable(TRUE);          //<使能 PLL
    clock_moudle_main_clk_input(MAIN_CLOCK_IN_MUX1, CLOCK_IN_MUX1_PLL); //MAIN CLOCK选择PLL

    clock_printf("really_out_freq : %u HZ\n", really_out_freq);
    return really_out_freq;
}

static const SYS_CLK_SET INPUT_RTOSC_32K1P = {
    .rc_protect = rc_clock_protect,
    .input      = NULL,
    .io_enable  = rtosc_clk_1pin_io_enable,
    .hsb_limit  = NULL,
    .lsb_limit  = NULL,
    .run        = rtosc_clock_run,
};

static const SYS_CLK_SET INPUT_RTOSC_32K2P = {
    .rc_protect = rc_clock_protect,
    .input      = NULL,
    .io_enable  = rtosc_clk_2pin_io_enable,
    .hsb_limit  = NULL,
    .lsb_limit  = NULL,
    .run        = rtosc_clock_run,
};

static const SYS_CLK_SET INPUT_RTOSC_24M2P = {
    .rc_protect = rc_clock_protect,
    .input      = NULL,
    .io_enable  = rtosc_clk_2pin_io_enable,
    .hsb_limit  = NULL,
    .lsb_limit  = NULL,
    .run        = rtosc_clock_run,
};

static const SYS_CLK_SET INPUT_BTOSC = {
    .rc_protect = rc_clock_protect,
    .input      = NULL,
    .io_enable  = btosc_clk_io_enable,
    .hsb_limit  = NULL,
    .lsb_limit  = NULL,
    .run        = bt_osc_clock_run,
};

static const SYS_CLK_SET INPUT_RC = {
    .rc_protect = NULL,
    .input      = NULL,
    .io_enable  = rc_clk_io_enable,
    .hsb_limit  = NULL,
    .lsb_limit  = NULL,
    .run        = rc_clock_run,
};

static const SYS_CLK_SET *sys_clock_from_original[] = {
    &INPUT_RTOSC_32K1P,
    &INPUT_RTOSC_32K2P,
    &INPUT_RTOSC_24M2P,
    &INPUT_BTOSC,
    &INPUT_RC,
};

static const SYS_CLK_SET PLL_INPUT_RTOSC_32K1P = {
    .rc_protect = rc_clock_protect,
    .input      = pll_input_rtosc,
    .io_enable  = rtosc_clk_1pin_io_enable,
    .hsb_limit  = hsb_limit,
    .lsb_limit  = lsb_limit,
    .run        = pll_moudle_run,
};

static const SYS_CLK_SET PLL_INPUT_RTOSC_32K2P = {
    .rc_protect = rc_clock_protect,
    .input      = pll_input_rtosc,
    .io_enable  = rtosc_clk_2pin_io_enable,
    .hsb_limit  = hsb_limit,
    .lsb_limit  = lsb_limit,
    .run        = pll_moudle_run,
};

static const SYS_CLK_SET PLL_INPUT_RTOSC_24M2P = {
    .rc_protect = rc_clock_protect,
    .input      = pll_input_rtosc,
    .io_enable  = rtosc_clk_2pin_io_enable,
    .hsb_limit  = hsb_limit,
    .lsb_limit  = lsb_limit,
    .run        = pll_moudle_run,
};

static const SYS_CLK_SET PLL_INPUT_BTOSC = {
    .rc_protect = rc_clock_protect,
    .input      = pll_input_bt_osc,
    .io_enable  = btosc_clk_io_enable,
    .hsb_limit  = hsb_limit,
    .lsb_limit  = lsb_limit,
    .run        = pll_moudle_run,
};

static const SYS_CLK_SET PLL_INPUT_RC = {
    .rc_protect = rc_clock_protect,
    .input      = pll_input_rc,
    .io_enable  = rc_clk_io_enable,
    .hsb_limit  = hsb_limit,
    .lsb_limit  = lsb_limit,
    .run        = pll_moudle_run,
};

static const SYS_CLK_SET PLL_INPUT_PAT = {
    .rc_protect = rc_clock_protect,
    .input      = pll_input_pat,
    .io_enable  = rc_clk_io_enable,
    .hsb_limit  = hsb_limit,
    .lsb_limit  = lsb_limit,
    .run        = pll_moudle_run,
};

static const SYS_CLK_SET *sys_clock_from_pll[] = {
    &PLL_INPUT_RTOSC_32K1P,
    &PLL_INPUT_RTOSC_32K2P,
    &PLL_INPUT_RTOSC_24M2P,
    &PLL_INPUT_BTOSC,
    &PLL_INPUT_RC,
    &PLL_INPUT_PAT,
};

static SYS_CLK_SET *clock_regisit(SYS_CLOCK_INPUT sys_in)
{
    SYS_CLK_SET *sys_clk_set = NULL;

    clock_printf("--func = %s\n", __FUNCTION__);
    switch (sys_in) {
    //<- 不支持PLL 后分频(原生时钟源)
    case SYS_CLOCK_INPUT_RTOSC_32K1P:
    case SYS_CLOCK_INPUT_RTOSC_32K2P:
    case SYS_CLOCK_INPUT_RTOSC_24M2P:
    case SYS_CLOCK_INPUT_BT_OSC:
    case SYS_CLOCK_INPUT_RC:
        sys_clk_set = (SYS_CLK_SET *)sys_clock_from_original[sys_in];
        break;

    //<- 支持PLL 后分频(衍生时钟源)
    case SYS_CLOCK_INPUT_PLL_32K1P:
    case SYS_CLOCK_INPUT_PLL_32K2P:
    case SYS_CLOCK_INPUT_PLL_24M2P:
    case SYS_CLOCK_INPUT_PLL_BT_OSC:
    case SYS_CLOCK_INPUT_PLL_RC:
    case SYS_CLOCK_INPUT_PLL_PAT:
        sys_clk_set = (SYS_CLK_SET *)sys_clock_from_pll[sys_in - SYS_CLOCK_INPUT_PLL_32K1P];
        break;

    default:
        clock_puts("- SYS_CLOCK_INPUT_DEFAULT\n");
        break;
    }

    return sys_clk_set;
}

static void clock_run(SYS_CLK_SET *sys_clk_set, u32 input_freq, u32 out_freq)
{
    u8 hsb_div = 1;
    u8 lsb_div = 1;
    u32 really_out_clk = 0;

    ASSERT(sys_clk_set != NULL);
    clock_printf("--func = %s\n", __FUNCTION__);

    //<clock set process
    if (sys_clk_set->rc_protect) {
        sys_clk_set->rc_protect();                      //<任何时钟切换需回到 RC时钟
    }
    if (sys_clk_set->input) {
        sys_clk_set->input();                           //<输入源选择
    }
    if (sys_clk_set->io_enable) {
        sys_clk_set->io_enable(TRUE, input_freq);       //<初始化输入源 IO
    }
    if (sys_clk_set->hsb_limit) {
        hsb_div = sys_clk_set->hsb_limit(out_freq);               //<hsb频率限制：120M
        hsb_div++;
    }
    if (sys_clk_set->lsb_limit) {
        lsb_div = sys_clk_set->lsb_limit(out_freq);               //<lsb频率限制：60M
        lsb_div++;
    }
    if (sys_clk_set->run) {
        really_out_clk = sys_clk_set->run(input_freq, out_freq); //<运行系统时钟设置
    }

    //<save clk var
    if (really_out_clk >= MHZ_UNIT) {
        g_clk_info.hsb_clk = really_out_clk / MHZ_UNIT / hsb_div;
    } else {
        g_clk_info.hsb_clk = really_out_clk / KHZ_UNIT / hsb_div;
    }

    g_clk_info.lsb_clk = g_clk_info.hsb_clk / lsb_div;
}

/*
 * APC Clock
 */
void apc_freq_set_api(u32 out_freq)
{
    clock_moudle_pll_apc_freq(out_freq);
}

/*
 * RFI Clock
 */
void rfi_freq_set_api(u32 out_freq)
{
    clock_moudle_pll_rfi_freq(out_freq);
}

/*
 * ALNK Clock
 */
void alnk_freq_set_api(u32 out_freq)
{
    clock_moudle_pll_alnk_freq(out_freq);
}

/*
 * FM Clock
 */
void fm_freq_set_api(u32 out_freq)
{
    //clock_moudle_pll_alnk_freq(out_freq);
}
/********************************************************************************/
/*
 *                   HW Abstract Layer
 */
static void hw_clock_init(u8 sys_in, u32 input_freq, u32 out_freq)
{
    SYS_CLK_SET *sys_clk_set;

#if 0
    LDO_CON &= ~0x700;     // bit10-8 clear
    LDO_CON |= 0x300;      // bit10-8 set
#endif

    sys_clk_set = clock_regisit(sys_in);            //init pointer
    clock_run(sys_clk_set, input_freq, out_freq);   //run clock

    ///临时加入
    rfi_freq_set_api(RFI_Hz);
    apc_freq_set_api(APC_Hz);

    /* clock_out_PA4(PA4_CLOCK_OUT_HSB); */
}

static u32 get_lsb_clk_hz(void)
{
    return (g_clk_info.lsb_clk * MHZ_UNIT);
}

static u32 get_hsb_clk_hz(void)
{
    return (g_clk_info.hsb_clk * MHZ_UNIT);
}

static struct clock_driver clock_drv_ins = {
    .on             = hw_clock_init,
    .off            = NULL,

    .set_sys_freq   = NULL,
    .get_sys_freq   = get_hsb_clk_hz,

    .set_lsb_freq   = NULL,
    .get_lsb_freq   = get_lsb_clk_hz,

    .set_alnk_freq   = rfi_freq_set_api,
    .get_alnk_freq   = NULL,

    .set_rfi_freq   = rfi_freq_set_api,
    .get_rfi_freq   = NULL,

    .set_apc_freq   = apc_freq_set_api,
    .get_apc_freq   = NULL,
};
REGISTER_CLOCK_DRIVER(clock_drv_ins);
