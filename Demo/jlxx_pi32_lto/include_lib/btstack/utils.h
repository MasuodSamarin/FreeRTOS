/*********************************************************************************************
    *   Filename        : utils.h

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2017-01-17 15:15

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/

#ifndef __UTILS_H
#define __UTILS_H


#if defined __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "btstack_defines.h"
#include "btstack-config.h"

    /**
     * @brief hci connection handle type
     */
    typedef uint16_t hci_con_handle_t;

    /**
     * @brief Length of a bluetooth device address.
     */
#define BD_ADDR_LEN 6
    typedef uint8_t bd_addr_t[BD_ADDR_LEN];

    /**
     * @brief link key and its type
     */
#define LINK_KEY_LEN 16
#define LINK_KEY_STR_LEN (LINK_KEY_LEN*2)
    typedef uint8_t link_key_t[LINK_KEY_LEN];

    typedef enum {
        COMBINATION_KEY = 0,	// standard pairing
        LOCAL_UNIT_KEY,			// ?
        REMOTE_UNIT_KEY,		// ?
        DEBUG_COMBINATION_KEY,	// SSP with debug
        UNAUTHENTICATED_COMBINATION_KEY_GENERATED_FROM_P192, // SSP Simple Pairing
        AUTHENTICATED_COMBINATION_KEY_GENERATED_FROM_P192,	 // SSP Passkey, Number confirm, OOB
        CHANGED_COMBINATION_KEY,							 // Link key changed using Change Connection Lnk Key
        UNAUTHENTICATED_COMBINATION_KEY_GENERATED_FROM_P256, // SSP Simpe Pairing
        AUTHENTICATED_COMBINATION_KEY_GENERATED_FROM_P256,   // SSP Passkey, Number confirm, OOB
    } link_key_type_t;

    /**
     * @brief The device name type
     */
#define DEVICE_NAME_LEN 248
    typedef uint8_t device_name_t[DEVICE_NAME_LEN + 1];


// helper for BT little endian format
#define READ_BT_16( buffer, pos) ( ((uint16_t) buffer[pos]) | (((uint16_t)buffer[pos+1]) << 8))
#define READ_BT_24( buffer, pos) ( ((uint32_t) buffer[pos]) | (((uint32_t)buffer[pos+1]) << 8) | (((uint32_t)buffer[pos+2]) << 16))
#define READ_BT_32( buffer, pos) ( ((uint32_t) buffer[pos]) | (((uint32_t)buffer[pos+1]) << 8) | (((uint32_t)buffer[pos+2]) << 16) | (((uint32_t) buffer[pos+3])) << 24)

// helper for SDP big endian format
#define READ_NET_16( buffer, pos) ( ((uint16_t) buffer[pos+1]) | (((uint16_t)buffer[pos  ]) << 8))
#define READ_NET_32( buffer, pos) ( ((uint32_t) buffer[pos+3]) | (((uint32_t)buffer[pos+2]) << 8) | (((uint32_t)buffer[pos+1]) << 16) | (((uint32_t) buffer[pos])) << 24)

// HCI CMD OGF/OCF
#define READ_CMD_OGF(buffer) (buffer[1] >> 2)
#define READ_CMD_OCF(buffer) ((buffer[1] & 0x03) << 8 | buffer[0])


// Code+Len=2, Pkts+Opcode=3; total=5
#define OFFSET_OF_DATA_IN_COMMAND_COMPLETE 5

#define READ_CONNECTION_HANDLE(buffer)  (READ_BT_16(buffer,0) & 0x0fff)
// ACL Packet
#define READ_ACL_CONNECTION_HANDLE( buffer ) ( READ_BT_16(buffer,0) & 0x0fff)
#define READ_ACL_FLAGS( buffer )      ( (buffer[1] >> 4) & 0x03)
#define READ_ACL_LENGTH( buffer )     (READ_BT_16(buffer, 2))

// L2CAP Packet
#define READ_L2CAP_LENGTH(buffer)     ( READ_BT_16(buffer, 4))
#define READ_L2CAP_CHANNEL_ID(buffer) ( READ_BT_16(buffer, 6))

// SCo Packet
#define READ_SCO_CONNECTION_HANDLE( buffer ) ( READ_BT_16(buffer,0) & 0x0fff)
#define READ_SCO_FLAGS( buffer )      ( buffer[1] >> 4 )
#define READ_SCO_LENGTH( buffer )     (READ_BT_8(buffer, 2))

    void bt_store_16(uint8_t *buffer, uint16_t pos, uint16_t value);
    void bt_store_32(uint8_t *buffer, uint16_t pos, uint32_t value);
    void bt_flip_addr(bd_addr_t dest, bd_addr_t src);

    void net_store_16(uint8_t *buffer, uint16_t pos, uint16_t value);
    void net_store_32(uint8_t *buffer, uint16_t pos, uint32_t value);

