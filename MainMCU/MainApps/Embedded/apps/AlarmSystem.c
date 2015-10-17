/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : AlarmSystem.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/23
  Last Modified :
  Description   : alarm system
  Function List :
  History       :
  1.Date        : 2015/9/23
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
typedef struct
{
    uint16_t flag;
    uint32_t u32AdcResultmV[INTER_ADC_TOTAL];
} ALARM_PWR_RESULT;

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/

#define DETECT_POWER_COUNT      ((u8)5)


#define D3V3E_NORMAL_ADC        ((uint16_t) (3300 / 2)) //mV
#define D3V3E_NORMAL_RANGE      ((uint16_t) 1)    //%
#define D3V3E_NORMAL_ADC_MAX    (uint16_t)(D3V3E_NORMAL_ADC * (100 + D3V3E_NORMAL_RANGE) / 100)
#define D3V3E_NORMAL_ADC_MIN    (uint16_t)(D3V3E_NORMAL_ADC * (100 - D3V3E_NORMAL_RANGE) / 100)

#define D5V6N_NORMAL_ADC        ((uint16_t) (5700 / 2)) //mV
#define D5V6N_NORMAL_RANGE      ((uint16_t) 4)    //%
#define D5V6N_NORMAL_ADC_MAX    (uint16_t)(D5V6N_NORMAL_ADC * (100 + D5V6N_NORMAL_RANGE) / 100)
#define D5V6N_NORMAL_ADC_MIN    (uint16_t)(D5V6N_NORMAL_ADC * (100 - D5V6N_NORMAL_RANGE) / 100)

#define D3V3N_NORMAL_ADC        ((uint16_t) (3300 / 2)) //mV
#define D3V3N_NORMAL_RANGE      ((uint16_t) 1)    //%
#define D3V3N_NORMAL_ADC_MAX    (uint16_t)(D3V3E_NORMAL_ADC * (100 + D3V3E_NORMAL_RANGE) / 100)
#define D3V3N_NORMAL_ADC_MIN    (uint16_t)(D3V3E_NORMAL_ADC * (100 - D3V3E_NORMAL_RANGE) / 100)

#define D5VAN_NORMAL_ADC        ((uint16_t) (5000 / 2)) //mV
#define D5VAN_NORMAL_RANGE      ((uint16_t) 1)    //%
#define D5VAN_NORMAL_ADC_MAX    (uint16_t)(D5VAN_NORMAL_ADC * (100 + D5VAN_NORMAL_RANGE) / 100)
#define D5VAN_NORMAL_ADC_MIN    (uint16_t)(D5VAN_NORMAL_ADC * (100 - D5VAN_NORMAL_RANGE) / 100)

#define D5V_SPO2_NORMAL_ADC     ((uint16_t) (5000 / 2)) //mV
#define D5V_SPO2_NORMAL_RANGE   ((uint16_t) 1)    //%
#define D5V_SPO2_NORMAL_ADC_MAX (uint16_t)(D5V_SPO2_NORMAL_ADC * (100 + D5V_SPO2_NORMAL_RANGE) / 100)
#define D5V_SPO2_NORMAL_ADC_MIN (uint16_t)(D5V_SPO2_NORMAL_ADC * (100 - D5V_SPO2_NORMAL_RANGE) / 100)

#define D5V_NIBP_NORMAL_ADC     ((uint16_t) (5000 /2)) //mV
#define D5V_NIBP_NORMAL_RANGE   ((uint16_t) 1)    //%
#define D5V_NIBP_NORMAL_ADC_MAX (uint16_t)(D5V_NIBP_NORMAL_ADC * (100 + D5V_NIBP_NORMAL_RANGE) / 100)
#define D5V_NIBP_NORMAL_ADC_MIN (uint16_t)(D5V_NIBP_NORMAL_ADC * (100 - D5V_NIBP_NORMAL_RANGE) / 100)

#define REF2V5N_NORMAL_ADC      ((uint16_t) (2500 / 2)) //mV
#define REF2V5N_NORMAL_RANGE    ((uint16_t) 1)    //%
#define REF2V5N_NORMAL_ADC_MAX  (uint16_t)(D3V3E_NORMAL_ADC * (100 + D3V3E_NORMAL_RANGE) / 100)
#define REF2V5N_NORMAL_ADC_MIN  (uint16_t)(D3V3E_NORMAL_ADC * (100 - D3V3E_NORMAL_RANGE) / 100)

