/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : driver_temp.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/10/30
  Last Modified :
  Description   : temputure module driver
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
TempDebug_Typedef           gTempDebug;
TempDebug_Typedef           *gpTempDebug = &gTempDebug;

/*----------------------------------------------*
 * module-wide global variables                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/
#define TEMP_ONOFF_PIN          GPIO_Pin_15
#define TEMP_ONOFF_PORT         GPIOB
#define TEMP_ONOFF_RCC          RCC_APB2Periph_GPIOB
#define TEMP_ONOFF_HIGH         GPIO_SetBits(TEMP_ONOFF_PORT, TEMP_ONOFF_PIN)
#define TEMP_ONOFF_LOW          GPIO_ResetBits(TEMP_ONOFF_PORT, TEMP_ONOFF_PIN)

#define TEMP_SEL_CTL1_PIN       GPIO_Pin_8
#define TEMP_SEL_CTL1_PORT      GPIOD
#define TEMP_SEL_CTL1_RCC       RCC_APB2Periph_GPIOD
#define TEMP_SEL_CTL1_HIGH      GPIO_SetBits(TEMP_SEL_CTL1_PORT, TEMP_SEL_CTL1_PIN)
#define TEMP_SEL_CTL1_LOW       GPIO_ResetBits(TEMP_SEL_CTL1_PORT, TEMP_SEL_CTL1_PIN)

#define TEMP_SEL_CTL0_PIN       GPIO_Pin_9
#define TEMP_SEL_CTL0_PORT      GPIOD
#define TEMP_SEL_CTL0_RCC       RCC_APB2Periph_GPIOD
#define TEMP_SEL_CTL0_HIGH      GPIO_SetBits(TEMP_SEL_CTL0_PORT, TEMP_SEL_CTL0_PIN)
#define TEMP_SEL_CTL0_LOW       GPIO_ResetBits(TEMP_SEL_CTL0_PORT, TEMP_SEL_CTL0_PIN)


/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

static void TempDevGPIOInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd( TEMP_ONOFF_RCC \
                            | TEMP_SEL_CTL1_RCC \
                            | TEMP_SEL_CTL0_RCC,
                            ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_InitStructure.GPIO_Pin = TEMP_ONOFF_PIN;      
    GPIO_Init(TEMP_ONOFF_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = TEMP_SEL_CTL1_PIN;      
    GPIO_Init(TEMP_SEL_CTL1_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = TEMP_SEL_CTL0_PIN;      
    GPIO_Init(TEMP_SEL_CTL0_PORT, &GPIO_InitStructure);

    TEMP_ONOFF_LOW;
    TEMP_SEL_CTL1_LOW;
    TEMP_SEL_CTL0_LOW;
}

int TempDevInit(void)
{
    TempDevGPIOInit();
    
    memset(gpTempDebug, 0x00, sizeof(TempDebug_Typedef));
    gpTempDebug->xEventGroup = xEventGroupCreate();
    do{} while (NULL == gpTempDebug->xEventGroup);
    return 0;
}


int TempDevCtrl(const TEMP_CTRL_CMD cmd, const TEMP_CTRL_CMD_VAL val)
{
    switch(cmd)
    {
    case TEMP_CTRL_CMD_ON:{
        TEMP_ONOFF_HIGH;
    }break;
    case TEMP_CTRL_CMD_OFF:{
        TEMP_ONOFF_LOW;
    }break;
    case TEMP_CTRL_CMD_25C:{
        TEMP_SEL_CTL1_LOW;
        TEMP_SEL_CTL0_LOW;
    }break;
    case TEMP_CTRL_CMD_37C:{
        TEMP_SEL_CTL1_LOW;
        TEMP_SEL_CTL0_HIGH;
    }break;
    case TEMP_CTRL_CMD_42C:{
        TEMP_SEL_CTL1_HIGH;
        TEMP_SEL_CTL0_HIGH;
    }break;
    default:{

    }break;
    }
    return 0;
}

int AioTempDebugCtrl(const AIOTEMPDEBCTRL_CMD cmd, void *arg)
{
    char cid  = (char )TEMP_DEB_CID_UNVALID;
    EventBits_t uxBits = 0;
    EventBits_t waitBits = 0;
    
    switch(cmd)
    {
    case TEMP_DEB_CID_SELFCHECK:{
        cid = (char)cmd;
        waitBits = TEMP_DEB_PKT_BIT_SELFCHECK;
    }break;
    default:{

    }break;
    }
    
    if ((char )TEMP_DEB_CID_UNVALID == cid)
    {
        return -1;
    }

    xEventGroupClearBits(gpTempDebug->xEventGroup, waitBits);
    sendAioDspPktByID(AIO_RX_TEMP_Debug_ID, &cid, 1, 0);

    uxBits = xEventGroupWaitBits(
            gpTempDebug->xEventGroup,   // The event group being tested.
            waitBits,                   // The bits within the event group to wait for.
            pdTRUE,                     // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
            pdFALSE,                    // Don't wait for both bits, either bit will do.
            1000 / portTICK_PERIOD_MS );// Wait a maximum of for either bit to be set.
    if (uxBits & waitBits)
    {
        return 0;
    }
    return -1;
}

