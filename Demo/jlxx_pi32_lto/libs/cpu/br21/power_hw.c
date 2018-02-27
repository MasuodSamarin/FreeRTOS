#include "typedef.h"
#include "stdarg.h"
#include "power_hw.h"
#include "power_interface.h"
#include "power_api.h"
#include "interrupt.h"

/* #define LOG_INFO_ENABLE */
#include "debug_log.h"


/* #ifdef SUPPORT_MS_EXTENSIONS */
/* #pragma bss_seg(".bt_power_bss") */
/* #pragma data_seg(".bt_power_data") */
/* #pragma const_seg(".bt_power_const") */
/* #pragma code_seg(".bt_power_code") */
/* #endif */

extern void bt_sfr_push(void);
extern void RF_poweroff_recover(void);



#define Tst0 0x1
#define Tst1 0x1
#define Tst2 0x1
#define Tst4 0x1
#define Tst5 0x1
#define Tst6 0x1

#define REGS_NUM        40

#define BT_REGS_NUM     5

#define RTC_REGS_NUM    3


struct power_driver_hdl {
    u8 mode;
    u8 delay_usec;
    u8 charge_sw;
    u8 keep_osci_flag;
    u8 osc_type;
    u8 pwr_mode;
    u8 pd_vddio_lev;
    u8 pd_rtcvdd_lev;
    u8 btosc_disable;

    u32 total;
    u32 regs[REGS_NUM];
    u32 bt_regs[BT_REGS_NUM];
    u32 rtc_regs[RTC_REGS_NUM];
    u32 osc_hz;
    u32 Tosc;
    u8 kstb0;
    u8 kstb1;
    u8 kstb2;
    u8 kstb3;
    u8 kstb4;
    u8 kstb5;
    u8 kstb6;

    void (*powerdown_enter)(void);
    void (*powerdown_exit)(u32);

    void (*poweroff_enter)(void);
    void (*poweroff_exit)(u32);
};

static struct power_driver_hdl  driver_hdl SEC(.non_volatile_ram);

#define __this (&driver_hdl)
/********************************************************************************/
/*
 *                   HW Sfr Layer
 */

////////////////////////////////////////////////////////////
//  function:   delay_us
//  description: delay n uS
////////////////////////////////////////////////////////////
extern void delay(u32 d);
static void delay_us(u32 n)
{
    // while(n--);
    delay(n * __this->delay_usec);
}

u8 LP_NK(u16 x)
{
    u16 i = 15;

    for (i = 15; i > 1 ; i--) {
        if (x > ((u16)(1 << (i - 1)))) {
            return i;
        }
    }

    if (x > 1) {
        return 1;
    }

    return 0;
}

#define P33_CS_H            JL_PMU->P33_CON  |= BIT(0)
#define P33_CS_L            JL_PMU->P33_CON  &= ~BIT(0)


static u8 p33_buf(u8 buf)
{
    JL_PMU->P33_DAT = buf;
    JL_PMU->P33_CON |= BIT(5);

    while (JL_PMU->P33_CON & BIT(1));

    return JL_PMU->P33_DAT;
}

static void p33_tx_1byte(u16 addr, u8 data0)
{
    P33_CS_H;

    p33_buf(((addr > 0x3f) << 6) | (addr & 0x3f));                      //wr    //adr 3

    if (addr > 0x3f) {
        p33_buf(addr >> 6);    //wr    //adr 3
    }

    p33_buf(data0);

    P33_CS_L;
}

static u8 p33_rx_1byte(u16 addr)
{
    u8 data;

    P33_CS_H;

    p33_buf(BIT(7) | ((addr > 0x3f) << 6) | (addr & 0x3f));             //rd    //adr 3

    if (addr > 0x3f) {
        p33_buf(addr >> 6);    //wr    //adr 3
    }

    data = p33_buf(0x5e);

    P33_CS_L;

    return data;
}

static void P33_CON_SET(u16 addr, u8 start, u8 len, u8 data)
{
    u8 reg = 0;
    reg = p33_rx_1byte(addr);
    SFR(reg, start, len, data);
    p33_tx_1byte(addr, reg);
}

static u8 P33_CON_GET(u8 addr)
{
    u8 reg = 0;
    reg = p33_rx_1byte(addr);
    return reg;
}

static void P33_CON_DEBUG(void)
{
    u8 i = 0;
    for (i = 0; i < P33_VLD_KEEP + 1; i++) {
        printf("--%x : %x--\n", i, P33_CON_GET(i));
    }
}

//for P33_ANA_CON0
static void LDO_1512_EN(u8 en)
{
    P33_CON_SET(P33_ANA_CON0, 0, 1, en);
}

static void MAIN_VBG_EN(u8 en)
{
    P33_CON_SET(P33_ANA_CON0, 1, 1, en);
}

static void LDO15_EN(u8 en)
{
    P33_CON_SET(P33_ANA_CON0, 2, 1, en);
}

static void DCDC15_EN(u8 en)
{
    P33_CON_SET(P33_ANA_CON0, 3, 1, en);
}

static void MLDO33_EN(u8 en)
{
    P33_CON_SET(P33_ANA_CON0, 4, 1, en);
}

static void PW_GATE_EN(u8 en)
{
    P33_CON_SET(P33_ANA_CON0, 5, 1, en);
}

static void MRTC33_EN(u8 en)
{
    P33_CON_SET(P33_ANA_CON0, 6, 1, en);
}

static void RC_OSC_EN(u8 en)
{
    P33_CON_SET(P33_ANA_CON0, 7, 1, en);
}

//for P33_ANA_CON1
static void WLDO12_EN(u8 en)
{
    P33_CON_SET(P33_ANA_CON1, 1, 1, en);
}

static void DVDDA_1512_EN(u8 en)
{
    P33_CON_SET(P33_ANA_CON1, 2, 1, en);
}

static void WLDO06_EN(u8 en)
{
    P33_CON_SET(P33_ANA_CON1, 3, 1, en);
}

static void WBTLDO_EN(u8 en)
{
    P33_CON_SET(P33_ANA_CON1, 4, 1, en);
}

static void WVDDIO_EN(u8 en)
{
    P33_CON_SET(P33_ANA_CON1, 5, 1, en);
}

static void WVBG_EN(u8 en)
{
    P33_CON_SET(P33_ANA_CON1, 6, 1, en);
}

static void DVDDA_SHORT_EN(u8 en)
{
    P33_CON_SET(P33_ANA_CON1, 7, 1, en);
}

//for P33_ANA_CON2
static void RTC_LEVEL(u8 lev)
{
    if (lev < 0 || lev > 7) {
        return;
    }

    P33_CON_SET(P33_ANA_CON2, 0, 3, lev);
}

