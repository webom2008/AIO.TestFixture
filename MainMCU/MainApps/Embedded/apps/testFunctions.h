/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : testFunctions.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/10/23
  Last Modified :
  Description   : testFunctions.c header file
  Function List :
  History       :
  1.Date        : 2015/10/23
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/

#ifndef __TESTFUNCTIONS_H__
#define __TESTFUNCTIONS_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern int testPrepareAllReady(void);
extern int getEcgSelfcheck(void);
extern int testAioBoardMaxCurrent(void);
extern int testEcgAmplitudeBand(void);
extern int testEcgPace(void);
extern int testEcgPolarity(void);
extern int testEcgProbeOff(void);
extern int testEcgQuickQRS(void);
extern int testIbpAmplitudeBand(void);
extern int testIbpProbeOff(void);
extern int testIbpSelfcheck(void);
extern int testNibpGasControl(void);
extern int testNibpOverPress(void);
extern int testNibpSelfcheck(void);
extern int testNibpVerify(void);
extern int testRespAmplitudeBand(void);
extern int testSpo2Uart(void);
extern int testTempPrecision(void);
extern int testTempProbeOff(void);
extern int testTempSelfcheck(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __TESTFUNCTIONS_H__ */
