/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : driver_uart2.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/2
  Last Modified :
  Description   : uart2 driver
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
EventGroupHandle_t xUart2RxEventGroup;  // Declare a variable to hold the created event group.

/*----------------------------------------------*
 * module-wide global variables                 *
 *----------------------------------------------*/
static UART_DEVICE_TypeDef uart2_device;

#define UART2_TX_DMA_BUF_LEN        sizeof(DmaUartProtocolPacket)
#define UART2_RX_DMA_BUF_LEN        sizeof(DmaUartProtocolPacket)

static u8 u8TxDMABuffer[UART2_TX_DMA_BUF_LEN*2];
static u8 u8RxDMABuffer[UART2_RX_DMA_BUF_LEN*2];
static UART_DMA_BUFFER_TypeDef  uart2_tx_dma_buf;
static UART_DMA_BUFFER_TypeDef  uart2_rx_dma_buf;

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

static void Uart2DmaBufferInit(void)
{
    uart2_tx_dma_buf.pPingPongBuff1     = u8TxDMABuffer;
    uart2_tx_dma_buf.pPingPongBuff2     = &u8TxDMABuffer[UART2_TX_DMA_BUF_LEN];
    uart2_tx_dma_buf.nBuff1MaxLength    = UART2_TX_DMA_BUF_LEN;
    uart2_tx_dma_buf.nBuff2MaxLength    = UART2_TX_DMA_BUF_LEN;
    uart2_tx_dma_buf.IdleBufferIndex    = 0;
    uart2_tx_dma_buf.nBuff1Offset       = 0;
    uart2_tx_dma_buf.nBuff2Offset       = 0;
    uart2_tx_dma_buf.IsDMAWroking       = 0;
    
    uart2_rx_dma_buf.pPingPongBuff1     = u8RxDMABuffer;
    uart2_rx_dma_buf.pPingPongBuff2     = &u8RxDMABuffer[UART2_RX_DMA_BUF_LEN];
    uart2_rx_dma_buf.nBuff1MaxLength    = UART2_RX_DMA_BUF_LEN;
    uart2_rx_dma_buf.nBuff2MaxLength    = UART2_RX_DMA_BUF_LEN;
    uart2_rx_dma_buf.IdleBufferIndex    = 1;
    uart2_rx_dma_buf.nBuff1Offset       = 0;
    uart2_rx_dma_buf.nBuff2Offset       = 0;
    uart2_rx_dma_buf.IsDMAWroking       = 0;
}