static u8 GET_RTC_LEVEL(void)
{
    u8 lev;
    lev = P33_CON_GET(P33_ANA_CON2);
    return (lev & 0x07);
}

static void VCM_DET_EN(u8 en)
{
    P33_CON_SET(P33_ANA_CON2, 3, 1, en);
}

static void D2SH_DIS(u8 en)
{
    P33_CON_SET(P33_ANA_CON2, 4, 1, !en);
}

static void WVBG_LEVEL(u8 lev)
{
    if (lev < 0 || lev > 7) {
        return;
    }

    P33_CON_SET(P33_ANA_CON2, 5, 3, lev);
}

//for P33_ANA_CON3
static void WVBG_CURRENT_LEV(u8 lev)
{
    if (lev < 0 || lev > 3) {
        return;
    }
    P33_CON_SET(P33_ANA_CON3, 0, 2, lev);
}

//for P33_CHG_CON
static void CHG_EN(u8 en)
{
    P33_CON_SET(P33_CHG_CON, 0, 1, en);
}

static void CHG_DET_EN(u8 en)
{
    P33_CON_SET(P33_CHG_CON, 1, 1, en);
}

static void CHG_CURRENT_LEV(u8 lev)
{
    if (lev < 0 || lev > 3) {
        return;
    }
    P33_CON_SET(P33_CHG_CON, 2, 2, lev);
}
static void CHG_LEVEL(u8 lev)
{
    if (lev < 0 || lev > 15) {
        return;
    }

    P33_CON_SET(P33_CHG_CON, 4, 4, lev);
}

static u8 GET_CHG_STA(u8 mode)
{
    u8 ret = 0;

    ret = P33_CON_GET(P33_CHG_CON);
    if (mode == 0) {
        ret = ret & 0x01;
    } else if (mode == 1) {
        ret = ret & 0x02 >> 1;
    } else if (mode == 2) {
        ret = ret & 0x0c >> 2;
    } else {
        ret = ret & 0xf0 >> 4;
    }
    return ret;
}

static void P33_PLVD_EN(u8 en)
{
    P33_CON_SET(P33_PLVD_CON, 0, 1, en);
    P33_CON_SET(P33_PLVD_CON, 6, 1, 1);
}

static void P33_VLVD_EN(u8 en)
{
    P33_CON_SET(P33_VLVD_CON, 0, 1, en);
    P33_CON_SET(P33_VLVD_CON, 6, 1, 1);
}

#define VDD50_LVD_2_0V        0
#define VDD50_LVD_2_1V        1
#define VDD50_LVD_2_2V        2
#define VDD50_LVD_2_3V        3
#define VDD50_LVD_2_4V        4
#define VDD50_LVD_2_5V        5
#define VDD50_LVD_2_6V        6
#define VDD50_LVD_2_7V        7
static void P33_VLVD_SET(u8 vlvd)
{
    u8 reg;

    reg = p33_rx_1byte(P33_VLVD_CON);

    reg &= ~(BIT(3) | BIT(4) | BIT(5));
    reg |= vlvd << 3;

    p33_tx_1byte(P33_VLVD_CON, reg);

    /* reg = p33_rx_1byte(P33_VLVD_CON); */
    /* printf("p33_vlvd_con=0x%x\n", (reg>>3)&0x7); */
}

static void LRC_CLK_EN(void)
{
    /* P33_CON_SET(P33_LRC_CON0, 4, 3, 7);  //RPPS */
    /* P33_CON_SET(P33_LRC_CON1, 2, 3, 4);  //CAP */
    P33_CON_SET(P33_LRC_CON0, 4, 3, 4);  //RPPS
    P33_CON_SET(P33_LRC_CON1, 2, 3, 6);  //CAP

    P33_CON_SET(P33_LRC_CON0, 7, 1, 0);  //RNPS S0
    P33_CON_SET(P33_LRC_CON1, 0, 2, 0);  //RNPS S1 S2

    P33_CON_SET(P33_LRC_CON0, 2, 2, 0);  //RP TRIM
    P33_CON_SET(P33_LRC_CON0, 1, 1, 0);  //RN TRIM

    P33_CON_SET(P33_LRC_CON0, 0, 1, 1);  //EN
}

static u8 READ_PMU_RESET_SOURCE(void)
{
    return P33_CON_GET(P33_RST_CON);
}

static void dcdc15_on(u16 delay_cnt)
{
    DCDC15_EN(1);  //DCDC 15 enable
    LDO15_EN(0);   //LDO  15 disable
    WBTLDO_EN(0);  //WBTLDO disable
    delay_us(delay_cnt);
}

static void ldo15_on(u16 delay_cnt)
{
    LDO15_EN(1);   //LDO  15 enable
    DCDC15_EN(0);  //DCDC 15 disable
    WBTLDO_EN(0);  //WBTLDO disable
    delay_us(delay_cnt);
}

static u8 __ldo_set_pwrmd(u8 mode, u16 delay_cnt)
{
    __this->pwr_mode = mode;

    if (mode == PWR_LDO15) {
        printf("ldo15\n");
        ldo15_on(delay_cnt);
    } else if (mode == PWR_DCDC15) {
        printf("dcdc15\n");
        dcdc15_on(delay_cnt);
    } else {
        printf("set pwr err\n");
        while (1);
    }

    DVDDA_1512_EN(1);
    delay_us(50);
    DVDDA_SHORT_EN(0);

    return 0;
}
////////////////////////////////////////////////////////////
//  function:   pwr_buf
//  description: power control write/read 1 byte
////////////////////////////////////////////////////////////

static u8 pwr_buf1(u8 buf)
{
    JL_PMU->DAT = buf;

    pmu_stran();

    while (pmu_swait());
    return JL_PMU->DAT;
}


AT_NON_VOLATILE_RAM_CODE
static u8 pwr_buf(u8 buf)
{
    JL_PMU->DAT = buf;

    pmu_stran();

    while (pmu_swait());
    return JL_PMU->DAT;
}

AT_NON_VOLATILE_RAM_CODE
static u8 pwr_readbyte(u8 addr)
{
    u8 pd_tmp;
    pmu_csen();
    pwr_buf(addr);
    pd_tmp = pwr_buf(0);
    pmu_csdis();
    return pd_tmp;
}

AT_NON_VOLATILE_RAM_CODE
static void pwr_writebyte(u8 addr, u8 dat)
{
    pmu_csen();
    pwr_buf(addr);
    pwr_buf(dat);
    pmu_csdis();
}

/************PMU POWER DOMAIN*************/
static void DVDDA_DVDD_SHORT(u8 en)
{
    if (en) {
        DVDDA_SHORT_EN(1);
        DVDDA_1512_EN(0);
    } else {
        DVDDA_1512_EN(1);
        DVDDA_SHORT_EN(0);
    }
}

