/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : driver_dpm2200.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/17
  Last Modified :
  Description   : driver_dpm2200.c header file
  Function List :
  History       :
  1.Date        : 2015/9/17
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/

#ifndef __DRIVER_DPM2200_H__
#define __DRIVER_DPM2200_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


typedef enum
{
    DPMCTRL_R_PRESS,
    DPMCTRL_W_UNITS,
    
    DPMCTRL_RESERVED             //Reserved 
} DPMCtrlCmd_TypeDef;

typedef enum
{
    PressUnits_PSI          = 0,
    PressUnits_mmHg_0C,
    PressUnits_mmHg_20C,
    PressUnits_inHg_0C,
    PressUnits_inHg_20C,
    PressUnits_cmH2O_20C,
    PressUnits_inH2O_4C,
    PressUnits_inH2O_20C,
    PressUnits_inH2O_60F,
    PressUnits_kgpcm2,
    PressUnits_kPa,
    PressUnits_mBar,
    PressUnits_Bar,
    
    PressUnits_RESERVED         //Reserved 
} PressUnits_TypeDef;

int DPM2200Init(void);
int DPM2200Open(void);
int DPM2200Read(const int location, char *pReadData, const int nDataLen);
int DPM2200Write(const int location, const int data);
int DPM2200Ctrl(const DPMCtrlCmd_TypeDef cmd, void *arg);
int DPM2200Close(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DRIVER_DPM2200_H__ */
