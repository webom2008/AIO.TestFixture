/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : AppTestedAIO.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/16
  Last Modified :
  Description   : app for tested aio board
  Function List :
  History       :
  1.Date        : 2015/9/16
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#include "includes.h"

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

//#define _INFO_
#define _ERROR_

#ifdef _INFO_
#define INFO(fmt, arg...) udprintf("[MainMCU]Info: "fmt, ##arg)
#else
#define INFO(fmt, arg...) do{}while(0)
#endif
#ifdef _ERROR_
#define ERROR(fmt, arg...) udprintf("[MainMCU]Error: "fmt, ##arg)
#else
#define ERROR(fmt, arg...) do{}while(0)
#endif

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

int AppTestedAIOInit(void)
{
    return 0;
}

int AppTestedAIOStart(void)
{
    int ret = 0;

    ret = createAioBoardTask();
    ret |= createAioDspUnpackTask();
    while(0 != ret);
    return 0;
}

