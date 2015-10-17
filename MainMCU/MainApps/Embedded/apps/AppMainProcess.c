/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : AppMainProcess.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/10/16
  Last Modified :
  Description   : app for main testfixture process: download and test
  Function List :
  History       :
  1.Date        : 2015/10/16
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
static void MainProcessTask(void *pvParameters);

/*----------------------------------------------*
 * project-wide global variables                *
 *----------------------------------------------*/
static xTaskHandle xMainProcessTaskHandle = NULL;
static u32 u32CreateAppCount;
/*----------------------------------------------*
 * module-wide global variables                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/
enum
{
    STATE_AIOBOARD_DETECT_READY,
    STATE_AIOBOARD_POWERUP,
    STATE_AIOBOARD_DETECT_CURRENT,
    STATE_DETECT_D3V3E_POWER,
    STATE_DETECT_OTHER_POWER,

    
    STATE_PROCESS_SUCCESS,
};

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/

#define AIOBOARD_CURRENT_MAX        ((int)100)








#define _INFO_
#define _ERROR_

#ifdef _INFO_
#define INFO(fmt, arg...) udprintf("[Process]Info: "fmt, ##arg)
#else
#define INFO(fmt, arg...) do{}while(0)
#endif
#ifdef _ERROR_
#define ERROR(fmt, arg...) udprintf("[Process]Error: "fmt, ##arg)
#else
#define ERROR(fmt, arg...) do{}while(0)
#endif
/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

int MainProcessInit(void)
{
    return 0;
}

int createMainProcessTask(void)
{
    portBASE_TYPE ret;
    
    ret = xTaskCreate(  MainProcessTask,
                        "MainProcessTask",
                        2*configMINIMAL_STACK_SIZE,
                        NULL,
                        RUN_ONCE_TASK_PRIORITY,
                        &xMainProcessTaskHandle);
    if (pdTRUE != ret)
    {
        return -1;
    }
    u32CreateAppCount++;
    return 0;
}

bool isMainProcessTaskRuning(void)
{
    if (NULL != xMainProcessTaskHandle)
    {
        return true;
    }
    return false;
}

static int testAIOBaordReady(void)
{
    Keys_Val keyVal = KEY_IDLE;
    
    KeysRead(KEY_2, &keyVal);
    if (KEY_PRESSED == keyVal)
    {
        return 1;
    }
    return 0;
}

static int testAIOBaordCurrent(void)
{
    DmaUartProtocolPacket txPacket;
    int current = 0;
    int i;
    
    DmaUartProtocolPacketInit(&txPacket);
    txPacket.ID = (u8)PKT_ID_TDM_RESULT;
    txPacket.ACK = DMA_UART_PACKET_ACK;

    for (i = 0; i < 5; i++)
    {
        sendCoopMcuPkt(&txPacket, 1000);
        
        xEventGroupWaitBits(
                xCoopMCUPktAckEventGroup,   // The event group being tested.
                COOPMCU_PKT_ACK_BIT_TDM,    // The bits within the event group to wait for.
                pdTRUE,                     // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
                pdFALSE,                    // Don't wait for both bits, either bit will do.
                DELAY_MAX_WAIT );           // Wait a maximum of for either bit to be set.

        current += getAIOBaordCurrent();
        vTaskDelay(100);
    }
    
    current = current / 5;

    if (current > AIOBOARD_CURRENT_MAX)
    {
        ERROR("AIOBaordCurrent = %d\r\n",current);
        return 1;
    }
    return 0;
}

static void sendErrorHappenForceEnd(void)
{
    AioDspProtocolPkt pkt;
    int i = 0;
    
    initComputerPkt(&pkt);
    pkt.DataAndCRC[i++] = (u8)COMP_ID_ERROR_INFO;
    pkt.DataAndCRC[i++] = (u8)ERR_INFO_ID_TEST_END;
    pkt.Length = i;
    pkt.DataAndCRC[pkt.Length] = crc8ComputerPkt(&pkt);
    sendComputerPkt(&pkt);
}

static void MainProcessTask(void *pvParameters)
{    
    int ret = 0;
    bool running = true;
    char s8Val = 0;
    int state = STATE_AIOBOARD_DETECT_READY;
    /* Just to stop compiler warnings. */
    ( void ) pvParameters;
    
    INFO("MainProcessTask[%d] running...\r\n",u32CreateAppCount);

    while(running)
    {
        switch(state)
        {
        case STATE_AIOBOARD_DETECT_READY:{
            ret = testAIOBaordReady();
            if (ret){
                state = STATE_AIOBOARD_POWERUP;
            }else{
                ERROR("E03-01:No AIO-Board deteced!!\r\n");
                vTaskDelay(1000 / portTICK_PERIOD_MS); //delay 1s
            }
        }break;
        
        case STATE_AIOBOARD_POWERUP:{
            s8Val = SW_ON;
            AioBoardCtrl(CTRL_CMD_AIOBOARD_SET_POWER,&s8Val);
            vTaskDelay(2000 / portTICK_PERIOD_MS); //delay 2s for power stable
            state = STATE_AIOBOARD_DETECT_CURRENT;
        }break;
        
        case STATE_AIOBOARD_DETECT_CURRENT:{
            if (0 == testAIOBaordCurrent())
            {
                state = STATE_DETECT_D3V3E_POWER;
            }
            else
            {
                ERROR("E04-01:AIO-Board Over Current!!\r\n");
                running = false;
            }
        }break;
        
        case STATE_DETECT_D3V3E_POWER:{
            ret = testAIOBaordD3V3EPower();
            if (0 == ret){
                state = STATE_DETECT_OTHER_POWER;
            }else{
                ERROR("E04-01:D3V3E power!!\r\n");
                running = false;
            }
        }break;
        
        case STATE_DETECT_OTHER_POWER:{
            ret = testAIOBaordOtherPower();
            if (0 == ret){
                state = STATE_PROCESS_SUCCESS;
            }else{
                ERROR("E04-02:Other power!!\r\n");
                running = false;
            }
        }break;
        
        case STATE_PROCESS_SUCCESS:{
            running = false;
        }break;
        
        default:
            break;
        }
    }

    if (STATE_PROCESS_SUCCESS != state)
    {
        sendErrorHappenForceEnd();
        INFO("Error happen!!!\r\n");
    }

    // power-off aio board
    s8Val = SW_OFF;
    AioBoardCtrl(CTRL_CMD_AIOBOARD_SET_POWER,&s8Val);
    
    INFO("MainProcessTask[%d] delete...\r\n",u32CreateAppCount);
    xMainProcessTaskHandle = NULL;
    vTaskDelete(NULL);
}

