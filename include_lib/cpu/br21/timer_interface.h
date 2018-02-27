/*********************************************************************************************
    *   Filename        : timer_interface.h

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2016-07-12 10:31

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/
#ifndef __TIMER_INTERFACE_H__
#define __TIMER_INTERFACE_H__

#include "typedef.h"

/**
 * @brief add periodic event
 *
 * @param ms
 * @param func
 *
 * @return
 */
void *periodic_timer_add(u32 ms, void (*func)(void));

/**
 * @brief delete periodic event
 *
 * @param priv
 */
void periodic_timer_remove(void *priv);

#endif /* __TIMER_INTERFACE_H__ */
