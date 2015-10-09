/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : driver_uart4_dma.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/25
  Last Modified :
  Description   : uart4 dma mode
  Function List :
  History       :
  1.Date        : 2015/9/25
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
EventGroupHandle_t xUart4RxEventGroup;  // Declare a variable to hold the created event group.

/*----------------------------------------------*
 * module-wide global variables                 *
 *----------------------------------------------*/
static UART_DEVICE_TypeDef uart4_device;

static u8 u8TxDMABuffer[UART4_TX_DMA_BUF_LEN*2];
static u8 u8RxDMABuffer[UART4_RX_DMA_BUF_LEN*2];
static UART_DMA_BUFFER_TypeDef  uart4_tx_dma_buf;
static UART_DMA_BUFFER_TypeDef  uart4_rx_dma_buf;

static xSemaphoreHandle xSerialTxHandleLock = NULL;
static xSemaphoreHandle xSerialRxHandleLock = NULL;

static u8 volatile xSerialRxParityFlag = DMA_UART_PACKET_PARITY_OK;
/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/
#define _INFO_
#define _ERROR_

#ifdef _INFO_
#define INFO(fmt, arg...) udprintf("[Uart4]Info: "fmt, ##arg)
#else
#define INFO(fmt, arg...) do{}while(0)
#endif
#ifdef _ERROR_
#define ERROR(fmt, arg...) udprintf("[Uart4]Error: "fmt, ##arg)
#else
#define ERROR(fmt, arg...) do{}while(0)
#endif
/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/
static void Uart4DmaBufferInit(void)
{
    uart4_tx_dma_buf.pPingPongBuff1     = u8TxDMABuffer;
    uart4_tx_dma_buf.pPingPongBuff2     = &u8TxDMABuffer[UART4_TX_DMA_BUF_LEN];
    uart4_tx_dma_buf.nBuff1MaxLength    = UART4_TX_DMA_BUF_LEN;
    uart4_tx_dma_buf.nBuff2MaxLength    = UART4_TX_DMA_BUF_LEN;
    uart4_tx_dma_buf.IdleBufferIndex    = 0;
    uart4_tx_dma_buf.nBuff1Offset       = 0;
    uart4_tx_dma_buf.nBuff2Offset       = 0;
    uart4_tx_dma_buf.IsDMAWroking       = 0;
    
    uart4_rx_dma_buf.pPingPongBuff1     = u8RxDMABuffer;
    uart4_rx_dma_buf.pPingPongBuff2     = &u8RxDMABuffer[UART4_RX_DMA_BUF_LEN];
    uart4_rx_dma_buf.nBuff1MaxLength    = UART4_RX_DMA_BUF_LEN;
    uart4_rx_dma_buf.nBuff2MaxLength    = UART4_RX_DMA_BUF_LEN;
    uart4_rx_dma_buf.IdleBufferIndex    = 1; /* init for buffer2 idle for received*/
    uart4_rx_dma_buf.nBuff1Offset       = 0;
    uart4_rx_dma_buf.nBuff2Offset       = 0;
    uart4_rx_dma_buf.IsDMAWroking       = 0;
}

