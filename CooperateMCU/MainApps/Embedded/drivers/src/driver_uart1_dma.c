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
EventGroupHandle_t xUart1RxEventGroup;  // Declare a variable to hold the created event group.

/*----------------------------------------------*
 * module-wide global variables                 *
 *----------------------------------------------*/
static UART_DEVICE_TypeDef uart1_device;

#define UART1_TX_DMA_BUF_LEN        64
#define UART1_RX_DMA_BUF_LEN        64

static u8 u8TxDMABuffer[UART1_TX_DMA_BUF_LEN*2];
static u8 u8RxDMABuffer[UART1_RX_DMA_BUF_LEN*2];
static UART_DMA_BUFFER_TypeDef  uart1_tx_dma_buf;
static UART_DMA_BUFFER_TypeDef  uart1_rx_dma_buf;

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

static void Uart1DmaBufferInit(void)
{
    uart1_tx_dma_buf.pPingPongBuff1     = u8TxDMABuffer;
    uart1_tx_dma_buf.pPingPongBuff2     = &u8TxDMABuffer[UART1_TX_DMA_BUF_LEN];
    uart1_tx_dma_buf.nBuff1MaxLength    = UART1_TX_DMA_BUF_LEN;
    uart1_tx_dma_buf.nBuff2MaxLength    = UART1_TX_DMA_BUF_LEN;
    uart1_tx_dma_buf.IdleBufferIndex    = 0;
    uart1_tx_dma_buf.nBuff1Offset       = 0;
    uart1_tx_dma_buf.nBuff2Offset       = 0;
    uart1_tx_dma_buf.IsDMAWroking       = 0;
    
    uart1_rx_dma_buf.pPingPongBuff1     = u8RxDMABuffer;
    uart1_rx_dma_buf.pPingPongBuff2     = &u8RxDMABuffer[UART1_RX_DMA_BUF_LEN];
    uart1_rx_dma_buf.nBuff1MaxLength    = UART1_RX_DMA_BUF_LEN;
    uart1_rx_dma_buf.nBuff2MaxLength    = UART1_RX_DMA_BUF_LEN;
    uart1_rx_dma_buf.IdleBufferIndex    = 1;
    uart1_rx_dma_buf.nBuff1Offset       = 0;
    uart1_rx_dma_buf.nBuff2Offset       = 0;
    uart1_rx_dma_buf.IsDMAWroking       = 0;
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
    Uart1DmaBufferInit();
    UartDeviceDefaultInit(&uart1_device);
    uart1_device.num            = UART_NUM01;
    uart1_device.mode           = UART_DMA_MODE;
    uart1_device.baudrate       = B230400;
    uart1_device.ParityType     = PARITY_NONE; //PARITY_NONE,PARITY_EVEN ,PARITY_ODD;
    uart1_device.IRQPriority    = IRQPriority10Uart1;
    uart1_device.pTxDMABuffer   = &uart1_tx_dma_buf;
    uart1_device.pRxDMABuffer   = &uart1_rx_dma_buf;
    
    xSerialTxHandleLock = xSemaphoreCreateMutex();
    xSerialRxHandleLock = xSemaphoreCreateMutex();
	xUart1RxEventGroup  = xEventGroupCreate();
	do{} while ((NULL == xSerialTxHandleLock) \
        ||(NULL == xSerialRxHandleLock)\
        ||(NULL == xUart1RxEventGroup));
    
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
    int rLen = 0;
    if (!uart1_device.IsDeviceOpen)
    {
        return -1;
    }
    
    if( pdTRUE != xSemaphoreTake( xSerialRxHandleLock, ( TickType_t ) 100 ))
    {
        return -2;
    }
    
    if (uart1_rx_dma_buf.IdleBufferIndex) //Buffer2 ready
    {
       rLen = MyMini(nDataLen, uart1_rx_dma_buf.nBuff2Offset);
       memcpy(pReadData, uart1_rx_dma_buf.pPingPongBuff2, rLen);
    }
    else //Buffer1 ready
    {
       rLen = MyMini(nDataLen, uart1_rx_dma_buf.nBuff1Offset);
       memcpy(pReadData, uart1_rx_dma_buf.pPingPongBuff1, rLen);
    }
    
    xSemaphoreGive( xSerialRxHandleLock );
    return rLen;
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
    int ready2writeLen = 0;
    
    if (!uart1_device.IsDeviceOpen)
    {
        return -1;
    }
    
    if( pdTRUE != xSemaphoreTake( xSerialTxHandleLock, ( TickType_t ) 100 ))
    {
        return -2;
    }

    if (uart1_tx_dma_buf.IdleBufferIndex) //buffer2 idle
    {
        ready2writeLen = uart1_tx_dma_buf.nBuff2Offset + nDataLen;
        if (ready2writeLen > uart1_tx_dma_buf.nBuff2MaxLength)
        {
            //Warnig: Not enugh spare to store.
        }
        else
        {
            memcpy( &uart1_tx_dma_buf.pPingPongBuff2[uart1_tx_dma_buf.nBuff2Offset],
                    pWriteData,
                    nDataLen);
            uart1_tx_dma_buf.nBuff2Offset += nDataLen;
        }
        if (!uart1_tx_dma_buf.IsDMAWroking)
        {
            DMA1_Channel4->CMAR = (uint32_t)uart1_tx_dma_buf.pPingPongBuff2;
            DMA1_Channel4->CNDTR = uart1_tx_dma_buf.nBuff2Offset;
            uart1_tx_dma_buf.IdleBufferIndex = 0;
            DMA_Cmd(DMA1_Channel4, ENABLE); // open DMA
            uart1_tx_dma_buf.IsDMAWroking = 1;
        }
    }
    else
    {
        ready2writeLen = uart1_tx_dma_buf.nBuff1Offset + nDataLen;
        if (ready2writeLen > uart1_tx_dma_buf.nBuff1MaxLength)
        {
            //Warnig: Not enugh spare to store.
        }
        else
        {
            memcpy( &uart1_tx_dma_buf.pPingPongBuff1[uart1_tx_dma_buf.nBuff1Offset],
                    pWriteData,
                    nDataLen);
            uart1_tx_dma_buf.nBuff1Offset += nDataLen;
        }
        if (!uart1_tx_dma_buf.IsDMAWroking)
        {
            DMA1_Channel4->CMAR = (uint32_t)uart1_tx_dma_buf.pPingPongBuff1;
            DMA1_Channel4->CNDTR = uart1_tx_dma_buf.nBuff1Offset;
            uart1_tx_dma_buf.IdleBufferIndex = 1;
            DMA_Cmd(DMA1_Channel4, ENABLE); // open DMA
            uart1_tx_dma_buf.IsDMAWroking = 1;
        }
    }
    
    xSemaphoreGive( xSerialTxHandleLock );
    return nDataLen;
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

void USART1_IRQHandler(void)
{
    volatile u32 tem_reg;
    volatile u16 u16BufferUsedLen = 0;
    BaseType_t xHigherPriorityTaskWoken, xResult;

	// xHigherPriorityTaskWoken must be initialised to pdFALSE.
	xHigherPriorityTaskWoken = pdFALSE;
    
    // uart idle interrupt
    if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
    {
        USART_ClearITPendingBit(USART1, USART_IT_IDLE);
        DMA_ClearFlag(DMA1_FLAG_GL5);//clear all interrupt flags     
        DMA_Cmd(DMA1_Channel5, DISABLE); //close DMA incase receive data while handling

        xResult = xSemaphoreTakeFromISR( xSerialRxHandleLock, &xHigherPriorityTaskWoken);
        if( pdTRUE == xResult)
        {
            if (uart1_rx_dma_buf.IdleBufferIndex) //buf1 busy, buf2 idle
            {
                u16BufferUsedLen = uart1_rx_dma_buf.nBuff1MaxLength - DMA_GetCurrDataCounter(DMA1_Channel5); 
                if (u16BufferUsedLen > 0)
                {
                    uart1_rx_dma_buf.nBuff1Offset = u16BufferUsedLen;
                    DMA1_Channel5->CMAR = (uint32_t)uart1_rx_dma_buf.pPingPongBuff2;
                    DMA1_Channel5->CNDTR = uart1_rx_dma_buf.nBuff2MaxLength;
                    uart1_rx_dma_buf.IdleBufferIndex = 0;
                }
            }
            else
            {
                u16BufferUsedLen = uart1_rx_dma_buf.nBuff2MaxLength - DMA_GetCurrDataCounter(DMA1_Channel5); 
                if (u16BufferUsedLen > 0)
                {
                    uart1_rx_dma_buf.nBuff2Offset = u16BufferUsedLen;
                    DMA1_Channel5->CMAR = (uint32_t)uart1_rx_dma_buf.pPingPongBuff1;
                    DMA1_Channel5->CNDTR = uart1_rx_dma_buf.nBuff1MaxLength;
                    uart1_rx_dma_buf.IdleBufferIndex = 1;
                }
            }
            xResult = xSemaphoreGiveFromISR( xSerialRxHandleLock ,&xHigherPriorityTaskWoken);

            if (u16BufferUsedLen > 0)
            {
                //boardcast message to handle
            	xResult = xEventGroupSetBitsFromISR(
            						xUart1RxEventGroup,	// The event group being updated.
            						UART_DMA_RX_INCOMPLETE_EVENT_BIT,// The bits being set.
            						&xHigherPriorityTaskWoken );
            } //End if u16BufferUsedLen > 0
        }// End if pdTRUE == xSemaphoreTakeFromISR
        DMA_Cmd(DMA1_Channel5, ENABLE);                 //open DMA after handled
        
        //clear Idle flag by read SR and DR
        tem_reg = USART1->SR;
        tem_reg = USART1->DR;
        tem_reg = tem_reg; // slove warning 
    }// End if USART_IT_IDLE

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
    
	if( xResult == pdPASS )
	{
		// If xHigherPriorityTaskWoken is now set to pdTRUE then a context
		// switch should be requested.  The macro used is port specific and 
		// will be either portYIELD_FROM_ISR() or portEND_SWITCHING_ISR() - 
		// refer to the documentation page for the port being used.
		portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	}
}

/*****************************************************************************
 Prototype    : DMA1_Channel5_IRQHandler
 Description  : DMA1_Channel5 for UART1_Rx
 Input        : void  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2015/6/16
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
void DMA1_Channel5_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken, xResult;

	// xHigherPriorityTaskWoken must be initialised to pdFALSE.
	xHigherPriorityTaskWoken = pdFALSE;
    
    DMA_ClearITPendingBit(DMA1_IT_TC5); 
//    DMA_ClearITPendingBit(DMA1_IT_TE5);
    DMA_Cmd(DMA1_Channel5, DISABLE);            //close DMA incase receive data while handling

    if( pdTRUE != xSemaphoreTakeFromISR( xSerialRxHandleLock, &xHigherPriorityTaskWoken))
    {
        return;
    }

    if (uart1_rx_dma_buf.IdleBufferIndex) //buf1 busy, buf2 idle
    {
        //buffer1 finished recevied mission (full), switch to buffer2
        uart1_rx_dma_buf.nBuff1Offset = uart1_rx_dma_buf.nBuff1MaxLength;
        
        DMA1_Channel5->CMAR = (uint32_t)uart1_rx_dma_buf.pPingPongBuff2;
        DMA1_Channel5->CNDTR = uart1_rx_dma_buf.nBuff2MaxLength;
        uart1_rx_dma_buf.IdleBufferIndex = 0;
    }
    else //buf2 busy, buf1 idle
    {
        //buffer2 finished recevied mission (full), switch to buffer1
        uart1_rx_dma_buf.nBuff2Offset = uart1_rx_dma_buf.nBuff2MaxLength;
        
        DMA1_Channel5->CMAR = (uint32_t)uart1_rx_dma_buf.pPingPongBuff1;
        DMA1_Channel5->CNDTR = uart1_rx_dma_buf.nBuff1MaxLength;
        uart1_rx_dma_buf.IdleBufferIndex = 1;
    }
    xSemaphoreGiveFromISR( xSerialRxHandleLock ,&xHigherPriorityTaskWoken);
    DMA_Cmd(DMA1_Channel5, ENABLE);             //open DMA after handled

    //boardcast message to handle
	xResult = xEventGroupSetBitsFromISR(
						xUart1RxEventGroup,	// The event group being updated.
						UART_DMA_RX_COMPLETE_EVENT_BIT,// The bits being set.
						&xHigherPriorityTaskWoken );

	// Was the message posted successfully?
	if( xResult == pdPASS )
	{
		// If xHigherPriorityTaskWoken is now set to pdTRUE then a context
		// switch should be requested.  The macro used is port specific and 
		// will be either portYIELD_FROM_ISR() or portEND_SWITCHING_ISR() - 
		// refer to the documentation page for the port being used.
		portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	}
}

/*****************************************************************************
 Prototype    : DMA1_Channel4_IRQHandler
 Description  : DMA1_Channel4 for UART1_Tx
 Input        : void  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2015/6/16
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
void DMA1_Channel4_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken, xResult;

	// xHigherPriorityTaskWoken must be initialised to pdFALSE.
	xHigherPriorityTaskWoken = pdFALSE;
    
    DMA_ClearITPendingBit(DMA1_IT_TC4);
//    DMA_ClearITPendingBit(DMA1_IT_TE4);
    DMA_Cmd(DMA1_Channel4, DISABLE);    // close DMA

    xResult = xSemaphoreTakeFromISR( xSerialTxHandleLock, &xHigherPriorityTaskWoken);
    if( pdTRUE != xResult)
    {
        return;
    }

    //After finish this send mission, need to do: clear status --> check next mission
    if (uart1_tx_dma_buf.IdleBufferIndex)
    {
        // reset buffer1
        uart1_tx_dma_buf.nBuff1Offset = 0;

        // check for buffer2
        if (uart1_tx_dma_buf.nBuff2Offset > 0)
        {
            DMA1_Channel4->CMAR = (uint32_t)uart1_tx_dma_buf.pPingPongBuff2;
            DMA1_Channel4->CNDTR = uart1_tx_dma_buf.nBuff2Offset;
            uart1_tx_dma_buf.IdleBufferIndex = 0;
            DMA_Cmd(DMA1_Channel4, ENABLE); // open DMA
        }
        else
        {
            uart1_tx_dma_buf.IsDMAWroking = 0;
        }
    }
    else
    {
        //reset buffer2
        uart1_tx_dma_buf.nBuff2Offset = 0;
        
        // check for buffer1
        if (uart1_tx_dma_buf.nBuff1Offset > 0)
        {
            DMA1_Channel4->CMAR = (uint32_t)uart1_tx_dma_buf.pPingPongBuff1;
            DMA1_Channel4->CNDTR = uart1_tx_dma_buf.nBuff1Offset;
            uart1_tx_dma_buf.IdleBufferIndex = 1;
            DMA_Cmd(DMA1_Channel4, ENABLE); // open DMA
        }
        else
        {
            uart1_tx_dma_buf.IsDMAWroking = 0;
        }
    }
    
    xResult = xSemaphoreGiveFromISR( xSerialTxHandleLock , &xHigherPriorityTaskWoken);
	if( xResult == pdPASS )
	{
		// If xHigherPriorityTaskWoken is now set to pdTRUE then a context
		// switch should be requested.  The macro used is port specific and 
		// will be either portYIELD_FROM_ISR() or portEND_SWITCHING_ISR() - 
		// refer to the documentation page for the port being used.
		portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	}
}

//============================================================
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

