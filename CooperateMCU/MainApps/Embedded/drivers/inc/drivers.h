/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : drivers.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/2
  Last Modified :
  Description   : drivers.c header file
  Function List :
  History       :
  1.Date        : 2015/9/2
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/

#ifndef __DRIVERS_H__
#define __DRIVERS_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "uart_common.h"
#include "driver_uart1.h"
#include "driver_uart2.h"
#include "driver_uart3.h"
#include "driver_uart4.h"
#include "driver_uart5.h"

#include "spi_common.h"

#include "i2c_common.h"

#include "driver_dpm2200.h"
#include "driver_tdm.h"

int DriverMoudleInit(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DRIVERS_H__ */
