/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : driver_interAdc.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/22
  Last Modified :
  Description   : driver_interAdc.c header file
  Function List :
  History       :
  1.Date        : 2015/9/22
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/

#ifndef __DRIVER_INTERADC_H__
#define __DRIVER_INTERADC_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

typedef enum
{
    INTER_ADC_D3V3E     = 0,
    INTER_ADC_5V6N,
    INTER_ADC_D3V3N,
    INTER_ADC_5VAN,
    INTER_ADC_5V_SPO2,
    INTER_ADC_5V_NIBP,
    INTER_ADC_REF2V5N,
    
    INTER_ADC_TOTAL
} INTER_ADC_CH;

typedef enum
{
    INTER_ADC_CMD_START     = 0,
    INTER_ADC_CMD_STOP,
    
    INTER_ADC_CMD_UNVALID
} INTER_ADC_CTRL_CMD;


int InterAdcInit(void);
int InterAdcOpen(void);
int InterAdcRead(uint32_t *pResultArray, const int totalNums);
int InterAdcWrite(char *pWriteData, const int nDataLen);
int InterAdcCtrl(const INTER_ADC_CTRL_CMD cmd, void *parameter);
int InterAdcClose(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DRIVER_INTERADC_H__ */
