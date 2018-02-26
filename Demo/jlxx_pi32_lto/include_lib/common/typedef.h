/*********************************************************************************************
    *   Filename        : typedef.h

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2016-07-12 10:53

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/
#ifndef _TYPEDEF_H_
#define _TYPEDEF_H_

#include "asm_type.h"

#if defined(__GNUC__)

///<locate code to x segment ever exist
#define SEC_USED(x)     __attribute__((section(#x),used))
///<locate code to x segment optimized by dependency
#define SEC(x)          __attribute__((section(#x)))
///<locate data to x segment
#define AT(x)           __attribute__((section(#x)))
#define SET(x)          __attribute__((x))
#define ALIGNED(x)	    __attribute__((aligned(x)))
#define _GNU_PACKED_	__attribute__((packed))
#define _NOINLINE_	    __attribute__((noinline))
#define _INLINE_	    __attribute__((always_inline))
#define _WEAK_	        __attribute__((weak))
#define _WEAKREF_	    __attribute__((weakref))

#else

#define sec(x)
#define AT(x)
#define SET(x)
#define ALIGNED(x)
#define _GNU_PACKED_
#define _NOINLINE_
#define _INLINE_
#define _WEAK_
#define _WEAKREF_

#endif

//#define     LITTLE_ENDIAN
//
//#ifdef LITTLE_ENDIAN
//#define ntohl(x) (u32)((x>>24)|((x>>8)&0xff00)|(x<<24)|((x&0xff00)<<8))
//#define ntoh(x) (u16)((x>>8&0x00ff)|x<<8&0xff00)
//#define NTOH(x) (x) = ntoh(x)
//#define NTOHL(x) (x) = ntohl(x)
//#define LD_WORD(ptr)        (u16)(*(u16*)(u8*)(ptr))
//#define LD_DWORD(ptr)        (u32)(*(u32*)(u8*)(ptr))
//#define ST_WORD(ptr,val)    *(u16*)(u8*)(ptr)=(u16)(val)
//#define ST_DWORD(ptr,val)    *(u32*)(u8*)(ptr)=(u32)(val)
//#else
//#define ntohl(x) (x)
//#define ntoh(x) (x)
//#define NTOH(x) (x) = ntoh(x)
//#define NTOHL(x) (x) = ntohl(x)
//#endif



#define FALSE    0
#define TRUE    1
#define false    0
#define true    1

#ifndef NULL
#define NULL    0
#endif // NULL

#define _banked_func
#define _xdata
#define _data
#define _root
#define _no_init
#define my_memset memset
#define my_memcpy memcpy
#define _static_  static

#define     BIT(n)              (1UL << (n))
#define     BitSET(REG,POS)     ((REG) |= (1L << (POS)))
#define     BitCLR(REG,POS)     ((REG) &= (~(1L<< (POS))))
#define     BitXOR(REG,POS)     ((REG) ^= (~(1L << (POS))))
#define     BitCHK_1(REG,POS)   (((REG) & (1L << (POS))) == (1L << (POS)))
#define     BitCHK_0(REG,POS)   (((REG) & (1L << (POS))) == 0x00)
#define     testBit(REG,POS)    ((REG) & (1L << (POS)))

#define     clrBit(x,y)         (x) &= ~(1L << (y))
#define     setBit(x,y)         (x) |= (1L << (y))


#define readb(addr)   *((unsigned char*)(addr))
#define readw(addr)   *((unsigned short *)(addr))
#define readl(addr)   *((unsigned long*)(addr))

#define writeb(addr, val)  *((unsigned char*)(addr)) = (val)
#define writew(addr, val)  *((unsigned short *)(addr)) = (u16)(val)
#define writel(addr, val)  *((unsigned long*)(addr)) = (val)

#define ALIGN_4BYTE(size)   ((size+3)&0xfffffffc)


#define CPU_BIG_EDIAN

#if defined(CPU_BIG_EDIAN)
#define __cpu_u16(lo, hi)  ((lo)|((hi)<<8))

#elif defined(CPU_LITTLE_EDIAN)
#define __cpu_u16(lo, hi)  ((hi)|((lo)<<8))
#else
#error "undefine cpu edian"
#endif

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

//
#define ARRAY_SIZE(array)  (sizeof(array)/sizeof(array[0]))
#define ARRAY_AND_SIZE(array)   array, (sizeof(array)/sizeof(array[0]))

#include "printf.h"

#if 1
#define ASSERT(a,...)   \
        do { \
            if(!(a)){ \
                printf("%s %d", __FILE__, __LINE__); \
                printf("ASSERT-FAILD: "#a"\n"__VA_ARGS__); \
                while(1); \
            } \
        }while(0);

#else
#define ASSERT(a,...)
#endif

#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#include "cpu.h"
#define SFR(sfr, start, len, dat) (sfr = (sfr & ~((~(0xffffffff << (len))) << (start))) | (((dat) & (~(0xffffffff << (len)))) << (start)))

int memcmp(const void *, const void *, long unsigned int);
void *memcpy(void *, const void *, long unsigned int);
void *memset(void *, int, long unsigned int);

#endif