int Uart4Init(void)
{
    Uart4DmaBufferInit();
    UartDeviceDefaultInit(&uart4_device);
    uart4_device.num            = UART_NUM04;
    uart4_device.mode           = UART_DMA_MODE;
    uart4_device.baudrate       = B230400;
    uart4_device.ParityType     = PARITY_NONE; //PARITY_NONE,PARITY_EVEN ,PARITY_ODD;
    uart4_device.IRQPriority    = IRQPriority11Uart4;
    uart4_device.pTxDMABuffer   = &uart4_tx_dma_buf;
    uart4_device.pRxDMABuffer   = &uart4_rx_dma_buf;
    
    xSerialTxHandleLock = xSemaphoreCreateMutex();
    xSerialRxHandleLock = xSemaphoreCreateMutex();
    xUart4RxEventGroup  = xEventGroupCreate();
    do{} while ((NULL == xSerialTxHandleLock) \
        ||(NULL == xSerialRxHandleLock)\
        ||(NULL == xUart4RxEventGroup));
    
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
    int rLen = 0;
    if (!uart4_device.IsDeviceOpen)
    {
        return -1;
    }
    
    if( pdTRUE != xSemaphoreTake( xSerialRxHandleLock, ( TickType_t ) 100 ))
    {
        ERROR("Uart4Read Rx Lock\r\n");
        return -2;
    }
    
    if (uart4_rx_dma_buf.IdleBufferIndex) //Buffer2 ready
    {
       rLen = MyMini(nDataLen, uart4_rx_dma_buf.nBuff2Offset);
       memcpy(pReadData, uart4_rx_dma_buf.pPingPongBuff2, rLen);
    }
    else //Buffer1 ready
    {
       rLen = MyMini(nDataLen, uart4_rx_dma_buf.nBuff1Offset);
       memcpy(pReadData, uart4_rx_dma_buf.pPingPongBuff1, rLen);
    }

    xSemaphoreGive( xSerialRxHandleLock );
    return rLen;
}


