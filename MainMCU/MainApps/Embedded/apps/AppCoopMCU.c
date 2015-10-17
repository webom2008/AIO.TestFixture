/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : AppCoopMCU.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/16
  Last Modified :
  Description   : cooperate MCU communicate
  Function List :
  History       :
  1.Date        : 2015/9/16
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#include "includes.h"

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
EventGroupHandle_t xCoopMCUPktAckEventGroup = NULL;

/*----------------------------------------------*
 * module-wide global variables                 *
 *----------------------------------------------*/
static QueueHandle_t    pCoopMcuRxPktQueue  = NULL;
static int gAIOBoardCurrent;
/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/

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
static void CoopMcuTimeoutPktTask(void *pvParameters);
static void CoopMcuUnpackTask(void *pvParameters);
static void CoopMcuExecutePktTask(void *pvParameters);



int AppCoopMcuInit(void)
{
    int ret = 0;
    
    pCoopMcuRxPktQueue  = xQueueCreate(2, sizeof(DmaUartProtocolPacket));
    xCoopMCUPktAckEventGroup = xEventGroupCreate();
    
    ret |= CoopMcuProtocolInit();
    ret |= AioStmUpdateInit();
    
    do{} while ((NULL == xCoopMCUPktAckEventGroup) \
                || (NULL == pCoopMcuRxPktQueue) \
                || (ret < 0));
    return 0;

}

int AppCoopMcuStart(void)
{
#ifndef CONFIG_DRIVER_TEST_UART3
    while (pdPASS != xTaskCreate(   CoopMcuUnpackTask,
                                    "CoopMcuUnpackTask",
                                    configMINIMAL_STACK_SIZE,
                                    NULL,
                                    COOP_MCU_UNPACK_TASK_PRIORITY,
                                    NULL));
    while (pdPASS != xTaskCreate(   CoopMcuExecutePktTask,
                                    "CoopMcuExecutePktTask",
                                    configMINIMAL_STACK_SIZE,
                                    NULL,
                                    COOP_MCU_EXE_PKT_TASK_PRIORITY,
                                    NULL));
    while (pdPASS != xTaskCreate(   CoopMcuTimeoutPktTask,
                                    "CoopMcuTimeoutPktTask",
                                    configMINIMAL_STACK_SIZE,
                                    NULL,
                                    COOP_MCU_TIMEOUT_PKT_TASK_PRIORITY,
                                    NULL));
#endif /* CONFIG_DRIVER_TEST_UART3 */

    return 0;
}

static void CoopMcuTimeoutPktTask(void *pvParameters)
{
    const TickType_t xTicksToWait = 100 / portTICK_PERIOD_MS;
    
    /* Just to stop compiler warnings. */
    ( void ) pvParameters;
    
    INFO("CoopMcuTimeoutPktTask running...\n");
    for (;;)
    {
        checkAndResendCoopMcuACKPkt();
        INFO("CoopMcuTimeoutPktTask running %d\n",getMyTimerTick());
        vTaskDelay(xTicksToWait);
    }
}


