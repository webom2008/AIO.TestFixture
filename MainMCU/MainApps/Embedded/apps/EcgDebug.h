/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : EcgDebug.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/11/2
  Last Modified :
  Description   : EcgDebug.c header file
  Function List :
  History       :
  1.Date        : 2015/11/2
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/


#ifndef __ECGDEBUG_H__
#define __ECGDEBUG_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


enum
{
    ECG_DEB_CID_UPLOAD_TYPE     = 0x01,
    ECG_DEB_CID_ALGO_RUN        = 0x02,
    ECG_DEB_CID_TACHY           = 0x03,
    ECG_DEB_CID_BRADY           = 0x04,
    ECG_DEB_CID_POLARVOL        = 0x05,
    ECG_DEB_CID_ST_TEMP         = 0x06,
    ECG_DEB_CID_ALGO_DEB        = 0x07,
    ECG_DEB_CID_GET_SELFCHECK   = 0x08,
};

typedef struct
{
    EventGroupHandle_t xEventGroup;
    u8 u8SelfcheckResult;
} EcgDebug_Typedef;

#define ECG_DEB_PKT_BIT_SELFCHECK       ((EventBits_t)(1<<0))


extern EcgDebug_Typedef    *gpEcgDebug;

extern int initAioEcgDebugResource(void);
extern int exeAioEcgDebugPacket(AioDspProtocolPkt *pPacket);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __ECGDEBUG_H__ */
