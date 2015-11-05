/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : driver_temp.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/10/30
  Last Modified :
  Description   : driver_temp.c header file
  Function List :
  History       :
  1.Date        : 2015/10/30
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/


#ifndef __DRIVER_TEMP_H__
#define __DRIVER_TEMP_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

typedef enum 
{
    TEMP_CTRL_CMD_ON,
    TEMP_CTRL_CMD_OFF,
    TEMP_CTRL_CMD_25C,
    TEMP_CTRL_CMD_37C,
    TEMP_CTRL_CMD_42C,
        
    TEMP_CTRL_CMD_UNVALID,
} TEMP_CTRL_CMD;

typedef enum 
{
    TEMP_CMD_VAL_UNVALID,
} TEMP_CTRL_CMD_VAL;


typedef struct
{
    EventGroupHandle_t xEventGroup;
    u8 u8SelfcheckResult;
} TempDebug_Typedef;

typedef enum
{
    TEMP_DEB_CID_SELFCHECK      = 0x01,
    TEMP_DEB_CID_UPLOAD_TYPE    = 0x02,

    TEMP_DEB_CID_UNVALID
}AIOTEMPDEBCTRL_CMD;


#define TEMP_DEB_PKT_BIT_SELFCHECK       ((EventBits_t)(1<<0))

extern TempDebug_Typedef        *gpTempDebug;

extern int TempDevInit(void);
extern int TempDevCtrl(const TEMP_CTRL_CMD cmd, const TEMP_CTRL_CMD_VAL val);
extern int AioTempDebugCtrl(const AIOTEMPDEBCTRL_CMD cmd, void *arg);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DRIVER_TEMP_H__ */
