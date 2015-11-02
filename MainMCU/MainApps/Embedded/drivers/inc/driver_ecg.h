/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : driver_ecg.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/10/30
  Last Modified :
  Description   : driver_ecg.c header file
  Function List :
  History       :
  1.Date        : 2015/10/30
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/


#ifndef __DRIVER_ECG_H__
#define __DRIVER_ECG_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */





typedef enum 
{
    CMD_ECG_RA_SEL,
    CMD_ECG_RL_SEL,
    CMD_ECG_LA_SEL,
    CMD_ECG_LL_SEL,
    CMD_ECG_V_SEL,
    CMD_ECG_RA_OFF,
    CMD_ECG_RL_OFF,
    CMD_ECG_LA_OFF,
    CMD_ECG_LL_OFF,
    CMD_ECG_V_OFF,

    CMD_ECG_CTRL_UNVALID,
} ECG_CTRL_CMD;

typedef enum 
{
    CMD_VAL_SEL_CH0,
    CMD_VAL_SEL_CH1,
    CMD_VAL_SEL_CH2,

    CMD_VAL_OFF_NORMAL,
    CMD_VAL_OFF_LEADOFF,
    
    CMD_VAL_UNVALID,
} ECG_CTRL_CMD_VAL;

int EcgDevInit(void);

int EcgDevCtrl(const ECG_CTRL_CMD cmd, const ECG_CTRL_CMD_VAL val);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DRIVER_ECG_H__ */
