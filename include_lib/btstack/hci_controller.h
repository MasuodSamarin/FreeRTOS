#ifndef _HCI_CONTROLLER_H_
#define _HCI_CONTROLLER_H_


#include "typedef.h"
#include "list.h"
#include "linked_list.h"
#include "utils.h"

/********************************************************************************/
/*
 *                   Memory footprint
 *
 *                   CMD Paylod + Event Payload
 */
#define ACL_PACKETS_TOTAL_NUM               4
#define ACL_DATA_PACKET_LENGTH              680

#define CONTROLLER_MAX_CMD_PAYLOAD          0x400
#define CONTROLLER_MAX_EVENT_PAYLOAD        0x1400

#define CONTROLLER_MAX_TOTAL_PAYLOAD        (CONTROLLER_MAX_CMD_PAYLOAD + \
                                            CONTROLLER_MAX_EVENT_PAYLOAD)

#define CONTROLLER_MAX_ACL_PAYLOAD          (680*5)

#define HCI_BUF_CMD_POS                     0
#define HCI_BUF_EVENT_POS                   (HCI_BUF_CMD_POS + CONTROLLER_MAX_CMD_PAYLOAD)

#define CONTROLLER_MAX_LE_EVENT_PAYLOAD     512

#define OGF_LINK_CONTROL                                    0x01
#define OGF_LINK_POLICY                                     0x02
#define OGF_CONTROLLER_BASEBAND                             0x03
#define OGF_INFORMATIONAL_PARAMETERS                        0x04
#define OGF_STATUS_PARAMETERS                               0x05
#define OGF_TESTING_COMMANDS                                0x06
#define OGF_LE_CONTROLLER_COMMANDS                          0x08

/********************************************************************************/
/*
 *                  Error Code
 *                  Core_v5.0 | Vol 2 | Part D | 2
 */
#define  SUCCESS                                                         0x00
#define  UNKNOWN_HCI_COMMAND                                             0x01
#define  UNKNOWN_CONNECTION_IDENTIFIER                                   0x02
#define  HARDWARE_FAILURE                                                0x03
#define  PAGE_TIMEOUT                                                    0x04
#define  AUTHENTICATION_FAILURE                                          0x05
#define  PIN_OR_KEY_MISSING                                              0x06
#define  MEMORY_CAPACITY_EXCEEDED                                        0x07
#define  CONNECTION_TIMEOUT                                              0x08
#define  CONNECTION_LIMIT_EXCEEDED                                       0x09
#define  SYNCHRONOUS_CONNECTION_LIMIT_TO_A_DEVICE_EXCEEDED               0x0A
#define  ACL_CONNECTION_ALREADY_EXISTS                                   0x0B
#define  COMMAND_DISALLOWED                                              0x0C
#define  CONNECTION_REJECTED_DUE_TO_LIMITED_RESOURCES                    0x0D
#define  CONNECTION_REJECTED_DUE_TO_SECURITY_REASONS                     0x0E
#define  CONNECTION_REJECTED_DUE_TO_UNACCEPTABLE_BD_ADDR                 0x0F
#define  CONNECTION_ACCEPT_TIMEOUT_EXCEEDED                              0x10
#define  UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE                          0x11
#define  INVALID_HCI_COMMAND_PARAMETERS                                  0x12
#define  REMOTE_USER_TERMINATED_CONNECTION                               0x13
#define  REMOTE_DEVICE_TERMINATED_CONNECTION_DUE_TO_LOW_RESOURCES        0x14
#define  REMOTE_DEVICE_TERMINATED_CONNECTION_DUE_TO_POWER_OFF            0x15
#define  CONNECTION_TERMINATED_BY_LOCAL_HOST                             0x16
#define  REPEATED_ATTEMPTS                                               0x17
#define  PAIRING_NOT_ALLOWED                                             0x18
#define  UNKNOWN_LMP_PDU                                                 0x19
#define  UNSUPPORTED_REMOTE_FEATURE_UNSUPPORTED_LMP_FEATURE              0x1A
#define  SCO_OFFSET_REJECTED                                             0x1B
#define  SCO_INTERVAL_REJECTED                                           0x1C
#define  SCO_AIR_MODE_REJECTED                                           0x1D
#define  INVALID_LMP_PARAMETERS_INVALID_LL_PARAMETERS                    0x1E
#define  UNSPECIFIED_ERROR                                               0x1F
#define  UNSUPPORTED_LMP_PARAMETER_VALUE_UNSUPPORTED_LL_PARAMETER_VALUE  0x20
#define  ROLE_CHANGE_NOT_ALLOWED                                         0x21
#define  LMP_RESPONSE_TIMEOUT_LL_RESPONSE_TIMEOUT                        0x22
#define  LMP_ERROR_TRANSACTION_COLLISION                                 0x23
#define  LMP_PDU_NOT_ALLOWED                                             0x24
#define  ENCRYPTION_MODE_NOT_ACCEPTABLE                                  0x25
#define  LINK_KEY_CANNOT_BE_CHANGED                                      0x26
#define  REQUESTED_QOS_NOT_SUPPORTED                                     0x27
#define  INSTANT_PASSED                                                  0x28
#define  PAIRING_WITH_UNIT_KEY_NOT_SUPPORTED                             0x29
#define  DIFFERENT_TRANSACTION_COLLISION                                 0x2A
#define  RESERVED                                                        0x2B
#define  QOS_UNACCEPTABLE_PARAMETER                                      0x2C
#define  QOS_REJECTED                                                    0x2D
#define  CHANNEL_CLASSIFICATION_NOT_SUPPORTED                            0x2E
#define  INSUFFICIENT_SECURITY                                           0x2F
#define  PARAMETER_OUT_OF_MANDATORY_RANGE                                0x30
#define  RESERVED_PAD1                                                   0x31
#define  ROLE_SWITCH_PENDING                                             0x32
#define  RESERVED_PAD2                                                   0x33
#define  RESERVED_SLOT_VIOLATION                                         0x34
#define  ROLE_SWITCH_FAILED                                              0x35
#define  EXTENDED_INQUIRY_RESPONSE_TOO_LARGE                             0x36
#define  SECURE_SIMPLE_PAIRING_NOT_SUPPORTED_BY_HOST                     0x37
#define  HOST_BUSY_PAIRING                                               0x38
#define  CONNECTION_REJECTED_DUE_TO_NO_SUITABLE_CHANNEL_FOUND            0x39
#define  CONTROLLER_BUSY                                                 0x3A
#define  UNACCEPTABLE_CONNECTION_PARAMETERS                              0x3B
#define  DIRECTED_ADVERTISING_TIMEOUT                                    0x3C
#define  CONNECTION_TERMINATED_DUE_TO_MIC_FAILURE                        0x3D
#define  CONNECTION_FAILED_TO_BE_ESTABLISHED                             0x3E
#define  MAC_CONNECTION_FAILED                                           0x3F
#define  COARSE_CLOCK_ADJUSTMENT_REJECTED_BUT_WILL_TRY_TO_ADJUST_USING_CLOCK_DRAGGING 0x40

/********************************************************************************/
/*
 *                  Link Control Commands
 *                  Core_v5.0 | Vol 2 | Part E | 7.1
 */
#define HCI_INQUIRY                                         0x0001
#define HCI_INQUIRY_CANCEL                                  0x0002
#define HCI_PERIODIC_INQUIRY_MODE                           0x0003
#define HCI_EXIT_PERIODIC_INQUIRY_MODE                      0x0004
#define HCI_CREATE_CONNECTION                               0x0005
#define HCI_DISCONNECT                                      0x0006
#define HCI_CREATE_CONNECTION_CANCEL                        0x0008
#define HCI_ACCEPT_CONNECTION_REQUEST                       0x0009
#define HCI_REJECT_CONNECTION_REQUEST                       0x000A
#define HCI_LINK_KEY_REQUEST_REPLY                          0x000B
#define HCI_LINK_KEY_REQUEST_NEGATIVE_REPLY                 0x000C
#define HCI_PIN_CODE_REQUEST_REPLY                          0x000D
#define HCI_PIN_CODE_REQUEST_NEGATIVE_REPLY                 0x000E
#define HCI_CHANGE_CONNECTION_PACKET_TYPE                   0x000F
#define HCI_AUTHENTICATION_REQUESTED                        0x0011
#define HCI_SET_CONNECTION_ENCRYPTION                       0x0013
#define HCI_CHANGE_CONNECTION_LINK_KEY                      0x0015
#define HCI_MASTER_LINK_KEY                                 0x0017
#define HCI_REMOTE_NAME_REQUEST                             0x0019
#define HCI_REMOTE_NAME_REQUEST_CANCEL                      0x001A
#define HCI_READ_REMOTE_SUPPORTED_FEATURES                  0x001B
#define HCI_READ_REMOTE_EXTENDED_FEATURES                   0x001C
#define HCI_READ_REMOTE_VERSION_INFORMATION                 0x001D
#define HCI_READ_CLOCK_OFFSET                               0x001F
#define HCI_READ_LMP_HANDLE                                 0x0020

#define HCI_SETUP_SYNCHRONOUS_CONNECTION                    0x0028
#define HCI_ACCEPT_SYNCHRONOUS_CONNECTION_REQUEST           0x0029
#define HCI_REJECT_SYNCHRONOUS_CONNECTION_REQUEST           0x002A
#define HCI_IO_CAPABILITY_REQUEST_REPLY                     0x002B
#define HCI_USER_CONFIRMATION_REQUEST_REPLY                 0x002C
#define HCI_USER_CONFIRMATION_REQUEST_NEGATIVE_REPLY        0x002D
#define HCI_USER_PASSKEY_REQUEST_REPLY                      0x002E
#define HCI_USER_PASSKEY_REQUEST_NEGATIVE_REPLY             0x002F
#define HCI_REMOTE_OOB_DATA_REQUEST_REPLY                   0x0030
#define HCI_REMOTE_OOB_DATA_REQUEST_NEGATIVE_REPLY          0x0033
#define HCI_IO_CAPABILITY_REQUEST_NEGATIVE_REPLY            0x0034
#define HCI_CREATE_PHYSICAL_LINK                            0x0035
#define HCI_ACCEPT_PHYSICAL_LINK                            0x0036
#define HCI_DISCONNECT_PHYSICAL_LINK                        0x0037
#define HCI_CREATE_LOGICAL_LINK                             0x0038
#define HCI_ACCEPT_LOGICAL_LINK                             0x0039
#define HCI_DISCONNEXT_LOGICAL_LINK                         0x003A
#define HCI_LOGICAL_LINK_CANCEL                             0x003B
#define HCI_FLOW_SPEC_MODIFY                                0x003C


