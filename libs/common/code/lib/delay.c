#include "typedef.h"
#include "power_api.h"

AT_NON_VOLATILE_RAM_CODE
void delay(u32 d)
{
    while (d--) {
        __builtin_pi32_nop();
    }
}

