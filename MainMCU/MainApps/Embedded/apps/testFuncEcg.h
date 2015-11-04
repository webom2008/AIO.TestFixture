/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : testFuncEcg.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/11/4
  Last Modified :
  Description   : testFuncEcg.c header file
  Function List :
  History       :
  1.Date        : 2015/11/4
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/

#ifndef __TESTFUNCECG_H__
#define __TESTFUNCECG_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern int testEcgSelfcheck(void);
extern int testEcgAmplitudeBand(void);
extern int testEcgPace(void);
extern int testEcgPolarity(void);
extern int testEcgProbeOff(void);
extern int testEcgQuickQRS(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __TESTFUNCECG_H__ */