#define HCI_BT_UPDATA_LINK                                  0x00fb

/********************************************************************************/
/*
 *                  Link Policy Commands
 *                  Core_v5.0 | Vol 2 | Part E | 7.2
 */
#define HCI_HOLD_MODE                                       0x0001
#define HCI_SNIFF_MODE                                      0x0003
#define HCI_EXIT_SNIFF_MODE                                 0x0004
#define HCI_PARK_STATE                                      0x0005
#define HCI_EXIT_PARK_STATE                                 0x0006
#define HCI_QOS_SETUP                                       0x0007
#define HCI_ROLE_DISCOVERY                                  0x0009
#define HCI_SWITCH_ROLE                                     0x000B
#define HCI_READ_LINK_POLICY_SETTINGS                       0x000C
#define HCI_WRITE_LINK_POLICY_SETTINGS                      0x000D
#define HCI_READ_DEFAULT_LINK_POLICY_SETTINGS               0x000E
#define HCI_WRITE_DEFAULT_LINK_POLICY_SETTINGS              0x000F
#define HCI_FLOW_SPECIFICATION                              0X0010
#define HCI_SNIFF_SUBRATING                                 0X0011


/********************************************************************************/
/*
 *                  Controller & Baseband Commands
 *                  Core_v5.0 | Vol 2 | Part E | 7.3
 */
#define HCI_SET_EVENT_MASK                                  0x0001
#define HCI_RESET                                           0x0003
#define HCI_SET_EVENT_FILTER                                0x0005
#define HCI_FLUSH                                           0x0008
#define HCI_READ_PIN_TYPE                                   0x0009
#define HCI_WRITE_PIN_TYPE                                  0x000A
#define HCI_CREAT_NEW_UNIT_KEY                              0x000B
#define HCI_READ_STORED_LINK_KEY                            0x000D
#define HCI_WRITE_STORED_LINK_KEY                           0x0011
#define HCI_DELETE_STORED_LINK_KEY                          0x0012
#define HCI_WRITE_LOCAL_NAME                                0x0013
#define HCI_READ_LOCAL_NAME                                 0x0014
#define HCI_READ_CONNECTION_ACCEPT_TIMEOUT                  0x0015
#define HCI_WRITE_CONNECTION_ACCEPT_TIMEOUT                 0x0016
#define HCI_READ_PAGE_TIMEOUT                               0x0017
#define HCI_WRITE_PAGE_TIMEOUT                              0x0018
#define HCI_READ_SCAN_ENABLE                                0x0019
#define HCI_WRITE_SCAN_ENABLE                               0x001A
#define HCI_READ_PAGE_SCAN_ACTIVITY                         0x001B
#define HCI_WRITE_PAGE_SCAN_ACTIVITY                        0x001C
#define HCI_READ_INQUIRY_SCAN_ACTIVITY                      0x001D
#define HCI_WRITE_INQUIRY_SCAN_ACTIVITY                     0x001E
#define HCI_READ_AUTHENTICATION_ENABLE                      0x001F
#define HCI_WRITE_AUTHENTICATION_ENABLE                     0x0020
#define HCI_READ_CLASS_OF_DEVICE                            0x0023
#define HCI_WRITE_CLASS_OF_DEVICE                           0x0024
#define HCI_READ_VOICE_SETTING                              0x0025
#define HCI_WRITE_VOICE_SETTING                             0x0026
#define HCI_READ_AUTOMATIC_FLUSH_TIMEOUT                    0x0027
#define HCI_WRITE_AUTOMATIC_FLUSH_TIMEOUT                   0x0028
#define HCI_READ_NUM_BROADCAST_RETRANSMISSIONS              0x0029
#define HCI_WRITE_NUM_BROADCAST_RETRANSMISSIONS             0x002A
#define HCI_READ_HOLD_MODE_ACTIVITY                         0x002B
#define HCI_WRITE_HOLD_MODE_ACTIVITY                        0x002C
#define HCI_READ_TRANSMIT_POWER_LEVEL                       0x002D
#define HCI_READ_SYNCHRONOUS_FLOW_CONTROL_ENABLE            0x002E
#define HCI_WRITE_SYNCHRONOUS_FLOW_CONTROL_ENABLE           0x002F
#define HCI_SET_CONTROLLER_TO_HOST_FLOW_CONTROL             0x0031
#define HCI_HOST_BUFFER_SIZE                                0x0033
#define HCI_HOST_NUMBER_OF_COMPLETED_PACKETS                0x0035
#define HCI_READ_LINK_SUPERVISION_TIMEOUT                   0x0036
#define HCI_WRITE_LINK_SUPERVISION_TIMEOUT                  0x0037
#define HCI_READ_NUMBER_OF_SUPPORTED_IAC                    0x0038
#define HCI_READ_CURRENT_IAC_LAP                            0x0039
#define HCI_WRITE_CURRENT_IAC_LAP                           0x003A
#define HCI_SET_AFH_HOST_CHANNEL_CLASSIFICATION             0x003F
#define HCI_READ_INQUIRY_SCAN_TYPE                          0x0042
#define HCI_WRITE_INQUIRY_SCAN_TYPE                         0x0043
#define HCI_READ_INQUIRY_MODE                               0x0044
#define HCI_WRITE_INQUIRY_MODE                              0x0045
#define HCI_READ_PAGE_SCAN_TYPE                             0x0046
#define HCI_WRITE_PAGE_SCAN_TYPE                            0x0047
#define HCI_READ_AFH_CHANNEL_ASSESSMENT_MODE                0x0048
#define HCI_WRITE_AFH_CHANNEL_ASSESSMENT_MODE               0x0049
#define HCI_READ_EXTENDED_INQUIRY_RESPONSE                  0x0051
#define HCI_WRITE_EXTENDED_INQUIRY_RESPONSE                 0x0052
#define HCI_REFRESH_ENCRYPTION_KEY                          0x0053
#define HCI_READ_SIMPLE_PAIRING_MODE                        0x0055
#define HCI_WRITE_SIMPLE_PAIRING_MODE                       0x0056
#define HCI_READ_LOCAL_OOB_DATA                             0x0057
#define HCI_READ_INQUIRY_RESPONSE_TRANSMIT_POWER_LEVEL      0x0058
#define HCI_WRITE_INQUIRY_TRANSMIT_POWER_LEVEL              0x0059
#define HCI_SEND_KEYPRESS_NOTIFICATION                      0x0060
#define HCI_READ_DEFAULT_ERR_DATA_REPORTING                 0x005A
#define HCI_WRITE_DEFAULT_ERR_DATA_REPORTING                0x005B
#define HCI_ENHANCED_FLUSH                                  0x005F
#define HCI_READ_LOGICAL_LINK_ACCEPT_TIMEOUT                0x0061
#define HCI_WRITE_LOGICAL_LINK_ACCEPT_TIMEOUT               0x0062
#define HCI_SET_EVENT_MASK_PAGE_2                           0x0063
#define HCI_READ_LOCATION_DATA                              0x0064
#define HCI_WRITE_LOCATION_DATA                             0x0065
#define HCI_READ_FLOW_CONTROL_MODE                          0x0066
#define HCI_WRITE_FLOW_CONTROL_MODE                         0x0067
#define HCI_READ_ENHANCE_TRANSMIT_POWER_LEVEL               0x0068
#define HCI_READ_BEST_EFFORT_FLUSH_TIMEOUT                  0x0069
#define HCI_WRITE_BEST_EFFORT_FLUSH_TIMEOUT                 0x006A
#define HCI_SHORT_RANGE_MODE                                0x006B
#define HCI_READ_LE_HOST_SUPPORT                            0x006C
#define HCI_WRITE_LE_HOST_SUPPORT                           0x006D
#define HCI_WRITE_SECURE_CONNECTIONS_HOST_SUPPORT           0x007A

/********************************************************************************/
/*
 *                  Informational Parameters Commands
 *                  Core_v5.0 | Vol 2 | Part E | 7.4
 */
#define HCI_READ_LOCAL_VERSION_INFORMATION                  0x0001
#define HCI_READ_LOCAL_SUPPORTED_COMMANDS                   0x0002
#define HCI_READ_LOCAL_SUPPORTED_FEATURES                   0x0003
#define HCI_READ_LOCAL_EXTENDED_FEATURES                    0x0004
#define HCI_READ_BUFFER_SIZE                                0x0005
#define HCI_READ_BD_ADDR                                    0x0009
#define HCI_READ_DATA_BLOCK_SIZE                            0x000A
#define HCI_READ_LOCAL_SUPPORTED_CODECS                     0x000B


/********************************************************************************/
/*
 *                  Status Parameters Commands
 *                  Core_v5.0 | Vol 2 | Part E | 7.5
 */
#define HCI_READ_FAILED_CONTACT_COUNTER                     0x0001
#define HCI_RESET_FAILED_CONTACT_COUNTER                    0x0002
#define HCI_READ_LINK_QUALITY                               0x0003
#define HCI_READ_RSSI                                       0x0005
#define HCI_READ_AFH_CHANNEL_MAP                            0x0006
#define HCI_READ_CLOCK                                      0x0007
#define HCI_READ_ENCRYPTION_KEY_SIZE                        0x0008
#define HCI_READ_LOCAL_AMP_INFO                             0x0009
#define HCI_READ_LOCAL_AMP_ASSOC                            0x000A
#define HCI_WRITE_REMOTE_AMP_ASSOC                          0x000B


