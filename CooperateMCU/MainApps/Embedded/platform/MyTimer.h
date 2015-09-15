/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : MyTimer.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/15
  Last Modified :
  Description   : MyTimer.c header file
  Function List :
  History       :
  1.Date        : 2015/9/15
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/

#ifndef __MYTIMER_H__
#define __MYTIMER_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


#define MY_TIM_TICK_PERIOD_MS           100

#define MY_TIM_DEFAULT_TIMEOUT_MS       400

int MyTimerInit(void);

int MyTimerStart(void);

u32 getMyTimerTick(void);

int IsMyTimerOnTime(const u32 u32Target);

int IsMyTimerOverTime(const u32 u32Base, const u32 u32Duration);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __MYTIMER_H__ */
