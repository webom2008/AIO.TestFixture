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
static QueueHandle_t    uart5_tx_queue  = NULL;
static QueueHandle_t    uart5_rx_queue  = NULL;
static xSemaphoreHandle xReadOpLock     = NULL; //read operate lock
static xSemaphoreHandle xWriteOpLock    = NULL; //write operate lock
/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/
#define UART5_TX_QUEUE_SIZE         128
#define UART5_RX_QUEUE_SIZE         230// 10ms buffer length

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/
int Uart5Init(void)
{
    UartDeviceDefaultInit(&uart5_device);
    uart5_device.num        = UART_NUM05;
    uart5_device.mode       = UART_INTERRUPT_MODE;
    uart5_device.baudrate   = B230400;
    uart5_device.ParityType = PARITY_NONE; //PARITY_NONE,PARITY_EVEN ,PARITY_ODD;
    uart5_device.IRQPriority= IRQPriority14Uart145;
        
    uart5_tx_queue  = xQueueCreate( UART5_TX_QUEUE_SIZE, sizeof( char ) );
    uart5_rx_queue  = xQueueCreate( UART5_RX_QUEUE_SIZE, sizeof( char ) );
    xReadOpLock     = xSemaphoreCreateMutex();
    xWriteOpLock    = xSemaphoreCreateMutex();
    
	do{} while ((NULL == uart5_tx_queue) \
                ||(NULL == uart5_rx_queue)\
                ||(NULL == xReadOpLock)\
                ||(NULL == xWriteOpLock));
    
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
    int i;
    if (!uart5_device.IsDeviceOpen)
    {
        return -1;
    }
    
    if( pdTRUE != xSemaphoreTake( xReadOpLock, ( TickType_t ) 5 ))
    {
        return -2;
    }
    
    for (i=0; i < nDataLen; i++)
    {
		if(pdPASS != xQueueReceive(uart5_rx_queue, pReadData++, (TickType_t)10))
		{
            break;
		}
    }
    xSemaphoreGive( xReadOpLock );
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
    if( pdTRUE != xSemaphoreTake( xWriteOpLock, ( TickType_t ) 5 ))
    {
        return -2;
    }
    
    for (i=0; i < nDataLen; i++)
    {
		if(pdPASS != xQueueSendToBack(uart5_tx_queue, (void *)pData++, (TickType_t)3))
		{
            // Failed to post the message, even after 10 ticks.
			break;
		}
    }
    xSemaphoreGive( xWriteOpLock );
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
    volatile char temp = 0;
    BaseType_t xHigherPriorityTaskWoken, xResult;

	// xHigherPriorityTaskWoken must be initialised to pdFALSE.
	xHigherPriorityTaskWoken = pdFALSE;
    
    if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)
    {
        temp = USART_ReceiveData(UART5);
        xResult = xQueueSendToBackFromISR(uart5_rx_queue, (void *)&temp, &xHigherPriorityTaskWoken);
        if (errQUEUE_FULL == xResult)
        {
            //TODO: do something.
            __ASM("nop;");
        }
        if(USART_GetFlagStatus(UART5, USART_FLAG_ORE) != RESET)
        {
            USART_ClearFlag(UART5, USART_FLAG_ORE);
            USART_ReceiveData(UART5);
        }
    }
  
    if(USART_GetITStatus(UART5, USART_IT_TXE) != RESET)
    {
        xResult = xQueueReceiveFromISR(uart5_tx_queue, (void *)&temp, &xHigherPriorityTaskWoken);
        if (pdPASS == xResult)
        {
            USART_SendData(UART5, temp);
        }
        else //empty
        {
            USART_ITConfig(UART5, USART_IT_TXE, DISABLE);
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

	if(xHigherPriorityTaskWoken)
	{
		taskYIELD ();
	}
}