/********************************************************************************/
/*
 *                  Testing Commands
 *                  Core_v5.0 | Vol 2 | Part E | 7.6
 */
#define HCI_READ_LOOPBACK_MODE                              0x0001
#define HCI_WRITE_LOOPBACK_MODE                             0x0002
#define HCI_ENABLE_DEVICE_UNDER_TEST_MODE                   0x0003
#define HCI_WRITE_SIMPLE_PAIRING_DEBUG_MODE                 0x0004
#define HCI_ENABLE_AMP_RECEIVER_REPORTS                     0x0007
#define HCI_AMP_TEST_END                                    0x0008
#define HCI_AMP_TEST                                        0x0009


/********************************************************************************/
/*
 *                  LE Controller Commands
 *                  Core_v5.0 | Vol 2 | Part E | 7.8
 */
#define  HCI_LE_SET_EVENT_MASK 							    0x01 //Mandatory
#define  HCI_LE_READ_BUFFER_SIZE 						    0x02 //Mandatory
#define  HCI_LE_READ_LOCAL_SUPPORT_FEATURES 			    0x03 //Mandatory
#define  HCI_LE_SET_RANDOM_ADDRESS 						    0x05
#define  HCI_LE_SET_ADVERTISING_PARAMETERS 				    0x06
#define  HCI_LE_READ_ADVERTISING_CHANNEL_TX_POWER		    0x07
#define  HCI_LE_SET_ADVERTISING_DATA 					    0x08
#define  HCI_LE_SET_SCAN_RESPONSE_DATA 					    0x09
#define  HCI_LE_SET_ADVERTISE_ENABLE 					    0x0a
#define  HCI_LE_SET_SCAN_PARAMETERS 					    0x0b
#define  HCI_LE_SET_SCAN_ENABLE 						    0x0c
#define  HCI_LE_CREATE_CONNECTION 						    0x0d
#define  HCI_LE_CREATE_CONNECTION_CANCEL 				    0x0e
#define  HCI_LE_READ_WHITE_LIST_SIZE 					    0x0f //Mandatory
#define  HCI_LE_CLEAR_WHITE_LIST 						    0x10 //Mandatory
#define  HCI_LE_ADD_DEVICE_TO_WHITE_LIST 				    0x11 //Mandatory
#define  HCI_LE_REMOVE_DEVICE_FROM_WHITE_LIST 			    0x12 //Mandatory
#define  HCI_LE_CONNECTION_UPDATE 						    0x13
#define  HCI_LE_SET_HOST_CHANNEL_CLASSIFICATION 		    0x14
#define  HCI_LE_READ_CHANNEL_MAP 						    0x15
#define  HCI_LE_READ_REMOTE_USED_FEATURES 				    0x16
#define  HCI_LE_ENCRYPT 								    0x17
#define  HCI_LE_RAND 									    0x18
#define  HCI_LE_START_ENCRYPT 							    0x19
#define  HCI_LE_LONG_TERM_KEY_REQUEST_REPLY 			    0x1a
#define  HCI_LE_LONG_TERM_KEY_REQUEST_NAGATIVE_REPLY 	    0x1b
#define  HCI_LE_READ_SUPPORTED_STATES 					    0x1c //Mandatory
#define  HCI_LE_RECEIVER_TEST 							    0x1d
#define  HCI_LE_TRANSMITTER_TEST 						    0x1e
#define  HCI_LE_TEST_END 								    0x1f //Mandatory
#define  HCI_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_REPLY   0X20
#define  HCI_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_NEGATIVE_REPLY  0x21
#define  HCI_LE_SET_DATA_LENGTH                             0x22
#define  HCI_LE_READ_SUGGESTED_DEFAULT_DATA_LENGTH          0x23
#define  HCI_LE_WRITE_SUGGESTED_DEFAULT_DATA_LENGTH         0x24
#define  HCI_LE_READ_LOCAL_P256_PUBLIC_KEY                  0x25
#define  HCI_LE_GENERATE_DHKEY                              0x26
#define  HCI_LE_ADD_DEVICE_TO_RESOLVING_LIST                0x27
#define  HCI_LE_REMOVE_DEVICE_FROM_RESOLVING_LIST           0x28
#define  HCI_LE_CLEAR_RESOLVING_LIST                        0x29
#define  HCI_LE_READ_RESOLVING_LIST_SIZE                    0x2A
#define  HCI_LE_READ_PEER_RESOLVABLE_ADDRESS                0x2B
#define  HCI_LE_READ_LOCAL_RESOLVABLE_ADDRESS               0x2C
#define  HCI_LE_SET_ADDRESS_RESOLUTION_ENABLE               0x2D
#define  HCI_LE_SET_RESOLVABLE_PRIVATE_ADDRESS_TIMEOUT      0x2E
#define  HCI_LE_READ_MAXIMUM_DATA_LENGTH                    0x2F


/********************************************************************************/
/*
 *                  Supported Commands
 *                  Core_v5.0 | Vol 2 | Part E | 6.27
 */
//Octet 0
#define  BIT_HCI_INQUIRY                                                    BIT(0)
#define  BIT_HCI_INQUIRY_CANCEL                                             BIT(1)
#define  BIT_HCI_PERIODIC_INQUIRY_MODE                                      BIT(2)
#define  BIT_HCI_EXIT_PERIODIC_INQUIRY_MODE                                 BIT(3)
#define  BIT_HCI_CREATE_CONNECTION                                          BIT(4)
#define  BIT_HCI_DISCONNECT                                                 BIT(5)
#define  BIT_HCI_ADD_SCO_CONNECTION                                         BIT(6)
#define  BIT_HCI_CREATE_CONNECTION_CANCEL                                   BIT(7)

#define SUPPORTED_COMMANDS_OCTET00       \
    BIT_HCI_INQUIRY|\
	BIT_HCI_INQUIRY_CANCEL|\
	BIT_HCI_PERIODIC_INQUIRY_MODE|\
	BIT_HCI_EXIT_PERIODIC_INQUIRY_MODE|\
	BIT_HCI_CREATE_CONNECTION|\
	BIT_HCI_DISCONNECT|\
	BIT_HCI_ADD_SCO_CONNECTION|\
	BIT_HCI_CREATE_CONNECTION_CANCEL

//Octet 1
#define  BIT_HCI_ACCEPT_CONNECTION_REQUEST                                  BIT(0)
#define  BIT_HCI_REJECT_CONNECTION_REQUEST                                  BIT(1)
#define  BIT_HCI_LINK_KEY_REQUEST_REPLY                                     BIT(2)
#define  BIT_HCI_LINK_KEY_REQUEST_NEGATIVE_REPLY                            BIT(3)
#define  BIT_HCI_PIN_CODE_REQUEST_REPLY                                     BIT(4)
#define  BIT_HCI_PIN_CODE_REQUEST_NEGATIVE_REPLY                            BIT(5)
#define  BIT_HCI_CHANGE_CONNECTION_PACKET_TYPE                              BIT(6)
#define  BIT_HCI_AUTHENTICATION_REQUESTED                                   BIT(7)

#define SUPPORTED_COMMANDS_OCTET01       \
	BIT_HCI_ACCEPT_CONNECTION_REQUEST|\
	BIT_HCI_REJECT_CONNECTION_REQUEST|\
	BIT_HCI_LINK_KEY_REQUEST_REPLY|\
	BIT_HCI_LINK_KEY_REQUEST_NEGATIVE_REPLY|\
	BIT_HCI_PIN_CODE_REQUEST_REPLY|\
	BIT_HCI_PIN_CODE_REQUEST_NEGATIVE_REPLY|\
	BIT_HCI_CHANGE_CONNECTION_PACKET_TYPE|\
	BIT_HCI_AUTHENTICATION_REQUESTED

//Octet 2
#define  BIT_HCI_SET_CONNECTION_ENCRYPTION                                  BIT(0)
#define  BIT_HCI_CHANGE_CONNECTION_LINK_KEY                                 BIT(1)
#define  BIT_HCI_MASTER_LINK_KEY                                            BIT(2)
#define  BIT_HCI_REMOTE_NAME_REQUEST                                        BIT(3)
#define  BIT_HCI_REMOTE_NAME_REQUEST_CANCEL                                 BIT(4)
#define  BIT_HCI_READ_REMOTE_SUPPORTED_FEATURES                             BIT(5)
#define  BIT_HCI_READ_REMOTE_EXTENDED_FEATURES                              BIT(6)
#define  BIT_HCI_READ_REMOTE_VERSION_INFORMATION                            BIT(7)

#define SUPPORTED_COMMANDS_OCTET02       \
	BIT_HCI_SET_CONNECTION_ENCRYPTION|\
	BIT_HCI_CHANGE_CONNECTION_LINK_KEY|\
	BIT_HCI_MASTER_LINK_KEY|\
	BIT_HCI_REMOTE_NAME_REQUEST|\
	BIT_HCI_REMOTE_NAME_REQUEST_CANCEL|\
	BIT_HCI_READ_REMOTE_SUPPORTED_FEATURES|\
	BIT_HCI_READ_REMOTE_EXTENDED_FEATURES|\
	BIT_HCI_READ_REMOTE_VERSION_INFORMATION

//Octet 3
#define  BIT_HCI_READ_CLOCK_OFFSET                                          BIT(0)
#define  BIT_HCI_READ_LMP_HANDLE                                            BIT(1)

#define SUPPORTED_COMMANDS_OCTET03       \
    BIT_HCI_READ_CLOCK_OFFSET|\
	BIT_HCI_READ_LMP_HANDLE

//Octet 4
#define  BIT_HCI_HOLD_MODE                                                  BIT(1)
#define  BIT_HCI_SNIFF_MODE                                                 BIT(2)
#define  BIT_HCI_EXIT_SNIFF_MODE                                            BIT(3)
#define  BIT_HCI_QOS_SETUP                                                  BIT(6)
#define  BIT_HCI_ROLE_DISCOVERY                                             BIT(7)

