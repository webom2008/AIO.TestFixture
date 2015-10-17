/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : AppMainProcess.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/10/16
  Last Modified :
  Description   : AppMainProcess.c header file
  Function List :
  History       :
  1.Date        : 2015/10/16
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/


#ifndef __APPMAINPROCESS_H__
#define __APPMAINPROCESS_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


int MainProcessInit(void);
bool isMainProcessTaskRuning(void);
int createMainProcessTask(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __APPMAINPROCESS_H__ */
