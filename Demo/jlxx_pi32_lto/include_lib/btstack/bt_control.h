/*********************************************************************************************
    *   Filename        : bt_control.h

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2017-01-17 15:13

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/

#ifndef __BT_CONTROL_H
#define __BT_CONTROL_H

#include <stdint.h>
#include <utils.h>

#if defined __cplusplus
extern "C" {
#endif

    typedef enum {
        POWER_WILL_SLEEP = 1,
        POWER_WILL_WAKE_UP
    } POWER_NOTIFICATION_t;

    typedef struct {
        int (*on)(void *config);              // <-- turn BT module on and configure
        int (*off)(void *config);             // <-- turn BT module off
        int (*sleep)(void *config);           // <-- put BT module to sleep    - only to be called after ON
        int (*wake)(void *config);            // <-- wake BT module from sleep - only to be called after SLEEP
        int (*valid)(void *config);           // <-- test if hardware can be supported
        const char *(*name)(void *config);    // <-- return hardware name

        /** support for UART baud rate changes - cmd has to be stored in hci_cmd_buffer
         * @return have command
         */
        int (*baudrate_cmd)(void *config, uint32_t baudrate, uint8_t *hci_cmd_buffer);

        /** support custom init sequences after RESET command - cmd has to be stored in hci_cmd_buffer
          * @return have command
          */
        int (*next_cmd)(void *config, uint8_t *hci_cmd_buffer);

        void (*register_for_power_notifications)(void (*cb)(POWER_NOTIFICATION_t event));

        void (*hw_error)(void);

        /** support for vendor-specific way to set BD ADDR - cmd has to be stored in hci_cmd_buffer
         * @return have command
         */
        int (*set_bd_addr_cmd)(void *config, bd_addr_t addr, uint8_t *hci_cmd_buffer);

    } bt_control_t;

#if defined __cplusplus
}
#endif

#endif // __BT_CONTROL_H
