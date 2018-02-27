/*********************************************************************************************
    *   Filename        : sys_timer.h

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2016-07-19 10:02

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/
#ifndef SYS_TIMER_H
#define SYS_TIMER_H


#include "typedef.h"
#include "list.h"

struct sys_timer {
    struct list_head entry;
    void (*fun)(struct sys_timer *);
    u32 jiffies;
    u32 user;
};


void sys_timer_register(struct sys_timer *timer, u32 msec,
                        void (*fun)(struct sys_timer *timer));

void sys_timer_remove(struct sys_timer *timer);

void sys_timer_reset(struct sys_timer *timer, u32 msec);

void sys_timer_set_user(struct sys_timer *timer, u32 user);

u32 sys_timer_get_user(struct sys_timer *timer);

void sys_timer_init(u32 nms);

void sys_timer_schedule();




#endif

