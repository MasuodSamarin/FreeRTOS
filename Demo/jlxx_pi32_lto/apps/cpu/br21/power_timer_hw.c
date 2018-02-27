/*********************************************************************************************
    *   Filename        : power_timer_hw.c

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2017-08-07 09:04

    *   Copyright:(c)JIELI  2011-2017  @ , All Rights Reserved.
*********************************************************************************************/
#include "malloc.h"
#include "timer_interface.h"
#include "interrupt.h"
#include "irq_interface.h"
#include "clock_interface.h"
#include "list.h"
#include "init.h"
#include "sdk_cfg.h"
#include "power_interface.h"
#include "jiffies.h"

struct timer_hdl {
    u32 ticks;
    int index;
    int prd;
    u32 fine_cnt;
    void *power_ctrl;
    struct list_head head;
};

struct timer_t {
    struct list_head entry;
    void (*func)(void);
    u32 period_ms;
};

static struct timer_hdl hdl;

#define __this  (&hdl)

static inline void timer_power_get(void);
static inline void timer_power_put(void);
static inline void timer_power_on(void);
static inline void timer_power_off(void);

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

#define MAX_TIME_CNT    0x7fff
#define MIN_TIME_CNT    0x100


#define TIMER_CON       JL_TIMER1->CON
#define TIMER_CNT       JL_TIMER1->CNT
#define TIMER_PRD       JL_TIMER1->PRD
#define TIMER_VETOR     IRQ_TIME1_IDX
#define TIMER_UNIT_MS   APP_TICKS_UNIT

#define MAX_TIMER_PERIOD_MS        (1000/TIMER_UNIT_MS)

static void __timer_isr(void)
{
    struct timer_t *p;

    /* putchar('w'); */
    timer_power_on();
    /* IO_DEBUG_1(3); */
    TIMER_CON |= BIT(14);

    __this->ticks++;

    list_for_each_entry(p, &__this->head, entry) {
        if ((__this->ticks % p->period_ms) == 0) {
            /* printf("period_ms : %d - func : %x\n", p->period_ms, p->func); */
            p->func();
        }
    }

    if (__this->ticks == MAX_TIMER_PERIOD_MS) {
        /* puts("@"); */
        __this->ticks = 0;
    }

    timer_power_off();
    /* IO_DEBUG_0(3); */
}
IRQ_REGISTER(TIMER_VETOR, __timer_isr);


static void __periodic_timer_init(void)
{
    /* puts("early_initcall - __timer0_init\n"); */
    u32 prd_cnt;
    u8 index;

    //20ms = PRD * DIV / TIMER_CLK
    //PRD = 20 * (TIMER_CLK / 1000) / DIV
    for (index = 0; index < (sizeof(timer_div) / sizeof(timer_div[0])); index++) {
        prd_cnt = TIMER_UNIT_MS * (APP_TIMER_CLK / 1000) / timer_div[index];
        if (prd_cnt > MIN_TIME_CNT && prd_cnt < MAX_TIME_CNT) {
            break;
        }
    }
    INIT_LIST_HEAD(&__this->head);

    IRQ_REQUEST(TIMER_VETOR, __timer_isr, 1);

    __this->index   = index;
    __this->prd     = prd_cnt;

    TIMER_CNT = 0;
    TIMER_PRD = __this->prd;

    /* TIMER_CON = (index << 4) | BIT(0); */
    TIMER_CON = (__this->index << 4) | BIT(0) | BIT(3);

    //wait until bt power get
    power_off_lock();
    timer_power_get();
}
early_initcall(__periodic_timer_init);


//API
static void *timer_malloc(int size)
{
    void *p;

    p = malloc(size);
    ASSERT(p != NULL, "%s:%s/%s", __FILE__, __LINE__, __func__);

    /* printf("timer malloc : %x\n", p); */
    return p;
}

static void timer_free(void *p)
{
    /* printf("timer free : %x\n", p); */
    free(p);
}

static bool __timer_is_listed(struct timer_t *timer)
{
    struct timer_t *p;

    list_for_each_entry(p, &__this->head, entry) {
        if (p == timer) {
            return TRUE;
        }
    }
    return FALSE;
}

