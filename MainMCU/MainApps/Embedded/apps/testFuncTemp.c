/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : testFuncTemp.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/11/5
  Last Modified :
  Description   : test for temperature
  Function List :
  History       :
  1.Date        : 2015/11/5
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#include "includes.h"

/*----------------------------------------------*
 * external variables                           *
 *----------------------------------------------*/
extern EventGroupHandle_t xCoopMCUPktAckEventGroup;
extern EventGroupHandle_t xCompPktAckEventGroup;
extern EventGroupHandle_t xDspPktAckEventGroup;
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
enum
{
    TEMP_TEST_25C   = 0,
    TEMP_TEST_37C,
    TEMP_TEST_42C,
    
};
/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/

#define TEMP_DELAY_MS                    5000

#define _INFO_
#define _ERROR_

#ifdef _INFO_
#define INFO(fmt, arg...) udprintf("[TempTest]Info: "fmt, ##arg)
#else
#define INFO(fmt, arg...) do{}while(0)
#endif
#ifdef _ERROR_
#define ERROR(fmt, arg...) udprintf("[TempTest]Error: "fmt, ##arg)
#else
#define ERROR(fmt, arg...) do{}while(0)
#endif
/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

int testTempSelfcheck(void)
{
    if (AioTempDebugCtrl(TEMP_DEB_CID_SELFCHECK, NULL) < 0)
    {
        ERROR("testTempSelfcheck timeout!!!\r\n");
        return -1;
    }

    if (0x00 == gpTempDebug->u8SelfcheckResult)
    {
        return 0;
    }
    
    ERROR("testTempSelfcheck error!!!\r\n");
    return -1;
}

static int waitTempAlarm(const u8 flag)
{
    EventBits_t uxBits = 0;
    u8 alarm = 0xFF;
    
    xEventGroupClearBits(xDspPktAckEventGroup, DSP_PKT_ACK_BIT_TEMP_DATA);
    
    uxBits = xEventGroupWaitBits(
            xDspPktAckEventGroup,   // The event group being tested.
            DSP_PKT_ACK_BIT_TEMP_DATA,    // The bits within the event group to wait for.
            pdTRUE,                     // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
            pdFALSE,                    // Don't wait for both bits, either bit will do.
            1000 / portTICK_PERIOD_MS );// Wait a maximum of for either bit to be set.
    if (uxBits & DSP_PKT_ACK_BIT_TEMP_DATA)
    {
        if (0x03 == (gpDspAckResult->u8TempProbeInfo&0x03))
        {
            alarm = 0x01;
        }
        else if (0x00 == (gpDspAckResult->u8TempProbeInfo&0x03))
        {
            alarm = 0x00;
        }

        if (((U8_FLAG_FALT == flag)&&(0x00 == alarm)) \
            || ((U8_FLAG_TRUE == flag)&&(0x01 == alarm)))
        {
            return 0;
        }
    }
    else
    {
        ERROR("waitTempAlarm timeout!!\r\n");
    }
    return -1;
}

int testTempProbeOff(void)
{
    int err_cnt = 0;
    
    TempDevCtrl(TEMP_CTRL_CMD_37C, TEMP_CMD_VAL_UNVALID);
    TempDevCtrl(TEMP_CTRL_CMD_ON, TEMP_CMD_VAL_UNVALID);

    vTaskDelay(TEMP_DELAY_MS/portTICK_PERIOD_MS);

    if (0 != waitTempAlarm(U8_FLAG_FALT))
    {
        err_cnt++;
        ERROR("testTempProbeOff error1 !!!\r\n");
    }
    
while(1)vTaskDelay(TEMP_DELAY_MS/portTICK_PERIOD_MS);

    TempDevCtrl(TEMP_CTRL_CMD_OFF, TEMP_CMD_VAL_UNVALID);
    vTaskDelay(TEMP_DELAY_MS/portTICK_PERIOD_MS);

    if (0 != waitTempAlarm(U8_FLAG_TRUE))
    {
        err_cnt++;
        ERROR("testTempProbeOff error2 !!!\r\n");
    }

    //restore default
    TempDevCtrl(TEMP_CTRL_CMD_ON, TEMP_CMD_VAL_UNVALID);
    
    if (err_cnt) return -1;
    else return 0;
}

