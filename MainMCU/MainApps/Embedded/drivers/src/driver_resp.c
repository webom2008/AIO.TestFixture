/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : driver_resp.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/10/30
  Last Modified :
  Description   : resp module driver
  Function List :
  History       :
  1.Date        : 2015/10/30
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
RespDebug_Typedef    gRespDebug;
RespDebug_Typedef    *gpRespDebug = &gRespDebug;

/*----------------------------------------------*
 * module-wide global variables                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/
#define WAVE_INPUT_SEL_PIN          GPIO_Pin_0
#define WAVE_INPUT_SEL_PORT         GPIOD
#define WAVE_INPUT_SEL_RCC          RCC_APB2Periph_GPIOD
#define WAVE_INPUT_SEL_HIGH         GPIO_SetBits(WAVE_INPUT_SEL_PORT, WAVE_INPUT_SEL_PIN)
#define WAVE_INPUT_SEL_LOW          GPIO_ResetBits(WAVE_INPUT_SEL_PORT, WAVE_INPUT_SEL_PIN)

#define RESP_LEAD_SEL_PIN           GPIO_Pin_1
#define RESP_LEAD_SEL_PORT          GPIOD
#define RESP_LEAD_SEL_RCC           RCC_APB2Periph_GPIOD
#define RESP_LEAD_SEL_HIGH          GPIO_SetBits(RESP_LEAD_SEL_PORT, RESP_LEAD_SEL_PIN)
#define RESP_LEAD_SEL_LOW           GPIO_ResetBits(RESP_LEAD_SEL_PORT, RESP_LEAD_SEL_PIN)

#define RESP_R_SEL_PIN              GPIO_Pin_5
#define RESP_R_SEL_PORT             GPIOD
#define RESP_R_SEL_RCC              RCC_APB2Periph_GPIOD
#define RESP_R_SEL_HIGH             GPIO_SetBits(RESP_R_SEL_PORT, RESP_R_SEL_PIN)
#define RESP_R_SEL_LOW              GPIO_ResetBits(RESP_R_SEL_PORT, RESP_R_SEL_PIN)

#define _INFO_
#define _ERROR_

#ifdef _INFO_
#define INFO(fmt, arg...) udprintf("[RespDev]Info: "fmt, ##arg)
#else
#define INFO(fmt, arg...) do{}while(0)
#endif
#ifdef _ERROR_
#define ERROR(fmt, arg...) udprintf("[RespDev]Error: "fmt, ##arg)
#else
#define ERROR(fmt, arg...) do{}while(0)
#endif
/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/
static void RespDevGPIOInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd( WAVE_INPUT_SEL_RCC \
                            | RESP_LEAD_SEL_RCC \
                            | RESP_R_SEL_RCC,
                            ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_InitStructure.GPIO_Pin = WAVE_INPUT_SEL_PIN;      
    GPIO_Init(WAVE_INPUT_SEL_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = RESP_LEAD_SEL_PIN;      
    GPIO_Init(RESP_LEAD_SEL_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = RESP_R_SEL_PIN;      
    GPIO_Init(RESP_R_SEL_PORT, &GPIO_InitStructure);

    WAVE_INPUT_SEL_LOW;
    RESP_LEAD_SEL_LOW;
    RESP_R_SEL_LOW;
}

int RespDevInit(void)
{
    memset(gpRespDebug, 0x00, sizeof(RespDebug_Typedef));
    gpRespDebug->xEventGroup = xEventGroupCreate();
    do{} while (NULL == gpRespDebug->xEventGroup);
    
    RespDevGPIOInit();
    return 0;
}

int RespDevCtrl(const RESP_CTRL_CMD cmd, const RESP_CTRL_CMD_VAL val)
{
    int ret = -1;
    switch(cmd)
    {
    case RESP_CTRL_CMD_WAVE_IN_RESP:{
        WAVE_INPUT_SEL_HIGH;
        ret = 0;
    }break;
    case RESP_CTRL_CMD_WAVE_IN_ECG_IBP:{
        WAVE_INPUT_SEL_LOW;
        ret = 0;
    }break;
    case RESP_CTRL_CMD_LEAD_I:{
        RESP_LEAD_SEL_HIGH;
        ret = 0;
    }break;
    case RESP_CTRL_CMD_LEAD_II:{
        RESP_LEAD_SEL_LOW;
        ret = 0;
    }break;
    case  RESP_CTRL_CMD_R_1KOHM:{
        RESP_R_SEL_LOW;
        ret = 0;
    }break;
    case RESP_CTRL_CMD_R_2KOHM:{
        RESP_R_SEL_HIGH;
        ret = 0;
    }break;
    default:{

    }break;
    }
    return ret;
}

int AioRESPDebugCtrl(const AIORESPDEBCTRL_CMD cmd, void *arg)
{
    char cid  = (char )RESP_DEB_CID_UNVALID;
    EventBits_t uxBits = 0;
    EventBits_t waitBits = 0;
    
    switch(cmd)
    {
    case RESP_DEB_CID_START_VPP:{
        cid = (char)cmd;
        waitBits = RESP_DEB_PKT_BIT_START_VPP;
    }break;
    case RESP_DEB_CID_STOP_VPP:{
        cid = (char)cmd;
        waitBits = RESP_DEB_PKT_BIT_STOP_VPP;
    }break;
    default:{

    }break;
    }
    
    if ((char )RESP_DEB_CID_UNVALID == cid)
    {
        return -1;
    }

    xEventGroupClearBits(gpRespDebug->xEventGroup, waitBits);
    sendAioDspPktByID(AIO_RX_RESP_Debug_ID, &cid, 1, 0);

    uxBits = xEventGroupWaitBits(
            gpRespDebug->xEventGroup,   // The event group being tested.
            waitBits,                   // The bits within the event group to wait for.
            pdTRUE,                     // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
            pdFALSE,                    // Don't wait for both bits, either bit will do.
            1000 / portTICK_PERIOD_MS );// Wait a maximum of for either bit to be set.
    if (uxBits & waitBits)
    {
        return 0;
    }
    ERROR("AioRESPDebugCtrl timeout!!!\r\n");
    return -1;
}

