#include "typedef.h"
#include "stdarg.h"
/* #include "RF.h" */
#include "power_hw.h"
#include "power_interface.h"
#include "power_api.h"
#include "spi_interface.h"
#include "icache_interface.h"

/* #define LOG_INFO_ENABLE */
#include "debug_log.h"

#pragma str_literal_override("bt_poweroff_reboot.rodata")

/* #define PF_DEBUG_EN */



struct rom_entry {
    u8 *boot_addr;
    u8 magic_addr[8];
};

/* static struct rom_entry     rom_entry_t SEC(.ram1_tail); */

/* #define __this      (&rom_entry_t) */

typedef enum {
    RUN_RAM = 0,
    MAX_INS,
    RUN_NORMAL = 0xff,
} SKIP_INS;

struct rom_interface {
    void (*maskrom_call)(SKIP_INS cmd);
    void (*maskrom_clear)(void);
    SKIP_INS(*maskrom_boottype)(void);
    void (*set_exception_isr_hook)(void *);
    void (*exception_init)(void);
};


int (*__maskrom_version)(void) = (int (*)(void))0x50004;

static const struct rom_interface rom_api_V1 = {
    .maskrom_call           = (void (*)(SKIP_INS))0x5000c,
    .maskrom_clear          = (void (*)(void))0x50024,
    .maskrom_boottype       = (SKIP_INS(*)(void))0x50034,
    .set_exception_isr_hook = (void (*)(void *))0x50054,
    .exception_init         = (void (*)(void))0x50060,
};


const struct rom_interface *__rom_api;
extern void delay(u32 d);

AT_NON_VOLATILE_RAM_CODE
void maskrom_api_init(void)
{
    //printf("version %x\n", __maskrom_version());
    __rom_api = NULL;

    if (0x20160620 == __maskrom_version()) {
        /* log_info("rom ver.A\n"); */
        __rom_api = &rom_api_V1;
    }
    if (0x20161101 == __maskrom_version()) {
        /* log_info("rom ver.C\n"); */
        __rom_api = &rom_api_V1;
    }
}


/*******************************************************************/
/*
 *-------------------LE Power off dependency
 */

AT_NON_VOLATILE_RAM_CODE
static void pll_init_lite(void)
{
    u32 osc_freq;

    JL_CLOCK->SYS_DIV = 0;
    /* osc_freq = g_boot_pll_info & (0x1ff << 16); */
    /* osc_freq >>= 16; */
    osc_freq = 240;
    if (osc_freq == 0) {
        osc_freq = 32768;
    } else {
        osc_freq *= 100000;
    }

    osc_freq = osc_freq / 2000000 - 2;

    ///sys switch rc clk 250k
    JL_CLOCK->CLK_CON0 &= ~(3 << 2); //clk mux0 sel rc
    JL_CLOCK->CLK_CON0 &= ~BIT(8);   //sys in clk mux0

    JL_CLOCK->PLL_CON &= ~(BIT(0) | BIT(1)); //dis pll rst

    //BT osc
    JL_CLOCK->PLL_CON &= ~(3 << 8);

    JL_CLOCK->PLL_CON &= ~(0x1f << 2);
    JL_CLOCK->PLL_CON |= osc_freq << 2;
    JL_CLOCK->PLL_CON |= BIT(7);
    JL_CLOCK->PLL_CON &= ~BIT(16); //REF 2M
    JL_CLOCK->PLL_CON |= BIT(0);   //EN PLL
    ///wait 10 us
    delay(1);
    JL_CLOCK->PLL_CON |= BIT(1); //RST
    ///wait 300 us
    delay(10);

    //SFR(SYS_DIV, 8, 3, 1);
    ///pll div
    JL_CLOCK->CLK_CON2 &= ~(0x3f); //pll_192M
    JL_CLOCK->CLK_CON2 |= BIT(4);  //pll div4 sys_clk = 192/2 = 96M
    /* JL_CLOCK->CLK_CON2 |= BIT(5);  //pll div4 sys_clk = 192/4 = 48M */
    //sys switch pll clk 48M
    JL_CLOCK->CLK_CON0 |= (3 << 6); //clk mux1 sel pll
    JL_CLOCK->CLK_CON0 |= BIT(8);    //sys in clk mux1
}

