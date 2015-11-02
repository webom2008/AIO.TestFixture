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
    default:{

    }break;
    }
    return 0;
}

