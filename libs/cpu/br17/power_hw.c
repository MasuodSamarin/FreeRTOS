#include "typedef.h"
/* #include "RF_bredr.h" */
/* #include "interrupt_isr.h" */
#include "stdarg.h"
#include "power_hw.h"
#include "power_interface.h"
#include "power_api.h"
#include "interrupt.h"

/* #define LOG_INFO_ENABLE */
#include "debug_log.h"


#pragma str_literal_override("bt_poweroff_reboot.rodata")

/* #define PD_DEBUG_EN */

#ifdef PD_DEBUG_EN
#define pd_putchar(x)        putchar(x)
#define pd_puts(x)           puts(x)
#define pd_u32hex(x)         put_u32hex(x)
#define pd_u8hex(x)          put_u8hex(x)
#define pd_buf(x,y)          printf_buf(x,y)
#define pd_printf            printf
#else
#define pd_putchar(...)
#define pd_puts(...)
#define pd_u32hex(...)
#define pd_u8hex(...)
#define pd_buf(...)
#define pd_printf(...)
#endif


#define Tst0 0x1
#define Tst1 0x1
#define Tst2 0x1
#define Tst4 0x1
#define Tst5 0x1
#define Tst6 0x1

#define REGS_NUM       40

#define BT_REGS_NUM    3

struct power_driver_hdl {
    u8 mode;
    u8 delay_usec;
    u8 d2sh_dis_sw;
    u8 charge_sw;

