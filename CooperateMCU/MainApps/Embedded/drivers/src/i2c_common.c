/******************************************************************************

    Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : i2c_common.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/4
  Last Modified :
  Description   : i2c common interrface
  Function List :
  History       :
  1.Date        : 2015/9/4
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/

#include "includes.h"
#include "i2c_common.h"

/*----------------------------------------------*
 * external variables                           *
 *----------------------------------------------*/

/*----------------------------------------------*
 * external routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * internal routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * project-wide global variables                *
 *----------------------------------------------*/

/*----------------------------------------------*
 * module-wide global variables                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

int I2CDeviceDefaultInit(I2C_DEVICE_TypeDef *pDevice)
{
    return 0;
}

int I2CCommonInit(const I2C_DEVICE_TypeDef *pDevice)
{
    return 0;
}

int I2CCommonTerminate(const I2C_DEVICE_TypeDef *pDevice)
{
    return 0;
}
