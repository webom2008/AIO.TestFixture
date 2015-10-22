/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : AioStmUpdate.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/14
  Last Modified :
  Description   : AioStmUpdate.c header file
  Function List :
  History       :
  1.Date        : 2015/9/14
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/

#ifndef __AIOSTMUPDATE_H__
#define __AIOSTMUPDATE_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


typedef struct _AioStmDev_DEF_
{
    SemaphoreHandle_t pStartUpdateSemaphore;
} AioStmDev_TypeDef;

extern AioStmDev_TypeDef *gpAioStmDev;

int AioStmUpdateInit(void);
void AioStmUpdateTask(void *pvParameter);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __AIOSTMUPDATE_H__ */
