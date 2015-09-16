/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : includes.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/4/8
  Last Modified :
  Description   : includes.h header file
  Function List :
  History       :
  1.Date        : 2015/4/8
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/

#ifndef __INCLUDES_H__
#define __INCLUDES_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

//include standard hearders
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

//includes freeRTOS headers
#include "defines.h"
#include "queue.h"
#include "list.h"
#include "semphr.h"
#include "timers.h"
#include "event_groups.h"

//incudes stm32 headers
#include "stm32f10x.h"

//includes utilities headers
#include "fifo.h"

//includes driver headers
#include "drivers.h"


//includes apps headers
#include "../apps/drivers_test.h"
#include "../apps/CoopMcuProtocol.h"
#include "../apps/AppCoopMCU.h"


//includes others
#include "../platform/MyTimer.h"

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __INCLUDES_H__ */

