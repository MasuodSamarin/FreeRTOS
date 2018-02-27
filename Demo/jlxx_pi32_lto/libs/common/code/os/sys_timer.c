/*********************************************************************************************
    *   Filename        : sys_timer.c

    *   Description     : Synchronous timer for system timeout

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2016-07-12 14:22

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/
#include "sys_timer.h"
#include "jiffies.h"
#include "os/rtos.h"


struct sys_timer_hdl {
    struct list_head timer_head;
    volatile unsigned long jiffies;
};
static struct sys_timer_hdl hdl;

unsigned long ticks;

#define __this  (&hdl)

u32 get_jiffies(void)
{
    return __this->jiffies;
}

AT_SYS_TIMER_CODE
static bool __timer_is_listed(struct sys_timer *timer)
{
    struct sys_timer *p;

    list_for_each_entry(p, &__this->timer_head, entry) {
        if (p == timer) {
            return TRUE;
        }
    }
    return FALSE;
}


AT_SYS_TIMER_CODE
static void __timer_insert(struct sys_timer *timer)
{
    struct sys_timer *p, *n;

    list_for_each_entry_safe(p, n, &__this->timer_head, entry) {
        if (p == timer) {
            list_del(&p->entry);
            break;
        }
    }

    list_for_each_entry(p, &__this->timer_head, entry) {
        if (p->jiffies > timer->jiffies) {
            __list_add(&timer->entry, p->entry.prev, &p->entry);
            return;
        }
    }

    list_add_tail(&timer->entry, &__this->timer_head);
}



AT_SYS_TIMER_CODE
void sys_timer_register(struct sys_timer *timer, u32 msec,
                        void (*fun)(struct sys_timer *timer))
{
    CPU_SR_ALLOC();

    OS_ENTER_CRITICAL();

    timer->fun = fun;
    msec = (msec < SYS_TICKS) ? SYS_TICKS : msec;
    timer->jiffies = __this->jiffies + msecs_to_jiffies(msec);
    __timer_insert(timer);

    OS_EXIT_CRITICAL();
}


AT_SYS_TIMER_CODE
void sys_timer_reset(struct sys_timer *timer, u32 msec)
{
    CPU_SR_ALLOC();

    OS_ENTER_CRITICAL();

    msec = (msec < SYS_TICKS) ? SYS_TICKS : msec;
    timer->jiffies = __this->jiffies + msecs_to_jiffies(msec);
    __timer_insert(timer);

    OS_EXIT_CRITICAL();
}



AT_SYS_TIMER_CODE
void sys_timer_remove(struct sys_timer *timer)
{
    CPU_SR_ALLOC();

    OS_ENTER_CRITICAL();
    /* printf("timeout->entry.prev %x\n", timer->entry.prev); */
    /* printf("timeout->entry.next %x\n", timer->entry.next); */
    if (__timer_is_listed(timer) == FALSE) {
        OS_EXIT_CRITICAL();
        return;
    }
    ASSERT(timer->entry.prev, "%s\n", "timer del prev NULL\n");
    ASSERT(timer->entry.next, "%s\n", "timer del next NULL\n");
    list_del(&timer->entry);
    OS_EXIT_CRITICAL();
}



AT_SYS_TIMER_CODE
void sys_timer_schedule()
{
    struct sys_timer *p, *n;

    __this->jiffies++;

    list_for_each_entry_safe(p, n, &__this->timer_head, entry) {
        if (time_before(__this->jiffies, p->jiffies)) {
            break;
        }
        if (list_empty(&p->entry)) {
            //guarantee
            continue;
        }
        sys_timer_remove(p);
        p->fun(p);
    }
#if 0
    while (!list_empty(&__this->timer_head)) {
        /*putchar('x');*/
        p = list_first_entry(&__this->timer_head, struct sys_timer, entry);
        if (time_after(__this->jiffies, p->jiffies)) {
            sys_timer_remove(p);
            p->fun(p);
            putchar('d');
        }
    }
#endif
}



AT_SYS_TIMER_CODE
void sys_timer_init(u32 nms)
{
    INIT_LIST_HEAD(&__this->timer_head);
    ticks = nms;
}


AT_SYS_TIMER_CODE
void sys_timer_set_user(struct sys_timer *timer, u32 user)
{
    timer->user = user;
}


AT_SYS_TIMER_CODE
u32 sys_timer_get_user(struct sys_timer *timer)
{
    return timer->user;
}

