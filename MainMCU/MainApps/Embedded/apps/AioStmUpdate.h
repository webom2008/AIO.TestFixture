/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : AioStmUpdate.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/15
  Last Modified :
  Description   : AioStmUpdate.c header file
  Function List :
  History       :
  1.Date        : 2015/9/15
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



#define PKT_ACK_BIT_AIOSTM_START    (EventBits_t)(1<<0)
#define PKT_ACK_BIT_AIOSTM_ERROR    (EventBits_t)(1<<1)
#define PKT_ACK_BIT_AIOSTM_BOOT     (EventBits_t)(1<<2)
#define PKT_ACK_BIT_AIOSTM_END      (EventBits_t)(1<<3)

typedef struct _AioStmDev_DEF_
{
    EventGroupHandle_t xPktAckEventGroup;
} AioStmDev_TypeDef;

extern AioStmDev_TypeDef *gpAioStmDev;

int AioStmUpdateInit(void);
int createAioStmUpdateTask(void);
int deleteAioStmUpdateTask(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __AIOSTMUPDATE_H__ */
