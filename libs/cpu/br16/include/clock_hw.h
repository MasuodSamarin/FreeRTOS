#ifndef __CLOCK_HW_H__
#define __CLOCK_HW_H__

#include "typedef.h"

//#define CLOCK_HAL_DEBUG

#ifdef CLOCK_HAL_DEBUG

#define RC_EN(...)                //SFR(CLK_CON0,  0,  1,  x)
#define OSC_EN(...)               //SFR(CLK_CON0,  1,  1,  x)
#define OSC_HIGH_CURRENT(...)     //SFR(CLK_CON0,  2,  1,  x)
#define OSC_CLOCK_32K(...)        //SFR(CLK_CON0,  3,  1,  x)
#define OSC_CLOCK_IN(...)         //SFR(CLK_CON0,  4,  2,  x)
//for MACRO - OSC_CLOCK_IN
enum {
    OSC_CLOCK_IN_BT_OSC = 0,
    OSC_CLOCK_IN_RTOSC,
    OSC_CLOCK_IN_PAT,
    OSC_CLOCK_IN_DISABLE,
};

#define CLOCK_IN_MUX0(x)			//SFR(CLK_CON0,  2,  2,  x)
#define CLOCK_IN_MUX1(x)			//SFR(CLK_CON0,  6,  2,  x)
//for MACRO - CLOCK_IN
enum {
    MAIN_CLOCK_IN_MUX0 = 0,
    MAIN_CLOCK_IN_MUX1,
};
enum {
    ///注:CLOCK_IN_MUX0_RC_0/CLOCK_IN_MUX0_RC_1/CLOCK_IN_MUX1_RC都是同一个RC，只是重复占据而已。
    ///   CLOCK_IN_MUX0_BT_OSC/CLOCK_IN_MUX1_BT_OSC，CLOCK_IN_MUX0_RTOSC/CLOCK_IN_MUX1_RTOSC同理。

    ///CKMUX0
    CLOCK_IN_MUX0_RC_0 = 0,
    CLOCK_IN_MUX0_RC_1,
    CLOCK_IN_MUX0_BT_OSC,
    CLOCK_IN_MUX0_RTOSC,

    ///CKMUX1
    CLOCK_IN_MUX1_PLL = 0,
    CLOCK_IN_MUX1_RC,
    CLOCK_IN_MUX1_BT_OSC,
    CLOCK_IN_MUX1_RTOSC,
};

#define CLOCK_SEL(...)            //SFR(CLK_CON0,  8,  1,  x)
#define SFR_MODE(...)             //SFR(CLK_CON0,  9,  1,  x)
#define PA4_CLOCK_OUT(...)        //SFR(CLK_CON0,  10,  3,  x)
#define PA5_CLOCK_OUT(x)     	//SFR(CLK_CON0,  13,  3,  x)

