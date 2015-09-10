/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : driver_uart5.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/8
  Last Modified :
  Description   : uart5 driver file
  Function List :
  History       :
  1.Date        : 2015/9/8
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#include "includes.h"
/*----------------------------------------------*
 * external variables                           *
 *----------------------------------------------*/

/*----------------------------------------------*
 * external routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * internal routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * project-wide global variables                *
 *----------------------------------------------*/

/*----------------------------------------------*
 * module-wide global variables                 *
 *----------------------------------------------*/
static UART_DEVICE_TypeDef uart5_device;
static char uart5_tx_buffer[128];
static CHAR_FIFO_StructDef uart5_tx_fifo;
static char uart5_rx_buffer[128];
static CHAR_FIFO_StructDef uart5_rx_fifo;
static xSemaphoreHandle xSerialTxHandleLock = NULL;
static xSemaphoreHandle xSerialRxHandleLock = NULL;
/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

static void Uart5BufferInit(void)
{
    char_fifo_init(&uart5_tx_fifo, uart5_tx_buffer, sizeof(uart5_tx_buffer));
    char_fifo_init(&uart5_rx_fifo, uart5_rx_buffer, sizeof(uart5_rx_buffer));
}

int Uart5Init(void)
{
    Uart5BufferInit();
    
    UartDeviceDefaultInit(&uart5_device);
    uart5_device.num        = UART_NUM05;
    uart5_device.mode       = UART_INTERRUPT_MODE;
    uart5_device.baudrate   = B230400;
    uart5_device.ParityType = PARITY_NONE; //PARITY_NONE,PARITY_EVEN ,PARITY_ODD;
    uart5_device.IRQPriority= IRQPriority06Uart5;

    xSerialTxHandleLock = xSemaphoreCreateMutex();
    xSerialRxHandleLock = xSemaphoreCreateMutex();
	do{} while ((NULL == xSerialTxHandleLock) \
        ||(NULL == xSerialRxHandleLock));
    
    return 0;
}

int Uart5Open(void)
{
    int ret = 0;

    if (uart5_device.IsDeviceOpen)
    {
        return ret;
    }
    
    ret |= UartCommonInit(&uart5_device);

    if(ret < 0)
    {
        return ret;
    }
    
    uart5_device.IsDeviceOpen = true;
    return 0;
}


int Uart5Read(char *pReadData, const int nDataLen)
{
    int ret = 0, i;
    if (!uart5_device.IsDeviceOpen)
    {
        return -1;
    }
    
    if( pdTRUE != xSemaphoreTake( xSerialRxHandleLock, ( TickType_t ) 100 ))
    {
        return -2;
    }
    
    for (i=0; i < nDataLen; i++)
    {
        ret = char_fifo_pop(&uart5_rx_fifo, pReadData++);
        if(ret < 0) break;
    }
    
    xSemaphoreGive( xSerialRxHandleLock );
    return i;
}


int Uart5Write(char *pWriteData, const int nDataLen)
{
    int i = 0;
    char *pData = pWriteData;
    
    if (!uart5_device.IsDeviceOpen)
    {
        return -1;
    }
    
    if( pdTRUE != xSemaphoreTake( xSerialTxHandleLock, ( TickType_t ) 100 ))
    {
        return -2;
    }
    
    for (i=0; i < nDataLen; i++)
    {
        char_fifo_push(&uart5_tx_fifo, pData++);
    }
    xSemaphoreGive( xSerialTxHandleLock );
    USART_ITConfig(UART5, USART_IT_TXE, ENABLE);
    
    return i;
}

int Uart5Ctrl(void)
{

    return 0;
}

int Uart5Close(void)
{
    if (uart5_device.IsDeviceOpen)
    {
        UartCommonTerminate(&uart5_device);
        uart5_device.IsDeviceOpen = false;
    }
    return 0;
}

void UART5_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken, xResult;
    int ret = 0;
    char temp = 0;

	// xHigherPriorityTaskWoken must be initialised to pdFALSE.
	xHigherPriorityTaskWoken = pdFALSE;
    
    if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)
    {
        temp = USART_ReceiveData(UART5);
        xResult = xSemaphoreTakeFromISR( xSerialRxHandleLock, &xHigherPriorityTaskWoken);
        if( pdTRUE == xResult)
        {
            char_fifo_push(&uart5_rx_fifo, &temp);
            xResult = xSemaphoreGiveFromISR( xSerialRxHandleLock ,&xHigherPriorityTaskWoken);
        }
        if(USART_GetFlagStatus(UART5, USART_FLAG_ORE) != RESET)
        {
            USART_ClearFlag(UART5, USART_FLAG_ORE);
            USART_ReceiveData(UART5);
        }
    }
  
    if(USART_GetITStatus(UART5, USART_IT_TXE) != RESET)
    {
        xResult = xSemaphoreTakeFromISR( xSerialTxHandleLock, &xHigherPriorityTaskWoken);
        if( pdTRUE == xResult)
        {
            ret = char_fifo_pop(&uart5_tx_fifo, &temp);
            xResult = xSemaphoreGiveFromISR( xSerialTxHandleLock , &xHigherPriorityTaskWoken);
            if(ret < 0) //empty
            {
                USART_ITConfig(UART5, USART_IT_TXE, DISABLE);
            }
            else
            {
                USART_SendData(UART5, temp);
            }
        }
    }
    
    // error happen
    if(USART_GetITStatus(UART5, USART_IT_PE) != RESET)
    {
        USART_ClearITPendingBit(UART5, USART_IT_PE);
//        udprintf("\r\n===============UART5.Parity error");
    }
    
    if(USART_GetITStatus(UART5, USART_IT_FE | USART_IT_NE) != RESET)
    {
        USART_ClearITPendingBit(UART5, USART_IT_FE | USART_IT_NE);
    }
    
    if( xResult == pdPASS )
	{
		// If xHigherPriorityTaskWoken is now set to pdTRUE then a context
		// switch should be requested.  The macro used is port specific and 
		// will be either portYIELD_FROM_ISR() or portEND_SWITCHING_ISR() - 
		// refer to the documentation page for the port being used.
		portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	}
}
