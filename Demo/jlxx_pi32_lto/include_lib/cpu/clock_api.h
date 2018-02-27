#ifndef _CLOCK_API_H_
#define _CLOCK_API_H_

#include "typedef.h"


/********************************************************************************/
/*
 *      API
 */
void clock_init(u8 sys_in, u32 input_freq, u32 out_freq);

void clock_switching(u32 out_freq);

#endif
