/*********************************************************************************************
    *   Filename        : att_server.h

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2017-01-17 15:16

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/
#ifndef __ATT_SERVER_H
#define __ATT_SERVER_H

#include <btstack.h>
#include <stdint.h>
#include "ble/att.h"

#if defined __cplusplus
extern "C" {
#endif

    /* API_START */
    /*
     * @brief setup ATT server
     * @param db attribute database created by compile-gatt.ph
     * @param read_callback, see att_db.h, can be NULL
     * @param write_callback, see attl.h, can be NULL
     */
    void att_server_init(uint8_t const *db, att_read_callback_t read_callback, att_write_callback_t write_callback);

    /*
     * @brief register packet handler for ATT server events:
     *        - ATT_EVENT_MTU_EXCHANGE_COMPLETE
     *        - ATT_EVENT_HANDLE_VALUE_INDICATION_COMPLETE
     * @param handler
     */
    void att_server_register_packet_handler(btstack_packet_handler_t handler);

    /*
     * @brief tests if a notification or indication can be send right now
     * @param con_handle
     * @return 1, if packet can be sent
     */
    int  att_server_can_send_packet_now(hci_con_handle_t con_handle);

    /**
     * @brief Request emission of ATT_EVENT_CAN_SEND_NOW as soon as possible
     * @note ATT_EVENT_CAN_SEND_NOW might be emitted during call to this function
     *       so packet handler should be ready to handle it
     * @param con_handle
     */
    void att_server_request_can_send_now_event(hci_con_handle_t con_handle);

    /*
     * @brief notify client about attribute value change
     * @param con_handle
     * @param attribute_handle
     * @param value
     * @param value_len
     * @return 0 if ok, error otherwise
     */
    int att_server_notify(hci_con_handle_t con_handle, uint16_t attribute_handle, uint8_t *value, uint16_t value_len);

    /*
     * @brief indicate value change to client. client is supposed to reply with an indication_response
     * @param con_handle
     * @param attribute_handle
     * @param value
     * @param value_len
     * @return 0 if ok, error otherwise
     */
    int att_server_indicate(hci_con_handle_t con_handle, uint16_t attribute_handle, uint8_t *value, uint16_t value_len);

    void att_process_init(void);
    void att_process_resume(void);
    /* API_END */

#if defined __cplusplus
}
#endif

#endif // __ATT_SERVER_H
