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


/* BASIC INTERRUPT DRIVEN SERIAL PORT DRIVER.   
 * 
 * This file only supports UART 1
 */

/* Standard includes. */
#include <stdlib.h>
#include <signal.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

/* Demo application includes. */
#include "serial.h"
#include "interrupt.h"
#include "irq_interface.h"
#include "sdk_cfg.h"

#define LOG_TAG     "[serial]"
#define LOG_INFO_ENABLE
#define LOG_ERROR_ENABLE
#define LOG_DUMP_ENABLE
#include "debug.h"

/* setup the hardware mapping. */
#define UART_BUF            JL_UART1->BUF
#define UART_CON            JL_UART1->CON0
#define UART_BAUD           JL_UART1->BAUD
#define UART_IRQ_INDEX      IRQ_UART1_IDX

#define UART_PORT_INIT()    \
    JL_IOMAP->CON1 &= ~(BIT(3) | BIT(2)); \
    JL_PORTB->OUT |= BIT(0) ;\
    JL_PORTB->DIR |= BIT(1) ;\
    JL_PORTB->DIR &= ~BIT(0)

/* Constants required to setup the hardware. */
#define UART_RX_IE_ENABLE       BIT(3)
#define UART_RX_PENDING         BIT(14)
#define UART_RX_CLR_PENDING     BIT(12)

#define UART_TX_IE_ENABLE       BIT(2)
#define UART_TX_PENDING         BIT(15)
#define UART_TX_CLR_PENDING     BIT(13)


/* Misc. constants. */
#define serNO_BLOCK				( ( TickType_t ) 0 )

/* Enable the UART Tx interrupt. */
#define vInterruptOn() IFG2 |= UTXIFG1

/* The queue used to hold received characters. */
static QueueHandle_t xRxedChars; 

/* The queue used to hold characters waiting transmission. */
static QueueHandle_t xCharsForTx; 

static volatile portBASE_TYPE xQueueEmpty = pdTRUE;


/* Interrupt service routines. */
static void vRxISR( void );
static void vTxISR( void );

/*-----------------------------------------------------------*/

static void uart_irq_handle(void)
{
    u8 value;

    //RX pending
    if ((UART_CON & UART_RX_IE_ENABLE) && (UART_CON & UART_RX_PENDING)) { 
        vRxISR();
        UART_CON |= UART_RX_CLR_PENDING; //clr rx pd
    }

    //TX pending
    if ((UART_CON & UART_TX_IE_ENABLE) && (UART_CON & UART_TX_PENDING)) { 
        vTxISR();
        UART_CON |= UART_TX_CLR_PENDING; //clr tx pd
    }
}
IRQ_REGISTER(UART_IRQ_INDEX, uart_irq_handle);

xComPortHandle xSerialPortInitMinimal( unsigned long ulWantedBaud, unsigned portBASE_TYPE uxQueueLength )
{

	/* Initialise the hardware. */

	/* Generate the baud rate constants for the wanted baud rate. */

	portENTER_CRITICAL();
	{
		/* Create the queues used by the com test task. */
		xRxedChars = xQueueCreate( uxQueueLength, ( unsigned portBASE_TYPE ) sizeof( signed char ) );
		xCharsForTx = xQueueCreate( uxQueueLength, ( unsigned portBASE_TYPE ) sizeof( signed char ) );
        UART_PORT_INIT();
        UART_BAUD   = (APP_UART_CLK / ulWantedBaud) / 4 - 1;
        IRQ_REQUEST(UART_IRQ_INDEX, uart_irq_handle, 3);
        UART_CON    = BIT(13) | BIT(12) | BIT(0) | BIT(3) | BIT(2);       //Tx_pending/Rx_pending/Enable/Rx_IE/Tx_IE
        xQueueEmpty = pdTRUE; //first tx empty
	}
	portEXIT_CRITICAL();
	
	/* Unlike other ports, this serial code does not allow for more than one
	com port.  We therefore don't return a pointer to a port structure and can
	instead just return NULL. */
	return NULL;
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE xSerialGetChar( xComPortHandle pxPort, signed char *pcRxedChar, TickType_t xBlockTime )
{
	/* Get the next character from the buffer.  Return false if no characters
	are available, or arrive before xBlockTime expires. */
	if( xQueueReceive( xRxedChars, pcRxedChar, xBlockTime ) )
	{
		return pdTRUE;
	}
	else
	{
		return pdFALSE;
	}
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE xSerialPutChar( xComPortHandle pxPort, signed char cOutChar, TickType_t xBlockTime )
{
signed portBASE_TYPE xReturn;

	/* Transmit a character. */

	portENTER_CRITICAL();
    {
        if ( xQueueEmpty == pdTRUE )
        {
            UART_BUF = cOutChar;
            __asm__ volatile("csync");
			xReturn = pdPASS;
        }
        else 
        {
			if( xQueueSend( xCharsForTx, &cOutChar, xBlockTime ) != pdPASS )
			{
				xReturn = pdFAIL;
			}			
			else
			{
				xReturn = pdPASS;				
			}
        }

		xQueueEmpty = pdFALSE;
	}
	portEXIT_CRITICAL();

	return xReturn;
}
/*-----------------------------------------------------------*/

/*
 * UART RX interrupt service routine.
 */
static void vRxISR( void )
{
signed char cChar;
portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	/* Get the character from the UART and post it on the queue of Rxed 
	characters. */
	cChar = UART_BUF;

    /* log_info("vRxISR : %c", cChar); */
    xQueueSendFromISR( xRxedChars, &cChar, &xHigherPriorityTaskWoken );

	if( xHigherPriorityTaskWoken )
	{
		/*If the post causes a task to wake force a context switch 
		as the woken task may have a higher priority than the task we have 
		interrupted. */
		taskYIELD();
	}
}
/*-----------------------------------------------------------*/

/*
 * UART Tx interrupt service routine.
 */
static void vTxISR( void )
{
signed char cChar;
portBASE_TYPE xTaskWoken = pdFALSE;

	/* The previous character has been transmitted.  See if there are any
	further characters waiting transmission. */

	if( xQueueReceiveFromISR( xCharsForTx, &cChar, &xTaskWoken ) == pdTRUE )
	{
        /* log_info("vTxISR : %x", cChar); */
		/* There was another character queued - transmit it now. */
		UART_BUF = cChar;
        __asm__ volatile("csync");
	}
	else
	{
		/* There were no other characters to transmit. */
		xQueueEmpty = pdTRUE;
	}
}

