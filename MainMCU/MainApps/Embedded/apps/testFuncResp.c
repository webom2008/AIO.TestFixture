/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : testFuncResp.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/11/4
  Last Modified :
  Description   : test for RESP module
  Function List :
  History       :
  1.Date        : 2015/11/4
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
typedef enum
{
    RESP_II = 0,
    RESP_I  = 1,
} RESP_CHANNEL;
/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/

#define RESP_AMP_DELAY_MS                    2000

#define _INFO_
#define _ERROR_

#ifdef _INFO_
#define INFO(fmt, arg...) udprintf("[RespTest]Info: "fmt, ##arg)
#else
#define INFO(fmt, arg...) do{}while(0)
#endif
#ifdef _ERROR_
#define ERROR(fmt, arg...) udprintf("[RespTest]Error: "fmt, ##arg)
#else
#define ERROR(fmt, arg...) do{}while(0)
#endif
/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

static int setAndWaitRespChannel(const RESP_CHANNEL channel)
{
    u8 pBuf[1] = {0,};
    EventBits_t uxBits = 0;

    pBuf[0] = (u8)channel;
    xEventGroupClearBits(xDspPktAckEventGroup, DSP_PKT_ACK_BIT_RESP_CH);
    sendAioDspPktByID(AIO_RESP_CHANNEL_SEL_ID, (char *)pBuf, 1, 0);
    
    uxBits = xEventGroupWaitBits(
            xDspPktAckEventGroup,   // The event group being tested.
            DSP_PKT_ACK_BIT_RESP_CH,    // The bits within the event group to wait for.
            pdTRUE,                     // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
            pdFALSE,                    // Don't wait for both bits, either bit will do.
            1000 / portTICK_PERIOD_MS );// Wait a maximum of for either bit to be set.
    if (uxBits & DSP_PKT_ACK_BIT_RESP_CH)
    {
        return 0;
    }
    ERROR("setAndWaitRespChannel RESP_CH timeout!!\r\n");
    return -1;
}

static int getVppFromAIO(int *pVppVal, const TickType_t sample_ms)
{
    //S1:Start AIO Factory
    if (AioRESPDebugCtrl(RESP_DEB_CID_START_VPP, NULL) < 0)
    {
        ERROR("getVppFromAIO AioRESPDebugCtrl!!!\r\n");
        return -1;
    }
    
    //S2:delay for 2 cycle
    vTaskDelay(sample_ms/portTICK_PERIOD_MS);;
    
    //S3:Stop and get AIO result
    if (AioRESPDebugCtrl(RESP_DEB_CID_STOP_VPP, NULL) < 0)
    {
        ERROR("getVppFromAIO AioRESPDebugCtrl!!!\r\n");
        return -1;
    }

    *pVppVal = gpRespDebug->RespVppResult;
    return 0;
}

