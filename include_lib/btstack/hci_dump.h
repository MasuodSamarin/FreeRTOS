/*********************************************************************************************
    *   Filename        : hci_dump.h

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2017-01-17 15:14

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/

#ifndef __HCI_DUMP_H
#define __HCI_DUMP_H

//#include <stdint.h>
#include "typedef.h"

#ifdef __AVR__
#include <avr/pgmspace.h>
#endif

#if defined __cplusplus
extern "C" {
#endif

    typedef enum {
        HCI_DUMP_BLUEZ = 0,
        HCI_DUMP_PACKETLOGGER,
        HCI_DUMP_STDOUT
    } hci_dump_format_t;

    void hci_dump_open(const char *filename, hci_dump_format_t format);
    void hci_dump_set_max_packets(int packets); // -1 for unlimited
    void hci_dump_packet(u8 packet_type, u8 in, u8 *packet, u16 len);
    void hci_dump_log(const char *format, ...);
    void hci_dump_close(void);

#ifdef __AVR__
    void hci_dump_log_P(PGM_P format, ...);
#endif

#if defined __cplusplus
}
#endif
#endif // __HCI_DUMP_H
