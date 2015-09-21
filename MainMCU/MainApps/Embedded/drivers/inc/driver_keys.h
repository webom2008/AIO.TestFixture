/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : driver_keys.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/21
  Last Modified :
  Description   : driver_keys.c header file
  Function List :
  History       :
  1.Date        : 2015/9/21
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/

#ifndef __DRIVER_KEYS_H__
#define __DRIVER_KEYS_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */




#define KEYS_EVENT_GROUP_BIT(type)          (1<<(type))


typedef enum 
{
    KEY_1         = 0,
    KEY_2,
    KEY_3,

    KEYS_TYPE_MAX
} Keys_Type;

typedef enum 
{
    KEY_IDLE         = 0,
    KEY_PRESSED,
} Keys_Val;

int KeysInit(void);
int KeysOpen(void);
int KeysRead(Keys_Type type, Keys_Val *pVal);
int KeysWrite(void);
int KeysCtrl(void);
int KeysClose(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DRIVER_KEYS_H__ */