static void RESET_MASK_SW(u8 sw)
{
    if (sw) {
        pwr_writebyte(WR_IVS_SET0, BIT(5));    //1f
    } else {
        pwr_writebyte(WR_IVS_SET1, BIT(5));    //1e
    }
}

static void D2SH_DIS_SW(u8 sw)
{
    D2SH_DIS(sw);
}

static void PMU_POWERDOWN(void)
{
    u8 pd_tmp;
    pd_tmp = pwr_readbyte(RD_PMU_CON0);
    //[6] pend [1] mode 0:power down  1:poweroff
    pwr_writebyte(WR_PMU_CON0, (pd_tmp &  ~BIT(1)) | BIT(6));
}

AT_VOLATILE_RAM_CODE
static void PMU_POWEROFF(void)
{
#if 0
    u8 pd_tmp;
    pd_tmp = pwr_readbyte(RD_PMU_CON0);
    //[6] pend [1] mode 0:power down  1:poweroff
    pwr_writebyte(WR_PMU_CON0, pd_tmp | BIT(1));
#endif
}

static void WLDO12_DISABLE(void)
{
    WLDO12_EN(0);
}

AT_VOLATILE_RAM_CODE
static void WLDO12_ENABLE(void)
{
    u8 pd_tmp;
    pd_tmp = pwr_readbyte(RD_PWR_SCON1);
    pwr_writebyte(WR_PWR_SCON1, pd_tmp | BIT(1));
}

static void SET_RTCVDD_LEV(u8 level)
{
    RTC_LEVEL(level);
}

static u8 GET_RTCVDD_LEV(void)
{
    return GET_RTC_LEVEL();
}

static u8 READ_PWR_SCON0(void)
{
    u8 pd_tmp;
    pd_tmp = pwr_readbyte(RD_PWR_SCON0);
    return pd_tmp;
}

static u8 IS_POWEROFF(void)
{
#if 0
    u32 pwr_down_wkup;

    pmu_csen();
    pwr_buf_flash(RD_PMU_CON0);
    pwr_down_wkup = pwr_buf_flash(0);
    pmu_csdis();

    if (pwr_down_wkup & BIT(5)) {
        return 1;
    }
#endif
    return 0;
}

AT_VOLATILE_RAM_CODE
static u8 POWER_IS_WAKEUP(void)
{
    u8 pd_tmp;

    pmu_csen();
    pwr_buf(RD_MSTA_CON);
    pd_tmp = pwr_buf(0);
    pmu_csdis();

    pd_tmp &= 0x0f;

    return (pd_tmp == 7) ? 1 : 0;
}

static void PMU_DEBUG(void)
{
#if 1
    u8 pd_tmp;

    /* printf("PDCON : %02x\n", JL_POWER_DOWN->CON); */

    pmu_csen();
    pwr_buf(RD_PMU_CON0);
    u8 i = 0;
    while (i <= 0x24) {
        pd_tmp = pwr_buf(0);
        printf("%02x : %02x\n", i, pd_tmp);
        i++;
    }
    pmu_csdis();
#endif
}

static void POWER_TO_CTL(u8 sw)
{
    u8 pd_tmp;

    pd_tmp = pwr_readbyte(RD_PMU_CON0);

    if (sw) {
        pd_tmp |= BIT(4);
    } else {
        pd_tmp &= ~BIT(4);
    }

    pd_tmp |= BIT(6);

    pwr_writebyte(WR_PMU_CON0, pd_tmp);
}

static void POWER_OSC_SEL(u8 mode)
{
    u8 pd_tmp;

    pd_tmp = pwr_readbyte(RD_PMU_CON1);
    pd_tmp &= ~(BIT(0) | BIT(1) | BIT(2));
    pd_tmp |= mode;
    pwr_writebyte(WR_PMU_CON1, pd_tmp);
}

static void POWER_RC_EN()
{
    RC_OSC_EN(1);
}

/************1.2V NORMAL POWER DOMAIN*************/

/************LDO_CON SFR*************/
static void VBG_OUTPUT_TO_ADC(u8 en)
{
    SFR(JL_SYSTEM->LDO_CON0, 0, 1, en); //VBG output to adc   0 : disable  1 : enable
}

static void ADC_PD_EN(u8 en)
{
    SFR(JL_SYSTEM->LDO_CON0, 1, 1, en);
}

//TO VDDIO
AT_VOLATILE_RAM_CODE
static void SET_VDDIO_LEV(u8 lev)
{
    //0(3.53v)  1(3.34v) 2(3.18v) 3(3.04v) 4(2.87v) 5(2.73v) 6(2.62v) 7(2.52v)
    SFR(JL_SYSTEM->LDO_CON0, 7, 3, lev);
}

AT_VOLATILE_RAM_CODE
static u8 GET_VDDIO_LEV(void)
{
    u32 tmp = 0;
    tmp |= BIT(7) | BIT(8) | BIT(9);
    return ((JL_SYSTEM->LDO_CON0 & tmp) >> 7);
}

//TO VDC15
static void SET_VDC15_LEV(u8 lev)
{
    //0(1.61v)  1(1.51v) 2(1.43v) 3(1.35v)
    SFR(JL_SYSTEM->LDO_CON0, 10, 2, lev);
}

static u8 GET_VDC15_LEV(void)
{
    u32 tmp = 0;
    tmp |= BIT(10) | BIT(11);
    return ((JL_SYSTEM->LDO_CON0 & tmp) >> 10);
}

//TO VDD15TO12
static void SET_VDD15TO12_LEV(u8 lev)
{
    //0(1.39v)  1(1.34v) 2(1.3v) 3(1.2v) 4(1.1v) 5(1.0v) 6(0.9v) 7(0.9v)
    SFR(JL_SYSTEM->LDO_CON0, 12, 3, lev);
}

static u8 GET_VDD15TO12_LEV(void)
{
    u32 tmp = 0;
    tmp |= BIT(12) | BIT(13) | BIT(14);
    return ((JL_SYSTEM->LDO_CON0 & tmp) >> 12);
}

//TO MVDD33TO12
static void SET_MVDD33TO12_LEV(u8 lev)
{
    //0(1.39v)  1(1.34v) 2(1.3v) 3(1.2v) 4(1.1v) 5(1.0v) 6(0.9v) 7(0.9v)
    SFR(JL_SYSTEM->LDO_CON0, 15, 3, lev);
}

static u8 GET_MVDD33TO12_LEV(void)
{
    u32 tmp = 0;
    tmp |= BIT(15) | BIT(16) | BIT(17);
    return ((JL_SYSTEM->LDO_CON0 & tmp) >> 15);
}

