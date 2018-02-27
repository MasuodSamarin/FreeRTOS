/*********************************************************************************************
    *   Filename        : core.h

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2017-01-17 15:16

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/

// *****************************************************************************
//  Core header for all BLE protocols and profiles
// *****************************************************************************

#ifndef __BLE_CORE
#define __BLE_CORE

#include "btstack-config.h"

#ifndef ENABLE_BLE
#error "ENABLE_BLE not defined but Classic protocol or profile include. Please add ENABLE_BLE in btstack-config.h, or remove all Classic protocols and profiles"
#endif

#endif
