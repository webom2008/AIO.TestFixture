/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : testFuncEcg.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/11/4
  Last Modified :
  Description   : test funciton for ecg module
  Function List :
  History       :
  1.Date        : 2015/11/4
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#include "includes.h"

/*----------------------------------------------*
 * external variables                           *
 *----------------------------------------------*/
extern EventGroupHandle_t xCoopMCUPktAckEventGroup;
extern EventGroupHandle_t xCompPktAckEventGroup;
extern EventGroupHandle_t xDspPktAckEventGroup;

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

typedef enum
{
    PACE_RA_LA_I     = 1,
    PACE_RA_LL_II,
    PACE_V,
}PACE_CHANNEL;

typedef enum
{
    CHECK_LL_OFF = 0x00,
    CHECK_LA_OFF,
    CHECK_RA_OFF,
    CHECK_RL_OFF,
    CHECK_V_OFF,
    CHECK_LL_LA_OFF,
    CHECK_LA_RA_OFF,
    CHECK_LL_RA_OFF,
    CHECK_LL_LA_RA_OFF,
    CHECK_ALL_OFF,
} CHECK_ECG_PROBE;


typedef enum
{ 
    ADULT     = 1,     
    CHILD     = 2,
    NEWBORN   = 3,
} PATIENT_TypeDef;

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/
#define ECG_AMP_DELAY_MS                    2000
#define ECG_PROBE_OFF_DELAY_MS              2000
#define ECG_POLARITY_DELAY_MS               2000
#define WAIT_FOR_WAVEFORM_STABLE_MS         4000


#define ECG_AMP_U1_1            ((u16)500) //uV
#define ECG_AMP_U1_1_RANGE      ((u8)1)    //%
#define ECG_AMP_U1_1_MAX        (u16)(ECG_AMP_U1_1 * (100 + ECG_AMP_U1_1_RANGE) / 100)
#define ECG_AMP_U1_1_MIN        (u16)(ECG_AMP_U1_1 * (100 - ECG_AMP_U1_1_RANGE) / 100)

#define ECG_AMP_U1_2            ((u16)1000) //uV
#define ECG_AMP_U1_2_RANGE      ((u8)1)    //%
#define ECG_AMP_U1_2_MAX        (u16)(ECG_AMP_U1_2 * (100 + ECG_AMP_U1_2_RANGE) / 100)
#define ECG_AMP_U1_2_MIN        (u16)(ECG_AMP_U1_2 * (100 - ECG_AMP_U1_2_RANGE) / 100)

#define ECG_AMP_U1_3            ((u16)500) //uV
#define ECG_AMP_U1_3_RANGE      ((u8)1)    //%
#define ECG_AMP_U1_3_MAX        (u16)(ECG_AMP_U1_3 * (100 + ECG_AMP_U1_3_RANGE) / 100)
#define ECG_AMP_U1_3_MIN        (u16)(ECG_AMP_U1_3 * (100 - ECG_AMP_U1_3_RANGE) / 100)

#define ECG_AMP_U2_1_MAX        (u16)(500) //uV
#define ECG_AMP_U2_1_MIN        (u16)(375) //uV
#define ECG_AMP_U2_2_MAX        (u16)(1000) //uV
#define ECG_AMP_U2_2_MIN        (u16)(750) //uV
#define ECG_AMP_U2_3_MAX        (u16)(500) //uV
#define ECG_AMP_U2_3_MIN        (u16)(375) //uV


#define ECG_AMP_U3_1_MAX        (u16)(500) //uV
#define ECG_AMP_U3_1_MIN        (u16)(375) //uV
#define ECG_AMP_U3_2_MAX        (u16)(1000) //uV
#define ECG_AMP_U3_2_MIN        (u16)(750) //uV
#define ECG_AMP_U3_3_MAX        (u16)(500) //uV
#define ECG_AMP_U3_3_MIN        (u16)(375) //uV

//#define PROBE_INFO_MASK_ALL     (u16)(0x3FF << 2)
#define PROBE_INFO_MASK_ALL     (u16)(0x1F << 2)
#define PROBE_INFO_MASK_LA      (u16)(0x01 << 2)
#define PROBE_INFO_MASK_RA      (u16)(0x01 << 3)
#define PROBE_INFO_MASK_LL      (u16)(0x01 << 4)
#define PROBE_INFO_MASK_RL      (u16)(0x01 << 5)
#define PROBE_INFO_MASK_V1      (u16)(0x01 << 6)
#define PROBE_INFO_MASK_V2      (u16)(0x01 << 7)
#define PROBE_INFO_MASK_V3      (u16)(0x01 << 8)
#define PROBE_INFO_MASK_V4      (u16)(0x01 << 9)
#define PROBE_INFO_MASK_V5      (u16)(0x01 << 10)
#define PROBE_INFO_MASK_V6      (u16)(0x01 << 11)


#define _INFO_
#define _ERROR_

#ifdef _INFO_
#define INFO(fmt, arg...) udprintf("[EcgTest]Info: "fmt, ##arg)
#else
#define INFO(fmt, arg...) do{}while(0)
#endif
#ifdef _ERROR_
#define ERROR(fmt, arg...) udprintf("[EcgTest]Error: "fmt, ##arg)
#else
#define ERROR(fmt, arg...) do{}while(0)
#endif
/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

int testEcgSelfcheck(void)
{
    u8 pBuf[1] = {0,};
    EventBits_t uxBits = 0;

    pBuf[0] = (u8)ECG_DEB_CID_GET_SELFCHECK;
    xEventGroupClearBits(gpEcgDebug->xEventGroup, ECG_DEB_PKT_BIT_SELFCHECK);
    sendAioDspPktByID(AIO_RX_ECG_Debug_ID, (char *)pBuf, 1, 0);
    
    uxBits = xEventGroupWaitBits(
            gpEcgDebug->xEventGroup,   // The event group being tested.
            ECG_DEB_PKT_BIT_SELFCHECK,    // The bits within the event group to wait for.
            pdTRUE,                     // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
            pdFALSE,                    // Don't wait for both bits, either bit will do.
            1000 / portTICK_PERIOD_MS );// Wait a maximum of for either bit to be set.
    if (uxBits & ECG_DEB_PKT_BIT_SELFCHECK)
    {
        if (0x00 == gpEcgDebug->u8SelfcheckResult)
        {
            INFO("EcgSelfcheck OK!!\r\n");
        }
        else
        {
            ERROR("EcgSelfcheck Error!!\r\n");
        }
        return 0;
    }
    ERROR("testEcgSelfcheck timeout!!!\r\n");
    return -1;
}

