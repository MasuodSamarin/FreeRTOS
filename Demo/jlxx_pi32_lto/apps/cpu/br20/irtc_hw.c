#include "my_malloc.h"
#include "irtc_hw.h"
#include "interrupt.h"
#include "irq_interface.h"
#include "clock_interface.h"
#include "list.h"
#include "init.h"
#include "sdk_cfg.h"

#include "debug.h"

/***********************************************
  name     : irtc_sr_byte
  parameter :   u8 data   --------- register value
  return       IRTC_BUF
  note  	read and write RTC register
************************************************/
static u8 irtc_sr_byte(u8 data)
{
    OS_ENTER_CRITICAL();
    JL_IRTC->DAT = data;
    JL_IRTC->CON |= BIT(12);   //kick start
    while (JL_IRTC->CON & BIT(9));
    OS_EXIT_CRITICAL();
    return JL_IRTC->DAT;
}
/***********************************************
  name     : read_rtc_reg
  parameter :   u8 reg --------register address
  return        u8   reg data
  note  	read a byte from rtc module
************************************************/
static u8 __read_rtc_reg(u8 reg)
{
    u8 dat = 0;
    OS_ENTER_CRITICAL();
    reg = reg | R_RTC_REG;
    IRTC_CS(1);
    irtc_sr_byte(reg);
    dat = irtc_sr_byte(0);
    IRTC_CS(0);
    OS_EXIT_CRITICAL();
    return dat;
}

/***********************************************
  name     : write_rtc_reg
  parameter :   u8 reg --------register address
                         u8 data  -------register value
  return       void
  note  	write a byte to rtc register
************************************************/
static void __write_rtc_reg(u8 reg, u8 data)
{
    OS_ENTER_CRITICAL();
    reg = reg | W_RTC_REG;
    IRTC_CS(1);
    irtc_sr_byte(reg);      //reg
    irtc_sr_byte(data);      //data
    IRTC_CS(0);
    OS_EXIT_CRITICAL();
}

static void rtc_irq_handler(void)
{
    u8 pending;

    if (IRTC_IS_512Hz()) {
        IRTC_CLEAR_512Hz();
        log_info("--X512--\n");
    }

    if (IRTC_IS_2Hz()) {
        IRTC_CLEAR_2Hz();
        log_info("--X2--\n");
    }
    if (IRTC_IS_WAKEUP()) {
        IRTC_CLEAR_WAKEUP();
        log_info("--Wakeup--\n");

        pending = __read_rtc_reg(RTC_CON_ADDR);
        if (IRTC_IS_ALMOUT(pending)) {  //alarm out

        }

        pending = __read_rtc_reg(WKUP_EN_ADDR);
        pending = __read_rtc_reg(WKUP_PND_ADDR) & pending;
        if (IRTC_IS_WAKEUP_IO0(pending)) {
            log_info("-WK_PR0\n");
            __write_rtc_reg(WKUP_CLR_PND_ADDR, pending | BIT(0 + 4));
        }
        if (IRTC_IS_WAKEUP_IO1(pending)) {
            log_info("-WK_PR1\n");
            __write_rtc_reg(WKUP_CLR_PND_ADDR, pending | BIT(1 + 4));
        }
        if (IRTC_IS_WAKEUP_IO2(pending)) {
            log_info("-WK_PR2\n");
            __write_rtc_reg(WKUP_CLR_PND_ADDR, pending | BIT(2 + 4));
        }
        if (IRTC_IS_WAKEUP_IO3(pending)) {
            log_info("-WK_PR3\n");
            __write_rtc_reg(WKUP_CLR_PND_ADDR, pending | BIT(3 + 4));
        }

        pending = __read_rtc_reg(ALL_WKUP_SRC_ADDR);
        if (IRTC_IS_WAKEUP_PORT(pending)) {
            log_info("IRTC_IS_WAKEUP_PORT\n");
        }
        if (IRTC_IS_WAKEUP_LDOIN(pending)) {
        }
    }
}
IRQ_REGISTER(IRQ_RTC_IDX, rtc_irq_handler);