//TO WVDD33TO12
AT_VOLATILE_RAM_CODE
static void SET_WVDD33TO12_LEV(u8 lev)
{
    //0(0.703v)  1(0.675v) 2(0.631v) 3(0.592v) 4(0.559v) 5(0.527v) 6(0.493v) 7(0.462v)
    SFR(JL_SYSTEM->LDO_CON0, 18, 3, lev);
}

static u8 GET_WVDD33TO12_LEV(void)
{
    u32 tmp = 0;
    tmp |= BIT(18) | BIT(19) | BIT(20);
    return ((JL_SYSTEM->LDO_CON0 & tmp) >> 18);
}

//TO DVDDA
static void SET_DVDDA_LEV(u8 lev)
{
    //0(1.39v)  1(1.34v) 2(1.3v) 3(1.2v) 4(1.1v) 5(1.0v) 6(0.9v) 7(0.9v)
    SFR(JL_SYSTEM->LDO_CON0, 21, 3, lev);
}

static u8 GET_DVDDA_LEV(void)
{
    u32 tmp = 0;
    tmp |= BIT(21) | BIT(22) | BIT(23);
    return ((JL_SYSTEM->LDO_CON0 & tmp) >> 21);
}

//TO BTAVDD
static void SET_BTAVDD_LEV(u8 lev)
{
    //0(1.45v)  1(1.4v) 2(1.35v) 3(1.3v)
    SFR(JL_SYSTEM->LDO_CON0, 24, 2, lev);
}

static u8 GET_BTAVDD_LEV(void)
{
    u32 tmp = 0;
    tmp |= BIT(24) | BIT(25);
    return ((JL_SYSTEM->LDO_CON0 & tmp) >> 24);
}

//TO DCDC_OSC
static void SET_DCDC_OSC_LEV(u8 lev)
{
    //0(2M)  1(1.5M) 2(1.2M) 3(1M) 4(805K) 5(710K) 6(639K) 7(578k)
    SFR(JL_SYSTEM->LDO_CON0, 26, 3, lev);
}

static u8 GET_DCDC_OSC_LEV(void)
{
    u32 tmp = 0;
    tmp |= BIT(26) | BIT(27) | BIT(28);
    return ((JL_SYSTEM->LDO_CON0 & tmp) >> 26);
}

static void ADC_VBAT_EN(u8 en)
{
    SFR(JL_SYSTEM->LDO_CON0, 30, 1, en);
}

static void ADC_LDOIN_EN(u8 en)
{
    SFR(JL_SYSTEM->LDO_CON0, 31, 1, en);
}

/************LVD_CON SFR*************/

static void SET_VDDIO_LVD_LEV(u8 lev)
{
    //0(2v)  1(2.2v) 2(2.4v) 3(2.6v)
    SFR(JL_SYSTEM->LVD_CON, 0, 2, lev);
}

static void LVD_OE_EN(u8 en)
{
    SFR(JL_SYSTEM->LVD_CON, 2, 1, en);
}


AT_VOLATILE_RAM_CODE
static void LVD_EN(u8 en)
{
    SFR(JL_SYSTEM->LVD_CON, 3, 1, en);
}

static void LVD_DETECT_SOURCE(u8 source)
{
    //0 : VDDIO  1 : PB5
    SFR(JL_SYSTEM->LVD_CON, 4, 1, source);
}

static void LVD_DETECT_MD(u8 md)
{
    //0 : RESET  1 : INTERRUPT
    SFR(JL_SYSTEM->LVD_CON, 5, 1, md);
}

static void CLR_LVD_PND(void)
{
    SFR(JL_SYSTEM->LVD_CON, 14, 1, 1);
}

static u8 LVD_PND(void)
{
    return (JL_SYSTEM->LVD_CON & BIT(15));
}



#define POWER_SET_NORMAL_DELAY_CNT   400
static void set_change_pwrmd_in()
{
    if (__this->pwr_mode == PWR_DCDC15) {
        __ldo_set_pwrmd(PWR_LDO15, POWER_SET_NORMAL_DELAY_CNT);

    }
}

/* extern const struct rtc_driver *__rtc_drv; */
static void __pmu_soft_poweroff(void)
{
    u8 pd_tmp;

    set_change_pwrmd_in();

    /*not diable d2sh sw */
    D2SH_DIS(0);

    /*关VCM_DET */
    VCM_DET_EN(0);

    /*close FAST CHARGE */
    CHG_EN(0);

    /*关RTC PLVD */
    P33_PLVD_EN(0);
    P33_VLVD_EN(0);

    /*关所有电源的keep位 */
    P33_CON_SET(P33_ANA_KEEP, 0, 8, 0);

    /* 软关机时用,close reset mask */
    RESET_MASK_SW(0);

    /*关powerdown and poweroff */
    pd_tmp = pwr_readbyte(RD_PMU_CON0);
    pwr_writebyte(WR_PMU_CON0, pd_tmp & ~BIT(0));

}

void __btosc_disable_sw(u8 sw)
{
    if (sw) {
        if (__this->osc_type != BT_OSC) {
            D2SH_DIS_SW(1);
        } else {
            D2SH_DIS_SW(0);
        }
    } else {
        D2SH_DIS_SW(__this->btosc_disable);
    }
}

AT_VOLATILE_RAM_CODE
void __use_rc_clk(void)
{
    /*clock change to RC*/
    SFR(JL_CLOCK->CLK_CON0, 0, 1, 1);
    SFR(JL_CLOCK->CLK_CON0, 8, 1, 0);

    /*close PLL*/
    JL_CLOCK->PLL_CON = 0;

    /*close BT OSC*/
    SFR(WLA_CON14, 10, 1, 0x0);   //osc en ; no connect should close
    SFR(WLA_CON17, 10, 4, 0x0);   //osc HCS
    SFR(WLA_CON17, 0, 5, 0x0);    //osc CLS
    SFR(WLA_CON17, 5, 5, 0x0);    //osc CRS
    SFR(WLA_CON14, 13, 1, 0x0);   //osc bt oe
    SFR(WLA_CON14, 14, 1, 0x0);   //osc fm oe
    SFR(WLA_CON17, 14, 2, 0x0);   //osc LDO level
    SFR(WLA_CON14, 11, 1, 0x0);   //osc ldo en
    SFR(WLA_CON14, 12, 1, 0x0);   //osc test
    SFR(WLA_CON18, 2, 2, 0x0);    //osc xhd current

    SFR(JL_FMA->CON1, 12, 1,  0x0);          //FMLDO_EN_12v
}

