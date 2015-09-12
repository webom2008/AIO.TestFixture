/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : version.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/3/10
  Last Modified :
  Description   : version
  Function List :
  History       :
  1.Date        : 2015/3/10
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/

#ifndef __VERSION_H__
#define __VERSION_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

const char VERSION_DATE[15] = __DATE__;
const char VERSION_TIME[12] = __TIME__;
const char VERSION_NUM[8]   = "V00.01";
const char VERSION_CURSOR[2]= "-";
const char VERSION_OS[20]   = "FreeRTOS V8.2.1";
    
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __VERSION_H__ */