#define USB_CLOCK_IN(x)         //SFR(CLK_CON1,  0,  2,  x)
//for MACRO - USB_CLOCK_IN
enum {
    USB_CLOCK_IN_PLL48M = 0,
    USB_CLOCK_IN_OSC,
    USB_CLOCK_IN_SYS,
    USB_CLOCK_IN_DISABLE,
};
#define DAC_CLOCK_IN(x)         //SFR(CLK_CON1,  2,  2,  x)
//for MACRO - DAC_CLOCK_IN
enum {
    DAC_CLOCK_IN_PLL24M = 0,
    DAC_CLOCK_IN_OSC,
    DAC_CLOCK_IN_SYS,
    DAC_CLOCK_IN_DISABLE,
};
#define LADC_CLOCK_IN(x)        //SFR(CLK_CON1,  4,  2,  x)
//for MACRO - LADC_CLOCK_IN
enum {
    LADC_CLOCK_IN_PLL96M = 0,
    LADC_CLOCK_IN_OSC,
    LADC_CLOCK_IN_SYS,
    LADC_CLOCK_IN_DISABLE,
};
#define APC_CLOCK_IN(x)        	//SFR(CLK_CON1,  6,  2,  x)
//for MACRO - APC_CLOCK_IN
enum {
    APC_CLOCK_IN_PLLAPC = 0,
    APC_CLOCK_IN_OSC,
    APC_CLOCK_IN_SYS,
    APC_CLOCK_IN_DISABLE,
};
#define LCD_CLOCK_IN(x)         //SFR(CLK_CON1,  8,  2,  x)
//for MACRO - LCD_CLOCK_IN
enum {
    LCD_CLOCK_IN_WCLK = 0,
    LCD_CLOCK_IN_RTC_OSC,
    LCD_CLOCK_IN_SYS,
    LCD_CLOCK_IN_DISABLE,
};
#define UART_CLOCK_IN(x)        //SFR(CLK_CON1,  10,  2,  x)
//for MACRO - UART_CLOCK_IN
enum {
    UART_CLOCK_IN_OSC = 0,
    UART_CLOCK_IN_PLL48M,
    UART_CLOCK_IN_LSB,
    UART_CLOCK_IN_DISABLE,
};
#define FM_CLOCK_IN(x)         //SFR(CLK_CON1,  12,  2,  x)
//for MACRO - FM_CLOCK_IN
enum {
    FM_CLOCK_IN_PLL_FM = 0,
    FM_CLOCK_IN_OSC,
    FM_CLOCK_IN_SYS,
    FM_CLOCK_IN_DISABLE,
};
#define BT_CLOCK_IN(x)         //SFR(CLK_CON1,  14,  2,  x)
//for MACRO - BT_CLOCK_IN
enum {
    BT_CLOCK_IN_PLL64M = 0,
    BT_CLOCK_IN_DISABLE_0,
    BT_CLOCK_IN_SYS,
    BT_CLOCK_IN_DISABLE_1,
};


#define PLL_SYS_SEL(x)          //SFR(CLK_CON2,  0,  2,  x)
//for MACRO - PLL_SYS_SEL
enum {
    PLL_SYS_SEL_PLL192M = 0,
    PLL_SYS_SEL_PLL480M,
    PLL_SYS_SEL_PLL_FM,			//<-调试用
    PLL_SYS_SEL_DISABLE,
};
#define PLL_SYS_DIV(x)          //SFR(CLK_CON2,  2,  4,  x)
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

#define PLL_ALNK_DIV(x)         //SFR(CLK_CON2,  24,  1,  x)
//for MACRO - PLL_ALNK_DIV
enum {
    PLL_ALNK_192M_DIV17 = 0,
    PLL_ALNK_480M_DIV39,
};

#define PLL_RFI_SEL(x)          //SFR(CLK_CON2,  12,  2,  x)
//for MACRO - PLL_RFI_SEL
enum {
    PLL_RFI_SEL_PLL192M = 0,
    PLL_RFI_SEL_PLL480M,
    PLL_RFI_SEL_PLL_FM,		//<-调试用
    PLL_RFI_SEL_DISABLE,
};
#define PLL_RFI_DIV(x)          //SFR(CLK_CON2,  14,  4,  x)
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
#define PLL_APC_SEL(x)	        //SFR(CLK_CON2,  18,  2,  x)
//for MACRO - PLL_APC_SEL
enum {
    PLL_APC_SEL_PLL192M = 0,
    PLL_APC_SEL_PLL480M,
    PLL_APC_SEL_PLL_FM,		//<-调试用
    PLL_APC_SEL_DISABLE,
};
#define PLL_APC_DIV(x)	        //SFR(CLK_CON2,  20,  4,  x)
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
#define PLL_ALINK_SEL(x)	    //SFR(CLK_CON2,  24,  1,  x)