AT_VOLATILE_RAM_CODE
void __hw_cache_idle()
{
    //等flash 操作完毕
    while (!(JL_DSP->CON & BIT(5)));
    JL_SFC->CON &= ~BIT(0);
}

AT_VOLATILE_RAM_CODE
void __hw_cache_run()
{
    JL_SFC->CON |= BIT(0);
}

AT_VOLATILE_RAM_CODE
void __hw_dvdd_sw04(void)
{
    SET_WVDD33TO12_LEV(7);                //set dvdd  to 0.4v when power down
}

AT_VOLATILE_RAM_CODE
void __hw_dvdd_sw33(void)
{
    SET_WVDD33TO12_LEV(1);   			  //set dvdd  to 3.3v when out of power down
}

u8 vddio_lev_backup = 1;
AT_VOLATILE_RAM_CODE
void __hw_ldo_sw26(void)
{
    vddio_lev_backup = GET_VDDIO_LEV();

    if (__this->pd_vddio_lev > 7 || __this->pd_vddio_lev < 0) {
        SET_VDDIO_LEV(7);    // VDDIO set to 2.6v
    } else {
        SET_VDDIO_LEV(__this->pd_vddio_lev);
    }
}

AT_VOLATILE_RAM_CODE
void __hw_ldo_sw33(void)
{
    SET_VDDIO_LEV(vddio_lev_backup);     // VDDIO set to 3.3v
}

void __hw_bt_analog_off(void)
{
    WLA_CON0 &= ~BIT(0);    //PMU -> BTLDO EN
    WLA_CON20 &= ~BIT(0);   //PMU -> SEL : software
}

void __hw_bt_analog_on(void)
{
    WLA_CON20 |= BIT(0);   //PMU -> SEL : hardware
    /* WLA_CON0 |= BIT(0); */
}

AT_VOLATILE_RAM_CODE
void __bt_osc_enter_powerdown(void)
{
    if (__this->osc_type == BT_OSC) {
        SFR(WLA_CON8, 24, 2, 0x0);  //XOSC XOSC_ANATEST_S10
        SFR(WLA_CON8, 23, 1, 0x0);  //XOSC XOSC_ANATEST_EN_12v
        SFR(WLA_CON8, 18, 5, 0x3);  //XOSC CRS
        SFR(WLA_CON8, 13, 5, 0x3);  //XOSC CLS
        SFR(WLA_CON8, 8, 5, 0x8);   //XOSC HCS
        SFR(WLA_CON8, 6, 2, 0x0);   //XOSC HD
        SFR(WLA_CON8, 5, 1, 0x0);   //XOSC_ADET_EN_12v
        SFR(WLA_CON8, 4, 1, 0x0);   //XOSC_CPTEST_EN_12v
        SFR(WLA_CON8, 3, 1, 0x0);   //XOSC_CMP_MODE_12v
        SFR(WLA_CON8, 2, 1, 0x1);   //XOSC_AAC_EN_12v
        SFR(WLA_CON8, 0, 2, 0x1);   //XOSCLDO_S10
        SFR(WLA_CON7, 31, 1, 0x1);  //XOSCLDO_PAS
        SFR(WLA_CON7, 28, 1, 0x1);  //XOSC EN
        SFR(WLA_CON7, 29, 1, 0x0);  //XOSC_BT_OE
        SFR(WLA_CON7, 30, 1, 0x1);  //XOSC SYS OE
    } else {

    }
}

u8 rtcvdd_lev_backup = 1;
void __hw_ldo_vbg_off(void)
{
    /* set rtc mldo level to min */
    rtcvdd_lev_backup = GET_RTCVDD_LEV();
    if (__this->pd_rtcvdd_lev > 7 || __this->pd_rtcvdd_lev < 0) {
        SET_RTCVDD_LEV(7);
    } else {
        SET_RTCVDD_LEV(__this->pd_rtcvdd_lev);
    }

    WVDDIO_EN(1);
    WVBG_LEVEL(0);

    P33_CON_SET(P33_ANA_KEEP, 0, 8, 0);
    P33_CON_SET(P33_ANA_KEEP, 6, 1, 1);
    P33_CON_SET(P33_VLD_KEEP, 2, 1, 1);

    /* DVDDA DVDD short enable */
    DVDDA_DVDD_SHORT(1);

    /*关VCM_DET */
    VCM_DET_EN(0);

    /*close FAST CHARGE */
    CHG_EN(0);

    VBG_OUTPUT_TO_ADC(0);
    ADC_PD_EN(0);
    ADC_VBAT_EN(0);
    ADC_LDOIN_EN(0);

    /*close RTC 32K ---keep_osci_flag can set to close or not */
    /* close_32K(__this->keep_osci_flag); */
}

void __hw_ldo_vbg_on(void)
{
    /* DVDDA DVDD short disable */
    DVDDA_DVDD_SHORT(0);

    /* set rtc mldo level to normal */
    SET_RTCVDD_LEV(rtcvdd_lev_backup);
}

/********************************************************************************/
/*
*                   HW API Layer
*/
AT_VOLATILE_RAM_CODE
static void __hw_power_off_enter(void)
{
    PMU_POWEROFF();
    WLDO12_ENABLE();
}

static void __hw_power_down_enter(void)
{
    PMU_POWERDOWN();
    WLDO12_DISABLE();
}


static void __hw_soft_power_off_enter(void)
{
    __pmu_soft_poweroff();

    WLDO12_DISABLE();
    pwr_writebyte(WR_IVS_SET0, 0x80);

    JL_POWER->CON |= BIT(2);
    JL_POWER->CON |= BIT(4);
}

/***********************************************************/
/*
 *
 * bb
 *      _________                         ___________
 *               |_______________________|
 * cpu           Tprp                 Tcke
 *      ____________                   ______________
 *                  |_________________|
 *
 */
/***********************************************************/
//RTC OSC sel 32K
/* #define TICK_PER_SEC 	31250 */
//BT OSC sel 12M / prescale 187500
#define TICK_PER_SEC        (__this->osc_hz)


/* static  */
AT_NON_VOLATILE_RAM_CODE
static u32 __tcnt_us(u32 x)
{
    u64 y;

    /* put_u32hex(x); */
    /* put_u32hex(__this->osc_hz); */
    y = (u64)x * (__this->osc_hz);
    /* put_u64hex(y); */
    y = y / 1000000L;
    /* put_u32hex(y); */
    return (u32)y;
}

/* static  */
AT_NON_VOLATILE_RAM_CODE
static u32 __tcnt_ms(u32 x)
{
    u64 y;

    /* put_u32hex(x); */
    /* put_u32hex(__this->osc_hz); */
    y = ((u64)x) * (__this->osc_hz);
    /* put_u64hex(y); */
    y = y / 1000L;
    /* put_u32hex(y); */
    return (u32)y;
}

