/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : i2c_common.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/4
  Last Modified :
  Description   : i2c_common.c header file
  Function List :
  History       :
  1.Date        : 2015/9/4
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/

#ifndef __I2C_COMMON_H__
#define __I2C_COMMON_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


#include <stdbool.h>
#include "stm32f10x.h"


typedef enum
{
    I2C_NUM01 = 0,
    I2C_NUM02,
    I2C_NUM03,

    I2C_NUM_MAX
} I2C_NUM_DEF;

typedef struct _I2C_DEVICE_TypeDef_
{
    I2C_NUM_DEF num;
} I2C_DEVICE_TypeDef;



int I2CDeviceDefaultInit(I2C_DEVICE_TypeDef *pDevice);

int I2CCommonInit(const I2C_DEVICE_TypeDef *pDevice);

int I2CCommonTerminate(const I2C_DEVICE_TypeDef *pDevice);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __I2C_COMMON_H__ */
