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

/*
	JL Start from V1.0.0
		
	+ usCriticalNesting now has a volatile qualifier.
*/

/* Standard includes. */
#include <stdlib.h>
#include <signal.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

#include "irq_interface.h"
#include "interrupt.h"
#define LOG_TAG         "[Port]"
#define LOG_INFO_ENABLE     
#include "debug.h"

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the MSP430 port.
 *----------------------------------------------------------*/

/* Constants required for hardware setup.  The tick ISR runs off the ACLK, 
not the MCLK. */
#define portACLK_FREQUENCY_HZ			( ( TickType_t ) 32768 )
#define portINITIAL_CRITICAL_NESTING	( ( uint16_t ) 10 )

/* We require the address of the pxCurrentTCB variable, but don't want to know
any details of its type. */
typedef void TCB_t;
extern volatile TCB_t * volatile pxCurrentTCB;

/* Most ports implement critical sections by placing the interrupt flags on
the stack before disabling interrupts.  Exiting the critical section is then
simply a case of popping the flags from the stack.  As mspgcc does not use
a frame pointer this cannot be done as modifying the stack will clobber all
the stack variables.  Instead each task maintains a count of the critical
section nesting depth.  Each time a critical section is entered the count is
incremented.  Each time a critical section is left the count is decremented -
with interrupts only being re-enabled if the count is zero.

usCriticalNesting will get set to zero when the scheduler starts, but must
not be initialised to zero as this will cause problems during the startup
sequence. */
volatile uint32_t usCriticalNesting = portINITIAL_CRITICAL_NESTING;
/*-----------------------------------------------------------*/

/* 
 * Macro to save a task context to the task stack.  This simply pushes all the 
 * general purpose br20 registers onto the stack, followed by the 
 * usCriticalNesting value used by the task.  Finally the resultant stack 
 * pointer value is saved into the task control block so it can be retrieved 
 * the next time the task executes.
 *
 */
#define portSAVE_CONTEXT()									\
	asm volatile (	"mov    sp, usp					\n\t"	\
					"push	{r3-r0}					\n\t"	\
					"pushs	{psr} 					\n\t"	\
					"movl   r0, 0x000c      		\n\t"	\
					"movh   r0, 0x7					\n\t"	\
					"memor  r0, 0, 1<<30			\n\t"	\
					"pushs	{rets, macch, maccl}    \n\t"	\
					"mov    r3, reti				\n\t"	\
					"push	{r3}					\n\t"	\
					"push	{r14-r4}				\n\t"	\
					"movl	r0, usCriticalNesting	\n\t"	\
					"movh	r0, usCriticalNesting	\n\t"	\
					"lw	    r1, r0				    \n\t"	\
					"push	{r1}					\n\t"	\
					"movl	r0, pxCurrentTCB		\n\t"	\
					"movh	r0, pxCurrentTCB		\n\t"	\
					"lw	    r1, r0				    \n\t"	\
                    "sw     sp, r1                  \n\t"   \
				);

/* 
 * Macro to restore a task context from the task stack.  This is effectively
 * the reverse of portSAVE_CONTEXT().  First the stack pointer value is
 * loaded from the task control block.  Next the value for usCriticalNesting
 * used by the task is retrieved from the stack - followed by the value of all
 * the general purpose msp430 registers.
 *
 * The bic instruction ensures there are no low power bits set in the status
 * register that is about to be popped from the stack.
 */
#define portRESTORE_CONTEXT()								\
	asm volatile (	"movl 	r0, pxCurrentTCB 		\n\t"	\
					"movh	r0, pxCurrentTCB 	 	\n\t"	\
					"lw     r1, r0 	 	            \n\t"	\
					"sw	    r1, sp				    \n\t"	\
					"pop	{r1}					\n\t"	\
					"movl 	r0, usCriticalNesting	\n\t"	\
					"movh 	r0, usCriticalNesting	\n\t"	\
					"sw	    r1, r0				    \n\t"	\
					"pop	{r14-r4}				\n\t"	\
					"pops	{rets,macch,maccl,reti,psr}\n\t"	\
					"pop	{r3-r0}					\n\t"	\
					"mov    usp, sp					\n\t"	\
					"mov	sp, ssp					\n\t"	\
					"rti							\n\t"	\
				);

/*-----------------------------------------------------------*/

/*
 * Sets up the periodic ISR used for the RTOS tick.  This uses timer 0, but
 * could have alternatively used the watchdog timer or timer 1.
 */
static void prvSetupTimerInterrupt( void );
/*-----------------------------------------------------------*/

