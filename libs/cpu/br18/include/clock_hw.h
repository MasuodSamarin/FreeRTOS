#ifndef __CLOCK_HW_H__
#define __CLOCK_HW_H__

#include "typedef.h"

//#define CLOCK_HAL_DEBUG

#ifdef CLOCK_HAL_DEBUG

#define RC_EN(...)                //SFR(JL_CLOCK->CLK_CON0,  0,  1,  x)
#define OSC_CLOCK_IN(...)         //SFR(JL_CLOCK->CLK_CON0,  4,  2,  x)
//for MACRO - OSC_CLOCK_IN
enum {
    OSC_CLOCK_IN_BT_OSC = 0,
    OSC_CLOCK_IN_RTOSC_H,
    OSC_CLOCK_IN_RTOSC_L,
    OSC_CLOCK_IN_PAT,
};

#define CLOCK_SEL(...)            	//SFR(JL_CLOCK->CLK_CON0,  6,  3,  x)
#define TEST_SEL(...)				//SFR(JL_CLOCK->CLK_CON0,  1,  1,  x)
//for MACRO - CLOCK_IN
enum {
    MAIN_CLOCK_IN_RC = 0,
    MAIN_CLOCK_IN_BTOSC = 4,
    MAIN_CLOCK_IN_RTOSC_H,
    MAIN_CLOCK_IN_RTOSC_L,
    MAIN_CLOCK_IN_PLL,

    MAIN_CLOCK_IN_PAT, //for test mode
};

#define SFR_MODE(...)             //SFR(JL_CLOCK->CLK_CON0,  9,  1,  x)
#define PB0_CLOCK_OUT(...)        //SFR(JL_CLOCK->CLK_CON0,  10,  3,  x)
#define PB9_CLOCK_OUT(x)			//SFR(JL_CLOCK->CLK_CON0,  13,  3,  x)

#define USB_CLOCK_IN(x)         //SFR(JL_CLOCK->CLK_CON1,  0,  2,  x)
//for MACRO - USB_CLOCK_IN
enum {
    USB_CLOCK_IN_PLL48M = 0,
    USB_CLOCK_IN_DISABLE_0,
    USB_CLOCK_IN_LSB,
    USB_CLOCK_IN_DISABLE_1,
};
#define DAC_CLOCK_IN(x)         //SFR(JL_CLOCK->CLK_CON1,  2,  2,  x)
//for MACRO - DAC_CLOCK_IN
enum {
    DAC_CLOCK_IN_PLL24M = 0,
    DAC_CLOCK_IN_DISABLE_0,
    DAC_CLOCK_IN_LSB,
    DAC_CLOCK_IN_DISABLE_1,
};
#define APC_CLOCK_IN(x)        	//SFR(JL_CLOCK->CLK_CON1,  6,  2,  x)
//for MACRO - APC_CLOCK_IN
enum {
    APC_CLOCK_IN_PLLAPC = 0,
    APC_CLOCK_IN_DISABLE_0,
    APC_CLOCK_IN_LSB,
    APC_CLOCK_IN_DISABLE_1,
};
#define LCD_CLOCK_IN(x)         //SFR(JL_CLOCK->CLK_CON1,  8,  2,  x)
//for MACRO - LCD_CLOCK_IN
enum {
    LCD_CLOCK_IN_WCLK = 0,
    LCD_CLOCK_IN_RTCOSC_L,
    LCD_CLOCK_IN_LSB,
    LCD_CLOCK_IN_DISABLE,
};
#define UART_CLOCK_IN(x)        //SFR(JL_CLOCK->CLK_CON1,  10,  2,  x)
//for MACRO - UART_CLOCK_IN
enum {
    UART_CLOCK_IN_OSC = 0,
    UART_CLOCK_IN_PLL48M,
    UART_CLOCK_IN_LSB,
    UART_CLOCK_IN_DISABLE,
};
#define RFI_CLOCK_IN(x)         //SFR(JL_CLOCK->CLK_CON1,  12,  2,  x)
//for MACRO - RFI_CLOCK_IN
enum {
    RFI_CLOCK_IN_PLL_RFI = 0,
    RFI_CLOCK_IN_DISABLE_0,
    RFI_CLOCK_IN_LSB,
    RFI_CLOCK_IN_DISABLE_1,
};
#define BT_CLOCK_IN(x)         //SFR(JL_CLOCK->CLK_CON1,  14,  2,  x)
//for MACRO - BT_CLOCK_IN
enum {
    BT_CLOCK_IN_WL = 0,
    BT_CLOCK_IN_DISABLE_0,
    BT_CLOCK_IN_LSB,
    BT_CLOCK_IN_DISABLE_1,
};


