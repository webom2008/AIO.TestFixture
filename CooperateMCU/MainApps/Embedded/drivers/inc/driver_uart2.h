/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : driver_uart2.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/2
  Last Modified :
  Description   : driver_uart2.c header file
  Function List :
  History       :
  1.Date        : 2015/9/2
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#ifndef __DRIVER_UART2_H__
#define __DRIVER_UART2_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

int Uart2Init(void);
int Uart2Open(void);
int Uart2Read(void);
int Uart2Write(void);
int Uart2Ctrl(void);
int Uart2Close(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DRIVER_UART2_H__ */
