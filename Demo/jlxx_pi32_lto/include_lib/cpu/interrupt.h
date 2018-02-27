/*******************************************************************************************
 *
 *   File Name: .h
 *
 *   Version: 1.00
 *
 *   Discription:
 *
 *   Author:Bingquan Cai
 *
 *   Email :bingquan_cai@zh-jieli.com
 *
 *   Date:
 *
 *   Copyright:(c)JIELI  2016  @ , All Rights Reserved.
 *
 *   *******************************************************************************************/

#ifndef __INTERRUPT_API_H__
#define __INTERRUPT_API_H__

#include "typedef.h"



/********************************************************************************/
/*
 *      API
 */
/**
 * @brief irq_init
 */
void irq_init(void);

/**
 * @brief irq_handler_register
 *
 * @param idx
 * @param hdl
 * @param prio
 */
void irq_handler_register(u32 idx, void *hdl, u32 prio);

/**
 * @brief irq_handler_unregister
 *
 * @param idx
 */
void irq_handler_unregister(u32 idx);

/**
 * @brief irq_common_handler
 *
 * @param idx
 */
void irq_common_handler(u32 idx);

/**
 * @brief irq_enable
 *
 * @param idx
 */
void irq_enable(u32 idx);

/**
 * @brief irq_read
 *
 * @param idx
 */
bool irq_status(u32 idx);

/**
 * @brief irq_disable
 *
 * @param idx
 */
void irq_disable(u32 idx);

/**
 * @brief irq_global_enable
 */
void irq_global_enable(void);

/**
 * @brief irq_global_disable
 */
void irq_global_disable(void);

/**
 * @brief irq_sys_enable
 */
void irq_sys_enable(void);

/**
 * @brief irq_sys_disable
 */
void irq_sys_disable(void);

#define IRQ_REGISTER(idx, hdl) \
    SET(interrupt("")) void irq_##hdl() \
    {\
        hdl();\
        irq_common_handler(idx);\
    }

#define IRQ_REQUEST(idx, hdl, prio) \
    irq_handler_register(idx, irq_##hdl, prio)

#define IRQ_RELEASE(idx) \
    irq_handler_unregister(idx)

#endif