static int checkEcgAmplitudeU1(const u16 ecg1, const u16 ecg2, const u16 ecg3)
{
    AioDspProtocolPkt pkt;
    int i = 0;
    u8 alarm = 0;

    if ((ecg1 > ECG_AMP_U1_1_MAX) || (ecg1 < ECG_AMP_U1_1_MIN))
    {
        alarm |= (0x01 << 0);
    }

    if ((ecg2 > ECG_AMP_U1_2_MAX) || (ecg2 < ECG_AMP_U1_2_MIN))
    {
        alarm |= (0x01 << 1);
    }
    
    if ((ecg3 > ECG_AMP_U1_3_MAX) || (ecg3 < ECG_AMP_U1_3_MIN))
    {
        alarm |= (0x01 << 2);
    }
    
    if (0 != alarm)
    {
        i = 0;
        initComputerPkt(&pkt);
        pkt.DataAndCRC[i++] = (u8)COMP_ID_ERROR_INFO;
        pkt.DataAndCRC[i++] = (u8)ERR_INFO_ID_ECG_AMP;
        pkt.DataAndCRC[i++] = alarm;
        pkt.Length = i;
        pkt.DataAndCRC[pkt.Length] = crc8ComputerPkt(&pkt);
        sendComputerPkt(&pkt);
        return -1;
    }
    return 0;
}

static int checkEcgAmplitudeU2(const u16 ecg1, const u16 ecg2, const u16 ecg3)
{
    AioDspProtocolPkt pkt;
    int i = 0;
    u8 alarm = 0;

    if ((ecg1 > ECG_AMP_U2_1_MAX) || (ecg1 < ECG_AMP_U2_1_MIN))
    {
        alarm |= (0x01 << 0);
    }

    if ((ecg2 > ECG_AMP_U2_2_MAX) || (ecg2 < ECG_AMP_U2_2_MIN))
    {
        alarm |= (0x01 << 1);
    }
    
    if ((ecg3 > ECG_AMP_U2_3_MAX) || (ecg3 < ECG_AMP_U2_3_MIN))
    {
        alarm |= (0x01 << 2);
    }
    
    if (0 != alarm)
    {
        i = 0;
        initComputerPkt(&pkt);
        pkt.DataAndCRC[i++] = (u8)COMP_ID_ERROR_INFO;
        pkt.DataAndCRC[i++] = (u8)ERR_INFO_ID_ECG_0P5HZ;
        pkt.DataAndCRC[i++] = alarm;
        pkt.Length = i;
        pkt.DataAndCRC[pkt.Length] = crc8ComputerPkt(&pkt);
        sendComputerPkt(&pkt);
        return -1;
    }
    return 0;
}

static int checkEcgAmplitudeU3(const u16 ecg1, const u16 ecg2, const u16 ecg3)
{
    AioDspProtocolPkt pkt;
    int i = 0;
    u8 alarm = 0;

    if ((ecg1 > ECG_AMP_U3_1_MAX) || (ecg1 < ECG_AMP_U3_1_MIN))
    {
        alarm |= (0x01 << 0);
    }

    if ((ecg2 > ECG_AMP_U3_2_MAX) || (ecg2 < ECG_AMP_U3_2_MIN))
    {
        alarm |= (0x01 << 1);
    }
    
    if ((ecg3 > ECG_AMP_U3_3_MAX) || (ecg3 < ECG_AMP_U3_3_MIN))
    {
        alarm |= (0x01 << 2);
    }
      
    if (0 != alarm)
    {
        i = 0;
        initComputerPkt(&pkt);
        pkt.DataAndCRC[i++] = (u8)COMP_ID_ERROR_INFO;
        pkt.DataAndCRC[i++] = (u8)ERR_INFO_ID_ECG_150HZ;
        pkt.DataAndCRC[i++] = alarm;
        pkt.Length = i;
        pkt.DataAndCRC[pkt.Length] = crc8ComputerPkt(&pkt);
        sendComputerPkt(&pkt);
        return -1;
    }
    return 0;
}