#define PIN_RESET_EN(x)    		//SFR(CLK_CON2,  26,  1,  x)
#define PIN_RESET_SEL(x)    	//SFR(CLK_CON2,  27,  1,  x)
//for MACRO - PIN_RESET_SEL
enum {
    PIN_RESET_SEL_PA0 = 0,
    PIN_RESET_SEL_PB5,
};
#define SFC_DELAY_SEL(x)	    //SFR(CLK_CON2,  28,  2,  x)
//for MACRO - SFC_DELAY_SEL
enum {
    SFC_DELAY_20MS = 0,
    SFC_DELAY_21MS,
    SFC_DELAY_22MS,
    SFC_DELAY_23MS,
};




#define PLL_EN(x)         		//SFR(PLL_CON,  0,  1,  x)
#define PLL_REST(x)             //SFR(PLL_CON,  1,  1,  x)
#define PLL_REFDS(x)          	//SFR(PLL_CON,  2,  5,  x)
#define PLL_REFDS_EN(x)        	//SFR(PLL_CON,  7,  1,  x)
#define PLL_REF_SEL(x)        	//SFR(PLL_CON,  8,  1,  x)
//for MACRO - PLL_REF_SEL
enum {
    PLL_REF_SEL_PLL_REF = 0,
    PLL_REF_SEL_RTOSC,
};

#define PLL_REF_CLK(x)         	//SFR(PLL_CON,  16,  1,  x)
//for MACRO - PLL_REF_CLK
enum {
    PLL_REF_CLK_2M = 0, //2M 鉴相器
    PLL_REF_CLK_12M,    //12M 鉴相器
};

#define PLL_RESOURCE(x)         //SFR(PLL_CON,  17,  2,  x)
//for MACRO - PLL_RESOURCE
enum {
    PLL_RESOURCE_BT_OSC = 0,
    PLL_RESOURCE_RTOSC,
    PLL_RESOURCE_RC,
    PLL_RESOURCE_PAT,
};

#define HSB_CLK_DIV(x)			//SFR(SYS_DIV,  0,  8,  x)
#define LSB_CLK_DIV(x)			//SFR(SYS_DIV,  8,  2,  x)
//for MACRO - LSB_CLK_DIV
enum {
    LSB_CLK_DIV1 = 0,
    LSB_CLK_DIV2,
    LSB_CLK_DIV3,
    LSB_CLK_DIV4,
};

#else

#define RC_EN(x)                SFR(CLK_CON0,  0,  1,  x)
#define OSC_EN(x)               SFR(CLK_CON0,  1,  1,  x)
#define OSC_HIGH_CURRENT(x)     SFR(CLK_CON0,  2,  1,  x)
#define OSC_CLOCK_32K(x)        SFR(CLK_CON0,  3,  1,  x)
#define OSC_CLOCK_IN(x)         SFR(CLK_CON0,  4,  2,  x)
//for MACRO - OSC_CLOCK_IN
enum {
    OSC_CLOCK_IN_BT_OSC = 0,
    OSC_CLOCK_IN_RTOSC,
    OSC_CLOCK_IN_PAT,
    OSC_CLOCK_IN_DISABLE,
};

#define CLOCK_IN_MUX0(x)             SFR(CLK_CON0,  2,  2,  x)
#define CLOCK_IN_MUX1(x)             SFR(CLK_CON0,  6,  2,  x)
//for MACRO - CLOCK_IN
enum {
    MAIN_CLOCK_IN_MUX0 = 0,
    MAIN_CLOCK_IN_MUX1,
};
enum {
    ///注:CLOCK_IN_MUX0_RC_0/CLOCK_IN_MUX0_RC_1/CLOCK_IN_MUX1_RC都是同一个RC，只是重复占据而已。
    ///   CLOCK_IN_MUX0_BT_OSC/CLOCK_IN_MUX1_BT_OSC，CLOCK_IN_MUX0_RTOSC/CLOCK_IN_MUX1_RTOSC同理。

    ///CKMUX0
    CLOCK_IN_MUX0_RC_0 = 0,
    CLOCK_IN_MUX0_RC_1,
    CLOCK_IN_MUX0_BT_OSC,
    CLOCK_IN_MUX0_RTOSC,

