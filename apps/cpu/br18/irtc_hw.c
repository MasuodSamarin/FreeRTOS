#include "malloc.h"
#include "irtc_hw.h"
#include "interrupt.h"
#include "irq_interface.h"
#include "clock_interface.h"
#include "list.h"
#include "init.h"
#include "sdk_cfg.h"

/***********************************************
  name     : irtc_sr_byte
  parameter :   u8 data   --------- register value
  return       IRTC_BUF
  note  	read and write RTC register
************************************************/
static u8 irtc_sr_byte(u8 data)
{
    u8 i;

    for (i = 0; i < 8; i++) {
        if (data & BIT(7)) {
            IRTC_DO(1); //DATA OUT 1
        } else {
            IRTC_DO(0); //DATA OUT 0
        }

        IRTC_CLK(1);    //CLK OUT 1
        data <<= 1;      //delay
        IRTC_CLK(0);    //CLK OUT 0

        if (IRTC_IS_DI()) {
            data |= BIT(0);
        }
    }

    return data;
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
        puts("--X512--\n");
    }

    if (IRTC_IS_2Hz()) {
        IRTC_CLEAR_2Hz();
        puts("--X2--\n");
    }
    if (IRTC_IS_WAKEUP()) {
        IRTC_CLEAR_WAKEUP();
        puts("--Wakeup--\n");

        pending = __read_rtc_reg(RTC_CON_RD);
        if (IRTC_IS_ALMOUT(pending)) {  //alarm out

        }

        pending = __read_rtc_reg(WKUP_EN_RD);
        pending = __read_rtc_reg(WKUP_PND_RD) & pending;
        if (IRTC_IS_WAKEUP_IO0(pending)) {
            /* puts("-WK_PR0\n"); */
            __write_rtc_reg(WKUP_CLR_PND_WR, pending | BIT(0 + 4));
        }
        if (IRTC_IS_WAKEUP_IO1(pending)) {
            __write_rtc_reg(WKUP_CLR_PND_WR, pending | BIT(1 + 4));
        }
        if (IRTC_IS_WAKEUP_IO2(pending)) {
            __write_rtc_reg(WKUP_CLR_PND_WR, pending | BIT(2 + 4));
        }
        if (IRTC_IS_WAKEUP_IO3(pending)) {
            __write_rtc_reg(WKUP_CLR_PND_WR, pending | BIT(3 + 4));
        }

        pending = __read_rtc_reg(ALL_WKUP_SRC_RD);
        if (IRTC_IS_WAKEUP_PORT(pending)) {
            puts("IRTC_IS_WAKEUP_PORT\n");
        }
        if (IRTC_IS_WAKEUP_LDOIN(pending)) {
        }
    }
}
IRQ_REGISTER(IRQ_RTC_IDX, rtc_irq_handler);

void __rtc_debug(void)
{
    u8 reg;

    reg = __read_rtc_reg(LDO_CON_RD);
    printf("LDO_CON_RD : %x\n", reg);
    reg = __read_rtc_reg(RTC_PR_HD_RD);
    printf("RTC_PR_HD_RD : %x\n", reg >> 4 & 0xf);
    reg = __read_rtc_reg(RTC_PR_OUT_RD);
    printf("RTC_PR_OUT_RD : %x\n", reg & 0xf);
    reg = __read_rtc_reg(RTC_PR_DIR_RD);
    printf("RTC_PR_DIR_RD : %x\n", reg >> 4 & 0xf);
    reg = __read_rtc_reg(RTC_PR_PU_RD);
    printf("RTC_PR_PU_RD : %x\n", reg & 0xf);
    reg = __read_rtc_reg(RTC_PR_PD_RD);
    printf("RTC_PR_PD_RD : %x\n", reg >> 4 & 0xf);
    reg = __read_rtc_reg(RTC_PR_DIE_RD);
    printf("RTC_PR_DIE_RD : %x\n", reg & 0xf);
    reg = __read_rtc_reg(WKUP_EN_RD);
    printf("WKUP_EN_RD : %x\n", reg & 0xf);
    reg = __read_rtc_reg(WKUP_EDGE_RD);
    printf("WKUP_EDGE_RD : %x\n", reg >> 4 & 0xf);
}

static void __rtc_set_port_hd(u8 index, bool set)
{
    u8 reg;

    reg = __read_rtc_reg(RTC_PR_HD_RD);
    reg = (set) ? reg | BIT(index + 4) : reg & ~BIT(index + 4) ;
    __write_rtc_reg(RTC_PR_HD_WR, reg);
}