int Uart4Write(char *pWriteData, const int nDataLen)
{
    int ready2writeLen = 0;
    
    if (!uart4_device.IsDeviceOpen)
    {
        return -1;
    }
    
    if( pdTRUE != xSemaphoreTake( xSerialTxHandleLock, ( TickType_t ) 100 ))
    {
        return -2;
    }

    if (uart4_tx_dma_buf.IdleBufferIndex) //buffer2 idle
    {
        ready2writeLen = uart4_tx_dma_buf.nBuff2Offset + nDataLen;
        if (ready2writeLen > uart4_tx_dma_buf.nBuff2MaxLength)
        {
            //Warnig: Not enugh spare to store.
        }
        else
        {
            memcpy( &uart4_tx_dma_buf.pPingPongBuff2[uart4_tx_dma_buf.nBuff2Offset],
                    pWriteData,
                    nDataLen);
            uart4_tx_dma_buf.nBuff2Offset += nDataLen;
        }
        if (!uart4_tx_dma_buf.IsDMAWroking)
        {
            DMA2_Channel5->CMAR = (uint32_t)uart4_tx_dma_buf.pPingPongBuff2;
            DMA2_Channel5->CNDTR = uart4_tx_dma_buf.nBuff2Offset;
            uart4_tx_dma_buf.IdleBufferIndex = 0;
            DMA_Cmd(DMA2_Channel5, ENABLE); // open DMA
            uart4_tx_dma_buf.IsDMAWroking = 1;
        }
    }
    else
    {
        ready2writeLen = uart4_tx_dma_buf.nBuff1Offset + nDataLen;
        if (ready2writeLen > uart4_tx_dma_buf.nBuff1MaxLength)
        {
            //Warnig: Not enugh spare to store.
        }
        else
        {
            memcpy( &uart4_tx_dma_buf.pPingPongBuff1[uart4_tx_dma_buf.nBuff1Offset],
                    pWriteData,
                    nDataLen);
            uart4_tx_dma_buf.nBuff1Offset += nDataLen;
        }
        if (!uart4_tx_dma_buf.IsDMAWroking)
        {
            DMA2_Channel5->CMAR = (uint32_t)uart4_tx_dma_buf.pPingPongBuff1;
            DMA2_Channel5->CNDTR = uart4_tx_dma_buf.nBuff1Offset;
            uart4_tx_dma_buf.IdleBufferIndex = 1;
            DMA_Cmd(DMA2_Channel5, ENABLE); // open DMA
            uart4_tx_dma_buf.IsDMAWroking = 1;
        }
    }
    
    xSemaphoreGive( xSerialTxHandleLock );
    return nDataLen;
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
    volatile u32 tem_reg;
    volatile u16 u16BufferUsedLen = 0;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE; // must be initialised to pdFALSE.
    BaseType_t xResult = pdFAIL;
    
    // uart idle interrupt
    if(USART_GetITStatus(UART4, USART_IT_IDLE) != RESET)
    {
        if (RESET == DMA_GetFlagStatus(DMA2_FLAG_TC3))
        {
            if (pdTRUE == xSemaphoreTakeFromISR( xSerialRxHandleLock, &xHigherPriorityTaskWoken))
            { 
                DMA_Cmd(DMA2_Channel3, DISABLE);    //close DMA must < 43us(1s/23040byte = 43 us/byte)
                DMA_ClearFlag(DMA2_FLAG_GL3);       //clear all interrupt flags     
                u16BufferUsedLen = UART4_RX_DMA_BUF_LEN - DMA_GetCurrDataCounter(DMA2_Channel3); 
                if (u16BufferUsedLen > 0)
                {
                    if (uart4_rx_dma_buf.IdleBufferIndex) //buf1 busy, buf2 idle
                    {
                        uart4_rx_dma_buf.nBuff1Offset = u16BufferUsedLen;
                        uart4_rx_dma_buf.IdleBufferIndex = 0;
                        DMA2_Channel3->CMAR = (uint32_t)uart4_rx_dma_buf.pPingPongBuff2;
                    }
                    else
                    {
                        uart4_rx_dma_buf.nBuff2Offset = u16BufferUsedLen;
                        uart4_rx_dma_buf.IdleBufferIndex = 1;
                        DMA2_Channel3->CMAR = (uint32_t)uart4_rx_dma_buf.pPingPongBuff1;
                    }
                    DMA2_Channel3->CNDTR = UART4_RX_DMA_BUF_LEN;
                }
                
                if(RESET != USART_GetFlagStatus(UART4, USART_FLAG_RXNE))
                {
                    ERROR("UART4_IRQ lost one RXNE!\r\n");
                }
                DMA_Cmd(DMA2_Channel3, ENABLE);     //open DMA after handled
                xResult = xSemaphoreGiveFromISR( xSerialRxHandleLock ,&xHigherPriorityTaskWoken);
            }
            else
            {
                ERROR("IRQ DMA-Rx Lock\r\n");
            }
            
            if (u16BufferUsedLen > 0)
            {
                //boardcast message to handle
                xResult = xEventGroupSetBitsFromISR(
                                                    xUart4RxEventGroup,                 // The event group being updated.
                                                    UART_DMA_RX_INCOMPLETE_EVENT_BIT,   // The bits being set.
                                                    &xHigherPriorityTaskWoken );
            } //End if u16BufferUsedLen > 0
        }
        
        USART_ReceiveData(UART4); // Clear IDLE interrupt flag bit
    }// End if USART_IT_IDLE
    
    // error happen
//    if(USART_GetITStatus(UART4, USART_IT_PE) != RESET)
//    {
//        USART_ClearITPendingBit(UART4, USART_IT_PE);
//        xSerialRxParityFlag = DMA_UART_PACKET_PARITY_ERR;
//    }
//    
//    if(USART_GetITStatus(UART4, USART_IT_FE | USART_IT_NE) != RESET)
//    {
//        USART_ClearITPendingBit(UART4, USART_IT_FE | USART_IT_NE);
//    }

    if( pdPASS == xResult )
    {
        // If xHigherPriorityTaskWoken is now set to pdTRUE then a context
        // switch should be requested.  The macro used is port specific and 
        // will be either portYIELD_FROM_ISR() or portEND_SWITCHING_ISR() - 
        // refer to the documentation page for the port being used.
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }
}

