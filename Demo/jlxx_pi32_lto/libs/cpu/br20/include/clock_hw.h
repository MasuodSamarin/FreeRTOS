#ifndef __CLOCK_HW_H__
#define __CLOCK_HW_H__

#include "typedef.h"

// #define CLOCK_HAL_DEBUG

#ifdef CLOCK_HAL_DEBUG

#define RC_EN(x)
#define TEST_SEL(x)
#define OSC_CLOCK_IN(x)
//for MACRO - OSC_CLOCK_IN
enum {
    OSC_CLOCK_IN_BT_OSC = 0,
    OSC_CLOCK_IN_RTOSC_H,
    OSC_CLOCK_IN_RTOSC_L,
    OSC_CLOCK_IN_PAT,
};

#define MAIN_CLOCK_SEL(x)
//for MACRO - CLOCK_IN
enum {
    MAIN_CLOCK_IN_RC = 0,

    MAIN_CLOCK_IN_BTOSC = 4,
    MAIN_CLOCK_IN_RTOSC_H,
    MAIN_CLOCK_IN_RTOSC_L,
    MAIN_CLOCK_IN_PLL,

    MAIN_CLOCK_IN_PAT, //for tes
};

#define SFR_MODE(x)
enum {
    SFR_CLOCK_IDLE = 0,
    SFR_CLOCK_ALWAYS_ON,
};
#define PB0_CLOCK_OUT(x)
#define PA2_CLOCK_OUT(x)


#define USB_CLOCK_IN(x)
//for MACRO - USB_CLOCK_IN
enum {
    USB_CLOCK_IN_PLL48M = 0,
    USB_CLOCK_IN_DISABLE,
    USB_CLOCK_IN_LSB,
    USB_CLOCK_IN_DISABLE_PAD,
};
#define DAC_CLOCK_IN(x)
//for MACRO - DAC_CLOCK_IN
enum {
    DAC_CLOCK_IN_PLL24M = 0,
    DAC_CLOCK_IN_DISABLE,
    DAC_CLOCK_IN_LSB,
    DAC_CLOCK_IN_DISABLE_PAD,
};
#define APC_CLOCK_IN(x)
//for MACRO - APC_CLOCK_IN
enum {
    APC_CLOCK_IN_PLL64M = 0,
    APC_CLOCK_IN_PLLAPC,
    APC_CLOCK_IN_LSB,
    APC_CLOCK_IN_DISABLE,
};
#define UART_CLOCK_IN(x)
//for MACRO - UART_CLOCK_IN
enum {
    UART_CLOCK_IN_OSC = 0,
    UART_CLOCK_IN_PLL48M,
    UART_CLOCK_IN_LSB,
    UART_CLOCK_IN_DISABLE,
};
#define BT_CLOCK_IN(x)
//for MACRO - BT_CLOCK_IN
enum {
    BT_CLOCK_IN_PLL64M = 0,
    BT_CLOCK_IN_DISABLE,
    BT_CLOCK_IN_LSB,
    BT_CLOCK_IN_DISABLE_PAD,
};

#define MEM_CLOCK_STATUS(x)
//for MACRO - MEM_CLOCK_STATUS
enum {
    MEM_CLOCK_IDLE = 0,
    MEM_CLOCK_ALWAYS_ON,
};

#define OTP_CLOCK_ENABLE(x)
//for MACRO - OTP_CLOCK_ENABLE
enum {
    OTP_CLOCK_ENABLE = 0,
    OTP_CLOCK_DISABLE,
};

#define SFC_CLOCK_DELAY(x)


#define PLL_SYS_SEL(x)
//for MACRO - PLL_SYS_SEL
enum {
    PLL_SYS_SEL_PLL192M = 0,
    PLL_SYS_SEL_PLL137M,
    PLL_SYS_SEL_PLL480M,
    PLL_SYS_SEL_DISABLE,
};
#define PLL_SYS_DIV(x)
//for MACRO - PLL_SYS_DIV
enum {
    PLL_SYS_DIV1 = 0,
    PLL_SYS_DIV3,
    PLL_SYS_DIV5,
    PLL_SYS_DIV7,

