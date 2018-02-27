/*********************************************************************************************
    *   Filename        : async_timer.h

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2016-08-04 09:22

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/
#ifndef __ASYNC_TIMER_H__
#define __ASYNC_TIMER_H__

#include "typedef.h"

/**
 * @brief Asynchronous Periodic Timer init
 */
void periodic_async_timer_init(void);

/**
 * @brief Asynchronous Periodic Timer Pre-Process
 */
void async_timer_schedule_probe(void);

/**
 * @brief Asynchronous Periodic Timer Add
 *
 * @param period_ms(unit:1ms)
 * @param func(user event)
 *
 * @return Asynchronous Periodic Timer handler
 */
void *periodic_async_timer_add(u32 period_ms, void (*func)(void));

/**
 * @brief Asynchronous Periodic Timer remove
 *
 * @param priv(handler)
 */
void periodic_async_timer_remove(void *priv);

#endif /* __ASYNC_TIMER_H__ */