/*****************************************************************************
 Prototype    : Uart2Init
 Description  : uart driver init
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
int Uart2Init(void)
{
    Uart2DmaBufferInit();
    UartDeviceDefaultInit(&uart2_device);
    uart2_device.num            = UART_NUM02;
    uart2_device.mode           = UART_DMA_MODE;
    uart2_device.baudrate       = B230400;
    uart2_device.ParityType     = PARITY_EVEN; //PARITY_NONE,PARITY_EVEN ,PARITY_ODD;
    uart2_device.IRQPriority    = IRQPriority11Uart23;
    uart2_device.pTxDMABuffer   = &uart2_tx_dma_buf;
    uart2_device.pRxDMABuffer   = &uart2_rx_dma_buf;
    
    xSerialTxHandleLock = xSemaphoreCreateMutex();
    xSerialRxHandleLock = xSemaphoreCreateMutex();
	xUart2RxEventGroup  = xEventGroupCreate();
	do{} while ((NULL == xSerialTxHandleLock) \
        ||(NULL == xSerialRxHandleLock)\
        ||(NULL == xUart2RxEventGroup));
    
    return 0;
}

/*****************************************************************************
 Prototype    : Uart2Open
 Description  : uart2 driver open
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
int Uart2Open(void)
{
    int ret = 0;

    if (uart2_device.IsDeviceOpen)
    {
        return ret;
    }
    
    ret |= UartCommonInit(&uart2_device);

    if(ret < 0)
    {
        return ret;
    }
    
    uart2_device.IsDeviceOpen = true;
    return 0;
}

/*****************************************************************************
 Prototype    : Uart2Read
 Description  : uart2 read function
 Input        : char *pReadData     
                const int nDataLen  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2015/9/7
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
int Uart2Read(char *pReadData, const int nDataLen)
{
    int rLen = 0;
    DmaUartProtocolPacket *pPacket = (DmaUartProtocolPacket *)pReadData;
    
    if (!uart2_device.IsDeviceOpen)
    {
        return -1;
    }
    
    if( pdTRUE != xSemaphoreTake( xSerialRxHandleLock, ( TickType_t ) 100 ))
    {
        return -2;
    }
    
    if (uart2_rx_dma_buf.IdleBufferIndex) //Buffer2 ready
    {
       rLen = MyMini(nDataLen, uart2_rx_dma_buf.nBuff2Offset);
       memcpy(pReadData, uart2_rx_dma_buf.pPingPongBuff2, rLen);
    }
    else //Buffer1 ready
    {
       rLen = MyMini(nDataLen, uart2_rx_dma_buf.nBuff1Offset);
       memcpy(pReadData, uart2_rx_dma_buf.pPingPongBuff1, rLen);
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

/*****************************************************************************
 Prototype    : Uart2Write
 Description  : uart2 write function
 Input        : char *pWriteData    
                const int nDataLen  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2015/9/7
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
int Uart2Write(char *pWriteData, const int nDataLen)
{
    int ready2writeLen = 0;
    
    if (!uart2_device.IsDeviceOpen)
    {
        return -1;
    }
    
    if( pdTRUE != xSemaphoreTake( xSerialTxHandleLock, ( TickType_t ) 100 ))
    {
        return -2;
    }

    if (uart2_tx_dma_buf.IdleBufferIndex) //buffer2 idle
    {
        ready2writeLen = uart2_tx_dma_buf.nBuff2Offset + nDataLen;
        if (ready2writeLen > uart2_tx_dma_buf.nBuff2MaxLength)
        {
            //Warnig: Not enugh spare to store.
        }
        else
        {
            memcpy( &uart2_tx_dma_buf.pPingPongBuff2[uart2_tx_dma_buf.nBuff2Offset],
                    pWriteData,
                    nDataLen);
            uart2_tx_dma_buf.nBuff2Offset += nDataLen;
        }
        if (!uart2_tx_dma_buf.IsDMAWroking)
        {
            DMA1_Channel7->CMAR = (uint32_t)uart2_tx_dma_buf.pPingPongBuff2;
            DMA1_Channel7->CNDTR = uart2_tx_dma_buf.nBuff2Offset;
            uart2_tx_dma_buf.IdleBufferIndex = 0;
            DMA_Cmd(DMA1_Channel7, ENABLE); // open DMA
            uart2_tx_dma_buf.IsDMAWroking = 1;
        }
    }
    else
    {
        ready2writeLen = uart2_tx_dma_buf.nBuff1Offset + nDataLen;
        if (ready2writeLen > uart2_tx_dma_buf.nBuff1MaxLength)
        {
            //Warnig: Not enugh spare to store.
        }
        else
        {
            memcpy( &uart2_tx_dma_buf.pPingPongBuff1[uart2_tx_dma_buf.nBuff1Offset],
                    pWriteData,
                    nDataLen);
            uart2_tx_dma_buf.nBuff1Offset += nDataLen;
        }
        if (!uart2_tx_dma_buf.IsDMAWroking)
        {
            DMA1_Channel7->CMAR = (uint32_t)uart2_tx_dma_buf.pPingPongBuff1;
            DMA1_Channel7->CNDTR = uart2_tx_dma_buf.nBuff1Offset;
            uart2_tx_dma_buf.IdleBufferIndex = 1;
            DMA_Cmd(DMA1_Channel7, ENABLE); // open DMA
            uart2_tx_dma_buf.IsDMAWroking = 1;
        }
    }
    
    xSemaphoreGive( xSerialTxHandleLock );
    return nDataLen;
}

/*****************************************************************************
 Prototype    : Uart2Ctrl
 Description  : uart2 control function
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
int Uart2Ctrl(void)
{

    return 0;
}

/*****************************************************************************
 Prototype    : Uart2Close
 Description  : uart2 drive close
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
int Uart2Close(void)
{
    if (uart2_device.IsDeviceOpen)
    {
        UartCommonTerminate(&uart2_device);
        uart2_device.IsDeviceOpen = false;
    }
    return 0;
}


/*****************************************************************************
 Prototype    : USART2_IRQHandler
 Description  : uart2 interrupt handler
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
void USART2_IRQHandler(void)
{
    volatile u32 tem_reg;
    volatile u16 u16BufferUsedLen = 0;
    BaseType_t xHigherPriorityTaskWoken, xResult;

	// xHigherPriorityTaskWoken must be initialised to pdFALSE.
	xHigherPriorityTaskWoken = pdFALSE;
    
    // error happen
    if(USART_GetITStatus(USART2, USART_IT_PE) != RESET)
    {
        USART_ClearITPendingBit(USART2, USART_IT_PE);
        xSerialRxParityFlag = DMA_UART_PACKET_PARITY_ERR;
    }
    
    // uart idle interrupt
    if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
    {
        USART_ClearITPendingBit(USART2, USART_IT_IDLE);
        DMA_ClearFlag(DMA1_FLAG_GL6);//clear all interrupt flags     
        DMA_Cmd(DMA1_Channel6, DISABLE); //close DMA incase receive data while handling
        
        xResult = xSemaphoreTakeFromISR( xSerialRxHandleLock, &xHigherPriorityTaskWoken);
        if( pdTRUE == xResult)
        {
            if (uart2_rx_dma_buf.IdleBufferIndex) //buf1 busy, buf2 idle
            {
                u16BufferUsedLen = uart2_rx_dma_buf.nBuff1MaxLength - DMA_GetCurrDataCounter(DMA1_Channel6); 
                if (u16BufferUsedLen > 0)
                {
                    uart2_rx_dma_buf.nBuff1Offset = u16BufferUsedLen;
                    DMA1_Channel6->CMAR = (uint32_t)uart2_rx_dma_buf.pPingPongBuff2;
                    DMA1_Channel6->CNDTR = uart2_rx_dma_buf.nBuff2MaxLength;
                    uart2_rx_dma_buf.IdleBufferIndex = 0;
                }
            }
            else
            {
                u16BufferUsedLen = uart2_rx_dma_buf.nBuff2MaxLength - DMA_GetCurrDataCounter(DMA1_Channel6); 
                if (u16BufferUsedLen > 0)
                {
                    uart2_rx_dma_buf.nBuff2Offset = u16BufferUsedLen;
                    DMA1_Channel6->CMAR = (uint32_t)uart2_rx_dma_buf.pPingPongBuff1;
                    DMA1_Channel6->CNDTR = uart2_rx_dma_buf.nBuff1MaxLength;
                    uart2_rx_dma_buf.IdleBufferIndex = 1;
                }
            }
            xResult = xSemaphoreGiveFromISR( xSerialRxHandleLock ,&xHigherPriorityTaskWoken);

            if (u16BufferUsedLen > 0)
            {
                //boardcast message to handle
            	xResult = xEventGroupSetBitsFromISR(
            						xUart2RxEventGroup,	// The event group being updated.
            						UART_DMA_RX_INCOMPLETE_EVENT_BIT,// The bits being set.
            						&xHigherPriorityTaskWoken );
            } //End if u16BufferUsedLen > 0
        }// End if pdTRUE == xSemaphoreTakeFromISR
        DMA_Cmd(DMA1_Channel6, ENABLE);                 //open DMA after handled
        
        //clear Idle flag by read SR and DR
        tem_reg = USART2->SR;
        tem_reg = USART2->DR;
        tem_reg = tem_reg; // slove warning 
    }// End if USART_IT_IDLE
    
    if(USART_GetITStatus(USART2, USART_IT_FE | USART_IT_NE) != RESET)
    {
        USART_ClearITPendingBit(USART2, USART_IT_FE | USART_IT_NE);
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
 Prototype    : DMA1_Channel6_IRQHandler
 Description  : uart2 DMA-Rx handler
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
void DMA1_Channel6_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken, xResult;

	// xHigherPriorityTaskWoken must be initialised to pdFALSE.
	xHigherPriorityTaskWoken = pdFALSE;
    
    DMA_ClearITPendingBit(DMA1_IT_TC6); 
//    DMA_ClearITPendingBit(DMA1_IT_TE6);
    DMA_Cmd(DMA1_Channel6, DISABLE);            //close DMA incase receive data while handling

    if( pdTRUE != xSemaphoreTakeFromISR( xSerialRxHandleLock, &xHigherPriorityTaskWoken))
    {
        return;
    }

    if (uart2_rx_dma_buf.IdleBufferIndex) //buf1 busy, buf2 idle
    {
        //buffer1 finished recevied mission (full), switch to buffer2
        uart2_rx_dma_buf.nBuff1Offset = uart2_rx_dma_buf.nBuff1MaxLength;
        
        DMA1_Channel6->CMAR = (uint32_t)uart2_rx_dma_buf.pPingPongBuff2;
        DMA1_Channel6->CNDTR = uart2_rx_dma_buf.nBuff2MaxLength;
        uart2_rx_dma_buf.IdleBufferIndex = 0;
    }
    else //buf2 busy, buf1 idle
    {
        //buffer2 finished recevied mission (full), switch to buffer1
        uart2_rx_dma_buf.nBuff2Offset = uart2_rx_dma_buf.nBuff2MaxLength;
        
        DMA1_Channel6->CMAR = (uint32_t)uart2_rx_dma_buf.pPingPongBuff1;
        DMA1_Channel6->CNDTR = uart2_rx_dma_buf.nBuff1MaxLength;
        uart2_rx_dma_buf.IdleBufferIndex = 1;
    }
    xSemaphoreGiveFromISR( xSerialRxHandleLock ,&xHigherPriorityTaskWoken);
    DMA_Cmd(DMA1_Channel6, ENABLE);             //open DMA after handled

    //boardcast message to handle
	xResult = xEventGroupSetBitsFromISR(
						xUart2RxEventGroup,	// The event group being updated.
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
 Prototype    : DMA1_Channel7_IRQHandler
 Description  : uart2 DMA-Tx Handler
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
void DMA1_Channel7_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken, xResult;
    DMA_ClearITPendingBit(DMA1_IT_TC7);
//    DMA_ClearITPendingBit(DMA1_IT_TE7);
    DMA_Cmd(DMA1_Channel7, DISABLE);    // close DMA
    
    xResult = xSemaphoreTakeFromISR( xSerialTxHandleLock, &xHigherPriorityTaskWoken);
    if( pdTRUE != xResult)
    {
        return;
    }

    //After finish this send mission, need to do: clear status --> check next mission
    if (uart2_tx_dma_buf.IdleBufferIndex)
    {
        // reset buffer1
        uart2_tx_dma_buf.nBuff1Offset = 0;

        // check for buffer2
        if (uart2_tx_dma_buf.nBuff2Offset > 0)
        {
            DMA1_Channel7->CMAR = (uint32_t)uart2_tx_dma_buf.pPingPongBuff2;
            DMA1_Channel7->CNDTR = uart2_tx_dma_buf.nBuff2Offset;
            uart2_tx_dma_buf.IdleBufferIndex = 0;
            DMA_Cmd(DMA1_Channel7, ENABLE); // open DMA
        }
        else
        {
            uart2_tx_dma_buf.IsDMAWroking = 0;
        }
    }
    else
    {
        //reset buffer2
        uart2_tx_dma_buf.nBuff2Offset = 0;
        
        // check for buffer1
        if (uart2_tx_dma_buf.nBuff1Offset > 0)
        {
            DMA1_Channel7->CMAR = (uint32_t)uart2_tx_dma_buf.pPingPongBuff1;
            DMA1_Channel7->CNDTR = uart2_tx_dma_buf.nBuff1Offset;
            uart2_tx_dma_buf.IdleBufferIndex = 1;
            DMA_Cmd(DMA1_Channel7, ENABLE); // open DMA
        }
        else
        {
            uart2_tx_dma_buf.IsDMAWroking = 0;
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

