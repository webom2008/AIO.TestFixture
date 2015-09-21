/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : driver_buzzer.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/21
  Last Modified :
  Description   : driver_buzzer.c header file
  Function List :
  History       :
  1.Date        : 2015/9/21
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/

#ifndef __DRIVER_BUZZER_H__
#define __DRIVER_BUZZER_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

typedef enum 
{
    BUZZER_OFF     = 0,
    BUZZER_ON,
    BUZZER_TRIGGER,
}Buzzer_Mode;

int BuzzerInit(void);
int BuzzerOpen(void);
int BuzzerRead(void);
int BuzzerWrite(void);
int BuzzerCtrl(Buzzer_Mode mode);
int BuzzerClose(void);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DRIVER_BUZZER_H__ */
