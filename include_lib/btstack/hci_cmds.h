/*********************************************************************************************
    *   Filename        : hci_cmds.h

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2017-01-17 15:14

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/

#ifndef __HCI_CMDS_H
#define __HCI_CMDS_H

#include <stdint.h>
#include "btstack_defines.h"

#if defined __cplusplus
extern "C" {
#endif


    typedef enum {
        BLE_PERIPHERAL_OK = 0xA0,
        BLE_PERIPHERAL_IN_WRONG_STATE,
        BLE_PERIPHERAL_DIFFERENT_CONTEXT_FOR_ADDRESS_ALREADY_EXISTS,
        BLE_PERIPHERAL_NOT_CONNECTED,
        BLE_VALUE_TOO_LONG,
        BLE_PERIPHERAL_BUSY,
        BLE_CHARACTERISTIC_NOTIFICATION_NOT_SUPPORTED,
        BLE_CHARACTERISTIC_INDICATION_NOT_SUPPORTED
    } le_command_status_t;

    /**
     * Default INQ Mode
     */
#define HCI_INQUIRY_LAP 0x9E8B33L  // 0x9E8B33: General/Unlimited Inquiry Access Code (GIAC)

    /**
     * SSP IO Capabilities - if capability is set, BTstack answers IO Capability Requests
     */
#define SSP_IO_CAPABILITY_DISPLAY_ONLY   0
#define SSP_IO_CAPABILITY_DISPLAY_YES_NO 1
#define SSP_IO_CAPABILITY_KEYBOARD_ONLY  2
#define SSP_IO_CAPABILITY_NO_INPUT_NO_OUTPUT 3
#define SSP_IO_CAPABILITY_UNKNOWN 0xff

    /**
     * SSP Authentication Requirements, see IO Capability Request Reply Commmand
     */

// Numeric comparison with automatic accept allowed.
#define SSP_IO_AUTHREQ_MITM_PROTECTION_NOT_REQUIRED_NO_BONDING 0x00

// Use IO Capabilities to deter- mine authentication procedure
#define SSP_IO_AUTHREQ_MITM_PROTECTION_REQUIRED_NO_BONDING 0x01

// Numeric compar- ison with automatic accept allowed.
#define SSP_IO_AUTHREQ_MITM_PROTECTION_NOT_REQUIRED_DEDICATED_BONDING 0x02

// Use IO Capabilities to determine authentication procedure
#define SSP_IO_AUTHREQ_MITM_PROTECTION_REQUIRED_DEDICATED_BONDING 0x03

// Numeric Compari- son with automatic accept allowed.
#define SSP_IO_AUTHREQ_MITM_PROTECTION_NOT_REQUIRED_GENERAL_BONDING 0x04

// . Use IO capabilities to determine authentication procedure.
#define SSP_IO_AUTHREQ_MITM_PROTECTION_REQUIRED_GENERAL_BONDING 0x05

    /**
     * Address types
     * @note: BTstack uses a custom addr type to refer to classic ACL and SCO devices
     */
    typedef enum {
        BD_ADDR_TYPE_LE_PUBLIC = 0,
        BD_ADDR_TYPE_LE_RANDOM = 1,
        BD_ADDR_TYPE_SCO       = 0xfe,
        BD_ADDR_TYPE_CLASSIC   = 0xff,
        BD_ADDR_TYPE_UNKNOWN   = 0xfe
    } bd_addr_type_t;

    /**
     *  Hardware state of Bluetooth controller
     */
    typedef enum {
        HCI_POWER_OFF = 0,
        HCI_POWER_ON,
        HCI_POWER_SLEEP
    } HCI_POWER_MODE;

    /**
     * State of BTstack
     */
    typedef enum {
        HCI_STATE_OFF = 0,
        HCI_STATE_INITIALIZING,
        HCI_STATE_WORKING,
        HCI_STATE_HALTING,
        HCI_STATE_SLEEPING,
        HCI_STATE_FALLING_ASLEEP
    } HCI_STATE;

    /**
     * compact HCI Command packet description
     */
    typedef struct {
        uint16_t    opcode;
        const char *format;
    } hci_cmd_t;


