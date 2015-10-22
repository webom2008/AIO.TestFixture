/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : driver_aiostm.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/10/22
  Last Modified :
  Description   : driver_aiostm.c header file
  Function List :
  History       :
  1.Date        : 2015/10/22
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/

#ifndef __DRIVER_AIOSTM_H__
#define __DRIVER_AIOSTM_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

typedef enum
{
    AIO_STM_CTRL_CMD_SET_BOOT0,
    AIO_STM_CTRL_CMD_GET_BOOT0,
    AIO_STM_CTRL_CMD_SET_UART,
    AIO_STM_CTRL_CMD_GET_UART,

    AIO_STM_CTRL_CMD_UNVALID
} AioStmCtrlCMD;

int AioStmOpen(void);
int AioStmCtrl(AioStmCtrlCMD cmd, void *pParameter);
int AioStmClose(void);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DRIVER_AIOSTM_H__ */
