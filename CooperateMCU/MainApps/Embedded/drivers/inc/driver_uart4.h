/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : driver_uart4.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/8
  Last Modified :
  Description   : driver_uart4.c header file
  Function List :
  History       :
  1.Date        : 2015/9/8
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/

#ifndef __DRIVER_UART4_H__
#define __DRIVER_UART4_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

int Uart4Init(void);
int Uart4Open(void);
int Uart4Read(char *pReadData, const int nDataLen);
int Uart4Write(char *pWriteData, const int nDataLen);
int Uart4Ctrl(void);
int Uart4Close(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DRIVER_UART4_H__ */
