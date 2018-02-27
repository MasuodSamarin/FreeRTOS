#include "stdarg.h"
/* #include "debug_hw.h" */
#include "debug_interface.h"

//#define DEBUG_DEBUG
#ifdef DEBUG_DEBUG
#define debug_puts              puts
#define debug_printf            printf
#define debug_put8              put_u8hex0
#define debug_put16             put_u16hex
#define debug_put32             put_u32hex
#else
#define debug_puts(...)
#define debug_printf(...)
#define debug_put8
#define debug_put16
#define debug_put32
#endif  /*DEBUG_DEBUG*/

static u16 flag;
static u16 flag1;

static void __debug_wr_en()
{
    DEBUG_WR_EN |= BIT(0);
}

static void __debug_wr_dis()
{
    DEBUG_WR_EN &= ~BIT(0);
}


static void __cpu_protect_ram(u32 start, u32 end, u8 range)
{
    DSP_BF_CON &= ~BIT(10);

    DSP_EX_LIMH = (volatile unsigned long)end;
    DSP_EX_LIML = (volatile unsigned long)start;

    if (range) {
        DSP_BF_CON |= BIT(11);
    } else {
        DSP_BF_CON &= ~BIT(11);
    }
    DSP_BF_CON |= BIT(10);
}

static void __cpu_protect_ram_suspend(void)
{
    //restore sfr setting
    flag = DSP_BF_CON;
    //disable if enable
    if (flag & BIT(10)) {
        DSP_BF_CON &= ~BIT(10);
    }
    debug_printf("flag : %x\n", flag);
}

static void __cpu_protect_ram_resume(void)
{
    ASSERT(flag != -1, "\n----use when no suspend %s %x %x\n", __func__, flag, DSP_BF_CON);

    if (flag & BIT(10)) {
        DSP_BF_CON |= BIT(10);
    }
    flag = -1;
}

static void __prp_protect_ram(u32 begin, u32 end, u8 range)
{
    DSP_BF_CON &= ~BIT(8);

    PRP_EX_LIMH = (volatile unsigned long)end;
    PRP_EX_LIML = (volatile unsigned long)begin;

    if (range) {
        DSP_BF_CON |= BIT(9);
    } else {
        DSP_BF_CON &= ~BIT(9);
    }
    DSP_BF_CON |= BIT(8);
}

static void __prp_protect_ram_suspend(void)
{
    //restore sfr setting
    flag1 = DSP_BF_CON;
    //disable if enable
    if (flag1 & BIT(8)) {
        DSP_BF_CON &= ~BIT(8);
    }
    debug_printf("flag1 : %x\n", flag1);
}

static void __prp_protect_ram_resume(void)
{
    ASSERT(flag1 != -1, "\n----use when no suspend %s %x %x\n", __func__, flag1, DSP_BF_CON);

    if (flag1 & BIT(8)) {
        DSP_BF_CON |= BIT(8);
    }
    flag1 = -1;
}

/* static void __pc_protect_ram(u32 start, u32 end) */
/* { */
/* DSP_PC_LIMH = (volatile unsigned long)end; */
/* DSP_PC_LIML = (volatile unsigned long)start; */
/* } */

static void debug_ioctrl(u8 ctrl, ...)
{
    va_list argptr;
    va_start(argptr, ctrl);

    switch (ctrl) {
    case PC_WR_RAM:
        __cpu_protect_ram(va_arg(argptr, int), va_arg(argptr, int), va_arg(argptr, u8));
        break;
    case PRP_WR_RAM:
        __prp_protect_ram(va_arg(argptr, int), va_arg(argptr, int), va_arg(argptr, u8));
        break;
    }

    va_end(argptr);
}

static void debug_suspend(void)
{
    __cpu_protect_ram_suspend();
    __prp_protect_ram_suspend();
}

static void debug_resume(void)
{
    __prp_protect_ram_resume();
    __prp_protect_ram_resume();
}

static struct debug_driver debug_drv_ins = {
    .on         = __debug_wr_en,
    .off        = __debug_wr_dis,

    .suspend    = debug_suspend,
    .resume     = debug_resume,

    .ioctrl     = debug_ioctrl,
};
REGISTER_DEBUG_DRIVER(debug_drv_ins);
