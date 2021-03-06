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
//    ret |= Uart4Init(); //delete for TDMx
//    ret |= Uart4Open(); //delete for TDMx
    ret |= Uart5Init();
    ret |= Uart5Open();
    
    ret |= DPM2200Init();
    ret |= DPM2200Open();
    
    ret |= TDMxInit();
    ret |= TDMxOpen();
    while (ret < 0); //error hanppen
    return 0;
}