#define PLL_SYS_SEL(x)          //SFR(JL_CLOCK->CLK_CON2,  0,  2,  x)
//for MACRO - PLL_SYS_SEL
enum {
    PLL_SYS_SEL_PLL192M = 0,
    PLL_SYS_SEL_PLL480M,
    PLL_SYS_SEL_PLL_FM,			//<-调试用
    PLL_SYS_SEL_DISABLE,
};
#define PLL_SYS_DIV(x)          //SFR(JL_CLOCK->CLK_CON2,  2,  4,  x)
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

#define PLL_ALNK_SEL(x)         //SFR(JL_CLOCK->CLK_CON2,  30,  1,  x)
//for MACRO - PLL_ALNK_SEL
enum {
    PLL_ALNK_192M_DIV17 = 0,
    PLL_ALNK_480M_DIV39,
};

#define PLL_RFI_SEL(x)          //SFR(JL_CLOCK->CLK_CON2,  12,  2,  x)
//for MACRO - PLL_RFI_SEL
enum {
    PLL_RFI_SEL_PLL192M = 0,
    PLL_RFI_SEL_PLL480M,
    PLL_RFI_SEL_PLL_FM,		//<-调试用
    PLL_RFI_SEL_DISABLE,
};
#define PLL_RFI_DIV(x)          //SFR(JL_CLOCK->CLK_CON2,  14,  4,  x)
//for MACRO - PLL_RFI_DIV
enum {
    PLL_RFI_DIV1 = 0,
    PLL_RFI_DIV3,
    PLL_RFI_DIV5,
    PLL_RFI_DIV7,

    PLL_RFI_DIV1X2 = 4,
    PLL_RFI_DIV3X2,
    PLL_RFI_DIV5X2,
    PLL_RFI_DIV7X2,

    PLL_RFI_DIV1X4 = 8,
    PLL_RFI_DIV3X4,
    PLL_RFI_DIV5X4,
    PLL_RFI_DIV7X4,

    PLL_RFI_DIV1X8 = 12,
    PLL_RFI_DIV3X8,
    PLL_RFI_DIV5X8,
    PLL_RFI_DIV7X8,
};

#define PLL_APC_SEL(x)	        //SFR(JL_CLOCK->CLK_CON2,  18,  2,  x)
//for MACRO - PLL_APC_SEL
enum {
    PLL_APC_SEL_PLL192M = 0,
    PLL_APC_SEL_PLL480M,
    PLL_APC_SEL_DISABLE,
};
#define PLL_APC_DIV(x)	        //SFR(JL_CLOCK->CLK_CON2,  20,  4,  x)
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

#define SFC_DELAY_SEL(x)	    //SFR(JL_CLOCK->CLK_CON1,  28,  2,  x)
//for MACRO - SFC_DELAY_SEL
enum {
    SFC_DELAY_20MS = 0,
    SFC_DELAY_21MS,
    SFC_DELAY_22MS,
    SFC_DELAY_23MS,
};

#define PLL_EN(x)         		//SFR(JL_CLOCK->PLL_CON,  0,  1,  x)
#define PLL_REST(x)             //SFR(JL_CLOCK->PLL_CON,  1,  1,  x)
#define PLL_REFDS(x)          	//SFR(JL_CLOCK->PLL_CON,  2,  5,  x)
#define PLL_REFDS_EN(x)        	//SFR(JL_CLOCK->PLL_CON,  7,  1,  x)
#define PLL_RSEL(x)        		//SFR(JL_CLOCK->PLL_CON,  8,  2,  x)
//for MACRO - PLL_RSEL
enum {
    PLL_RSEL_BTOSC_DIFF = 0, 	//btosc 差分时钟
    PLL_RSEL_RTOSC_DIFF,		//rtosc 差分时钟
    PLL_RSEL_PLL_REF_SEL,
};

#define PLL_REF_SEL(x)         	//SFR(JL_CLOCK->PLL_CON,  17,  2,  x)
//for MACRO - PLL_RSEL
enum {
    PLL_REF_SEL_BTOSC = 0, 	//btosc
    PLL_REF_SEL_RTOSC,		//rtosc