int testEcgAmplitudeBand(void)
{
    u16 u1_ecg1, u1_ecg2, u1_ecg3;
    u16 u2_ecg1, u2_ecg2, u2_ecg3;
    u16 u3_ecg1, u3_ecg2, u3_ecg3;
    int ret = 0;
    //S1:ECG switch to EcgOut
    if( EcgDevCtrl(CMD_ECG_ALL_SEL_ECGOUT, CMD_VAL_UNVALID) < 0)
    {
        ERROR("testEcgAmplitudeBand EcgDevCtrl!!!\r\n");
        return -1;
    }
    
    //S2:Set Waveform Device
    if (WavefromCtrl(WF_CTRL_10Hz_1Vpp_SIN, NULL) < 0)
    {
        ERROR("testEcgAmplitudeBand WavefromCtrl!!!\r\n");
        return -1;
    }
    
    //S3:delay
    vTaskDelay(ECG_AMP_DELAY_MS/portTICK_PERIOD_MS);

    //S4:Start AIO Factory
    if (AioEcgDebugCtrl(ECG_DEB_CID_START_VPP, NULL) < 0)
    {
        ERROR("testEcgAmplitudeBand AioEcgDebugCtrl!!!\r\n");
        return -1;
    }
    
    //S5:delay for 2 cycle
    WavefromDelay(WAVEFORM_DELAY_10Hz_SIN, 2);
    
    //S6:Stop and get AIO result
    if (AioEcgDebugCtrl(ECG_DEB_CID_STOP_VPP, NULL) < 0)
    {
        ERROR("testEcgAmplitudeBand AioEcgDebugCtrl!!!\r\n");
        return -1;
    }

    u1_ecg1 = gpEcgDebug->ecgVppResult.VppECG1;
    u1_ecg2 = gpEcgDebug->ecgVppResult.VppECG2;
    u1_ecg3 = gpEcgDebug->ecgVppResult.VppECG3;
    udprintf("WF_CTRL_10Hz_1Vpp_SIN Result:\r\n");
    udprintf("u1_ecg1 = %d uV\r\n",u1_ecg1);
    udprintf("u1_ecg2 = %d uV\r\n",u1_ecg2);
    udprintf("u1_ecg3 = %d uV\r\n",u1_ecg3);
        
    //S2:Set Waveform Device
    if (WavefromCtrl(WF_CTRL_0P5Hz_1Vpp_SIN, NULL) < 0)
    {
        ERROR("testEcgAmplitudeBand WavefromCtrl!!!\r\n");
        return -1;
    }
    
    //S3:delay
    vTaskDelay(ECG_AMP_DELAY_MS/portTICK_PERIOD_MS);
    
    //S4:Start AIO Factory
    if (AioEcgDebugCtrl(ECG_DEB_CID_START_VPP, NULL) < 0)
    {
        ERROR("testEcgAmplitudeBand AioEcgDebugCtrl!!!\r\n");
        return -1;
    }
    
    //S5:delay for 2 cycle
    WavefromDelay(WAVEFORM_DELAY_0P5Hz_SIN, 2);
    
    //S6:Stop and get AIO result
    if (AioEcgDebugCtrl(ECG_DEB_CID_STOP_VPP, NULL) < 0)
    {
        ERROR("testEcgAmplitudeBand AioEcgDebugCtrl!!!\r\n");
        return -1;
    }
    
    u2_ecg1 = gpEcgDebug->ecgVppResult.VppECG1;
    u2_ecg2 = gpEcgDebug->ecgVppResult.VppECG2;
    u2_ecg3 = gpEcgDebug->ecgVppResult.VppECG3;
    udprintf("WF_CTRL_0P5Hz_1Vpp_SIN Result:\r\n");
    udprintf("u2_ecg1 = %d uV\r\n",u2_ecg1);
    udprintf("u2_ecg2 = %d uV\r\n",u2_ecg2);
    udprintf("u2_ecg3 = %d uV\r\n",u2_ecg3);

    
    //S2:Set Waveform Device
    if (WavefromCtrl(WF_CTRL_150Hz_1Vpp_SIN, NULL) < 0)
    {
        ERROR("testEcgAmplitudeBand WavefromCtrl!!!\r\n");
        return -1;
    }
    
    //S3:delay
    vTaskDelay(ECG_AMP_DELAY_MS/portTICK_PERIOD_MS);
    
    //S4:Start AIO Factory
    if (AioEcgDebugCtrl(ECG_DEB_CID_START_VPP, NULL) < 0)
    {
        ERROR("testEcgAmplitudeBand AioEcgDebugCtrl!!!\r\n");
        return -1;
    }
    
    //S5:delay for 2 cycle
    WavefromDelay(WAVEFORM_DELAY_150Hz_SIN, 2);
    
    //S6:Stop and get AIO result
    if (AioEcgDebugCtrl(ECG_DEB_CID_STOP_VPP, NULL) < 0)
    {
        ERROR("testEcgAmplitudeBand AioEcgDebugCtrl!!!\r\n");
        return -1;
    }
    
    u3_ecg1 = gpEcgDebug->ecgVppResult.VppECG1;
    u3_ecg2 = gpEcgDebug->ecgVppResult.VppECG2;
    u3_ecg3 = gpEcgDebug->ecgVppResult.VppECG3;
    udprintf("WF_CTRL_150Hz_1Vpp_SIN Result:\r\n");
    udprintf("u3_ecg1 = %d uV\r\n",u3_ecg1);
    udprintf("u3_ecg2 = %d uV\r\n",u3_ecg2);
    udprintf("u3_ecg3 = %d uV\r\n",u3_ecg3);
    
    ret |= checkEcgAmplitudeU1(u1_ecg1, u1_ecg2, u1_ecg3);
    ret |= checkEcgAmplitudeU2(u2_ecg1, u2_ecg2, u2_ecg3);
    ret |= checkEcgAmplitudeU3(u3_ecg1, u3_ecg2, u3_ecg3);
    return ret;
}

static int waitAndCheckProbeInfo(CHECK_ECG_PROBE type)
{
    EventBits_t uxBits = 0;
    u8 alarm = 0x01;
    u16 info;
    AioDspProtocolPkt pkt;
    int i = 0;
    
    xEventGroupClearBits(xDspPktAckEventGroup, DSP_PKT_ACK_BIT_ECG_PROBE);
    uxBits = xEventGroupWaitBits(
            xDspPktAckEventGroup,   // The event group being tested.
            DSP_PKT_ACK_BIT_ECG_PROBE,    // The bits within the event group to wait for.
            pdTRUE,                     // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
            pdFALSE,                    // Don't wait for both bits, either bit will do.
            10000 / portTICK_PERIOD_MS );// Wait a maximum of for either bit to be set.
    if (uxBits & DSP_PKT_ACK_BIT_ECG_PROBE)
    {
        info = gpDspAckResult->u16EcgProbeInfo;
        udprintf("u16EcgProbeInfo = 0x%04x\r\n",info);
        info = info & PROBE_INFO_MASK_ALL;
        switch(type)
        {
        case CHECK_LL_OFF:{
            if (info & PROBE_INFO_MASK_LL) alarm = 0;
        }break;
        case CHECK_LA_OFF:{
            if (info & PROBE_INFO_MASK_LA) alarm = 0;
        }break;
        case CHECK_RA_OFF:{
            if (info & PROBE_INFO_MASK_RA) alarm = 0;
        }break;
        case CHECK_RL_OFF:{
            if (info & PROBE_INFO_MASK_RL) alarm = 0;
        }break;
        case CHECK_V_OFF:{
            if (info & PROBE_INFO_MASK_V1) alarm = 0;
        }break;
        case CHECK_LL_LA_OFF:{
            if ((info & PROBE_INFO_MASK_LL) && (info & PROBE_INFO_MASK_LA))
            {
                alarm = 0;
            }
        }break;
        case CHECK_LA_RA_OFF:{
            if ((info & PROBE_INFO_MASK_LA) && (info & PROBE_INFO_MASK_RA))
            {
                alarm = 0;
            }
        }break;
        case CHECK_LL_RA_OFF:{
            if ((info & PROBE_INFO_MASK_LL) && (info & PROBE_INFO_MASK_RA))
            {
                alarm = 0;
            }
        }break;
        case CHECK_LL_LA_RA_OFF:{
            if ((info & PROBE_INFO_MASK_LL) \
                && (info & PROBE_INFO_MASK_RA) \
                && (info & PROBE_INFO_MASK_LA))
            {
                alarm = 0;
            }
        }break;
        case CHECK_ALL_OFF:{
            if (info & PROBE_INFO_MASK_RL)
            {
                alarm = 0;
            }
        }break;
        default:{

        }break;
        } // End of switch(type)
    } //End of if (uxBits & DSP_PKT_ACK_BIT_ECG_PROBE)
    else
    {
        udprintf("u16EcgProbeInfo timeout\r\n");
    }
    
    if (alarm)
    {
        i = 0;
        initComputerPkt(&pkt);
        pkt.DataAndCRC[i++] = (u8)COMP_ID_ERROR_INFO;
        pkt.DataAndCRC[i++] = (u8)ERR_INFO_ID_ECG_PROBE;
        pkt.DataAndCRC[i++] = (u8)type;
        pkt.Length = i;
        pkt.DataAndCRC[pkt.Length] = crc8ComputerPkt(&pkt);
        sendComputerPkt(&pkt);
        return -1;
    }
    return 0;
}

