#ifndef __KEY_DRV_INTERFACE_H__
#define __KEY_DRV_INTERFACE_H__

#include "typedef.h"

struct key_driver {

    void (*init)(void);

    u8(*get_key_number)(void);

    void (*ioctrl)(int, ...);
};

#define KEY_NUMBER_INVALID      0xff


#endif