#define PWR_BIT_D3V3E_MASK      ((uint16_t) (0x0001 << INTER_ADC_D3V3E))
#define PWR_BIT_5V6N_MASK       ((uint16_t) (0x0001 << INTER_ADC_5V6N))
#define PWR_BIT_D3V3N_MASK      ((uint16_t) (0x0001 << INTER_ADC_D3V3N))
#define PWR_BIT_5VAN_MASK       ((uint16_t) (0x0001 << INTER_ADC_5VAN))
#define PWR_BIT_5V_SPO2_MASK    ((uint16_t) (0x0001 << INTER_ADC_5V_SPO2))
#define PWR_BIT_5V_NIBP_MASK    ((uint16_t) (0x0001 << INTER_ADC_5V_NIBP))
#define PWR_BIT_REF2V5N_MASK    ((uint16_t) (0x0001 << INTER_ADC_REF2V5N))

//#define _INFO_
#define _ERROR_

#ifdef _INFO_
#define INFO(fmt, arg...) udprintf("[Alarm]Info: "fmt, ##arg)
#else
#define INFO(fmt, arg...) do{}while(0)
#endif
#ifdef _ERROR_
#define ERROR(fmt, arg...) udprintf("[Alarm]Error: "fmt, ##arg)
#else
#define ERROR(fmt, arg...) do{}while(0)
#endif
/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

