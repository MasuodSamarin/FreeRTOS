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
#define IRTC_CLK(x)				SFR(JL_IRTC->CON, 9,  1, x)
#define IRTC_DO(x)				SFR(JL_IRTC->CON, 10, 1, x)
#define IRTC_IS_DI()		    (JL_IRTC->CON & BIT(11))

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



///RTC PORT REG OPERATE CMD
//FOR PORTR REG
#define RTC_PR_IN_RD		        (R_RTC_REG | PORTR_IN_ADDR)  //<read  PORTR_IN
#define RTC_PR_HD_WR	        	(W_RTC_REG | PORTR_HD_ADDR)  //<write PORTR_HD
#define RTC_PR_HD_RD            	(R_RTC_REG | PORTR_HD_ADDR)  //<read  PORTR_HD
#define RTC_PR_OUT_WR	        	(W_RTC_REG | PORTR_OUT_ADDR) //<write PORTR_OUT
#define RTC_PR_OUT_RD         	    (R_RTC_REG | PORTR_OUT_ADDR) //<read  PORTR_OUT
#define RTC_PR_DIR_WR	        	(W_RTC_REG | PORTR_DIR_ADDR) //<write PORTR_DIR
#define RTC_PR_DIR_RD           	(R_RTC_REG | PORTR_DIR_ADDR) //<read  PORTR_DIR
#define RTC_PR_PU_WR            	(W_RTC_REG | PORTR_PU_ADDR)  //<write PORTR_PU
#define RTC_PR_PU_RD            	(R_RTC_REG | PORTR_PU_ADDR)  //<read  PORTR_PU
#define RTC_PR_PD_WR             	(W_RTC_REG | PORTR_PD_ADDR)  //<write PORTR_PD
#define RTC_PR_PD_RD            	(R_RTC_REG | PORTR_PD_ADDR)  //<read  PORTR_PD
#define RTC_PR_DIE_WR	        	(W_RTC_REG | PORTR_DIE_ADDR) //<write PORTR_DIE
#define RTC_PR_DIE_RD           	(R_RTC_REG | PORTR_DIE_ADDR) //<read  PORTR_DIE

#define RTC_PR_RESET_WR	        	(W_RTC_REG | PORTR_RESET_ADDR) //<write  PORTR_RESET_ADDR IN PORTR_DIE
#define RTC_PR_RESET_RD           	(R_RTC_REG | PORTR_RESET_ADDR) //<read  PORTR_RESET_ADDR IN PORTR_DIE

//FOR RTOSC REG
#define RTC_CON_RD		        	(R_RTC_REG | RTC_CON_ADDR)   //<read  RTC_CON
#define RTC_CON_WR		        	(W_RTC_REG | RTC_CON_ADDR)   //<write RTC_CON
#define OSC_CON_RD			        (R_RTC_REG | OSC_CON_ADDR)   //<read  OSC_CON
#define OSC_CON_WR		            (W_RTC_REG | OSC_CON_ADDR)   //<write OSC_CON
#define LDO_CON_RD			        (R_RTC_REG | LDO_CON_ADDR)   //<read  LDO_CON
#define LDO_CON_WR		            (W_RTC_REG | LDO_CON_ADDR)   //<write LDO_CON
#define OSH_CON_RD			        (R_RTC_REG | OSH_CON_ADDR)   //<read  OSH_CON
#define OSH_CON_WR		            (W_RTC_REG | OSH_CON_ADDR)   //<write OSH_CON
#define DIFF_CON_RD			        (R_RTC_REG | DIFF_CON_ADDR)   //<read  DIFF_CON
#define DIFF_CON_WR		            (W_RTC_REG | DIFF_CON_ADDR)   //<write DIFF_CON

//FOR WAKEUP REG
#define WKUP_EN_RD			        (R_RTC_REG | WKUP_EN_ADDR)   	//<read  WKUP_EN
#define WKUP_EN_WR			        (W_RTC_REG | WKUP_EN_ADDR)   	//<write WKUP_EN
#define WKUP_EDGE_RD		        (R_RTC_REG | WKUP_EDGE_ADDR) 	//<read  WKUP_EDGE
#define WKUP_EDGE_WR		        (W_RTC_REG | WKUP_EDGE_ADDR) 	//<write WKUP_EDGE
#define WKUP_PND_RD		            (R_RTC_REG | WKUP_PND_ADDR)		//<read  WKUP_PND
#define WKUP_CLR_PND_WR		        (W_RTC_REG | WKUP_CLR_PND_ADDR)	//<write WKUP_CLR_PND
#define ALL_WKUP_SRC_RD		        (R_RTC_REG | ALL_WKUP_SRC_ADDR)		//<read  ALL_WKUP_SRC

//FOR pluse counter control  register
#define PCNT_CON_RD                 (R_RTC_REG | PCNT_CON_ADDR)      //<read PCNT_CON
#define PCNT_CON_WR                 (W_RTC_REG | PCNT_CON_ADDR)      //<write PCNT_CON
#define PCNT_VALUE_RD               (R_RTC_REG | PCNT_VALUE_ADDR)      //<read PCNT_VALUE
#define PCNT_VALUE_WR               (W_RTC_REG | PCNT_VALUE_ADDR)      //<write PCNT_VALUE


#define WRITE_RTC 			            0x40
#define READ_RTC  			            0xC0
#define WRITE_ALM  			            0x10
#define READ_ALM  			            0x90

#endif
