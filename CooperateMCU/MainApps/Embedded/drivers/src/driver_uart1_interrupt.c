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
#include "defines.h"
#include "fifo.h"
#include "uart_common.h"
#include "driver_uart1.h"

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
static char uart1_tx_buffer[128];
static CHAR_FIFO_StructDef uart1_tx_fifo;
static char uart1_rx_buffer[128];
static CHAR_FIFO_StructDef uart1_rx_fifo;

/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/


static void Uart1BufferInit(void)
{
    char_fifo_init(&uart1_tx_fifo, uart1_tx_buffer, sizeof(uart1_tx_buffer));
    char_fifo_init(&uart1_rx_fifo, uart1_rx_buffer, sizeof(uart1_rx_buffer));
}

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
    Uart1BufferInit();
    
    UartDeviceDefaultInit(&uart1_device);
    uart1_device.num        = UART_NUM01;
    uart1_device.mode       = UART_INTERRUPT_MODE;
//    uart1_device.mode       = UART_DMA_MODE;
    uart1_device.baudrate   = B230400;
    uart1_device.ParityType = PARITY_NONE; //PARITY_NONE,PARITY_EVEN ,PARITY_ODD;
    uart1_device.IRQPriority= IRQPriority10Uart1;
    
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
    int ret = 0, i;
    if (!uart1_device.IsDeviceOpen)
    {
        return -1;
    }
    
    for (i=0; i < nDataLen; i++)
    {
        ret = char_fifo_pop(&uart1_rx_fifo, pReadData++);
        if(ret < 0) break;
    }
    
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
    
    for (i=0; i < nDataLen; i++)
    {
        char_fifo_push(&uart1_tx_fifo, pData++);
    }
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
    int ret = 0;
    char temp = 0;
    
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        temp = USART_ReceiveData(USART1);
        char_fifo_push(&uart1_rx_fifo, &temp);
        if(USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)
        {
            USART_ClearFlag(USART1, USART_FLAG_ORE);
            USART_ReceiveData(USART1);
        }
    }
  
    if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
    {
        ret = char_fifo_pop(&uart1_tx_fifo, &temp);
        if(ret < 0) //empty
        {
            USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
        }
        else
        {
            USART_SendData(USART1, temp);
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
