/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : EcgDebug.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/11/2
  Last Modified :
  Description   : ecg debug interface
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

/*----------------------------------------------*
 * external routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * internal routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * project-wide global variables                *
 *----------------------------------------------*/

EcgDebug_Typedef    gEcgDebug;
EcgDebug_Typedef    *gpEcgDebug = &gEcgDebug;
/*----------------------------------------------*
 * module-wide global variables                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/
#define _INFO_
#define _ERROR_

#ifdef _INFO_
#define INFO(fmt, arg...) udprintf("[ECG]Info: "fmt, ##arg)
#else
#define INFO(fmt, arg...) do{}while(0)
#endif
#ifdef _ERROR_
#define ERROR(fmt, arg...) udprintf("[ECG]Error: "fmt, ##arg)
#else
#define ERROR(fmt, arg...) do{}while(0)
#endif
/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

int initAioEcgDebugResource(void)
{
    memset(gpEcgDebug, 0x00, sizeof(EcgDebug_Typedef));
    gpEcgDebug->xEventGroup = xEventGroupCreate();
    do{} while (NULL == gpEcgDebug->xEventGroup);
    return 0;
}

int exeAioEcgDebugPacket(AioDspProtocolPkt *pPacket)
{
    switch(pPacket->DataAndCRC[0])
    {
    case (u8)ECG_DEB_CID_GET_SELFCHECK:{
        gpEcgDebug->u8SelfcheckResult = pPacket->DataAndCRC[1];
        xEventGroupSetBits( gpEcgDebug->xEventGroup, 
                            ECG_DEB_PKT_BIT_SELFCHECK);
    }break;
    case (u8)ECG_DEB_CID_START_VPP:{
        xEventGroupSetBits( gpEcgDebug->xEventGroup, 
                            ECG_DEB_PKT_BIT_START_VPP);
    }break;
    case (u8)ECG_DEB_CID_STOP_VPP:{
        gpEcgDebug->ecgVppResult.VppECG1 = (u16)((pPacket->DataAndCRC[1] << 8) \
                                            |pPacket->DataAndCRC[2]);
        gpEcgDebug->ecgVppResult.VppECG2 = (u16)((pPacket->DataAndCRC[3] << 8) \
                                            |pPacket->DataAndCRC[4]);
        gpEcgDebug->ecgVppResult.VppECG3 = (u16)((pPacket->DataAndCRC[5] << 8) \
                                            |pPacket->DataAndCRC[6]);
        xEventGroupSetBits( gpEcgDebug->xEventGroup, 
                            ECG_DEB_PKT_BIT_STOP_VPP);
    }break;
    default:{

    }break;
    }
    return 0;
}

int AioEcgDebugCtrl(const AIOECGDEBCTRL_CMD cmd, void *arg)
{
    char cid  = (char )ECG_DEB_CID_UNVALID;
    EventBits_t uxBits = 0;
    EventBits_t waitBits = 0;
    switch(cmd)
    {
    case ECG_DEB_CID_START_VPP:{
        cid = (char)cmd;
        waitBits = ECG_DEB_PKT_BIT_START_VPP;
    }break;
    case ECG_DEB_CID_STOP_VPP:{
        cid = (char)cmd;
        waitBits = ECG_DEB_PKT_BIT_STOP_VPP;
    }break;
    default:{

    }break;
    }
    
    if ((char )ECG_DEB_CID_UNVALID == cid)
    {
        return -1;
    }

    xEventGroupClearBits(gpEcgDebug->xEventGroup, waitBits);
    sendAioDspPktByID(AIO_RX_ECG_Debug_ID, &cid, 1, 0);

    uxBits = xEventGroupWaitBits(
            gpEcgDebug->xEventGroup,   // The event group being tested.
            waitBits,                   // The bits within the event group to wait for.
            pdTRUE,                     // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
            pdFALSE,                    // Don't wait for both bits, either bit will do.
            1000 / portTICK_PERIOD_MS );// Wait a maximum of for either bit to be set.
    if (uxBits & waitBits)
    {
        return 0;
    }
    ERROR("AioEcgDebugCtrl timeout!!!\r\n");
    return -1;
}