static int checkTempPrecison(const int type)
{
    int ret = -1;
    EventBits_t uxBits = 0;
    
    xEventGroupClearBits(xDspPktAckEventGroup, DSP_PKT_ACK_BIT_TEMP_DATA);
    
    uxBits = xEventGroupWaitBits(
            xDspPktAckEventGroup,   // The event group being tested.
            DSP_PKT_ACK_BIT_TEMP_DATA,    // The bits within the event group to wait for.
            pdTRUE,                     // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
            pdFALSE,                    // Don't wait for both bits, either bit will do.
            1000 / portTICK_PERIOD_MS );// Wait a maximum of for either bit to be set.
    if (uxBits & DSP_PKT_ACK_BIT_TEMP_DATA)
    {
        switch(type)
        {
        case TEMP_TEST_25C:{
            if ((gpDspAckResult->u16Temp1Val > 240) \
                &&(gpDspAckResult->u16Temp2Val > 240)\
                &&(gpDspAckResult->u16Temp1Val < 260)\
                &&(gpDspAckResult->u16Temp2Val < 260))
            {
                ret = 0;
            }
        }break;
        case TEMP_TEST_37C:{
            if ((gpDspAckResult->u16Temp1Val > 360) \
                &&(gpDspAckResult->u16Temp2Val > 360)\
                &&(gpDspAckResult->u16Temp1Val < 380)\
                &&(gpDspAckResult->u16Temp2Val < 380))
            {
                ret = 0;
            }
        }break;
        case TEMP_TEST_42C:{
            if ((gpDspAckResult->u16Temp1Val > 410) \
                &&(gpDspAckResult->u16Temp2Val > 410)\
                &&(gpDspAckResult->u16Temp1Val < 430)\
                &&(gpDspAckResult->u16Temp2Val < 430))
            {
                ret = 0;
            }
        }break;
        default:
            break;
        }
        return ret;
    }
    ERROR("waitTempAlarm timeout!!\r\n");
    return ret;
}

int testTempPrecision(void)
{
    int err_cnt = 0;
    
    TempDevCtrl(TEMP_CTRL_CMD_ON, TEMP_CMD_VAL_UNVALID);

    TempDevCtrl(TEMP_CTRL_CMD_25C, TEMP_CMD_VAL_UNVALID);
    INFO("Ready to TEMP_TEST_25C!!!\r\n");
    vTaskDelay(TEMP_DELAY_MS/portTICK_PERIOD_MS);
    if (checkTempPrecison(TEMP_TEST_25C) < 0)
    {
        err_cnt++;
        ERROR("testTempPrecision TEMP_TEST_25C !!!\r\n");
    }

    
    TempDevCtrl(TEMP_CTRL_CMD_37C, TEMP_CMD_VAL_UNVALID);
    INFO("Ready to TEMP_CTRL_CMD_37C!!!\r\n");
    vTaskDelay(TEMP_DELAY_MS/portTICK_PERIOD_MS);
    if (checkTempPrecison(TEMP_TEST_37C) < 0)
    {
        err_cnt++;
        ERROR("testTempPrecision TEMP_TEST_37C !!!\r\n");
    }

    
    TempDevCtrl(TEMP_CTRL_CMD_42C, TEMP_CMD_VAL_UNVALID);
    INFO("Ready to TEMP_CTRL_CMD_42C!!!\r\n");
    vTaskDelay(TEMP_DELAY_MS/portTICK_PERIOD_MS);
    if (checkTempPrecison(TEMP_TEST_42C) < 0)
    {
        err_cnt++;
        ERROR("testTempPrecision TEMP_TEST_42C !!!\r\n");
    }
    
    if (err_cnt) return -1;
    else return 0;
}

