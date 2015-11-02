/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : AioStmUpdate.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/15
  Last Modified :
  Description   : AIO-STM Update interrface
  Function List :
  History       :
  1.Date        : 2015/9/15
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#include "includes.h"

/*----------------------------------------------*
 * external variables                           *
 *----------------------------------------------*/
extern EventGroupHandle_t xCoopMCUPktAckEventGroup;

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

int AioStmUpdateInit(void)
{
    return 0;
}

static int setAioStmBoot0State(char pin_level)
{
    EventBits_t uxBits = 0;
    DmaUartProtocolPacket txPacket;
    const TickType_t xTicksToWait = 2000 / portTICK_PERIOD_MS;
    
    DmaUartProtocolPacketInit(&txPacket);
    txPacket.ID = PKT_ID_AIOSTM_UPDATE_BOOT;
    txPacket.DataLen = 1;
    txPacket.Data[0] = (u8)pin_level;
    txPacket.ACK = DMA_UART_PACKET_ACK;

    sendCoopMcuPkt(&txPacket, MY_TIM_DEFAULT_TIMEOUT_MS);

    //wait ack signal
    uxBits = xEventGroupWaitBits(
            xCoopMCUPktAckEventGroup, // The event group being tested.
            COOPMCU_PKT_ACK_BIT_AIOSTM_BOOT \
            | COOPMCU_PKT_ACK_BIT_AIOSTM_ERROR, // The bits within the event group to wait for.
            pdTRUE,         // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
            pdFALSE,        // Don't wait for both bits, either bit will do.
            xTicksToWait ); // Wait a maximum of 100ms for either bit to be set.

    if(0 != (uxBits & COOPMCU_PKT_ACK_BIT_AIOSTM_BOOT))
    {
        return 0;
    }
    else if(0 != (uxBits & COOPMCU_PKT_ACK_BIT_AIOSTM_ERROR))
    {
        return 1;
    }
    else //timeout
    {
        return -1;
    }
}


static int sendAioStmUpdateStart(void)
{
    EventBits_t uxBits = 0;
    DmaUartProtocolPacket txPacket;
    const TickType_t xTicksToWait = 2000 / portTICK_PERIOD_MS;
    
    DmaUartProtocolPacketInit(&txPacket);
    txPacket.ID = PKT_ID_AIOSTM_UPDATE_START;
    txPacket.DataLen = 0;
    txPacket.ACK = DMA_UART_PACKET_ACK;

    sendCoopMcuPkt(&txPacket, MY_TIM_DEFAULT_TIMEOUT_MS);

    //wait ack signal
    uxBits = xEventGroupWaitBits(
            xCoopMCUPktAckEventGroup, // The event group being tested.
            COOPMCU_PKT_ACK_BIT_AIOSTM_START \
            | COOPMCU_PKT_ACK_BIT_AIOSTM_ERROR, // The bits within the event group to wait for.
            pdTRUE,         // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
            pdFALSE,        // Don't wait for both bits, either bit will do.
            xTicksToWait ); // Wait a maximum of 100ms for either bit to be set.

    if(0 != (uxBits & COOPMCU_PKT_ACK_BIT_AIOSTM_START))
    {
        return 0;
    }
    else if(0 != (uxBits & COOPMCU_PKT_ACK_BIT_AIOSTM_ERROR))
    {
        return 1;
    }
    else
    {
        return -1;
    }
}


static int waitAioStmUpdateEnd(void)
{
    EventBits_t uxBits = 0;
    
    uxBits = xEventGroupWaitBits(
            xCoopMCUPktAckEventGroup, // The event group being tested.
            COOPMCU_PKT_ACK_BIT_AIOSTM_END \
            | COOPMCU_PKT_ACK_BIT_AIOSTM_ERROR, // The bits within the event group to wait for.
            pdTRUE,             // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
            pdFALSE,            // Don't wait for both bits, either bit will do.
            DELAY_MAX_WAIT );   // Wait a maximum of 100ms for either bit to be set.

    if(0 != (uxBits & COOPMCU_PKT_ACK_BIT_AIOSTM_END))
    {
        return 0;
    }
    else if(0 != (uxBits & COOPMCU_PKT_ACK_BIT_AIOSTM_ERROR))
    {
        return 1;
    }
    else
    {
        return -1;
    }
}


static void resetAioBoardPower(void)
{
    char sw = SW_OFF;
    sw = SW_OFF;
    AioBoardCtrl(AIOBRD_CTRL_SET_PWR, &sw);
    vTaskDelay(1000);
    sw = SW_ON;
    AioBoardCtrl(AIOBRD_CTRL_SET_PWR, &sw);
    vTaskDelay(1000);
}

int sendAndWaitAIOStmBoot(void)
{
    //make the test board into BOOT mode
    if (0 != setAioStmBoot0State(PIN_HIGH)) //BOOT0 = 1
    {
        return -1;
    }
        
    resetAioBoardPower();

    //Send CO-MCU AioStmUpdateTask
    if (0 != sendAioStmUpdateStart())
    {
        return -2;
    }

    //Wait for End
    if (0 != waitAioStmUpdateEnd())
    {
        return -3;
    }
        
    //End of Task
    if (0 != setAioStmBoot0State(PIN_LOW)) //BOOT0 = 0
    {
        return -4;
    }

    resetAioBoardPower();
    return 0;//Normal finished!
}

