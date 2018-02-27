/*********************************************************************************************
    *   Filename        : att_dispatch.h

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2017-01-17 15:16

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/


/**
 * Dispatcher for independent implementation of ATT client and server
 */

#ifndef __ATT_DISPATCH_H
#define __ATT_DISPATCH_H

#include <btstack.h>

#if defined __cplusplus
extern "C" {
#endif

    /**
     * @brief reset att dispatchter
     * @param packet_hander for ATT client packets
     */
    void att_dispatch_register_client(btstack_packet_handler_t packet_handler);

    /**
     * @brief reset att dispatchter
     * @param packet_hander for ATT server packets
     */
    void att_dispatch_register_server(btstack_packet_handler_t packet_handler);

    /**
     * @brief can send packet for client
     * @param con_handle
     */
    int att_dispatch_client_can_send_now(hci_con_handle_t con_handle);

    /**
     * @brief can send packet for server
     * @param con_handle
     */
    int att_dispatch_server_can_send_now(hci_con_handle_t con_handle);

    /**
     * @brief Request emission of L2CAP_EVENT_CAN_SEND_NOW as soon as possible for client
     * @note L2CAP_EVENT_CAN_SEND_NOW might be emitted during call to this function
     *       so packet handler should be ready to handle it
     * @param con_handle
     */
    void att_dispatch_client_request_can_send_now_event(hci_con_handle_t con_handle);

    /**
     * @brief Request emission of L2CAP_EVENT_CAN_SEND_NOW as soon as possible for server
     * @note L2CAP_EVENT_CAN_SEND_NOW might be emitted during call to this function
     *       so packet handler should be ready to handle it
     * @param con_handle
     */
    void att_dispatch_server_request_can_send_now_event(hci_con_handle_t con_handle);

#if defined __cplusplus
}
#endif

#endif // __ATT_DISPATCH_H
