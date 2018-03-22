/*
 * FreeRTOS Kernel V10.0.1
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <cpu.h>

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE. 
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/

#define configUSE_PREEMPTION		    0
#define configUSE_IDLE_HOOK			    1
#define configUSE_TICK_HOOK			    0
#define configCPU_CLOCK_HZ			    ( ( unsigned long ) 48000000L) /* Clock setup from main.c in the demo application. */
#define configTICK_RATE_HZ			    ( ( TickType_t ) 1000 )
#define configMAX_PRIORITIES		    ( 4 )
#define configMINIMAL_STACK_SIZE	    ( ( unsigned short ) 50 )
#define configTOTAL_HEAP_SIZE		    ( ( size_t ) ( 0x1000 ) )
#define configMAX_TASK_NAME_LEN		    ( 8 )
#define configUSE_TRACE_FACILITY	    0
#define configUSE_16_BIT_TICKS		    0
#define configIDLE_SHOULD_YIELD		    1

//user define 
#define configCHECK_FOR_STACK_OVERFLOW  1
#define configUSE_MALLOC_FAILED_HOOK    1
#define configUSE_LIST_DATA_INTEGRITY_CHECK_BYTES   1

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 		0
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet		0
#define INCLUDE_uxTaskPriorityGet		0
#define INCLUDE_vTaskDelete				1
#define INCLUDE_vTaskCleanUpResources	0
#define INCLUDE_vTaskSuspend			0
#define INCLUDE_vTaskDelayUntil			1
#define INCLUDE_vTaskDelay				1

#include "typedef.h"

// The following macro implementing the stub in kernel
//
#define traceMALLOC( a, b ) \
    log_info("[traceMALLOC] MALLOC pvAddress : 0x%x / uiSize : 0x%x", a, b)

#define traceTASK_CREATE( a ) \
        log_info("[traceTASK_CREATE] pcName %s", a->pcTaskName); \
        log_info("[traceTASK_CREATE] pxNewTCB :0x%x / pxNewTCB->pxTopOfStack 0x%x", a, a->pxTopOfStack); \
        log_info_hexdump((u8 *)a->pxTopOfStack, (u32)a - (u32)a->pxTopOfStack)

#define traceTASK_DELAY_UNTIL( a ) \
    log_info("[traceTASK_DELAY_UNTIL] : 0x%x", a)

// #define traceTASK_INCREMENT_TICK( a ) \
    // log_info("[traceTASK_INCREMENT_TICK] : 0x%x", a)

// #define traceMOVED_TASK_TO_READY_STATE( a ) \
        // log_info("[traceMOVED_TASK_TO_READY_STATE] 0x%x", a->xStateListItem.pvContainer); \

// #define tracePOST_MOVED_TASK_TO_READY_STATE( a ) \
        // log_info("[tracePOST_MOVED_TASK_TO_READY_STATE] 0x%x", a->xStateListItem.pvContainer); \

#define configASSERT( a ) \
        do { \
            if(! ( a ) ){ \
                printf("%s %d", __FILE__, __LINE__); \
                printf("ASSERT-FAILD: "#a"\n"); \
                while( 1 ); \
            } \
        }while( 0 );


#endif /* FREERTOS_CONFIG_H */