int testEcgProbeOff(void)
{
    int error = 0;
    
    //S1:ECG switch to EcgOut
    EcgDevCtrl(CMD_ECG_ALL_SEL_ECGOUT, CMD_VAL_UNVALID);
    EcgDevCtrl(CMD_ECG_ALL_OFF, CMD_VAL_OFF_NORMAL);
    
    //S2:Set Waveform Device
    if (WavefromCtrl(WF_CTRL_2Hz_16Vpp_SIN, NULL) < 0)
    {
        ERROR("testEcgProbeOff WavefromCtrl!!!\r\n");
        return -1;
    }
    
    //S3:delay
    vTaskDelay(ECG_PROBE_OFF_DELAY_MS/portTICK_PERIOD_MS);
//while(1){

    EcgDevCtrl(CMD_ECG_LL_OFF, CMD_VAL_OFF_LEADOFF);
    vTaskDelay(ECG_PROBE_OFF_DELAY_MS/portTICK_PERIOD_MS);
    if (waitAndCheckProbeInfo(CHECK_LL_OFF) < 0)
    {
        //ERROR("testEcgProbeOff CHECK_LL_OFF!!!\r\n");
        error++;
    }
    EcgDevCtrl(CMD_ECG_LL_OFF, CMD_VAL_OFF_NORMAL);
    vTaskDelay(ECG_PROBE_OFF_DELAY_MS/portTICK_PERIOD_MS);

//vTaskDelay(ECG_PROBE_OFF_DELAY_MS/portTICK_PERIOD_MS);
//if (waitAndCheckProbeInfo(CHECK_LL_OFF) < 0)
//{
//ERROR("testEcgProbeOff CMD_VAL_OFF_NORMAL End!!!\r\n");
//error++;
//}
//}
    EcgDevCtrl(CMD_ECG_LA_OFF, CMD_VAL_OFF_LEADOFF);
    vTaskDelay(ECG_PROBE_OFF_DELAY_MS/portTICK_PERIOD_MS);
    if (waitAndCheckProbeInfo(CHECK_LA_OFF) < 0)
    {
        ERROR("testEcgProbeOff CHECK_LA_OFF!!!\r\n");
        error++;
    }
    EcgDevCtrl(CMD_ECG_LA_OFF, CMD_VAL_OFF_NORMAL);
    vTaskDelay(ECG_PROBE_OFF_DELAY_MS/portTICK_PERIOD_MS);

    EcgDevCtrl(CMD_ECG_RA_OFF, CMD_VAL_OFF_LEADOFF);
    vTaskDelay(ECG_PROBE_OFF_DELAY_MS/portTICK_PERIOD_MS);
    if (waitAndCheckProbeInfo(CHECK_RA_OFF) < 0)
    {
        ERROR("testEcgProbeOff CHECK_RA_OFF!!!\r\n");
        error++;
    }
    EcgDevCtrl(CMD_ECG_RA_OFF, CMD_VAL_OFF_NORMAL);
    vTaskDelay(ECG_PROBE_OFF_DELAY_MS/portTICK_PERIOD_MS);

    EcgDevCtrl(CMD_ECG_RL_OFF, CMD_VAL_OFF_LEADOFF);
    vTaskDelay(ECG_PROBE_OFF_DELAY_MS/portTICK_PERIOD_MS);
    if (waitAndCheckProbeInfo(CHECK_RL_OFF) < 0)
    {
        ERROR("testEcgProbeOff CHECK_RL_OFF!!!\r\n");
        error++;
    }
    EcgDevCtrl(CMD_ECG_RL_OFF, CMD_VAL_OFF_NORMAL);
    vTaskDelay(ECG_PROBE_OFF_DELAY_MS/portTICK_PERIOD_MS);

    EcgDevCtrl(CMD_ECG_V_OFF, CMD_VAL_OFF_LEADOFF);
    vTaskDelay(ECG_PROBE_OFF_DELAY_MS/portTICK_PERIOD_MS);
    if (waitAndCheckProbeInfo(CHECK_V_OFF) < 0)
    {
        ERROR("testEcgProbeOff CHECK_V_OFF!!!\r\n");
        error++;
    }
    EcgDevCtrl(CMD_ECG_V_OFF, CMD_VAL_OFF_NORMAL);
    vTaskDelay(ECG_PROBE_OFF_DELAY_MS/portTICK_PERIOD_MS);
    
    EcgDevCtrl(CMD_ECG_LL_LA_OFF, CMD_VAL_OFF_LEADOFF);
    vTaskDelay(ECG_PROBE_OFF_DELAY_MS/portTICK_PERIOD_MS);
    if (waitAndCheckProbeInfo(CHECK_LL_LA_OFF) < 0)
    {
        ERROR("testEcgProbeOff CHECK_LL_LA_OFF!!!\r\n");
        error++;
    }
    EcgDevCtrl(CMD_ECG_LL_LA_OFF, CMD_VAL_OFF_NORMAL);
    vTaskDelay(ECG_PROBE_OFF_DELAY_MS/portTICK_PERIOD_MS);
    
    EcgDevCtrl(CMD_ECG_LA_RA_OFF, CMD_VAL_OFF_LEADOFF);
    vTaskDelay(ECG_PROBE_OFF_DELAY_MS/portTICK_PERIOD_MS);
    if (waitAndCheckProbeInfo(CHECK_LA_RA_OFF) < 0)
    {
        ERROR("testEcgProbeOff CHECK_LA_RA_OFF!!!\r\n");
        error++;
    }
    EcgDevCtrl(CMD_ECG_LA_RA_OFF, CMD_VAL_OFF_NORMAL);
    vTaskDelay(ECG_PROBE_OFF_DELAY_MS/portTICK_PERIOD_MS);

    EcgDevCtrl(CMD_ECG_LL_RA_OFF, CMD_VAL_OFF_LEADOFF);
    vTaskDelay(ECG_PROBE_OFF_DELAY_MS/portTICK_PERIOD_MS);
    if (waitAndCheckProbeInfo(CHECK_LL_RA_OFF) < 0)
    {
        ERROR("testEcgProbeOff CHECK_LL_RA_OFF!!!\r\n");
        error++;
    }
    EcgDevCtrl(CMD_ECG_LL_RA_OFF, CMD_VAL_OFF_NORMAL);
    vTaskDelay(ECG_PROBE_OFF_DELAY_MS/portTICK_PERIOD_MS);

    EcgDevCtrl(CMD_ECG_LL_LA_RA_OFF, CMD_VAL_OFF_LEADOFF);
    vTaskDelay(ECG_PROBE_OFF_DELAY_MS/portTICK_PERIOD_MS);
    if (waitAndCheckProbeInfo(CHECK_LL_LA_RA_OFF) < 0)
    {
        ERROR("testEcgProbeOff CHECK_LL_LA_RA_OFF!!!\r\n");
        error++;
    }
    EcgDevCtrl(CMD_ECG_LL_LA_RA_OFF, CMD_VAL_OFF_NORMAL);
    vTaskDelay(ECG_PROBE_OFF_DELAY_MS/portTICK_PERIOD_MS);

    EcgDevCtrl(CMD_ECG_ALL_OFF, CMD_VAL_OFF_LEADOFF);
    vTaskDelay(ECG_PROBE_OFF_DELAY_MS/portTICK_PERIOD_MS);
    if (waitAndCheckProbeInfo(CHECK_ALL_OFF) < 0)
    {
        ERROR("testEcgProbeOff CHECK_ALL_OFF!!!\r\n");
        error++;
    }
    EcgDevCtrl(CMD_ECG_ALL_OFF, CMD_VAL_OFF_NORMAL);
    vTaskDelay(ECG_PROBE_OFF_DELAY_MS/portTICK_PERIOD_MS);

    if (error) return -1;
    else return 0;
}


