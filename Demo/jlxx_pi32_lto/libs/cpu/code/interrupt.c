/*********************************************************************************************
    *   Filename        : interrupt.c

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2017-01-10 19:34

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/
#include "irq_interface.h"
#include "interrupt.h"
#include "init.h"


AT_IRQ_HW_CODE
void irq_init(void)
{
    ASSERT(__irq_drv->init, "%s", __func__);
    __irq_drv->init();
    __irq_drv->normal_disable();
}



AT_IRQ_HW_CODE
void irq_handler_register(u32 idx, void *hdl, u32 prio)
{
    ASSERT(__irq_drv->handler_register, "%s", __func__);
    __irq_drv->handler_register(idx, hdl, prio);
}


AT_IRQ_HW_CODE
void irq_handler_unregister(u32 idx)
{
    ASSERT(__irq_drv->handler_unregister, "%s", __func__);
    __irq_drv->handler_unregister(idx);
}


AT_IRQ_HW_CODE
void irq_common_handler(u32 idx)
{
    ASSERT(__irq_drv->commmon_handler, "%s", __func__);
    __irq_drv->commmon_handler(idx);
}


AT_IRQ_HW_CODE
void irq_enable(u32 idx)
{
    ASSERT(__irq_drv->enable, "%s", __func__);
    __irq_drv->enable(idx);
}

AT_IRQ_HW_CODE
bool irq_status(u32 idx)
{
    ASSERT(__irq_drv->enable, "%s", __func__);
    return __irq_drv->status(idx);
}

AT_IRQ_HW_CODE
void irq_disable(u32 idx)
{
    ASSERT(__irq_drv->disable, "%s", __func__);
    __irq_drv->disable(idx);
}


AT_IRQ_HW_CODE
void irq_global_enable(void)
{
    /* u32 tmp; */
    /* __asm__ volatile ("mov %0,ie0" : "=r"(tmp)); */
    /* printf("ie0 =%08x \n",tmp); */
    /* __asm__ volatile ("mov %0,ie1" : "=r"(tmp)); */
    /* printf("ie1 =%08x \n",tmp); */
    /* __asm__ volatile ("mov %0,icfg" : "=r"(tmp)); */
    /* printf("icfg =%08x \n",tmp); */

    ASSERT(__irq_drv->normal_enable, "%s", __func__);
    __irq_drv->normal_enable();
}


AT_IRQ_HW_CODE
void irq_global_disable(void)
{
    ASSERT(__irq_drv->normal_disable, "%s", __func__);
    __irq_drv->normal_disable();
}


AT_IRQ_HW_CODE
void irq_sys_enable(void)
{
    ASSERT(__irq_drv->sys_enable, "%s", __func__);
    __irq_drv->sys_enable();
}


AT_IRQ_HW_CODE
void irq_sys_disable(void)
{
    ASSERT(__irq_drv->sys_disable, "%s", __func__);
    __irq_drv->sys_disable();
}


AT_IRQ_HW_CODE
void irq_save(void)
{

}


AT_IRQ_HW_CODE
void irq_restore(void)
{

}

