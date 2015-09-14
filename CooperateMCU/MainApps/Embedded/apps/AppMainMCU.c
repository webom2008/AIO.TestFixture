/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : AppMainMCU.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/12
  Last Modified :
  Description   : communicate with main mcu
  Function List :
  History       :
  1.Date        : 2015/9/12
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#include "includes.h"
/*----------------------------------------------*
 * external variables                           *
 *----------------------------------------------*/
extern EventGroupHandle_t xUart2RxEventGroup;

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
static QueueHandle_t    pMainMcuRxPktQueue  = NULL;

/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/
//#define _SEND_DEMO_PKT_
//#define _INFO_
#define _ERROR_

#ifdef _INFO_
#define INFO(fmt, arg...) udprintf("[MainMCU]Info: "fmt, ##arg)
#else
#define INFO(fmt, arg...) do{}while(0)
#endif
#ifdef _ERROR_
#define ERROR(fmt, arg...) udprintf("[MainMCU]Error: "fmt, ##arg)
#else
#define ERROR(fmt, arg...) do{}while(0)
#endif

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

static void MainMcuExecutePktTask(void *pvParameters)
{
    DmaUartProtocolPacket rxPacket;
    
	/* Just to stop compiler warnings. */
	( void ) pvParameters;
    
    INFO("MainMcuExecutePktTask running...\n");
    for (;;)
    {
        if(xQueueReceive(pMainMcuRxPktQueue, (void *)&rxPacket, DELAY_MAX_WAIT))
        {
            INFO("PKT_ID=0X%02X, Data=%s\n",rxPacket.ID ,rxPacket.Data);
            switch (rxPacket.ID)
            {
            case PKT_ID_DRIVER_TEST:
                break;
            case PKT_ID_CONNECTED:
                break;
            default:
                INFO("PKT_ID=0X%02X unKnown!\n",rxPacket.ID);
                break;
            }
        }

    }
}

static void MainMcuUnpackTask(void *pvParameters)
{
    int rLen = 0;
    DmaUartProtocolPacket rxPacket;
    EventBits_t uxBits;
    const TickType_t xTicksToWait = 100 / portTICK_PERIOD_MS;
#ifdef _SEND_DEMO_PKT_
    int test_count = 0;
#endif
    
	/* Just to stop compiler warnings. */
	( void ) pvParameters;
    
    INFO("MainMcuUnpackTask running...\n");
    for (;;)
    {
        rLen = 0;
		uxBits = xEventGroupWaitBits(
					xUart2RxEventGroup,	// The event group being tested.
					UART_DMA_RX_COMPLETE_EVENT_BIT \
					| UART_DMA_RX_INCOMPLETE_EVENT_BIT,	// The bits within the event group to wait for.
					pdTRUE,			// BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
					pdFALSE,		// Don't wait for both bits, either bit will do.
					xTicksToWait );	// Wait a maximum of 100ms for either bit to be set.

        memset(&rxPacket, 0x00, sizeof(DmaUartProtocolPacket));
        if( ( uxBits & UART_DMA_RX_COMPLETE_EVENT_BIT ) != 0 )
		{
            rLen = Uart2Read((char *)&rxPacket, sizeof(DmaUartProtocolPacket));
            INFO("Uart2Read COMPLETE rLen=%d\n",rLen);
		}
		else if( ( uxBits & UART_DMA_RX_INCOMPLETE_EVENT_BIT ) != 0 )
		{
            rLen = Uart2Read((char *)&rxPacket, sizeof(DmaUartProtocolPacket));
            INFO("Uart2Read INCOMPLETE rLen=%d\n",rLen);
		}
		else
		{
#ifdef _SEND_DEMO_PKT_ // For Create Tx Test Packet
            DmaUartProtocolPacketInit(&rxPacket);
            rxPacket.ID = (u8)PKT_ID_DRIVER_TEST;
            sprintf((char *)rxPacket.Data, "%d", test_count++);
            Uart2Write((char *)&rxPacket, sizeof(DmaUartProtocolPacket));
            continue;
#endif
            // do nothing.
		}

        if (rLen <= 0) continue;
        
        if ((DMA_UART_START_HEADER_TAG == rxPacket.StartHeader ) \
            && (DMA_UART_END_HEADER_TAG == rxPacket.EndHeader)  \
            && (DMA_UART_PACKET_PARITY_OK == rxPacket.ParityTag))
        {
//            INFO("PKT_ID=0X%02X, Data=%s\n",rxPacket.ID ,rxPacket.Data);
    		xQueueSendToBack(pMainMcuRxPktQueue, (void *)&rxPacket, DELAY_NO_WAIT);
        }
        else
        {
            INFO("PKT ERROR\n");
        }
    }
}


int AppMainMcuInit(void)
{
    int ret = 0;
    
    pMainMcuRxPktQueue  = xQueueCreate(2, sizeof(DmaUartProtocolPacket));
    
	if(NULL == pMainMcuRxPktQueue)
	{
        ERROR("Create pMainMcuRxPktQueue Failed!");
        ret = -1;
    }
    while (ret < 0); //error hanppen
    return 0;
}

int AppMainMcuStart(void)
{
#ifndef CONFIG_DRIVER_TEST_UART2
    xTaskCreate(MainMcuUnpackTask,
                "MainMcuUnpackTask",
                configMINIMAL_STACK_SIZE,
                NULL,
                MAIN_MCU_UNPACK_TASK_PRIORITY,
                NULL);
    xTaskCreate(MainMcuExecutePktTask,
                "MainMcuExecutePktTask",
                configMINIMAL_STACK_SIZE,
                NULL,
                MAIN_MCU_EXE_PKT_TASK_PRIORITY,
                NULL);
#endif /* CONFIG_DRIVER_TEST_UART2 */

    return 0;
}

