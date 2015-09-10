/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : driver_uart4.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/8
  Last Modified :
  Description   : uart4 driver file
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
static UART_DEVICE_TypeDef uart4_device;
static QueueHandle_t    uart4_tx_queue  = NULL;
static QueueHandle_t    uart4_rx_queue  = NULL;
static xSemaphoreHandle xReadOpLock     = NULL; //read operate lock
static xSemaphoreHandle xWriteOpLock    = NULL; //write operate lock
/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/
#define UART4_TX_QUEUE_SIZE         128
#define UART4_RX_QUEUE_SIZE         230// 10ms buffer length

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/
int Uart4Init(void)
{
    UartDeviceDefaultInit(&uart4_device);
    uart4_device.num        = UART_NUM04;
    uart4_device.mode       = UART_INTERRUPT_MODE;
    uart4_device.baudrate   = B230400;
    uart4_device.ParityType = PARITY_NONE; //PARITY_NONE,PARITY_EVEN ,PARITY_ODD;
    uart4_device.IRQPriority= IRQPriority08Uart4;
        
    uart4_tx_queue  = xQueueCreate( UART4_TX_QUEUE_SIZE, sizeof( char ) );
    uart4_rx_queue  = xQueueCreate( UART4_RX_QUEUE_SIZE, sizeof( char ) );
    xReadOpLock     = xSemaphoreCreateMutex();
    xWriteOpLock    = xSemaphoreCreateMutex();
    
	do{} while ((NULL == uart4_tx_queue) \
                ||(NULL == uart4_rx_queue)\
                ||(NULL == xReadOpLock)\
                ||(NULL == xWriteOpLock));
    
    return 0;
}

int Uart4Open(void)
{
    int ret = 0;

    if (uart4_device.IsDeviceOpen)
    {
        return ret;
    }
    
    ret |= UartCommonInit(&uart4_device);

    if(ret < 0)
    {
        return ret;
    }
    
    uart4_device.IsDeviceOpen = true;
    return 0;
}


int Uart4Read(char *pReadData, const int nDataLen)
{
    int i;
    if (!uart4_device.IsDeviceOpen)
    {
        return -1;
    }
    
    if( pdTRUE != xSemaphoreTake( xReadOpLock, ( TickType_t ) 5 ))
    {
        return -2;
    }
    
    for (i=0; i < nDataLen; i++)
    {
		if(pdPASS != xQueueReceive(uart4_rx_queue, pReadData++, (TickType_t)10))
		{
            break;
		}
    }
    xSemaphoreGive( xReadOpLock );
    return i;
}


int Uart4Write(char *pWriteData, const int nDataLen)
{
    int i = 0;
    char *pData = pWriteData;
    
    if (!uart4_device.IsDeviceOpen)
    {
        return -1;
    }
    if( pdTRUE != xSemaphoreTake( xWriteOpLock, ( TickType_t ) 5 ))
    {
        return -2;
    }
    
    for (i=0; i < nDataLen; i++)
    {
		if(pdPASS != xQueueSendToBack(uart4_tx_queue, (void *)pData++, (TickType_t)3))
		{
            // Failed to post the message, even after 10 ticks.
			break;
		}
    }
    xSemaphoreGive( xWriteOpLock );
    USART_ITConfig(UART4, USART_IT_TXE, ENABLE);
    
    return i;
}

int Uart4Ctrl(void)
{

    return 0;
}

int Uart4Close(void)
{
    if (uart4_device.IsDeviceOpen)
    {
        UartCommonTerminate(&uart4_device);
        uart4_device.IsDeviceOpen = false;
    }
    return 0;
}

void UART4_IRQHandler(void)
{
    volatile char temp = 0;
    BaseType_t xHigherPriorityTaskWoken, xResult;

	// xHigherPriorityTaskWoken must be initialised to pdFALSE.
	xHigherPriorityTaskWoken = pdFALSE;
    
    if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
    {
        temp = USART_ReceiveData(UART4);
        xResult = xQueueSendToBackFromISR(uart4_rx_queue, (void *)&temp, &xHigherPriorityTaskWoken);
        if (errQUEUE_FULL == xResult)
        {
            //TODO: do something.
            __ASM("nop;");
        }
        if(USART_GetFlagStatus(UART4, USART_FLAG_ORE) != RESET)
        {
            USART_ClearFlag(UART4, USART_FLAG_ORE);
            USART_ReceiveData(UART4);
        }
    }
  
    if(USART_GetITStatus(UART4, USART_IT_TXE) != RESET)
    {
        xResult = xQueueReceiveFromISR(uart4_tx_queue, (void *)&temp, &xHigherPriorityTaskWoken);
        if (pdPASS == xResult)
        {
            USART_SendData(UART4, temp);
        }
        else //empty
        {
            USART_ITConfig(UART4, USART_IT_TXE, DISABLE);
        }
    }
    
    // error happen
    if(USART_GetITStatus(UART4, USART_IT_PE) != RESET)
    {
        USART_ClearITPendingBit(UART4, USART_IT_PE);
//        udprintf("\r\n===============UART4.Parity error");
    }
    
    if(USART_GetITStatus(UART4, USART_IT_FE | USART_IT_NE) != RESET)
    {
        USART_ClearITPendingBit(UART4, USART_IT_FE | USART_IT_NE);
    }

	if(xHigherPriorityTaskWoken)
	{
		taskYIELD ();
	}
}
