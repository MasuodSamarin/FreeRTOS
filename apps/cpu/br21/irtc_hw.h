#ifndef __IRTC_HW_H__
#define __IRTC_HW_H__

#include "typedef.h"


#define IRTC_WAKEUP_IE(x)	   SFR(JL_IRTC->CON, 3, 1, x)
#define IRTC_IS_WAKEUP()       (JL_IRTC->CON & BIT(7) && (JL_IRTC->CON & BIT(3)))
#define IRTC_CLEAR_WAKEUP()    SFR(JL_IRTC->CON,  15,  1,  1)

#define IRTC_X512Hz_IE(x)	   SFR(JL_IRTC->CON, 2, 1, x)
#define IRTC_IS_512Hz()        (JL_IRTC->CON & BIT(6) && (JL_IRTC->CON & BIT(2)))
#define IRTC_CLEAR_512Hz()     SFR(JL_IRTC->CON,  14,  1,  1)

#define IRTC_X2Hz_IE(x)		   SFR(JL_IRTC->CON, 1, 1, x)
#define IRTC_IS_2Hz()          (JL_IRTC->CON & BIT(5) && (JL_IRTC->CON & BIT(1)))
#define IRTC_CLEAR_2Hz()       SFR(JL_IRTC->CON,  13,  1,  1)

#define IRTC_IS_ALMOUT(x)      ((x & BIT(5)) && x & BIT(6))

#define IRTC_IS_WAKEUP_IO0(x)     (x & BIT(0))
#define IRTC_IS_WAKEUP_IO1(x)     (x & BIT(1))
#define IRTC_IS_WAKEUP_IO2(x)     (x & BIT(2))
#define IRTC_IS_WAKEUP_IO3(x)     (x & BIT(3))

#define IRTC_IS_WAKEUP_OVERFLOW(x)  (x & BIT(0))
#define IRTC_IS_WAKEUP_PORT(x)      (x & BIT(1))
#define IRTC_IS_WAKEUP_ALM(x)       (x & BIT(2))
#define IRTC_IS_WAKEUP_LDOIN(x)     (x & BIT(3))

///IRTC DATA TRANSFER
#define IRTC_CS(x)				SFR(JL_IRTC->CON, 8,  1, x)

///RTC REG (WRITE or READ) CONTROL CMD
#define W_RTC_REG          	0x20 //<WRITE REG
#define R_RTC_REG          	0xA0 //<READ REG
///RTC REG ADDR
//FOR PORTR REG ADDR
#define PORTR_IN_ADDR		0x1	 //<R only
#define PORTR_HD_ADDR		0x1	 //<R&W
#define PORTR_OUT_ADDR		0x2	 //<R&W
#define PORTR_DIR_ADDR		0x2	 //<R&W
#define PORTR_PU_ADDR		0x3	 //<R&W
#define PORTR_PD_ADDR		0x3  //<R&W
#define PORTR_DIE_ADDR		0x8	 //<R&W
#define PORTR_RESET_ADDR    0x8	 //<R&W
//FOR RTOSC REG ADDR
#define RTC_CON_ADDR		0x0
#define OSC_CON_ADDR		0x6
#define LDO_CON_ADDR		0x7
#define DIFF_CON_ADDR		0x9
#define OSH_CON_ADDR		0xa

//FOR WAKEUP REG ADDR
#define WKUP_EN_ADDR		0x4 //<R&W
#define WKUP_EDGE_ADDR		0x4 //<R&W
#define WKUP_PND_ADDR		0x5 //<R only
#define WKUP_CLR_PND_ADDR	0x5 //<W only
#define ALL_WKUP_SRC_ADDR   0x13 //R only

//FOR pluse counter control  register
#define PCNT_CON_ADDR       0x11
#define PCNT_VALUE_ADDR     0x12

/* RESERVED      	  1bit RW  */ //BIT 7
/* RESERVED      	  1bit RW  */ //BIT 6
/* X32XS			  2bit RW  */ //BIT 4-5
/* RESERVED      	  1bit RW  */ //BIT 3
/* X32TS			  1bit RW  */ //BIT 2
/* X32OE    		  1bit RW  */ //BIT 1
/* X32EN              1bit RW  */ //BIT 0
#define RTC_CON_ADDR06		0x06

/* LDO5V_PND          1bit RW  */ //BIT 7  LDO5V pending
/* LDO5V_PND_CLR      1bit RW  */ //BIT 6  clear LDO5V pending
/* PWR_EN[1]		  2bit RW  */ //BIT 5  0:PR1做普通IO口 1:输出内部Wake Up信号
/* PWR_EN[0]          1bit RW  */ //BIT 4  0:PR0做普通IO口 1:输出内部Wake Up信号
/* RESERVED      	  1bit RW  */ //BIT 3
/* LDO5V_DET		  1bit RW  */ //BIT 2  0:LDO5V没有插入 1:LDO5V插入
/* LDO5V_EDGE    	  1bit RW  */ //BIT 1  0:上升沿   1:下降沿
/* LDO5V_EN           1bit RW  */ //BIT 0
#define RTC_CON_ADDR07		0x07

/* RESERVED        	  4bit RW  */ //BIT 4-7
/* LVD5V WAKEUP       1bit RW  */ //BIT 3
/* ALMOUT      	      1bit RW  */ //BIT 2
/* EDGE_WAKEUP        1bit RW  */ //BIT 1
/* PCNT OVF           1bit RW  */ //BIT 0
#define RTC_CON_ADDR13		0x13

/* RESERVED        	  5bit RW  */ //BIT 3-7
/* ADC MUX            3bit RW  */ //BIT 0-2
#define RTC_CON_ADDR14		0x14


#define WRITE_RTC 			            0x40
#define READ_RTC  			            0xC0
#define WRITE_ALM  			            0x10
#define READ_ALM  			            0x90

#endif
