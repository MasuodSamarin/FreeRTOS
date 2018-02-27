/*********************************************************************************************
    *   Filename        : sdk_cfg.h

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2017-02-03 15:28

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/
#ifndef _SDK_CONFIG_
#define _SDK_CONFIG_

#include "cpu.h"


/********************************************************************************/
/*
*          --------调试类配置
 */
///< 0：关闭串口调试打印
///< 1：打开串口调试打印
//
///<配置串口打印调试功能：0/1
#define APP_UART_DEBUG      1

///< UART0：使用串口0出口
#define UART0   0
///< UART1：使用串口1出口
#define UART1   1
///< UART2：使用串口2出口
#define UART2   2


#if (APP_UART_DEBUG == 1)

///<配置串口打能出口,可选配置：UART0/UART1/UART2
#define DEBUG_UART   UART0

#if (DEBUG_UART == UART0)
#define UART_TXPA5_RXPA6
/* #define UART_TXPB6_RXPB7 */
//#define UART_TXPC2_RXPC3
//#define UART_TXPA14_RXPA15
#endif

#if (DEBUG_UART == UART1)
#define UART_TXPB0_RXPB1
//#define UART_TXPC0_RXPC1
//#define UART_TXPA1_RXPA2
//#define UART_USBP_USBM
#endif

#if (DEBUG_UART == UART2)
//#define UART_TXPA3_RXPA4
#define UART_TXPA9_RXPA10
//#define UART_TXPB9_RXPB10
//#define UART_TXPC4_RXPC5
#endif
#endif


///<串口串口波特率
#define APP_UART_BAUD       460800L

/********************************************************************************/
/*
 *           --------时钟类配置
 */
#include "clock_interface.h"

///<配置系统时钟源，可选配置：(详见clock_interface.h)
#define APP_CLOCK_IN        SYS_CLOCK_INPUT_PLL_BT_OSC

///<配置晶振频率，根据晶振实际频率
#define APP_OSC_Hz          24000000L

///<配置系统频率
#define APP_SYS_Hz          48000000L

#include "clock_api.h"

///<配置系统外设时钟源
#define APP_UART_CLK        24000000L
#define APP_SYS_CLK         SYS_CLK
#define APP_TIMER_CLK       24000000L
#define APP_ADC_CLK         SYS_CLK

/********************************************************************************/
/*
 *           --------电源类配置
 */
#include "power_api.h"

///<配置系统电源,可选配置：PWR_NO_CHANGE/PWR_LDO33/PWR_LDO15/PWR_DCDC15
#define APP_SYS_POWER_SELECT		PWR_LDO15

/********************************************************************************/
/*
 *           --------蓝牙射频类配置
 */
#include "btstack_defines.h"

///<配置蓝牙射频发射功率,可选配置：0~15
#define APP_BT_RF_TXPOWER               0

///<配置蓝牙频偏校准电容(L/R),推荐配置：0x10/0x11/0x12
#define APP_BT_INTERNAL_CAP             0x11, 0x11

///<配置蓝牙MAC地址,6 bytes
#define APP_BT_MAC_ADDR                 0x20, 0x22, 0x33, 0x44, 0x55, 0x66
/********************************************************************************/
/*
 *           --------系统低功耗设置
 */
///<   ______                  ______
///<___|    |__________________|    |___________
//      9mA        300uA
//      3ms         17ms
///<BLE 功耗估算公式：(3/20*9mA + 17/20*0.3mA) ~= 1.6mA/h
//
///配置低功耗模式,可选配置：0/SLEEP_EN/DEEP_SLEEP_EN
#define APP_LOWPOWER_BTOSC_DISABLE       0
///<  0:  不使用低功耗模式
///<  BT_SLEEP_EN:  使用睡眠模式(低功耗)
///<  BT_DEEP_SLEEP_EN:  使用深睡眠模式(极低功耗)
//
///配置低功耗模式,可选配置：0/SLEEP_EN/DEEP_SLEEP_EN
#define APP_LOWPOWER_SELECT     0//SLEEP_EN

///<  BT_OSC:  使用蓝牙晶振作为低功耗运行时钟
///<  RTC_OSCH:  使用低速RTC晶振作为低功耗运行时钟
///<  RTC_OSCL:  使用高速蓝牙晶振作为低功耗运行时钟

///<配置低功耗模式使用的晶振,可选配置：BT_OSC/RTC_OSCH/RTC_OSCL
#define APP_LOWPOWER_OSC_TYPE   BT_OSC//
#if (APP_LOWPOWER_SELECT == DEEP_SLEEP_EN) && (APP_LOWPOWER_OSC_TYPE != RTC_OSCL)
#error "error setting : deepsleep must use RTC_OSCL"
#endif

///<配置晶振频率,单位赫兹,可选配置：32768L//24000000L
#define APP_LOWPOWER_OSC_HZ    24000000L
#if (APP_LOWPOWER_SELECT == DEEP_SLEEP_EN) && (APP_LOWPOWER_OSC_HZ != 32768)
#error "error setting : deepsleep must use 32768"
#endif

///<配置系统定时器运行节拍,单位毫秒,可选配置：10~1000
#define APP_TICKS_UNIT                  10

#endif

