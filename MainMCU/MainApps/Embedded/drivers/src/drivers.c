/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : drivers.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/2
  Last Modified :
  Description   : driver top interface
  Function List :
  History       :
  1.Date        : 2015/9/2
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#include "drivers.h"

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
int DriverMoudleInit(void)
{
    int ret = 0;
    ret |= Uart1Init();
    ret |= Uart1Open();
    ret |= Uart2Init();
    ret |= Uart2Open();
    ret |= Uart3Init();
    ret |= Uart3Open();
    ret |= Uart4Init();
    ret |= Uart4Open();
    ret |= Uart5Init();
    ret |= Uart5Open();
    
    ret |= AioBoardInit();
    ret |= AioBoardOpen();
    
    ret |= LedsInit();
    ret |= LedsOpen();
    ret |= KeysInit();
    ret |= KeysOpen();
    ret |= BuzzerInit();
    ret |= BuzzerOpen();

    ret |= Dac8568Init();
    ret |= Dac8568Open();
    
    ret |= InterAdcInit();
    ret |= InterAdcOpen();
    while (ret < 0); //error hanppen
    return 0;
}
