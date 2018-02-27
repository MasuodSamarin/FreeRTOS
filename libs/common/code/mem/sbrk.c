
/*******************************************************************************************
 File name:sbrk.c

 Version: 1.00

 Discription:  系统动态内存分配，申请物理空间函数。



 Author:yulin deng

 Email :flowingfeeze@163.com

 Date:星期三, 09-03-2014  03:32:34下午



 Copyright:(c)JIELI  2014  @ , All Rights Reserved.
*******************************************************************************************/

#include "typedef.h"

#include "pmalloc.h"
#include "my_malloc.h"
#include "mmu_tag.h"

/* #define DEBUG_ENABLE */
#include "debug_log.h"

extern ufat_t *vfree_by_addr(u32 vaddr, u32 size);
extern void vfill_tag(ufat_t *fat, u32 vaddr, u8 mod);

ufat_t *mem_fat ;

/*
 u32 mem_count ;
//static unsigned char mem[0x10000 * 10] AT(.mem_malloc) ALIGNED(512);
void*  sbrk(int increment)
{
    //static unsigned char * __init_addr ;
    unsigned char * ret ;



//    if((__init_addr >= mem + sizeof(mem)) && (increment > 0))
//    {
//        return (void*) - 1 ;
//    }

   if(mem_count <incrememnt)
   {

       vrealloc(mem_fat, )

   }

   if((malloc_mmu->rampos -  APP_BEGIN_ADDR + increment) >= (malloc_mmu->count * PAGE_SIZE))
   {
       mem_printf("vrelloc mem %x  %x  \r\n" ,malloc_mmu->rampos -  APP_BEGIN_ADDR + increment,malloc_mmu->count) ;

       if(malloc_mmu->count = vrealloc(malloc_mmu ,PAGE_COUNT(increment) ) )
       {
           mem_printf("increment = %x sbrk %x ", increment , ret) ;
         return -1 ;
       }

   }

    ret = malloc_mmu->rampos ;
    malloc_mmu->rampos += increment;

    mem_printf("increment = %x sbrk %x ", increment , ret) ;

    return ret  ;

}
#endif
*/

#define mem_heap_addr   (MMU_ADDR_BEGIN + STACK_TASK_MAX_SIZE)
//u8 mem_heap[4*1024*1024+0x20000];// AT(.mem_heap) ;
//extern u32 pm_usd_map[(MAX_SDRAM_SIZE/PAGE_SIZE)/32+1] ;
extern u32 pm_usd_map[(MAX_SDRAM_SIZE + ((32 * PAGE_SIZE) - 1)) / (32 * PAGE_SIZE)];

void *sbrk(void *addr, int incr)
{

//     printf_buf(pm_usd_map,sizeof(pm_usd_map)) ;


//    static   int  alloc_size = 0 ;

//    void *sbrk_ptr;
//    ufat_t *end ;
    //mem_printf("ssbrk:%08x %x \r\n", addr, incr) ;

    //sbrk_ptr = (void *)(mem_heap_addr + alloc_size);

    if (incr > 0) {
        mem_fat = vmalloc(incr) ;
        if (addr == 0) {
            addr = (void *)mem_heap_addr ;
        } else if (addr == (void *) - 1) {
            addr = (void *)(MMU_ADDR_BEGIN + MMU_MAX_SIZE);
        }
        if (mem_fat) {
            vfill_tag((ufat_t *)mem_fat, (u32)addr, 0) ;
            //mem_printf("sbrk reta=0x%x\n", addr) ;
            //mem_buf0(pm_usd_map,sizeof(pm_usd_map)) ;
            return (void *)addr;
        } else {
            //mem_printf("ssbrkFAILLLLL\n") ;
//            mem_buf0(pm_usd_map,sizeof(pm_usd_map)) ;
            return (void *) - 1 ;
        }
    } else {
        vfree_by_addr((u32)addr, -incr) ;
//mem_buf0(pm_usd_map,sizeof(pm_usd_map)) ;
        return 0 ;
    }

//#if 0
//    alloc_size += incr;
//    //if(alloc_size > sizeof(mem_heap))
//    if(mem_fat)
//    {
//        if(alloc_size > get_fat_count(mem_fat,&end)* PAGE_SIZE)
//        {
//
//            if(!vrealloc(mem_fat,alloc_size))
//            {
//                return -1 ;
//            }
//            vfill_tag(mem_fat,mem_heap_addr,0) ;
//            //mem_printf("ssbrk err \r\n") ;
//            //return -1;
//        }
//
//    }else
//    {
//        mem_fat = vmalloc(incr) ;
//        if(mem_fat)
//        {
//          vfill_tag(mem_fat,mem_heap_addr,0) ;
//        }else
//        {
//            return -1 ;
//        }
//
//    }
//
////    mem_printf("ssbrk %x \r\n" ,sbrk_ptr ) ;
//    return sbrk_ptr;
//#endif
}



