/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : AppGeneralIO.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/21
  Last Modified :
  Description   : AppGeneralIO.c header file
  Function List :
  History       :
  1.Date        : 2015/9/21
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/

#ifndef __APPGENERALIO_H__
#define __APPGENERALIO_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

typedef enum
{
    LED_STATUS_IDLE,
    LED_STATUS_RUNNING,
    LED_STATUS_ERROR,
    LED_STATUS_SUCCESS,
    
} LedStatus_Typedef;

void setLedStatus(LedStatus_Typedef type);

int AppGeneralIOInit(void);

int AppGeneralIOStart(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __APPGENERALIO_H__ */