static uint16_t convPolarityVolt2Dac(int volt_mV)
{
    int volt = 2500;//2.5V REF
    volt += volt_mV;
    return Dac8568mV2Dac((uint16_t)volt);
}

static int waitAndCheckEcgOverLoad(void)
{
    EventBits_t uxBits = 0;
    
    xEventGroupClearBits(xDspPktAckEventGroup, DSP_PKT_ACK_BIT_ECG_PROBE);
    uxBits = xEventGroupWaitBits(
            xDspPktAckEventGroup,   // The event group being tested.
            DSP_PKT_ACK_BIT_ECG_PROBE,    // The bits within the event group to wait for.
            pdTRUE,                     // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
            pdFALSE,                    // Don't wait for both bits, either bit will do.
            1000 / portTICK_PERIOD_MS );// Wait a maximum of for either bit to be set.
    if (uxBits & DSP_PKT_ACK_BIT_ECG_PROBE)
    {
        if(gpDspAckResult->u16EcgProbeInfo & PROBE_INFO_MASK_RL)
        {
            return 1;
        }
        else
        {
            return 0;
        }
        
    } //End of if (uxBits & DSP_PKT_ACK_BIT_ECG_PROBE)
    
    return -1;
}

static int checkPolarityByVolt(int volt_mV, const int alarm)
{
    int ret = 0;
    int err_cnt = 0;
    OpDacRegister_TypeDef OpDacReg;
    
    OpDacReg.channel = DAC_CH_ALL;
    OpDacReg.val = convPolarityVolt2Dac(volt_mV);
    INFO("DAC val =%d\r\n",OpDacReg.val);
    Dac8568Ctrl(DAC_CTRL_W_IN_UP_RES, &OpDacReg);

    EcgDevCtrl(CMD_ECG_LL_SEL, CMD_VAL_SEL_CH2);
    EcgDevCtrl(CMD_ECG_RA_SEL, CMD_VAL_SEL_CH0);
    EcgDevCtrl(CMD_ECG_LA_SEL, CMD_VAL_SEL_CH0);
    EcgDevCtrl(CMD_ECG_RL_SEL, CMD_VAL_SEL_CH0);
    EcgDevCtrl(CMD_ECG_V_SEL, CMD_VAL_SEL_CH0);
    vTaskDelay(ECG_POLARITY_DELAY_MS/portTICK_PERIOD_MS);
    ret = waitAndCheckEcgOverLoad();
    if (alarm != ret)
    {
        ERROR("CMD_ECG_LL_SEL %d\r\n",ret);
        err_cnt++;
    }
    
    EcgDevCtrl(CMD_ECG_RA_SEL, CMD_VAL_SEL_CH2);
    EcgDevCtrl(CMD_ECG_LL_SEL, CMD_VAL_SEL_CH0);
    EcgDevCtrl(CMD_ECG_LA_SEL, CMD_VAL_SEL_CH0);
    EcgDevCtrl(CMD_ECG_RL_SEL, CMD_VAL_SEL_CH0);
    EcgDevCtrl(CMD_ECG_V_SEL, CMD_VAL_SEL_CH0);
    vTaskDelay(ECG_POLARITY_DELAY_MS/portTICK_PERIOD_MS);
    ret = waitAndCheckEcgOverLoad();
    if (alarm != ret)
    {
        ERROR("CMD_ECG_RA_SEL %d\r\n",ret);
        err_cnt++;
    }
    
    EcgDevCtrl(CMD_ECG_LA_SEL, CMD_VAL_SEL_CH2);
    EcgDevCtrl(CMD_ECG_RA_SEL, CMD_VAL_SEL_CH0);
    EcgDevCtrl(CMD_ECG_LL_SEL, CMD_VAL_SEL_CH0);
    EcgDevCtrl(CMD_ECG_RL_SEL, CMD_VAL_SEL_CH0);
    EcgDevCtrl(CMD_ECG_V_SEL, CMD_VAL_SEL_CH0);
    vTaskDelay(ECG_POLARITY_DELAY_MS/portTICK_PERIOD_MS);
    ret = waitAndCheckEcgOverLoad();
    if (alarm != ret)
    {
        ERROR("CMD_ECG_LA_SEL %d\r\n",ret);
        err_cnt++;
    }
    
//    EcgDevCtrl(CMD_ECG_RL_SEL, CMD_VAL_SEL_CH2);
//    EcgDevCtrl(CMD_ECG_RA_SEL, CMD_VAL_SEL_CH0);
//    EcgDevCtrl(CMD_ECG_LA_SEL, CMD_VAL_SEL_CH0);
//    EcgDevCtrl(CMD_ECG_LL_SEL, CMD_VAL_SEL_CH0);
//    EcgDevCtrl(CMD_ECG_V_SEL, CMD_VAL_SEL_CH0);
//    vTaskDelay(ECG_POLARITY_DELAY_MS/portTICK_PERIOD_MS);
//    ret = waitAndCheckEcgOverLoad();
//    if (alarm != ret)
//    {
//        ERROR("CMD_ECG_RL_SEL %d\r\n",ret);
//        err_cnt++;
//    }
    
    EcgDevCtrl(CMD_ECG_V_SEL, CMD_VAL_SEL_CH2);
    EcgDevCtrl(CMD_ECG_RA_SEL, CMD_VAL_SEL_CH0);
    EcgDevCtrl(CMD_ECG_LA_SEL, CMD_VAL_SEL_CH0);
    EcgDevCtrl(CMD_ECG_RL_SEL, CMD_VAL_SEL_CH0);
    EcgDevCtrl(CMD_ECG_LL_SEL, CMD_VAL_SEL_CH0);
    vTaskDelay(ECG_POLARITY_DELAY_MS/portTICK_PERIOD_MS);
    ret = waitAndCheckEcgOverLoad();
    if (alarm != ret)
    {
        ERROR("CMD_ECG_V_SEL %d\r\n",ret);
        err_cnt++;
    }

    if (err_cnt > 0) return -1;
    else return 0;
}

