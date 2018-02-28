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

/*-----------------------------------------------------------
 * Characters on the LCD are used to simulate LED's.  In this case the 'ParTest'
 * is really operating on the LCD display.
 *-----------------------------------------------------------*/

/*
 * This demo is configured to execute on the br20 board.
 *
 * There is a single genuine on board LED referenced as LED 10.
 */

/* Standard includes. */
#include <signal.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Demo application includes. */
#include "partest.h"
#include "interrupt.h"
#include "irq_interface.h"
#include "sdk_cfg.h"

/* Constants required to access the "LED's".  The LED segments are turned on
and off to generate '*' characters. */
#define partstNUM_LEDS			( ( unsigned char ) 6 )

/*
 * Toggle the single genuine built in LED.
 */
static void prvToggleOnBoardLED( void );

/*-----------------------------------------------------------*/

void vParTestInitialise( void )
{
	/* Initialise the LCD hardware. */

	/* Used for the onboard LED. */

	// Setup port functions
	
	/* Initialise all segments to off. */
}
/*-----------------------------------------------------------*/

void vParTestSetLED( unsigned portBASE_TYPE uxLED, signed portBASE_TYPE xValue )
{
	/* Set or clear the output [in this case show or hide the '*' character. */
	if( uxLED < ( portBASE_TYPE ) partstNUM_LEDS )
	{
		vTaskSuspendAll();
		{
			if( xValue )
			{
                IO_DEBUG_1(A, uxLED);
			}
			else
			{
                IO_DEBUG_0(A, uxLED);
			}
		}
		xTaskResumeAll();
	}
}
/*-----------------------------------------------------------*/

void vParTestToggleLED( unsigned portBASE_TYPE uxLED )
{
	if( uxLED < ( portBASE_TYPE ) partstNUM_LEDS )
	{
		vTaskSuspendAll();
		{
            IO_DEBUG_TOGGLE(A, uxLED);
		}
		xTaskResumeAll();
	}
}
/*-----------------------------------------------------------*/

