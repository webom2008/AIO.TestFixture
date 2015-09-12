/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : driver_uart5.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/8
  Last Modified :
  Description   : driver_uart5.c header file
  Function List :
  History       :
  1.Date        : 2015/9/8
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/

#ifndef __DRIVER_UART5_H__
#define __DRIVER_UART5_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

int Uart5Init(void);
int Uart5Open(void);
int Uart5Read(char *pReadData, const int nDataLen);
int Uart5Write(char *pWriteData, const int nDataLen);
int Uart5Ctrl(void);
int Uart5Close(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DRIVER_UART5_H__ */
