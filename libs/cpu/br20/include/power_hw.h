//*********************************************************************************//
// Module name : pwr_ctl
// Description : 3.3V pmu control sfr
// By Designer : shusheng
// Dat changed :                                                                   //
//*********************************************************************************//

#ifndef _PWR_CTL_
#define _PWR_CTL_


////////////////////////////////////////////////////////////
//
//  power manager unit defines
//
////////////////////////////////////////////////////////////
#define pmu_csen()      (JL_PMU->CON |= BIT(0))
#define pmu_csdis()     (JL_PMU->CON &= ~BIT(0))
#define pmu_stran()     (JL_PMU->CON |= BIT(5))
#define pmu_swait()     (JL_PMU->CON & BIT(1))

#define pmu_go()        (JL_PMU->CON |= BIT(4))
#define pmu_wto()       (JL_PMU->CON & BIT(7))




#define RD_PMU_CON0         (0x00)
#define WR_PMU_CON0         (RD_PMU_CON0 | 0x80)
// {TOPND, CLRPND, PDRUN, TOEN, DISEN[1:0], PDMD, PDEN}


#define RD_PMU_CON1         (0x01)
#define WR_PMU_CON1         (RD_PMU_CON1 | 0x80)
// {PORFLAG, CLRFLAG, CKDIV[1:0], PUSLOW, CKSEL[2:0]}


#define RD_SOFT_FLAG        (0x02)
#define WR_SOFT_FLAG        (RD_SOFT_FLAG | 0x80)
// {SFLAG[7:0]}


#define RD_PWR_SCON0        (0x03)
#define WR_PWR_SCON0        (RD_PWR_SCON0 | 0x80)
//


#define RD_STB10_SET        (0x04)
#define WR_STB10_SET        (RD_STB10_SET | 0x80)
// {STB1_SET[3:0], STB0_SET[3:0]}
#define RD_STB32_SET        (0x05)
#define WR_STB32_SET        (RD_STB32_SET | 0x80)
// {STB3_SET[3:0], STB2_SET[3:0]}
#define RD_STB54_SET        (0x06)
#define WR_STB54_SET        (RD_STB54_SET | 0x80)
// {STB5_SET[3:0], STB4_SET[3:0]}
#define RD_STB6_SET         (0x07)
#define WR_STB6_SET         (RD_STB6_SET | 0x80)
// {MSTM[3:0], STB6_SET[3:0]}


#define RD_LVC3_SET         (0x08)
#define WR_LVC3_SET         (RD_LVC3_SET | 0x80)
// {LVC[31:24]}
#define RD_LVC2_SET         (0x09)
#define WR_LVC2_SET         (RD_LVC2_SET | 0x80)
// {LVC[23:16]}
#define RD_LVC1_SET         (0x0a)
#define WR_LVC1_SET         (RD_LVC1_SET | 0x80)
// {LVC[15:8]}
#define RD_LVC0_SET         (0x0b)
#define WR_LVC0_SET         (RD_LVC0_SET | 0x80)
// {LVC[7:0]}


#define RD_RSC3_SET         (0x0c)
#define WR_RSC3_SET         (RD_RSC3_SET | 0x80)
// {RSC[31:24]}
#define RD_RSC2_SET         (0x0d)
#define WR_RSC2_SET         (RD_RSC2_SET | 0x80)
// {RSC[23:16]}
#define RD_RSC1_SET         (0x0e)
#define WR_RSC1_SET         (RD_RSC1_SET | 0x80)
// {RSC[15:8]}
#define RD_RSC0_SET         (0x0f)
#define WR_RSC0_SET         (RD_RSC0_SET | 0x80)
// {RSC[15:8]}


#define RD_PRC1_SET         (0x10)
#define WR_PRC1_SET         (RD_PRC1_SET | 0x80)
// {PRC[15:8]}
#define RD_PRC0_SET         (0x11)
#define WR_PRC0_SET         (RD_PRC0_SET | 0x80)
// {PRC[7:0]}


