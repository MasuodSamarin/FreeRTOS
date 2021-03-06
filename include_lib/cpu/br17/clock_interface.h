#ifndef	_CLOCK_INTERFACE_H_
#define _CLOCK_INTERFACE_H_

#include "typedef.h"

//此头文件为apps 依赖接口文件，不能依赖driver.h

#define RTC_OSC_Hz     32768L
#define RC_OSC_Hz      250000L
// #define OSC_Hz         24000000L
#define SPI_MAX_CLK	   100000000L	//<-SPI最大支持频率

#define RFI_Hz		   80000000L	//<-固定频率
#define APC_Hz		   96000000L	//<-根据运算能力可调整
#define LSB_Hz         80000000L

// #define SYS_Hz          96000000L

typedef enum {
    ///原生时钟源作系统时钟源
    SYS_CLOCK_INPUT_RTOSC_32K1P = 0, //RTOSC 单脚(32K)
    SYS_CLOCK_INPUT_RTOSC_32K2P,     //RTOSC 双脚(32K)
    SYS_CLOCK_INPUT_RTOSC_24M2P,     //RTOSC 双脚(12-26M)
    SYS_CLOCK_INPUT_BT_OSC,          //BTOSC 双脚(12-26M)
    SYS_CLOCK_INPUT_RC,
    SYS_CLOCK_INPUT_PAT,

    ///衍生时钟源作系统时钟源
    SYS_CLOCK_INPUT_PLL_32K1P,
    SYS_CLOCK_INPUT_PLL_32K2P,
    SYS_CLOCK_INPUT_PLL_24M2P,
    SYS_CLOCK_INPUT_PLL_BT_OSC,
    SYS_CLOCK_INPUT_PLL_PAT,
} SYS_CLOCK_INPUT;

typedef enum {
    PB0_CLOCK_OUTPUT = 0,
    PB0_CLOCK_OUT_LSB,
    PB0_CLOCK_OUT_BT_OSC,
    PB0_CLOCK_OUT_RTOSC_H,

    PB0_CLOCK_OUT_RC = 4,
    PB0_CLOCK_OUT_HSB,
    PB0_CLOCK_OUT_RTOSC_L,
    PB0_CLOCK_OUT_PLL_SYS,
} PB0_CLK_OUT;

typedef enum {
    PB9_CLOCK_OUTPUT = 0,
    PB9_CLOCK_OUT_FM_LO_D2,			//<-由射频模块产生
    PB9_CLOCK_OUT_PLL_RFI,
    PB9_CLOCK_OUT_PLL_192M,

    PB9_CLOCK_OUT_BT_LO_D32 = 4,	//<-由射频模块产生
    PB9_CLOCK_OUT_WL,
    PB9_CLOCK_OUT_APC,
    PB9_CLOCK_OUT_RCCL,
} PB9_CLK_OUT;


struct clock_driver {
    void (*on)(u8 type, u32 input_freq, u32 output_freq);
    void (*off)(u8 type);

    void (*switching)(u32 freq);
};

#define REGISTER_CLOCK_DRIVER(drv) \
	const struct clock_driver *__clock_drv \
			SEC_USED(.text) = &drv

extern const struct clock_driver *__clock_drv;

//[internal]
u32 clock_get_apc_freq(void);

u32 clock_get_sfc_freq(void);

u32 clock_get_hsb_freq(void);

u32 clock_get_lsb_freq(void);

void clock_dump(void);

#define USB_CLK         NULL
#define DAC_CLK         NULL
#define APC_CLK         NULL
#define UART_CLK        NULL
#define BT_CLK          NULL


#define SFC_CLK         clock_get_sfc_freq()
#define SYS_CLK         clock_get_hsb_freq()
#define LSB_CLK         clock_get_lsb_freq()

#define IOSC_CLK        NULL
#define OSC_CLK         NULL

#endif

