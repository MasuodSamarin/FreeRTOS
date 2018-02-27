/*********************************************************************************************
    *   Filename        : ancs_client.h

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2017-01-17 15:16

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/

#ifndef __ANCS_CLIENT_H
#define __ANCS_CLIENT_H

#if defined __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "btstack_defines.h"

    /* API_START */

    void ancs_client_init(void);
    void ancs_client_register_callback(btstack_packet_handler_t callback);
    const char *ancs_client_attribute_name_for_id(int id);

    /* API_END */

#if defined __cplusplus
}
#endif

#endif
