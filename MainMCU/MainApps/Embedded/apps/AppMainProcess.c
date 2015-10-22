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
extern EventGroupHandle_t xCompPktAckEventGroup;
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
typedef enum
{
    STATE_AIOBOARD_START,
    STATE_AIOBOARD_DETECT_READY,
    STATE_AIOBOARD_POWERUP,
    STATE_AIOBOARD_DETECT_CURRENT,
    STATE_DETECT_D3V3E_POWER,
    STATE_DETECT_OTHER_POWER,
    STATE_DOWNLOAD_AIOSTM_BOOT,
    STATE_DOWNLOAD_AIODSP_APP,
    STATE_DOWNLOAD_AIOSTM_APP,

    
    STATE_PROCESS_SUCCESS,
    STATE_PROCESS_UNVALID,
} MainProcessState_Typedef;

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
    EventBits_t uxBits;
    
    DmaUartProtocolPacketInit(&txPacket);
    txPacket.ID = (u8)PKT_ID_TDM_RESULT;
    txPacket.ACK = DMA_UART_PACKET_ACK;

    for (i = 0; i < 5; i++)
    {
        sendCoopMcuPkt(&txPacket, 1000);
        
        uxBits = xEventGroupWaitBits(
                xCoopMCUPktAckEventGroup,   // The event group being tested.
                COOPMCU_PKT_ACK_BIT_TDM,    // The bits within the event group to wait for.
                pdTRUE,                     // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
                pdFALSE,                    // Don't wait for both bits, either bit will do.
                1000 / portTICK_PERIOD_MS );// Wait a maximum of for either bit to be set.
        if (uxBits & COOPMCU_PKT_ACK_BIT_TDM)
        {
            current += getAIOBaordCurrent();
        }
        else //timeout
        {
            ERROR("testAIOBaordCurrent timeout!!!\r\n");
            return 1;
        }
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

static void sendErrorForDownloadCount(void)
{
    AioDspProtocolPkt pkt;
    int i = 0;
    
    initComputerPkt(&pkt);
    pkt.DataAndCRC[i++] = (u8)COMP_ID_ERROR_INFO;
    pkt.DataAndCRC[i++] = (u8)ERR_INFO_ID_DOWNLOAD_CNT;
    pkt.Length = i;
    pkt.DataAndCRC[pkt.Length] = crc8ComputerPkt(&pkt);
    sendComputerPkt(&pkt);
}

static void sendMainProcessState(MainProcessState_Typedef state)
{
    AioDspProtocolPkt pkt;
    int i = 0;
    
    initComputerPkt(&pkt);
    pkt.DataAndCRC[i++] = (u8)COMP_ID_PROCESS_STATE;
    pkt.DataAndCRC[i++] = (u8)state;
    pkt.Length = i;
    pkt.DataAndCRC[pkt.Length] = crc8ComputerPkt(&pkt);
    sendComputerPkt(&pkt);
}

static int sendAndWaitAIODspApp(void)
{
    AioDspProtocolPkt pkt;
    int i = 0;
    EventBits_t uxBits;
//    char s8Val = 0;

    initComputerPkt(&pkt);
    pkt.DataAndCRC[i++] = (u8)COMP_ID_AIODSP_APP;
    pkt.Length = i;
    pkt.DataAndCRC[pkt.Length] = crc8ComputerPkt(&pkt);

    for (i = 0; i < 3; i++)
    {
//        vTaskDelay(DELAY_MAX_WAIT);
//        //Reset AIO power
//        s8Val = SW_OFF;
//        AioBoardCtrl(CTRL_CMD_AIOBOARD_SET_POWER,&s8Val);
//        vTaskDelay(2000 / portTICK_PERIOD_MS);
//        s8Val = SW_ON;
//        AioBoardCtrl(CTRL_CMD_AIOBOARD_SET_POWER,&s8Val);
//        vTaskDelay(1000 / portTICK_PERIOD_MS);
//        
        sendComputerPkt(&pkt);
        
        uxBits = xEventGroupWaitBits(
                xCompPktAckEventGroup,      // The event group being tested.
                COMP_PKT_BIT_AIODSP_APP,     // The bits within the event group to wait for.
                pdTRUE,                     // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
                pdFALSE,                    // Don't wait for both bits, either bit will do.
                DELAY_MAX_WAIT );           // Wait a maximum of for either bit to be set.
        if (uxBits & COMP_PKT_BIT_AIODSP_APP)
        {
            if (0 == gpComputerReult->u8AioDspAppResult)//success
            {
                return 0;
            }
            else //error happen
            {
                continue;
            }
        }
    }
    return -1;
}

static int sendAndWaitAIOStmApp(void)
{
    AioDspProtocolPkt pkt;
    int i = 0;
    EventBits_t uxBits;
    
    initComputerPkt(&pkt);
    pkt.DataAndCRC[i++] = (u8)COMP_ID_AIOSTM_APP;
    pkt.Length = i;
    pkt.DataAndCRC[pkt.Length] = crc8ComputerPkt(&pkt);

    for (i = 0; i < 3; i++)
    {
        sendComputerPkt(&pkt);
        
        uxBits = xEventGroupWaitBits(
                xCompPktAckEventGroup,      // The event group being tested.
                COMP_PKT_BIT_AIOSTM_APP,    // The bits within the event group to wait for.
                pdTRUE,                     // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
                pdFALSE,                    // Don't wait for both bits, either bit will do.
                DELAY_MAX_WAIT );           // Wait a maximum of for either bit to be set.
        if (uxBits & COMP_PKT_BIT_AIOSTM_APP)
        {
            if (0 == gpComputerReult->u8AioStmAppResult)//success
            {
                return 0;
            }
            else //error happen
            {
                continue;
            }
        }
    }
    return -1;
}

//#define SKIP_STATE_DETECT_OTHER_POWER
//#define SKIP_STATE_DOWNLOAD_AIOSTM_BOOT
//#define SKIP_STATE_DOWNLOAD_AIODSP_APP
//#define SKIP_STATE_DOWNLOAD_AIOSTM_APP

static void MainProcessTask(void *pvParameters)
{    
    int ret = 0;
    bool running = true;
    char s8Val = 0;
    int s32Val = 0;
    MainProcessState_Typedef state = STATE_AIOBOARD_DETECT_READY;
    /* Just to stop compiler warnings. */
    ( void ) pvParameters;
    
    INFO("MainProcessTask[%d] running...\r\n",u32CreateAppCount);

    if ((0 != SecurFlashCtrl(SECUR_CTRL_R_DOWNLOAD_CNT, &s32Val))||(0 == s32Val))
    {
        sendErrorForDownloadCount();
        setLedStatus(LED_STATUS_ERROR);
        xMainProcessTaskHandle = NULL;
        vTaskDelete(NULL);
        return;
    }
    setLedStatus(LED_STATUS_RUNNING);
    sendMainProcessState(STATE_AIOBOARD_START);
    while(running)
    {
        sendMainProcessState(state);
        switch(state)
        {
        case STATE_AIOBOARD_DETECT_READY:{
            ret = testAIOBaordReady();
            if (ret){
                state = STATE_AIOBOARD_POWERUP;
            }else{
                s8Val++;
                ERROR("E03-01:No AIO-Board deteced!!\r\n");
                vTaskDelay(1000 / portTICK_PERIOD_MS); //delay 1s
            }
            if (s8Val > 5)
            {
                running = false;
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
#ifndef SKIP_STATE_DETECT_OTHER_POWER
            ret = testAIOBaordOtherPower();
#endif
            if (0 == ret){
                state = STATE_DOWNLOAD_AIOSTM_BOOT;
            }else{
                ERROR("E04-02:Other power!!\r\n");
                running = false;
            }
        }break;
        
        case STATE_DOWNLOAD_AIOSTM_BOOT:{
#ifndef SKIP_STATE_DOWNLOAD_AIOSTM_BOOT
            ret = sendAndWaitAIOStmBoot();
#endif
            if (0 == ret){
                state = STATE_DOWNLOAD_AIODSP_APP;
            }else{
                ERROR("E04-02:AIOSTM_BOOT!!\r\n");
                running = false;
            }
        }break;
        
        case STATE_DOWNLOAD_AIODSP_APP:{
#ifndef SKIP_STATE_DOWNLOAD_AIODSP_APP
            ret = sendAndWaitAIODspApp();
#endif
            if (0 == ret){
                state = STATE_DOWNLOAD_AIOSTM_APP;
                SecurFlashCtrl(SECUR_CTRL_W_DEC_DOWNLOAD_CNT, NULL);
                vTaskDelay(20000 / portTICK_PERIOD_MS); //delay 20s for AIODSP boot
            }else{
                ERROR("E04-02:AIODSP_APP!!\r\n");
                running = false;
            }
        }break;
        
        case STATE_DOWNLOAD_AIOSTM_APP:{
#ifndef SKIP_STATE_DOWNLOAD_AIOSTM_APP
            ret = sendAndWaitAIOStmApp();
#endif
            if (0 == ret){
                state = STATE_PROCESS_SUCCESS;
                vTaskDelay(8000 / portTICK_PERIOD_MS); //delay 8s for AIOSTM boot
                
            }else{
                ERROR("E04-02:AIOSTM_APP!!\r\n");
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
        setLedStatus(LED_STATUS_ERROR);
        INFO("Error happen!!!\r\n");
    }
    else
    {
        setLedStatus(LED_STATUS_SUCCESS);
    }

    // power-off aio board
    s8Val = SW_OFF;
    AioBoardCtrl(CTRL_CMD_AIOBOARD_SET_POWER,&s8Val);
    
    INFO("MainProcessTask[%d] delete...\r\n",u32CreateAppCount);
    xMainProcessTaskHandle = NULL;
    vTaskDelete(NULL);
}

