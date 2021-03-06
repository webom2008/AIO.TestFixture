/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : uart5_test.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/8
  Last Modified :
  Description   : test uart5 driver
  Function List :
  History       :
  1.Date        : 2015/9/8
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/

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
static u32 test_uart5_rx_count;
static u32 test_uart5_tx_count;
/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/

#define _TEST_UART5_INFO_
#ifdef _TEST_UART5_INFO_
#define TEST_UART5_INFO(fmt, arg...) udprintf("\r\n[TEST] "fmt, ##arg)
#else
#define TEST_UART5_INFO(fmt, arg...) do{}while(0)
#endif

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/


static void uart5_driver_task(void *pvParameters)
{
    unsigned int test_count = 0;
    const TickType_t xTicksToWait = 1000 / portTICK_PERIOD_MS; //delay 1s
    char txBuf[32];
    
	/* Just to stop compiler warnings. */
	( void ) pvParameters;
    
    udprintf("\r\n[TEST] uart5_driver_task running...");
    for (;;)
    {
        TEST_UART5_INFO(">>uart5_driver_task :%d,read_count=%d",test_count++,test_uart5_rx_count);
        memset(txBuf, 0x00, sizeof(txBuf));
        sprintf(txBuf, "Uart5Write:%d", test_count);
        Uart5Write(txBuf, strlen(txBuf));
        
        vTaskDelay(xTicksToWait);
    }
}


static void uart5_unpack_task(void *pvParameters)
{
    int rLen = 0;
    char rxBuf[32];
    const TickType_t xTicksToWait = 100 / portTICK_PERIOD_MS;
    
	/* Just to stop compiler warnings. */
	( void ) pvParameters;
    
    udprintf("\r\n[TEST] uart5_unpack_task running...");
    for (;;)
    {
        rLen = 0;
        memset(rxBuf, 0x00, sizeof(rxBuf));
        rLen = Uart5Read(rxBuf, sizeof(rxBuf));
        if (rLen > 0)
        {
            test_uart5_rx_count += rLen;
            TEST_UART5_INFO(">>uart5_unpack_task rLen:%d",rLen);
        }
        vTaskDelay(xTicksToWait);
    }
}

