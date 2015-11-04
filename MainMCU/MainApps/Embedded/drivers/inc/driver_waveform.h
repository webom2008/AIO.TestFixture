/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : driver_waveform.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/11/2
  Last Modified :
  Description   : driver_waveform.c header file
  Function List :
  History       :
  1.Date        : 2015/11/2
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/

#ifndef __DRIVER_WAVEFORM_H__
#define __DRIVER_WAVEFORM_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


typedef enum 
{
    WF_CTRL_10Hz_1Vpp_SIN,
    WF_CTRL_0P5Hz_1Vpp_SIN,
    WF_CTRL_150Hz_1Vpp_SIN,
    WF_CTRL_2Hz_16Vpp_SIN,
    WF_CTRL_2Hz_16mVpp_SIN,
    WF_CTRL_PACE_A,
    WF_CTRL_PACE_B,
    WF_CTRL_PACE_C,
    WF_CTRL_PACE_D,
    WF_CTRL_PACE_E,
    WF_CTRL_PACE_F,
    WF_CTRL_PACE_G,
    WF_CTRL_PACE_H,
    
    WF_CTRL_UNVALID,
} WAVEFORM_CTRL_CMD;

typedef enum 
{
    WAVEFORM_DELAY_10Hz_SIN,
    WAVEFORM_DELAY_0P5Hz_SIN,
    WAVEFORM_DELAY_150Hz_SIN,
    
    WAVEFORM_DELAY_UNVALID,
} WAVEFORM_DELAY;


#define WF_COMM_CID_SET_SIN_10Hz1Vpp        ((u8)0x00)
#define WF_COMM_CID_SET_SIN_0P5Hz1Vpp       ((u8)0x01)
#define WF_COMM_CID_SET_SIN_150Hz1Vpp       ((u8)0x02)
#define WF_COMM_CID_SET_SIN_2Hz16Vpp        ((u8)0x03)
#define WF_COMM_CID_SET_SIN_2Hz16mVpp       ((u8)0x04)
#define WF_COMM_CID_SET_PACE_A              ((u8)0x05)
#define WF_COMM_CID_SET_PACE_B              ((u8)0x06)
#define WF_COMM_CID_SET_PACE_C              ((u8)0x07)
#define WF_COMM_CID_SET_PACE_D              ((u8)0x08)
#define WF_COMM_CID_SET_PACE_E              ((u8)0x09)
#define WF_COMM_CID_SET_PACE_F              ((u8)0x0A)
#define WF_COMM_CID_SET_PACE_G              ((u8)0x0B)
#define WF_COMM_CID_SET_PACE_H              ((u8)0x0C)
#define WF_COMM_CID_SET_ARB                 ((u8)0xFF)


extern int WavefromCtrl(const WAVEFORM_CTRL_CMD cmd, void *arg);
extern int WavefromDelay(const WAVEFORM_DELAY type, const char times);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DRIVER_WAVEFORM_H__ */
