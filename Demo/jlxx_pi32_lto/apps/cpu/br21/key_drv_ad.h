/*********************************************************************************************
    *   Filename        : key_drv_ad.h

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2017-02-16 14:41

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/
#ifndef _KEY_DRV_AD_H_
#define _KEY_DRV_AD_H_

#include "cpu.h"
#include "key_drv_interface.h"

enum {
    GET_SYS_VOLTAGE,
};


#define REGISTER_KEY_ADC_DRIVER(drv) \
	const struct key_driver *__adc_key_drv \
			SEC(.adc_key_drv) = &drv

extern const struct key_driver *__adc_key_drv;

#endif