#define RD_STC1_READ        (0x12)
// {STC[15:8]}
#define RD_STC0_READ        (0x13)
// {STC[7:0]}


#define RD_LVC3_READ        (0x14)
// {LVC[31:24]}
#define RD_LVC2_READ        (0x15)
// {LVC[23:16]}
#define RD_LVC1_READ        (0x16)
// {LVC[15:8]}
#define RD_LVC0_READ        (0x17)
// {LVC[7:0]}


#define RD_MSTA_CON         (0x1c)
#define WR_MSTA_CON         (RD_MSTA_CON | 0x80)

#define RD_IVS_READ         (0x1d)
#define RD_IVS_SET1         (0x1e)
#define WR_IVS_SET1         (0x1e | 0x80)
#define RD_IVS_SET0         (0x1f)
#define WR_IVS_SET0         (0x1f | 0x80)

#define RD_PLVD_CON         (0x20)
#define WR_PLVD_CON         (RD_PLVD_CON | 0x80)

#define RD_PWR_SCON1        (0x21)
#define WR_PWR_SCON1        (RD_PWR_SCON1 | 0x80)

#define RD_PWR_SCON2        (0x22)
#define WR_PWR_SCON2        (RD_PWR_SCON2 | 0x80)

#define RD_WLDO_CON        (0x23)
#define WR_WLDO_CON        (RD_WLDO_CON | 0x80)

#define RD_PWR_CON0_KEEP   (0x24)
#define WR_PWR_CON0_KEEP   (RD_PWR_CON0_KEEP | 0x80)

//
////////////////////////////////////////////////////////////
//
//  low power mode defines
//
////////////////////////////////////////////////////////////
#define LP_OSC_DIV(z)  (z > 32768 ? 64 : 1)
#define LP_FREQ(z)     (z / LP_OSC_DIV(z))
#define LP_nS(z)       (1000000000L / LP_FREQ(z))

// stable time(uS)
#define Tstb0   100L
#define Tstb1   100L
#define Tstb2   100L
#define Tstb3   1000L
#define Tstb4   100L
#define Tstb5   600L
#define Tstb6   900L

#define LP_TN(x,z) ((x * 1000L) / LP_nS(z))

#define Nstb0(z)       LP_TN(Tstb0,z)
#define Nstb1(z)       LP_TN(Tstb1,z)
#define Nstb2(z)       LP_TN(Tstb2,z)
#define Nstb3(z)       LP_TN(Tstb3,z)
#define Nstb4(z)       LP_TN(Tstb4,z)
#define Nstb5(z)       LP_TN(Tstb5,z)
#define Nstb6(z)       LP_TN(Tstb6,z)

extern u8 LP_NK(u16 x);
// #define LP_NK(x)   (x > 16384 ? 15 : \
//                     x > 8192  ? 14 : \
//                     x > 4096  ? 13 : \
//                     x > 2048  ? 12 : \
//                     x > 1024  ? 11 : \
//                     x >  512  ? 10 : \
//                     x >  256  ?  9 : \
//                     x >  128  ?  8 : \
//                     x >   64  ?  7 : \
//                     x >   32  ?  6 : \
//                     x >   16  ?  5 : \
//                     x >    8  ?  4 : \
//                     x >    4  ?  3 : \
//                     x >    2  ?  2 : \
//                     x >    1  ?  1 : 0)
//
#define Kstb0(z)   (LP_NK(Nstb0(z)))
#define Kstb1(z)   (LP_NK(Nstb1(z)))
#define Kstb2(z)   (LP_NK(Nstb2(z)))
#define Kstb3(z)   (LP_NK(Nstb3(z)))
#define Kstb4(z)   (LP_NK(Nstb4(z)))
#define Kstb5(z)   (LP_NK(Nstb5(z)))
#define Kstb6(z)   (LP_NK(Nstb6(z)))