/* static  */
AT_NON_VOLATILE_RAM_CODE
static u32 __tus_cnt(u32 x)
{
    u64 y;
    /* put_u32hex(x); */
    /* put_u32hex(__this->osc_hz); */
    y = ((u64)__this->total) * 1000000L;
    /* put_u64hex(y); */
    y = y / (__this->osc_hz);
    /* put_u32hex(y); */

    return (u32)y;
}

/* static  */
#define HW_RESERVE_TIME_US  (Tstb4+Tstb5+Tstb6+SLEEP_RECOVER_AFTER_EXIT_MS*1000L+150)
AT_NON_VOLATILE_RAM_CODE
static u32 __hw_poweroff_time(u32 usec, u8 mode)
{
    u32 tmp32;
    u32 Tcke, Tprp, max_cnt;

    if (mode) {
        Tprp = __tcnt_ms(DSLEEP_SAVE_BEFORE_ENTER_MS);      ///保存时间
        Tcke = __tcnt_ms(DSLEEP_RECOVER_AFTER_EXIT_MS);     ///恢复时间6000L;// ms
        /* Tcke = __tcnt_ms(500);    ///恢复时间6000L;// ms */
    } else {
        Tprp = __tcnt_ms(SLEEP_SAVE_BEFORE_ENTER_MS);       ///保存时间
        Tcke = __tcnt_ms(SLEEP_RECOVER_AFTER_EXIT_MS);      ///恢复时间6000L;// ms
    }

    /* log_info("osc hz: %08d\n", TICK_PER_SEC); */
    //total cnt = uSEC / (1/period(Hz)/1000000)(us)
    __this->total = __tcnt_us(usec);

    /* puts("usec1 : "); */
    /* put_u32hex(usec); */
    /* puts("Total : "); */
    /* put_u32hex(__this->total); */
    /* max_cnt = Tprp + Tcke + __tcnt_ms(3); */
    max_cnt = Tprp + Tcke + __tcnt_us(HW_RESERVE_TIME_US);
    /* printf("u:%d t:%d tm:%d\n", usec, __this->total, max_cnt);  */

    if (__this->total < max_cnt) {
        /* putchar('*'); */
        /* put_u16hex(__this->total); */
        /* put_u16hex(max_cnt); */
        return 0;
    }

    pmu_csen();
    pwr_buf(WR_LVC3_SET);
    tmp32 = __this->total;
    pwr_buf((u8)(tmp32 >> 24));
    pwr_buf((u8)(tmp32 >> 16));
    pwr_buf((u8)(tmp32 >> 8));
    pwr_buf((u8)(tmp32 >> 0));
    tmp32 = __this->total - Tcke  - (1 << __this->kstb4) -
            (1 << __this->kstb5) -
            (1 << __this->kstb6);
    pwr_buf((u8)(tmp32 >> 24));
    pwr_buf((u8)(tmp32 >> 16));
    pwr_buf((u8)(tmp32 >> 8));
    pwr_buf((u8)(tmp32 >> 0));

    pwr_buf((u8)(Tprp >> 8));
    pwr_buf((u8)(Tprp >> 0));
    pmu_csdis();


    usec = __tus_cnt(__this->total);

    return usec;
}



static void __regs_push(u32 *ptr, u8 num)
{
    u32 *ptr_begin;

    ptr_begin = ptr;
    /////////////////io///////////////////
    *ptr++ = JL_PORTA->DIR;
    *ptr++ = JL_PORTA->DIE;
    *ptr++ = JL_PORTA->PU;
    *ptr++ = JL_PORTA->PD;

    *ptr++ = JL_PORTB->DIR;
    *ptr++ = JL_PORTB->DIE;
    *ptr++ = JL_PORTB->PU;
    *ptr++ = JL_PORTB->PD;

    *ptr++ = JL_PORTC->DIR;
    *ptr++ = JL_PORTC->DIE;
    *ptr++ = JL_PORTC->PU;
    *ptr++ = JL_PORTC->PD;

    *ptr++ = JL_PORTD->DIR;
    *ptr++ = JL_PORTD->DIE;
    *ptr++ = JL_PORTD->PU;
    *ptr++ = JL_PORTD->PD;

    *ptr++ = JL_USB->IO_CON0;
    *ptr++ = JL_USB->CON0;
    *ptr++ = JL_USB->CON1;
    *ptr++ = JL_USB->IO_CON1;

    /////////////////usb///////////////////
    JL_USB->IO_CON0 |= BIT(2) | BIT(3); //DP设置为输入//DP设置为输入
    JL_USB->IO_CON0 &= ~(BIT(7) | BIT(6) | BIT(5) | BIT(4));//close usb io PU PD

    JL_USB->CON0 = 0;


    *ptr++ = JL_GPADC->CON;
    while (!(BIT(7) & JL_GPADC->CON));
    JL_GPADC->CON = 0;
    /////////////////adc///////////////////
    /////////////////dac///////////////////
    *ptr++ = JL_AUDIO->DAA_CON0;
    *ptr++ = JL_AUDIO->DAA_CON1;
    *ptr++ = JL_AUDIO->DAA_CON2;
    *ptr++ = JL_AUDIO->DAA_CON3;

    *ptr++ = JL_AUDIO->ADA_CON0;
    *ptr++ = JL_AUDIO->ADA_CON1;

    JL_AUDIO->DAA_CON0 = 0;
    JL_AUDIO->DAA_CON1 = 0;
    JL_AUDIO->DAA_CON2 = 0;
    JL_AUDIO->DAA_CON3 = 0;

    JL_AUDIO->ADA_CON0 = 0;
    JL_AUDIO->ADA_CON1 = 0;

    *ptr++ = JL_SYSTEM->LVD_CON;

    *ptr++ = JL_SYSTEM->LDO_CON0;

    /* log_info("ptr : %08d - %08d\n", (ptr - ptr_begin), num); */
    JL_SYSTEM->LVD_CON = 0;

    ASSERT(((ptr - ptr_begin) <= (num)), "%s\n", __func__);
}

void __bt_regs_push(u32 *ptr, u8 num)
{
    u32 *ptr_begin;

    //push regs
    ptr_begin = ptr;

    *ptr++ = WLA_CON14;
    *ptr++ = WLA_CON17;
    *ptr++ = WLA_CON18;
    *ptr++ = WLA_CON7;
    *ptr++ = WLA_CON8;
    ASSERT(((ptr - ptr_begin) <= (num)), "%s\n", __func__);

    //changer regs
    //bt osc en
    //HCS[3:0] = 1000, CLS[4:0] = 10000, CRS[4:0] = 10000,   XCK_SYS_OE = 1
    //WLA_CON14[4:1], WLA_CON13[9:5],   WLA_CON13[14:10],   WLA_CON14[7]
    SFR(WLA_CON14, 1, 4, 0x0);
}