/* 
 * Initialise the stack of a task to look exactly as if a call to 
 * portSAVE_CONTEXT had been called.
 * 
 * See the header file portable.h.
 */
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
	/* 
		Place a few bytes of known values on the bottom of the stack. 
		This is just useful for debugging and can be included if required.

		*pxTopOfStack = ( StackType_t ) 0x1111;
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0x2222;
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0x3333;
		pxTopOfStack--; 
	*/

	/* Next the general purpose registers r3 - r1. */
	*pxTopOfStack = ( StackType_t ) 0x33333333;     //r3
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x22222222;     //r2
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x11111111;     //r1
	pxTopOfStack--;

	/* When the task starts is will expect to find the function parameter in
	R0. */
	*pxTopOfStack = ( StackType_t ) pvParameters;   //r0
	pxTopOfStack--;

    //psr rets macch maccl
	*pxTopOfStack = ( StackType_t ) 0x5555aaaa;     //psr
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x5555aaaa;     //rets
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x5555aaaa;     //macch
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x5555aaaa;     //maccl
	pxTopOfStack--;

    //reti
	*pxTopOfStack = ( StackType_t ) pxCode;         //reti
	pxTopOfStack--;

	/* Next the general purpose registers r14 - r4. */
	*pxTopOfStack = ( StackType_t ) 0x14141414;     //r14
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x13131313;     //r13
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x12121212;     //r12
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x11111111;     //r11
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x10101010;     //r10
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x09090909;     //r9
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x08080808;     //r8
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x07070707;     //r7
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x06060606;     //r6
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x05050505;     //r5
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x04040404;     //r4
	pxTopOfStack--;

	/* The code generated by the mspgcc compiler does not maintain separate
	stack and frame pointers. The portENTER_CRITICAL macro cannot therefore
	use the stack as per other ports.  Instead a variable is used to keep
	track of the critical section nesting.  This variable has to be stored
	as part of the task context and is initially set to zero. */
	*pxTopOfStack = ( StackType_t ) portNO_CRITICAL_SECTION_NESTING;	   //usCriticalNesting

	/* Return a pointer to the top of the stack we have generated so this can
	be stored in the task control block for the task. */
	return pxTopOfStack;
}
/*-----------------------------------------------------------*/

void vPortYield_handler( void ) __attribute__ ( ( noreturn ) );

BaseType_t xPortStartScheduler( void )
{
    irq_handler_register(IRQ_SOFT0_IDX, vPortYield_handler, 0);
	/* Setup the hardware to generate the tick.  Interrupts are disabled when
	this function is called. */
	prvSetupTimerInterrupt();

	/* Restore the context of the first task that is going to run. */
	portRESTORE_CONTEXT();

	/* Should not get here as the tasks are now running! */
	return pdTRUE;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
	/* It is unlikely that the MSP430 port will get stopped.  If required simply
	disable the tick interrupt here. */
}
/*-----------------------------------------------------------*/

/*
 * Manual context switch called by portYIELD or taskYIELD.  
 *
 * The first thing we do is save the registers so we can use a naked attribute.
 */
void vPortYield_handler( void )
{
    log_info("@*");     
	/* We want the stack of the task being saved to look exactly as if the task
	was saved during a pre-emptive RTOS tick ISR.  Before calling an ISR the 
	msp430 places the status register onto the stack.  As this is a function 
	call and not an ISR we have to do this manually. */

	/* Save the context of the current task. */
	portSAVE_CONTEXT();

	/* Switch to the highest priority task that is ready to run. */
	vTaskSwitchContext();

	/* Restore the context of the new task. */
	portRESTORE_CONTEXT();
}

/*
 * Hardware initialisation to generate the RTOS tick.  This uses timer 0
 * but could alternatively use the watchdog timer or timer 1. 
 */
#define TIMER_CON       JL_TIMER1->CON
#define TIMER_CNT       JL_TIMER1->CNT
#define TIMER_PRD       JL_TIMER1->PRD
#define TIMER_VETOR     IRQ_TIME1_IDX

/* 
 * The interrupt service routine used depends on whether the pre-emptive
 * scheduler is being used or not.
 */

#if configUSE_PREEMPTION == 1

	/*
	 * Tick ISR for preemptive scheduler.  We can use a naked attribute as
	 * the context is saved at the start of vPortYieldFromTick().  The tick
	 * count is incremented after the context is saved.
	 */
	void prvTickISR( void ) __attribute__ ( ( naked ) );
	void prvTickISR( void )
	{
		/* Save the context of the interrupted task. */
		portSAVE_CONTEXT();

		/* Increment the tick count then switch to the highest priority task
		that is ready to run. */
		if( xTaskIncrementTick() != pdFALSE )
		{
			vTaskSwitchContext();
		}

		/* Restore the context of the new task. */
		portRESTORE_CONTEXT();
	}
    IRQ_REGISTER(TIMER_VETOR, prvTickISR);

#else

	/*
	 * Tick ISR for the cooperative scheduler.  All this does is increment the
	 * tick count.  We don't need to switch context, this can only be done by
	 * manual calls to taskYIELD();
	 */
	void prvTickISR( void );
	void prvTickISR( void )
	{
        log_info("*");     
		xTaskIncrementTick();
	}
    IRQ_REGISTER(TIMER_VETOR, prvTickISR);
#endif

static void prvSetupTimerInterrupt( void )
{
    u8 scale;

    IRQ_REQUEST(TIMER_VETOR, prvTickISR, 1);

    TIMER_CNT = 0;
    TIMER_PRD = portACLK_FREQUENCY_HZ / configTICK_RATE_HZ;

    scale = 0;
    TIMER_CON = (scale << 4) | BIT(0) | BIT(3);
}
/*-----------------------------------------------------------*/



	