#define pd_con_init_rtc                         \
	    /*              1bit RW  */ ( 0<<15)  | \
        /* BGB keep     1bit RW  */ ( 0<<14)  | \
        /*              1bit RW  */ ( 0<<13)  | \
        /*              1bit RW  */ ( 0<<12)  | \
        /* PLL keep     1bit RW  */ ( 0<<11)  | \
        /* BT OSC keep  1bit RW  */ ( 0<<10)  | \
        /*              1bit RW  */ ( 0<<9 )  | \
        /* PLL CLK keep 1bit RW  */ ( 0<<8 )  | \
        /* PMU PND      1bit RO  */ ( 0<<7 )  | \
        /* SPI KIST     1bit WO  */ ( 0<<5 )  | \
        /* SPI BUSY     1bit RO  */ ( 0<<1 )  | \
        /* CS           1bit RW  */ ( 0<<0 )

#define pd_con_init_bt                          \
	    /*              1bit RW  */ ( 0<<15)  | \
        /* BGB keep     1bit RW  */ ( 0<<14)  | \
        /*              1bit RW  */ ( 0<<13)  | \
        /*              1bit RW  */ ( 0<<12)  | \
        /* PLL keep     1bit RW  */ ( 0<<11)  | \
        /* BT OSC keep  1bit RW  */ ( 1<<10)  | \
        /*              1bit RW  */ ( 0<<9 )  | \
        /* PLL CLK keep 1bit RW  */ ( 0<<8 )  | \
        /* PMU PND      1bit RO  */ ( 0<<7 )  | \
        /* SPI KIST     1bit WO  */ ( 0<<5 )  | \
        /* SPI BUSY     1bit RO  */ ( 0<<1 )  | \
        /* CS           1bit RW  */ ( 0<<0 )

#define pd_con0_init                            \
        /* TO PND       1bit RO  */ ( 0<<7 )  | \
        /* CLR TO PND   1bit RW  */ ( 1<<6 )  | \
        /* PD_RUN FLAG  1bit RO  */ ( 0<<5 )  | \
        /* TO EN        1bit RW  */ ( 1<<4 )  | \
        /* DIS EN       2bit RW  */ ( 3<<2 )  | \
        /* PD MD        1bit RW  */ ( 0<<1 )  | \
        /* PD_EN        1bit RW  */ ( 1<<0 )

#define pd_con1_init_rtcl                       \
        /* POR FLAG     1bit RO  */ ( 0<<7 )  | \
        /* CLR POR PND  1bit RW  */ ( 1<<6 )  | \
        /* CK DIV       2bit RW  */ ( 0<<4 )  /* 0:div0 1:div4 2:div16 3:div64 */  | \
        /* PU SLOW      1bit RW  */ ( 0<<3 )  | \
        /* CK SEL       3bit RW  */ ( 3<<0 )  /* 0:RC 1:BT 2:RTCH 3:RTCL 4:LRC 32K */


#define pd_con1_init_rtch                      \
        /* POR FLAG     1bit RO  */ ( 0<<7 )  | \
        /* CLR POR PND  1bit RW  */ ( 1<<6 )  | \
        /* CK DIV       2bit RW  */ ( 3<<4 )  /* 0:div0 1:div4 2:div16 3:div64 */  | \
        /* PU SLOW      1bit RW  */ ( 0<<3 )  | \
        /* CK SEL       3bit RW  */ ( 2<<0 )  /* 0:RC 1:BT 2:RTCH 3:RTCL 4:LRC 32K */

#define pd_con1_init_bt                         \
        /* POR FLAG     1bit RO  */ ( 0<<7 )  | \
        /* CLR POR PND  1bit RW  */ ( 1<<6 )  | \
        /* CK DIV       2bit RW  */ ( 3<<4 )  /* 0:div0 1:div4 2:div16 3:div64 */  | \
        /* PU SLOW      1bit RW  */ ( 0<<3 )  | \
        /* CK SEL       3bit RW  */ ( 1<<0 )  /* 0:RC 1:BT 2:RTCH 3:RTCL 4:LRC 32K */

