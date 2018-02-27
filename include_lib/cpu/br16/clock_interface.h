#ifndef	_CLOCK_INTERFACE_H_
#define _CLOCK_INTERFACE_H_

#include "typedef.h"

//此头文件为apps 依赖接口文件，不能依赖driver.h

#define RTC_OSC_Hz      32768L
#define RC_OSC_Hz       250000L
#define OSC_Hz          12000000L

#define SYS_Hz          96000000L
#define FM_SYS_Hz       38400000L       //38.4M
#define BT_SYS_Hz  	    120000000L
#define FLAC_SYS_Hz     120000000L
#define BT_RCCL_SYS_Hz  24000000L


#define FM_Hz		    96000000L	//<-固定频率
#define RFI_Hz		    80000000L	//<-固定频率
#define APC_Hz		    96000000L	//<-根据运算能力可调整
#define LSB_Hz          80000000L

typedef enum {
    ///原生时钟源作系统时钟源
    SYS_CLOCK_INPUT_RTOSC_32K1P = 0, //RTOSC 单脚(32K)
    SYS_CLOCK_INPUT_RTOSC_32K2P,     //RTOSC 双脚(32K)
    SYS_CLOCK_INPUT_RTOSC_24M2P,     //RTOSC 双脚(12-26M)
    SYS_CLOCK_INPUT_BT_OSC,          //BTOSC 双脚(12-26M)
    SYS_CLOCK_INPUT_RC,

    ///衍生时钟源作系统时钟源
    SYS_CLOCK_INPUT_PLL_32K1P,
    SYS_CLOCK_INPUT_PLL_32K2P,
    SYS_CLOCK_INPUT_PLL_24M2P,
    SYS_CLOCK_INPUT_PLL_BT_OSC,
    SYS_CLOCK_INPUT_PLL_RC,
    SYS_CLOCK_INPUT_PLL_PAT,
} SYS_CLOCK_INPUT;

#define SYS_CLOCK_IN    SYS_CLOCK_INPUT_PLL_BT_OSC

typedef enum {
    PA4_CLOCK_OUTPUT = 0,
    PA4_CLOCK_OUT_LSB,
    PA4_CLOCK_OUT_BT_OSC,
    PA4_CLOCK_OUT_RTC_OSC,

    PA4_CLOCK_OUT_RC = 4,
    PA4_CLOCK_OUT_HSB,
    PA4_CLOCK_OUT_RC1,
    PA4_CLOCK_OUT_PLL_SYS,
} PA4_CLK_OUT;

typedef enum {
    PA5_CLOCK_OUTPUT = 0,
    PA5_CLOCK_OUT_FM_LO_D2,			//<-由射频模块产生
    PA5_CLOCK_OUT_PLL_RFI,
    PA5_CLOCK_OUT_PLL_FM,

    PA5_CLOCK_OUT_BT_LO_D32 = 4,	//<-由射频模块产生
    PA5_CLOCK_OUT_PLL_MDM,
    PA5_CLOCK_OUT_RESERVED,
    PA5_CLOCK_OUT_RCCL,
} PA5_CLK_OUT;

struct clock_driver {
    void (*on)(u8 type, u32 input_freq, u32 output_freq);
    void (*off)(u8 type);

    void (*set_sys_freq)(u32 freq);
    u32(*get_sys_freq)();

    void (*set_lsb_freq)(u32 freq);
    u32(*get_lsb_freq)();

    void (*set_alnk_freq)(u32 freq);
    u32(*get_alnk_freq)();

    void (*set_rfi_freq)(u32 freq);
    u32(*get_rfi_freq)();

    void (*set_apc_freq)(u32 freq);
    u32(*get_apc_freq)();
};

#define REGISTER_CLOCK_DRIVER(drv) \
	const struct clock_driver *__clock_drv \
			sec(.text) = &drv

extern const struct clock_driver *__clock_drv;



#endif

