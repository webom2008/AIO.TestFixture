/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : driver_uart3.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/8
  Last Modified :
  Description   : uart3 deriver
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
static UART_DEVICE_TypeDef uart3_device;
static QueueHandle_t    uart3_tx_queue  = NULL;
static QueueHandle_t    uart3_rx_queue  = NULL;
static xSemaphoreHandle xReadOpLock     = NULL; //read operate lock
static xSemaphoreHandle xWriteOpLock    = NULL; //write operate lock

/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/
#define UART3_TX_QUEUE_SIZE         230
#define UART3_RX_QUEUE_SIZE         230// 10ms buffer length

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/
int Uart3Init(void)
{
    UartDeviceDefaultInit(&uart3_device);
    uart3_device.num        = UART_NUM03;
    uart3_device.mode       = UART_INTERRUPT_MODE;
    uart3_device.baudrate   = B230400;
    uart3_device.ParityType = PARITY_NONE; //PARITY_NONE,PARITY_EVEN ,PARITY_ODD;
    uart3_device.IRQPriority= IRQPriority11Uart23;
        
    uart3_tx_queue  = xQueueCreate( UART3_TX_QUEUE_SIZE, sizeof( char ) );
    uart3_rx_queue  = xQueueCreate( UART3_RX_QUEUE_SIZE, sizeof( char ) );
    xReadOpLock     = xSemaphoreCreateMutex();
    xWriteOpLock    = xSemaphoreCreateMutex();
    
	do{} while ((NULL == uart3_tx_queue) \
                ||(NULL == uart3_rx_queue)\
                ||(NULL == xReadOpLock)\
                ||(NULL == xWriteOpLock));
    
    return 0;
}

int Uart3Open(void)
{
    int ret = 0;

    if (uart3_device.IsDeviceOpen)
    {
        return ret;
    }
    
    ret |= UartCommonInit(&uart3_device);

    if(ret < 0)
    {
        return ret;
    }
    
    uart3_device.IsDeviceOpen = true;
    return 0;
}

int Uart3Read(char *pReadData, const int nDataLen)
{
    int i;
    if (!uart3_device.IsDeviceOpen)
    {
        return -1;
    }
    
    if( pdTRUE != xSemaphoreTake( xReadOpLock, ( TickType_t ) 5 ))
    {
        return -2;
    }
    
    for (i=0; i < nDataLen; i++)
    {
		if(pdPASS != xQueueReceive(uart3_rx_queue, pReadData++, (TickType_t)10))
		{
            break;
		}
    }
    xSemaphoreGive( xReadOpLock );
    return i;
}


int Uart3Write(char *pWriteData, const int nDataLen)
{
    int i = 0;
    char *pData = pWriteData;
    
    if (!uart3_device.IsDeviceOpen)
    {
        return -1;
    }
    if( pdTRUE != xSemaphoreTake( xWriteOpLock, ( TickType_t ) 5 ))
    {
        return -2;
    }
    
    for (i=0; i < nDataLen; i++)
    {
		if(pdPASS != xQueueSendToBack(uart3_tx_queue, (void *)pData++, (TickType_t)3))
		{
            // Failed to post the message, even after 10 ticks.
			break;
		}
    }
    xSemaphoreGive( xWriteOpLock );
    USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
    
    return i;
}

int Uart3Ctrl(void)
{
    return 0;
}

int Uart3Close(void)
{
    if (uart3_device.IsDeviceOpen)
    {
        UartCommonTerminate(&uart3_device);
        uart3_device.IsDeviceOpen = false;
    }
    return 0;
}

void USART3_IRQHandler(void)
{
    volatile char temp = 0;
    BaseType_t xHigherPriorityTaskWoken, xResult;

	// xHigherPriorityTaskWoken must be initialised to pdFALSE.
	xHigherPriorityTaskWoken = pdFALSE;
    
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        temp = USART_ReceiveData(USART3);
        xResult = xQueueSendToBackFromISR(uart3_rx_queue, (void *)&temp, &xHigherPriorityTaskWoken);
        if (errQUEUE_FULL == xResult)
        {
            //TODO: do something.
            __ASM("nop;");
        }
        if(USART_GetFlagStatus(USART3, USART_FLAG_ORE) != RESET)
        {
            USART_ClearFlag(USART3, USART_FLAG_ORE);
            USART_ReceiveData(USART3);
        }
    }
  
    if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET)
    {
        xResult = xQueueReceiveFromISR(uart3_tx_queue, (void *)&temp, &xHigherPriorityTaskWoken);
        if (pdPASS == xResult)
        {
            USART_SendData(USART3, temp);
        }
        else //empty
        {
            USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
        }
    }
    
    // error happen
    if(USART_GetITStatus(USART3, USART_IT_PE) != RESET)
    {
        USART_ClearITPendingBit(USART3, USART_IT_PE);
//        udprintf("\r\n===============Uart3.Parity error");
    }
    
    if(USART_GetITStatus(USART3, USART_IT_FE | USART_IT_NE) != RESET)
    {
        USART_ClearITPendingBit(USART3, USART_IT_FE | USART_IT_NE);
    }

	if(xHigherPriorityTaskWoken)
	{
		taskYIELD ();
	}
}