/*****************************************************************************
 Prototype    : DMA2_Channel3_IRQHandler
 Description  : uart4 DMA-Rx handler
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
void DMA2_Channel3_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE; // must be initialised to pdFALSE.
    BaseType_t xResult = pdFAIL;

    if( pdTRUE != xSemaphoreTakeFromISR( xSerialRxHandleLock, &xHigherPriorityTaskWoken))
    {
        ERROR("DMA-Rx Lock\r\n");
        return;
    }
    
    //close DMA must < 43us(1s/23040byte = 43 us/byte)
    //close DMA incase receive data while handling
    DMA_Cmd(DMA2_Channel3, DISABLE); 
    DMA2_Channel3->CNDTR = UART4_RX_DMA_BUF_LEN;
    if (uart4_rx_dma_buf.IdleBufferIndex) //buf1 busy, buf2 idle
    {
        //buffer1 finished recevied mission (full), switch to buffer2
        uart4_rx_dma_buf.nBuff1Offset = UART4_RX_DMA_BUF_LEN;
        uart4_rx_dma_buf.IdleBufferIndex = 0;
        DMA2_Channel3->CMAR = (uint32_t)uart4_rx_dma_buf.pPingPongBuff2;
    }
    else //buf2 busy, buf1 idle
    {
        //buffer2 finished recevied mission (full), switch to buffer1
        uart4_rx_dma_buf.nBuff2Offset = UART4_RX_DMA_BUF_LEN;
        uart4_rx_dma_buf.IdleBufferIndex = 1;
        DMA2_Channel3->CMAR = (uint32_t)uart4_rx_dma_buf.pPingPongBuff1;
    }
    DMA_Cmd(DMA2_Channel3, ENABLE);             //open DMA after handled
    xSemaphoreGiveFromISR( xSerialRxHandleLock ,&xHigherPriorityTaskWoken);
    DMA_ClearITPendingBit(DMA2_IT_TC3);
    
    //boardcast message to handle
    xResult = xEventGroupSetBitsFromISR(
                                        xUart4RxEventGroup,             // The event group being updated.
                                        UART_DMA_RX_COMPLETE_EVENT_BIT, // The bits being set.
                                        &xHigherPriorityTaskWoken );

    // Was the message posted successfully?
    if( pdPASS == xResult )
    {
        // If xHigherPriorityTaskWoken is now set to pdTRUE then a context
        // switch should be requested.  The macro used is port specific and 
        // will be either portYIELD_FROM_ISR() or portEND_SWITCHING_ISR() - 
        // refer to the documentation page for the port being used.
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }
}

/*****************************************************************************
 Prototype    : DMA2_Channel4_5_IRQHandler
 Description  : uart4 DMA-Tx Handler
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
void DMA2_Channel4_5_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE; // must be initialised to pdFALSE.
    BaseType_t xResult = pdFAIL;

    if(RESET != DMA_GetITStatus(DMA2_IT_TC5))
    {
        DMA_ClearITPendingBit(DMA2_IT_TC5);
        DMA_Cmd(DMA2_Channel5, DISABLE);    // close DMA
        
        xResult = xSemaphoreTakeFromISR( xSerialTxHandleLock, &xHigherPriorityTaskWoken);
        if( pdTRUE != xResult)
        {
            return;
        }

        //After finish this send mission, need to do: clear status --> check next mission
        if (uart4_tx_dma_buf.IdleBufferIndex)
        {
            // reset buffer1
            uart4_tx_dma_buf.nBuff1Offset = 0;

            // check for buffer2
            if (uart4_tx_dma_buf.nBuff2Offset > 0)
            {
                DMA2_Channel5->CMAR = (uint32_t)uart4_tx_dma_buf.pPingPongBuff2;
                DMA2_Channel5->CNDTR = uart4_tx_dma_buf.nBuff2Offset;
                uart4_tx_dma_buf.IdleBufferIndex = 0;
                DMA_Cmd(DMA2_Channel5, ENABLE); // open DMA
            }
            else
            {
                uart4_tx_dma_buf.IsDMAWroking = 0;
            }
        }
        else
        {
            //reset buffer2
            uart4_tx_dma_buf.nBuff2Offset = 0;
            
            // check for buffer1
            if (uart4_tx_dma_buf.nBuff1Offset > 0)
            {
                DMA2_Channel5->CMAR = (uint32_t)uart4_tx_dma_buf.pPingPongBuff1;
                DMA2_Channel5->CNDTR = uart4_tx_dma_buf.nBuff1Offset;
                uart4_tx_dma_buf.IdleBufferIndex = 1;
                DMA_Cmd(DMA2_Channel5, ENABLE); // open DMA
            }
            else
            {
                uart4_tx_dma_buf.IsDMAWroking = 0;
            }
        }
    
        xResult = xSemaphoreGiveFromISR( xSerialTxHandleLock , &xHigherPriorityTaskWoken);
    }
    else
    {

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

