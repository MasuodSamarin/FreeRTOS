/*********************************************************************************************
    *   Filename        : async_timer.c

    *   Description     : Asynchronous period timer

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2016-08-04 09:11

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/
#include "async_timer.h"
#include "list.h"
#include "os/rtos.h"
#include "task_schedule.h"


#define MAX_ASYNC_TIMER_PERIOD_MS        (10000>>1)

struct timer_t {
    struct list_head entry;
    void (*func)(void);
    u32 period_ms;
};

struct async_timer_hdl {
    u32 ticks;
    struct list_head head;
    void *timer;
    OS_SEM sem;
};

static struct async_timer_hdl hdl;

#define __this  (&hdl)


//API
static void *timer_malloc(int size)
{
    void *p;

    p = malloc(size);
    ASSERT(p != NULL, "%s:%s/%s", __FILE__, __LINE__, __func__);

    /* printf("async timer malloc : %x\n", p); */
    return p;
}

static void timer_free(void *p)
{
    /* printf("async timer free : %x\n", p); */
    free(p);
}


static void __async_timer_wakeup(void)
{
    os_sem_post(&__this->sem);
}

static void __async_timer_sleep(void)
{
    os_sem_pend(&__this->sem, 0);
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

void async_timer_schedule_probe(void)
{
    __async_timer_wakeup();
}

static void __async_timer_schedule_post(void)
{
    struct timer_t *p;
    /* puts("\n**********"); */
    /* puts(TASK_NAME_ASYNC_TIMER); */
    /* puts("**********\n"); */

    while (1) {
        /* IO_DEBUG_TOGGLE(B, 3); */

        __this->ticks++;

        list_for_each_entry(p, &__this->head, entry) {
            if ((__this->ticks % p->period_ms) == 0) {
                /* printf("period_ms : %x - func : %x\n", p->period_ms, p->func); */
                p->func();
            }
        }

        if (__this->ticks == MAX_ASYNC_TIMER_PERIOD_MS) {
            __this->ticks = 0;
        }

        __async_timer_sleep();
    }
}


void periodic_async_timer_init(void)
{
    u8 err;

    err = os_sem_create(&__this->sem, 1);

    err = os_task_create(__async_timer_schedule_post, (void *)0, TASK_PRIO_ASYNC_TIMER, 10, TASK_NAME_ASYNC_TIMER);

    INIT_LIST_HEAD(&__this->head);
}

void *periodic_async_timer_add(u32 period_ms, void (*func)(void))
{
    struct timer_t *timer;

    CPU_SR_ALLOC();
    OS_ENTER_CRITICAL();

    timer = timer_malloc(sizeof(*timer));

    timer->func = func;
    timer->period_ms = period_ms >> 1;
    __timer_insert(timer);

    OS_EXIT_CRITICAL();

    return timer;
}

void periodic_async_timer_remove(void *priv)
{
    struct timer_t *timer = (struct timer_t *)priv;

    CPU_SR_ALLOC();
    OS_ENTER_CRITICAL();

    list_del(&timer->entry);

    timer_free(timer);

    OS_EXIT_CRITICAL();
}