    PLL_REF_SEL_PAT = 3,
};

#define PLL_REF_CLK(x)         	//SFR(JL_CLOCK->PLL_CON,  16,  1,  x)
//for MACRO - PLL_REF_CLK
enum {
    PLL_REF_CLK_2M = 0, //2M 鉴相器
    PLL_REF_CLK_12M,    //12M 鉴相器
};

#define HSB_CLK_DIV(x)			//SFR(JL_CLOCK->SYS_DIV,  0,  8,  x)
#define LSB_CLK_DIV(x)			//SFR(JL_CLOCK->SYS_DIV,  8,  2,  x)


#else

#define RC_EN(x)                SFR(JL_CLOCK->CLK_CON0,  0,  1,  x)
#define OSC_CLOCK_IN(x)         SFR(JL_CLOCK->CLK_CON0,  4,  2,  x)
//for MACRO - OSC_CLOCK_IN
enum {
    OSC_CLOCK_IN_BT_OSC = 0,
    OSC_CLOCK_IN_RTOSC_H,
    OSC_CLOCK_IN_RTOSC_L,
    OSC_CLOCK_IN_PAT,
};

#define CLOCK_SEL(x)			SFR(JL_CLOCK->CLK_CON0,  6,  3,  x)
#define TEST_SEL(x)				SFR(JL_CLOCK->CLK_CON0,  1,  1,  x)
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
#define PB0_CLOCK_OUT(x)        SFR(JL_CLOCK->CLK_CON0,  10,  3,  x)
#define PB9_CLOCK_OUT(x)     	SFR(JL_CLOCK->CLK_CON0,  13,  3,  x)


#define USB_CLOCK_IN(x)         SFR(JL_CLOCK->CLK_CON1,  0,  2,  x)
//for MACRO - USB_CLOCK_IN
enum {
    USB_CLOCK_IN_PLL48M = 0,
    USB_CLOCK_IN_DISABLE_0,
    USB_CLOCK_IN_LSB,
    USB_CLOCK_IN_DISABLE_1,
};
#define DAC_CLOCK_IN(x)         SFR(JL_CLOCK->CLK_CON1,  2,  2,  x)
//for MACRO - DAC_CLOCK_IN
enum {
    DAC_CLOCK_IN_PLL24M = 0,
    DAC_CLOCK_IN_DISABLE_0,
    DAC_CLOCK_IN_LSB,
    DAC_CLOCK_IN_DISABLE_1,
};
#define APC_CLOCK_IN(x)        	SFR(JL_CLOCK->CLK_CON1,  6,  2,  x)
//for MACRO - APC_CLOCK_IN
enum {
    APC_CLOCK_IN_PLLAPC = 0,
    APC_CLOCK_IN_DISABLE_0,
    APC_CLOCK_IN_LSB,
    APC_CLOCK_IN_DISABLE_1,
};
#define LCD_CLOCK_IN(x)         SFR(JL_CLOCK->CLK_CON1,  8,  2,  x)
//for MACRO - LCD_CLOCK_IN
enum {
    LCD_CLOCK_IN_WCLK = 0,
    LCD_CLOCK_IN_RTCOSC_L,
    LCD_CLOCK_IN_LSB,
    LCD_CLOCK_IN_DISABLE,
};
#define UART_CLOCK_IN(x)        SFR(JL_CLOCK->CLK_CON1,  10,  2,  x)
//for MACRO - UART_CLOCK_IN
enum {
    UART_CLOCK_IN_OSC = 0,
    UART_CLOCK_IN_PLL48M,
    UART_CLOCK_IN_LSB,
    UART_CLOCK_IN_DISABLE,
};
#define RFI_CLOCK_IN(x)         SFR(JL_CLOCK->CLK_CON1,  12,  2,  x)
//for MACRO - RFI_CLOCK_IN
enum {
    RFI_CLOCK_IN_PLL_RFI = 0,
    RFI_CLOCK_IN_DISABLE_0,
    RFI_CLOCK_IN_LSB,
    RFI_CLOCK_IN_DISABLE_1,
};
#define BT_CLOCK_IN(x)         SFR(JL_CLOCK->CLK_CON1,  14,  2,  x)
//for MACRO - BT_CLOCK_IN
enum {
    BT_CLOCK_IN_WL = 0,
    BT_CLOCK_IN_DISABLE_0,
    BT_CLOCK_IN_LSB,
    BT_CLOCK_IN_DISABLE_1,
};


