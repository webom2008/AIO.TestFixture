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

void alarmPowerDetect(void)
{
    int i = 0;
    static u8 pkt_num;
    AioDspProtocolPkt pkt;
    ALARM_PWR_RESULT *pAlarmPwrResult = (ALARM_PWR_RESULT *)&pkt.DataAndCRC[1]; 

    initComputerPkt(&pkt);
    pkt.DataAndCRC[0] = (u8)COMP_ID_PWR_ALARM;
    
    if (0 == InterAdcRead(pAlarmPwrResult->u32AdcResultmV, INTER_ADC_TOTAL))
    {
        INFO("InterAdcRead :\r\n");
        for(i = 0; i < INTER_ADC_TOTAL; i++)
        {
            INFO("%d :%d mV\r\n", i, pAlarmPwrResult->u32AdcResultmV[i]);
        }

        if((pAlarmPwrResult->u32AdcResultmV[INTER_ADC_D3V3E] < D3V3E_NORMAL_ADC_MIN) \
            || (pAlarmPwrResult->u32AdcResultmV[INTER_ADC_D3V3E] > D3V3E_NORMAL_ADC_MAX))
        {
            pAlarmPwrResult->flag |= PWR_BIT_D3V3E_MASK;
        }

        if((pAlarmPwrResult->u32AdcResultmV[INTER_ADC_5V6N] < D5V6N_NORMAL_ADC_MIN) \
            || (pAlarmPwrResult->u32AdcResultmV[INTER_ADC_5V6N] > D5V6N_NORMAL_ADC_MAX))
        {
            pAlarmPwrResult->flag |= PWR_BIT_5V6N_MASK;
        }
        
        if((pAlarmPwrResult->u32AdcResultmV[INTER_ADC_D3V3N] < D3V3N_NORMAL_ADC_MIN) \
            || (pAlarmPwrResult->u32AdcResultmV[INTER_ADC_D3V3N] > D3V3N_NORMAL_ADC_MAX))
        {
            pAlarmPwrResult->flag |= PWR_BIT_D3V3N_MASK;
        }
        
        if((pAlarmPwrResult->u32AdcResultmV[INTER_ADC_5VAN] < D5VAN_NORMAL_ADC_MIN) \
            || (pAlarmPwrResult->u32AdcResultmV[INTER_ADC_5VAN] > D5VAN_NORMAL_ADC_MAX))
        {
            pAlarmPwrResult->flag |= PWR_BIT_5VAN_MASK;
        }
        
        if((pAlarmPwrResult->u32AdcResultmV[INTER_ADC_5V_SPO2] < D5V_SPO2_NORMAL_ADC_MIN) \
            || (pAlarmPwrResult->u32AdcResultmV[INTER_ADC_5V_SPO2] > D5V_SPO2_NORMAL_ADC_MAX))
        {
            pAlarmPwrResult->flag |= PWR_BIT_5V_SPO2_MASK;
        }
        
        if((pAlarmPwrResult->u32AdcResultmV[INTER_ADC_5V_NIBP] < D5V_NIBP_NORMAL_ADC_MIN) \
            || (pAlarmPwrResult->u32AdcResultmV[INTER_ADC_5V_NIBP] > D5V_NIBP_NORMAL_ADC_MAX))
        {
            pAlarmPwrResult->flag |= PWR_BIT_5V_NIBP_MASK;
        }
        
        if((pAlarmPwrResult->u32AdcResultmV[INTER_ADC_REF2V5N] < REF2V5N_NORMAL_ADC_MIN) \
            || (pAlarmPwrResult->u32AdcResultmV[INTER_ADC_REF2V5N] > REF2V5N_NORMAL_ADC_MAX))
        {
            pAlarmPwrResult->flag |= PWR_BIT_REF2V5N_MASK;
        }

        pkt.Length = sizeof(ALARM_PWR_RESULT) + 1;// 1 CID + data
        pkt.PacketNum = pkt_num++;
        pkt.DataAndCRC[pkt.Length] = crc8ComputerPkt(&pkt);
        sendComputerPkt(&pkt);
    }
}