    PLL_SYS_DIV1X2 = 4,
    PLL_SYS_DIV3X2,
    PLL_SYS_DIV5X2,
    PLL_SYS_DIV7X2,

    PLL_SYS_DIV1X4 = 8,
    PLL_SYS_DIV3X4,
    PLL_SYS_DIV5X4,
    PLL_SYS_DIV7X4,

    PLL_SYS_DIV1X8 = 12,
    PLL_SYS_DIV3X8,
    PLL_SYS_DIV5X8,
    PLL_SYS_DIV7X8,
};

#define PLL_APC_SEL(x)
//for MACRO - PLL_APC_SEL
enum {
    PLL_APC_SEL_PLL192M = 0,
    PLL_APC_SEL_PLL137M,
    PLL_APC_SEL_DISABLE,
};
#define PLL_APC_DIV(x)
//for MACRO - PLL_APC_DIV
enum {
    PLL_APC_DIV1 = 0,
    PLL_APC_DIV3,
    PLL_APC_DIV5,
    PLL_APC_DIV7,

    PLL_APC_DIV1X2 = 4,
    PLL_APC_DIV3X2,
    PLL_APC_DIV5X2,
    PLL_APC_DIV7X2,

    PLL_APC_DIV1X4 = 8,
    PLL_APC_DIV3X4,
    PLL_APC_DIV5X4,
    PLL_APC_DIV7X4,

    PLL_APC_DIV1X8 = 12,
    PLL_APC_DIV3X8,
    PLL_APC_DIV5X8,
    PLL_APC_DIV7X8,
};

#define PLL_ALNK_SEL(x)
//for MACRO - PLL_ALNK_SEL
enum {
    PLL_ALNK_192M_DIV17 = 0,
    PLL_ALNK_480M_DIV39,
};

#define PLL_EN(x)
#define PLL_REST(x)
#define PLL_DIVn(x)
#define PLL_DIVn_EN(x)
#define PLL_REF_SEL(x)
//for MACRO - PLL_RSEL
enum {
    PLL_REF_SEL_BTOSC = 0, 	//bt
    PLL_REF_SEL_RTOSC,		//rt
    PLL_REF_SEL_PAT = 3,
};

#define PLL_TEST(x)
#define PLL_DSMS(x)
#define PLL_DSM_TSEL(x)
#define PLL_DSM_RSEL(x)
#define PLL_DSM_MSEL(x)

#define PLL_DIVSEL(x)

//for MACRO - PLL_RSEL
enum {
    PLL_DIVIDER_INTE = 0,
    PLL_DIVIDER_FRAC,
};

#define PLL_PFD(x)
#define PLL_ICP(x)
#define PLL_LPFR2(x)
#define PLL_LPFR3(x)


#define PLL_RSEL(x)
//for MACRO - PLL_RSEL
enum {
    PLL_RSEL_BTOSC_DIFF = 0,
    PLL_RSEL_RTOSC_DIFF,
    PLL_RSEL_PLL_REF_SEL,
};

#define PLL_DIVm(x)
#define PLL_LD012A(x)
#define PLL_LDO12D(x)
#define PLL_IVCO(x)
#define PLL_LDO_BYPASS(x)
#define PLL_TSSEL(x)
#define PLL_TSOE(x)

#define PLL_CLK480M_OE(x)
#define PLL_CLK320M_OE(x)
#define PLL_CLK192M_OE(x)
#define PLL_CLK137M_OE(x)
#define PLL_CLK107M_OE(x)

#define PLL_FRAC(x)
#define PLL_DMAX(x)
#define PLL_DMIN(x)
#define PLL_DSTP(x)

#define HSB_CLK_DIV(x)
#define LSB_CLK_DIV(x)
#define OTP_CLK_DIV(x)


#else

