/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : uart4_test.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/8
  Last Modified :
  Description   : test uart4 driver
  Function List :
  History       :
  1.Date        : 2015/9/8
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/

/*----------------------------------------------*
 * external variables                           *
 *----------------------------------------------*/
extern EventGroupHandle_t xUart4RxEventGroup;
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
static u32 test_uart4_rx_count;
static u32 test_uart4_tx_count;
/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/

#define _TEST_UART4_INFO_
#ifdef _TEST_UART4_INFO_
#define TEST_UART4_INFO(fmt, arg...) udprintf("[TEST] "fmt, ##arg)
#else
#define TEST_UART4_INFO(fmt, arg...) do{}while(0)
#endif

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/


static void uart4_driver_task(void *pvParameters)
{
    unsigned int test_count = 0;
    const TickType_t xTicksToWait = 1000 / portTICK_PERIOD_MS; //delay 1s
    char txBuf[32];
    
	/* Just to stop compiler warnings. */
	( void ) pvParameters;
    
    udprintf("[TEST] uart4_driver_task running...\r\n");
    for (;;)
    {
        TEST_UART4_INFO(">>uart4_driver_task :%d read_count=%d\r\n",test_count++, test_uart4_rx_count);
        memset(txBuf, 0x00, sizeof(txBuf));
        sprintf(txBuf, "Uart4Write:%d\r\n", test_count);
        Uart4Write(txBuf, strlen(txBuf));
        
        vTaskDelay(xTicksToWait);
    }
}


static void uart4_unpack_task(void *pvParameters)
{
    int rLen = 0;
    char rxBuf[UART4_RX_DMA_BUF_LEN];
    EventBits_t uxBits;
    
	/* Just to stop compiler warnings. */
	( void ) pvParameters;
    
    udprintf("[TEST] uart4_unpack_task running...\r\n");
    for (;;)
    {
        rLen = 0;
		uxBits = xEventGroupWaitBits(
					xUart4RxEventGroup,	                // The event group being tested.
					UART_DMA_RX_COMPLETE_EVENT_BIT \
					| UART_DMA_RX_INCOMPLETE_EVENT_BIT,	// The bits within the event group to wait for.
					pdTRUE,			                    // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
					pdFALSE,		                    // Don't wait for both bits, either bit will do.
					DELAY_MAX_WAIT );	                // Wait a maximum for either bit to be set.

        memset(rxBuf, 0x00, UART4_RX_DMA_BUF_LEN);
        if (0 != ( uxBits & UART_DMA_RX_COMPLETE_EVENT_BIT ) \
            || (0 != ( uxBits & UART_DMA_RX_COMPLETE_EVENT_BIT)))
		{
            rLen = Uart4Read(rxBuf, UART4_RX_DMA_BUF_LEN);
            if (rLen > 0)
            {
                test_uart4_rx_count += rLen;
                TEST_UART4_INFO(">>uart4_unpack_task rLen:%d\r\n",rLen);
            }
		}
    }
}