#define PLL_SYS_SEL(x)          SFR(JL_CLOCK->CLK_CON2,  0,  2,  x)
//for MACRO - PLL_SYS_SEL
enum {
    PLL_SYS_SEL_PLL192M = 0,
    PLL_SYS_SEL_PLL480M,
    PLL_SYS_SEL_PLL_FM,			//<-调试用
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

#define PLL_ALNK_SEL(x)         SFR(JL_CLOCK->CLK_CON2,  30,  1,  x)
//for MACRO - PLL_ALNK_SEL
enum {
    PLL_ALNK_192M_DIV17 = 0,
    PLL_ALNK_480M_DIV39,
};

#define PLL_RFI_SEL(x)          SFR(JL_CLOCK->CLK_CON2,  12,  2,  x)
//for MACRO - PLL_RFI_SEL
enum {
    PLL_RFI_SEL_PLL192M = 0,
    PLL_RFI_SEL_PLL480M,
    PLL_RFI_SEL_PLL_FM,		//<-调试用
    PLL_RFI_SEL_DISABLE,
};
#define PLL_RFI_DIV(x)          SFR(JL_CLOCK->CLK_CON2,  14,  4,  x)
//for MACRO - PLL_RFI_DIV
enum {
    PLL_RFI_DIV1 = 0,
    PLL_RFI_DIV3,
    PLL_RFI_DIV5,
    PLL_RFI_DIV7,

    PLL_RFI_DIV1X2 = 4,
    PLL_RFI_DIV3X2,
    PLL_RFI_DIV5X2,
    PLL_RFI_DIV7X2,

    PLL_RFI_DIV1X4 = 8,
    PLL_RFI_DIV3X4,
    PLL_RFI_DIV5X4,
    PLL_RFI_DIV7X4,

    PLL_RFI_DIV1X8 = 12,
    PLL_RFI_DIV3X8,
    PLL_RFI_DIV5X8,
    PLL_RFI_DIV7X8,
};

#define PLL_APC_SEL(x)	        SFR(JL_CLOCK->CLK_CON2,  18,  2,  x)
//for MACRO - PLL_APC_SEL
enum {
    PLL_APC_SEL_PLL192M = 0,
    PLL_APC_SEL_PLL480M,
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

#define SFC_DELAY_SEL(x)	    SFR(JL_CLOCK->CLK_CON1,  28,  2,  x)
//for MACRO - SFC_DELAY_SEL
enum {
    SFC_DELAY_20MS = 0,
    SFC_DELAY_21MS,
    SFC_DELAY_22MS,
    SFC_DELAY_23MS,
};


#define PLL_EN(x)         		SFR(JL_CLOCK->PLL_CON,  0,  1,  x)
#define PLL_REST(x)             SFR(JL_CLOCK->PLL_CON,  1,  1,  x)
#define PLL_REFDS(x)          	SFR(JL_CLOCK->PLL_CON,  2,  5,  x)
#define PLL_REFDS_EN(x)        	SFR(JL_CLOCK->PLL_CON,  7,  1,  x)
#define PLL_RSEL(x)        		SFR(JL_CLOCK->PLL_CON,  8,  2,  x)
//for MACRO - PLL_RSEL
enum {
    PLL_RSEL_BTOSC_DIFF = 0, 	//btosc 差分时钟
    PLL_RSEL_RTOSC_DIFF,		//rtosc 差分时钟
    PLL_RSEL_PLL_REF_SEL,
};

#define PLL_REF_SEL(x)         	SFR(JL_CLOCK->PLL_CON,  17,  2,  x)
//for MACRO - PLL_RSEL
enum {
    PLL_REF_SEL_BTOSC = 0, 	//btosc
    PLL_REF_SEL_RTOSC,		//rtosc

    PLL_REF_SEL_PAT = 3,
};

#define PLL_REF_CLK(x)         	SFR(JL_CLOCK->PLL_CON,  16,  1,  x)
//for MACRO - PLL_REF_CLK
enum {
    PLL_REF_CLK_2M = 0, //2M 鉴相器
    PLL_REF_CLK_12M,    //12M 鉴相器
};


#define HSB_CLK_DIV(x)			SFR(JL_CLOCK->SYS_DIV,  0,  8,  x)
#define LSB_CLK_DIV(x)			SFR(JL_CLOCK->SYS_DIV,  8,  3,  x)

/********************************************************************************/


#endif
#endif