void __bt_regs_pop(u32 *ptr, u8 num)
{
    u32 *ptr_begin;

    ptr_begin = ptr;

    WLA_CON14 = *ptr++;
    WLA_CON17 = *ptr++;
    WLA_CON18 = *ptr++;
    WLA_CON7 = *ptr++;
    WLA_CON8 = *ptr++;

    ASSERT(((ptr - ptr_begin) <= (num)), "%s\n", __func__);
}

static void __regs_pop(u32 *ptr, u8 num)
{
    u32 *ptr_begin;

    ptr_begin = ptr;

    JL_PORTA->DIR = *ptr++ ;
    JL_PORTA->DIE = *ptr++ ;
    JL_PORTA->PU  = *ptr++ ;
    JL_PORTA->PD = *ptr++ ;

    JL_PORTB->DIR = *ptr++ ;
    JL_PORTB->DIE = *ptr++ ;
    JL_PORTB->PU = *ptr++ ;
    JL_PORTB->PD = *ptr++ ;

    JL_PORTC->DIR = *ptr++ ;
    JL_PORTC->DIE = *ptr++ ;
    JL_PORTC->PU = *ptr++ ;
    JL_PORTC->PD = *ptr++ ;

    JL_PORTD->DIR = *ptr++ ;
    JL_PORTD->DIE = *ptr++ ;
    JL_PORTD->PU = *ptr++ ;
    JL_PORTD->PD = *ptr++ ;

    JL_USB->IO_CON0 = *ptr++ ;
    JL_USB->CON0 = *ptr++ ;
    JL_USB->CON1 = *ptr++ ;
    JL_USB->IO_CON1 = *ptr++ ;

    JL_GPADC->CON = *ptr++ ;
    //JL_ADC->CON |= BIT(6);

    JL_AUDIO->DAA_CON2 = *ptr++ ;
    JL_AUDIO->DAA_CON0 = *ptr++ ;
    JL_SYSTEM->LVD_CON = *ptr++ ;
    JL_SYSTEM->LDO_CON0 = *ptr++;

    ASSERT(((ptr - ptr_begin) <= (num)), "%s\n", __func__);
}

AT_VOLATILE_RAM_CODE
static void __hw_power_init(u8 osc_type)
{
    if (osc_type == BT_OSC) {
        JL_PMU->CON = pd_con_init_bt;
    } else {
        JL_PMU->CON = pd_con_init_rtc;
    }

    pmu_csen();
    pwr_buf(WR_PMU_CON0);
    pwr_buf(pd_con0_init);
    switch (osc_type) {
    case BT_OSC:
        pwr_buf(pd_con1_init_bt);
        break;
    case RTC_OSCL:
        pwr_buf(pd_con1_init_rtcl);
        break;
    case RTC_OSCH:
        pwr_buf(pd_con1_init_rtch);
        break;
    case LRC_32K:
        pwr_buf(pd_con1_init_lrc_32k);
        LRC_CLK_EN();
        break;
    }
    pwr_buf(pd_con2_init);
    pmu_csdis();

    pmu_csen();
    pwr_buf(WR_STB10_SET);
    pwr_buf(pd_con4_init(__this->kstb1, __this->kstb0));
    pwr_buf(pd_con5_init(__this->kstb3, __this->kstb2));
    pwr_buf(pd_con6_init(__this->kstb5, __this->kstb4));
    pwr_buf(pd_con7_init(__this->kstb6));
    pmu_csdis();

    pmu_csen();
    pwr_buf(WR_WLDO_CON);
    pwr_buf(pd_con20_init);
    pmu_csdis();

    __this->pd_vddio_lev = 7;
    P33_VLVD_SET(VDD50_LVD_2_0V);
}

static u8 __hw_power_is_poweroff(void)
{
    u32 pwr_down_wkup;

    pmu_csen();
    pwr_buf1(RD_PMU_CON0);
    pwr_down_wkup = pwr_buf1(0);
    pmu_csdis();

    if (pwr_down_wkup & BIT(5)) {
        return 1;
    }
    return 0;
}

AT_NON_VOLATILE_RAM_CODE
void OTP_ENABLE(bool enable)
{
    if (enable) {
        JL_CRC->REG = 0x7ea4;
        JL_OTP->CMD0 = 0x80;
        JL_OTP->CMD1 = 0x80;
        JL_CRC->REG = 0;
    } else {
        JL_CRC->REG = 0x7ea4;
        JL_OTP->CMD0 &= ~BIT(7);
        JL_OTP->CMD1 &= ~BIT(7);
        JL_CRC->REG = 0;
    }
}
/********************************************************************************/
/*
 *                   HW Abstract Layer
 */
/********************************************************************************/

AT_NON_VOLATILE_RAM_CODE
static u32 __do_power_off(u32 usec, int mode)
{
    if (mode) {
        /* puts("off\n"); */
        __hw_power_off_enter();
    } else {
        /* puts("down\n"); */
        __hw_power_down_enter();
    }

    return __hw_poweroff_time(usec, mode);
}

AT_VOLATILE_RAM_CODE
static void __power_init()
{
    __hw_power_init(__this->osc_type);
}

static u32 __power_suspend(u32 usec)
{
    __this->mode = 0;

    //putchar('@');
    return __do_power_off(usec, __this->mode);
}


static void __power_down_enter(void)
{
    __regs_push(__this->regs, REGS_NUM);

    __hw_ldo_vbg_off();

    if (__this->powerdown_enter) {
        __this->powerdown_enter();
    }

    if (__this->osc_type == BT_OSC) {
        WBTLDO_EN(1);
        __bt_regs_push(__this->bt_regs, BT_REGS_NUM);
    } else {
        WBTLDO_EN(0);
    }

    __hw_bt_analog_off();

    __btosc_disable_sw(1);

    P33_CON_SET(P33_ANA_CON1, 0, 1, 1);

    WVBG_CURRENT_LEV(0);

    P33_PLVD_EN(0);
    P33_VLVD_EN(0);

    __hw_cache_idle();
    OTP_ENABLE(0);
    __hw_dvdd_sw04();
    __hw_ldo_sw26();
    __bt_osc_enter_powerdown();
}