#define SUPPORTED_COMMANDS_OCTET04       \
	BIT_HCI_ROLE_DISCOVERY

//Octet 5
#define  BIT_HCI_SWITCH_ROLE                                                BIT(0)
#define  BIT_HCI_READ_LINK_POLICY_SETTINGS                                  BIT(1)
#define  BIT_HCI_WRITE_LINK_POLICY_SETTINGS                                 BIT(2)
#define  BIT_HCI_READ_DEFAULT_LINK_POLICY_SETTINGS                          BIT(3)
#define  BIT_HCI_WRITE_DEFAULT_LINK_POLICY_SETTINGS                         BIT(4)
#define  BIT_HCI_FLOW_SPECIFICATION                                         BIT(5)
#define  BIT_HCI_SET_EVENT_MASK                                             BIT(6)
#define  BIT_HCI_RESET                                                      BIT(7)

#define SUPPORTED_COMMANDS_OCTET05       \
	BIT_HCI_SWITCH_ROLE|\
	BIT_HCI_READ_LINK_POLICY_SETTINGS|\
	BIT_HCI_WRITE_LINK_POLICY_SETTINGS|\
	BIT_HCI_READ_DEFAULT_LINK_POLICY_SETTINGS|\
	BIT_HCI_WRITE_DEFAULT_LINK_POLICY_SETTINGS|\
	BIT_HCI_FLOW_SPECIFICATION|\
	BIT_HCI_SET_EVENT_MASK|\
	BIT_HCI_RESET

//Octet 6
#define  BIT_HCI_SET_EVENT_FILTER                                           BIT(0)
#define  BIT_HCI_FLUSH                                                      BIT(1)
#define  BIT_HCI_READ_PIN_TYPE                                              BIT(2)
#define  BIT_HCI_WRITE_PIN_TYPE                                             BIT(3)
#define  BIT_HCI_CREAT_NEW_UNIT_KEY                                         BIT(4)
#define  BIT_HCI_READ_STORED_LINK_KEY                                       BIT(5)
#define  BIT_HCI_WRITE_STORED_LINK_KEY                                      BIT(6)
#define  BIT_HCI_DELETE_STORED_LINK_KEY                                     BIT(7)

#define SUPPORTED_COMMANDS_OCTET06       \
	BIT_HCI_SET_EVENT_FILTER|\
	BIT_HCI_FLUSH|\
	BIT_HCI_READ_PIN_TYPE|\
	BIT_HCI_WRITE_PIN_TYPE|\
	BIT_HCI_CREAT_NEW_UNIT_KEY|\
	BIT_HCI_READ_STORED_LINK_KEY|\
	BIT_HCI_WRITE_STORED_LINK_KEY|\
	BIT_HCI_DELETE_STORED_LINK_KEY

//Octet 7
#define  BIT_HCI_WRITE_LOCAL_NAME                                           BIT(0)
#define  BIT_HCI_READ_LOCAL_NAME                                            BIT(1)
#define  BIT_HCI_READ_CONNECTION_ACCEPT_TIMEOUT                             BIT(2)
#define  BIT_HCI_WRITE_CONNECTION_ACCEPT_TIMEOUT                            BIT(3)
#define  BIT_HCI_READ_PAGE_TIMEOUT                                          BIT(4)
#define  BIT_HCI_WRITE_PAGE_TIMEOUT                                         BIT(5)
#define  BIT_HCI_READ_SCAN_ENABLE                                           BIT(6)
#define  BIT_HCI_WRITE_SCAN_ENABLE                                          BIT(7)

#define SUPPORTED_COMMANDS_OCTET07       \
	BIT_HCI_WRITE_LOCAL_NAME|\
	BIT_HCI_READ_LOCAL_NAME|\
	BIT_HCI_READ_CONNECTION_ACCEPT_TIMEOUT|\
	BIT_HCI_WRITE_CONNECTION_ACCEPT_TIMEOUT|\
	BIT_HCI_READ_PAGE_TIMEOUT|\
	BIT_HCI_WRITE_PAGE_TIMEOUT|\
	BIT_HCI_READ_SCAN_ENABLE|\
	BIT_HCI_WRITE_SCAN_ENABLE

//Octet 8
#define  BIT_HCI_READ_PAGE_SCAN_ACTIVITY                                    BIT(0)
#define  BIT_HCI_WRITE_PAGE_SCAN_ACTIVITY                                   BIT(1)
#define  BIT_HCI_READ_INQUIRY_SCAN_ACTIVITY                                 BIT(2)
#define  BIT_HCI_WRITE_INQUIRY_SCAN_ACTIVITY                                BIT(3)
#define  BIT_HCI_READ_AUTHENTICATION_ENABLE                                 BIT(4)
#define  BIT_HCI_WRITE_AUTHENTICATION_ENABLE                                BIT(5)

#define SUPPORTED_COMMANDS_OCTET08       \
	BIT_HCI_READ_PAGE_SCAN_ACTIVITY|\
	BIT_HCI_WRITE_PAGE_SCAN_ACTIVITY|\
	BIT_HCI_READ_INQUIRY_SCAN_ACTIVITY|\
	BIT_HCI_WRITE_INQUIRY_SCAN_ACTIVITY|\
	BIT_HCI_READ_AUTHENTICATION_ENABLE|\
	BIT_HCI_WRITE_AUTHENTICATION_ENABLE

//Octet 9
#define  BIT_HCI_READ_CLASS_OF_DEVICE                                       BIT(0)
#define  BIT_HCI_WRITE_CLASS_OF_DEVICE                                      BIT(1)
#define  BIT_HCI_READ_VOICE_SETTING                                         BIT(2)
#define  BIT_HCI_WRITE_VOICE_SETTING                                        BIT(3)
#define  BIT_HCI_READ_AUTOMATIC_FLUSH_TIMEOUT                               BIT(4)
#define  BIT_HCI_WRITE_AUTOMATIC_FLUSH_TIMEOUT                              BIT(5)
#define  BIT_HCI_READ_NUM_BROADCAST_RETRANSMISSIONS                         BIT(6)
#define  BIT_HCI_WRITE_NUM_BROADCAST_RETRANSMISSIONS                        BIT(7)

#define SUPPORTED_COMMANDS_OCTET09       \
	BIT_HCI_READ_CLASS_OF_DEVICE|\
	BIT_HCI_WRITE_CLASS_OF_DEVICE|\
	BIT_HCI_READ_VOICE_SETTING|\
	BIT_HCI_WRITE_VOICE_SETTING|\
	BIT_HCI_READ_AUTOMATIC_FLUSH_TIMEOUT|\
	BIT_HCI_WRITE_AUTOMATIC_FLUSH_TIMEOUT|\
	BIT_HCI_READ_NUM_BROADCAST_RETRANSMISSIONS|\
	BIT_HCI_WRITE_NUM_BROADCAST_RETRANSMISSIONS

//Octet 10
#define  BIT_HCI_READ_HOLD_MODE_ACTIVITY                                    BIT(0)
#define  BIT_HCI_WRITE_HOLD_MODE_ACTIVITY                                   BIT(1)
#define  BIT_HCI_READ_TRANSMIT_POWER_LEVEL                                  BIT(2)
#define  BIT_HCI_READ_SYNCHRONOUS_FLOW_CONTROL_ENABLE                       BIT(3)
#define  BIT_HCI_WRITE_SYNCHRONOUS_FLOW_CONTROL_ENABLE                      BIT(4)
#define  BIT_HCI_SET_CONTROLLER_TO_HOST_FLOW_CONTROL                        BIT(5)
#define  BIT_HCI_HOST_BUFFER_SIZE                                           BIT(6)
#define  BIT_HCI_HOST_NUMBER_OF_COMPLETED_PACKETS                           BIT(7)

#define SUPPORTED_COMMANDS_OCTET10       \
	BIT_HCI_READ_HOLD_MODE_ACTIVITY|\
	BIT_HCI_WRITE_HOLD_MODE_ACTIVITY|\
	BIT_HCI_READ_TRANSMIT_POWER_LEVEL|\
	BIT_HCI_READ_SYNCHRONOUS_FLOW_CONTROL_ENABLE|\
	BIT_HCI_WRITE_SYNCHRONOUS_FLOW_CONTROL_ENABLE|\
	BIT_HCI_SET_CONTROLLER_TO_HOST_FLOW_CONTROL|\
	BIT_HCI_HOST_BUFFER_SIZE|\
	BIT_HCI_HOST_NUMBER_OF_COMPLETED_PACKETS

//Octet 11
#define  BIT_HCI_READ_LINK_SUPERVISION_TIMEOUT                              BIT(0)
#define  BIT_HCI_WRITE_LINK_SUPERVISION_TIMEOUT                             BIT(1)
#define  BIT_HCI_READ_NUMBER_OF_SUPPORTED_IAC                               BIT(2)
#define  BIT_HCI_READ_CURRENT_IAC_LAP                                       BIT(3)
#define  BIT_HCI_WRITE_CURRENT_IAC_LAP                                      BIT(4)

#define SUPPORTED_COMMANDS_OCTET11       \
	BIT_HCI_READ_LINK_SUPERVISION_TIMEOUT|\
	BIT_HCI_WRITE_LINK_SUPERVISION_TIMEOUT|\
	BIT_HCI_READ_NUMBER_OF_SUPPORTED_IAC|\
	BIT_HCI_READ_CURRENT_IAC_LAP|\
	BIT_HCI_WRITE_CURRENT_IAC_LAP

//Octet 12
#define  BIT_HCI_SET_AFH_HOST_CHANNEL_CLASSIFICATION                        BIT(1)
#define  BIT_HCI_READ_INQUIRY_SCAN_TYPE                                     BIT(4)
#define  BIT_HCI_WRITE_INQUIRY_SCAN_TYPE                                    BIT(5)
#define  BIT_HCI_READ_INQUIRY_MODE                                          BIT(6)
#define  BIT_HCI_WRITE_INQUIRY_MODE                                         BIT(7)

