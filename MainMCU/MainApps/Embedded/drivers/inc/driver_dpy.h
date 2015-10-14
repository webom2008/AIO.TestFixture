/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : driver_dpy.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/10/14
  Last Modified :
  Description   : driver_dpy.c header file
  Function List :
  History       :
  1.Date        : 2015/10/14
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/

#ifndef __DRIVER_DPY_H__
#define __DRIVER_DPY_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

int LedsDisplayInit(void);
int LedsDisplayOpen(void);
int LedsDisplayRead(void);
int LedsDisplayWrite(int data);
int LedsDisplayCtrl(void);
int LedsDisplayClose(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DRIVER_DPY_H__ */