int testEcgPolarity(void)
{
    int error = 0;
    
    //S1:ECG switch to EcgOut
    EcgDevCtrl(CMD_ECG_ALL_SEL_ECGOUT, CMD_VAL_UNVALID);
    
    //S2:Set Waveform Device
    if (WavefromCtrl(WF_CTRL_2Hz_16mVpp_SIN, NULL) < 0)
    {
        ERROR("testEcgPolarity WavefromCtrl!!!\r\n");
        return -1;
    }
    
    //S3:delay
    vTaskDelay(ECG_POLARITY_DELAY_MS/portTICK_PERIOD_MS);

    if (checkPolarityByVolt(516, 0) < 0)
    {
        ERROR("testEcgPolarity checkPositive516mVPolarity!!!\r\n");
        error++;
    }
    else
    {
        INFO("testEcgPolarity +516mV OK\r\n");
    }
    
    if (checkPolarityByVolt(-516, 0) < 0)
    {
        ERROR("testEcgPolarity checkNegative516mVPolarity!!!\r\n");
        error++;
    }
    else
    {
        INFO("testEcgPolarity -516mV OK\r\n");
    }

    if (checkPolarityByVolt(566, 1) < 0)
    {
        ERROR("testEcgPolarity checkPositive566mVPolarity!!!\r\n");
        error++;
    }
    else
    {
        INFO("testEcgPolarity +566mV OK\r\n");
    }
    
    if (checkPolarityByVolt(-566, 1) < 0)
    {
        ERROR("testEcgPolarity checkNegative566mVPolarity!!!\r\n");
        error++;
    }
    else
    {
        INFO("testEcgPolarity -566mV OK\r\n");
    }
    
    if (error) return -1;
    else return 0;
}


static int setPaceChannelAndSW(const PACE_CHANNEL channel, const u8 flag)
{
    u8 pBuf[1] = {0,};
    EventBits_t uxBits = 0;

    pBuf[0] = (u8)channel;
    xEventGroupClearBits(xDspPktAckEventGroup, DSP_PKT_ACK_BIT_PACE_CH);
    sendAioDspPktByID(AIO_ECG_PACE_CHANNEL_ID, (char *)pBuf, 1, 0);
    
    uxBits = xEventGroupWaitBits(
            xDspPktAckEventGroup,   // The event group being tested.
            DSP_PKT_ACK_BIT_PACE_CH,    // The bits within the event group to wait for.
            pdTRUE,                     // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
            pdFALSE,                    // Don't wait for both bits, either bit will do.
            1000 / portTICK_PERIOD_MS );// Wait a maximum of for either bit to be set.
    if ((EventBits_t)0x0 == (uxBits & DSP_PKT_ACK_BIT_PACE_CH))
    {
        ERROR("setPaceChannelAndSW PACE_CH timeout!!\r\n");
        return -1;
    }
    
    pBuf[0] = (u8)flag;
    xEventGroupClearBits(xDspPktAckEventGroup, DSP_PKT_ACK_BIT_PACE_SW);
    sendAioDspPktByID(AIO_ECG_PACE_SW_ID, (char *)pBuf, 1, 0);
    
    uxBits = xEventGroupWaitBits(
            xDspPktAckEventGroup,   // The event group being tested.
            DSP_PKT_ACK_BIT_PACE_SW,    // The bits within the event group to wait for.
            pdTRUE,                     // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
            pdFALSE,                    // Don't wait for both bits, either bit will do.
            1000 / portTICK_PERIOD_MS );// Wait a maximum of for either bit to be set.
    if ((EventBits_t)0x0 == (uxBits & DSP_PKT_ACK_BIT_PACE_SW))
    {
        ERROR("setPaceChannelAndSW PACE_SW timeout!!\r\n");
        return -1;
    }

    return 0;
}

static int getAioPaceCountAt5s(void)
{
    int count = 0;
    //60bpm, test 5s result 4~5 paces
    //Start AIO Factory
    if (AioEcgDebugCtrl(ECG_DEB_CID_START_PACE_CNT, NULL) < 0)
    {
        ERROR("getAioPaceCountAt5s AioEcgDebugCtrl!!!\r\n");
        return -1;
    }
    
    vTaskDelay(5000/portTICK_PERIOD_MS); //delay for 5 s
    
    //S6:Stop and get AIO result
    if (AioEcgDebugCtrl(ECG_DEB_CID_STOP_PACE_CNT, NULL) < 0)
    {
        ERROR("getAioPaceCountAt5s AioEcgDebugCtrl!!!\r\n");
        return -1;
    }

    count = gpEcgDebug->u8PaceCount;
    INFO("getAioPaceCountAt5s count =%d\r\n",count);
    return  count;
}