static int checkRespByChannel(  const RESP_CHANNEL channel,
                                int *pGetU1,
                                int *pGetU2,
                                int *pGetU3)
{
    TickType_t sample_ms = 0;
    
    //switch channel both AIO and AIO-Test
    if (RESP_II == channel)
    {
        RespDevCtrl(RESP_CTRL_CMD_LEAD_II, RESP_CMD_VAL_UNVALID);
    }
    else
    {
        RespDevCtrl(RESP_CTRL_CMD_LEAD_I, RESP_CMD_VAL_UNVALID);
    }
    
    if (setAndWaitRespChannel(channel) < 0)
    {
        ERROR("checkRespByChannel set AIO failed!!!\r\n");
        return -1;
    }
    
    //set Wave ch2 = 100mVpp 0.333Hz SIN, 20bpm
    if (WavefromCtrl(WF_CTRL_0P333Hz_100mVpp_SIN, NULL) < 0)
    {
        ERROR("checkRespByChannel WavefromCtrl 0P333Hz!!!\r\n");
        return -1;
    }
    vTaskDelay(RESP_AMP_DELAY_MS/portTICK_PERIOD_MS);

    //get Vpp from AIO
    sample_ms = 3000*2; //delay for 2 cycle
    if (getVppFromAIO(pGetU1, sample_ms) < 0)
    {
        ERROR("checkRespByChannel getVppFromAIO 0P333Hz!!!\r\n");
        return -1;
    }

    //set Wave ch2 = 100mVpp 0.117Hz SIN, 7bpm
    if (WavefromCtrl(WF_CTRL_0P117Hz_100mVpp_SIN, NULL) < 0)
    {
        ERROR("checkRespByChannel WavefromCtrl 0P117Hz!!!\r\n");
        return -1;
    }
    vTaskDelay(RESP_AMP_DELAY_MS/portTICK_PERIOD_MS);

    //get Vpp from AIO
    sample_ms = 8547*2; //delay for 2 cycle
    if (getVppFromAIO(pGetU2, sample_ms) < 0)
    {
        ERROR("checkRespByChannel getVppFromAIO 0P117Hz!!!\r\n");
        return -1;
    }

    //set Wave ch2 = 100mVpp 2.5Hz SIN, 150bpm
    if (WavefromCtrl(WF_CTRL_2P5Hz_100mVpp_SIN, NULL) < 0)
    {
        ERROR("checkRespByChannel WavefromCtrl 2P5Hz!!!\r\n");
        return -1;
    }
    vTaskDelay(RESP_AMP_DELAY_MS/portTICK_PERIOD_MS);

    //get Vpp from AIO
    sample_ms = 400*2; //delay for 2 cycle
    if (getVppFromAIO(pGetU3, sample_ms) < 0)
    {
        ERROR("checkRespByChannel getVppFromAIO 2P5Hz!!!\r\n");
        return -1;
    }

    return 0;
}

int testRespAmplitudeBand(void)
{
    int u1_I, u1_II;
    int u2_I, u2_II;
    int u3_I, u3_II;
    int err_cnt = 0;
    
    EcgDevCtrl(CMD_ECG_ALL_SEL_ECGOUT, CMD_VAL_UNVALID);
    RespDevCtrl(RESP_CTRL_CMD_WAVE_IN_RESP, RESP_CMD_VAL_UNVALID);
    
    //switch base-R 2Kohm
    RespDevCtrl(RESP_CTRL_CMD_R_2KOHM, RESP_CMD_VAL_UNVALID);

    //set waveform ch1 -> 1Vpp 1Hz QRS
    if (WavefromCtrl(WF_CTRL_1Hz_1Vpp_QRS, NULL) < 0)
    {
        ERROR("testRespAmplitudeBand WavefromCtrl!!!\r\n");
        RespDevCtrl(RESP_CTRL_CMD_WAVE_IN_ECG_IBP, RESP_CMD_VAL_UNVALID);
        return -1;
    }

    //test channel I
    if (checkRespByChannel(RESP_I, &u1_I, &u2_I, &u3_I) < 0)
    {
        ERROR("testRespAmplitudeBand test channel I!!!\r\n");
        err_cnt++;
    }
    
    udprintf("RESP_I Result:\r\n");
    udprintf("I:U1 = %d uV\r\n", u1_I);
    udprintf("I:U2 = %d uV\r\n", u2_I);
    udprintf("I:U3 = %d uV\r\n", u3_I);
    
    //test channel II
    if (checkRespByChannel(RESP_II, &u1_II, &u2_II, &u3_II) < 0)
    {
        ERROR("testRespAmplitudeBand test channel II!!!\r\n");
        err_cnt++;
    }
    
    udprintf("RESP_II Result:\r\n");
    udprintf("II:U1 = %d uV\r\n", u1_II);
    udprintf("II:U2 = %d uV\r\n", u2_II);
    udprintf("II:U3 = %d uV\r\n", u3_II);
    
    RespDevCtrl(RESP_CTRL_CMD_WAVE_IN_ECG_IBP, RESP_CMD_VAL_UNVALID);
    return 0;
}