static void __timer_insert(struct timer_t *timer)
{
    struct timer_t *p;

    list_for_each_entry(p, &__this->head, entry) {
        if (p == timer) {
            list_del(&p->entry);
            break;
        }
    }

    //sort the list << samll -> large
    list_for_each_entry(p, &__this->head, entry) {
        if (p->period_ms > timer->period_ms) {
            __list_add(&timer->entry, p->entry.prev, &p->entry);
            return;
        }
    }

    list_add_tail(&timer->entry, &__this->head);
}

void *periodic_timer_add(u32 period_ms, void (*func)(void))
{
    struct timer_t *timer;

    CPU_SR_ALLOC();

    OS_ENTER_CRITICAL();

    timer = timer_malloc(sizeof(*timer));

    timer->func = func;
    period_ms = (period_ms < TIMER_UNIT_MS) ? TIMER_UNIT_MS : period_ms;
    timer->period_ms = period_ms / TIMER_UNIT_MS;    //Unit 1ms->HW timer Unit 2ms
    __timer_insert(timer);

    OS_EXIT_CRITICAL();

    return timer;
}

void periodic_timer_remove(void *priv)
{
    struct timer_t *timer = (struct timer_t *)priv;

    CPU_SR_ALLOC();

    OS_ENTER_CRITICAL();

    list_del(&timer->entry);

    timer_free(timer);

    OS_EXIT_CRITICAL();
}

u32 time_record[5];

static u32 __power_get_timeout(void *priv)
{
    u32 clkn;

    clkn = TIMER_PRD - TIMER_CNT;

    /* printf("timer clkn : %d / %d\n", TIMER_PRD, TIMER_CNT); */
    /* puts("get_timeout :\n"); */
    /* put_u32hex(TIMER_PRD);put_u32hex(TIMER_CNT); */
    time_record[0] = clkn;

    clkn = clkn * timer_div[__this->index] / (APP_TIMER_CLK / 1000000L);

    /* printf("-timer clkn : %d\n", clkn); */
    /* printf("--timer Div : %d/ Prd : %d\n", timer_div[__this->index], __this->prd); */

    return clkn < 4 * 625 ? 0 : clkn - 4 * 625;
    /* return clkn < 2 * 625 ? 0 : clkn - 2 * 625; */
}

static void __power_suspend_post(void *priv, u32 usec)
{
    TIMER_CON &= ~BIT(0);
    /* printf("usec 2 cnt : %d\n", usec); */
    __this->fine_cnt = usec * (APP_TIMER_CLK / 1000000L) / timer_div[__this->index];
    /* printf("fine cnt : %d\n", __this->fine_cnt); */

    u32 clkn;

    clkn = TIMER_PRD - TIMER_CNT;

    time_record[1] = clkn;
    ASSERT((clkn > __this->fine_cnt), "%d > %d timer %s\n", clkn, __this->fine_cnt, __func__);
}


static void __power_resume(void *priv, u32 usec)
{
    time_record[2] = TIMER_PRD - TIMER_CNT;
    /* printf("\n0 - time %d\n", time_record[0]); */
    /* printf("1 - time %d\n", time_record[1]); */
    /* printf("2 - time %d\n", time_record[2]); */
    /* printf("3 - fine time %d\n", __this->fine_cnt); */
    /* TIMER_CNT += usec * (TIMER_CLK/1000000L) / timer_div[__this->index]; */
    /* ASSERT((TIMER_CNT + __this->fine_cnt) < TIMER_PRD, "%d < %d %s\n", TIMER_CNT, TIMER_PRD, __func__); */
    TIMER_CNT += __this->fine_cnt;
    /* printf("4 - TIMER_CNT %d\n", TIMER_CNT); */
    /* printf("5 - TIMER_PRD %d\n", TIMER_PRD); */
    /* printf("6 - time con 0x%x\n", TIMER_CON); */
    TIMER_CON |= BIT(0);
}

const struct power_operation timer_power_ops  = {
    .get_timeout 	= __power_get_timeout,
    .suspend_probe 	= NULL,
    .suspend_post 	= __power_suspend_post,
    .resume 		= __power_resume,
};

static inline void timer_power_get(void)
{
    __this->power_ctrl = power_get(NULL, &timer_power_ops);
}

static inline void timer_power_put(void)
{
    power_put(__this->power_ctrl);
}

static inline void timer_power_on(void)
{
    power_on(__this->power_ctrl);
}

static inline void timer_power_off(void)
{
    power_off(__this->power_ctrl);
}
