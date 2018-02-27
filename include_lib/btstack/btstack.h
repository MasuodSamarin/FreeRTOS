/*********************************************************************************************
    *   Filename        : btstack.h

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2017-01-17 15:13

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/

#ifndef __BTSTACK_H
#define __BTSTACK_H

#include <hci_cmds.h>
#include <btstack_run_loop.h>
#include <utils.h>

#include "btstack-config.h"

#include <stdint.h>

#if defined __cplusplus
extern "C" {
#endif

// Default TCP port for BTstack daemon
#ifndef BTSTACK_PORT
#define BTSTACK_PORT            13333
#endif

// UNIX domain socket for BTstack */
#ifndef BTSTACK_UNIX
#define BTSTACK_UNIX            "/tmp/BTstack"
#endif

// packet handler
// typedef void (*btstack_packet_handler_t) (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);

// optional: if called before bt_open, TCP socket is used instead of local unix socket
//           note: address is not copied and must be valid during bt_open
    void bt_use_tcp(const char *address, uint16_t port);

// init BTstack library
    int bt_open(void);

// stop using BTstack library
    int bt_close(void);

// send hci cmd packet
    int bt_send_cmd(const hci_cmd_t *cmd, ...);

// register packet handler -- channel only valid for l2cap and rfcomm packets
// @returns old packet handler
    btstack_packet_handler_t bt_register_packet_handler(btstack_packet_handler_t handler);

    void bt_send_acl(uint8_t *data, uint16_t len);

    void bt_send_l2cap(uint16_t local_cid, uint8_t *data, uint16_t len);
    void bt_send_rfcomm(uint16_t rfcom_cid, uint8_t *data, uint16_t len);

#if defined __cplusplus
}
#endif

#endif // __BTSTACK_H
