/*********************************************************************************************
    *   Filename        : hci_transport.h

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2017-01-17 15:14

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/
#ifndef __HCI_TRANSPORT_H
#define __HCI_TRANSPORT_H

//#include <stdint.h>
#include "typedef.h"

#if defined __cplusplus
extern "C" {
#endif

    /* HCI packet types */
    typedef struct {
        int (*open)(void *transport_config);
        int (*close)(void *transport_config);
        int (*send_packet)(u8 packet_type, u8 *packet, int size);
        void (*register_packet_handler)(void (*handler)(u8 packet_type, const u8 *packet, u16 size));
        const char *(*get_transport_name)(void);
        // custom extension for UART transport implementations
        int (*set_baudrate)(u32 baudrate);
        // support async transport layers, e.g. IRQ driven without buffers
        int (*can_send_packet_now)(u8 packet_type);
    } hci_transport_t;

    typedef struct {
        const char *device_name;
        u32   baudrate_init; // initial baud rate
        u32   baudrate_main; // = 0: same as initial baudrate
        int   flowcontrol; //
    } hci_uart_config_t;


// inline various hci_transport_X.h files
    extern hci_transport_t *hci_transport_h4_instance(void);
    extern hci_transport_t *hci_transport_h4_dma_instance(void);
    extern hci_transport_t *hci_transport_h4_iphone_instance(void);
    extern hci_transport_t *hci_transport_h5_instance(void);
    extern hci_transport_t *hci_transport_usb_instance(void);

// support for "enforece wake device" in h4 - used by iOS power management
    extern void hci_transport_h4_iphone_set_enforce_wake_device(char *path);

    hci_transport_t *ble_hci_transport_h4_instance();

#if defined __cplusplus
}
#endif

#endif // __HCI_TRANSPORT_H
