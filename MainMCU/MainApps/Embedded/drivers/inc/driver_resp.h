/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : driver_resp.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/10/30
  Last Modified :
  Description   : driver_resp.c header file
  Function List :
  History       :
  1.Date        : 2015/10/30
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/

#ifndef __DRIVER_RESP_H__
#define __DRIVER_RESP_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

typedef enum 
{
    RESP_CTRL_CMD_WAVE_IN_RESP,
    RESP_CTRL_CMD_WAVE_IN_ECG_IBP,
    RESP_CTRL_CMD_LEAD_I,
    RESP_CTRL_CMD_LEAD_II,
    RESP_CTRL_CMD_R_1KOHM,
    RESP_CTRL_CMD_R_2KOHM,
    
    RESP_CTRL_CMD_UNVALID,
} RESP_CTRL_CMD;

typedef enum 
{
    RESP_CMD_VAL_UNVALID,
} RESP_CTRL_CMD_VAL;


typedef enum
{
    RESP_DEB_CID_SAMPLE_SW      = 0x01,
    RESP_DEB_CID_START_VPP      = 0x02,
    RESP_DEB_CID_STOP_VPP       = 0x03,

    RESP_DEB_CID_UNVALID
}AIORESPDEBCTRL_CMD;


typedef struct
{
    EventGroupHandle_t xEventGroup;
    int RespVppResult;
} RespDebug_Typedef;


#define RESP_DEB_PKT_BIT_START_VPP          ((EventBits_t)(1<<0))
#define RESP_DEB_PKT_BIT_STOP_VPP           ((EventBits_t)(1<<1))



extern RespDebug_Typedef    *gpRespDebug;

extern int RespDevInit(void);
extern int RespDevCtrl(const RESP_CTRL_CMD cmd, const RESP_CTRL_CMD_VAL val);
extern int AioRESPDebugCtrl(const AIORESPDEBCTRL_CMD cmd, void *arg);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DRIVER_RESP_H__ */
