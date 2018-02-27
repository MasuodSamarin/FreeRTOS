#include "clock_hw.h"
#include "clock_interface.h"
/* #include "rtc_driver.h" */
/* #define DEBUG_ENABLE */
#include "debug_log.h"


typedef struct _SYS_CLK_SET {
    void (*rc_protect)(void);
    void (*input)(void);
    void (*io_enable)(bool enable, u32 in_freq);
    u8(*hsb_limit)(u32 out_freq);
    u8(*lsb_limit)(u32 out_freq);
    u8(*sfc_limit)(u32 out_freq);
    u32(*run)(u32 in_freq, u32 out_freq);
} SYS_CLK_SET;

typedef struct _CLK_INFO {
    u32 hsb_clk; //<HSB CLK
    u32 lsb_clk; //<LSB CLK
    u32 fm_clk;
    u32 sfc_clk; //<SFC CLK
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
#define HSB_CLK_MAX		(120 * MHZ_UNIT) //120M
#define LSB_CLK_MAX		(60 *  MHZ_UNIT) //60M
#define SFC_CLK_MAX		(100 * MHZ_UNIT) //100M

#define PLL_REF_2M		(2 *  MHZ_UNIT) //2M
#define PLL_REF_12M		(12 * MHZ_UNIT) //12M
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

static void clock_moudle_bt_osc_init(bool enable, u32 in_freq)
{
    log_info("--func = %s\n", __FUNCTION__);

#ifndef CLOCK_HAL_DEBUG
    if (enable == TRUE) {
        BT_CLOCK_IN(BT_CLOCK_IN_LSB); //BT_CLK LSB

        osc_delay(in_freq);
        osc_delay(in_freq);

        SFR(WLA_CON17, 10, 4, 0x8);   //osc HCS
        SFR(WLA_CON17, 0, 5, 0x10);   //osc CLS
        SFR(WLA_CON17, 5, 5, 0x10);   //osc CRS
        SFR(WLA_CON14, 13, 1, 0x0);   //osc bt oe
        SFR(WLA_CON14, 14, 1, 0x1);   //osc fm oe
        SFR(WLA_CON17, 14, 2, 0x2);   //osc LDO level
        SFR(WLA_CON14, 11, 1, 0x1);   //osc ldo en
        SFR(WLA_CON14, 12, 1, 0x0);   //osc test
        SFR(WLA_CON18, 2, 2, 0x2);    //osc xhd current
        SFR(WLA_CON14, 10, 1, 0x1);   //osc en ; no connect should close


        BT_CLOCK_IN(BT_CLOCK_IN_DISABLE_0); //bt clk close for less power
    } else {
        BT_CLOCK_IN(BT_CLOCK_IN_LSB); //BT_CLK LSB

        SFR(WLA_CON14, 10, 1, 0x0);   //osc en ; no connect should close

        BT_CLOCK_IN(BT_CLOCK_IN_DISABLE_0); //bt clk close for less power
    }
#endif
}

static void clock_moudle_main_clk_input(tu8 ckmux)
{
    log_info("--func = %s\n", __FUNCTION__);
    log_info("MAIN_CLOCK_IN = 0x%x\n", ckmux);

    CLOCK_SEL(ckmux);
    TEST_SEL(((ckmux == MAIN_CLOCK_IN_PAT) ? 1 : 0));
}

static void clock_moudle_rc_init(bool enable)
{
    log_info("--func = %s\n", __FUNCTION__);

    if (enable == TRUE) {
        RC_EN(1); //RC en
        delay(100);
    } else {
        RC_EN(0);
    }
}

static void rc_clock_protect(void)
{
    log_info("--func = %s\n", __FUNCTION__);

    clock_moudle_rc_init(TRUE);
    clock_moudle_main_clk_input(MAIN_CLOCK_IN_RC);
}

static void clock_moudle_rtosc_1pin_init(bool enable, u32 in_freq)
{
    log_info("--func = %s\n", __FUNCTION__);
    if (enable == TRUE) {
#ifndef CLOCK_HAL_DEBUG
        /*RTOSC port in*/

        /* __rtc_drv->portr_control(PORTR_CTL_PD  , BIT(3) , 0 ); */
        /* __rtc_drv->portr_control(PORTR_CTL_PU  , BIT(3) , 0 ); */
        /* __rtc_drv->portr_control(PORTR_CTL_DIR , BIT(3) , 1 ); */
        /* __rtc_drv->portr_control(PORTR_CTL_DIE , BIT(3) , 0 ); */
#endif
        if (in_freq == 32768L) {
            /*RTOSC X32K1P enable*/
            /* __rtc_drv->io_ctrl(RTC_HW_OP_SET_X32K1POE , 1); */
            /* __rtc_drv->io_ctrl(RTC_HW_OP_SET_X32K1PEN , 1); */

            /*RTOSC X32K1P High Current*/
        } else {
            //g_clock_moudle_err = 0x1;
            return;
        }
        osc_delay(in_freq);
    } else {
    }
}


static void clock_moudle_rtosc_2pin_init(bool enable, u32 in_freq)
{
    log_info("--func = %s\n", __FUNCTION__);

    if (enable == TRUE) {
#ifndef CLOCK_HAL_DEBUG
        /*RTOSC port in*/
        /* __rtc_drv->portr_control(PORTR_CTL_PD  ,BIT(2)| BIT(3) , 0 ); */
        /* __rtc_drv->portr_control(PORTR_CTL_PU  ,BIT(2)| BIT(3) , 0 ); */
        /* __rtc_drv->portr_control(PORTR_CTL_DIR ,BIT(2)| BIT(3) , 1 ); */
        /* __rtc_drv->portr_control(PORTR_CTL_DIE ,BIT(2)| BIT(3) , 0 ); */
#endif
        if (in_freq == 32768L) {
            /*RTOSC X32K2P enable*/
            /* __rtc_drv->io_ctrl(RTC_HW_OP_SET_X32EN , 1); */

            /*RTOSC X32K2P High Current*/
            /* __rtc_drv->io_ctrl(RTC_HW_OP_SET_X32XS , 1); */
        } else if ((in_freq >= 1000000L)
                   && (in_freq <= 26000000L)) {
            /* __rtc_drv->io_ctrl(RTC_HW_OP_SET_X32EN , 0); //close X32K2P */
            if (in_freq > 12000000L) {
                /*RTOSC X24M2P High Current*/
                /* __rtc_drv->io_ctrl(RTC_HW_OP_SET_X24XS , 1); */
            } else {
                /*RTOSC X24M2P High Current*/
                /* __rtc_drv->io_ctrl(RTC_HW_OP_SET_X24XS , 0); */
            }
            /* __rtc_drv->io_ctrl(RTC_HW_OP_SET_X24EN , 1);  //enable X24M2P */
            /* __rtc_drv->io_ctrl(RTC_HW_OP_SET_X24OE , 1); */
        } else {
            //g_clock_moudle_err = 0x2;
            return;
        }
    } else {
    }
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
    PLL_ALNK_SEL(div);
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
    log_info("--func = %s\n", __FUNCTION__);
#if 1
    PLL_RSEL(PLL_RSEL_RTOSC_DIFF);
#else
    PLL_RSEL(PLL_RSEL_PLL_REF_SEL);
    PLL_REF_SEL(PLL_REF_SEL_RTOSC);
#endif
}

static void pll_input_bt_osc(void)
{
    log_info("--func = %s\n", __FUNCTION__);
#if 1
    PLL_RSEL(PLL_RSEL_BTOSC_DIFF);
#else
    PLL_RSEL(PLL_RSEL_PLL_REF_SEL);
    PLL_REF_SEL(PLL_REF_SEL_BTOSC);
#endif
}

static void pll_input_pat(void)
{
    log_info("--func = %s\n", __FUNCTION__);
    PLL_RSEL(PLL_RSEL_PLL_REF_SEL);
    PLL_REF_SEL(PLL_REF_SEL_PAT);
}

static void clock_moudle_pll_ref_clk(tu32 input_freq)
{
    tu32 ref_freq;
    tu8  ref_div;
    tu8  ref_clk;

    log_info("--func = %s\n", __FUNCTION__);

    if (input_freq < PLL_REF_12M) {
        ref_clk = PLL_REF_CLK_2M;
        ref_freq = PLL_REF_2M;
    } else {
        ref_clk = PLL_REF_CLK_12M;
        ref_freq = PLL_REF_12M;
    }
    log_info("ref clk : 0x%x\n", ref_clk);
    log_info("ref freq : 0x%x\n", ref_freq);

    ref_div = input_freq / ref_freq;
    log_info("ref div : 0x%x\n", ref_div);
    if ((input_freq == PLL_REF_2M)
        || (input_freq == PLL_REF_12M)) {
        PLL_REFDS(0);
        PLL_REFDS_EN(0);
        log_info("ref 0 \n");
    } else {
        PLL_REFDS(ref_div - 2);
        PLL_REFDS_EN(1);
        log_info("ref -2 \n");
    }

    PLL_REF_CLK(ref_clk); //鉴相器选择(2M or 12M)
}

static void clock_moudle_pll_enable(bool enable)
{
    log_info("--func = %s\n", __FUNCTION__);
    if (enable == TRUE) {
        log_info("HAL - pll_enable\n");
        PLL_EN(1);
        delay(1);	    //>10us
        PLL_REST(1);    //复位释放(需在PLL EN使能10us后才能释放)
        delay(10 + 10); //>300us
    } else {
        log_info("HAL - pll_disable\n");
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

    log_info("--func = %s\n", __FUNCTION__);
    for (div = 0; div < 256; div++) {
        if ((out_freq / (div + 1)) <= HSB_CLK_MAX) {
            break;
        }
    }
    clock_moudle_hsb_clk_div(div);

    /* div = JL_CLOCK->SYS_DIV & 0xFF; */
    log_info("hsb_div=0x%x\n", div);

    return div;
}

static void clock_moudle_lsb_clk_div(tu8 div)
{
    LSB_CLK_DIV(div);
}

static u8 lsb_limit(u32 out_freq)
{
    tu8 div;

    log_info("--func = %s\n", __FUNCTION__);
    for (div = 0; div < 8; div++) {
        if ((out_freq / (div + 1)) <= LSB_CLK_MAX) {
            break;
        }
    }
    clock_moudle_lsb_clk_div(div);

    /* div = (JL_CLOCK->SYS_DIV & (0x7<<8))>>8; */
    log_info("lsb_div=0x%x\n", div);

    return div;
}

_NOINLINE_
static void set_spi_speed(u8 baud) AT(.volatile_ram)
{
    OS_ENTER_CRITICAL();

    while (!(JL_DSP->CON & BIT(5)));
    JL_SFC->CON &= ~BIT(0);
    JL_SFC->BAUD = baud;
    JL_SFC->CON |= BIT(0);

    OS_EXIT_CRITICAL();
}

static u8 sfc_limit(u32 out_freq)
{
    tu8 div;

    log_info("--func = %s\n", __FUNCTION__);
    for (div = 0; div < 256; div++) {
        if ((out_freq / (div + 1)) <= SFC_CLK_MAX) {
            break;
        }
    }
    set_spi_speed(div);
    log_info("sfc baud=0x%x\n", div);

    return div;
}
/*
 *		--- Clock I/O debug ---
 */
void clock_out_PB0(tu8 out)
{
    if (out == PB0_CLOCK_OUTPUT) {
        PB0_CLOCK_OUT(PB0_CLOCK_OUTPUT);
    } else {
        PB0_CLOCK_OUT(out);
        JL_PORTB->HD |= BIT(0);
        JL_PORTB->DIR &= ~BIT(0);
    }
}

void clock_out_PB9(tu8 out)
{
    if (out == PB9_CLOCK_OUTPUT) {
        PB9_CLOCK_OUT(PB9_CLOCK_OUTPUT);
    } else {
        PB9_CLOCK_OUT(out);
        JL_PORTB->HD |= BIT(9);
        JL_PORTB->DIR &= ~BIT(9);
    }
}
/*
 * LSB Clock
 */
static u32 get_lsb_div(void)
{
    return (1 + ((JL_CLOCK->SYS_DIV & (0x7 << 8)) >> 8));
}

static void set_lsb_div(u8 div)
{
    div &= 0x07;
    if (div == 0) {
        return;
    }

    SFR(JL_CLOCK->SYS_DIV, 8, 3, div - 1);
    g_clk_info.lsb_clk = g_clk_info.hsb_clk / div;
}
/*
 * HSB Clock
 */
static u32 get_hsb_div(void)
{
    return (1 + (JL_CLOCK->SYS_DIV & 0xFF));
}

static void set_sys_div(u8 div)
{
    if (div == 0) {
        return;
    }

    SFR(JL_CLOCK->SYS_DIV, 0, 8, div - 1);
    g_clk_info.hsb_clk /= div;
    g_clk_info.lsb_clk /= div;
}
/********************************************************************************/
/*
 *                   HW API Layer
 */

static void rtosc_clk_1pin_io_enable(bool enable, u32 in_freq)
{
    log_info("--func = %s\n", __FUNCTION__);
    clock_moudle_rtosc_1pin_init(enable, in_freq);
}

static void rtosc_clk_2pin_io_enable(bool enable, u32 in_freq)
{
    log_info("--func = %s\n", __FUNCTION__);
    clock_moudle_rtosc_2pin_init(enable, in_freq);
}

static void btosc_clk_io_enable(bool enable, u32 in_freq)
{
    log_info("--func = %s\n", __FUNCTION__);
    clock_moudle_bt_osc_init(enable, in_freq);
}

static void rc_clk_io_enable(bool enable, u32 in_freq)
{
    log_info("--func = %s\n", __FUNCTION__);
    clock_moudle_rc_init(enable);
}

static u32 rtosch_clock_run(u32 input_freq, u32 out_freq)
{
    log_info("--func = %s\n", __FUNCTION__);
    clock_moudle_main_clk_input(MAIN_CLOCK_IN_RTOSC_H);

    return input_freq;
}

static u32 rtoscl_clock_run(u32 input_freq, u32 out_freq)
{
    log_info("--func = %s\n", __FUNCTION__);
    clock_moudle_main_clk_input(MAIN_CLOCK_IN_RTOSC_L);

    return input_freq;
}

static u32 bt_osc_clock_run(u32 input_freq, u32 out_freq)
{
    log_info("--func = %s\n", __FUNCTION__);
    clock_moudle_main_clk_input(MAIN_CLOCK_IN_BTOSC);

    return input_freq;
}

static u32 rc_clock_run(u32 input_freq, u32 out_freq)
{
    log_info("--func = %s\n", __FUNCTION__);
    clock_moudle_main_clk_input(MAIN_CLOCK_IN_RC);

    return input_freq;
}

static u32 pat_clock_run(u32 input_freq, u32 out_freq)
{
    log_info("--func = %s\n", __FUNCTION__);
    clock_moudle_main_clk_input(MAIN_CLOCK_IN_PAT);

    return input_freq;
}

static u32 clock_moudle_pll_sys_freq(u32 out_freq)
{
    tu8 index;

    log_info("--func = %s\n", __FUNCTION__);
    log_info("sys out_freq : 0x%x\n", out_freq);

    for (index = 0; index < PLL_CLK_480M_TAB_SIZE; index++) {
        if (out_freq == pll_clk480_tab[index]) {
            clock_moudle_pll_sys_clk(PLL_SYS_SEL_PLL480M, index);
            log_info("pll_clk480_tab : 0x%x\n", index);
            return pll_clk480_tab[index];
        }
    }
    for (index = 0; index < PLL_CLK_192M_TAB_SIZE; index++) {
        if (out_freq == pll_clk192_tab[index]) {
            clock_moudle_pll_sys_clk(PLL_SYS_SEL_PLL192M, index);
            log_info("pll_clk192_tab : 0x%x\n", index);
            return pll_clk192_tab[index];
        }
    }
    return 0;
}

static void clock_moudle_pll_alnk_freq(u32 out_freq)
{
    tu8 index;

    log_info("--func = %s\n", __FUNCTION__);
    log_info("\n alnk out_freq : 0x%x\n", out_freq);

    for (index = 0; index < ALNK_CLK_TAB_SIZE; index++) {
        if (out_freq == alnk_clk_tab[index]) {
            clock_moudle_pll_alnk_clk((tu8)NULL, index);
            log_info("\n alnk_clk_tab : 0x%x\n", index);
            return;
        }
    }
}

static void clock_moudle_pll_rfi_freq(u32 out_freq)
{
    tu8 index;

    log_info("--func = %s\n", __FUNCTION__);
    log_info("\n rfi out_freq : 0x%x\n", out_freq);

    for (index = 0; index < PLL_CLK_192M_TAB_SIZE; index++) {
        if (out_freq == pll_clk192_tab[index]) {
            clock_moudle_pll_rfi_clk(PLL_RFI_SEL_PLL192M, index);
            log_info("pll_clk192_tab : 0x%x\n", index);
            return;
        }
    }
    for (index = 0; index < PLL_CLK_480M_TAB_SIZE; index++) {
        if (out_freq == pll_clk480_tab[index]) {
            clock_moudle_pll_rfi_clk(PLL_RFI_SEL_PLL480M, index);
            log_info("pll_clk480_tab : 0x%x\n", index);
            return;
        }
    }
}

static void clock_moudle_pll_apc_freq(u32 out_freq)
{
    tu8 index;

    log_info("--func = %s\n", __FUNCTION__);
    log_info("\n apc out_freq : 0x%x\n", out_freq);

    for (index = 0; index < PLL_CLK_192M_TAB_SIZE; index++) {
        if (out_freq == pll_clk192_tab[index]) {
            clock_moudle_pll_apc_clk(PLL_APC_SEL_PLL192M, index);
            log_info("pll_clk192_tab : 0x%x\n", index);
            return;
        }
    }
    for (index = 0; index < PLL_CLK_480M_TAB_SIZE; index++) {
        if (out_freq == pll_clk480_tab[index]) {
            clock_moudle_pll_apc_clk(PLL_APC_SEL_PLL480M, index);
            log_info("pll_clk480_tab : 0x%x\n", index);
            return;
        }
    }
}

static u32 pll_moudle_run(u32 input_freq, u32 out_freq)
{
    u32 really_out_freq;

    log_info("--func = %s\n", __FUNCTION__);
    log_info("PLL in freq : 0x%x\n", input_freq);

    clock_moudle_pll_enable(FALSE);         //<关闭 PLL
    clock_moudle_pll_ref_clk(input_freq);   //<设置 PLL 参考源和参考频率
    really_out_freq = clock_moudle_pll_sys_freq(out_freq);    //<设置 PLL 后分频系数
    clock_moudle_pll_enable(TRUE);          //<使能 PLL
    clock_moudle_main_clk_input(MAIN_CLOCK_IN_PLL); //MAIN CLOCK选择PLL

    log_info("really_out_freq : %u HZ\n", really_out_freq);
    return really_out_freq;
}

static const SYS_CLK_SET INPUT_RTOSC_32K1P = {
    .rc_protect = rc_clock_protect,
    .input      = NULL,
    .io_enable  = rtosc_clk_1pin_io_enable,
    .hsb_limit  = NULL,
    .lsb_limit  = NULL,
    .run        = rtoscl_clock_run,
};

static const SYS_CLK_SET INPUT_RTOSC_32K2P = {
    .rc_protect = rc_clock_protect,
    .input      = NULL,
    .io_enable  = rtosc_clk_2pin_io_enable,
    .hsb_limit  = NULL,
    .lsb_limit  = NULL,
    .run        = rtoscl_clock_run,
};

static const SYS_CLK_SET INPUT_RTOSC_24M2P = {
    .rc_protect = rc_clock_protect,
    .input      = NULL,
    .io_enable  = rtosc_clk_2pin_io_enable,
    .hsb_limit  = NULL,
    .lsb_limit  = NULL,
    .run        = rtosch_clock_run,
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

static const SYS_CLK_SET INPUT_PAT = {
    .rc_protect = NULL,
    .input      = NULL,
    .io_enable  = NULL,
    .hsb_limit  = hsb_limit,
    .lsb_limit  = lsb_limit,
    .sfc_limit  = sfc_limit,
    .run        = pat_clock_run,
};

static const SYS_CLK_SET *sys_clock_from_original[] = {
    &INPUT_RTOSC_32K1P,
    &INPUT_RTOSC_32K2P,
    &INPUT_RTOSC_24M2P,
    &INPUT_BTOSC,
    &INPUT_RC,
    &INPUT_PAT,
};

static const SYS_CLK_SET PLL_INPUT_RTOSC_32K1P = {
    .rc_protect = rc_clock_protect,
    .input      = pll_input_rtosc,
    .io_enable  = rtosc_clk_1pin_io_enable,
    .hsb_limit  = hsb_limit,
    .lsb_limit  = lsb_limit,
    .sfc_limit  = sfc_limit,
    .run        = pll_moudle_run,
};

static const SYS_CLK_SET PLL_INPUT_RTOSC_32K2P = {
    .rc_protect = rc_clock_protect,
    .input      = pll_input_rtosc,
    .io_enable  = rtosc_clk_2pin_io_enable,
    .hsb_limit  = hsb_limit,
    .lsb_limit  = lsb_limit,
    .sfc_limit  = sfc_limit,
    .run        = pll_moudle_run,
};

static const SYS_CLK_SET PLL_INPUT_RTOSC_24M2P = {
    .rc_protect = rc_clock_protect,
    .input      = pll_input_rtosc,
    .io_enable  = rtosc_clk_2pin_io_enable,
    .hsb_limit  = hsb_limit,
    .lsb_limit  = lsb_limit,
    .sfc_limit  = sfc_limit,
    .run        = pll_moudle_run,
};

static const SYS_CLK_SET PLL_INPUT_BTOSC = {
    .rc_protect = rc_clock_protect,
    .input      = pll_input_bt_osc,
    .io_enable  = btosc_clk_io_enable,
    .hsb_limit  = hsb_limit,
    .lsb_limit  = lsb_limit,
    .sfc_limit  = sfc_limit,
    .run        = pll_moudle_run,
};

static const SYS_CLK_SET PLL_INPUT_PAT = {
    .rc_protect = rc_clock_protect,
    .input      = pll_input_pat,
    .io_enable  = rc_clk_io_enable,
    .hsb_limit  = hsb_limit,
    .lsb_limit  = lsb_limit,
    .sfc_limit  = sfc_limit,
    .run        = pll_moudle_run,
};

static const SYS_CLK_SET *sys_clock_from_pll[] = {
    &PLL_INPUT_RTOSC_32K1P,
    &PLL_INPUT_RTOSC_32K2P,
    &PLL_INPUT_RTOSC_24M2P,
    &PLL_INPUT_BTOSC,
    &PLL_INPUT_PAT,
};

/*
 * Clock FLOW
 */
static void clock_status_init(void)
{
    memset(&g_clk_info, 0, sizeof(g_clk_info)); //clear var
    SFR(JL_CLOCK->SYS_DIV, 0, 8, 0);                      //clear hsb div
    SFR(JL_CLOCK->SYS_DIV, 8, 3, 0);                      //clear lsb div
}

static SYS_CLK_SET *clock_regisit(SYS_CLOCK_INPUT sys_in)
{
    SYS_CLK_SET *sys_clk_set = NULL;

    log_info("--func = %s\n", __FUNCTION__);
    switch (sys_in) {
    //<- 不支持PLL 后分频(原生时钟源)
    case SYS_CLOCK_INPUT_RTOSC_32K1P:
    case SYS_CLOCK_INPUT_RTOSC_32K2P:
    case SYS_CLOCK_INPUT_RTOSC_24M2P:
    case SYS_CLOCK_INPUT_BT_OSC:
    case SYS_CLOCK_INPUT_RC:
    case SYS_CLOCK_INPUT_PAT:
        sys_clk_set = (SYS_CLK_SET *)sys_clock_from_original[sys_in];
        break;

    //<- 支持PLL 后分频(衍生时钟源)
    case SYS_CLOCK_INPUT_PLL_32K1P:
    case SYS_CLOCK_INPUT_PLL_32K2P:
    case SYS_CLOCK_INPUT_PLL_24M2P:
    case SYS_CLOCK_INPUT_PLL_BT_OSC:
    case SYS_CLOCK_INPUT_PLL_PAT:
        sys_clk_set = (SYS_CLK_SET *)sys_clock_from_pll[sys_in - SYS_CLOCK_INPUT_PLL_32K1P];
        break;

    default:
        log_info("- SYS_CLOCK_INPUT_DEFAULT\n");
        break;
    }

    return sys_clk_set;
}

static void clock_run(SYS_CLK_SET *sys_clk_set, u32 input_freq, u32 out_freq)
{
    u8 hsb_div = 1;
    u8 lsb_div = 1;
    u8 sfc_div = 1;
    u32 really_out_clk = 0;

    ASSERT(sys_clk_set != NULL);
    log_info("--func = %s\n", __FUNCTION__);

    OS_ENTER_CRITICAL();

    //<clock set process
    if (sys_clk_set->hsb_limit) {
        hsb_div = sys_clk_set->hsb_limit(out_freq);               //<hsb频率限制：120M
        hsb_div++;
    }
    if (sys_clk_set->lsb_limit) {
        lsb_div = sys_clk_set->lsb_limit(out_freq / hsb_div);               //<lsb频率限制：60M
        lsb_div++;
    }
    if (sys_clk_set->sfc_limit) {
        sfc_div = sys_clk_set->sfc_limit(out_freq / hsb_div);               //<sfc频率限制：100M
        sfc_div++;
    }
    if (sys_clk_set->rc_protect) {
        sys_clk_set->rc_protect();                      //<任何时钟切换需回到 RC时钟
    }
    if (sys_clk_set->input) {
        sys_clk_set->input();                           //<输入源选择
    }
    if (sys_clk_set->io_enable) {
        sys_clk_set->io_enable(TRUE, input_freq);       //<初始化输入源 IO
    }
    if (sys_clk_set->run) {
        really_out_clk = sys_clk_set->run(input_freq, out_freq); //<运行系统时钟设置
    }

    OS_EXIT_CRITICAL();

    //<save clk var
    g_clk_info.hsb_clk = really_out_clk / hsb_div;
    g_clk_info.lsb_clk = g_clk_info.hsb_clk / lsb_div;
    g_clk_info.sfc_clk = g_clk_info.hsb_clk / sfc_div;
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

    clock_status_init();    						//init clock status
    sys_clk_set = clock_regisit(sys_in);            //init pointer
    clock_run(sys_clk_set, input_freq, out_freq);   //run clock

    ///临时加入
    rfi_freq_set_api(RFI_Hz);
    apc_freq_set_api(APC_Hz);
}

static u32 get_lsb_clk_hz(void)
{
    return g_clk_info.lsb_clk;
}

static u32 get_hsb_clk_hz(void)
{
    return g_clk_info.hsb_clk;
}

static u32 get_sfc_clk_hz(void)
{
    return g_clk_info.sfc_clk;
}


/*
 * SET HSB CLK
 */
static u32 clock_set_freq_fast_func(u32 out_freq)
{
    tu8 index;
    tu8 which_table = 0;
    u8 hsb_div = 1;
    u8 lsb_div = 1;
    u8 sfc_div = 1;
    u32 really_out_clk = 0;

    //<limit protect
    hsb_div = hsb_limit(out_freq);               //<hsb频率限制
    hsb_div++;
    lsb_div = lsb_limit(out_freq / hsb_div);               //<lsb频率限制
    lsb_div++;
    sfc_div = sfc_limit(out_freq / hsb_div);               //<sfc频率限制
    sfc_div++;

    //<SET PLL
    for (index = 0; index < 16; index++) {
        if (out_freq == pll_clk480_tab[index]) {
            which_table = 2;
            really_out_clk = out_freq;
            goto _set_fre;//break;
        }
    }
    for (index = 0; index < 16; index++) {
        if (out_freq == pll_clk192_tab[index]) {
            which_table = 1;
            really_out_clk = out_freq;
            goto _set_fre;
            //break;
        }
    }
    return -1;

_set_fre:
    //任何时钟切换需回到RC 时钟
    OS_ENTER_CRITICAL();
#if 1
    rc_clock_protect(); //RUN RC
#else
    clock_moudle_main_clk_input(MAIN_CLOCK_IN_BTOSC);
#endif
    if (which_table == 2) {
        clock_moudle_pll_sys_clk(PLL_SYS_SEL_PLL480M, index);
    } else {
        clock_moudle_pll_sys_clk(PLL_SYS_SEL_PLL192M, index);
    }

    clock_moudle_main_clk_input(MAIN_CLOCK_IN_PLL); //MAIN CLOCK选择PLL

    OS_EXIT_CRITICAL();

    //<save clk var
    g_clk_info.hsb_clk = really_out_clk / hsb_div;
    g_clk_info.lsb_clk = g_clk_info.hsb_clk / lsb_div;
    g_clk_info.sfc_clk = g_clk_info.hsb_clk / sfc_div;

    return 0;
}

static void clock_set_freq_fast(u32 out_freq)
{
    clock_status_init();    //init clock status

    clock_set_freq_fast_func(out_freq);
}

static struct clock_driver clock_drv_ins = {
    .on             = hw_clock_init,
    .off            = NULL,

    .set_sys_div    = set_sys_div,
    .set_sys_freq   = clock_set_freq_fast,
    .get_sys_freq   = get_hsb_clk_hz,

    .set_lsb_freq   = NULL,
    .get_lsb_freq   = get_lsb_clk_hz,

    .set_lsb_div    = set_lsb_div,
    .get_lsb_div    = get_lsb_div,
    .set_alnk_freq   = rfi_freq_set_api,
    .get_alnk_freq   = NULL,

    .set_rfi_freq   = rfi_freq_set_api,
    .get_rfi_freq   = NULL,

    .set_apc_freq   = apc_freq_set_api,
    .get_apc_freq   = NULL,

    .get_sfc_freq   = get_sfc_clk_hz,
};
REGISTER_CLOCK_DRIVER(clock_drv_ins);
