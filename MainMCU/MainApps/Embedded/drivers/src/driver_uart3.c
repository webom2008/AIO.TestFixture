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
EventGroupHandle_t xUart3RxEventGroup;  // Declare a variable to hold the created event group.

/*----------------------------------------------*
 * module-wide global variables                 *
 *----------------------------------------------*/
static UART_DEVICE_TypeDef uart3_device;

#define UART3_TX_DMA_BUF_LEN        sizeof(DmaUartProtocolPacket)
#define UART3_RX_DMA_BUF_LEN        sizeof(DmaUartProtocolPacket)

static u8 u8TxDMABuffer[UART3_TX_DMA_BUF_LEN*2];
static u8 u8RxDMABuffer[UART3_RX_DMA_BUF_LEN*2];
static UART_DMA_BUFFER_TypeDef  uart3_tx_dma_buf;
static UART_DMA_BUFFER_TypeDef  uart3_rx_dma_buf;

static xSemaphoreHandle xSerialTxHandleLock = NULL;
static xSemaphoreHandle xSerialRxHandleLock = NULL;

static u8 volatile xSerialRxParityFlag = DMA_UART_PACKET_PARITY_OK;
/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

static void Uart3DmaBufferInit(void)
{
    uart3_tx_dma_buf.pPingPongBuff1     = u8TxDMABuffer;
    uart3_tx_dma_buf.pPingPongBuff2     = &u8TxDMABuffer[UART3_TX_DMA_BUF_LEN];
    uart3_tx_dma_buf.nBuff1MaxLength    = UART3_TX_DMA_BUF_LEN;
    uart3_tx_dma_buf.nBuff2MaxLength    = UART3_TX_DMA_BUF_LEN;
    uart3_tx_dma_buf.IdleBufferIndex    = 0;
    uart3_tx_dma_buf.nBuff1Offset       = 0;
    uart3_tx_dma_buf.nBuff2Offset       = 0;
    uart3_tx_dma_buf.IsDMAWroking       = 0;
    
    uart3_rx_dma_buf.pPingPongBuff1     = u8RxDMABuffer;
    uart3_rx_dma_buf.pPingPongBuff2     = &u8RxDMABuffer[UART3_RX_DMA_BUF_LEN];
    uart3_rx_dma_buf.nBuff1MaxLength    = UART3_RX_DMA_BUF_LEN;
    uart3_rx_dma_buf.nBuff2MaxLength    = UART3_RX_DMA_BUF_LEN;
    uart3_rx_dma_buf.IdleBufferIndex    = 1;
    uart3_rx_dma_buf.nBuff1Offset       = 0;
    uart3_rx_dma_buf.nBuff2Offset       = 0;
    uart3_rx_dma_buf.IsDMAWroking       = 0;
}
int Uart3Init(void)
{
    Uart3DmaBufferInit();
    UartDeviceDefaultInit(&uart3_device);
    uart3_device.num        = UART_NUM03;
    uart3_device.mode       = UART_DMA_MODE;
    uart3_device.baudrate   = B230400;
    uart3_device.ParityType = PARITY_EVEN; //PARITY_NONE,PARITY_EVEN ,PARITY_ODD;
    uart3_device.IRQPriority= IRQPriority11Uart23;
    uart3_device.pTxDMABuffer   = &uart3_tx_dma_buf;
    uart3_device.pRxDMABuffer   = &uart3_rx_dma_buf;
    
    xSerialTxHandleLock = xSemaphoreCreateMutex();
    xSerialRxHandleLock = xSemaphoreCreateMutex();
	xUart3RxEventGroup  = xEventGroupCreate();
	do{} while ((NULL == xSerialTxHandleLock) \
        ||(NULL == xSerialRxHandleLock)\
        ||(NULL == xUart3RxEventGroup));
    
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
    int rLen = 0;
    DmaUartProtocolPacket *pPacket = (DmaUartProtocolPacket *)pReadData;
    
    if (!uart3_device.IsDeviceOpen)
    {
        return -1;
    }
    
    if( pdTRUE != xSemaphoreTake( xSerialRxHandleLock, ( TickType_t ) 100 ))
    {
        return -2;
    }
    
    if (uart3_rx_dma_buf.IdleBufferIndex) //Buffer2 ready
    {
       rLen = MyMini(nDataLen, uart3_rx_dma_buf.nBuff2Offset);
       memcpy(pReadData, uart3_rx_dma_buf.pPingPongBuff2, rLen);
    }
    else //Buffer1 ready
    {
       rLen = MyMini(nDataLen, uart3_rx_dma_buf.nBuff1Offset);
       memcpy(pReadData, uart3_rx_dma_buf.pPingPongBuff1, rLen);
    }

    xSemaphoreGive( xSerialRxHandleLock );
    
    if (DMA_UART_PACKET_PARITY_ERR == xSerialRxParityFlag)
    {
        pPacket->ParityTag = DMA_UART_PACKET_PARITY_ERR;
        xSerialRxParityFlag = DMA_UART_PACKET_PARITY_OK;
    }
    else
    {
        pPacket->ParityTag = DMA_UART_PACKET_PARITY_OK;
    }
    
    return rLen;
}