/*******************************************************************/
/*
 *-------------------LE Power off dependency
 */
/* #include "ble_interface.h" */
#include "irq_interface.h"
#include "interrupt.h"
void RF_init();

AT_NON_VOLATILE_RAM_CODE
static void debug_ie()
{
    int tmp;
    __asm__ volatile("mov %0,icfg" : "=r"(tmp));
    log_info("icfg : \n");
    log_info(tmp);
    __asm__ volatile("mov %0,ie0" : "=r"(tmp));
    log_info("ie0 : \n");
    log_info(tmp);
    __asm__ volatile("mov %0,ie1" : "=r"(tmp));
    log_info("ie1 : \n");
    log_info(tmp);

}

AT_NON_VOLATILE_RAM_CODE
static void debug_stack(void)
{
    int tmp;
    __asm__ volatile("mov %0,usp" : "=r"(tmp));
    log_info("usp : \n");
    log_info(tmp);

    __asm__ volatile("mov %0,ssp" : "=r"(tmp));
    log_info("ssp : \n");
    log_info(tmp);

    __asm__ volatile("mov %0,sp" : "=r"(tmp));
    log_info("sp : \n");
    log_info(tmp);
}

static int ticks;

#define MAX_TIME_CNT    0x7fff
#define MIN_TIME_CNT    0x100

#define MAX_TIMER_PERIOD_MS        (10000>>1)

AT_NON_VOLATILE_RAM_CODE
static void __timer0_isr1(void)
{
    struct timer_t *p;

    /* IO_DEBUG_1(3); */
    JL_TIMER0->CON |= BIT(14);

    ticks++;

    if (ticks == 500) {
        ticks = 0;
        log_info("X");
        debug_stack();
    }
    /* IO_DEBUG_0(3); */
}
AT_NON_VOLATILE_RAM_CODE
IRQ_REGISTER(IRQ_TIME0_IDX, __timer0_isr1);


static const u32 timer_div[] = {
    /*0000*/    1,
    /*0001*/    4,
    /*0010*/    16,
    /*0011*/    64,
    /*0100*/    2,
    /*0101*/    8,
    /*0110*/    32,
    /*0111*/    128,
    /*1000*/    256,
    /*1001*/    4 * 256,
    /*1010*/    16 * 256,
    /*1011*/    64 * 256,
    /*1100*/    2 * 256,
    /*1101*/    8 * 256,
    /*1110*/    32 * 256,
    /*1111*/    128 * 256,
};
AT_NON_VOLATILE_RAM_CODE
static void __periodic_timer0_init(void)
{
    /* log_info("early_initcall - __timer0_init\n"); */
    u32 clk;
    u32 prd_cnt;
    u8 index;

    clk = 24000000L;

    clk /= 1000;
    clk *= 2;

    for (index = 0; index < (sizeof(timer_div) / sizeof(timer_div[0])); index++) {
        prd_cnt = clk / timer_div[index];
        if (prd_cnt > MIN_TIME_CNT && prd_cnt < MAX_TIME_CNT) {
            break;
        }
    }

    IRQ_REQUEST(IRQ_TIME0_IDX, __timer0_isr1, 1);

    JL_TIMER0->CNT = 0;
    JL_TIMER0->PRD = prd_cnt;
    JL_TIMER0->CON = (index << 4) | BIT(3) | BIT(0);
}

AT_NON_VOLATILE_RAM_CODE
static void exception_isr(void)
{
    u32 rets, reti;

    __asm__ volatile("mov %0,RETS" : "=r"(rets));
    __asm__ volatile("mov %0,RETI" : "=r"(reti));

    log_info("\n\n---------------------------SDK exception_isr---------------------\n\n");
    log_info(JL_DEBUG->MSG);

    if (JL_DEBUG->MSG & BIT(2)) {
        log_info("Peripheral ex limit err\n");
    }
    if (JL_DEBUG->MSG & BIT(8)) {
        log_info("DSP MMU err,DSP write MMU All 1\n");
    }
    if (JL_DEBUG->MSG & BIT(9)) {
        log_info("PRP MMU err,PRP write MMU 1\n");
    }
    if (JL_DEBUG->MSG & BIT(16)) {
        log_info("PC err,PC over limit\n");
    }
    if (JL_DEBUG->MSG & BIT(17)) {
        log_info("DSP ex err,DSP store over limit\n");
    }
    if (JL_DEBUG->MSG & BIT(18)) {
        log_info("DSP illegal,DSP instruction\n");
    }
    if (JL_DEBUG->MSG & BIT(19)) {
        log_info("DSP misaligned err\n");
    }

    u32 tmp;

    log_info("RETS : \n");
    log_info(rets);
    log_info("RETI : \n");
    log_info(reti);
    debug_stack();
    while (1);
}