#define SUPPORTED_COMMANDS_OCTET12       \
	BIT_HCI_SET_AFH_HOST_CHANNEL_CLASSIFICATION|\
	BIT_HCI_READ_INQUIRY_SCAN_TYPE|\
	BIT_HCI_WRITE_INQUIRY_SCAN_TYPE|\
	BIT_HCI_READ_INQUIRY_MODE|\
	BIT_HCI_WRITE_INQUIRY_MODE

//Octet 13
#define  BIT_HCI_READ_PAGE_SCAN_TYPE                                        BIT(0)
#define  BIT_HCI_WRITE_PAGE_SCAN_TYPE                                       BIT(1)
#define  BIT_HCI_READ_AFH_CHANNEL_ASSESSMENT_MODE                           BIT(2)
#define  BIT_HCI_WRITE_AFH_CHANNEL_ASSESSMENT_MODE                          BIT(3)

#define SUPPORTED_COMMANDS_OCTET13       \
	BIT_HCI_READ_PAGE_SCAN_TYPE|\
	BIT_HCI_WRITE_PAGE_SCAN_TYPE|\
	BIT_HCI_READ_AFH_CHANNEL_ASSESSMENT_MODE|\
	BIT_HCI_WRITE_AFH_CHANNEL_ASSESSMENT_MODE

//Octet 14
#define  BIT_HCI_READ_LOCAL_VERSION_INFORMATION                             BIT(3)
#define  BIT_HCI_READ_LOCAL_SUPPORTED_FEATURES                              BIT(5)
#define  BIT_HCI_READ_LOCAL_EXTENDED_FEATURES                               BIT(6)
#define  BIT_HCI_READ_BUFFER_SIZE                                           BIT(7)

#define SUPPORTED_COMMANDS_OCTET14       \
	BIT_HCI_READ_LOCAL_VERSION_INFORMATION|\
	BIT_HCI_READ_LOCAL_SUPPORTED_FEATURES|\
	BIT_HCI_READ_LOCAL_EXTENDED_FEATURES|\
	BIT_HCI_READ_BUFFER_SIZE

//Octet 15
#define  BIT_HCI_READ_BD_ADDR                                               BIT(1)
#define  BIT_HCI_READ_FAILED_CONTACT_COUNTER                                BIT(2)
#define  BIT_HCI_RESET_FAILED_CONTACT_COUNTER                               BIT(3)
#define  BIT_HCI_READ_LINK_QUALITY                                          BIT(4)
#define  BIT_HCI_READ_RSSI                                                  BIT(5)
#define  BIT_HCI_READ_AFH_CHANNEL_MAP                                       BIT(6)
#define  BIT_HCI_READ_CLOCK                                                 BIT(7)

#define SUPPORTED_COMMANDS_OCTET15       \
	BIT_HCI_READ_BD_ADDR|\
	BIT_HCI_READ_FAILED_CONTACT_COUNTER|\
	BIT_HCI_RESET_FAILED_CONTACT_COUNTER|\
	BIT_HCI_READ_LINK_QUALITY|\
	BIT_HCI_READ_RSSI|\
	BIT_HCI_READ_AFH_CHANNEL_MAP|\
	BIT_HCI_READ_CLOCK

//Octet 16
#define  BIT_HCI_READ_LOOPBACK_MODE                                         BIT(0)
#define  BIT_HCI_WRITE_LOOPBACK_MODE                                        BIT(1)
#define  BIT_HCI_ENABLE_DEVICE_UNDER_TEST_MODE                              BIT(2)
#define  BIT_HCI_SETUP_SYNCHRONOUS_CONNECTION                               BIT(3)
#define  BIT_HCI_ACCEPT_SYNCHRONOUS_CONNECTION_REQUEST                      BIT(4)
#define  BIT_HCI_REJECT_SYNCHRONOUS_CONNECTION_REQUEST                      BIT(5)

#define SUPPORTED_COMMANDS_OCTET16       \
	BIT_HCI_READ_LOOPBACK_MODE|\
	BIT_HCI_WRITE_LOOPBACK_MODE|\
	BIT_HCI_ENABLE_DEVICE_UNDER_TEST_MODE|\
	BIT_HCI_SETUP_SYNCHRONOUS_CONNECTION|\
	BIT_HCI_ACCEPT_SYNCHRONOUS_CONNECTION_REQUEST|\
	BIT_HCI_REJECT_SYNCHRONOUS_CONNECTION_REQUEST

//Octet 17
#define  BIT_HCI_READ_EXTENDED_INQUIRY_RESPONSE                             BIT(0)
#define  BIT_HCI_WRITE_EXTENDED_INQUIRY_RESPONSE                            BIT(1)
#define  BIT_HCI_REFRESH_ENCRYPTION_KEY                                     BIT(2)
#define  BIT_HCI_SNIFF_SUBRATING                                            BIT(4)
#define  BIT_HCI_READ_SIMPLE_PAIRING_MODE                                   BIT(5)
#define  BIT_HCI_WRITE_SIMPLE_PAIRING_MODE                                  BIT(6)
#define  BIT_HCI_READ_LOCAL_OOB_DATA                                        BIT(7)

#define SUPPORTED_COMMANDS_OCTET17       \
	BIT_HCI_READ_EXTENDED_INQUIRY_RESPONSE|\
	BIT_HCI_WRITE_EXTENDED_INQUIRY_RESPONSE|\
	BIT_HCI_REFRESH_ENCRYPTION_KEY|\
	BIT_HCI_SNIFF_SUBRATING|\
	BIT_HCI_READ_SIMPLE_PAIRING_MODE|\
	BIT_HCI_WRITE_SIMPLE_PAIRING_MODE|\
	BIT_HCI_READ_LOCAL_OOB_DATA

//Octet 18
#define  BIT_HCI_READ_INQUIRY_RESPONSE_TRANSMIT_POWER_LEVEL                 BIT(0)
#define  BIT_HCI_WRITE_INQUIRY_TRANSMIT_POWER_LEVEL                         BIT(1)
#define  BIT_HCI_READ_DEFAULT_ERR_DATA_REPORTING                            BIT(2)
#define  BIT_HCI_WRITE_DEFAULT_ERR_DATA_REPORTING                           BIT(3)
#define  BIT_HCI_IO_CAPABILITY_REQUEST_REPLY                                BIT(7)

#define SUPPORTED_COMMANDS_OCTET18       \
	BIT_HCI_READ_INQUIRY_RESPONSE_TRANSMIT_POWER_LEVEL|\
	BIT_HCI_READ_DEFAULT_ERR_DATA_REPORTING|\
	BIT_HCI_WRITE_DEFAULT_ERR_DATA_REPORTING|\
	BIT_HCI_IO_CAPABILITY_REQUEST_REPLY
// BIT_HCI_WRITE_INQUIRY_TRANSMIT_POWER_LEVEL|\

//Octet 19
#define  BIT_HCI_USER_CONFIRMATION_REQUEST_REPLY                            BIT(0)
#define  BIT_HCI_USER_CONFIRMATION_REQUEST_NEGATIVE_REPLY                   BIT(1)
#define  BIT_HCI_USER_PASSKEY_REQUEST_REPLY                                 BIT(2)
#define  BIT_HCI_USER_PASSKEY_REQUEST_NEGATIVE_REPLY                        BIT(3)
#define  BIT_HCI_REMOTE_OOB_DATA_REQUEST_REPLY                              BIT(4)
#define  BIT_HCI_WRITE_SIMPLE_PAIRING_DEBUG_MODE                            BIT(5)
#define  BIT_HCI_ENHANCED_FLUSH                                             BIT(6)
#define  BIT_HCI_REMOTE_OOB_DATA_REQUEST_NEGATIVE_REPLY                     BIT(7)

#define SUPPORTED_COMMANDS_OCTET19       \
	BIT_HCI_USER_CONFIRMATION_REQUEST_REPLY|\
	BIT_HCI_USER_CONFIRMATION_REQUEST_NEGATIVE_REPLY|\
	BIT_HCI_USER_PASSKEY_REQUEST_REPLY|\
	BIT_HCI_USER_PASSKEY_REQUEST_NEGATIVE_REPLY|\
	BIT_HCI_REMOTE_OOB_DATA_REQUEST_REPLY|\
	BIT_HCI_WRITE_SIMPLE_PAIRING_DEBUG_MODE|\
	BIT_HCI_ENHANCED_FLUSH|\
	BIT_HCI_REMOTE_OOB_DATA_REQUEST_NEGATIVE_REPLY

//Octet 20
#define  BIT_HCI_SEND_KEYPRESS_NOTIFICATION                                 BIT(2)
#define  BIT_HCI_IO_CAPABILITY_REQUEST_NEGATIVE_REPLY                       BIT(3)
#define  BIT_HCI_READ_ENCRYPTION_KEY_SIZE                                   BIT(4)

#define SUPPORTED_COMMANDS_OCTET20       \
	BIT_HCI_SEND_KEYPRESS_NOTIFICATION|\
	BIT_HCI_IO_CAPABILITY_REQUEST_NEGATIVE_REPLY|\
	BIT_HCI_READ_ENCRYPTION_KEY_SIZE

//Octet 21
#define  BIT_HCI_CREATE_PHYSICAL_LINK                                       BIT(0)
#define  BIT_HCI_ACCEPT_PHYSICAL_LINK                                       BIT(1)
#define  BIT_HCI_DISCONNECT_PHYSICAL_LINK                                   BIT(2)
#define  BIT_HCI_CREATE_LOGICAL_LINK                                        BIT(3)
#define  BIT_HCI_ACCEPT_LOGICAL_LINK                                        BIT(4)
#define  BIT_HCI_DISCONNEXT_LOGICAL_LINK                                    BIT(5)
#define  BIT_HCI_LOGICAL_LINK_CANCEL                                        BIT(6)
#define  BIT_HCI_FLOW_SPEC_MODIFY                                           BIT(7)

#define SUPPORTED_COMMANDS_OCTET21       \
	0