static void __rtc_set_port_reset(u8 index, bool level, bool enable)
{
    u8 reg;
    u8 addr = PORTR_RESET_ADDR;

    reg = __read_rtc_reg(addr);
    reg = (level) ? reg | BIT(7) : reg & ~BIT(7);
    reg = reg | index << 4;
    reg = reg | enable << 6;
    __write_rtc_reg(addr, reg);
}

static void __rtc_sfr_set(u8 addr, u8 start, u8 len, u8 data)
{
    u8 reg = 0;

    reg = __read_rtc_reg(addr);
    SFR(reg, start, len, data);
    __write_rtc_reg(addr, reg);
}

static void rtc_set_port_hd(u8 index, bool set)
{
    __rtc_sfr_set(PORTR_HD_ADDR, index + 4, 1, set);
}

static void rtc_set_port_out(u8 index, bool set)
{
    __rtc_sfr_set(PORTR_OUT_ADDR, index, 1, set);
}

static void rtc_set_port_dir(u8 index, bool set)
{
    __rtc_sfr_set(PORTR_DIR_ADDR, index + 4, 1, set);
}

static void rtc_set_port_pu(u8 index, bool set)
{
    __rtc_sfr_set(PORTR_PU_ADDR, index, 1, set);
}

static void rtc_set_port_pd(u8 index, bool set)
{
    __rtc_sfr_set(PORTR_PD_ADDR, index + 4, 1, set);
}

static void rtc_set_port_die(u8 index, bool set)
{
    __rtc_sfr_set(PORTR_DIE_ADDR, index, 1, set);
}

static void rtc_set_wakeup_en(u8 index, bool set)
{
    __rtc_sfr_set(WKUP_EN_ADDR, index, 1, set);
}

static void rtc_set_wakeup_edge(u8 index, bool edge)
{
    __rtc_sfr_set(WKUP_EDGE_ADDR, index + 4, 1, edge);
}

static void rtc_clr_wakeup_pending(u8 index)
{
    __rtc_sfr_set(WKUP_PND_ADDR, index + 4, 1, 1);
}

static void close_32k(u8 keep_osci_flag)
{
    if (keep_osci_flag) {
        __rtc_sfr_set(RTC_CON_ADDR06, 0, 8, 0x01);
    } else {
        __rtc_sfr_set(RTC_CON_ADDR06, 0, 8, 0x00);
    }
}

/*1: PR1 port voltage  drive ADC  , 0: no this function*/
static void portr1_adcen_ctl(u8 val)
{
    __rtc_sfr_set(RTC_CON_ADDR14, 0, 1, val);
}

/*1: PR2 port voltage  drive ADC  , 0: no this function*/
static void portr2_adcen_ctl(u8 val)
{
    __rtc_sfr_set(RTC_CON_ADDR14, 1, 1, val);
}

static void clr_ldo5v_pending(void)
{
    __rtc_sfr_set(RTC_CON_ADDR07, 6, 1, 1);
}

static void rtc_init(void)
{
    u8 reg;

    reg = __read_rtc_reg(RTC_CON_ADDR);

    IRTC_CLEAR_WAKEUP();
    IRTC_CLEAR_512Hz();
    IRTC_CLEAR_2Hz();

    __write_rtc_reg(WKUP_EN_ADDR, 0x00);//禁止唤醒
    __write_rtc_reg(WKUP_CLR_PND_ADDR, 0xf0);//clr_pnd

    /* IRTC_X2Hz_IE(1); */
    IRTC_WAKEUP_IE(1);

    IRQ_REQUEST(IRQ_RTC_IDX, rtc_irq_handler, 1) ; //rtc_isr
}
early_initcall(rtc_init);