/*****************************************************************************
 Prototype    : testAIOBaordD3V3EPower
 Description  : D3V3E power detect
 Input        : void  
 Output       : None
 Return Value : 1: power alarm      0: power normal
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2015/10/16
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
int testAIOBaordD3V3EPower(void)
{
    int i;
    uint32_t adc_array[INTER_ADC_TOTAL];
    uint32_t d3v3e_adc = 0;
    AioDspProtocolPkt pkt;
   
    
    InterAdcCtrl(INTER_ADC_CMD_START,NULL);
    
//    InterAdcRead(adc_array, INTER_ADC_TOTAL); //drop first time
    for (i = 0; i < DETECT_POWER_COUNT; i++)
    {
        InterAdcRead(adc_array, INTER_ADC_TOTAL);
        d3v3e_adc += adc_array[INTER_ADC_D3V3E];
        INFO("[%d]=%d \r\n", i, adc_array[INTER_ADC_D3V3E]);
        vTaskDelay(5);
    }
    
    InterAdcCtrl(INTER_ADC_CMD_STOP,NULL);

    //calc average
    d3v3e_adc = d3v3e_adc / DETECT_POWER_COUNT;
    INFO("average=%d \r\n", d3v3e_adc);
    
    if((d3v3e_adc < D3V3E_NORMAL_ADC_MIN) || (d3v3e_adc > D3V3E_NORMAL_ADC_MAX))
    {
        i = 0;
        initComputerPkt(&pkt);
        pkt.DataAndCRC[i++] = (u8)COMP_ID_ERROR_INFO;
        pkt.DataAndCRC[i++] = (u8)ERR_INFO_ID_D3V3E_PWR;
        pkt.DataAndCRC[i++] = (u8)((d3v3e_adc >> 24) & 0xFF);
        pkt.DataAndCRC[i++] = (u8)((d3v3e_adc >> 16) & 0xFF);
        pkt.DataAndCRC[i++] = (u8)((d3v3e_adc >> 8) & 0xFF);
        pkt.DataAndCRC[i++] = (u8)(d3v3e_adc & 0xFF);
        pkt.Length = i;
        pkt.DataAndCRC[pkt.Length] = crc8ComputerPkt(&pkt);
        sendComputerPkt(&pkt);
        
        ERROR("D3V3E=%d mV\r\n", d3v3e_adc);
        return 1;
    }
    return 0;
}

uint16_t testAIOBaordOtherPower(void)
{
    int i,j;
    uint32_t adc_array[INTER_ADC_TOTAL];
    AioDspProtocolPkt pkt;
    ALARM_PWR_RESULT *pAlarmPwrResult = NULL; 

    initComputerPkt(&pkt);
    pkt.DataAndCRC[0] = (u8)COMP_ID_ERROR_INFO;
    pkt.DataAndCRC[1] = (u8)ERR_INFO_ID_OTHER_PWR;
    pAlarmPwrResult = (ALARM_PWR_RESULT *)&pkt.DataAndCRC[2];

    
    InterAdcCtrl(INTER_ADC_CMD_START, NULL);
//    InterAdcRead(adc_array, INTER_ADC_TOTAL); //drop first time
    for (i = 0; i < DETECT_POWER_COUNT; i++)
    {
        InterAdcRead(adc_array, INTER_ADC_TOTAL);
        for (j = 0; j < INTER_ADC_TOTAL; j++)
        {
            pAlarmPwrResult->u32AdcResultmV[j] += adc_array[j];
        }
    }
    InterAdcCtrl(INTER_ADC_CMD_STOP, NULL);

    //calc average
    for (j = 0; j < INTER_ADC_TOTAL; j++)
    {
        pAlarmPwrResult->u32AdcResultmV[j] = pAlarmPwrResult->u32AdcResultmV[j] / DETECT_POWER_COUNT;
        INFO("average[%d]=%d \r\n", j, pAlarmPwrResult->u32AdcResultmV[j]);
        vTaskDelay(5);
    }
    
    if((pAlarmPwrResult->u32AdcResultmV[INTER_ADC_D3V3E] < D3V3E_NORMAL_ADC_MIN) \
        || (pAlarmPwrResult->u32AdcResultmV[INTER_ADC_D3V3E] > D3V3E_NORMAL_ADC_MAX))
    {
        pAlarmPwrResult->flag |= PWR_BIT_D3V3E_MASK;
        ERROR("D3V3E=%d mV\r\n", pAlarmPwrResult->u32AdcResultmV[INTER_ADC_D3V3E]);
    }

    if((pAlarmPwrResult->u32AdcResultmV[INTER_ADC_5V6N] < D5V6N_NORMAL_ADC_MIN) \
        || (pAlarmPwrResult->u32AdcResultmV[INTER_ADC_5V6N] > D5V6N_NORMAL_ADC_MAX))
    {
        pAlarmPwrResult->flag |= PWR_BIT_5V6N_MASK;
        ERROR("5V6N=%d mV\r\n", pAlarmPwrResult->u32AdcResultmV[INTER_ADC_5V6N]);
    }
    
    if((pAlarmPwrResult->u32AdcResultmV[INTER_ADC_D3V3N] < D3V3N_NORMAL_ADC_MIN) \
        || (pAlarmPwrResult->u32AdcResultmV[INTER_ADC_D3V3N] > D3V3N_NORMAL_ADC_MAX))
    {
        pAlarmPwrResult->flag |= PWR_BIT_D3V3N_MASK;
        ERROR("D3V3N=%d mV\r\n", pAlarmPwrResult->u32AdcResultmV[INTER_ADC_D3V3N]);
    }
    
    if((pAlarmPwrResult->u32AdcResultmV[INTER_ADC_5VAN] < D5VAN_NORMAL_ADC_MIN) \
        || (pAlarmPwrResult->u32AdcResultmV[INTER_ADC_5VAN] > D5VAN_NORMAL_ADC_MAX))
    {
        pAlarmPwrResult->flag |= PWR_BIT_5VAN_MASK;
        ERROR("5VAN=%d mV\r\n", pAlarmPwrResult->u32AdcResultmV[INTER_ADC_5VAN]);
    }
    
    if((pAlarmPwrResult->u32AdcResultmV[INTER_ADC_5V_SPO2] < D5V_SPO2_NORMAL_ADC_MIN) \
        || (pAlarmPwrResult->u32AdcResultmV[INTER_ADC_5V_SPO2] > D5V_SPO2_NORMAL_ADC_MAX))
    {
        pAlarmPwrResult->flag |= PWR_BIT_5V_SPO2_MASK;
        ERROR("5V_SPO2=%d mV\r\n", pAlarmPwrResult->u32AdcResultmV[INTER_ADC_5V_SPO2]);
    }
    
    if((pAlarmPwrResult->u32AdcResultmV[INTER_ADC_5V_NIBP] < D5V_NIBP_NORMAL_ADC_MIN) \
        || (pAlarmPwrResult->u32AdcResultmV[INTER_ADC_5V_NIBP] > D5V_NIBP_NORMAL_ADC_MAX))
    {
        pAlarmPwrResult->flag |= PWR_BIT_5V_NIBP_MASK;
        ERROR("5V_NIBP=%d mV\r\n", pAlarmPwrResult->u32AdcResultmV[INTER_ADC_5V_NIBP]);
    }
    
    if((pAlarmPwrResult->u32AdcResultmV[INTER_ADC_REF2V5N] < REF2V5N_NORMAL_ADC_MIN) \
        || (pAlarmPwrResult->u32AdcResultmV[INTER_ADC_REF2V5N] > REF2V5N_NORMAL_ADC_MAX))
    {
        pAlarmPwrResult->flag |= PWR_BIT_REF2V5N_MASK;
        ERROR("REF2V5N=%d mV\r\n", pAlarmPwrResult->u32AdcResultmV[INTER_ADC_REF2V5N]);
    }

    pkt.Length = sizeof(ALARM_PWR_RESULT) + 2;// 2 CID + data
    pkt.DataAndCRC[pkt.Length] = crc8ComputerPkt(&pkt);
    sendComputerPkt(&pkt);
    return pAlarmPwrResult->flag;
}
