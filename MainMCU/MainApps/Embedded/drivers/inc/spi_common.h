/******************************************************************************

  Copyright (C), 2005-2014, CVTE.
  
 ******************************************************************************
  File Name     : spi_common.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/4
  Last Modified :
  Description   : spi_common.c header file
  Function List :
  History       :
  1.Date        : 2015/9/4
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/

#ifndef __SPI_COMMON_H__
#define __SPI_COMMON_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <stdbool.h>
#include "stm32f10x.h"


typedef enum
{
    SPI_NUM01 = 0,
    SPI_NUM02,
    SPI_NUM03,

    SPI_NUM_MAX
} SPI_NUM_DEF;

typedef struct _SPI_DEVICE_TypeDef_
{
    SPI_NUM_DEF num;
} SPI_DEVICE_TypeDef;



int SpiDeviceDefaultInit(SPI_DEVICE_TypeDef *pDevice);

int SpiCommonInit(const SPI_DEVICE_TypeDef *pDevice);

int SpiCommonTerminate(const SPI_DEVICE_TypeDef *pDevice);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __SPI_COMMON_H__ */
