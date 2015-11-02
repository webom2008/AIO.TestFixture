/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : driver_aioboard.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/16
  Last Modified :
  Description   : driver_aioboard.c header file
  Function List :
  History       :
  1.Date        : 2015/9/16
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/

#ifndef __DRIVER_AIOBOARD_H__
#define __DRIVER_AIOBOARD_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

typedef enum
{
    AIOBRD_CTRL_SET_PWR,

    AIOBRD_CTRL_UNVALID
}CTRL_CMD_AIOBOARD;

int AioBoardInit(void);
int AioBoardOpen(void);
int AioBoardRead(char *pReadData, const int nDataLen);
int AioBoardWrite(char *pWriteData, const int nDataLen);
int AioBoardCtrl(CTRL_CMD_AIOBOARD cmd, void *pData);
int AioBoardClose(void);

int createAioBoardTask(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DRIVER_AIOBOARD_H__ */