AT_VOLATILE_RAM_CODE
static void __power_down_exit(u32 usec)
{
    /* while (__hw_power_is_wakeup() == 0) { */
    /* pd_puts(__func__); */
    /* } */
    __hw_ldo_sw33();
    __hw_dvdd_sw33();
    OTP_ENABLE(1);
    __hw_cache_run();

    P33_PLVD_EN(1);
    P33_VLVD_EN(1);

    __btosc_disable_sw(0);

    __hw_bt_analog_on();

    if (__this->osc_type == BT_OSC) {
        WBTLDO_EN(0);
        __bt_regs_pop(__this->bt_regs, BT_REGS_NUM);
    }

    __regs_pop(__this->regs, REGS_NUM);

    if (__this->powerdown_exit) {
        __this->powerdown_exit(usec);
    }

    __hw_ldo_vbg_on();
}


AT_NON_VOLATILE_RAM_CODE
static u32 __power_off(u32 usec)
{
    u32 tmp;
    __this->mode = 1;


    int res;

    pwr_writebyte(WR_SOFT_FLAG, BIT(0) | BIT(4));

    res = pwr_readbyte(RD_SOFT_FLAG);
    /* log_info("RD_SOFT_FLAG : %x\n", res); */


    /* puts("_poweroff\n"); */
    /* put_u32hex(usec); */
    tmp = __do_power_off(usec, __this->mode);
    return tmp;
}

void poweroff_probe(void);

AT_NON_VOLATILE_RAM_CODE
static void __power_off_enter(void)
{
    poweroff_probe();
    bt_sfr_push();
    if (__this->poweroff_enter) {
        __this->poweroff_enter();
    }
}

AT_VOLATILE_RAM_CODE
static void __power_off_exit(u32 usec)
{
    RF_poweroff_recover();
    if (__this->poweroff_exit) {
        __this->poweroff_exit(usec);
    }
}

static void __soft_power_off_enter()
{
    /* maskrom_clear(); */
    __hw_soft_power_off_enter();
}

static void __enter_sleep(void)
{
    pwr_writebyte(WR_IVS_SET0, BIT(7));

    JL_POWER->CON &= ~BIT(3);
    /* SFR(JL_SYSTEM->LDO_CON0,18,3,1); */
    /* SFR(JL_SYSTEM->LDO_CON0,18,3,2); */

    /*enter sleep mode*/
    JL_POWER->CON |= BIT(2);

    JL_POWER->CON |= BIT(4);


}

static void __sys_idle_enter(void)
{
    /*close IRQ*/
    irq_global_disable();
    irq_sys_disable();

    __hw_ldo_vbg_off();

    /*deal IO*/
    u8 pd_tmp;
    pd_tmp = pwr_readbyte(RD_PWR_CON0_KEEP);
    pwr_writebyte(WR_PWR_CON0_KEEP, BIT(5) | BIT(6) | BIT(7));

    pd_tmp = pwr_readbyte(RD_PWR_SCON0);
    pwr_writebyte(WR_PWR_SCON0, pd_tmp | BIT(7));

    pd_tmp = pwr_readbyte(RD_PWR_SCON1);
    pwr_writebyte(WR_PWR_SCON1, pd_tmp | BIT(1));

    pd_tmp = 0x02;
    pwr_writebyte(WR_WLDO_CON, pd_tmp);

    SET_WVDD33TO12_LEV(0);

    __hw_bt_analog_off();

    SET_VDD15TO12_LEV(7);
    SET_MVDD33TO12_LEV(7);
    SET_DVDDA_LEV(7);

    /*close LVD*/
    LVD_EN(0);

    __use_rc_clk();

    /*VDDIO set to 2.6V*/
    __hw_ldo_sw26();

    __btosc_disable_sw(1);

    /*close cache*/
    __hw_cache_idle();

    __enter_sleep();
}

static u32 __power_ioctrl(int ctrl, ...)
{
    va_list argptr;
    va_start(argptr, ctrl);
    u32 ret = 0;

    switch (ctrl) {
    case POWER_OSC_INFO:
        __this->osc_type = va_arg(argptr, int);
        __this->osc_hz = va_arg(argptr, int);
        __this->kstb0 = Kstb0(__this->osc_hz);
        __this->kstb1 = Kstb1(__this->osc_hz);
        __this->kstb2 = Kstb2(__this->osc_hz);
        __this->kstb3 = Kstb3(__this->osc_hz);
        __this->kstb4 = Kstb4(__this->osc_hz);
        __this->kstb5 = Kstb5(__this->osc_hz);
        __this->kstb6 = Kstb6(__this->osc_hz);
        if ((__this->osc_type == BT_OSC) || \
            (__this->osc_type == RTC_OSCH)) {
            __this->osc_hz >>= 6;   //default div 64
        }
        log_info("POWER_OSC_INFO: %04d - %08d\n", __this->osc_type, __this->osc_hz);
        break;
    case POWER_DELAY_US:
        __this->delay_usec = va_arg(argptr, int);
        log_info("POWER_DELAY_US: %x\n", __this->delay_usec);
        break;

    case POWER_SET_RESET_MASK:
        log_info("POWER_SET_RESET_MASK\n");
        RESET_MASK_SW(va_arg(argptr, int));     //open reset mask
        break;
    case POWER_SET_BTOSC_DISABLE:
        __this->btosc_disable = va_arg(argptr, int);
        D2SH_DIS_SW(__this->btosc_disable);
        break;
    case POWER_SET_KEEP_OSCI_FLAG:
        __this->keep_osci_flag = va_arg(argptr, int);
        break;
    case POWER_SET_SLEEP_CALLBACK:
        __this->powerdown_enter = (void (*)(void))va_arg(argptr, int);
        __this->powerdown_exit = (void (*)(u32))va_arg(argptr, int);
        break;
    case POWER_SET_DEEPSLEEP_CALLBACK:
        __this->poweroff_enter = (void (*)(void))va_arg(argptr, int);
        __this->poweroff_exit = (void (*)(u32))va_arg(argptr, int);
        break;
    case POWER_SET_IDLE:
        __sys_idle_enter();
        break;
    case POWER_SET_SOFTOFF:
        __soft_power_off_enter();
        break;
    case POWER_SET_MODE:
        __ldo_set_pwrmd(va_arg(argptr, int), 400);
        break;
    default:
        break;
    }

    va_end(argptr);

    return ret;
}


u8 __power_is_poweroff()
{
    /* __hw_pmu_reset_mask(); */
    return __hw_power_is_poweroff();
}


AT_NON_VOLATILE_RAM_CODE
static struct power_driver power_ins  = {
    .init               = __power_init,
    .set_suspend_timer  = __power_suspend,
    .suspend_enter      = __power_down_enter,
    .suspend_exit       = __power_down_exit,
    .set_off_timer      = NULL,//__power_off,
    .off_enter          = NULL,//__power_off_enter,
    .off_exit           = NULL,//__power_off_exit,
    .ioctrl             = __power_ioctrl,
};
REGISTER_POWER_OPERATION(power_ins);