static int checkPaceA2PaceH(void)
{
    int error_cnt = 0;
    
    if (WavefromCtrl(WF_CTRL_PACE_A, NULL) < 0)
    {
        ERROR("checkECG1Pace WF_CTRL_PACE_A!!!\r\n");
        return -1;
    }
    else
    {
        INFO("WF_CTRL_PACE_A\r\n");
    }
    vTaskDelay(WAIT_FOR_WAVEFORM_STABLE_MS/portTICK_PERIOD_MS);
    if (getAioPaceCountAt5s() < 4) //60bpm(1pcs), 5s must >= 4pcs
    {
        ERROR("checkECG1Pace getAioPaceCountAt5s!!!\r\n");
        error_cnt++;
    }
    
    if (WavefromCtrl(WF_CTRL_PACE_B, NULL) < 0)
    {
        ERROR("checkECG1Pace WF_CTRL_PACE_B!!!\r\n");
        return -1;
    }
    else
    {
        INFO("WF_CTRL_PACE_B\r\n");
    }
    vTaskDelay(WAIT_FOR_WAVEFORM_STABLE_MS/portTICK_PERIOD_MS);
    if (getAioPaceCountAt5s() < 4) //60bpm(1pcs), 5s must >= 4pcs
    {
        ERROR("checkECG1Pace getAioPaceCountAt5s!!!\r\n");
        error_cnt++;
    }
    
    if (WavefromCtrl(WF_CTRL_PACE_C, NULL) < 0)
    {
        ERROR("checkECG1Pace WF_CTRL_PACE_C!!!\r\n");
        return -1;
    }
    else
    {
        INFO("WF_CTRL_PACE_C\r\n");
    }
    vTaskDelay(WAIT_FOR_WAVEFORM_STABLE_MS/portTICK_PERIOD_MS); //wait for waveform stable
    if (getAioPaceCountAt5s() < 4) //60bpm(1pcs), 5s must >= 4pcs
    {
        ERROR("checkECG1Pace getAioPaceCountAt5s!!!\r\n");
        error_cnt++;
    }
    
    if (WavefromCtrl(WF_CTRL_PACE_D, NULL) < 0)
    {
        ERROR("checkECG1Pace WF_CTRL_PACE_D!!!\r\n");
        return -1;
    }
    else
    {
        INFO("WF_CTRL_PACE_D\r\n");
    }
    vTaskDelay(WAIT_FOR_WAVEFORM_STABLE_MS/portTICK_PERIOD_MS); //wait for waveform stable
    if (getAioPaceCountAt5s() < 4) //60bpm(1pcs), 5s must >= 4pcs
    {
        ERROR("checkECG1Pace getAioPaceCountAt5s!!!\r\n");
        error_cnt++;
    }
    
    if (WavefromCtrl(WF_CTRL_PACE_E, NULL) < 0)
    {
        ERROR("checkECG1Pace WF_CTRL_PACE_E!!!\r\n");
        return -1;
    }
    else
    {
        INFO("WF_CTRL_PACE_E\r\n");
    }
    vTaskDelay(WAIT_FOR_WAVEFORM_STABLE_MS/portTICK_PERIOD_MS); //wait for waveform stable
    if (getAioPaceCountAt5s() < 4) //60bpm(1pcs), 5s must >= 4pcs
    {
        ERROR("checkECG1Pace getAioPaceCountAt5s!!!\r\n");
        error_cnt++;
    }
    
    if (WavefromCtrl(WF_CTRL_PACE_F, NULL) < 0)
    {
        ERROR("checkECG1Pace WF_CTRL_PACE_F!!!\r\n");
        return -1;
    }
    else
    {
        INFO("WF_CTRL_PACE_F\r\n");
    }
    vTaskDelay(WAIT_FOR_WAVEFORM_STABLE_MS/portTICK_PERIOD_MS); //wait for waveform stable
    if (getAioPaceCountAt5s() < 4) //60bpm(1pcs), 5s must >= 4pcs
    {
        ERROR("checkECG1Pace getAioPaceCountAt5s!!!\r\n");
        error_cnt++;
    }
    
    if (WavefromCtrl(WF_CTRL_PACE_G, NULL) < 0)
    {
        ERROR("checkECG1Pace WF_CTRL_PACE_G!!!\r\n");
        return -1;
    }
    else
    {
        INFO("WF_CTRL_PACE_G\r\n");
    }
    vTaskDelay(WAIT_FOR_WAVEFORM_STABLE_MS/portTICK_PERIOD_MS); //wait for waveform stable
    if (getAioPaceCountAt5s() < 4) //60bpm(1pcs), 5s must >= 4pcs
    {
        ERROR("checkECG1Pace getAioPaceCountAt5s!!!\r\n");
        error_cnt++;
    }
    
    if (WavefromCtrl(WF_CTRL_PACE_H, NULL) < 0)
    {
        ERROR("checkECG1Pace WF_CTRL_PACE_H!!!\r\n");
        return -1;
    }
    else
    {
        INFO("WF_CTRL_PACE_H\r\n");
    }
    vTaskDelay(WAIT_FOR_WAVEFORM_STABLE_MS/portTICK_PERIOD_MS); //wait for waveform stable
    if (getAioPaceCountAt5s() < 4) //60bpm(1pcs), 5s must >= 4pcs
    {
        ERROR("checkECG1Pace getAioPaceCountAt5s!!!\r\n");
        error_cnt++;
    }
    
    if (error_cnt) return -1;
    else return 0;
}

static int checkECG1Pace(void)
{
    setPaceChannelAndSW(PACE_RA_LA_I, U8_FLAG_TRUE);
    EcgDevCtrl(CMD_ECG_RA_SEL, CMD_VAL_SEL_CH1);
    EcgDevCtrl(CMD_ECG_LA_SEL, CMD_VAL_SEL_CH1);
    EcgDevCtrl(CMD_ECG_RL_SEL, CMD_VAL_SEL_CH0);
    EcgDevCtrl(CMD_ECG_LL_SEL, CMD_VAL_SEL_CH0);
    EcgDevCtrl(CMD_ECG_V_SEL, CMD_VAL_SEL_CH0);
    
    return checkPaceA2PaceH();
}

static int checkECG2Pace(void)
{
    setPaceChannelAndSW(PACE_RA_LL_II, U8_FLAG_TRUE);
    EcgDevCtrl(CMD_ECG_RA_SEL, CMD_VAL_SEL_CH1);
    EcgDevCtrl(CMD_ECG_LL_SEL, CMD_VAL_SEL_CH1);
    EcgDevCtrl(CMD_ECG_RL_SEL, CMD_VAL_SEL_CH0);
    EcgDevCtrl(CMD_ECG_LA_SEL, CMD_VAL_SEL_CH0);
    EcgDevCtrl(CMD_ECG_V_SEL, CMD_VAL_SEL_CH0);
    
    return checkPaceA2PaceH();
}