// BIT_HCI_CREATE_PHYSICAL_LINK|\
// BIT_HCI_ACCEPT_PHYSICAL_LINK|\
// BIT_HCI_DISCONNECT_PHYSICAL_LINK|\
// BIT_HCI_CREATE_LOGICAL_LINK|\
// BIT_HCI_ACCEPT_LOGICAL_LINK|\
// BIT_HCI_DISCONNEXT_LOGICAL_LINK|\
// BIT_HCI_LOGICAL_LINK_CANCEL|\
// BIT_HCI_FLOW_SPEC_MODIFY

//Octet 22
#define  BIT_HCI_READ_LOGICAL_LINK_ACCEPT_TIMEOUT                           BIT(0)
#define  BIT_HCI_WRITE_LOGICAL_LINK_ACCEPT_TIMEOUT                          BIT(1)
#define  BIT_HCI_SET_EVENT_MASK_PAGE_2                                      BIT(2)
#define  BIT_HCI_READ_LOCATION_DATA                                         BIT(3)
#define  BIT_HCI_WRITE_LOCATION_DATA                                        BIT(4)
#define  BIT_HCI_READ_LOCAL_AMP_INFO                                        BIT(5)
#define  BIT_HCI_READ_LOCAL_AMP_ASSOC                                       BIT(6)
#define  BIT_HCI_WRITE_REMOTE_AMP_ASSOC                                     BIT(7)

#define SUPPORTED_COMMANDS_OCTET22       \
	0
// BIT_HCI_READ_LOGICAL_LINK_ACCEPT_TIMEOUT|\
// BIT_HCI_WRITE_LOGICAL_LINK_ACCEPT_TIMEOUT|\
// BIT_HCI_SET_EVENT_MASK_PAGE_2|\
// BIT_HCI_READ_LOCATION_DATA|\
// BIT_HCI_WRITE_LOCATION_DATA|\
// BIT_HCI_READ_LOCAL_AMP_INFO|\
// BIT_HCI_READ_LOCAL_AMP_ASSOC|\
// BIT_HCI_WRITE_REMOTE_AMP_ASSOC

//Octet 23
#define  BIT_HCI_READ_FLOW_CONTROL_MODE                                     BIT(0)
#define  BIT_HCI_WRITE_FLOW_CONTROL_MODE                                    BIT(1)
#define  BIT_HCI_READ_DATA_BLOCK_SIZE                                       BIT(2)
#define  BIT_HCI_ENABLE_AMP_RECEIVER_REPORTS                                BIT(5)
#define  BIT_HCI_AMP_TEST_END                                               BIT(6)
#define  BIT_HCI_AMP_TEST                                                   BIT(7)

#define SUPPORTED_COMMANDS_OCTET23       \
	0
// BIT_HCI_READ_FLOW_CONTROL_MODE|\
// BIT_HCI_WRITE_FLOW_CONTROL_MODE|\
// BIT_HCI_READ_DATA_BLOCK_SIZE|\
// BIT_HCI_ENABLE_AMP_RECEIVER_REPORTS|\
// BIT_HCI_AMP_TEST_END|\
// BIT_HCI_AMP_TEST

//Octet 24
#define  BIT_HCI_READ_ENHANCE_TRANSMIT_POWER_LEVEL                          BIT(0)
#define  BIT_HCI_READ_BEST_EFFORT_FLUSH_TIMEOUT                             BIT(2)
#define  BIT_HCI_WRITE_BEST_EFFORT_FLUSH_TIMEOUT                            BIT(3)
#define  BIT_HCI_SHORT_RANGE_MODE                                           BIT(4)
#define  BIT_HCI_READ_LE_HOST_SUPPORT                                       BIT(5)
#define  BIT_HCI_WRITE_LE_HOST_SUPPORT                                      BIT(6)

#define SUPPORTED_COMMANDS_OCTET24       \
	BIT_HCI_READ_ENHANCE_TRANSMIT_POWER_LEVEL|\
	BIT_HCI_READ_BEST_EFFORT_FLUSH_TIMEOUT|\
	BIT_HCI_WRITE_BEST_EFFORT_FLUSH_TIMEOUT|\
	BIT_HCI_SHORT_RANGE_MODE|\
	BIT_HCI_READ_LE_HOST_SUPPORT|\
	BIT_HCI_WRITE_LE_HOST_SUPPORT

//Octet 25
#define  BIT_HCI_LE_SET_EVENT_MASK 							                BIT(0)//Mandatory
#define  BIT_HCI_LE_READ_BUFFER_SIZE 						                BIT(1)//Mandatory
#define  BIT_HCI_LE_READ_LOCAL_SUPPORT_FEATURES 			                BIT(2)//Mandatory
#define  BIT_HCI_LE_SET_RANDOM_ADDRESS 						                BIT(4)
#define  BIT_HCI_LE_SET_ADVERTISING_PARAMETERS 				                BIT(5)
#define  BIT_HCI_LE_READ_ADVERTISING_CHANNEL_TX_POWER		                BIT(6)
#define  BIT_HCI_LE_SET_ADVERTISING_DATA 					                BIT(7)

#define SUPPORTED_COMMANDS_OCTET25       \
	BIT_HCI_LE_SET_EVENT_MASK|\
	BIT_HCI_LE_READ_BUFFER_SIZE|\
	BIT_HCI_LE_READ_LOCAL_SUPPORT_FEATURES|\
	BIT_HCI_LE_SET_RANDOM_ADDRESS|\
	BIT_HCI_LE_SET_ADVERTISING_PARAMETERS|\
	BIT_HCI_LE_READ_ADVERTISING_CHANNEL_TX_POWER|\
	BIT_HCI_LE_SET_ADVERTISING_DATA

//Octet 26
#define  BIT_HCI_LE_SET_SCAN_RESPONSE_DATA 					                BIT(0)
#define  BIT_HCI_LE_SET_ADVERTISE_ENABLE 					                BIT(1)
#define  BIT_HCI_LE_SET_SCAN_PARAMETERS 					                BIT(2)
#define  BIT_HCI_LE_SET_SCAN_ENABLE 						                BIT(3)
#define  BIT_HCI_LE_CREATE_CONNECTION 						                BIT(4)
#define  BIT_HCI_LE_CREATE_CONNECTION_CANCEL 				                BIT(5)
#define  BIT_HCI_LE_READ_WHITE_LIST_SIZE 					                BIT(6)//Mandatory
#define  BIT_HCI_LE_CLEAR_WHITE_LIST 						                BIT(7)//Mandatory

#define SUPPORTED_COMMANDS_OCTET26       \
	BIT_HCI_LE_SET_SCAN_RESPONSE_DATA|\
	BIT_HCI_LE_SET_ADVERTISE_ENABLE|\
	BIT_HCI_LE_SET_SCAN_PARAMETERS|\
	BIT_HCI_LE_SET_SCAN_ENABLE|\
	BIT_HCI_LE_CREATE_CONNECTION|\
	BIT_HCI_LE_CREATE_CONNECTION_CANCEL|\
	BIT_HCI_LE_READ_WHITE_LIST_SIZE|\
	BIT_HCI_LE_CLEAR_WHITE_LIST

//Octet 27
#define  BIT_HCI_LE_ADD_DEVICE_TO_WHITE_LIST 				                BIT(0)//Mandatory
#define  BIT_HCI_LE_REMOVE_DEVICE_FROM_WHITE_LIST 			                BIT(1)//Mandatory
#define  BIT_HCI_LE_CONNECTION_UPDATE 						                BIT(2)
#define  BIT_HCI_LE_SET_HOST_CHANNEL_CLASSIFICATION 		                BIT(3)
#define  BIT_HCI_LE_READ_CHANNEL_MAP 						                BIT(4)
#define  BIT_HCI_LE_READ_REMOTE_USED_FEATURES 				                BIT(5)
#define  BIT_HCI_LE_ENCRYPT 								                BIT(6)
#define  BIT_HCI_LE_RAND 									                BIT(7)

#define SUPPORTED_COMMANDS_OCTET27       \
	BIT_HCI_LE_ADD_DEVICE_TO_WHITE_LIST|\
	BIT_HCI_LE_REMOVE_DEVICE_FROM_WHITE_LIST|\
	BIT_HCI_LE_CONNECTION_UPDATE|\
	BIT_HCI_LE_SET_HOST_CHANNEL_CLASSIFICATION|\
	BIT_HCI_LE_READ_CHANNEL_MAP|\
	BIT_HCI_LE_READ_REMOTE_USED_FEATURES|\
	BIT_HCI_LE_ENCRYPT|\
	BIT_HCI_LE_RAND

//Octet 28
#define  BIT_HCI_LE_START_ENCRYPT 							                BIT(0)
#define  BIT_HCI_LE_LONG_TERM_KEY_REQUEST_REPLY 			                BIT(1)
#define  BIT_HCI_LE_LONG_TERM_KEY_REQUEST_NAGATIVE_REPLY 	                BIT(2)
#define  BIT_HCI_LE_READ_SUPPORTED_STATES 					                BIT(3)//Mandatory
#define  BIT_HCI_LE_RECEIVER_TEST 							                BIT(4)
#define  BIT_HCI_LE_TRANSMITTER_TEST 						                BIT(5)
#define  BIT_HCI_LE_TEST_END 								                BIT(6)//Mandatory

#define SUPPORTED_COMMANDS_OCTET28       \
	BIT_HCI_LE_START_ENCRYPT|\
	BIT_HCI_LE_LONG_TERM_KEY_REQUEST_REPLY|\
	BIT_HCI_LE_LONG_TERM_KEY_REQUEST_NAGATIVE_REPLY|\
	BIT_HCI_LE_READ_SUPPORTED_STATES|\
	BIT_HCI_LE_RECEIVER_TEST|\
	BIT_HCI_LE_TRANSMITTER_TEST|\
	BIT_HCI_LE_TEST_END

//Octet 29
#define  BIT_HCI_READ_LOCAL_SUPPORTED_CODECS                                BIT(5)

