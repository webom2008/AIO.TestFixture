/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : uart1_test.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/3
  Last Modified :
  Description   : test for uart1
  Function List :
  History       :
  1.Date        : 2015/9/3
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/

/*----------------------------------------------*
 * external variables                           *
 *----------------------------------------------*/
#ifdef CONFIG_UART1_DMA_MODE
extern EventGroupHandle_t xUart1RxEventGroup;
#endif

/*----------------------------------------------*
 * external routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * internal routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * project-wide global variables                *
 *----------------------------------------------*/
static u32 test_uart1_rx_count;
static u32 test_uart1_tx_count;

/*----------------------------------------------*
 * module-wide global variables                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/

#define _TEST_INFO_
#ifdef _TEST_INFO_
#define TEST_INFO(fmt, arg...) udprintf("\r\n[TEST] "fmt, ##arg)
#else
#define TEST_INFO(fmt, arg...) do{}while(0)
#endif

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/



static void uart_driver_task(void *pvParameters)
{
#ifdef CONFIG_UART1_INT_MODE
    char rBuf[64];
    int rLen = 0;
#endif
    unsigned int test_count = 0;
    const TickType_t xTicksToWait = 1000 / portTICK_PERIOD_MS; //delay 1s
	/* Just to stop compiler warnings. */
	( void ) pvParameters;
    
    udprintf("\r\n[TEST] uart_driver_task running...");
    for (;;)
    {
        TEST_INFO(">>uart_driver_task :%d",test_count++);
        TEST_INFO(">>Uart1Write Testing...");
#ifdef CONFIG_UART1_INT_MODE
        TEST_INFO(">>Uart1Read :");
        memset(rBuf, 0x00, sizeof(rBuf));
        rLen = Uart1Read(rBuf, sizeof(rBuf));
        if (rLen > 0)
        {
            TEST_INFO("rLen=%d :",rLen);
            if (rLen >= sizeof(rBuf)) rLen = sizeof(rBuf) -1;
            rBuf[rLen] = '\0';
            TEST_INFO("%s",rBuf);
        }
        else
        {
            TEST_INFO("Empty");
        }
#endif
        udprintf("\r\ntotoal read count=%d",test_uart1_rx_count);
        vTaskDelay(xTicksToWait);
    }
}


#ifdef CONFIG_UART1_DMA_MODE
static void uart1_unpack_task(void *pvParameters)
{
    int rLen = 0;
    char rBuf[64];
    EventBits_t uxBits;
    const TickType_t xTicksToWait = 100 / portTICK_PERIOD_MS;
    
	/* Just to stop compiler warnings. */
	( void ) pvParameters;
    
    udprintf("\r\n[TEST] uart1_unpack_task running...");
    for (;;)
    {
//        udprintf("\r\n>>uart1_unpack_task Testing...");
		uxBits = xEventGroupWaitBits(
					xUart1RxEventGroup,	// The event group being tested.
					UART1_DMA_RX_COMPLETE_EVENT_BIT \
					| UART1_DMA_RX_INCOMPLETE_EVENT_BIT,	// The bits within the event group to wait for.
					pdTRUE,			// BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
					pdFALSE,		// Don't wait for both bits, either bit will do.
					xTicksToWait );	// Wait a maximum of 100ms for either bit to be set.

		if( ( uxBits & UART1_DMA_RX_COMPLETE_EVENT_BIT ) != 0 )
		{
            rLen = Uart1Read(rBuf, sizeof(rBuf));
            test_uart1_rx_count += rLen;
            TEST_INFO("Uart1Read COMPLETE rLen=%d",rLen);
		}
		else if( ( uxBits & UART1_DMA_RX_INCOMPLETE_EVENT_BIT ) != 0 )
		{
            rLen = Uart1Read(rBuf, sizeof(rBuf));
            test_uart1_rx_count += rLen;
            TEST_INFO("Uart1Read INCOMPLETE rLen=%d",rLen);
		}
		else
		{
		}
    }
}
#endif