// HCI Commands - see hci_cmds.c for info on parameters
    extern const hci_cmd_t btstack_get_state;
    extern const hci_cmd_t btstack_set_power_mode;
    extern const hci_cmd_t btstack_set_acl_capture_mode;
    extern const hci_cmd_t btstack_get_version;
    extern const hci_cmd_t btstack_get_system_bluetooth_enabled;
    extern const hci_cmd_t btstack_set_system_bluetooth_enabled;
    extern const hci_cmd_t btstack_set_discoverable;
    extern const hci_cmd_t btstack_set_bluetooth_enabled;    // only used by btstack config

    extern const hci_cmd_t hci_accept_connection_request;
    extern const hci_cmd_t hci_accept_synchronous_connection_command;
    extern const hci_cmd_t hci_authentication_requested;
    extern const hci_cmd_t hci_change_connection_link_key;
    extern const hci_cmd_t hci_change_connection_packet_type;
    extern const hci_cmd_t hci_create_connection;
    extern const hci_cmd_t hci_create_connection_cancel;
    extern const hci_cmd_t hci_delete_stored_link_key;
    extern const hci_cmd_t hci_enhanced_setup_synchronous_connection_command;
    extern const hci_cmd_t hci_enhanced_accept_synchronous_connection_command;
    extern const hci_cmd_t hci_disconnect;
    extern const hci_cmd_t hci_host_buffer_size;
    extern const hci_cmd_t hci_inquiry;
    extern const hci_cmd_t hci_io_capability_request_reply;
    extern const hci_cmd_t hci_io_capability_request_negative_reply;
    extern const hci_cmd_t hci_inquiry_cancel;
    extern const hci_cmd_t hci_link_key_request_negative_reply;
    extern const hci_cmd_t hci_link_key_request_reply;
    extern const hci_cmd_t hci_pin_code_request_reply;
    extern const hci_cmd_t hci_pin_code_request_negative_reply;
    extern const hci_cmd_t hci_qos_setup;
    extern const hci_cmd_t hci_read_bd_addr;
    extern const hci_cmd_t hci_read_buffer_size;
    extern const hci_cmd_t hci_read_le_host_supported;
    extern const hci_cmd_t hci_read_link_policy_settings;
    extern const hci_cmd_t hci_read_link_supervision_timeout;
    extern const hci_cmd_t hci_read_local_version_information;
    extern const hci_cmd_t hci_read_local_supported_commands;
    extern const hci_cmd_t hci_read_local_supported_features;
    extern const hci_cmd_t hci_read_num_broadcast_retransmissions;
    extern const hci_cmd_t hci_read_remote_supported_features_command;
    extern const hci_cmd_t hci_read_rssi;
    extern const hci_cmd_t hci_reject_connection_request;
    extern const hci_cmd_t hci_remote_name_request;
    extern const hci_cmd_t hci_remote_name_request_cancel;
    extern const hci_cmd_t hci_remote_oob_data_request_negative_reply;
    extern const hci_cmd_t hci_reset;
    extern const hci_cmd_t hci_role_discovery;
    extern const hci_cmd_t hci_set_event_mask;
    extern const hci_cmd_t hci_set_connection_encryption;
    extern const hci_cmd_t hci_setup_synchronous_connection_command;
    extern const hci_cmd_t hci_sniff_mode;
    extern const hci_cmd_t hci_switch_role_command;
    extern const hci_cmd_t hci_user_confirmation_request_negative_reply;
    extern const hci_cmd_t hci_user_confirmation_request_reply;
    extern const hci_cmd_t hci_user_passkey_request_negative_reply;
    extern const hci_cmd_t hci_user_passkey_request_reply;
    extern const hci_cmd_t hci_write_authentication_enable;
    extern const hci_cmd_t hci_write_class_of_device;
    extern const hci_cmd_t hci_write_extended_inquiry_response;
    extern const hci_cmd_t hci_write_inquiry_mode;
    extern const hci_cmd_t hci_write_le_host_supported;
    extern const hci_cmd_t hci_write_link_policy_settings;
    extern const hci_cmd_t hci_write_link_supervision_timeout;
    extern const hci_cmd_t hci_write_local_name;
    extern const hci_cmd_t hci_write_num_broadcast_retransmissions;
    extern const hci_cmd_t hci_write_page_timeout;
    extern const hci_cmd_t hci_write_scan_enable;
    extern const hci_cmd_t hci_write_simple_pairing_mode;
    extern const hci_cmd_t hci_write_synchronous_flow_control_enable;

    extern const hci_cmd_t hci_le_add_device_to_white_list;
    extern const hci_cmd_t hci_le_clear_white_list;
    extern const hci_cmd_t hci_le_connection_update;
    extern const hci_cmd_t hci_le_create_connection;
    extern const hci_cmd_t hci_le_create_connection_cancel;
    extern const hci_cmd_t hci_le_encrypt;
    extern const hci_cmd_t hci_le_long_term_key_negative_reply;
    extern const hci_cmd_t hci_le_long_term_key_request_reply;
    extern const hci_cmd_t hci_le_rand;
    extern const hci_cmd_t hci_le_read_advertising_channel_tx_power;
    extern const hci_cmd_t hci_le_read_buffer_size ;
    extern const hci_cmd_t hci_le_read_channel_map;
    extern const hci_cmd_t hci_le_read_remote_used_features;
    extern const hci_cmd_t hci_le_read_supported_features;
    extern const hci_cmd_t hci_le_read_supported_states;
    extern const hci_cmd_t hci_le_read_white_list_size;
    extern const hci_cmd_t hci_le_receiver_test;
    extern const hci_cmd_t hci_le_remove_device_from_white_list;
    extern const hci_cmd_t hci_le_set_advertise_enable;
    extern const hci_cmd_t hci_le_set_advertising_data;
    extern const hci_cmd_t hci_le_set_advertising_parameters;
    extern const hci_cmd_t hci_le_set_event_mask;
    extern const hci_cmd_t hci_le_set_host_channel_classification;
    extern const hci_cmd_t hci_le_set_random_address;
    extern const hci_cmd_t hci_le_set_scan_enable;
    extern const hci_cmd_t hci_le_set_scan_parameters;
    extern const hci_cmd_t hci_le_set_scan_response_data;
    extern const hci_cmd_t hci_le_start_encryption;
    extern const hci_cmd_t hci_le_test_end;
    extern const hci_cmd_t hci_le_transmitter_test;
    extern const hci_cmd_t hci_le_set_data_length;
    extern const hci_cmd_t hci_le_read_suggested_default_data_length;
    extern const hci_cmd_t hci_le_write_suggested_default_data_length;
    extern const hci_cmd_t hci_le_add_device_to_resolving_list;
    extern const hci_cmd_t hci_le_remove_device_from_resolving_list;
    extern const hci_cmd_t hci_le_read_resolving_list_size;
    extern const hci_cmd_t hci_le_peer_resolvable_address;
    extern const hci_cmd_t hci_le_local_resolvable_address;
    extern const hci_cmd_t hci_le_set_address_resolution_enable;
    extern const hci_cmd_t hci_le_set_resolvable_private_address_timeout;
    extern const hci_cmd_t hci_le_clear_resolving_list;
    extern const hci_cmd_t hci_le_read_maximum_data_length;


    extern const hci_cmd_t hci_read_remote_version_information;

    extern const hci_cmd_t l2cap_accept_connection;
    extern const hci_cmd_t l2cap_create_channel;
    extern const hci_cmd_t l2cap_create_channel_mtu;
    extern const hci_cmd_t l2cap_decline_connection;
    extern const hci_cmd_t l2cap_disconnect;
    extern const hci_cmd_t l2cap_register_service;
    extern const hci_cmd_t l2cap_unregister_service;

    extern const hci_cmd_t sdp_register_service_record;
    extern const hci_cmd_t sdp_unregister_service_record;
    extern const hci_cmd_t sdp_client_query_rfcomm_services;
    extern const hci_cmd_t sdp_client_query_services;


