/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : driver_uart3.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/8
  Last Modified :
  Description   : driver_uart3.c header file
  Function List :
  History       :
  1.Date        : 2015/9/8
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/

#ifndef __DRIVER_UART3_H__
#define __DRIVER_UART3_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


typedef enum
{
    Uart3CTRL_ClearRxQueue,
    Uart3CTRL_ClearTxQueue,
    
    Uart3CTRL_RESERVED             //Reserved 
} Uart3CtrlCmd_TypeDef;


int Uart3Init(void);
int Uart3Open(void);
int Uart3Read(char *pReadData, const int nDataLen);
int Uart3Write(char *pWriteData, const int nDataLen);
int Uart3Ctrl(const Uart3CtrlCmd_TypeDef cmd, void *arg);
int Uart3Close(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DRIVER_UART3_H__ */