#define SUPPORTED_COMMANDS_OCTET29       \
	0
// BIT_HCI_READ_LOCAL_SUPPORTED_CODECS

//Octet 30
#define SUPPORTED_COMMANDS_OCTET30       \
    0

//Octet 31
#define SUPPORTED_COMMANDS_OCTET31       \
    0

//Octet 32
#define SUPPORTED_COMMANDS_OCTET32       \
    0

//Octet 33
#define  BIT_HCI_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_REPLY               BIT(4)
#define  BIT_HCI_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_NEGATIVE_REPLY      BIT(5)
#define  BIT_HCI_LE_SET_DATA_LENGTH                                         BIT(6)
#define  BIT_HCI_LE_READ_SUGGESTED_DEFAULT_DATA_LENGTH                      BIT(7)

#define SUPPORTED_COMMANDS_OCTET33       \
	0
// BIT_HCI_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_REPLY|\
// BIT_HCI_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_NEGATIVE_REPLY|\
// BIT_HCI_LE_SET_DATA_LENGTH|\
// BIT_HCI_LE_READ_SUGGESTED_DEFAULT_DATA_LENGTH

//Octet 34
#define  BIT_HCI_LE_WRITE_SUGGESTED_DEFAULT_DATA_LENGTH                     BIT(0)
#define  BIT_HCI_LE_READ_LOCAL_P256_PUBLIC_KEY                              BIT(1)
#define  BIT_HCI_LE_GENERATE_DHKEY                                          BIT(2)
#define  BIT_HCI_LE_ADD_DEVICE_TO_RESOLVING_LIST                            BIT(3)
#define  BIT_HCI_LE_REMOVE_DEVICE_FROM_RESOLVING_LIST                       BIT(4)
#define  BIT_HCI_LE_CLEAR_RESOLVING_LIST                                    BIT(5)
#define  BIT_HCI_LE_READ_RESOLVING_LIST_SIZE                                BIT(6)
#define  BIT_HCI_LE_READ_PEER_RESOLVABLE_ADDRESS                            BIT(7)

#define SUPPORTED_COMMANDS_OCTET34       \
	0
// BIT_HCI_LE_WRITE_SUGGESTED_DEFAULT_DATA_LENGTH|\
// BIT_HCI_LE_READ_LOCAL_P256_PUBLIC_KEY|\
// BIT_HCI_LE_GENERATE_DHKEY|\
// BIT_HCI_LE_ADD_DEVICE_TO_RESOLVING_LIST|\
// BIT_HCI_LE_REMOVE_DEVICE_FROM_RESOLVING_LIST|\
// BIT_HCI_LE_CLEAR_RESOLVING_LIST|\
// BIT_HCI_LE_READ_RESOLVING_LIST_SIZE|\
// BIT_HCI_LE_READ_PEER_RESOLVABLE_ADDRESS

//Octet 35
#define  BIT_HCI_LE_READ_LOCAL_RESOLVABLE_ADDRESS                           BIT(0)
#define  BIT_HCI_LE_SET_ADDRESS_RESOLUTION_ENABLE                           BIT(1)
#define  BIT_HCI_LE_SET_RESOLVABLE_PRIVATE_ADDRESS_TIMEOUT                  BIT(2)
#define  BIT_HCI_LE_READ_MAXIMUM_DATA_LENGTH                                BIT(3)

#define SUPPORTED_COMMANDS_OCTET35       \
	0
// BIT_HCI_LE_READ_LOCAL_RESOLVABLE_ADDRESS|\
// BIT_HCI_LE_SET_ADDRESS_RESOLUTION_ENABLE|\
// BIT_HCI_LE_SET_RESOLVABLE_PRIVATE_ADDRESS_TIMEOUT|\
// BIT_HCI_LE_READ_MAXIMUM_DATA_LENGTH

//Octet 36
#define SUPPORTED_COMMANDS_OCTET36       \
    0

//Octet 37
#define SUPPORTED_COMMANDS_OCTET37       \
    0

//Octet 38
#define SUPPORTED_COMMANDS_OCTET38       \
    0

//Octet 39
#define SUPPORTED_COMMANDS_OCTET39       \
    0

#define SUPPORTED_COMMANDS(idx, cmd)        SUPPORTED_COMMANDS_OCTET##idx & BIT_##cmd


/********************************************************************************/
/*
 *                  Supported Features
 *                  Core_v5.0 | Vol 2 | Part C | 3.3
 */
#define BIT_3_SLOT_PACKETS                                                  BIT(0)
#define BIT_5_SLOT_PACKETS                                                  BIT(1)
#define BIT_ENCRYPTION                                                      BIT(2)
#define BIT_SLOT_OFFSET                                                     BIT(3)
#define BIT_TIMING_ACCURACY                                                 BIT(4)
#define BIT_ROLE_SWITCH                                                     BIT(5)
#define BIT_HOLD_MODE                                                       BIT(6)
#define BIT_SNIFF_MODE                                                      BIT(7)

#define SUPPORTED_FEATURES_OCTET0       \
    BIT_3_SLOT_PACKETS|BIT_5_SLOT_PACKETS|BIT_ENCRYPTION|BIT_SLOT_OFFSET|BIT_TIMING_ACCURACY|BIT_ROLE_SWITCH|BIT_HOLD_MODE|BIT_SNIFF_MODE

#define BIT_PARK_STATE                                                      BIT(0)
#define BIT_POWER_CONTROL_REQUESTS                                          BIT(1)
#define BIT_CQDDR                                                           BIT(2)
#define BIT_SCO_LINK                                                        BIT(3)
#define BIT_HV2_PACKETS                                                     BIT(4)
#define BIT_HV3_PACKETS                                                     BIT(5)
#define BIT_u_LAW_LOG_SYNC_DATA                                             BIT(6)
#define BIT_A_LAW_LOG_SYNV_DATA                                             BIT(7)

#define SUPPORTED_FEATURES_OCTET1       \
    BIT_PARK_STATE|BIT_POWER_CONTROL_REQUESTS|BIT_CQDDR|BIT_SCO_LINK|BIT_HV2_PACKETS|BIT_HV3_PACKETS|BIT_u_LAW_LOG_SYNC_DATA|BIT_A_LAW_LOG_SYNV_DATA

#define BIT_CVSD_SYNCHRONOUS_DATA                                           BIT(0)
#define BIT_PAGING_PARAMETER_NEGOTIATION                                    BIT(1)
#define BIT_POWER_CONTROL                                                   BIT(2)
#define BIT_TRANSPARENT_SYNCHRONOUS_DATA                                    BIT(3)
#define BIT_FLOW_CONTROL_L                                                  BIT(4)
#define BIT_FLOW_CONTROL_M                                                  BIT(5)
#define BIT_FLOW_CONTROL_H                                                  BIT(6)
#define BIT_BROADCAST_ENCRYPTION                                            BIT(7)

#define SUPPORTED_FEATURES_OCTET2       \
     BIT_CVSD_SYNCHRONOUS_DATA|BIT_PAGING_PARAMETER_NEGOTIATION|BIT_POWER_CONTROL|BIT_TRANSPARENT_SYNCHRONOUS_DATA|BIT_BROADCAST_ENCRYPTION

#define BIT_ENHANCED_DATA_RATE_ACL_2MBS_MODE                                BIT(1)
#define BIT_ENHANCED_DATA_RATE_ACL_3MBS_MODE                                BIT(2)
#define BIT_ENHANCED_INQUIRY_SCAN                                           BIT(3)
#define BIT_INTERLACED_INQUIRY_SCAN                                         BIT(4)
#define BIT_INTERLACED_PAGE_SCAN                                            BIT(5)
#define BIT_RSSI_WITH_INQUIRY_RESULTS                                       BIT(6)
#define BIT_EXTENDED_SCO_LINK_EV3                                           BIT(7)

#define SUPPORTED_FEATURES_OCTET3       \
    BIT_ENHANCED_DATA_RATE_ACL_2MBS_MODE|BIT_ENHANCED_DATA_RATE_ACL_3MBS_MODE|BIT_ENHANCED_INQUIRY_SCAN|BIT_INTERLACED_INQUIRY_SCAN|BIT_INTERLACED_PAGE_SCAN|BIT_RSSI_WITH_INQUIRY_RESULTS

#define BIT_EV4_PACKETS                                                     BIT(0)
#define BIT_EV5_PACKETS                                                     BIT(1)
#define BIT_AFH_CAPABLE_SLAVE                                               BIT(3)
#define BIT_AFH_CLASSIFICATION_SLAVE                                        BIT(4)
#define BIT_BREDR_NOT_SUPPORTED                                             BIT(5)
#define BIT_LE_SUPPORTED_CONTROLLER                                         BIT(6)
#define BIT_3_SLOT_ENHANCED_DATA_RATE_ACL_PACKETS                           BIT(7)

#define SUPPORTED_FEATURES_OCTET4       \
    BIT_AFH_CAPABLE_SLAVE|BIT_AFH_CLASSIFICATION_SLAVE|BIT_LE_SUPPORTED_CONTROLLER|BIT_3_SLOT_ENHANCED_DATA_RATE_ACL_PACKETS

#define BIT_5_SLOT_ENHANCED_DATA_RATE_ACL_PACKETS                           BIT(0)
#define BIT_SNIFF_SUBRATING                                                 BIT(1)
#define BIT_PAUSE_ENCRYPTION                                                BIT(2)
#define BIT_AFH_CAPABLE_MASTER                                              BIT(3)
#define BIT_AFH_CLASSIFICATION_MASTER                                       BIT(4)
#define BIT_ENHANCED_DATA_RATE_eSCO_2MBS_MODE                               BIT(5)
#define BIT_ENHANCED_DATA_RATE_eSCO_3MBS_MODE                               BIT(6)
#define BIT_3_SLOT_ENHANCED_DATA_RATE_eSCO_PACKETS                          BIT(7)