static void __rtc_set_port_out(u8 index, bool set)
{
    u8 reg;

    reg = __read_rtc_reg(RTC_PR_OUT_RD);
    reg = (set) ? reg | BIT(index) : reg & ~BIT(index);
    __write_rtc_reg(RTC_PR_OUT_WR, reg);
}

static void __rtc_set_port_dir(u8 index, bool set)
{
    u8 reg;

    reg = __read_rtc_reg(RTC_PR_DIR_RD);
    reg = (set) ? reg | BIT(index + 4) : reg & ~BIT(index + 4) ;
    __write_rtc_reg(RTC_PR_DIR_WR, reg);
}

static void __rtc_set_port_pu(u8 index, bool set)
{
    u8 reg;

    reg = __read_rtc_reg(RTC_PR_PU_RD);
    reg = (set) ? reg | BIT(index) : reg & ~BIT(index);
    __write_rtc_reg(RTC_PR_PU_WR, reg);
}

static void __rtc_set_port_pd(u8 index, bool set)
{
    u8 reg;

    reg = __read_rtc_reg(RTC_PR_PD_RD);
    reg = (set) ? reg | BIT(index + 4) : reg & ~BIT(index + 4) ;
    __write_rtc_reg(RTC_PR_PD_WR, reg);
}

static void __rtc_set_port_die(u8 index, bool set)
{
    u8 reg;

    reg = __read_rtc_reg(RTC_PR_DIE_RD);
    reg = (set) ? reg | BIT(index) : reg & ~BIT(index);
    __write_rtc_reg(RTC_PR_DIE_WR, reg);
}

static void __rtc_set_port_reset(u8 index, bool level, bool enable)
{
    u8 reg;

    reg = __read_rtc_reg(RTC_PR_RESET_RD);
    reg = (level) ? reg | BIT(7) : reg & ~BIT(7);
    reg = reg | index << 4;
    reg = reg | enable << 6;
    __write_rtc_reg(RTC_PR_RESET_WR, reg);
}

static void __rtc_set_wakeup_en(u8 index, bool set)
{
    u8 reg;

    reg = __read_rtc_reg(WKUP_EN_RD);
    reg = (set) ? reg | BIT(index) : reg & ~BIT(index);
    __write_rtc_reg(WKUP_EN_WR, reg);
}

static void __rtc_set_wakeup_edge(u8 index, bool edge)
{
    u8 reg;

    reg = __read_rtc_reg(WKUP_EDGE_RD);
    reg = (edge) ? reg & ~BIT(index + 4) : reg | BIT(index + 4) ;
    __write_rtc_reg(WKUP_EDGE_WR, reg);
}

static void __rtc_clr_wakeup_pending(u8 index)
{
    u8 reg;

    reg = __read_rtc_reg(WKUP_PND_RD);
    reg |= BIT(index + 4);
    __write_rtc_reg(WKUP_CLR_PND_WR, reg);
}


/**
 * @brief rtc_set_wakeup_io
 *
 * @param index : PR number
 * @param edge : 1 - rise edge ; 0 - fall edge
 */
void rtc_set_wakeup_io(u8 index, bool edge)
{
    __rtc_set_port_dir(index, 1);
    __rtc_set_port_die(index, 1);
    __rtc_set_port_pd(index, edge);
    __rtc_set_port_pu(index, !edge);

    __rtc_clr_wakeup_pending(index);
    __rtc_set_wakeup_en(index, 1);
    __rtc_set_wakeup_edge(index, edge);

    /* __rtc_debug(); */
}

void rtc_set_io_highz(void)
{
    u8 reg;

    reg = __read_rtc_reg(RTC_PR_DIR_RD);
    __write_rtc_reg(RTC_PR_DIR_WR, reg | 0xf0);

    reg = __read_rtc_reg(RTC_PR_DIE_RD);
    __write_rtc_reg(RTC_PR_DIE_WR, reg & 0xf0);

    __write_rtc_reg(RTC_PR_PD_WR, 0x0);
}

static void rtc_init(void)
{
    u8 reg;

    reg = __read_rtc_reg(RTC_CON_RD);

    IRTC_CLEAR_WAKEUP();
    IRTC_CLEAR_512Hz();
    IRTC_CLEAR_2Hz();

    __write_rtc_reg(WKUP_EN_WR, 0x00);//禁止唤醒
    __write_rtc_reg(WKUP_CLR_PND_WR, 0xf0);//clr_pnd

    /* IRTC_X2Hz_IE(1); */
    /* IRTC_WAKEUP_IE(1); */

    IRQ_REQUEST(IRQ_RTC_IDX, rtc_irq_handler, 1) ; //rtc_isr
}
early_initcall(rtc_init);