static int checkECG3Pace(void)
{
    setPaceChannelAndSW(PACE_V, U8_FLAG_TRUE);
    EcgDevCtrl(CMD_ECG_V_SEL, CMD_VAL_SEL_CH1);
    EcgDevCtrl(CMD_ECG_LA_SEL, CMD_VAL_SEL_CH0);
    EcgDevCtrl(CMD_ECG_RL_SEL, CMD_VAL_SEL_CH0);
    EcgDevCtrl(CMD_ECG_LL_SEL, CMD_VAL_SEL_CH0);
    EcgDevCtrl(CMD_ECG_RA_SEL, CMD_VAL_SEL_CH0);
    
    return checkPaceA2PaceH();
}


int testEcgPace(void)
{
    int err_cnt = 0;

    if (checkECG1Pace() < 0)
    {
        err_cnt++;
        ERROR("testEcgPace checkECG1Pace!!!\r\n");
    }

    if (checkECG2Pace() < 0)
    {
        err_cnt++;
        ERROR("testEcgPace checkECG2Pace!!!\r\n");
    }
    
    if (checkECG3Pace() < 0)
    {
        err_cnt++;
        ERROR("testEcgPace checkECG3Pace!!!\r\n");
    }

    //restore default
    setPaceChannelAndSW(PACE_RA_LL_II, U8_FLAG_FALT);
    EcgDevCtrl(CMD_ECG_ALL_SEL_ECGOUT, CMD_VAL_UNVALID);
    
    if (err_cnt) return -1;
    else return 0;
}
         
static int checkQRSTempA(void)
{
    int ret = 0;
    int err_cnt = 0;
    EventBits_t uxBits = 0;
    
    if (WavefromCtrl(WF_CTRL_QRS_A, NULL) < 0)
    {
        ERROR("checkQRSTempA WF_CTRL_QRS_A!!!\r\n");
        return -1;
    }
    vTaskDelay(WAIT_FOR_WAVEFORM_STABLE_MS/portTICK_PERIOD_MS); //wait for waveform stable

    INFO("Waiting for QRS result!!! 10s...\r\n");
    vTaskDelay(10000/portTICK_PERIOD_MS); //wait for waveform stable

    ret = getAioPaceCountAt5s();
    if ((ret < 0)||(ret > 6)) //30bpm, 5s must >= 2.5pcs
    {
        ERROR("checkECG1Pace getAioPaceCountAt5s!!!\r\n");
        err_cnt++;
    }

    xEventGroupClearBits(xDspPktAckEventGroup, DSP_PKT_ACK_BIT_HR_RR);
    uxBits = xEventGroupWaitBits(
            xDspPktAckEventGroup,       // The event group being tested.
            DSP_PKT_ACK_BIT_HR_RR,      // The bits within the event group to wait for.
            pdTRUE,                     // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
            pdFALSE,                    // Don't wait for both bits, either bit will do.
            1000 / portTICK_PERIOD_MS );// Wait a maximum of for either bit to be set.
    if (uxBits & DSP_PKT_ACK_BIT_HR_RR)
    {
        INFO("checkECG1Pace HR=%d!!!\r\n", gpDspAckResult->u16HR);
        if ((gpDspAckResult->u16HR < 115) || (gpDspAckResult->u16HR > 125)) //120bpm
        {
            err_cnt++;
        }
    }
    else
    {
        ERROR("checkECG1Pace HR_RR timeout!!!\r\n");
        err_cnt++;
    }

    if (err_cnt) return -1;
    else return 0;
}

static int setAndWaitPatientType(const PATIENT_TypeDef type)
{
    u8 pBuf[1] = {0,};
    EventBits_t uxBits = 0;

    pBuf[0] = (u8)type;
    xEventGroupClearBits(xDspPktAckEventGroup, DSP_PKT_ACK_BIT_PATIENT);
    sendAioDspPktByID(COM_PATIENT_TYPE_ID, (char *)pBuf, 1, 0);
    
    uxBits = xEventGroupWaitBits(
            xDspPktAckEventGroup,   // The event group being tested.
            DSP_PKT_ACK_BIT_PATIENT,    // The bits within the event group to wait for.
            pdTRUE,                     // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
            pdFALSE,                    // Don't wait for both bits, either bit will do.
            1000 / portTICK_PERIOD_MS );// Wait a maximum of for either bit to be set.
    if ((EventBits_t)0x0 == (uxBits & DSP_PKT_ACK_BIT_PATIENT))
    {
        ERROR("setAndWaitPatientType timeout!!\r\n");
        return -1;
    }

    INFO("setAndWaitPatientType OK!!\r\n");
    return 0;
}

int testEcgQuickQRS(void)
{
    int err_cnt = 0;
    
    setPaceChannelAndSW(PACE_RA_LL_II, U8_FLAG_TRUE);
    EcgDevCtrl(CMD_ECG_RA_SEL, CMD_VAL_SEL_CH1);
    EcgDevCtrl(CMD_ECG_LL_SEL, CMD_VAL_SEL_CH1);
    EcgDevCtrl(CMD_ECG_RL_SEL, CMD_VAL_SEL_CH0);
    EcgDevCtrl(CMD_ECG_LA_SEL, CMD_VAL_SEL_CH0);
    EcgDevCtrl(CMD_ECG_V_SEL, CMD_VAL_SEL_CH0);

    if (setAndWaitPatientType(NEWBORN) < 0)
    {
        ERROR("setAndWaitPatientType NEWBORN!!!\r\n");
        err_cnt++;
    }
    
    if (checkQRSTempA() < 0)
    {
        ERROR("testEcgQuickQRS checkQRSTempA!!!\r\n");
        err_cnt++;
    }
    
    if (setAndWaitPatientType(ADULT) < 0)
    {
        ERROR("setAndWaitPatientType ADULT!!!\r\n");
        err_cnt++;
    }
    
    //restore default
    setPaceChannelAndSW(PACE_RA_LL_II, U8_FLAG_FALT);
    EcgDevCtrl(CMD_ECG_ALL_SEL_ECGOUT, CMD_VAL_UNVALID);
    
    if (err_cnt) return -1;
    else return 0;
}

