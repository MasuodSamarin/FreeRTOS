#ifndef	_DAC_INTERFACE_H_
#define _DAC_INTERFACE_H_

#include "typedef.h"


struct dac_driver {
    void (*init)();

    void (*handler_register)(void (*)(void));

    void (*ioctrl)(u8, ...);
};

#define REGISTER_DAC_DRIVER(drv) \
	const struct dac_driver *__dac_drv \
			sec(.text) = &drv

extern const struct dac_driver *__dac_drv;

#endif
