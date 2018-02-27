/*******************************************************************************************
 *
 *   File Name: interrupt_driver.c
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
#include "irq_interface.h"
#include "list.h"

struct irq_data {
    struct list_head entry;
    u8 idx;
    u8 prio;
};

static struct irq_data irq[MAX_IRQ_ENTRY_NUM];

struct irq_hdl {
    struct list_head irq_head;
};

static struct irq_hdl hdl;

#define __this      (&hdl)


AT_IRQ_HW_CODE
static void __hw_irq_clr_pending(u32 irq_idx)
{
    if (irq_idx < 32) {
        JL_NVIC->ILAT0_CLR = BIT(irq_idx);
    } else {
        irq_idx = irq_idx - 32;
        JL_NVIC->ILAT1_CLR = BIT(irq_idx);
    }
}


AT_IRQ_HW_CODE
static void __hw_irq_disable(u32 index)
{
    int tmp;

    if (index < 32) {
        __asm__ volatile("mov %0,ie0" : "=r"(tmp));
        tmp &= ~(BIT(index) & 0xfffffffe);
        __asm__ volatile("mov ie0,%0" :: "r"(tmp));
    } else {
        index = index - 32;
        __asm__ volatile("mov %0,ie1" : "=r"(tmp));
        tmp &= ~BIT(index);
        __asm__ volatile("mov ie1,%0" :: "r"(tmp));
    }

}



AT_IRQ_HW_CODE
static void __hw_irq_enable(u32 index)
{
    int tmp;

    if (index < 32) {
        __asm__ volatile("mov %0,ie0" : "=r"(tmp));
        tmp |= (BIT(index) & 0xfffffffc);
        __asm__ volatile("mov ie0,%0" :: "r"(tmp));
    } else {
        index = index - 32;
        __asm__ volatile("mov %0,ie1" : "=r"(tmp));
        tmp |= BIT(index);
        __asm__ volatile("mov ie1,%0" :: "r"(tmp));
    }
}


AT_IRQ_HW_CODE
static void __hw_irq_set_prio(unsigned int index, unsigned int priority)
{
    unsigned int a, b;

    a =  index / 16;
    b = (index % 16) * 2;

    switch (a) {
    case 0 :
        JL_NVIC->IPCON0 &= ~(0x3 << (b));
        JL_NVIC->IPCON0 |= ((priority & 0x3) << (b));
        break ;
    case 1:
        JL_NVIC->IPCON1 &= ~(0x3 << (b));
        JL_NVIC->IPCON1 |= ((priority & 0x3) << (b));
        break ;
    case 2:
        JL_NVIC->IPCON2 &= ~(0x3 << (b));
        JL_NVIC->IPCON2 |= ((priority & 0x3) << (b));
        break ;
    default:
        JL_NVIC->IPCON3 &= ~(0x3 << (b));
        JL_NVIC->IPCON3 |= ((priority & 0x3) << (b));
    }
}


AT_IRQ_HW_CODE
static void __hw_irq_os_disable()
{
    int tmp;

    __asm__ volatile("mov %0,ie0" : "=r"(tmp));
    tmp &= ~(BIT(0));
    __asm__ volatile("mov ie0,%0" :: "r"(tmp));
}


AT_IRQ_HW_CODE
static void __hw_irq_os_enable()
{
    int tmp;
    __asm__ volatile("mov %0,ie0" : "=r"(tmp));
    tmp |= (BIT(0));
    __asm__ volatile("mov ie0,%0" :: "r"(tmp));
}



AT_IRQ_HW_CODE
static void __hw_irq_disable_all(void)
{
    int tmp = 0;

    __asm__ volatile("mov %0,ie0" : "=r"(tmp));
    /* printf("ie0 =%08x \n",tmp);
    */
    tmp = 0;
    __asm__ volatile("mov ie0,%0" :: "r"(tmp));
    __asm__ volatile("mov %0,ie0" : "=r"(tmp));
    /* printf("ie0 =%08x \n",tmp);
    */
}