const char Date[] = "\r\n*********LE SDK Main "__DATE__ " "__TIME__ "******\r\n";

typedef struct __FAST_SFC_SET {
    u32 flash_size;
    u32 flash_base;;
    u32 cfg_file_addr;
    u32 app_file_addr;
    u32 spi_mode;
    u32 flash_mode;
    u32 spi_div;
    u32 spi_run_mode;
    u32 pll_info;
    u16 spi_port;
    u16 dec_key;
} FAST_SFC_SET;

#define RAM1_SAVE_VALUES_ADDR   (RAM1_ENTRY_ADDR - sizeof(FAST_SFC_SET)) //(RAM1_END - ROM_USE_SIZE - SFC_INFO_SIZE)

AT_NON_VOLATILE_RAM_CODE
static void flash_recover(void)
{
    FAST_SFC_SET *sfc_info = (FAST_SFC_SET *)RAM1_SAVE_VALUES_ADDR;

    /* log_info("**flash_recover\n"); */
    /* printf_buf(sfc_info, sizeof(FAST_SFC_SET)); */

    __spi0_drv->ioctrl(SET_SPI_INIT, \
                       sfc_info->spi_mode, \
                       sfc_info->spi_port, \
                       sfc_info->spi_div);

    __icache_drv->ioctrl(CACHE_SET_SFC_INIT, \
                         sfc_info->spi_mode, \
                         sfc_info->flash_mode, \
                         sfc_info->spi_port, \
                         sfc_info->spi_run_mode, \
                         sfc_info->spi_div);

    __icache_drv->ioctrl(CACHE_SET_SFC_RUN, \
                         sfc_info->flash_base, \
                         sfc_info->dec_key, \
                         sfc_info->flash_size, \
                         sfc_info->cfg_file_addr, \
                         sfc_info->app_file_addr);
}


AT_NON_VOLATILE_RAM_CODE
void poweroff_post(void)
{
    /* close_wdt(); */
    int cnt;
    power_set_proweroff();

    /* IO_DEBUG_TOGGLE(A, 1); */
    /* IO_DEBUG_1(A, 0); */

    pll_init_lite();
    /* uart_init(460800L); */

    flash_recover();
    /* log_info((char *)Date); */

    /* if (bt_power_is_poweroff_post()) { */
    /* log_info("bt deep sleep wakeup\n"); */
    /* } */

    /* SYS_RESUME(); */
    /* log_info("YYY\n"); */
#if 1
    while (1) {
        /* IO_DEBUG_TOGGLE(A, 0); */
        delay(500000);        //for bt osc
        /* nop(5000);        //for rtc osc */
        /* log_info(cnt++); */
        log_info("a");

        /* __asm__ volatile("idle"); */
        /* __asm__ volatile("nop"); */
        /* __asm__ volatile("nop"); */
    }
#else
    irq_init();

    RF_poweroff_recover();//
    /* RF_init(); */

    __ble_ops->init();

    bt_poweroff_recover();

    irq_handler_register(0, exception_isr, 3);

    /* __periodic_timer0_init(); */

    /* debug_stack(); */

    irq_sys_disable();
    irq_sys_enable();
    irq_global_enable();

    while (1) {
        IO_DEBUG_TOGGLE(A, 1);
        /* delay(500000);        //for bt osc */
        /* putchar('*'); */
        __asm__ volatile("idle");
        __asm__ volatile("nop");
        __asm__ volatile("nop");
    }
#endif
}

extern void _reboot_start();

AT_NON_VOLATILE_RAM_CODE
void poweroff_probe(void)
{
    maskrom_api_init();

    ASSERT(__rom_api, "ROM VER error");

    *((u32 *)RAM1_ENTRY_ADDR) = (u32)_reboot_start;

    __rom_api->maskrom_call(RUN_RAM);
}
