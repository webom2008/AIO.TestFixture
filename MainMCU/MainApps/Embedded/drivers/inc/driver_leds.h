/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : driver_leds.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/21
  Last Modified :
  Description   : driver_leds.c header file
  Function List :
  History       :
  1.Date        : 2015/9/21
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/


#ifndef __DRIVER_LEDS_H__
#define __DRIVER_LEDS_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

typedef enum 
{
    LED_RED         = 0,
    LED_GREEN,

    LEDS_TYPE_MAX
} Leds_Type;

typedef enum 
{
    LED_OFF     = 0,
    LED_ON,
    LED_TRIGGER,
}Leds_Mode;

int LedsInit(void);
int LedsOpen(void);
int LedsRead(char *pReadData, const int nDataLen);
int LedsWrite(char *pWriteData, const int nDataLen);
int LedsCtrl(Leds_Type type, Leds_Mode mode);
int LedsClose(void);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DRIVER_LEDS_H__ */
