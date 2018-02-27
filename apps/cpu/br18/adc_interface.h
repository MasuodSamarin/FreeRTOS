#ifndef __ADC_DRIVER_H__
#define __ADC_DRIVER_H__

#include "typedef.h"

//AD channel define
enum {
    AD_CHANNEL_SEL_PA3 = 0,
    AD_CHANNEL_SEL_PA4,
    AD_CHANNEL_SEL_PA5,
    AD_CHANNEL_SEL_PA6,
    AD_CHANNEL_SEL_PA9,
    AD_CHANNEL_SEL_PA10,

    AD_CHANNEL_SEL_PB7,
    AD_CHANNEL_SEL_PB8,
    AD_CHANNEL_SEL_PB9,
    AD_CHANNEL_SEL_PB10,
    AD_CHANNEL_SEL_PB11,
    AD_CHANNEL_SEL_PB12,


    AD_CHANNEL_SEL_PR1,
    AD_CHANNEL_SEL_PR2,

    AD_CHANNEL_SEL_LDOIN,
    AD_CHANNEL_SEL_LDOREF,
};

enum {
    SET_ADC_LVD_EN,
    SET_ADC_LVD_CH,
    SET_ADC_CHANNEL,
    GET_ADC_VALUE,
};

struct adc_driver {

    void (*on)(u8 channel);

    void (*off)(void);

    u8(*ioctrl)(int, ...);
};

#define REGISTER_ADC_DRIVER(drv) \
	const struct adc_driver *__adc_drv \
			SEC(.adc_drv) = &drv

extern const struct adc_driver *__adc_drv;


#endif
