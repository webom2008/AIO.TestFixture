/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : uart3_test.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/8
  Last Modified :
  Description   : test uart3 driver
  Function List :
  History       :
  1.Date        : 2015/9/8
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/

/*----------------------------------------------*
 * external variables                           *
 *----------------------------------------------*/
extern EventGroupHandle_t xUart3RxEventGroup;

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
static u32 test_uart3_rx_count;
static u32 test_uart3_tx_count;
/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/

#define _TEST_UART3_INFO_
#ifdef _TEST_UART3_INFO_
#define TEST_UART3_INFO(fmt, arg...) udprintf("\r\n[TEST] "fmt, ##arg)
#else
#define TEST_UART3_INFO(fmt, arg...) do{}while(0)
#endif

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/


static void uart3_driver_task(void *pvParameters)
{
    unsigned int test_count = 0;
    const TickType_t xTicksToWait = 1000 / portTICK_PERIOD_MS; //delay 1s
    DmaUartProtocolPacket txPacket;
    
    /* Just to stop compiler warnings. */
    ( void ) pvParameters;
    
    udprintf("\r\n[TEST] uart3_driver_task running...");
    for (;;)
    {
        TEST_UART3_INFO(">>uart3_driver_task :%d",test_count++);
        DmaUartProtocolPacketInit(&txPacket);
        txPacket.ID = (u8)PKT_ID_DRIVER_TEST;
        sprintf((char *)txPacket.Data, "%d", test_count);
        Uart3Write((char *)&txPacket, sizeof(DmaUartProtocolPacket));
        TEST_UART3_INFO(">>totoal read count=%d",test_uart3_rx_count);
        vTaskDelay(xTicksToWait);
    }
}


static void uart3_unpack_task(void *pvParameters)
{
    int rLen = 0;
    DmaUartProtocolPacket rxPacket;
    EventBits_t uxBits;
    const TickType_t xTicksToWait = 100 / portTICK_PERIOD_MS;
    
    /* Just to stop compiler warnings. */
    ( void ) pvParameters;
    
    udprintf("\r\n[TEST] uart3_unpack_task running...");
    for (;;)
    {
        rLen = 0;
        uxBits = xEventGroupWaitBits(
                xUart3RxEventGroup, // The event group being tested.
                UART_DMA_RX_COMPLETE_EVENT_BIT \
                | UART_DMA_RX_INCOMPLETE_EVENT_BIT, // The bits within the event group to wait for.
                pdTRUE,         // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
                pdFALSE,        // Don't wait for both bits, either bit will do.
                xTicksToWait ); // Wait a maximum of 100ms for either bit to be set.

        memset(&rxPacket, 0x00, sizeof(DmaUartProtocolPacket));
        if( ( uxBits & UART_DMA_RX_COMPLETE_EVENT_BIT ) != 0 )
        {
            rLen = Uart3Read((char *)&rxPacket, sizeof(DmaUartProtocolPacket));
            if (rLen > 0)
            {
                test_uart3_rx_count += rLen;
            }
            TEST_UART3_INFO("Uart3Read COMPLETE rLen=%d",rLen);
        }
        else if( ( uxBits & UART_DMA_RX_INCOMPLETE_EVENT_BIT ) != 0 )
        {
            rLen = Uart3Read((char *)&rxPacket, sizeof(DmaUartProtocolPacket));
            if (rLen > 0)
            {
                test_uart3_rx_count += rLen;
            }
            TEST_UART3_INFO("Uart3Read INCOMPLETE rLen=%d",rLen);
        }
        else
        {
        }

        if (rLen <= 0) continue;
        
        if ((DMA_UART_START_HEADER_TAG == rxPacket.StartHeader ) \
            && (DMA_UART_END_HEADER_TAG == rxPacket.EndHeader)  \
            && (DMA_UART_PACKET_PARITY_OK == rxPacket.ParityTag))
        {
            TEST_UART3_INFO("PKT_ID=0X%03X, Data=%s",rxPacket.ID ,rxPacket.Data);
        }
        else
        {
            TEST_UART3_INFO("PKT ERROR");
        }
    }
}

