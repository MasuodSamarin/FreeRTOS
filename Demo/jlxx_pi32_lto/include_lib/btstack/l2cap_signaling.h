/*********************************************************************************************
    *   Filename        : l2cap_signaling.h

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2017-01-17 15:15

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/

#ifndef __L2CAP_SIGNALING_H
#define __L2CAP_SIGNALING_H

#include <stdint.h>
#include <stdarg.h>
#include <utils.h>
#include <hci_cmds.h>

#if defined __cplusplus
extern "C" {
#endif

    typedef enum {
        COMMAND_REJECT = 1,
        CONNECTION_REQUEST,
        CONNECTION_RESPONSE,
        CONFIGURE_REQUEST,
        CONFIGURE_RESPONSE,
        DISCONNECTION_REQUEST,
        DISCONNECTION_RESPONSE,
        ECHO_REQUEST,
        ECHO_RESPONSE,
        INFORMATION_REQUEST,
        INFORMATION_RESPONSE,
        CONNECTION_PARAMETER_UPDATE_REQUEST = 0x12,
        CONNECTION_PARAMETER_UPDATE_RESPONSE,
        COMMAND_REJECT_LE = 0x14,  // internal to BTstack
    } L2CAP_SIGNALING_COMMANDS;

    /*uint16_t l2cap_create_signaling_classic(uint8_t * acl_buffer,hci_con_handle_t handle, L2CAP_SIGNALING_COMMANDS cmd, uint8_t identifier, va_list argptr);*/
    uint16_t l2cap_create_signaling_le(uint8_t *acl_buffer, hci_con_handle_t handle, L2CAP_SIGNALING_COMMANDS cmd, uint8_t identifier, va_list argptr);
    uint16_t l2cap_le_create_connection_parameter_update_request(uint8_t *acl_buffer, uint16_t handle, uint16_t interval_min, uint16_t interval_max, uint16_t slave_latency, uint16_t timeout_multiplier);
    uint16_t l2cap_le_create_connection_parameter_update_response(uint8_t *acl_buffer, uint16_t handle, uint16_t response);
    /*uint8_t  l2cap_next_sig_id(void);
    uint16_t l2cap_next_local_cid(void);*/

#if defined __cplusplus
}
#endif

#endif // __L2CAP_SIGNALING_H