    ///CKMUX1
    CLOCK_IN_MUX1_PLL = 0,
    CLOCK_IN_MUX1_RC,
    CLOCK_IN_MUX1_BT_OSC,
    CLOCK_IN_MUX1_RTOSC,
};

#define CLOCK_SEL(x)            SFR(CLK_CON0,  8,  1,  x)
#define SFR_MODE(x)             SFR(CLK_CON0,  9,  1,  x)
#define PA4_CLOCK_OUT(x)        SFR(CLK_CON0,  10,  3,  x)
#define PA5_CLOCK_OUT(x)     	SFR(CLK_CON0,  13,  3,  x)


#define USB_CLOCK_IN(x)         SFR(CLK_CON1,  0,  2,  x)
//for MACRO - USB_CLOCK_IN
enum {
    USB_CLOCK_IN_PLL48M = 0,
    USB_CLOCK_IN_OSC,
    USB_CLOCK_IN_SYS,
    USB_CLOCK_IN_DISABLE,
};
#define DAC_CLOCK_IN(x)         SFR(CLK_CON1,  2,  2,  x)
//for MACRO - DAC_CLOCK_IN
enum {
    DAC_CLOCK_IN_PLL24M = 0,
    DAC_CLOCK_IN_OSC,
    DAC_CLOCK_IN_SYS,
    DAC_CLOCK_IN_DISABLE,
};
#define LADC_CLOCK_IN(x)        SFR(CLK_CON1,  4,  2,  x)
//for MACRO - LADC_CLOCK_IN
enum {
    LADC_CLOCK_IN_PLL96M = 0,
    LADC_CLOCK_IN_OSC,
    LADC_CLOCK_IN_SYS,
    LADC_CLOCK_IN_DISABLE,
};
#define APC_CLOCK_IN(x)        	SFR(CLK_CON1,  6,  2,  x)
//for MACRO - APC_CLOCK_IN
enum {
    APC_CLOCK_IN_PLLAPC = 0,
    APC_CLOCK_IN_OSC,
    APC_CLOCK_IN_SYS,
    APC_CLOCK_IN_DISABLE,
};
#define LCD_CLOCK_IN(x)         SFR(CLK_CON1,  8,  2,  x)
//for MACRO - LCD_CLOCK_IN
enum {
    LCD_CLOCK_IN_WCLK = 0,
    LCD_CLOCK_IN_RTC_OSC,
    LCD_CLOCK_IN_SYS,
    LCD_CLOCK_IN_DISABLE,
};
#define UART_CLOCK_IN(x)        SFR(CLK_CON1,  10,  2,  x)
//for MACRO - UART_CLOCK_IN
enum {
    UART_CLOCK_IN_OSC = 0,
    UART_CLOCK_IN_PLL48M,
    UART_CLOCK_IN_LSB,
    UART_CLOCK_IN_DISABLE,
};
#define FM_CLOCK_IN(x)         SFR(CLK_CON1,  12,  2,  x)
//for MACRO - FM_CLOCK_IN
enum {
    FM_CLOCK_IN_PLL_FM = 0,
    FM_CLOCK_IN_OSC,
    FM_CLOCK_IN_SYS,
    FM_CLOCK_IN_DISABLE,
};
#define BT_CLOCK_IN(x)         SFR(CLK_CON1,  14,  2,  x)
//for MACRO - BT_CLOCK_IN
enum {
    BT_CLOCK_IN_PLL64M = 0,
    BT_CLOCK_IN_DISABLE_0,
    BT_CLOCK_IN_SYS,
    BT_CLOCK_IN_DISABLE_1,
};


#define PLL_SYS_SEL(x)          SFR(CLK_CON2,  0,  2,  x)
//for MACRO - PLL_SYS_SEL
enum {
    PLL_SYS_SEL_PLL192M = 0,
    PLL_SYS_SEL_PLL480M,
    PLL_SYS_SEL_PLL_FM,			//<-调试用
    PLL_SYS_SEL_DISABLE,
};
#define PLL_SYS_DIV(x)          SFR(CLK_CON2,  2,  4,  x)
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