    u8 keep_osci_flag;
    u8 osc_type;
    u8 btavdd_type;
    u32 total;
    u32 regs[REGS_NUM];
    u32 bt_regs[BT_REGS_NUM];
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

////////////////////////////////////////////////////////////
//  function:   pwr_buf
//  description: power control write/read 1 byte
////////////////////////////////////////////////////////////

static u8 pwr_buf1(u8 buf)
{
    JL_POWER_DOWN->DAT = buf;

    pmu_stran();

    while (pmu_swait());
    return JL_POWER_DOWN->DAT;
}


AT_NON_VOLATILE_RAM_CODE
static u8 pwr_buf(u8 buf)
{
    JL_POWER_DOWN->DAT = buf;

    pmu_stran();

    while (pmu_swait());
    return JL_POWER_DOWN->DAT;
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

////////////////////////////////////////////////////////////
//  function:    dcdc15_on
//  description: dcdc15 5v -> 1.5V on (bluetooth need this)
////////////////////////////////////////////////////////////
static void dcdc15_on(void)
{
    u8 pd_tmp;

    pd_tmp = pwr_readbyte(RD_PWR_SCON0);

    pwr_writebyte(WR_PWR_SCON0, pd_tmp | BIT(3));

    delay_us(400);   // ~200us

    SFR(JL_FMA->CON1, 12, 1,  0x1);        //FMLDO_EN_12v
    SFR(JL_FMA->CON8, 0,  2,  1);          //3.0v   default 0
    SFR(JL_SYSTEM->LDO_CON, 7, 3, 4);      // 1 I   VDDIO voltage select control bit; default      001(0 - 7 ： 3.53 - 2.52)
    SFR(JL_SYSTEM->LDO_CON, 10, 2, 3);     // 1 I   VDC15 voltage select control bit; default      01(0 -3 ： 1.61 - 1.35)
}

////////////////////////////////////////////////////////////
//  function:    dcdc15_off
//  description: dcdc15 5v -> 1.5v off
////////////////////////////////////////////////////////////
static void dcdc15_off(void)
{
    u8 pd_tmp;

    pd_tmp = pwr_readbyte(RD_PWR_SCON0);

    pwr_writebyte(WR_PWR_SCON0, pd_tmp & ~BIT(3));
}

////////////////////////////////////////////////////////////
//  function:    ldo15_on
//  description: ldo 5v -> 1.5V on
////////////////////////////////////////////////////////////
static void ldo15_on(void)
{

    u8 pd_tmp;

    pd_tmp = pwr_readbyte(RD_PWR_SCON0);

    pwr_writebyte(WR_PWR_SCON0, pd_tmp | BIT(2));

    delay_us(400);    // ~10uS
}

////////////////////////////////////////////////////////////
//  function:    ldo15_off
//  description: ldo 5v -> 1.5v off
////////////////////////////////////////////////////////////
static void ldo15_off(void)
{
    u8 pd_tmp;

    pd_tmp = pwr_readbyte(RD_PWR_SCON0);

    pwr_writebyte(WR_PWR_SCON0, pd_tmp & ~BIT(2));
}

/* DVDDA DVDD short enable */
static void dvdda_dvdd_short(u8 en)
{
    u8 pd_tmp;

    pd_tmp = pwr_readbyte(RD_PWR_SCON1);

    if (en) {
        pwr_writebyte(WR_PWR_SCON1, pd_tmp | BIT(7));
    } else {
        pwr_writebyte(WR_PWR_SCON1, pd_tmp & ~BIT(7));
    }
}

/*set 1.2v from 3.3v */
static void set_12_from_33(u8 en)
{
    u8 pd_tmp;

    pd_tmp = pwr_readbyte(RD_PWR_SCON0);

    if (en) {
        pwr_writebyte(WR_PWR_SCON0, pd_tmp | BIT(1));
    } else {
        pwr_writebyte(WR_PWR_SCON0, pd_tmp & ~BIT(1));
    }
}

/*set 1.2v from 1.5v */
static void set_12_from_15(u8 en)
{
    u8 pd_tmp;
    u8 pd_tmp1;

    pd_tmp = pwr_readbyte(RD_PWR_SCON0);
    pd_tmp1 = pwr_readbyte(RD_PWR_SCON1);

    if (en) {
        /*enable DVDD 1.2v from 1.5v */
        pwr_writebyte(WR_PWR_SCON0, pd_tmp | BIT(0));
        /*enable DVDDA 1.2v from 1.5v and enable 1.5v to bt 1.35v*/
        if (__this->btavdd_type) {
            pwr_writebyte(WR_PWR_SCON1, pd_tmp1 | BIT(2));
        } else {
            pwr_writebyte(WR_PWR_SCON1, pd_tmp1 | BIT(2) | BIT(3));
        }

    } else {
        /*disable DVDD 1.2v from 1.5v */
        pwr_writebyte(WR_PWR_SCON0, pd_tmp & ~BIT(0));
        /*disable DVDDA 1.2v from 1.5v and disable 1.5v to bt 1.35v*/
        pwr_writebyte(WR_PWR_SCON1, pd_tmp1 & ~(BIT(2) | BIT(3)));
    }
}

////////////////////////////////////////////////////////////
//  function:    sys_use_ldo33
//  description: digital system use ldo33
////////////////////////////////////////////////////////////
static void sys_use_ldo33(void)
{

    u8 pd_tmp;

    pd_tmp = pwr_readbyte(RD_PWR_SCON0);

    if (pd_tmp & BIT(0)) {         //from 1.5 to 1.2 enable
        SFR(JL_SYSTEM->LDO_CON, 15, 3, 0b010);     // 3.3 -> 1.2 DVDD set to 1.3v

        /*enable 1.2v from 3.3v */
        set_12_from_33(1);
        delay_us(400);     // ~400uS

        /* DVDDA DVDD short enable */
        dvdda_dvdd_short(1);
        delay_us(400);     // ~400uS

        /*disable DVDD 1.2v from 1.5v */
        /*disable DVDDA 1.2v from 1.5v */
        set_12_from_15(0);

        SFR(JL_SYSTEM->LDO_CON, 15, 3, 0b011);     // 3.3 -> 1.2 DVDD set to 1.2v
    } else {
        return;
    }
}

////////////////////////////////////////////////////////////
//  function:    sys_use_ldo15
//  description: digital system use ldo15
////////////////////////////////////////////////////////////
static void sys_use_ldo15(void)
{
    u8 pd_tmp;

    pd_tmp = pwr_readbyte(RD_PWR_SCON0);

    if (pd_tmp & BIT(0)) {         //from 1.5 to 1.2 enable
        if (pd_tmp & BIT(3)) {     // BTDCDC15_EN,need to close dcdc15
            //DCDC15 diable
            dcdc15_off();
            return;
        } else if (pd_tmp & BIT(2)) { //LDO15_EN
            return;
        }
    } else {
        SFR(JL_SYSTEM->LDO_CON, 12, 3, 0b010);     // 1.5 -> 1.2 DVDD set to 1.3v
        SFR(JL_SYSTEM->LDO_CON, 21, 3, 0b010);     // 1.5 -> 1.2 DVDDA set to 1.3v

        /*enable DVDD 1.2v from 1.5v */
        /*enable DVDDA 1.2v from 1.5v */
        set_12_from_15(1);
        delay_us(400);     // ~400uS

        /* from 3.3 to 1.2 disable  */
        set_12_from_33(0);

        /* DVDDA DVDD short disable */
        dvdda_dvdd_short(0);

        SFR(JL_SYSTEM->LDO_CON, 12, 3, 0b011);     // 1.5 -> 1.2 DVDD set to 1.2v
        SFR(JL_SYSTEM->LDO_CON, 21, 3, 0b011);     // 1.5 -> 1.2 DVDDA set to 1.2v

        return;
    }
}

////////////////////////////////////////////////////////////
//  function:    sys_use_dcdc15
//  description: digital system use dcdc15
////////////////////////////////////////////////////////////
static void sys_use_dcdc15(void)
{
    u8 pd_tmp;

    pd_tmp = pwr_readbyte(RD_PWR_SCON0);

    if (pd_tmp & BIT(0)) {         //from 1.5 to 1.2 enable
        if (pd_tmp & BIT(3)) {     // BTDCDC15_EN
            return;
        } else if (pd_tmp & BIT(2)) { // BTLDO15_EN
            //LDO15 disable
            ldo15_off();
            return;
        }
    } else {
        SFR(JL_SYSTEM->LDO_CON, 12, 3, 0b010);     // 1.5 -> 1.2 DVDD set to 1.3v
        SFR(JL_SYSTEM->LDO_CON, 21, 3, 0b010);     // 1.5 -> 1.2 DVDDA set to 1.3v

        /*enable DVDD 1.2v from 1.5v */
        /*enable DVDDA 1.2v from 1.5v */
        set_12_from_15(1);
        delay_us(400);     // ~400uS

        /* from 3.3 to 1.2 disable  */
        set_12_from_33(0);
        /* DVDDA DVDD short disable */
        dvdda_dvdd_short(0);

        /* SFR(JL_SYSTEM->LDO_CON, 12, 3, 0b011);     // 1.5 -> 1.2 DVDD set to 1.2v  */
        /* SFR(JL_SYSTEM->LDO_CON, 21, 3, 0b011);     // 1.5 -> 1.2 DVDDA set to 1.2v  */

        SFR(JL_SYSTEM->LDO_CON, 12, 3, 0b100);     // 1.5 -> 1.2 DVDD set to 1.1v
        SFR(JL_SYSTEM->LDO_CON, 21, 3, 0b100);     // 1.5 -> 1.2 DVDDA set to 1.1v

        /* SFR(JL_SYSTEM->LDO_CON, 12, 3, 0b101);     // 1.5 -> 1.2 DVDD set to 1.0v  */
        /* SFR(JL_SYSTEM->LDO_CON, 21, 3, 0b101);     // 1.5 -> 1.2 DVDDA set to 1.0v  */
        return;
    }
}

////////////////////////////////////////////////////////////
//  function:   get_pwrmd
//  description: get power mode
//
//  return:
//  1:      LDOIN 5v -> VDDIO 3.3v -> DVDD 1.2v
//  2:      LDOIN 5v -> LDO 1.5v -> DVDD 1.2v, support bluetooth, pin compatible with BT15
//  3:      LDOIN 5v -> DCDC 1.5v -> DVDD 1.2v, support bluetooth
//  others: configuration error
////////////////////////////////////////////////////////////
static u8 __ldo_get_pwrmd(void)
{
    u8 pd_tmp;

    pd_tmp = pwr_readbyte(RD_PWR_SCON0);

    if (pd_tmp & BIT(0)) {             // VDD12 from 1.5v
        if (pd_tmp & BIT(3)) {         // BTDCDC15
            return 3;
        } else if (pd_tmp & BIT(2)) {  // BTLDO15
            return 2;
        }
    } else {						 // VDD12 from 3.3v
        return 1;
    }

    return 0xff;
}

////////////////////////////////////////////////////////////
//  function:   set_pwrmd
//  description: set power mode
//
//  option:
//  sm == 0:  no change
//  sm == 1:  LDOIN 5v -> VDDIO 3.3v -> DVDD 1.2v
//  sm == 2:  LDOIN 5v -> LDO 1.5v -> DVDD 1.2v, support bluetooth, pin compatible with BT15
//  sm == 3:  LDOIN 5v -> DCDC 1.5v -> DVDD 1.2v, support bluetooth
//
//  returun:
//  0:        success
//  others:   configuration error
////////////////////////////////////////////////////////////
static u8 __ldo_set_pwrmd(u8 sm)
{
    u8 tmp8;
    tmp8 = __ldo_get_pwrmd();

    /* printf("LDO PWR MODE : %08x\n", tmp8); */
    if (tmp8 == 1) {
        if ((sm == 0) || (sm == 1)) {
            return 0;
        } else if (sm == 2) {
            ldo15_on();
            sys_use_ldo15();
            return 0;
        } else if (sm == 3) {
            dcdc15_on();
            sys_use_dcdc15();
            return 0;
        }
    } else if (tmp8 == 2) {
        if (sm == 1) {
            sys_use_ldo33();
            ldo15_off();
            return 0;
        } else if ((sm == 0) || (sm == 2)) {
            return 0;
        } else if (sm == 3) {
            sys_use_ldo33();
            ldo15_off();
            dcdc15_on();
            sys_use_dcdc15();
            return 0;
        }
    } else if (tmp8 == 3) {
        if (sm == 1) {
            sys_use_ldo33();
            dcdc15_off();
            return 0;
        } else if (sm == 2) {
            sys_use_ldo33();
            dcdc15_off();
            ldo15_on();
            sys_use_ldo15();
            return 0;
        } else if ((sm == 0) || (sm == 3)) {
            return 0;
        }
    }
    return 0xff;
}

/* VCM_DET 开关 */
AT_NON_VOLATILE_RAM_CODE
static void __vcm_det_sw(u8 sw)
{
    u8 pd_tmp;

    pd_tmp = pwr_readbyte(RD_PWR_SCON2);

    if (sw) {
        pwr_writebyte(WR_PWR_SCON2, pd_tmp | BIT(3));
    } else {
        pwr_writebyte(WR_PWR_SCON2, pd_tmp & ~BIT(3));
    }
}

/* FAST CHARGE 开关 */
AT_VOLATILE_RAM_CODE
static void __fast_charge_sw(u8 sw)
{
    u8 pd_tmp = 0;

    if (__this->charge_sw == sw) {
        return;
    }

    printf("------charge sw:%d\n", sw);

    pd_tmp = pwr_readbyte(RD_PWR_SCON1);
    if (sw) {
        pd_tmp |= BIT(4) | BIT(5) | BIT(6);
    } else {
        pd_tmp &= ~(BIT(4) | BIT(5) | BIT(6));
    }

    pwr_writebyte(WR_PWR_SCON1, pd_tmp);

    __this->charge_sw = sw;
}

AT_VOLATILE_RAM_CODE
static u32 __fast_charge_sta()
{
    u8 pd_tmp;
    u8 flag = 0;

    pd_tmp = pwr_readbyte(RD_PWR_SCON1);

    flag = (pd_tmp & BIT(6)) ? 1 : 0;

    return (u32)flag;
}

/* RESET MASK 开关 */
AT_VOLATILE_RAM_CODE
static void __reset_mask_sw(u8 sw)
{
    if (sw) {
        pwr_writebyte(WR_IVS_SET0, BIT(5));    //1f
    } else {
        pwr_writebyte(WR_IVS_SET1, BIT(5));    //1e
    }
}

/*D2SH DIS 开关 */
AT_VOLATILE_RAM_CODE
static void __d2sh_dis_sw(u8 sw)
{
    u8 pd_tmp;

    pd_tmp = pwr_readbyte(RD_PWR_SCON2);

    if (sw) {
        pwr_writebyte(WR_PWR_SCON2, pd_tmp | BIT(4));
    } else {
        pwr_writebyte(WR_PWR_SCON2, pd_tmp & ~BIT(4));
    }
}

#define rtc_cs_h            JL_IRTC->CON  |= BIT(8)
#define rtc_cs_l            JL_IRTC->CON  &= ~BIT(8)

#define rtc_ck_h            JL_IRTC->CON  |= BIT(9)
#define rtc_ck_l            JL_IRTC->CON  &= ~BIT(9)

#define rtc_do_h            JL_IRTC->CON  |= BIT(10)
#define rtc_do_l            JL_IRTC->CON  &= ~BIT(10)

#define rtc_di              JL_IRTC->CON & BIT(11)

static void rtc_tbuf(u8 buf)
{
    u8 aaa, cnt;

    cnt = 8;
    while (cnt--) {
        aaa = buf & BIT(7);
        buf = buf << 1;

        if (aaa) {
            rtc_do_h;
        } else {
            rtc_do_l;
        }
        rtc_ck_h;
        rtc_ck_l;
    }
}

static u8 rtc_rbuf(void)
{
    u8 bbb, cnt;

    cnt = 8;
    while (cnt--) {

        rtc_ck_h;
        rtc_ck_l;

        bbb = bbb << 1;
        if (rtc_di) {
            bbb = bbb | BIT(0);
        }
    }
    return bbb;
}


static void rtc_sendbyte(u8 addr, u8 dat)
{
    rtc_cs_h;
    rtc_tbuf(addr);
    rtc_tbuf(dat);
    rtc_cs_l;
}

static u8 rtc_readbyte(u8 addr)
{
    u8 aa;

    rtc_cs_h;
    rtc_tbuf(addr);
    aa = rtc_rbuf();
    rtc_cs_l;

    return aa;
}


static void __pmu_powerdown(void)
{
    u8 pd_tmp;

    pd_tmp = pwr_readbyte(RD_PMU_CON0);

    //[6] pend [1] mode 0:power down  1:poweroff
    pwr_writebyte(WR_PMU_CON0, (pd_tmp &  ~BIT(1)) | BIT(6));
}

AT_NON_VOLATILE_RAM_CODE
static void __pmu_poweroff(void)
{
    u8 pd_tmp;

    /* 关VCM_DET */
    __vcm_det_sw(0);

    /*close FAST CHARGE */
    /* __fast_charge_sw(0); */

    /* rtc_port_lower_close();  //关PR口 */

    /* __rtc_drv->portr_control(PORTR_CTL_ADCEN , BIT(3) , 0); */
    /* __rtc_drv->portr_control(PORTR_CTL_ADCEN , BIT(2) , 0); */

    /* if (__this->poweroff_wakeup_io_callback) { */
    /* __this->poweroff_wakeup_io_callback(); */
    /* } */

    pd_tmp = pwr_readbyte(RD_PMU_CON0);
    //[6] pend [1] mode 0:power down  1:poweroff
    pwr_writebyte(WR_PMU_CON0, pd_tmp | BIT(1));
}

void rtc_port_lower(void);
void rtc_ldo5v_wkup_init(void);

/* extern const struct rtc_driver *__rtc_drv; */
static void __pmu_soft_poweroff(void)
{
    u8 pd_tmp;

    /*use 3.3v to 1.2v before enter soft poweroff */
    __ldo_set_pwrmd(1);

    /*not diable d2sh sw */
    __d2sh_dis_sw(0);

    /*关VCM_DET */
    __vcm_det_sw(0);

    /*close FAST CHARGE */
    __fast_charge_sw(0);

    /*关RTC PLVD */
    pwr_writebyte(WR_PLVD_CON, 0x00);

    /*关所有电源的keep位 */
    pwr_writebyte(WR_PWR_CON0_KEEP, 0x00);

    /* 软关机时用,close reset mask */
    __reset_mask_sw(0);
    /* pwr_writebyte(WR_IVS_SET1 , BIT(5)); */

    /*关powerdown and poweroff */
    pd_tmp = pwr_readbyte(RD_PMU_CON0);
    pwr_writebyte(WR_PMU_CON0, pd_tmp & ~BIT(0));

    /*close RTC 32K ---keep_osci_flag can set to close or not */
    /* if(!__this->keep_osci_flag) */
    /* __rtc_drv->io_ctrl(RTC_HW_OP_SET_X32EN , 0); */
    /* [> __rtc_drv->io_ctrl(RTC_HW_OP_SET_X32OE , 0); <] */
    /* [> __rtc_drv->io_ctrl(RTC_HW_OP_SET_X32TS , 0); <] */
    /* __rtc_drv->io_ctrl(RTC_HW_OP_SET_X32XS , 0); */

    /* rtc_port_lower_close();  //关PR口 */

    /* __rtc_drv->portr_control(PORTR_CTL_ADCEN , BIT(3) , 0); */
    /* __rtc_drv->portr_control(PORTR_CTL_ADCEN , BIT(2) , 0); */
}

static void __ldo12_disable(void)
{
    u8 pd_tmp;

    pd_tmp = pwr_readbyte(RD_PWR_SCON1);

    pwr_writebyte(WR_PWR_SCON1, pd_tmp & ~BIT(1));
}

AT_NON_VOLATILE_RAM_CODE
static void __ldo12_enable(void)
{
    u8 pd_tmp;

    pd_tmp = pwr_readbyte(RD_PWR_SCON1);

    pwr_writebyte(WR_PWR_SCON1, pd_tmp | BIT(1));
}

void __pmu_debug(void);
static void __ldo_disable(void)
{
    __ldo12_disable();
    /* __pmu_debug(); */


    pwr_writebyte(WR_IVS_SET0, 0x80);
}


extern void power_put_u16hex0(u16 dat);
/* AT_VOLATILE_RAM_CODE */
void __pmu_debug(void)
{
    u8 pd_tmp;

    /* printf("PDCON : %02x\n", JL_POWER_DOWN->CON); */
    /* printf("WLA_CON13 : %02x\n", WLA_CON13); */

    /* pmu_csen(); */
    /* pwr_buf(WR_MD_CON); */
    /* pwr_buf(1); */
    /* pmu_csdis(); */

    pmu_csen();
    pwr_buf1(RD_PMU_CON0);
    u8 i = 0;
    while (i <= 0x24) {
        pd_tmp = pwr_buf1(0);
        /* power_put_u16hex0((u16)i); */
        /* power_put_u16hex0((u16)pd_tmp); */
        /* power_putchar('\n'); */
        pd_printf("%02x : %02x\n", i, pd_tmp);
        i++;
    }
    pmu_csdis();
}
/********************************************************************************/
/*
*                   HW API Layer
*/
#if 0
static void debug_pd(void)
{
    u8 tmp;
    pmu_csen();
    pwr_buf(RD_PMU_CON0);
    tmp = pwr_buf(0);
    pd_printf("RD_PWR_CON0: %02x \n", tmp);
    tmp = pwr_buf(0);
    pd_printf("RD_PWR_CON1: %02x \n", tmp);
    pmu_csdis();
}
#endif

static void __hw_power_down_enter(void)
{
    __pmu_powerdown();
    __ldo12_disable();
}

AT_NON_VOLATILE_RAM_CODE
static void __hw_power_off_enter(void)
{
    __pmu_poweroff();
    __ldo12_enable();

}

static void __hw_soft_power_off_enter(void)
{
    __pmu_soft_poweroff();
    __ldo_disable();
    JL_POWER->CON |= BIT(2);
}


/* static void __hw_power_down_exit(void) */
/* { */

/* } */

/* static void __hw_power_off_exit(void) */
/* { */

/* } */

AT_VOLATILE_RAM_CODE
static void __hw_power_init(u8 osc_type)
{
    if (osc_type == BT_OSC) {
        JL_POWER_DOWN->CON = pd_con_init_bt;
    } else {
        JL_POWER_DOWN->CON = pd_con_init_rtc;
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
    pwr_buf(WR_PLVD_CON);
    pwr_buf(pd_con20_init);
    pmu_csdis();

    pmu_csen();
    pwr_buf(WR_PWR_SCON2);
    pwr_buf(pd_con22_init);
    pwr_buf(pd_con23_init);
    pwr_buf(pd_con24_init);
    pmu_csdis();

    /* __this->charge_sw = __fast_charge_sta(); */
    /* __fast_charge_sw(0);   //先把快速充电关掉 */
    /* __pmu_debug(); */
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


/* #define T_DEBUG_EN */

#ifdef T_DEBUG_EN
#define t_putchar(x)        putchar(x)
#define t_puts(x)           puts(x)
#define t_put_u8hex(x)      put_u8hex(x)
#define t_put_u32hex(x)     put_u32hex(x)
#define t_put_u64hex(x)     put_u64hex(x)
#define t_buf(x,y)          printf_buf(x,y)
#define t_printf            printf
#else
#define t_putchar(...)
#define t_puts(...)
#define t_put_u8hex(...)
#define t_put_u32hex(...)
#define t_put_u64hex(...)
#define t_buf(...)
#define t_printf(...)
#endif
/* static  */
AT_NON_VOLATILE_RAM_CODE
static u32 __tcnt_us(u32 x)
{
    u64 y;

    t_puts("\n");
    t_puts(__func__);
    t_put_u32hex(x);
    t_put_u32hex(__this->osc_hz);
    y = (u64)x * (__this->osc_hz);
    t_put_u64hex(y);
    y = y / 1000000L;
    t_put_u32hex(y);
    return (u32)y;
}

/* static  */
AT_NON_VOLATILE_RAM_CODE
static u32 __tcnt_ms(u32 x)
{
    u64 y;

    t_puts("\n");
    t_puts(__func__);
    t_put_u32hex(x);
    t_put_u32hex(__this->osc_hz);
    y = ((u64)x) * (__this->osc_hz);
    t_put_u64hex(y);
    y = y / 1000L;
    t_put_u32hex(y);
    return (u32)y;
}

/* static  */
AT_NON_VOLATILE_RAM_CODE
static u32 __tus_cnt(u32 x)
{
    t_puts("\n");
    t_puts(__func__);
    u64 y;
    t_put_u32hex(x);
    t_put_u32hex(__this->osc_hz);
    y = ((u64)__this->total) * 1000000L;
    t_put_u64hex(y);
    y = y / (__this->osc_hz);
    t_put_u32hex(y);

    return (u32)y;
}

/* static  */
AT_NON_VOLATILE_RAM_CODE
static u32 __hw_poweroff_time(u32 usec, u8 mode)
{
    u32 tmp32;
    u32 Tcke, Tprp, max_cnt;

    if (mode) {
        Tprp = __tcnt_ms(1);   ///保存时间
        Tcke = __tcnt_ms(10);  ///恢复时间6000L;// ms
        /* Tcke = __tcnt_ms(500);    ///恢复时间6000L;// ms */
    } else {
        Tprp = __tcnt_ms(1);      ///保存时间
        Tcke = __tcnt_ms(1);      ///恢复时间6000L;// ms
    }

    /* pd_printf("osc hz: %08d\n", TICK_PER_SEC); */
    //total cnt = uSEC / (1/period(Hz)/1000000)(us)
    __this->total = __tcnt_us(usec);

    /* puts("usec1 : "); */
    /* put_u32hex(usec); */
    /* puts("Total : "); */
    /* put_u32hex(__this->total); */
    max_cnt = Tprp + Tcke + __tcnt_ms(5);
    /* printf("uSEC:%d total:%d tptc:%d\n",uSEC,__this->total,max_cnt);  */

    if (__this->total < max_cnt) {
        /* putchar('X'); */
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

    *ptr++ = JL_USB->IO_CON;
    *ptr++ = JL_USB->CON0;
    *ptr++ = JL_USB->CON1;

    /////////////////usb///////////////////
    JL_USB->IO_CON |= BIT(2) | BIT(3); //DP设置为输入//DP设置为输入
    JL_USB->IO_CON &= ~(BIT(7) | BIT(6) | BIT(5) | BIT(4));//close usb io PU PD

    JL_USB->CON0 = 0;


    *ptr++ = JL_ADC->CON;

    while (!(BIT(7) & JL_ADC->CON));

    /////////////////adc///////////////////
    JL_ADC->CON = 0;

    *ptr++ = JL_AUDIO->DAA_CON2;
    /////////////////vcm///////////////////
    JL_AUDIO->DAA_CON2 = 0;

    *ptr++ = JL_AUDIO->DAA_CON0;

    JL_AUDIO->DAA_CON0 = 0;

    *ptr++ = JL_SYSTEM->LVD_CON;

    *ptr++ = JL_SYSTEM->LDO_CON;

    /* printf("ptr : %08d - %08d\n", (ptr - ptr_begin), num); */
    JL_SYSTEM->LVD_CON = 0;

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

    JL_USB->IO_CON = *ptr++ ;
    JL_USB->CON0 = *ptr++ ;
    JL_USB->CON1 = *ptr++ ;

    JL_ADC->CON = *ptr++ ;
    //JL_ADC->CON |= BIT(6);

    JL_AUDIO->DAA_CON2 = *ptr++ ;
    JL_AUDIO->DAA_CON0 = *ptr++ ;
    JL_SYSTEM->LVD_CON = *ptr++ ;
    JL_SYSTEM->LDO_CON = *ptr++;

    ASSERT(((ptr - ptr_begin) <= (num)), "%s\n", __func__);
}

AT_VOLATILE_RAM_CODE
static void __D2SH_DIS_SW(u8 sw)
{
    if (sw) {
        if (__this->osc_type != BT_OSC) {
            __d2sh_dis_sw(1);
        } else {
            __d2sh_dis_sw(0);
        }
    } else {
        __d2sh_dis_sw(__this->d2sh_dis_sw);
    }
}

AT_VOLATILE_RAM_CODE
static void __hw_cache_idle()
{
    //等flash 操作完毕
    while (!(JL_DSP->CON & BIT(5)));
    JL_SFC->CON &= ~BIT(0);
}

AT_VOLATILE_RAM_CODE
static void __hw_cache_run()
{
    JL_SFC->CON |= BIT(0);
}

AT_VOLATILE_RAM_CODE
static void __hw_dvdd_sw04(void)
{
    SFR(JL_SYSTEM->LDO_CON, 18, 3, 0x7);  //set dvdd  to 0.4v when power down
}

AT_VOLATILE_RAM_CODE
static void __hw_dvdd_sw33(void)
{
    SFR(JL_SYSTEM->LDO_CON, 18, 3, 0x1);    ////set dvdd  to 3.3v when out of power down
}

AT_VOLATILE_RAM_CODE
static void __hw_ldo_sw26(void)
{
    SFR(JL_SYSTEM->LDO_CON, 7, 3, 0x7);    // VDDIO set to 2.6v
}

AT_VOLATILE_RAM_CODE
static void __hw_ldo_sw33(void)
{
    SFR(JL_SYSTEM->LDO_CON, 7, 3, 0x1);    // VDDIO set to 3.3v
}

AT_VOLATILE_RAM_CODE
static void __hw_bt_analog_off(void)
{
    WLA_CON0 &= ~BIT(0);
}

static void __hw_bt_analog_on(void)
{
    WLA_CON0 |= BIT(0);
}

static void __bt_regs_push(u32 *ptr, u8 num)
{
    u32 *ptr_begin;

    //push regs
    ptr_begin = ptr;

    *ptr++ = WLA_CON14;
    *ptr++ = WLA_CON17;
    *ptr++ = WLA_CON18;

    ASSERT(((ptr - ptr_begin) <= (num)), "%s\n", __func__);

    //changer regs
    //bt osc en
    //HCS[3:0] = 1000, CLS[4:0] = 10000, CRS[4:0] = 10000,   XCK_SYS_OE = 1
    //WLA_CON14[4:1], WLA_CON13[9:5],   WLA_CON13[14:10],   WLA_CON14[7]
    SFR(WLA_CON14, 1, 4, 0x0);
}


static void __bt_regs_pop(u32 *ptr, u8 num)
{
    u32 *ptr_begin;

    ptr_begin = ptr;

    WLA_CON14 = *ptr++;
    WLA_CON17 = *ptr++;
    WLA_CON18 = *ptr++;

    ASSERT(((ptr - ptr_begin) <= (num)), "%s\n", __func__);
}

AT_VOLATILE_RAM_CODE
static void  __bt_osc_enter_powerdown(void)
{
    if (__this->osc_type == BT_OSC) {
        SFR(WLA_CON17, 10, 4, 0x0);   //osc HCS
        SFR(WLA_CON17, 0, 5, 0x7);    //osc CLS
        SFR(WLA_CON17, 5, 5, 0x7);    //osc CRS
        SFR(WLA_CON14, 13, 1, 0x0);   //osc bt oe
        SFR(WLA_CON14, 14, 1, 0x1);   //osc fm oe
        SFR(WLA_CON17, 14, 2, 0x0);   //osc LDO level
        SFR(WLA_CON14, 11, 1, 0x0);   //osc ldo en
        SFR(WLA_CON14, 12, 1, 0x0);   //osc test
        SFR(WLA_CON18, 2, 2, 0x0);    //osc xhd current
    }
}

/* rtc mldo level setting bit0 - bit2 2.6v-3.3v */
static void  __set_rtc_mldo_level(u8 level)
{
    u8 pd_tmp;

    pd_tmp = pwr_readbyte(RD_PWR_SCON2);
    pd_tmp &= ~(BIT(0) | BIT(1) | BIT(2));
    pwr_writebyte(WR_PWR_SCON2, pd_tmp | level);
}

static void  __hw_ldo_vbg_off(void)
{
    /* set rtc mldo level to min */
    __set_rtc_mldo_level(0x07);

    /* DVDDA DVDD short enable */
    dvdda_dvdd_short(1);

    /*关VCM_DET */
    __vcm_det_sw(0);

    /*close FAST CHARGE */
    __fast_charge_sw(0);

    /* __pmu_debug(); */

    JL_SYSTEM->LDO_CON &= ~(BIT(0) | BIT(1) | BIT(30) | BIT(31));    //VBG

    SFR(JL_FMA->CON1, 12, 1,  0x0);          //FMLDO_EN_12v
}

static void  __hw_ldo_vbg_on(void)
{
    /* DVDDA DVDD short disable */
    dvdda_dvdd_short(0);

    /* set rtc mldo level to min */
    __set_rtc_mldo_level(0x01);

    SFR(JL_FMA->CON1, 12, 1,  0x1);          //FMLDO_EN_12v
}

AT_VOLATILE_RAM_CODE
static u8 __hw_power_is_wakeup(void)
{
    u8 pd_tmp;

    pmu_csen();
    pwr_buf(RD_STB6_SET);
    pd_tmp = pwr_buf(0);
    pmu_csdis();

    return (pd_tmp & BIT(7)) ? 1 : 0;
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
        __bt_regs_push(__this->bt_regs, BT_REGS_NUM);
    }

    __hw_bt_analog_off();

    __hw_cache_idle();
    __hw_dvdd_sw04();
    __hw_ldo_sw26();
    __D2SH_DIS_SW(1);
    __bt_osc_enter_powerdown();
}

AT_VOLATILE_RAM_CODE
static void __power_down_exit(u32 usec)
{
    while (__hw_power_is_wakeup() == 0) {
        /* pd_puts(__func__); */
    }
    __D2SH_DIS_SW(0);
    __hw_ldo_sw33();
    __hw_dvdd_sw33();
    __hw_cache_run();

    __hw_bt_analog_on();

    if (__this->osc_type == BT_OSC) {
        __bt_regs_pop(__this->bt_regs, BT_REGS_NUM);
    }

    __regs_pop(__this->regs, REGS_NUM);

    if (__this->powerdown_exit) {
        __this->powerdown_exit(usec);
    }

    __hw_ldo_vbg_on();
}



AT_VOLATILE_RAM_CODE
void poweroff_wait_pending(void)
{
    u8 pd_tmp;

__READ_PENDING:

    pmu_csen();
    pwr_buf(RD_PMU_CON0);
    pd_tmp = pwr_buf(0);
    pmu_csdis();

    if (pd_tmp & BIT(7)) {
        pd_putchar('w');
        return;
    } else {
        /* power_putchar('n');	 */
        /* power_put_u16hex0(pd_tmp); */
        delay(100);
        goto __READ_PENDING;
    }

}

AT_VOLATILE_RAM_CODE
void test_power_init(void)
{
    __this->osc_type = RTC_OSCL;
    __this->osc_hz = 32768L;

    __this->kstb0 = Kstb0(__this->osc_hz);
    __this->kstb1 = Kstb1(__this->osc_hz);
    __this->kstb2 = Kstb2(__this->osc_hz);
    __this->kstb3 = Kstb3(__this->osc_hz);
    __this->kstb4 = Kstb4(__this->osc_hz);
    __this->kstb5 = Kstb5(__this->osc_hz);
    __this->kstb6 = Kstb6(__this->osc_hz);
    /* printf("ksb0:%x,ksb1:%x,ksb2:%x,ksb3:%x,ksb4:%x,ksb5:%x,ksb6:%x\n", */
    /* __this->kstb0, */
    /* __this->kstb1, */
    /* __this->kstb2, */
    /* __this->kstb3, */
    /* __this->kstb4, */
    /* __this->kstb5, */
    /* __this->kstb6); */

    if ((__this->osc_type == BT_OSC) || \
        (__this->osc_type == RTC_OSCH)) {
        __this->osc_hz >>= 6;   //default div 64
    }

    __power_init();
}



AT_NON_VOLATILE_RAM_CODE
static u32 __power_off(u32 usec)
{
    u32 tmp;
    __this->mode = 1;


    int res;

    pwr_writebyte(WR_SOFT_FLAG, BIT(0) | BIT(4));

    res = pwr_readbyte(RD_SOFT_FLAG);
    /* printf("RD_SOFT_FLAG : %x\n", res); */


    /* puts("_poweroff\n"); */
    /* put_u32hex(usec); */
    tmp = __do_power_off(usec, __this->mode);
    return tmp;
}

void poweroff_probe(void);
void bt_sfr_push(void);
void RF_poweroff_recover(void);

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
    OS_ENTER_CRITICAL();
    /* maskrom_clear(); */
    __hw_soft_power_off_enter();
}

static void __enter_sleep(void)
{
    pwr_writebyte(WR_IVS_SET0, BIT(7));

    JL_POWER->CON &= ~BIT(3);
    /* SFR(JL_SYSTEM->LDO_CON,18,3,1); */
    /* SFR(JL_SYSTEM->LDO_CON,18,3,2); */

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

    SFR(JL_SYSTEM->LDO_CON, 18, 3, 0);

    __hw_bt_analog_off();

    SFR(JL_SYSTEM->LDO_CON, 12, 6, 0x3f);
    SFR(JL_SYSTEM->LDO_CON, 21, 3, 7);

    /*close LVD*/
    JL_SYSTEM->LVD_CON = 0;

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

    /*VDDIO set to 2.6V*/
    __hw_ldo_sw26();

    __D2SH_DIS_SW(1);

    /*close cache*/
    __hw_cache_idle();

    __enter_sleep();
}

static u32 __power_ioctrl(int ctrl, ...)
{
    va_list argptr;
    va_start(argptr, ctrl);
    int sw;
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
        pd_printf("POWER_OSC_INFO: %04d - %08d\n", __this->osc_type, __this->osc_hz);
        break;
    case POWER_DELAY_US:
        __this->delay_usec = va_arg(argptr, int);
        pd_printf("POWER_DELAY_US: %x\n", __this->delay_usec);
        break;

    case POWER_SET_RESET_MASK:
        t_printf("POWER_SET_RESET_MASK\n");
        sw = va_arg(argptr, int);
        __reset_mask_sw(sw);     //open reset mask
        break;
    case POWER_SET_BTOSC_DISABLE:
        __this->d2sh_dis_sw = va_arg(argptr, int);
        t_printf("POWER_SET_D2SH_DIS:%d\n", __this->d2sh_dis_sw);
        __d2sh_dis_sw(__this->d2sh_dis_sw);
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
        __ldo_set_pwrmd(va_arg(argptr, int));
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
    .set_off_timer      = __power_off,
    .off_enter          = __power_off_enter,
    .off_exit           = __power_off_exit,
    .ioctrl             = __power_ioctrl,
};
REGISTER_POWER_OPERATION(power_ins);

