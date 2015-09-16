/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : driver_aioboard.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/16
  Last Modified :
  Description   : driver for aio board
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

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

int AioBoardInit(void)
{
    return 0;
}

int AioBoardOpen(void)
{
    return 0;
}

int AioBoardRead(char *pReadData, const int nDataLen)
{
    return 0;
}

int AioBoardWrite(char *pWriteData, const int nDataLen)
{
    return 0;
}

int AioBoardCtrl(CTRL_CMD_AIOBOARD cmd, void *pData)
{
    char *pVal = (char *)pData;

    switch(cmd)
    {
    case CTRL_CMD_AIOBOARD_SET_POWER:
        if (SW_ON == *pVal)
        {

        }
        else
        {

        }
        break;
    default:
        break;
    }
    return 0;
}

int AioBoardClose(void)
{
    return 0;
}