void __rtc_debug(void)
{
    u8 reg;

    reg = __read_rtc_reg(LDO_CON_ADDR);
    printf("LDO_CON_ADDR : %x\n", reg);
    reg = __read_rtc_reg(PORTR_HD_ADDR);
    printf("PORTR_HD_ADDR : %x\n", reg >> 4 & 0xf);
    reg = __read_rtc_reg(PORTR_OUT_ADDR);
    printf("PORTR_OUT_ADDR : %x\n", reg & 0xf);
    reg = __read_rtc_reg(PORTR_DIR_ADDR);
    printf("PORTR_DIR_ADDR : %x\n", reg >> 4 & 0xf);
    reg = __read_rtc_reg(PORTR_PU_ADDR);
    printf("PORTR_PU_ADDR : %x\n", reg & 0xf);
    reg = __read_rtc_reg(PORTR_PD_ADDR);
    printf("PORTR_PD_ADDR : %x\n", reg >> 4 & 0xf);
    reg = __read_rtc_reg(PORTR_DIE_ADDR);
    printf("PORTR_DIE_ADDR : %x\n", reg & 0xf);
    reg = __read_rtc_reg(WKUP_EN_ADDR);
    printf("WKUP_EN_ADDR : %x\n", reg & 0xf);
    reg = __read_rtc_reg(WKUP_EDGE_ADDR);
    printf("WKUP_EDGE_ADDR : %x\n", reg >> 4 & 0xf);
    reg = __read_rtc_reg(RTC_CON_ADDR);
    printf("RTC_CON_ADDR : %x\n", reg);
    reg = __read_rtc_reg(PORTR_DIE_ADDR);
    printf("PORTR_DIE_ADDR : %x\n", reg);
}

static u8 rtc_die_bk = 0;
static u8 rtc_pupd_bk = 0;
static u8 rtc_dir_bk = 0;

void rtc_port_store(void)
{
    OS_ENTER_CRITICAL();
    rtc_die_bk  = __read_rtc_reg(PORTR_DIE_ADDR);
    rtc_pupd_bk = __read_rtc_reg(PORTR_PD_ADDR);
    rtc_dir_bk  = __read_rtc_reg(PORTR_DIR_ADDR);
    OS_EXIT_CRITICAL();
}

void rtc_port_close(void)
{
    OS_ENTER_CRITICAL();
    __write_rtc_reg(PORTR_DIE_ADDR, 0x00);
    __write_rtc_reg(PORTR_PD_ADDR, 0x00);
    __write_rtc_reg(PORTR_DIR_ADDR, 0xf0);
    OS_EXIT_CRITICAL();
}

void rtc_port_restore(void)
{
    OS_ENTER_CRITICAL();
    __write_rtc_reg(0x04, 0x00);
    __write_rtc_reg(PORTR_DIE_ADDR, rtc_die_bk);
    __write_rtc_reg(PORTR_PD_ADDR,  rtc_pupd_bk);
    __write_rtc_reg(PORTR_DIR_ADDR, rtc_dir_bk);
    OS_EXIT_CRITICAL();
}

void rtc_low_power(void)
{
    /*close RTC 32K ---keep_osci_flag can set to close or not */
    /* close_32K(__this->keep_osci_flag); */
    close_32k(0);

    rtc_port_close();                     //关PR口
    portr1_adcen_ctl(0);
    portr2_adcen_ctl(0);
    clr_ldo5v_pending();
}

/**
 * @brief rtc_set_wakeup_io
 *
 * @param index : PR number
 * @param edge : 1 - rise edge ; 0 - fall edge
 */
void rtc_set_wakeup_io(u8 index, bool edge)
{
    rtc_set_port_dir(index, 1);
    rtc_set_port_die(index, 1);
    rtc_set_port_pd(index, edge);
    rtc_set_port_pu(index, !edge);

    rtc_clr_wakeup_pending(index);
    rtc_set_wakeup_en(index, 1);
    rtc_set_wakeup_edge(index, !edge);
}

