/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : uart_common.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/8/26
  Last Modified :
  Description   : uart_common.c header file
  Function List :
  History       :
  1.Date        : 2015/8/26
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#ifndef __UART_COMMON_H__
#define __UART_COMMON_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <stdbool.h>
#include "stm32f10x.h"



//defines for DMA MODE
#define UART_DMA_RX_INCOMPLETE_EVENT_BIT    (1 << 0)
#define UART_DMA_RX_COMPLETE_EVENT_BIT	    (1 << 1)


typedef enum
{
    UART_INTERRUPT_MODE = 0,
    UART_DMA_MODE
} UART_WORK_MODE_DEF;

typedef enum
{
    UART_NUM01 = 0,
    UART_NUM02,
    UART_NUM03,
    UART_NUM04,
    UART_NUM05,

    UART_NUM_MAX
} UART_NUM_DEF;


typedef enum
{
    B9600   = 9600u,
    B19200  = 19200u,
    B115200 = 115200u,
    B230400 = 230400u
} UART_BAUDRATE_DEF;

typedef enum
{
    PARITY_NONE  = 0,
    PARITY_ODD, //奇校验
    PARITY_EVEN //偶校验
} UART_PARITY_DEF;

typedef struct _UART_DMA_BUFFER_DEF_
{
    uint8_t     *pPingPongBuff1;
    uint8_t     *pPingPongBuff2;
    uint8_t     IdleBufferIndex;    /* current idle buffer index: 0-buffer1 idle, 1-buffer2 idle*/
    uint8_t     IsDMAWroking;       /* 0: idle , 1: working */
    uint16_t    nBuff1Offset;
    uint16_t    nBuff2Offset;
    uint16_t    nBuff1MaxLength;
    uint16_t    nBuff2MaxLength;
} UART_DMA_BUFFER_TypeDef;

typedef struct _UART_DEVICE_DEF_
{
    UART_NUM_DEF num;
    UART_WORK_MODE_DEF mode;
    UART_BAUDRATE_DEF baudrate;
    UART_PARITY_DEF ParityType;
    UART_DMA_BUFFER_TypeDef *pTxDMABuffer;
    UART_DMA_BUFFER_TypeDef *pRxDMABuffer;
    bool IsDeviceOpen;
    uint8_t IRQPriority;
} UART_DEVICE_TypeDef;

int UartDeviceDefaultInit(UART_DEVICE_TypeDef *pUartDevice);

int UartCommonInit(const UART_DEVICE_TypeDef *pUartDevice);

int UartCommonTerminate(const UART_DEVICE_TypeDef *pUartDevice);
int udprintf(const char* fmt, ...);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __UART_COMMON_H__ */