#define pd_con1_init_lrc_32k                      \
        /* POR FLAG     1bit RO  */ ( 0<<7 )  | \
        /* CLR POR PND  1bit RW  */ ( 1<<6 )  | \
        /* CK DIV       2bit RW  */ ( 0<<4 )  /* 0:div0 1:div4 2:div16 3:div64 */  | \
        /* PU SLOW      1bit RW  */ ( 0<<3 )  | \
        /* CK SEL       3bit RW  */ ( 4<<0 )  /* 0:RC 1:BT 2:RTCH 3:RTCL 4:LRC 32K */

#define pd_con2_init                           \
		/* SOFT USE     1bit RW  */ ( 0<<7 ) | \
		/* SOFT USE     1bit RW  */ ( 0<<6 ) | \
		/* SOFT USE     1bit RW  */ ( 0<<5 ) | \
		/* SOFT USE     1bit RW  */ ( 0<<4 ) | \
		/* SOFT USE     1bit RW  */ ( 0<<3 ) | \
		/* SOFT USE     1bit RW  */ ( 0<<2 ) | \
		/* SOFT USE     1bit RW  */ ( 0<<1 ) | \
		/* SOFT USE     1bit RW  */ ( 1<<0 )

#define pd_con3_init                            \
        /* RC EN        1bit RW  */ ( 0<<7 )  | \
        /* MRTC33       1bit RW  */ ( 1<<6 )  | \
        /* PW GATE      1bit RW  */ ( 1<<5 )  | \
        /* MLDO33       1bit RW  */ ( 1<<4 )  | \
        /* DCDC 15      1bit RW  */ ( 0<<3 )  | \
        /* LDO 15       1bit RW  */ ( 0<<2 )  | \
        /* LDO 3312     1bit RW  */ ( 1<<1 )  | \
        /* LDO 1512     1bit RW  */ ( 0<<0 )

#define pd_con4_init(x,y)                        \
        /* STB1 SET     4bit RW  */ ( x <<4 )  | \
        /* STB0 SET     4bit RW  */ ( y <<0 )


#define pd_con5_init(x,y)                        \
        /* STB3 SET     4bit RW  */ ( x <<4 )  | \
        /* STB2 SET     4bit RW  */ ( y <<0 )

#define pd_con6_init(x,y)                        \
        /* STB5 SET     4bit RW  */ ( x <<4 )  | \
        /* STB4 SET     4bit RW  */ ( y <<0 )

#define pd_con7_init(x)                         \
        /* STB6 SET     4bit RW  */ ( x <<0 )

#define LVC_INIT        k_total

#define pd_con8_init                            \
        /* LVC[31:24]   8bit WO */ ((LVC_INIT >> 24) & 0xff)
#define pd_con9_init                            \
        /* LVC[23:16]   8bit WO */ ((LVC_INIT >> 16) & 0xff)
#define pd_cona_init                            \
        /* LVC[15:08]   8bit WO */ ((LVC_INIT >>  8) & 0xff)
#define pd_conb_init                            \
        /* LVC[07:00]   8bit WO */ ((LVC_INIT )      & 0xff)

#define RSC_INIT       k_rsc

#define pd_conc_init                            \
        /* RSC[31:24]   8bit WO */ ((RSC_INIT >> 24) & 0xff)
#define pd_cond_init                            \
        /* RSC[23:16]   8bit WO */ ((RSC_INIT >> 16) & 0xff)
#define pd_cone_init                            \
        /* RSC[15:08]   8bit WO */ ((RSC_INIT >>  8) & 0xff)
#define pd_conf_init                            \
        /* RSC[07:00]   8bit WO */ ((RSC_INIT )      & 0xff)

#define PRP_INIT        k_prp

