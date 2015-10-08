/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : driver_uart1.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/2
  Last Modified :
  Description   : uart1 driver
  Function List :
  History       :
  1.Date        : 2015/9/2
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
static UART_DEVICE_TypeDef uart1_device;
static QueueHandle_t    uart1_tx_queue  = NULL;
static QueueHandle_t    uart1_rx_queue  = NULL;
static xSemaphoreHandle xReadOpLock     = NULL; //read operate lock
static xSemaphoreHandle xWriteOpLock    = NULL; //write operate lock

/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/
#define UART1_TX_QUEUE_SIZE         128
#define UART1_RX_QUEUE_SIZE         230// 10ms buffer length

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

/*****************************************************************************
 Prototype    : Uart1Init
 Description  : init uart1
 Input        : void  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2015/9/2
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
int Uart1Init(void)
{
    UartDeviceDefaultInit(&uart1_device);
    uart1_device.num        = UART_NUM01;
    uart1_device.mode       = UART_INTERRUPT_MODE;
//    uart1_device.mode       = UART_DMA_MODE;
    uart1_device.baudrate   = B230400;
    uart1_device.ParityType = PARITY_NONE; //PARITY_NONE,PARITY_EVEN ,PARITY_ODD;
    uart1_device.IRQPriority= IRQPriority14Uart15;
        
    uart1_tx_queue  = xQueueCreate( UART1_TX_QUEUE_SIZE, sizeof( char ) );
    uart1_rx_queue  = xQueueCreate( UART1_RX_QUEUE_SIZE, sizeof( char ) );
    xReadOpLock     = xSemaphoreCreateMutex();
    xWriteOpLock    = xSemaphoreCreateMutex();

    do{} while ((NULL == uart1_tx_queue) \
                ||(NULL == uart1_rx_queue)\
                ||(NULL == xReadOpLock)\
                ||(NULL == xWriteOpLock));
    
    return 0;
}

/*****************************************************************************
 Prototype    : Uart1Open
 Description  : open uart1 device
 Input        : void  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2015/9/2
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
int Uart1Open(void)
{
    int ret = 0;

    if (uart1_device.IsDeviceOpen)
    {
        return ret;
    }
    
    ret |= UartCommonInit(&uart1_device);

    if(ret < 0)
    {
        return ret;
    }
    
    uart1_device.IsDeviceOpen = true;
    return 0;
}

/*****************************************************************************
 Prototype    : Uart1Read
 Description  : read uart1 buffer
 Input        : char *pReadData     
                const int nDataLen  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2015/9/2
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
int Uart1Read(char *pReadData, const int nDataLen)
{
    int i;
    if (!uart1_device.IsDeviceOpen)
    {
        return -1;
    }
    
    if( pdTRUE != xSemaphoreTake( xReadOpLock, ( TickType_t ) 5 ))
    {
        return -2;
    }
    
    for (i=0; i < nDataLen; i++)
    {
		if(pdPASS != xQueueReceive(uart1_rx_queue, pReadData++, (TickType_t)10))
		{
            break;
		}
    }
    xSemaphoreGive( xReadOpLock );
    return i;
}

/*****************************************************************************
 Prototype    : Uart1Write
 Description  : write date to uart1
 Input        : const char *pWriteData  
                const int nDataLen      
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2015/9/2
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
int Uart1Write(char *pWriteData, const int nDataLen)
{
    int i = 0;
    char *pData = pWriteData;
    
    if (!uart1_device.IsDeviceOpen)
    {
        return -1;
    }
    if( pdTRUE != xSemaphoreTake( xWriteOpLock, ( TickType_t ) 5 ))
    {
        return -2;
    }
    
    for (i=0; i < nDataLen; i++)
    {
        if(pdPASS != xQueueSendToBack(uart1_tx_queue, (void *)pData++, (TickType_t)3))
        {
            // Failed to post the message, even after 10 ticks.
            break;
        }
    }
    xSemaphoreGive( xWriteOpLock );
    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
    
    return i;
}

int Uart1Ctrl(void)
{

    return 0;
}

/*****************************************************************************
 Prototype    : Uart1Close
 Description  : close uart1 device
 Input        : void  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2015/9/2
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
int Uart1Close(void)
{
    if (uart1_device.IsDeviceOpen)
    {
        UartCommonTerminate(&uart1_device);
        uart1_device.IsDeviceOpen = false;
    }
    return 0;
}

/*****************************************************************************
 Prototype    : USART1_IRQHandler
 Description  : uart1 interrupt hander
 Input        : void  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2015/9/2
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
void USART1_IRQHandler(void)
{
    volatile char temp = 0;
    BaseType_t xHigherPriorityTaskWoken, xResult;

	// We have not woken a task at the start of the ISR.
	xHigherPriorityTaskWoken = pdFALSE;
    
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        temp = USART_ReceiveData(USART1);
        xResult = xQueueSendToBackFromISR(uart1_rx_queue, (void *)&temp, &xHigherPriorityTaskWoken);
        if (errQUEUE_FULL == xResult)
        {
            //TODO: do something.
            __ASM("nop;");
        }
        if(USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)
        {
            USART_ClearFlag(USART1, USART_FLAG_ORE);
            USART_ReceiveData(USART1);
        }
    }
  
    if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
    {
        xResult = xQueueReceiveFromISR(uart1_tx_queue, (void *)&temp, &xHigherPriorityTaskWoken);
        if (pdPASS == xResult)
        {
            USART_SendData(USART1, temp);
        }
        else //empty
        {
            USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
        }
    }
    
    // error happen
    if(USART_GetITStatus(USART1, USART_IT_PE) != RESET)
    {
        USART_ClearITPendingBit(USART1, USART_IT_PE);
//        udprintf("\r\n===============Uart1.Parity error");
    }
    
    if(USART_GetITStatus(USART1, USART_IT_FE | USART_IT_NE) != RESET)
    {
        USART_ClearITPendingBit(USART1, USART_IT_FE | USART_IT_NE);
    }

	if(xHigherPriorityTaskWoken)
	{
		taskYIELD ();
	}
}

#if 1 // user define printf
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
static char strbuf[80];

int udprintf(const char* fmt, ...)
{
    va_list argptr;
    int nLen = 0;
    
    memset(strbuf, 0, sizeof(strbuf));
    
    va_start(argptr, fmt);
    vsnprintf(strbuf, sizeof(strbuf), fmt, argptr);
    va_end(argptr);
    
    nLen = strlen(strbuf);
    Uart1Write(strbuf, nLen);
    return nLen;
}
#else
int udprintf(const char* fmt, ...)
{
    return 0;
}
#include <stdio.h>

#pragma import(__use_no_semihosting) 

_sys_exit(int x) 
{ 
  x = x; 
}
 
struct __FILE 
{ 
  int handle; 
}; 

FILE __stdout;

#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

PUTCHAR_PROTOTYPE
{

#if 0 //wait mode
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART */
    USART_SendData(USART1, (uint8_t) ch);

    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
    {}
#else //interrupt mode
    xSerialPutChar(ch);
#endif
    return ch;
}
#endif