#define SUPPORTED_FEATURES_OCTET5       \
    BIT_5_SLOT_ENHANCED_DATA_RATE_ACL_PACKETS|BIT_SNIFF_SUBRATING|BIT_PAUSE_ENCRYPTION|BIT_AFH_CAPABLE_MASTER|BIT_AFH_CLASSIFICATION_MASTER

#define BIT_EXTENDED_INQUIRY_RESPONSE                                       BIT(0)
#define BIT_SIMULTANEOUS_LE_AND_BREDR_TO_SAME_DEVICE_CAPABLE_CONTROLLER     BIT(1)
#define BIT_SECURE_SIMPLE_PAIRING                                           BIT(3)
#define BIT_ENCAPSULATED_PDU                                                BIT(4)
#define BIT_ERRONEOUS_DATA_REPORTING                                        BIT(5)
#define BIT_NON_FLUSHABLE_PACKET_BOUNDARY_FLAG                              BIT(6)

#define SUPPORTED_FEATURES_OCTET6       \
    BIT_EXTENDED_INQUIRY_RESPONSE|BIT_SIMULTANEOUS_LE_AND_BREDR_TO_SAME_DEVICE_CAPABLE_CONTROLLER|BIT_SECURE_SIMPLE_PAIRING|BIT_ENCAPSULATED_PDU|BIT_NON_FLUSHABLE_PACKET_BOUNDARY_FLAG

#define BIT_LINK_SUPERVISION_TIMEOUT_CHANGED_EVENT                          BIT(0)
#define BIT_INQUIRY_TX_POWER_LEVEL                                          BIT(1)
#define BIT_ENHANCED_POWER_CONTROL                                          BIT(2)
#define BIT_EXTENED_FEATURES                                                BIT(7)

#define SUPPORTED_FEATURES_OCTET7       \
    BIT_LINK_SUPERVISION_TIMEOUT_CHANGED_EVENT|BIT_INQUIRY_TX_POWER_LEVEL|BIT_ENHANCED_POWER_CONTROL|BIT_EXTENED_FEATURES

#define SUPPORTED_FEATURES(idx, feature)        SUPPORTED_FEATURES_OCTET##idx & BIT_##feature

/********************************************************************************/
/*
 *                  Supported Extended Features
 *                  Core_v5.0 | Vol 2 | Part C | 3.3
 */
#define BIT_SECURE_SIMPLE_PAIRING_HOST_SUPPORT                              BIT(0)
#define BIT_LE_SUPPORTED_HOST                                               BIT(1)
#define BIT_SIMULTANEOUS_LE_AND_BREDR_TO_SAME_DEVICE_CAPABLE_HOST           BIT(2)
#define BIT_SECURE_CONNECTIONS_HOST_SUPPORT                                 BIT(3)

#define SUPPORTED_EXTENDED_FEATURES_OCTET0       \
    0

#define BIT_CONNECTIONLESS_SLAVE_BROADCAST_MASTER_OPERATION                 BIT(0)
#define BIT_CONNECTIONLESS_SLAVE_BROADCAST_SLAVE_OPERATION                  BIT(1)
#define BIT_SYNCHRONIZATION_TRAIN                                           BIT(2)
#define BIT_SYNCHRONIZATION_SCAN                                            BIT(3)
#define BIT_INQUIRY_RESPONSE_NOTIFICATION_EVENT                             BIT(4)
#define BIT_GENERALIZED_INTERLACED_SCAN                                     BIT(5)
#define BIT_COARSR_CLOCK_ADJUSTMENT                                         BIT(6)

#define SUPPORTED_EXTENDED_FEATURES_OCTET1       \
    0

#define BIT_SECURE_CONNECTIONS_CONTROLLER_SUPPORT                           BIT(0)
#define BIT_PING
#define BIT_TRAIN_NUDGING                                                   BIT(3)

#define SUPPORTED_EXTENDED_FEATURES_OCTET2       \
    0

/********************************************************************************/
/*
 *                  Events
 *                  Core_v5.0 | Vol 2 | Part E | 7.7
 */
#define NO_EVENTS_SPECIFIED                                                 0x00
#define INQUIRY_COMPLETE_EVENT                                              0x01
#define INQUIRY_RESULT_EVENT                                                0x02
#define CONNECTION_COMPLETE_EVENT                                           0x03
#define CONNECTION_REQUEST_EVENT                                            0x04
#define DISCONNECTION_COMPLETE_EVENT                                        0x05
#define AUTHENTICATION_COMPLETE_EVENT                                       0x06
#define REMOTE_NAME_REQUEST_COMPLETE_EVENT                                  0x07

#define ENCRYPTION_CHANGE_EVENT                                             0x08
#define CHANGE_CONNECTION_LINK_KEY_COMPLETE_EVENT                           0x09
#define MASTER_LINK_KEY_COMPLETE_EVENT                                      0x0a
#define READ_REMOTE_SUPPORTED_FEATURES_COMPLETE_EVENT                       0x0b
#define READ_REMOTE_VERSION_INFORMATION_COMPLETE_EVENT                      0x0c
#define QOS_SETUP_COMPLETE_EVENT                                            0x0d
#define COMMAND_COMPLETE                                                    0x0e
#define COMMAND_STATUS                                                      0x0f

#define HARDWARE_ERROR_EVENT                                                0x10
#define FLUSH_OCCURRED_EVENT                                                0x11
#define ROLE_CHANGE_EVENT                                                   0x12
#define NUMBER_OF_COMPLETED_PACKETS                                         0x13
#define MODE_CHANGE_EVENT                                                   0x14
#define RETURN_LINK_KEYS_EVENT                                              0x15
#define PIN_CODE_REQUEST_EVENT                                              0x16
#define LINK_KEY_REQUEST_EVENT                                              0x17

#define LINK_KEY_NOTIFICATION_EVENT
#define LOOPBACK_COMMAND_EVENT
#define DATA_BUFFER_OVERFLOW_EVENT
#define MAX_SLOTS_CHANGE_EVENT
#define READ_CLOCK_OFFSET_COMPLETE_EVENT
#define CONNECTION_PACKET_TYPE_CHANGED_EVENT
#define QOS_VIOLATION_EVENT

#define PAGE_SCAN_REPETITION_MODE_CHANGE_EVENT
#define FLOW_SPECIFICATION_COMPLETE_EVENT
#define INQUIRY_RESULT_WITH_RSSI_EVENT
#define READ_REMOTE_EXTENDED_FEATURES_COMPLETE_EVENT

#define SYNCHRONOUS_CONNECTION_COMPLETE_EVENT
#define SYNCHRONOUS_CONNECTION_CHANGED_EVENT
#define SNIFF_SUBRATING_EVENT
#define EXTENDED_INQUIRY_RESULT_EVENT

#define ENCRYPTION_KEY_REFRESH_COMPLETE_EVENT
#define IO_CAPABILITY_REQUEST_EVENT
#define IO_CAPABILITY_REQUEST_REPLY_EVENT
#define USER_CONFIRMATION_REQUEST_EVENT
#define USER_PASSKEY_REQUEST_EVENT
#define REMOTE_OOB_DATA_REQUEST_EVENT
#define SIMPLE_PAIRING_COMPLETE_EVENT

#define LINK_SUPERVISION_TIMEOUT_CHANGED_EVENT
#define ENHANCED_FLUSH_COMPLETE_EVENT
#define USER_PASSKEY_NOTIFICATION_EVENT
#define KEYPRESS_NOTIFICATION_EVENT
#define REMOTE_HOST_SUPPORTED_FEATURES_NOTIFICATION_EVENT

#define LE_META_EVENT                                                       61

#define HCI_ACL_HEADER_SIZE     4
#define HCI_SCO_HEADER_SIZE     3

typedef int (*cmd_handler)(u16 opcode, const u8 *cmd, u32 len);


struct hci_cmd_registration {
    linked_item_t   item;
    cmd_handler link_control;
    cmd_handler link_policy;
    cmd_handler controller_baseband;
    cmd_handler informational_parameters;
    cmd_handler status_parameters;
    cmd_handler testing_commands;
    cmd_handler le_controller;
    cmd_handler vendor;
};

/********************************************************************************/
/*
 *                  HCI Command Parameter
 */
//--------------HCI READ (R attr)

#define LOCAL_NAME_LEN  					248
#define EXT_FEATURES_MAXIMUM_PAGE_NUMBER	2

struct read_remote_version_paramter {
    u16 connection_handle;
} _GNU_PACKED_;

struct disconnect_paramter {
    u16 connection_handle;
    u8 reason;
// };
} _GNU_PACKED_;

struct hci_parameter {
    u8  event_mask[8];
    u8  hci_version;
    u16 hci_revision;
    u8  lmp_pal_version;
    u16 lmp_pal_subversion;
    u16 manufacturer_name;
    u8  local_supported_commands[64];
    u8  simple_pairing_hash_c[16];
    u8  simple_pairing_randomizer_r[16];
    u8  local_name[LOCAL_NAME_LEN];

    u8 addr[6];
    u8 features[8];
    u8 ext_features[8 * EXT_FEATURES_MAXIMUM_PAGE_NUMBER];	//max 2 page

    u8  secure_conn_host_support;

    struct read_remote_version_paramter read_remote_version_param;
    struct disconnect_paramter disconn_param;
};

void hci_controller_reset(void);

extern struct hci_parameter hci_param;

void hci_public_addr_verify(void);

void __hci_send_event(int code, const char *format, ...);

void __hci_send_event_dynamic(int code, const char *format, ...);

void __hci_emit_event_of_cmd_complete(int opcode, const char *format, ...);

void __hci_emit_event_of_cmd_status(int opcode, u8 status);

void hci_controller_init(void);

void hci_add_cmd_handler(struct hci_cmd_registration *item);

void device_manager_init(void);

int le_controller_set_mac(bd_addr_t *addr);

void bt_set_cap(u8 sel_l, u8 sel_r);

void bt_set_tx_power(u8 txpower);

//Events show info
void hci_read_remote_version_information_show(const char *str, u8 version, u16 name, u16 subversion);

#endif