#define pd_con10_init                           \
        /* PRP[15:08]   8bit WO */ ((PRP_INIT >>  8) & 0xff)
#define pd_con11_init                           \
        /* PRP[07:00]   8bit WO */ ((PRP_INIT )      & 0xff)

#define pd_con12_init                           \
        /* STC[15:8]    8bit RO  */ ( 0<<0 )

#define pd_con13_init                           \
        /* STC[07:0]    8bit RO  */ ( 0<<0 )

#define pd_con14_init                           \
        /* LVC[31:24]   8bit RO  */ ( 0<<0 )

#define pd_con15_init                           \
        /* LVC[23:16]   8bit RO  */ ( 0<<0 )

#define pd_con16_init                           \
        /* LVC[15:8]    8bit RO  */ ( 0<<0 )

#define pd_con17_init                           \
        /* LVC[07:0]    8bit RO  */ ( 0<<0 )

#define pd_con1c_init                           \
        /* RTC RST SRC  3bit RO  */ ( 0<<5 )  | \
        /*              1bit RW  */ ( 0<<0 )

#define pd_con1e_init                           \
        /* PWR VLD  EN  1bit WO  */ ( 0<<7 )  | \
        /* PW DIS EN    1bit WO  */ ( 0<<6 )  | \
        /* RST MASK     1bit WO  */ ( 0<<5 )  | \
        /* MLD012 SW ON 1bit WO  */ ( 0<<4 )  | \
        /* ALL VLD EN   1bit WO  */ ( 0<<3 )  | \
        /* ANA VLD EN   1bit WO  */ ( 0<<2 )  | \
        /* CLK VLD EN   1bit WO  */ ( 0<<1 )  | \
        /* MVRAM PWR EN 1bit WO  */ ( 0<<0 )

#define pd_con1f_init                           \
        /* PWR VLD  DIS 1bit WO  */ ( 0<<7 )  | \
        /* PW DIS DIS   1bit WO  */ ( 0<<6 )  | \
        /* RST MASK     1bit WO  */ ( 0<<5 )  | \
        /* MLD012 SW OFF1bit WO  */ ( 0<<4 )  | \
        /* ALL VLD DIS  1bit WO  */ ( 0<<3 )  | \
        /* ANA VLD DIS  1bit WO  */ ( 0<<2 )  | \
        /* CLK VLD DIS  1bit WO  */ ( 0<<1 )  | \
        /* MVRAM PWR DIS1bit WO  */ ( 0<<0 )

// RTC LVD
#define pd_con20_init                           \
        /* PLVD LOAD    1bit RW  */ ( 0<<4 )  | \
        /* PLVD EN      1bit RW  */ ( 1<<3 )  | \
        /* PLVD OE      1bit RW  */ ( 1<<2 )  | \
        /* PLVD_LEVEL   2bit RW  */ ( 3<<0 )

#define pd_con21_init                           \
        /* DVDDA SW     1bit RW  */ ( 0<<7 )  | \
        /* FAST CAG     1bit RW  */ ( 0<<6 )  | \
        /* S1 VBAT      1bit RW  */ ( 0<<5 )  | \
        /* S0 VBAT      1bit RW  */ ( 0<<4 )  | \
        /* BTAVDD EN    1bit RW  */ ( 1<<3 )  | \
        /* DVDDA 1512 EN1bit RW  */ ( 1<<2 )  | \
        /* WLDO12 EN    1bit RW  */ ( 1<<1 )  | \
        /* WFM30 EN     1bit RW  */ ( 1<<0 )

#define pd_con22_init                           \
        /*              1bit RW  */ ( 0<<5 )  | \
        /* D2SH DIS     1bit RW  */ ( 0<<4 )  | \
        /* VCM DET      1bit RW  */ ( 1<<3 )  | \
        /* RTC33 LEVEL  3bit RW  */ ( 1<<0 )