#define PLL_ALNK_DIV(x)         SFR(CLK_CON2,  24,  1,  x)
//for MACRO - PLL_ALNK_DIV
enum {
    PLL_ALNK_192M_DIV17 = 0,
    PLL_ALNK_480M_DIV39,
};

#define PLL_RFI_SEL(x)          SFR(CLK_CON2,  12,  2,  x)
//for MACRO - PLL_RFI_SEL
enum {
    PLL_RFI_SEL_PLL192M = 0,
    PLL_RFI_SEL_PLL480M,
    PLL_RFI_SEL_PLL_FM,		//<-调试用
    PLL_RFI_SEL_DISABLE,
};
#define PLL_RFI_DIV(x)          SFR(CLK_CON2,  14,  4,  x)
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
#define PLL_APC_SEL(x)	        SFR(CLK_CON2,  18,  2,  x)
//for MACRO - PLL_APC_SEL
enum {
    PLL_APC_SEL_PLL192M = 0,
    PLL_APC_SEL_PLL480M,
    PLL_APC_SEL_PLL_FM,		//<-调试用
    PLL_APC_SEL_DISABLE,
};
#define PLL_APC_DIV(x)	        SFR(CLK_CON2,  20,  4,  x)
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
#define PLL_ALINK_SEL(x)	    SFR(CLK_CON2,  24,  1,  x)
//for MACRO - PLL_ALINK_SEL
enum {
    PLL_ALINK_SEL_12288M = 0,
    PLL_ALINK_SEL_112896M,
};
#define PIN_RESET_EN(x)    		SFR(CLK_CON2,  26,  1,  x)
#define PIN_RESET_SEL(x)    	SFR(CLK_CON2,  27,  1,  x)
//for MACRO - PIN_RESET_SEL
enum {
    PIN_RESET_SEL_PA0 = 0,
    PIN_RESET_SEL_PB5,
};
#define SFC_DELAY_SEL(x)	    SFR(CLK_CON2,  28,  2,  x)
//for MACRO - SFC_DELAY_SEL
enum {
    SFC_DELAY_20MS = 0,
    SFC_DELAY_21MS,
    SFC_DELAY_22MS,
    SFC_DELAY_23MS,
};




#define PLL_EN(x)         		SFR(PLL_CON,  0,  1,  x)
#define PLL_REST(x)             SFR(PLL_CON,  1,  1,  x)
#define PLL_REFDS(x)          	SFR(PLL_CON,  2,  5,  x)
#define PLL_REFDS_EN(x)        	SFR(PLL_CON,  7,  1,  x)
#define PLL_REF_SEL(x)        	SFR(PLL_CON,  8,  1,  x)
//for MACRO - PLL_REF_SEL
enum {
    PLL_REF_SEL_PLL_REF = 0,
    PLL_REF_SEL_RTOSC,
};

#define PLL_REF_CLK(x)         	SFR(PLL_CON,  16,  1,  x)
//for MACRO - PLL_REF_CLK
enum {
    PLL_REF_CLK_2M = 0, //2M 鉴相器
    PLL_REF_CLK_12M,    //12M 鉴相器
};

#define PLL_RESOURCE(x)         SFR(PLL_CON,  17,  2,  x)
//for MACRO - PLL_RESOURCE
enum {
    PLL_RESOURCE_BT_OSC = 0,
    PLL_RESOURCE_RTOSC,
    PLL_RESOURCE_RC,
    PLL_RESOURCE_PAT,
};

#define HSB_CLK_DIV(x)			SFR(SYS_DIV,  0,  8,  x)
#define LSB_CLK_DIV(x)			SFR(SYS_DIV,  8,  3,  x)
//for MACRO - LSB_CLK_DIV
enum {
    LSB_CLK_DIV1 = 0,
    LSB_CLK_DIV2,
    LSB_CLK_DIV3,
    LSB_CLK_DIV4,
};

/********************************************************************************/



#endif
#endif
