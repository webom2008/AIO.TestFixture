/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : version.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/3/16
  Last Modified :
  Description   : version define
  Function List :
  History       :
  1.Date        : 2015/3/16
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#include <string.h>
#include "version.h"
/*----------------------------------------------*
 * external variables                           *
 *----------------------------------------------*/

/*----------------------------------------------*
 * external routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/

/*----------------------------------------------*
 * project-wide global variables                *
 *----------------------------------------------*/

/*----------------------------------------------*
 * internal variables                           *
 *----------------------------------------------*/
char APP_VERSION[60];

/*----------------------------------------------*
 * internal routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/
 
void version_init(void)
{
    memset(APP_VERSION, 0, sizeof(APP_VERSION));
    strcat(APP_VERSION, VERSION_OS);
    strcat(APP_VERSION, VERSION_CURSOR);
    strcat(APP_VERSION, VERSION_DATE);
    strcat(APP_VERSION, VERSION_CURSOR);
    strcat(APP_VERSION, VERSION_TIME);
    strcat(APP_VERSION, VERSION_CURSOR);
    strcat(APP_VERSION, VERSION_NUM);
}