// hack: compilation with the android ndk causes an error as there's a swap64 macro
#ifdef swap64
#undef swap64
#endif

    void swapX(const uint8_t *src, uint8_t *dst, int len);
    void swap24(const uint8_t  src[3],  uint8_t dst[3]);
    void swap56(const uint8_t  src[7],  uint8_t dst[7]);
    void swap64(const uint8_t  src[8],  uint8_t dst[8]);
    void swap128(const uint8_t src[16], uint8_t dst[16]);

    void printf_hexdump(const void *data, int size);
    void hexdump(const void *data, int size);
    void hexdumpf(const void *data, int size);
    char *uuid128_to_str(uint8_t *uuid);
    void printUUID128(uint8_t *uuid);
    void log_key(const char *name, sm_key_t key);

// @deprecated please use more convenient bd_addr_to_str
    void print_bd_addr(bd_addr_t addr);

    char *bd_addr_to_str(bd_addr_t addr);
    char *link_key_to_str(link_key_t link_key);
    char *link_key_type_to_str(link_key_type_t link_key);

    int sscan_bd_addr(uint8_t *addr_string, bd_addr_t addr);
    int sscan_link_key(char *addr_string, link_key_t link_key);

    uint8_t crc8_check(uint8_t *data, uint16_t len, uint8_t check_sum);
    uint8_t crc8_calc(uint8_t *data, uint16_t len);

#define BD_ADDR_CMP(a,b) memcmp(a,b, BD_ADDR_LEN)
#define BD_ADDR_COPY(dest,src) memcpy(dest,src,BD_ADDR_LEN)

    int is_authenticated_link_key(link_key_type_t link_key_type);

    /*******************************************************************/
    /*
     *-------------------New Untils
     */
    int bd_addr_cmp(bd_addr_t a, bd_addr_t b);

    void bd_addr_copy(bd_addr_t dest, bd_addr_t src);

    /**
     * @brief Read 16/24/32 bit little endian value from buffer
     * @param buffer
     * @param position in buffer
     * @return value
     */
    uint16_t little_endian_read_16(const uint8_t *buffer, int position);
    uint32_t little_endian_read_24(const uint8_t *buffer, int position);
    uint32_t little_endian_read_32(const uint8_t *buffer, int position);

    /**
     * @brief Write 16/32 bit little endian value into buffer
     * @param buffer
     * @param position in buffer
     * @param value
     */
    void little_endian_store_16(uint8_t *buffer, uint16_t position, uint16_t value);
    void little_endian_store_32(uint8_t *buffer, uint16_t position, uint32_t value);

    /**
     * @brief Read 16/24/32 bit big endian value from buffer
     * @param buffer
     * @param position in buffer
     * @return value
     */
    uint32_t big_endian_read_16(const uint8_t *buffer, int pos);
    uint32_t big_endian_read_32(const uint8_t *buffer, int pos);

    /**
     * @brief Write 16/32 bit big endian value into buffer
     * @param buffer
     * @param position in buffer
     * @param value
     */
    void big_endian_store_16(uint8_t *buffer, uint16_t pos, uint16_t value);
    void big_endian_store_32(uint8_t *buffer, uint16_t pos, uint32_t value);

    /**
     * @brief Copy from source to destination and reverse byte order
     * @param src
     * @param dest
     * @param len
     */
    void reverse_bytes(const uint8_t *src, uint8_t *dest, int len);

    /**
     * @brief Wrapper around reverse_bytes for common buffer sizes
     * @param src
     * @param dest
     */
    void reverse_24(const uint8_t *src, uint8_t *dest);
    void reverse_32(const uint8_t *src, uint8_t *dest);
    void reverse_48(const uint8_t *src, uint8_t *dest);
    void reverse_56(const uint8_t *src, uint8_t *dest);
    void reverse_64(const uint8_t *src, uint8_t *dest);
    void reverse_128(const uint8_t *src, uint8_t *dest);
    void reverse_256(const uint8_t *src, uint8_t *dest);

    void reverse_bd_addr(const bd_addr_t src, bd_addr_t dest);

    void uuid_add_bluetooth_prefix(uint8_t *uuid, uint32_t shortUUID);
    int uuid_has_bluetooth_prefix(uint8_t *uuid128);

#if defined __cplusplus
}
#endif

#endif // __UTILS_H