#define RC_EN(x)                SFR(JL_CLOCK->CLK_CON0,  0,  1,  x)
#define TEST_SEL(x)				SFR(JL_CLOCK->CLK_CON0,  2,  1,  x)
#define OSC_CLOCK_IN(x)         SFR(JL_CLOCK->CLK_CON0,  4,  2,  x)
//for MACRO - OSC_CLOCK_IN
enum {
    OSC_CLOCK_IN_BT_OSC = 0,
    OSC_CLOCK_IN_RTOSC_H,
    OSC_CLOCK_IN_RTOSC_L,
    OSC_CLOCK_IN_PAT,
};

#define MAIN_CLOCK_SEL(x)	    SFR(JL_CLOCK->CLK_CON0,  6,  3,  x)
//for MACRO - CLOCK_IN
enum {
    MAIN_CLOCK_IN_RC = 0,

    MAIN_CLOCK_IN_BTOSC = 4,
    MAIN_CLOCK_IN_RTOSC_H,
    MAIN_CLOCK_IN_RTOSC_L,
    MAIN_CLOCK_IN_PLL,

    MAIN_CLOCK_IN_PAT, //for test mode
};

#define SFR_MODE(x)             SFR(JL_CLOCK->CLK_CON0,  9,  1,  x)
enum {
    SFR_CLOCK_IDLE = 0,
    SFR_CLOCK_ALWAYS_ON,
};
#define PB0_CLOCK_OUT(x)        SFR(JL_CLOCK->CLK_CON0,  10,  3,  x)
#define PA2_CLOCK_OUT(x)     	SFR(JL_CLOCK->CLK_CON0,  13,  3,  x)


#define USB_CLOCK_IN(x)         SFR(JL_CLOCK->CLK_CON1,  0,  2,  x)
//for MACRO - USB_CLOCK_IN
enum {
    USB_CLOCK_IN_PLL48M = 0,
    USB_CLOCK_IN_DISABLE,
    USB_CLOCK_IN_LSB,
    USB_CLOCK_IN_DISABLE_PAD,
};
#define DAC_CLOCK_IN(x)         SFR(JL_CLOCK->CLK_CON1,  2,  2,  x)
//for MACRO - DAC_CLOCK_IN
enum {
    DAC_CLOCK_IN_PLL24M = 0,
    DAC_CLOCK_IN_DISABLE,
    DAC_CLOCK_IN_LSB,
    DAC_CLOCK_IN_DISABLE_PAD,
};
#define APC_CLOCK_IN(x)        	SFR(JL_CLOCK->CLK_CON1,  6,  2,  x)
//for MACRO - APC_CLOCK_IN
enum {
    APC_CLOCK_IN_PLL64M = 0,
    APC_CLOCK_IN_PLLAPC,
    APC_CLOCK_IN_LSB,
    APC_CLOCK_IN_DISABLE,
};
#define UART_CLOCK_IN(x)        SFR(JL_CLOCK->CLK_CON1,  10,  2,  x)
//for MACRO - UART_CLOCK_IN
enum {
    UART_CLOCK_IN_OSC = 0,
    UART_CLOCK_IN_PLL48M,
    UART_CLOCK_IN_LSB,
    UART_CLOCK_IN_DISABLE,
};
#define BT_CLOCK_IN(x)          SFR(JL_CLOCK->CLK_CON1,  14,  2,  x)
//for MACRO - BT_CLOCK_IN
enum {
    BT_CLOCK_IN_PLL64M = 0,
    BT_CLOCK_IN_DISABLE,
    BT_CLOCK_IN_LSB,
    BT_CLOCK_IN_DISABLE_PAD,
};

#define MEM_CLOCK_STATUS(x)     SFR(JL_CLOCK->CLK_CON1,  16,  1,  x)
//for MACRO - MEM_CLOCK_STATUS
enum {
    MEM_CLOCK_IDLE = 0,
    MEM_CLOCK_ALWAYS_ON,
};

