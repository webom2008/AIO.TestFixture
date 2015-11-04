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


typedef enum
{
    ECG_DEB_CID_UPLOAD_TYPE     = 0x01,
    ECG_DEB_CID_ALGO_RUN        = 0x02,
    ECG_DEB_CID_TACHY           = 0x03,
    ECG_DEB_CID_BRADY           = 0x04,
    ECG_DEB_CID_POLARVOL        = 0x05,
    ECG_DEB_CID_ST_TEMP         = 0x06,
    ECG_DEB_CID_ALGO_DEB        = 0x07,
    ECG_DEB_CID_GET_SELFCHECK   = 0x08,
    ECG_DEB_CID_START_VPP       = 0x09,
    ECG_DEB_CID_STOP_VPP        = 0x0A,
    ECG_DEB_CID_START_PACE_CNT  = 0x0B,
    ECG_DEB_CID_STOP_PACE_CNT   = 0x0C,

    ECG_DEB_CID_UNVALID
}AIOECGDEBCTRL_CMD;

typedef struct
{
    u16 VppECG1;
    u16 VppECG2;
    u16 VppECG3;
} EcgDebVppResult;

typedef struct
{
    EventGroupHandle_t xEventGroup;
    u8 u8SelfcheckResult;
    u8 u8PaceCount;
    EcgDebVppResult ecgVppResult;
} EcgDebug_Typedef;


#define ECG_DEB_PKT_BIT_SELFCHECK       ((EventBits_t)(1<<0))
#define ECG_DEB_PKT_BIT_START_VPP       ((EventBits_t)(1<<1))
#define ECG_DEB_PKT_BIT_STOP_VPP        ((EventBits_t)(1<<2))
#define ECG_DEB_PKT_BIT_START_PACE_CNT  ((EventBits_t)(1<<3))
#define ECG_DEB_PKT_BIT_STOP_PACE_CNT   ((EventBits_t)(1<<4))



extern EcgDebug_Typedef    *gpEcgDebug;

extern int initAioEcgDebugResource(void);
extern int exeAioEcgDebugPacket(AioDspProtocolPkt *pPacket);
extern int AioEcgDebugCtrl(const AIOECGDEBCTRL_CMD cmd, void *arg);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __ECGDEBUG_H__ */
