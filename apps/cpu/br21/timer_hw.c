/*********************************************************************************************
    *   Filename        : timer_hw.c

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2016-07-12 10:36

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/
#include "my_malloc.h"
#include "timer_interface.h"
#include "interrupt.h"
#include "irq_interface.h"
#include "clock_interface.h"
#include "list.h"
#include "init.h"
#include "sdk_cfg.h"

#define MAX_TIME_CNT    0x7fff
#define MIN_TIME_CNT    0x100

#define MAX_TIMER_PERIOD_MS        (10000>>1)

struct timer_hdl {
    u32 ticks;
    struct list_head head;
};

struct timer_t {
    struct list_head entry;
    void (*func)(void);
    u32 period_ms;
};

static struct timer_hdl hdl;

#define __this  (&hdl)

static inline void user_power_off(struct timer_hdl *hdl);

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


static void __timer0_isr(void)
{
    struct timer_t *p;

    /* IO_DEBUG_1(3); */
    JL_TIMER0->CON |= BIT(14);

    __this->ticks++;

    /* puts("*"); */
    list_for_each_entry(p, &__this->head, entry) {
        if ((__this->ticks % p->period_ms) == 0) {
            /* printf("period_ms : %d - func : %x\n", p->period_ms, p->func); */
            p->func();
        }
    }

    if (__this->ticks == MAX_TIMER_PERIOD_MS) {
        __this->ticks = 0;
    }
    /* IO_DEBUG_0(3); */
}
IRQ_REGISTER(IRQ_TIME0_IDX, __timer0_isr);


static void __periodic_timer0_init(void)
{
    /* puts("early_initcall - __timer0_init\n"); */
    u32 clk;
    u32 prd_cnt;
    u8 index;

    clk = APP_TIMER_CLK;

    clk /= 1000;
    clk *= 2;

    for (index = 0; index < (sizeof(timer_div) / sizeof(timer_div[0])); index++) {
        prd_cnt = clk / timer_div[index];
        if (prd_cnt > MIN_TIME_CNT && prd_cnt < MAX_TIME_CNT) {
            break;
        }
    }
    INIT_LIST_HEAD(&__this->head);

    IRQ_REQUEST(IRQ_TIME0_IDX, __timer0_isr, 1);

    JL_TIMER0->CNT = 0;
    JL_TIMER0->PRD = prd_cnt;
    /* JL_TIMER0->CON = (index << 4) | BIT(0); */
    JL_TIMER0->CON = (index << 4) | BIT(0) | BIT(3);
}
early_initcall(__periodic_timer0_init);


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
    period_ms = (period_ms < 2) ? 2 : period_ms;
    timer->period_ms = period_ms >> 1;      //Unit 1ms->HW timer Unit 2ms
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