#define OTP_CLOCK_ENABLE(x)     SFR(JL_CLOCK->CLK_CON1,  19,  1,  x)
//for MACRO - OTP_CLOCK_ENABLE
enum {
    OTP_CLOCK_ENABLE = 0,
    OTP_CLOCK_DISABLE,
};

#define SFC_CLOCK_DELAY(x)      SFR(JL_CLOCK->CLK_CON1,  28,  2,  x)


#define PLL_SYS_SEL(x)          SFR(JL_CLOCK->CLK_CON2,  0,  2,  x)
//for MACRO - PLL_SYS_SEL
enum {
    PLL_SYS_SEL_PLL192M = 0,
    PLL_SYS_SEL_PLL137M,
    PLL_SYS_SEL_PLL480M,
    PLL_SYS_SEL_DISABLE,
};
#define PLL_SYS_DIV(x)          SFR(JL_CLOCK->CLK_CON2,  2,  4,  x)
//for MACRO - PLL_SYS_DIV
enum {
    PLL_SYS_DIV1 = 0,
    PLL_SYS_DIV3,
    PLL_SYS_DIV5,
    PLL_SYS_DIV7,

    PLL_SYS_DIV1X2 = 4,
    PLL_SYS_DIV3X2,
    PLL_SYS_DIV5X2,
    PLL_SYS_DIV7X2,

    PLL_SYS_DIV1X4 = 8,
    PLL_SYS_DIV3X4,
    PLL_SYS_DIV5X4,
    PLL_SYS_DIV7X4,

    PLL_SYS_DIV1X8 = 12,
    PLL_SYS_DIV3X8,
    PLL_SYS_DIV5X8,
    PLL_SYS_DIV7X8,
};

#define PLL_APC_SEL(x)	        SFR(JL_CLOCK->CLK_CON2,  18,  2,  x)
//for MACRO - PLL_APC_SEL
enum {
    PLL_APC_SEL_PLL192M = 0,
    PLL_APC_SEL_PLL137M,
    PLL_APC_SEL_DISABLE,
};
#define PLL_APC_DIV(x)	        SFR(JL_CLOCK->CLK_CON2,  20,  4,  x)
//for MACRO - PLL_APC_DIV
enum {
    PLL_APC_DIV1 = 0,
    PLL_APC_DIV3,
    PLL_APC_DIV5,
    PLL_APC_DIV7,

    PLL_APC_DIV1X2 = 4,
    PLL_APC_DIV3X2,
    PLL_APC_DIV5X2,
    PLL_APC_DIV7X2,

    PLL_APC_DIV1X4 = 8,
    PLL_APC_DIV3X4,
    PLL_APC_DIV5X4,
    PLL_APC_DIV7X4,

    PLL_APC_DIV1X8 = 12,
    PLL_APC_DIV3X8,
    PLL_APC_DIV5X8,
    PLL_APC_DIV7X8,
};

#define PLL_ALNK_SEL(x)         SFR(JL_CLOCK->CLK_CON2,  30,  1,  x)
//for MACRO - PLL_ALNK_SEL
enum {
    PLL_ALNK_192M_DIV17 = 0,
    PLL_ALNK_480M_DIV39,
};

#define PLL_EN(x)         		SFR(JL_CLOCK->PLL_CON,  0,  1,  x)
#define PLL_REST(x)             SFR(JL_CLOCK->PLL_CON,  1,  1,  x)
#define PLL_DIVn(x)          	SFR(JL_CLOCK->PLL_CON,  2,  5,  x)
#define PLL_DIVn_EN(x)        	SFR(JL_CLOCK->PLL_CON,  7,  1,  x)
#define PLL_REF_SEL(x)         	SFR(JL_CLOCK->PLL_CON,  8,  2,  x)
//for MACRO - PLL_RSEL
enum {
    PLL_REF_SEL_BTOSC = 0, 	//btosc
    PLL_REF_SEL_RTOSC,		//rtosc
    PLL_REF_SEL_PAT = 3,
};

