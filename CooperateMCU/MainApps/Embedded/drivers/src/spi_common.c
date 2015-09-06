/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : spi_common.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/4
  Last Modified :
  Description   : spi common interrfaces
  Function List :
  History       :
  1.Date        : 2015/9/4
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/

#include "includes.h"
#include "spi_common.h"

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


int SpiDeviceDefaultInit(SPI_DEVICE_TypeDef *pDevice)
{
    return 0;
}

int SpiCommonInit(const SPI_DEVICE_TypeDef *pDevice)
{
    return 0;
}

int SpiCommonTerminate(const SPI_DEVICE_TypeDef *pDevice)
{
    return 0;
}