int Uart3Write(char *pWriteData, const int nDataLen)
{
    int ready2writeLen = 0;
    
    if (!uart3_device.IsDeviceOpen)
    {
        return -1;
    }
    
    if( pdTRUE != xSemaphoreTake( xSerialTxHandleLock, ( TickType_t ) 100 ))
    {
        return -2;
    }

    if (uart3_tx_dma_buf.IdleBufferIndex) //buffer2 idle
    {
        ready2writeLen = uart3_tx_dma_buf.nBuff2Offset + nDataLen;
        if (ready2writeLen > uart3_tx_dma_buf.nBuff2MaxLength)
        {
            //Warnig: Not enugh spare to store.
        }
        else
        {
            memcpy( &uart3_tx_dma_buf.pPingPongBuff2[uart3_tx_dma_buf.nBuff2Offset],
                    pWriteData,
                    nDataLen);
            uart3_tx_dma_buf.nBuff2Offset += nDataLen;
        }
        if (!uart3_tx_dma_buf.IsDMAWroking)
        {
            DMA1_Channel2->CMAR = (uint32_t)uart3_tx_dma_buf.pPingPongBuff2;
            DMA1_Channel2->CNDTR = uart3_tx_dma_buf.nBuff2Offset;
            uart3_tx_dma_buf.IdleBufferIndex = 0;
            DMA_Cmd(DMA1_Channel2, ENABLE); // open DMA
            uart3_tx_dma_buf.IsDMAWroking = 1;
        }
    }
    else
    {
        ready2writeLen = uart3_tx_dma_buf.nBuff1Offset + nDataLen;
        if (ready2writeLen > uart3_tx_dma_buf.nBuff1MaxLength)
        {
            //Warnig: Not enugh spare to store.
        }
        else
        {
            memcpy( &uart3_tx_dma_buf.pPingPongBuff1[uart3_tx_dma_buf.nBuff1Offset],
                    pWriteData,
                    nDataLen);
            uart3_tx_dma_buf.nBuff1Offset += nDataLen;
        }
        if (!uart3_tx_dma_buf.IsDMAWroking)
        {
            DMA1_Channel2->CMAR = (uint32_t)uart3_tx_dma_buf.pPingPongBuff1;
            DMA1_Channel2->CNDTR = uart3_tx_dma_buf.nBuff1Offset;
            uart3_tx_dma_buf.IdleBufferIndex = 1;
            DMA_Cmd(DMA1_Channel2, ENABLE); // open DMA
            uart3_tx_dma_buf.IsDMAWroking = 1;
        }
    }
    
    xSemaphoreGive( xSerialTxHandleLock );
    return nDataLen;
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
    volatile u32 tem_reg;
    volatile u16 u16BufferUsedLen = 0;
    BaseType_t xHigherPriorityTaskWoken, xResult;

	// xHigherPriorityTaskWoken must be initialised to pdFALSE.
	xHigherPriorityTaskWoken = pdFALSE;
    
    // error happen
    if(USART_GetITStatus(USART3, USART_IT_PE) != RESET)
    {
        USART_ClearITPendingBit(USART3, USART_IT_PE);
        xSerialRxParityFlag = DMA_UART_PACKET_PARITY_ERR;
    }
    
    // uart idle interrupt
    if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)
    {
        USART_ClearITPendingBit(USART3, USART_IT_IDLE);
        DMA_ClearFlag(DMA1_FLAG_GL3);//clear all interrupt flags     
        DMA_Cmd(DMA1_Channel3, DISABLE); //close DMA incase receive data while handling
        
        xResult = xSemaphoreTakeFromISR( xSerialRxHandleLock, &xHigherPriorityTaskWoken);
        if( pdTRUE == xResult)
        {
            if (uart3_rx_dma_buf.IdleBufferIndex) //buf1 busy, buf2 idle
            {
                u16BufferUsedLen = uart3_rx_dma_buf.nBuff1MaxLength - DMA_GetCurrDataCounter(DMA1_Channel3); 
                if (u16BufferUsedLen > 0)
                {
                    uart3_rx_dma_buf.nBuff1Offset = u16BufferUsedLen;
                    DMA1_Channel3->CMAR = (uint32_t)uart3_rx_dma_buf.pPingPongBuff2;
                    DMA1_Channel3->CNDTR = uart3_rx_dma_buf.nBuff2MaxLength;
                    uart3_rx_dma_buf.IdleBufferIndex = 0;
                }
            }
            else
            {
                u16BufferUsedLen = uart3_rx_dma_buf.nBuff2MaxLength - DMA_GetCurrDataCounter(DMA1_Channel3); 
                if (u16BufferUsedLen > 0)
                {
                    uart3_rx_dma_buf.nBuff2Offset = u16BufferUsedLen;
                    DMA1_Channel3->CMAR = (uint32_t)uart3_rx_dma_buf.pPingPongBuff1;
                    DMA1_Channel3->CNDTR = uart3_rx_dma_buf.nBuff1MaxLength;
                    uart3_rx_dma_buf.IdleBufferIndex = 1;
                }
            }
            xResult = xSemaphoreGiveFromISR( xSerialRxHandleLock ,&xHigherPriorityTaskWoken);

            if (u16BufferUsedLen > 0)
            {
                //boardcast message to handle
            	xResult = xEventGroupSetBitsFromISR(
            						xUart3RxEventGroup,	// The event group being updated.
            						UART_DMA_RX_INCOMPLETE_EVENT_BIT,// The bits being set.
            						&xHigherPriorityTaskWoken );
            } //End if u16BufferUsedLen > 0
        }// End if pdTRUE == xSemaphoreTakeFromISR
        DMA_Cmd(DMA1_Channel3, ENABLE);                 //open DMA after handled
        
        //clear Idle flag by read SR and DR
        tem_reg = USART3->SR;
        tem_reg = USART3->DR;
        tem_reg = tem_reg; // slove warning 
    }// End if USART_IT_IDLE
    
    if(USART_GetITStatus(USART3, USART_IT_FE | USART_IT_NE) != RESET)
    {
        USART_ClearITPendingBit(USART3, USART_IT_FE | USART_IT_NE);
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
 Prototype    : DMA1_Channel3_IRQHandler
 Description  : uart3 DMA-Rx handler
 Input        : void  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2015/9/7
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
void DMA1_Channel3_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken, xResult;

	// xHigherPriorityTaskWoken must be initialised to pdFALSE.
	xHigherPriorityTaskWoken = pdFALSE;
    
    DMA_ClearITPendingBit(DMA1_IT_TC3); 
    DMA_Cmd(DMA1_Channel3, DISABLE);            //close DMA incase receive data while handling

    if( pdTRUE != xSemaphoreTakeFromISR( xSerialRxHandleLock, &xHigherPriorityTaskWoken))
    {
        return;
    }

    if (uart3_rx_dma_buf.IdleBufferIndex) //buf1 busy, buf2 idle
    {
        //buffer1 finished recevied mission (full), switch to buffer2
        uart3_rx_dma_buf.nBuff1Offset = uart3_rx_dma_buf.nBuff1MaxLength;
        
        DMA1_Channel3->CMAR = (uint32_t)uart3_rx_dma_buf.pPingPongBuff2;
        DMA1_Channel3->CNDTR = uart3_rx_dma_buf.nBuff2MaxLength;
        uart3_rx_dma_buf.IdleBufferIndex = 0;
    }
    else //buf2 busy, buf1 idle
    {
        //buffer2 finished recevied mission (full), switch to buffer1
        uart3_rx_dma_buf.nBuff2Offset = uart3_rx_dma_buf.nBuff2MaxLength;
        
        DMA1_Channel3->CMAR = (uint32_t)uart3_rx_dma_buf.pPingPongBuff1;
        DMA1_Channel3->CNDTR = uart3_rx_dma_buf.nBuff1MaxLength;
        uart3_rx_dma_buf.IdleBufferIndex = 1;
    }
    xSemaphoreGiveFromISR( xSerialRxHandleLock ,&xHigherPriorityTaskWoken);
    DMA_Cmd(DMA1_Channel3, ENABLE);             //open DMA after handled

    //boardcast message to handle
	xResult = xEventGroupSetBitsFromISR(
						xUart3RxEventGroup,	// The event group being updated.
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
 Prototype    : DMA1_Channel2_IRQHandler
 Description  : uart3 DMA-Tx Handler
 Input        : void  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2015/9/7
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
void DMA1_Channel2_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken, xResult;
    DMA_ClearITPendingBit(DMA1_IT_TC2);
    DMA_Cmd(DMA1_Channel2, DISABLE);    // close DMA
    
    xResult = xSemaphoreTakeFromISR( xSerialTxHandleLock, &xHigherPriorityTaskWoken);
    if( pdTRUE != xResult)
    {
        return;
    }

    //After finish this send mission, need to do: clear status --> check next mission
    if (uart3_tx_dma_buf.IdleBufferIndex)
    {
        // reset buffer1
        uart3_tx_dma_buf.nBuff1Offset = 0;

        // check for buffer2
        if (uart3_tx_dma_buf.nBuff2Offset > 0)
        {
            DMA1_Channel2->CMAR = (uint32_t)uart3_tx_dma_buf.pPingPongBuff2;
            DMA1_Channel2->CNDTR = uart3_tx_dma_buf.nBuff2Offset;
            uart3_tx_dma_buf.IdleBufferIndex = 0;
            DMA_Cmd(DMA1_Channel2, ENABLE); // open DMA
        }
        else
        {
            uart3_tx_dma_buf.IsDMAWroking = 0;
        }
    }
    else
    {
        //reset buffer2
        uart3_tx_dma_buf.nBuff2Offset = 0;
        
        // check for buffer1
        if (uart3_tx_dma_buf.nBuff1Offset > 0)
        {
            DMA1_Channel2->CMAR = (uint32_t)uart3_tx_dma_buf.pPingPongBuff1;
            DMA1_Channel2->CNDTR = uart3_tx_dma_buf.nBuff1Offset;
            uart3_tx_dma_buf.IdleBufferIndex = 1;
            DMA_Cmd(DMA1_Channel2, ENABLE); // open DMA
        }
        else
        {
            uart3_tx_dma_buf.IsDMAWroking = 0;
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