// accept connection @param bd_addr(48), rfcomm_cid (16)
    extern const hci_cmd_t rfcomm_accept_connection;
// create rfcomm channel: @param bd_addr(48), channel (8)
    extern const hci_cmd_t rfcomm_create_channel;
// create rfcomm channel: @param bd_addr(48), channel (8), mtu (16), credits (8)
    extern const hci_cmd_t rfcomm_create_channel_with_initial_credits;
// decline rfcomm disconnect,@param bd_addr(48), rfcomm cid (16), reason(8)
    extern const hci_cmd_t rfcomm_decline_connection;
// disconnect rfcomm disconnect, @param rfcomm_cid(8), reason(8)
    extern const hci_cmd_t rfcomm_disconnect;
// register rfcomm service: @param channel(8), mtu (16)
    extern const hci_cmd_t rfcomm_register_service;
// register rfcomm service: @param channel(8), mtu (16), initial credits (8)
    extern const hci_cmd_t rfcomm_register_service_with_initial_credits;
// unregister rfcomm service, @param service_channel(16)
    extern const hci_cmd_t rfcomm_unregister_service;
// request persisten rfcomm channel for service name: serive name (char*)
    extern const hci_cmd_t rfcomm_persistent_channel_for_service;

    extern const hci_cmd_t gap_disconnect_cmd;
    extern const hci_cmd_t gap_le_scan_start;
    extern const hci_cmd_t gap_le_scan_stop;
    extern const hci_cmd_t gap_le_set_scan_parameters;
    extern const hci_cmd_t gap_le_connect_cmd;
    extern const hci_cmd_t gap_le_connect_cancel_cmd;
    extern const hci_cmd_t gatt_discover_primary_services_cmd;

    extern const hci_cmd_t hci_vendor_le_write_mac;

#if defined __cplusplus
}
#endif

#endif // __HCI_CMDS_H
