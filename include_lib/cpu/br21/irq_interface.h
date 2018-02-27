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
#ifndef __INTERRUPT_INTERFACE_H__
#define __INTERRUPT_INTERFACE_H__

#include "typedef.h"

#define IRQ_TIME0_IDX      2
#define IRQ_TIME1_IDX      3
#define IRQ_TIME2_IDX      4
#define IRQ_TIME3_IDX      5
#define IRQ_USB_SOF_IDX    6
#define IRQ_USB_CTRL_IDX   7
#define IRQ_RTC_IDX        8
#define IRQ_ALINK_IDX      9
#define IRQ_DAC_IDX        10
#define IRQ_PORT_IDX       11
#define IRQ_SPI0_IDX       12
#define IRQ_SPI1_IDX       13
#define IRQ_SD0_IDX       	14
#define IRQ_SD1_IDX       	15
#define IRQ_UART0_IDX      16
#define IRQ_UART1_IDX      17
#define IRQ_UART2_IDX      18
#define IRQ_PAP_IDX        19
#define IRQ_IIC_IDX        20
#define IRQ_SARADC_IDX     21
#define IRQ_FM_HWFE_IDX    22
#define IRQ_FM_IDX         23
#define IRQ_FM_LOFC_IDX    24
#define IRQ_BREDR_IDX      25
#define IRQ_BT_CLKN_IDX    26
#define IRQ_BT_DBG_IDX     27
#define IRQ_BT_PCM_IDX     28
#define IRQ_SRC_IDX        29
#define IRQ_EQ_IDX         31
#define IRQ_WD_IDX         34
#define IRQ_BLE_RX_IDX     36
#define IRQ_BLE_EVENT_IDX  37
#define IRQ_SOFT0_IDX      62
#define IRQ_SOFT_IDX       63
#define IRQ_EXCEPTION_IDX  64

#define IRQ_MEM_ADDR        0x00000



#define MAX_IRQ_ENTRY_NUM   65


struct irq_driver {
    void (*init)(void);

    void (*handler_register)(u32, void *, u32);

    void (*handler_unregister)(u32);

    void (*commmon_handler)(u32);

    void (*enable)(u32);

    void (*disable)(u32);

    bool (*status)(u32);

    void (*normal_enable)();

    void (*normal_disable)();

    void (*sys_enable)();

    void (*sys_disable)();

    void (*ioctrl)();
};

#define REGISTER_IRQ_DRIVER(drv) \
	const struct irq_driver *__irq_drv \
			SEC_USED(.irq_drv) = &drv

extern const struct irq_driver *__irq_drv;

#define AT_IRQ_HW_CODE     AT(.irq_hw_code)

#endif




