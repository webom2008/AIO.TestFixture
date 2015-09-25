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


static void uart1_driver_task(void *pvParameters)
{
    unsigned int test_count = 0;
    const TickType_t xTicksToWait = 1000 / portTICK_PERIOD_MS; //delay 1s
    /* Just to stop compiler warnings. */
    ( void ) pvParameters;
    
    udprintf("\r\n[TEST] uart1_driver_task running...");
    for (;;)
    {
        TEST_INFO(">>uart1_driver_task :%d",test_count++);
        TEST_INFO(">>totoal read count=%d",test_uart1_rx_count);
        vTaskDelay(xTicksToWait);
    }
}


#ifdef CONFIG_UART1_INT_MODE
static void uart1_unpack_task(void *pvParameters)
{
    int rLen = 0;
    char rBuf[100];
    const TickType_t xTicksToWait = 4 / portTICK_PERIOD_MS;
    
    /* Just to stop compiler warnings. */
    ( void ) pvParameters;

    udprintf("\r\n[TEST] uart1_unpack_task running...");
    for (;;)
    {
        rLen = 0;
        memset(rBuf, 0x00, sizeof(rBuf));
        rLen = Uart1Read(rBuf, sizeof(rBuf));
        if (rLen > 0)
        {
            test_uart1_rx_count += rLen;
        }
        vTaskDelay(xTicksToWait);
    }
}
#endif


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
                                    xUart1RxEventGroup,                 // The event group being tested.
                                    UART_DMA_RX_COMPLETE_EVENT_BIT \
                                    | UART_DMA_RX_INCOMPLETE_EVENT_BIT, // The bits within the event group to wait for.
                                    pdTRUE,                             // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
                                    pdFALSE,                            // Don't wait for both bits, either bit will do.
                                    xTicksToWait );                     // Wait a maximum of 100ms for either bit to be set.

        if( ( uxBits & UART_DMA_RX_COMPLETE_EVENT_BIT ) != 0 )
        {
            rLen = Uart1Read(rBuf, sizeof(rBuf));
            test_uart1_rx_count += rLen;
            TEST_INFO("Uart1Read COMPLETE rLen=%d",rLen);
        }
        else if( ( uxBits & UART_DMA_RX_INCOMPLETE_EVENT_BIT ) != 0 )
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