AT_IRQ_HW_CODE
static void __hw_irq_global_enable(void)
{
    int tmp;
    __asm__ volatile("mov %0,icfg" : "=r"(tmp));
    tmp |= BIT(8) ;
    __asm__ volatile("mov icfg,%0" : : "r"(tmp));
}


AT_IRQ_HW_CODE
static void __hw_irq_global_disable(void)
{
    int tmp;
    __asm__ volatile("mov %0,icfg" : "=r"(tmp));
    tmp &= ~BIT(8) ;
    __asm__ volatile("mov icfg,%0" : : "r"(tmp));
}



AT_IRQ_HW_CODE
static void __hw_irq_sys_enable(void)
{
    OS_EXIT_CRITICAL();
}


AT_IRQ_HW_CODE
static void __hw_irq_sys_disable(void)
{
    OS_ENTER_CRITICAL();
}



AT_IRQ_HW_CODE
static void hw_irq_common_handler(unsigned int idx)
{
    __hw_irq_clr_pending(idx);
}



AT_IRQ_HW_CODE
static void hw_irq_handler_register(unsigned int irq_idx, void *handler, unsigned int irq_prio)
{
    struct irq_data *priv;

    unsigned int *israddr = (unsigned int *)IRQ_MEM_ADDR;

    priv = &irq[irq_idx];
    /* priv = malloc(); */

    priv->idx = irq_idx;
    priv->prio = irq_prio;

    /* list_del(&priv->entry); */
    /* list_add_tail(&priv->entry, &__this->irq_head); */

    israddr[irq_idx] = (unsigned int)handler;

    /* if (unlikely(irq_idx == IRQ_EXCEPTION_IDX)) { */
    /* [> puts(__func__); <] */
    /* [> puts(" - manual jump\n"); <] */
    /* return; */
    /* } */

    __hw_irq_set_prio(irq_idx, irq_prio);

    __hw_irq_clr_pending(irq_idx);

    /* if (unlikely(irq_idx == 0)) */
    /* { */
    /* return; */
    /* } */
    __hw_irq_enable(irq_idx);
}


AT_IRQ_HW_CODE
void trig_fun()
{
    __asm__ volatile("trigger");
}


AT_IRQ_HW_CODE
void error_isr_handler()
{
    puts(__func__);
    while (1);
}


AT_IRQ_HW_CODE
static void hw_irq_handler_unregister(unsigned int irq_idx)
{
    struct irq_data *priv;

    if (unlikely(irq_idx == 64)) {
        puts(__func__);
        return;
    }

    unsigned int *israddr = (unsigned int *)IRQ_MEM_ADDR;

    struct irq_data *p, *n;

    /* list_for_each_entry_safe(p, n, &__this->irq_head, entry) { */
    /* list_del(&p->entry); */
    /* } */

    israddr[irq_idx] = error_isr_handler;

    __hw_irq_clr_pending(irq_idx);

    __hw_irq_disable(irq_idx);
}




AT_IRQ_HW_CODE
static void hw_irq_init(void)
{
    unsigned int i;

    u32 *israddr = (u32 *)IRQ_MEM_ADDR; //   no dcatch init enter = 0xff80;
//  vm_mutex_init();

    /* INIT_LIST_HEAD(&__this->irq_head); */

    for (i = 1; i < 32; i++) {
        israddr[i] = (u32)error_isr_handler;
    }
    __hw_irq_disable_all();

    /* __hw_irq_global_enable(); */
}


AT_IRQ_HW_CODE
static const struct irq_driver irq_drv_ins = {
    .init               = hw_irq_init,

    .handler_register   = hw_irq_handler_register,

    .commmon_handler    = hw_irq_common_handler,

    .handler_unregister = hw_irq_handler_unregister,

    .enable             = __hw_irq_enable,

    .disable            = __hw_irq_disable,

    .normal_enable      = __hw_irq_global_enable,

    .normal_disable     = __hw_irq_global_disable,

    .sys_enable         = __hw_irq_sys_enable,

    .sys_disable        = __hw_irq_sys_disable,
};
REGISTER_IRQ_DRIVER(irq_drv_ins);
