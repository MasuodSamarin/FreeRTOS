#ifndef	_CLOCK_INTERFACE_H_
#define _CLOCK_INTERFACE_H_

#include "typedef.h"

//此头文件为apps 依赖接口文件，不能依赖driver.h

#define RTC_OSC_Hz     32768L
#define RC_OSC_Hz      250000L
// #define OSC_Hz         24000000L
#define SPI_MAX_CLK	   100000000L	//<-SPI最大支持频率


typedef enum {
    ///原生时钟源作系统时钟源
    SYS_CLOCK_INPUT_RC,
    SYS_CLOCK_INPUT_BT_OSC,          //BTOSC 双脚(12-26M)
    SYS_CLOCK_INPUT_RTOSCH,
    SYS_CLOCK_INPUT_RTOSCL,
    SYS_CLOCK_INPUT_PAT,

    ///衍生时钟源作系统时钟源
    SYS_CLOCK_INPUT_PLL_BT_OSC,
    SYS_CLOCK_INPUT_PLL_RTOSCH,
    SYS_CLOCK_INPUT_PLL_PAT,
} SYS_CLOCK_INPUT;

typedef enum {
    SYS_ICLOCK_INPUT_BTOSC,          //BTOSC 双脚(12-26M)
    SYS_ICLOCK_INPUT_RTOSCH,
    SYS_ICLOCK_INPUT_RTOSCL,
    SYS_ICLOCK_INPUT_PAT,
} SYS_ICLOCK_INPUT;

typedef enum {
    PB0_CLOCK_OUTPUT = 0,
    PB0_CLOCK_OUT_BT_OSC,
    PB0_CLOCK_OUT_RTOSCH,
    PB0_CLOCK_OUT_RTOSCL,

    PB0_CLOCK_OUT_LSB = 4,
    PB0_CLOCK_OUT_HSB,
    PB0_CLOCK_OUT_SFC,
    PB0_CLOCK_OUT_PLL,
} PB0_CLK_OUT;

typedef enum {
    PA2_CLOCK_OUTPUT = 0,
    PA2_CLOCK_OUT_RC,
    PA2_CLOCK_OUT_LRC,
    PA2_CLOCK_OUT_RCCL,

    PA2_CLOCK_OUT_BT_LO_D32 = 4,
    PA2_CLOCK_OUT_APC,
    PA2_CLOCK_OUT_PLL320,
    PA2_CLOCK_OUT_PLL107,
} PA2_CLK_OUT;


struct clock_driver {
    void (*on)(u8 type, u32 input_freq, u32 output_freq);
    void (*off)(u8 type);
    void (*switching)(u32 out_freq);
};

#define REGISTER_CLOCK_DRIVER(drv) \
	const struct clock_driver *__clock_drv \
			SEC_USED(.text) = &drv

extern const struct clock_driver *__clock_drv;

//[internal]
u32 clock_get_usb_freq(void);

u32 clock_get_dac_freq(void);

u32 clock_get_apc_freq(void);

u32 clock_get_uart_freq(void);

u32 clock_get_bt_freq(void);

u32 clock_get_sfc_freq(void);

u32 clock_get_hsb_freq(void);

u32 clock_get_lsb_freq(void);

u32 clock_get_otp_freq(void);

u32 clock_get_iosc_freq(void);

u32 clock_get_osc_freq(void);

void clock_dump(void);

void clock_debug(void);

#define USB_CLK         clock_get_usb_freq()
#define DAC_CLK         clock_get_dac_freq()
#define APC_CLK         clock_get_apc_freq()
#define UART_CLK        clock_get_uart_freq()
#define BT_CLK          clock_get_bt_freq()


#define SFC_CLK         clock_get_sfc_freq()
#define SYS_CLK         clock_get_hsb_freq()
#define LSB_CLK         clock_get_lsb_freq()
#define OTP_CLK         clock_get_otp_freq()

#define IOSC_CLK        clock_get_iosc_freq()
#define OSC_CLK         clock_get_osc_freq()


#endif

