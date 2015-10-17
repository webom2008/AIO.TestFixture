/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : AppCoopMCU.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/16
  Last Modified :
  Description   : AppCoopMCU.c header file
  Function List :
  History       :
  1.Date        : 2015/9/16
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#ifndef __APPCOOPMCU_H__
#define __APPCOOPMCU_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


#define COOPMCU_PKT_ACK_BIT_TDM    ((EventBits_t)(1<<0))



int AppCoopMcuInit(void);

int AppCoopMcuStart(void);

int getAIOBaordCurrent(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __APPCOOPMCU_H__ */
