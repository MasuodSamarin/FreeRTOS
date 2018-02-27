#ifndef __IRTC_INTERFACE_H__
#define __IRTC_INTERFACE_H__

#include "typedef.h"

void rtc_port_store(void);

void rtc_port_close(void);

void rtc_port_restore(void);

void rtc_low_power(void);

void rtc_set_wakeup_io(u8 index, bool edge);

#endif
