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
static xTaskHandle xAioStmUpdateTaskHandle = NULL;
AioStmDev_TypeDef gAioStmDev;
AioStmDev_TypeDef *gpAioStmDev = &gAioStmDev;

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
static void AioStmUpdateTask(void *pvParameters);



int AioStmUpdateInit(void)
{
    gpAioStmDev->xPktAckEventGroup = xEventGroupCreate();
    do{} while (NULL == gpAioStmDev->xPktAckEventGroup);
    return 0;
}


int createAioStmUpdateTask(void)
{
    portBASE_TYPE ret;
    
    if (NULL != xAioStmUpdateTaskHandle)
    {
        vTaskDelete(xAioStmUpdateTaskHandle);
    }
    
    ret = xTaskCreate(  AioStmUpdateTask,
                        "AioStmUpdateTask",
                        configMINIMAL_STACK_SIZE,
                        NULL,
                        RUN_ONCE_TASK_PRIORITY,
                        &xAioStmUpdateTaskHandle);
    if (pdTRUE != ret)
    {
        return -1;
    }
    return 0;
}

int deleteAioStmUpdateTask(void)
{
    if (NULL != xAioStmUpdateTaskHandle)
    {
        vTaskDelete(xAioStmUpdateTaskHandle);
    }
    return 0;
}

static int setAioStmBoot0State(char pin_level)
{
    EventBits_t uxBits = 0;
    DmaUartProtocolPacket txPacket;
    const TickType_t xTicksToWait = 1000 / portTICK_PERIOD_MS;
    
    DmaUartProtocolPacketInit(&txPacket);
    txPacket.ID = PKT_ID_AIOSTM_UPDATE_BOOT;
    txPacket.DataLen = 1;
    txPacket.Data[0] = (u8)pin_level;
    txPacket.ACK = DMA_UART_PACKET_ACK;

    sendCoopMcuPkt(&txPacket, MY_TIM_DEFAULT_TIMEOUT_MS);

    //wait ack signal
    uxBits = xEventGroupWaitBits(
            gpAioStmDev->xPktAckEventGroup, // The event group being tested.
            PKT_ACK_BIT_AIOSTM_BOOT \
            | PKT_ACK_BIT_AIOSTM_ERROR, // The bits within the event group to wait for.
            pdTRUE,         // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
            pdFALSE,        // Don't wait for both bits, either bit will do.
            xTicksToWait ); // Wait a maximum of 100ms for either bit to be set.

    if(0 != (uxBits & PKT_ACK_BIT_AIOSTM_BOOT))
    {
        return 0;
    }
    else if(0 != (uxBits & PKT_ACK_BIT_AIOSTM_ERROR))
    {
        return 1;
    }
    else
    {
        return -1;
    }
}


static int sendAioStmUpdateStart(void)
{
    EventBits_t uxBits = 0;
    DmaUartProtocolPacket txPacket;
    const TickType_t xTicksToWait = 1000 / portTICK_PERIOD_MS;
    
    DmaUartProtocolPacketInit(&txPacket);
    txPacket.ID = PKT_ID_AIOSTM_UPDATE_START;
    txPacket.DataLen = 0;
    txPacket.ACK = DMA_UART_PACKET_ACK;

    sendCoopMcuPkt(&txPacket, MY_TIM_DEFAULT_TIMEOUT_MS);

    //wait ack signal
    uxBits = xEventGroupWaitBits(
            gpAioStmDev->xPktAckEventGroup, // The event group being tested.
            PKT_ACK_BIT_AIOSTM_START \
            | PKT_ACK_BIT_AIOSTM_ERROR, // The bits within the event group to wait for.
            pdTRUE,         // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
            pdFALSE,        // Don't wait for both bits, either bit will do.
            xTicksToWait ); // Wait a maximum of 100ms for either bit to be set.

    if(0 != (uxBits & PKT_ACK_BIT_AIOSTM_START))
    {
        return 0;
    }
    else if(0 != (uxBits & PKT_ACK_BIT_AIOSTM_ERROR))
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
            gpAioStmDev->xPktAckEventGroup, // The event group being tested.
            PKT_ACK_BIT_AIOSTM_END \
            | PKT_ACK_BIT_AIOSTM_ERROR, // The bits within the event group to wait for.
            pdTRUE,             // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
            pdFALSE,            // Don't wait for both bits, either bit will do.
            DELAY_MAX_WAIT );   // Wait a maximum of 100ms for either bit to be set.

    if(0 != (uxBits & PKT_ACK_BIT_AIOSTM_END))
    {
        return 0;
    }
    else if(0 != (uxBits & PKT_ACK_BIT_AIOSTM_ERROR))
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

static void AioStmUpdateTask(void *pvParameters)
{    
    char err_flag = 1;
    char sw = SW_OFF;
    
    /* Just to stop compiler warnings. */
    ( void ) pvParameters;
    
    INFO("AioStmUpdateTask running...\n");
    for (;;)
    {
        //make the test board into BOOT mode
        if (0 != setAioStmBoot0State(PIN_HIGH)) //BOOT0 = 1
        {
            break;
        }

        sw = SW_OFF;
        AioBoardCtrl(CTRL_CMD_AIOBOARD_SET_POWER, &sw);
        vTaskDelay(1000);
        sw = SW_ON;
        AioBoardCtrl(CTRL_CMD_AIOBOARD_SET_POWER, &sw);
        vTaskDelay(1000);

        //Send CO-MCU AioStmUpdateTask
        if (0 != sendAioStmUpdateStart())
        {
            break;
        }

        //Wait for End
        if (0 != waitAioStmUpdateEnd())
        {
            break;
        }
        
        //End of Task
        if (0 != setAioStmBoot0State(PIN_LOW)) //BOOT0 = 0
        {
            break;
        }

        //Normal finished!
        err_flag = 0;
    }
    
    if (err_flag)
    {
        //TODO: error hanppen
    }
    vTaskDelete(NULL); //delete myself
}