// 进poweroff要设置下
#define pd_con23_init                           \
        /* WLDO EN      1bit RW  */ ( 1<<4 )  | \
        /* WLDO LVL LOW 2bit RW  */ ( 0<<2 )  | \
        /* WLDO LEVEL   2bit RW  */ ( 3<<0 )

#define pd_con24_init                           \
        /* RC EN KEEP   1bit RW  */ ( 0<<7 )  | \
        /* MRTC33 KEEP  1bit RW  */ ( 1<<6 )  | \
        /* PW GATE KEEP 1bit RW  */ ( 0<<5 )  | \
        /* MLDO33 KEEP  1bit RW  */ ( 0<<4 )  | \
        /* DCDC 15 KEEP 1bit RW  */ ( 0<<3 )  | \
        /* LDO 15 KEEP  1bit RW  */ ( 0<<2 )  | \
        /* MLDO12 KEEP  1bit RW  */ ( 0<<1 )  | \
        /* LDO 1512 KEEP1bit RW  */ ( 0<<0 )


//
////////////////////////////////////////////////////////////
//
//  P33
//
////////////////////////////////////////////////////////////

#define P33_ANA_CON0       0x00
#define p33_con0_init                             \
        /* RC EN           1bit RW  */ ( 0<<7)  | \
        /* MRTC33          1bit RW  */ ( 0<<6)  | \
        /* PW GATE         1bit RW  */ ( 0<<5)  | \
        /* MLDO33          1bit RW  */ ( 0<<4)  | \
        /* DCDC 15         1bit RW  */ ( 0<<3 ) | \
        /* LDO  15         1bit RW  */ ( 0<<2 ) | \
        /* MVBG            1bit RW  */ ( 0<<1 ) | \
        /* LDO 1512        1bit RW  */ ( 0<<0 )

#define P33_ANA_CON1       0x01
#define p33_con1_init                             \
        /* DVDDA SHORT     1bit RW  */ ( 0<<7)  | \
        /* WVBG            1bit RW  */ ( 0<<6)  | \
        /* WVDDIO          1bit RW  */ ( 0<<5)  | \
        /* WBTLDO          1bit RW  */ ( 0<<4)  | \
        /* WLDO06          1bit RW  */ ( 0<<3 ) | \
        /* DVDDA 1512 EN   1bit RW  */ ( 0<<2 ) | \
        /* WLDO12          1bit RW  */ ( 0<<1 ) | \
        /* reserved        1bit RW  */ ( 0<<0 )

#define P33_ANA_CON2       0x02
#define p33_con2_init                             \
        /* WVBG LEV        3bit RW  */ ( 0<<5 ) |       /*0(1.056V)   1(1.090V)   2(1.124V)   3(1.158V)*/ \
												        /*4(1.193V)   5(1.227V)   6(1.262V)   7(1.296V)*/ \
	   	/* D2SH DIS        1bit RW  */ ( 0<<4 ) | \
        /* VCM_DET         1bit RW  */ ( 0<<3 ) | \
        /* RTCVDD          3bit RW  */ ( 0<<0 )         /*0(3.52V)   1(3.33V)   2(3.15V)   3(2.96V)*/
/*4(2.86V)   5(2.72V)   6(2.57V)   7(2.47V)*/

#define P33_ANA_CON3       0x03
#define p33_con3_init                             \
        /* reserved        6bit RW  */ ( 0<<2 ) | \
        /* HD WVBG         2bit RW  */ ( 0<<0 )         /*0(min)   1()   2()   3(max)*/

#define P33_CHG_CON        0x04
#define p33_con4_init                             \
        /* CHG LEV         4bit RW  */ ( 0<<4 ) |       /*0 (3.85V)   1 (3.9V)   2 (3.95V)   3 (4.0V)*/ \
												        /*4 (4.05V)   5 (4.1V)   6 (4.15V)   7 (4.2V)*/ \
												        /*8 (4.25V)   9 (4.3V)   10(4.35V)   11(4.4V)*/ \
												        /*12(4.45V)   13(4.5V)   14(4.55V)   15(4.6V)*/ \
	   	/* CHG CURRENT     2bit RW  */ ( 0<<2 ) | \     /*0(min)   1()   2()   3(max)*/ \
        /* CHG DET EN      1bit RW  */ ( 0<<1 ) | \
        /* CHG EN          1bit RW  */ ( 0<<0 )

