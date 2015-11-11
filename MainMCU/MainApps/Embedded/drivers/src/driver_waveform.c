/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : driver_waveform.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/11/2
  Last Modified :
  Description   : wave form generater
  Function List :
  History       :
  1.Date        : 2015/11/2
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#include "includes.h"

/*----------------------------------------------*
 * external variables                           *
 *----------------------------------------------*/
extern EventGroupHandle_t xCompPktAckEventGroup;

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

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

static int setAndWaitByPC(const u8 type)
{
    AioDspProtocolPkt pkt;
    int i = 0;
    EventBits_t uxBits;
    
    initComputerPkt(&pkt);
    pkt.DataAndCRC[i++] = (u8)COMP_ID_WAVEFORM_COMM;
    pkt.DataAndCRC[i++] = type;
    pkt.Length = i;
    pkt.DataAndCRC[pkt.Length] = crc8ComputerPkt(&pkt);

    for (i = 0; i < 3; i++)
    {
        xEventGroupClearBits(xCompPktAckEventGroup, COMP_PKT_BIT_WAVEFORM_COMM);
        sendComputerPkt(&pkt);
        uxBits = xEventGroupWaitBits(
                xCompPktAckEventGroup,      // The event group being tested.
                COMP_PKT_BIT_WAVEFORM_COMM, // The bits within the event group to wait for.
                pdTRUE,                     // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
                pdFALSE,                    // Don't wait for both bits, either bit will do.
                10000/portTICK_PERIOD_MS);  // Wait a maximum of for either bit to be set.
        if (uxBits & COMP_PKT_BIT_WAVEFORM_COMM)
        {
            if (type == gpComputerReult->u8WaveformCID)
            {
                return 0;
            }
        }
    }
    return -2;
}

int WavefromCtrl(const WAVEFORM_CTRL_CMD cmd, void *arg)
{
    int ret = -1;
    switch(cmd)
    {
    case WF_CTRL_10Hz_1Vpp_SIN:{
        ret = setAndWaitByPC(WF_COMM_CID_SET_SIN_10Hz1Vpp);
    }break;
    case WF_CTRL_0P5Hz_1Vpp_SIN:{
        ret = setAndWaitByPC(WF_COMM_CID_SET_SIN_0P5Hz1Vpp);
    }break;
    case WF_CTRL_150Hz_1Vpp_SIN:{
        ret = setAndWaitByPC(WF_COMM_CID_SET_SIN_150Hz1Vpp);
    }break;
    case WF_CTRL_2Hz_16Vpp_SIN:{
        ret = setAndWaitByPC(WF_COMM_CID_SET_SIN_2Hz16Vpp);
    }break;
    case WF_CTRL_2Hz_16mVpp_SIN:{
        ret = setAndWaitByPC(WF_COMM_CID_SET_SIN_2Hz16mVpp);
    }break;
    case WF_CTRL_PACE_A:{
        ret = setAndWaitByPC(WF_COMM_CID_SET_PACE_A);
    }break;
    case WF_CTRL_PACE_B:{
        ret = setAndWaitByPC(WF_COMM_CID_SET_PACE_B);
    }break;
    case WF_CTRL_PACE_C:{
        ret = setAndWaitByPC(WF_COMM_CID_SET_PACE_C);
    }break;
    case WF_CTRL_PACE_D:{
        ret = setAndWaitByPC(WF_COMM_CID_SET_PACE_D);
    }break;
    case WF_CTRL_PACE_E:{
        ret = setAndWaitByPC(WF_COMM_CID_SET_PACE_E);
    }break;
    case WF_CTRL_PACE_F:{
        ret = setAndWaitByPC(WF_COMM_CID_SET_PACE_F);
    }break;
    case WF_CTRL_PACE_G:{
        ret = setAndWaitByPC(WF_COMM_CID_SET_PACE_G);
    }break;
    case WF_CTRL_PACE_H:{
        ret = setAndWaitByPC(WF_COMM_CID_SET_PACE_H);
    }break;
    case WF_CTRL_QRS_A:{
        ret = setAndWaitByPC(WF_COMM_CID_SET_QRS_A);
    }break;
    case WF_CTRL_1Hz_1Vpp_QRS:{
        ret = setAndWaitByPC(WF_COMM_CID_SET_QRS_1Hz1Vpp);
    }break;
    case WF_CTRL_0P333Hz_100mVpp_SIN:{
        ret = setAndWaitByPC(WF_COMM_CID_SET_SIN_0P333Hz100mVpp);
    }break;
    case WF_CTRL_0P117Hz_100mVpp_SIN:{
        ret = setAndWaitByPC(WF_COMM_CID_SET_SIN_0P117Hz100mVpp);
    }break;
    case WF_CTRL_2P5Hz_100mVpp_SIN:{
        ret = setAndWaitByPC(WF_COMM_CID_SET_SIN_2P5Hz100mVpp);
    }break;
    default:{}break;
    }
    return ret;
}

int WavefromDelay(const WAVEFORM_DELAY type, const char times)
{
    switch(type)
    {
    case WAVEFORM_DELAY_10Hz_SIN:{
        vTaskDelay((100*times)/portTICK_PERIOD_MS);
    }break;
    case WAVEFORM_DELAY_0P5Hz_SIN:{
        vTaskDelay((2000*times)/portTICK_PERIOD_MS);
    }break;
    case WAVEFORM_DELAY_150Hz_SIN:{
        vTaskDelay((7*times)/portTICK_PERIOD_MS);
    }break;
    default:{

    }break;
    }
    return 0;
}