#define PLL_TEST(x)         	SFR(JL_CLOCK->PLL_CON,  10, 1,  x)
#define PLL_DSMS(x)         	SFR(JL_CLOCK->PLL_CON,  11, 1,  x)
#define PLL_DSM_TSEL(x)         SFR(JL_CLOCK->PLL_CON,  12, 4,  x)
#define PLL_DSM_RSEL(x)         SFR(JL_CLOCK->PLL_CON,  16, 2,  x)
#define PLL_DSM_MSEL(x)         SFR(JL_CLOCK->PLL_CON,  18, 2,  x)

#define PLL_DIVSEL(x)           SFR(JL_CLOCK->PLL_CON,  20, 2,  x)

//for MACRO - PLL_RSEL
enum {
    PLL_DIVIDER_INTE = 0,
    PLL_DIVIDER_FRAC,
};

#define PLL_PFD(x)              SFR(JL_CLOCK->PLL_CON,  22, 2,  x)
#define PLL_ICP(x)              SFR(JL_CLOCK->PLL_CON,  24, 2,  x)
#define PLL_LPFR2(x)            SFR(JL_CLOCK->PLL_CON,  26, 2,  x)
#define PLL_LPFR3(x)            SFR(JL_CLOCK->PLL_CON,  28, 2,  x)


#define PLL_RSEL(x)        		SFR(JL_CLOCK->PLL_CON,  30, 2,  x)
//for MACRO - PLL_RSEL
enum {
    PLL_RSEL_BTOSC_DIFF = 0, 	//btosc 差分时钟
    PLL_RSEL_RTOSC_DIFF,		//rtosc 差分时钟
    PLL_RSEL_PLL_REF_SEL,
};

#define PLL_DIVm(x)             SFR(JL_CLOCK->PLL_CON1, 0,  8,  x)
#define PLL_LD012A(x)           SFR(JL_CLOCK->PLL_CON1, 8,  2,  x)
#define PLL_LDO12D(x)           SFR(JL_CLOCK->PLL_CON1, 10, 2,  x)
#define PLL_IVCO(x)             SFR(JL_CLOCK->PLL_CON1, 12, 3,  x)
#define PLL_LDO_BYPASS(x)       SFR(JL_CLOCK->PLL_CON1, 15, 1,  x)
#define PLL_TSSEL(x)            SFR(JL_CLOCK->PLL_CON1, 16, 2,  x)
#define PLL_TSOE(x)             SFR(JL_CLOCK->PLL_CON1, 18, 1,  x)

#define PLL_CLK480M_OE(x)       SFR(JL_CLOCK->PLL_CON1, 24, 1,  x)
#define PLL_CLK320M_OE(x)       SFR(JL_CLOCK->PLL_CON1, 25, 1,  x)
#define PLL_CLK192M_OE(x)       SFR(JL_CLOCK->PLL_CON1, 26, 1,  x)
#define PLL_CLK137M_OE(x)       SFR(JL_CLOCK->PLL_CON1, 27, 1,  x)
#define PLL_CLK107M_OE(x)       SFR(JL_CLOCK->PLL_CON1, 28, 1,  x)

#define PLL_FRAC(x)             SFR(JL_CLOCK->PLL_INTF, 0, 32,  x)
#define PLL_DMAX(x)             SFR(JL_CLOCK->PLL_DMAX, 0, 24,  x)
#define PLL_DMIN(x)             SFR(JL_CLOCK->PLL_DMIN, 0, 24,  x)
#define PLL_DSTP(x)             SFR(JL_CLOCK->PLL_DSTP, 0, 24,  x)

#define HSB_CLK_DIV(x)			SFR(JL_CLOCK->SYS_DIV,  0,  8,  x)
#define LSB_CLK_DIV(x)			SFR(JL_CLOCK->SYS_DIV,  8,  3,  x)
#define OTP_CLK_DIV(x)			SFR(JL_CLOCK->SYS_DIV,  16, 4,  x)

/********************************************************************************/


#endif
#endif
