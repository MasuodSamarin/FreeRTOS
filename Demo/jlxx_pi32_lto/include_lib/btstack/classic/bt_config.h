#ifndef BT_CONFIG_H
#define BT_CONFIG_H

//#include "config.h"
#include "typedef.h"
#include "uart.h"


//tmp_def
#define AT_POWER        AT(.poweroff_text)


//tmp_def
#define S_DEVICE_ROLE_SLAVE           BIT(0)//手机连接设备，设备做从
#define S_DEVICE_ROLE_SLAVE_TO_MASTER BIT(1)//手机连接设备，设备从变主

#define M_DEVICE_ROLE_MASTER          BIT(2)//设备回连手机，设备做主
#define M_DEVICE_ROLE_MASTER_TO_SLAVE BIT(3)//设备回连手机，设备主变从

#define DEVICE_ROLE	(S_DEVICE_ROLE_SLAVE | M_DEVICE_ROLE_MASTER_TO_SLAVE)

//配置Low power mode
#define SNIFF_EN                            BIT(0)  ///<SNIFF使能
#define SNIFF_TOW_CONN_ENTER_POWERDOWN_EN   BIT(3)  ///<SNIFF 等待两台都连接才进powerdown
#define SNIFF_CNT_TIME                      10      ///<空闲10S之后进入sniff模式

//tmp_def
#define NORMAL_MODE         0
#define TEST_BQB_MODE       1       ///<测试bqb认证
#define TEST_FCC_MODE       2       ///<测试fcc认证
#define TEST_FRE_OFF_MODE   3       ///<测试频偏(使用频谱分析仪-手提测试仪-中心频率默认2422M)
#define TEST_PERFOR_MODE    4       ///<指标性能测试(使用MT8852A仪器测试,测试芯片性能的时候使用)
#define TEST_BOX_MODE       5       ///<测试盒测试

#define SNIFF_T_SLOT         800
#define SNIFF_WAKEUP_SLOT    10
#define SNIFF_ATTEMPT_SLOT   12
#define SNIFF_IN_SLOT        80  //在可以进入power down的状态前延时80slot,预留时间给page_scan

//tmp_def
typedef enum {
    ACTIVE_MODE = 0,
    HOLD_MODE,
    SNIFF_MODE,
    PACK_STATE,
} BLUETOOTH_MODE;


#define  COST_DOWN_TOTAL_TIME  2048

/******************************sniff mode cfg*********************************************/
//abab
//abba
// #define T_SNIFF        800//1024//  1024\640ms   2048\72 1.28s\45ms     800\36  640ms\25ms
// #define WAKEUP_SLOT    10
// #define ATTEMPT_SNIFF   12
// #define IN_SLOT  80  //在可以进入power down的状态前延时80slot,预留时间给page_scan
// #define ONE_CONN_IN_SLOT  4  //预留时间给page_scan
#define OUT_SLOT 4  //提早4slot 退出power down的状态
// #define SNIFF_AB_TYPE  (COST_DOWN_TOTAL_TIME/T_SNIFF)

/****************************no connection ********************************************/
//abab
//abba
#define PAGE_SCAN_SLOT   		32
#define INQUIRY_SCAN_SLOT   	32
#define SNIFF_PAGE_SCAN   		(PAGE_SCAN_SLOT)
#define ACTIVE_SLOT 			(PAGE_SCAN_SLOT+INQUIRY_SCAN_SLOT)
#define SLEEP_SLOT  			(COST_DOWN_TOTAL_TIME-ACTIVE_SLOT)
#define PWR_AB_TYPE  			(COST_DOWN_TOTAL_TIME/(ACTIVE_SLOT+SLEEP_SLOT))


/**************************one conn\page scan\inquiry scan normal**************************/
#define CONN_PS_SLOT      		2000//900
#define PAGE_SCAN_CONN    		32

#define PAGE_SCAN_NOR			32
#define INQUIRY_SCAN_NOR		32
#define PAGE_SLOT_NOR			84
#define INQUIRY_SLOT_NOR		84

/****************************page_conn *************************************/
#define PAGE_SLOT_CONN    120
#define CONN_PAGE_SLOT    120