static void CoopMcuUnpackTask(void *pvParameters)
{
    int rLen = 0;
    DmaUartProtocolPacket rxPacket;
    EventBits_t uxBits;
    const TickType_t xTicksToWait = 100 / portTICK_PERIOD_MS;

    /* Just to stop compiler warnings. */
    ( void ) pvParameters;

    INFO("MainMcuUnpackTask running...\n");
    for (;;)
    {
        rLen = 0;
        uxBits = xEventGroupWaitBits(
                    xUart3RxEventGroup,     // The event group being tested.
                    UART_DMA_RX_COMPLETE_EVENT_BIT \
                    | UART_DMA_RX_INCOMPLETE_EVENT_BIT, // The bits within the event group to wait for.
                    pdTRUE,         // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
                    pdFALSE,        // Don't wait for both bits, either bit will do.
                    xTicksToWait ); // Wait a maximum of 100ms for either bit to be set.

        memset(&rxPacket, 0x00, sizeof(DmaUartProtocolPacket));
        if( ( uxBits & UART_DMA_RX_COMPLETE_EVENT_BIT ) != 0 )
        {
            rLen = Uart3Read((char *)&rxPacket, sizeof(DmaUartProtocolPacket));
            INFO("Uart3Read COMPLETE rLen=%d\n",rLen);
        }
        else if( ( uxBits & UART_DMA_RX_INCOMPLETE_EVENT_BIT ) != 0 )
        {
            rLen = Uart3Read((char *)&rxPacket, sizeof(DmaUartProtocolPacket));
            INFO("Uart3Read INCOMPLETE rLen=%d\n",rLen);
        }
        else
        {
            // do nothing.
        }

        if (rLen <= 0) continue;
        
        if ((DMA_UART_START_HEADER_TAG == rxPacket.StartHeader ) \
            && (DMA_UART_END_HEADER_TAG == rxPacket.EndHeader)  \
            && (DMA_UART_PACKET_PARITY_OK == rxPacket.ParityTag))
        {
//            INFO("PKT_ID=0X%02X, Data=%s\n",rxPacket.ID ,rxPacket.Data);
            xQueueSendToBack(pCoopMcuRxPktQueue, (void *)&rxPacket, DELAY_NO_WAIT);
        }
        else
        {
            INFO("PKT ERROR\n");
        }
    }
}

static void testCoopMcuAndMyself(DmaUartProtocolPacket *pPkt)
{
    udprintf("PKT_ID=0X%02X, Data=%s\r\n",pPkt->ID ,pPkt->Data);
}

int getAIOBaordCurrent(void)
{
    return gAIOBoardCurrent;
}

static void CoopMcuExecutePktTask(void *pvParameters)
{
    DmaUartProtocolPacket rxPacket;
    
    /* Just to stop compiler warnings. */
    ( void ) pvParameters;
    
    INFO("MainMcuExecutePktTask running...\n");
    for (;;)
    {
        if(xQueueReceive(pCoopMcuRxPktQueue, (void *)&rxPacket, DELAY_MAX_WAIT))
        {
            INFO("PKT_ID=0X%02X, Data=%s\n",rxPacket.ID ,rxPacket.Data);
            
            if (DMA_UART_PACKET_NACK == rxPacket.ACK) //delete pkt from send buffer
            {
                deleteCoopMcuAckPkt(rxPacket.ID);
            }
            
            switch (rxPacket.ID)
            {
            case PKT_ID_DRIVER_TEST:
                testCoopMcuAndMyself(&rxPacket);
                break;
            case PKT_ID_AIOSTM_UPDATE_START:
                xEventGroupSetBits( gpAioStmDev->xPktAckEventGroup, 
                                    PKT_ACK_BIT_AIOSTM_START);
                break;
            case PKT_ID_AIOSTM_UPDATE_ERROR:
//                deleteAioStmUpdateTask();
                xEventGroupSetBits( gpAioStmDev->xPktAckEventGroup, 
                                    PKT_ACK_BIT_AIOSTM_ERROR);
                break;
            case PKT_ID_AIOSTM_UPDATE_BOOT:
                xEventGroupSetBits( gpAioStmDev->xPktAckEventGroup, 
                                    PKT_ACK_BIT_AIOSTM_BOOT);
                break;
            case PKT_ID_AIOSTM_UPDATE_END:
                xEventGroupSetBits( gpAioStmDev->xPktAckEventGroup, 
                                    PKT_ACK_BIT_AIOSTM_END);
                break;
            case PKT_ID_TDM_RESULT:
                gAIOBoardCurrent = (int)((rxPacket.Data[0]<<24) \
                                    | (rxPacket.Data[1]<<16) \
                                    | (rxPacket.Data[2]<<8) \
                                    | rxPacket.Data[3]);
//                udprintf("TDM = %d\r\n",gAIOBoardCurrent);
                xEventGroupSetBits( xCoopMCUPktAckEventGroup, 
                                    COOPMCU_PKT_ACK_BIT_TDM);
                break;
            default:
                INFO("PKT_ID=0X%02X unKnown!\n",rxPacket.ID);
                break;
            }
        }
    }
}


