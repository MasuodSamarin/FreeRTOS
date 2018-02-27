#include "stdarg.h"
/* #include "debug_hw.h" */
#include "debug_interface.h"

/* #define DEBUG_ENABLE */
#include "debug_log.h"


static u32 flag;
static u32 suspend_cnt;
static u32 flag1;
static u32 suspend_cnt1;

static void __debug_wr_en()
{
    JL_DEBUG->WR_EN |= BIT(0);
}

static void __debug_wr_dis()
{
    JL_DEBUG->WR_EN &= ~BIT(0);
}


static void __cpu_protect_ram(u32 start, u32 end, u8 range)
{
    JL_DEBUG->DSP_BF_CON &= ~BIT(10);

    JL_DEBUG->DSP_EX_LIMH = (volatile unsigned long)end;
    JL_DEBUG->DSP_EX_LIML = (volatile unsigned long)start;

    if (range) {
        JL_DEBUG->DSP_BF_CON |= BIT(11);
    } else {
        JL_DEBUG->DSP_BF_CON &= ~BIT(11);
    }

    JL_DEBUG->DSP_BF_CON |= BIT(10);
    suspend_cnt = 0;
}

static void __cpu_protect_ram_suspend(void)
{
    //restore sfr setting
    flag = JL_DEBUG->DSP_BF_CON;
    suspend_cnt++;
    //disable if enable
    if (flag & BIT(10)) {
        JL_DEBUG->DSP_BF_CON &= ~BIT(10);
    }
    log_info("flag : %x\n", flag);
}

static void __cpu_protect_ram_resume(void)
{
    ASSERT(suspend_cnt, "\n----use when no suspend %s %x %x\n", __func__, suspend_cnt, JL_DEBUG->DSP_BF_CON);

    if (flag & BIT(10)) {
        JL_DEBUG->DSP_BF_CON |= BIT(10);
    }
    /* flag = -1; */
    suspend_cnt--;
}

static void __prp_protect_ram(u32 begin, u32 end, u8 range)
{
    JL_DEBUG->DSP_BF_CON &= ~BIT(8);

    JL_DEBUG->PRP_EX_LIMH = (volatile unsigned long)end;
    JL_DEBUG->PRP_EX_LIML = (volatile unsigned long)begin;

    if (range) {
        JL_DEBUG->DSP_BF_CON |= BIT(9);
    } else {
        JL_DEBUG->DSP_BF_CON &= ~BIT(9);
    }
    JL_DEBUG->DSP_BF_CON |= BIT(8);
    suspend_cnt1 = 0;
}

static void __prp_protect_ram_suspend(void)
{
    //restore sfr setting
    flag1 = JL_DEBUG->DSP_BF_CON;
    suspend_cnt1++;
    //disable if enable
    if (flag1 & BIT(8)) {
        JL_DEBUG->DSP_BF_CON &= ~BIT(8);
    }
    log_info("flag1 : %x\n", flag1);
}

static void __prp_protect_ram_resume(void)
{
    ASSERT(suspend_cnt1, "\n----use when no suspend %s %x %x\n", __func__, suspend_cnt1, JL_DEBUG->DSP_BF_CON);

    if (flag1 & BIT(8)) {
        JL_DEBUG->DSP_BF_CON |= BIT(8);
    }
    /* flag1 = -1; */
    suspend_cnt1--;
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
    __cpu_protect_ram_resume();
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