///---sdp service record profile---///
#define SUPPORT_PROFILE_SPP    1
#define SUPPORT_PROFILE_HSP    0
#define SUPPORT_PROFILE_HFP    1
#define SUPPORT_PROFILE_A2DP   1
#define SUPPORT_PROFILE_AVCTP  1
#define SUPPORT_PROFILE_HID    1

///---rfcomm channel---///
#define RFCOMM_CH_SPP   2
#define RFCOMM_CH_HSP   3
#define RFCOMM_CH_HFP   4
#define RFCOMM_CH_HFP_AG    6


///---A2DP---///
#define A2DP_SOURCE 0
#define A2DP_SINK   1
#define A2DP_ROLE   A2DP_SINK


// #define HAVE_TICK       1

#define BREDR_DEVICE   2
#define SUPPORT_CONN_NUM BREDR_DEVICE

// #define HCI_ACL_PAYLOAD_SIZE 0x0300


#define MAX_SPP_CONNECTIONS 2


// #define MAX_NO_HCI_CONNECTIONS 2*BREDR_DEVICE  //2 for acl, 1 for sco
// #define MAX_NO_L2CAP_SERVICES  7*BREDR_DEVICE //avdtp, avctp, hfp, rfcomm,sdp,hid,a2dp
// #define MAX_NO_L2CAP_CHANNELS  7*BREDR_DEVICE
// #define MAX_NO_RFCOMM_MULTIPLEXERS 1*BREDR_DEVICE //MAX_SPP_CONNECTIONS
// #define MAX_NO_RFCOMM_SERVICES 3*BREDR_DEVICE
// #define MAX_NO_RFCOMM_CHANNELS  3*BREDR_DEVICE //MAX_SPP_CONNECTIONS*2
// #define MAX_NO_DB_MEM_DEVICE_LINK_KEYS  0
// #define MAX_NO_DB_MEM_DEVICE_NAMES 0
// #define MAX_NO_DB_MEM_SERVICES 0

#define BTSTATE_CHANNEL_DISCONNECTION     0
#define BTSTATE_CHANNEL_CONNECTION        1

////----蓝牙状态:LED显示----////
#define  BT_STA_POWER_ON      		0x00    //上电
#define  BT_STA_NO_CONNECTION  		0x01    //无连接,初始化已完成,等待连接
#define  BT_STA_CONNECTED    		0x02    //已连接
#define  BT_STA_INCOMING_CALLING	0x03    //电话呼入
#define  BT_STA_OUTGOING_CALLING	0x04    //电话呼出
#define  BT_STA_CALL_ACTIVE  		0x05    //通话中
#define  BT_STA_BATTERY_LOW  		0x06    //电池电量低


////----协议支持定义----////
#define BTCFG_SUPPORT_PROFILE_SPP     0x01
#define BTCFG_SUPPORT_PROFILE_HFP     0x02
#define BTCFG_SUPPORT_PROFILE_A2DP    0x04
#define BTCFG_SUPPORT_PROFILE_AVRCP   0x08
#define BTCFG_SUPPORT_PROFILE_HID     0x10
////连接通道成功的标志要跟支持协议的标志bit一致
///注意：有相应的判断会涉及这两个定义的位置。
#define    SPP_CH       0x01
#define    HFP_CH       0x02
#define    A2DP_CH      0x04    //media
#define    AVCTP_CH     0x08
#define    HID_CH       0x10
#define    AVDTP_CH     0x20
#define    HFP_AG_CH    0x80

#define  ONE_CONN_TIME  			128
#define  TWO_CONN_TIME  			28
#define  ONE_IN_SNIFF_TIME 			20
#define  TWO_CONN_MUSIC_TIME  		128
#define  TWO_CONN_STEREO_TIME 		100
#define  TWO_CONN_GET_TIME 			80

#define  TWO_CONN_THE_ADD_TIME 10

struct bt_sleep {
    void (*suspend)();
    void (*resume)();
    void (*release)();
};

#define BT_SLEEP_REGISTER(suspend, resume,release) \
	const struct bt_sleep bt_##suspend_##resume##release AT(.bt_sleep)= { \
		suspend, resume, release \
	};

typedef struct _local_addr {
    u16 crc;
    u8 addr[6];
} local_addr_t;


#endif