#define P33_PLVD_CON       0x05
#define p33_con5_init                             \
        /* LVD_PND		   1bit RW  */ ( 0<<7)  | \
        /* PND_CLR		   1bit RW  */ ( 0<<6)  | \
        /* reserved		   1bit RW  */ ( 0<<4 ) | \
        /* PLVD LEV        2bit RW  */ ( 0<<2 ) | \     /*0(1.8V)   1(1.9V)   2(2.0V)   3(2.1V)*/ \
        /* PLVD LOAD       1bit RW  */ ( 0<<1 ) | \
        /* PLVD EN         1bit RW  */ ( 0<<0 )

#define P33_VLVD_CON       0x06
#define p33_con6_init                             \
        /* LVD_PND		   1bit RW  */ ( 0<<7)  | \
        /* PND_CLR		   1bit RW  */ ( 0<<6)  | \
        /* VLVD LEV        2bit RW  */ ( 0<<3 ) | \     /*0(1.8V)   1(1.9V)   2(2.0V)   3(2.1V)*/ \
        /* VLVD OE		   1bit RW  */ ( 0<<2 ) | \
        /* VLVD PS         1bit RW  */ ( 0<<1 ) | \
        /* VLVD EN         1bit RW  */ ( 0<<0 )

#define P33_RST_CON        0x07
#define p33_con7_init                             \
        /* reserved		   3bit RW  */ ( 0<<5 ) | \
        /* SYS RST         1bit RW  */ ( 0<<4 ) | \
        /* LONG PRESS RST  1bit RW  */ ( 0<<3 ) | \
        /* VCM	   		   1bit RW  */ ( 0<<2 ) | \
        /* RTC LVD    	   1bit RW  */ ( 0<<1 ) | \
        /* RTC POR         1bit RW  */ ( 0<<0 )

#define P33_LRC_CON0       0x08
#define p33_con8_init      0x00

#define P33_LRC_CON1       0x09
#define p33_con9_init      0x00

#define P33_AUX_CON0       0x0a
#define p33_cona_init                             \
        /* reserved		   4bit RW  */ ( 0<<4 ) | \
        /* VLVD WKUP EN	   1bit RW  */ ( 0<<3 ) | \
        /* VLVD RST EN	   1bit RW  */ ( 0<<2 ) | \
        /* PLVD WKUP EN    1bit RW  */ ( 0<<1 ) | \
        /* PLVD RST EN     1bit RW  */ ( 0<<0 )

#define P33_ANA_KEEP       0x0c
#define p33_conc_init                             \
        /* RC EN           1bit RW  */ ( 0<<7 ) | \
        /* MRTC33		   1bit RW  */ ( 0<<6 ) | \
        /* PW GATE		   1bit RW  */ ( 0<<5 ) | \
        /* MLDO33          1bit RW  */ ( 0<<4 ) | \
        /* DCDC 15         1bit RW  */ ( 0<<3 ) | \
        /* LDO 15	   	   1bit RW  */ ( 0<<2 ) | \
        /* MVBG    	       1bit RW  */ ( 0<<1 ) | \
        /* LDO 1512        1bit RW  */ ( 0<<0 )

#define P33_VLD_KEEP       0x0d
#define p33_cond_init                             \
        /* reserved		   5bit RW  */ ( 0<<3 ) | \
        /* OSH DIFF	       1bit RW  */ ( 0<<2 ) | \
        /* RTC WKUP        1bit RW  */ ( 0<<1 ) | \
        /* SOURCE CLOCK    1bit RW  */ ( 0<<0 )


#endif
