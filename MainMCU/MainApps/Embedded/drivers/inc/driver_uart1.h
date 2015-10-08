/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : driver_uart1.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/2
  Last Modified :
  Description   : driver_uart1.c header file
  Function List :
  History       :
  1.Date        : 2015/9/2
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#ifndef __DRIVER_UART1_H__
#define __DRIVER_UART1_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define UART1_TX_DMA_BUF_LEN        64
#define UART1_RX_DMA_BUF_LEN        64

int Uart1Init(void);
int Uart1Open(void);
int Uart1Read(char *pReadData, const int nDataLen);
int Uart1Write(char *pWriteData, const int nDataLen);
int Uart1Ctrl(void);
int Uart1Close(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DRIVER_UART1_H__ */
