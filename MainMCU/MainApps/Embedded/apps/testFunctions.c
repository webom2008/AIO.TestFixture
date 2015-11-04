/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : testFunctions.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/10/23
  Last Modified :
  Description   : test process interfaces
  Function List :
  History       :
  1.Date        : 2015/10/23
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
static int gAioBoardMaxCurrent = 0;

/*----------------------------------------------*
 * module-wide global variables                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

enum
{
    BOTH_RELE    = 0,
    SLOW_RELE    = 1,
    BOTH_HOLD    = 2,
    FAST_RELE    = 3,
};

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/
#define AIOBOARD_PUMP_ON_CURRENT_MAX        ((int)500) //mA
#define AIOBOARD_PUMP_OFF_CURRENT_MAX       ((int)150) //mA

#define NIBP_DEBUG_CID_PUMP                 (u8)(0x03)
#define NIBP_DEBUG_CID_RELEASE              (u8)(0x04)
#define NIBP_DEBUG_CHECK_DSP_PROTECT        (u8)(0x05)
#define NIBP_DEBUG_CHECK_STM_PROTECT        (u8)(0x06)

#define NIBP_VERIFY_DELAY_MS                100
#define ECG_AMP_DELAY_MS                    2000
#define ECG_PROBE_OFF_DELAY_MS              2000
#define ECG_POLARITY_DELAY_MS               2000


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
#define INFO(fmt, arg...) udprintf("[Test]Info: "fmt, ##arg)
#else
#define INFO(fmt, arg...) do{}while(0)
#endif
#ifdef _ERROR_
#define ERROR(fmt, arg...) udprintf("[Test]Error: "fmt, ##arg)
#else
#define ERROR(fmt, arg...) do{}while(0)
#endif
/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

static int testComputerConnect(void)
{
    AioDspProtocolPkt pkt;
    int i = 0;
    EventBits_t uxBits;
    
    initComputerPkt(&pkt);
    pkt.DataAndCRC[i++] = (u8)COMP_ID_CONNECT_TEST;
    pkt.Length = i;
    pkt.DataAndCRC[pkt.Length] = crc8ComputerPkt(&pkt);

    for (i = 0; i < 3; i++)
    {
        sendComputerPkt(&pkt);
        
        uxBits = xEventGroupWaitBits(
                xCompPktAckEventGroup,      // The event group being tested.
                COMP_PKT_BIT_CONNECTTED,    // The bits within the event group to wait for.
                pdTRUE,                     // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
                pdFALSE,                    // Don't wait for both bits, either bit will do.
                1000 );                 // Wait a maximum of for either bit to be set.
        if (uxBits & COMP_PKT_BIT_CONNECTTED)
        {
            return 0;
        }
    }
    return -1;
}

static int testWaveformConnect(void)
{
    AioDspProtocolPkt pkt;
    int i = 0;
    EventBits_t uxBits;
    
    initComputerPkt(&pkt);
    pkt.DataAndCRC[i++] = (u8)COMP_ID_WAVEFORM_CONNECT;
    pkt.Length = i;
    pkt.DataAndCRC[pkt.Length] = crc8ComputerPkt(&pkt);

    for (i = 0; i < 3; i++)
    {
        sendComputerPkt(&pkt);
        
        uxBits = xEventGroupWaitBits(
                xCompPktAckEventGroup,      // The event group being tested.
                COMP_PKT_BIT_WF_CONNECTTED,    // The bits within the event group to wait for.
                pdTRUE,                     // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
                pdFALSE,                    // Don't wait for both bits, either bit will do.
                1000 );                 // Wait a maximum of for either bit to be set.
        if (uxBits & COMP_PKT_BIT_WF_CONNECTTED)
        {
            return 0;
        }
    }
    return -1;
}

static int testDPM2200Connect(void)
{
    DmaUartProtocolPacket txPacket;
    EventBits_t uxBits;
    
    DmaUartProtocolPacketInit(&txPacket);
    txPacket.ID = (u8)PKT_ID_DPM_UNITS;
    txPacket.ACK = DMA_UART_PACKET_NACK;

    xEventGroupClearBits(xCoopMCUPktAckEventGroup, COOPMCU_PKT_ACK_BIT_DPM_UNITS);
    sendCoopMcuPkt(&txPacket, 0);
    uxBits = xEventGroupWaitBits(
            xCoopMCUPktAckEventGroup,   // The event group being tested.
            COOPMCU_PKT_ACK_BIT_DPM_UNITS,    // The bits within the event group to wait for.
            pdTRUE,                     // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
            pdFALSE,                    // Don't wait for both bits, either bit will do.
            2000 / portTICK_PERIOD_MS );// Wait a maximum of for either bit to be set.
    if (uxBits & COOPMCU_PKT_ACK_BIT_DPM_UNITS)
    {
        return 0;
    }
    ERROR("testDPM2200Connect timeout!!!\r\n");
    return -1;
}

/*****************************************************************************
 Prototype    : testPrepareAllReady
 Description  : test prepare 
 Input        : void  
 Output       : None
 Return Value : <0 -- not ready; >=0 -- ready
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2015/10/23
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
int testPrepareAllReady(void)
{
    AioDspProtocolPkt pkt;
    int i = 0;
    
    gAioBoardMaxCurrent = 0;
    
    if (testComputerConnect() < 0)
    {
        i = 0;
        initComputerPkt(&pkt);
        pkt.DataAndCRC[i++] = (u8)COMP_ID_ERROR_INFO;
        pkt.DataAndCRC[i++] = (u8)ERR_INFO_ID_PC_LOST;
        pkt.Length = i;
        pkt.DataAndCRC[pkt.Length] = crc8ComputerPkt(&pkt);
        sendComputerPkt(&pkt);
        return -1;
    }
#ifndef SKIP_CHECK_DPM2200_CONNECT
    if (testDPM2200Connect() < 0)
    {
        i = 0;
        initComputerPkt(&pkt);
        pkt.DataAndCRC[i++] = (u8)COMP_ID_ERROR_INFO;
        pkt.DataAndCRC[i++] = (u8)ERR_INFO_ID_DPM_LOST;
        pkt.Length = i;
        pkt.DataAndCRC[pkt.Length] = crc8ComputerPkt(&pkt);
        sendComputerPkt(&pkt);
        return -2;
    }
#endif
#ifndef SKIP_CHECK_WAVEFORM_CONNECT
    if (testWaveformConnect() < 0)
    {
        i = 0;
        initComputerPkt(&pkt);
        pkt.DataAndCRC[i++] = (u8)COMP_ID_ERROR_INFO;
        pkt.DataAndCRC[i++] = (u8)ERR_INFO_ID_WAVEFORM_LOST;
        pkt.Length = i;
        pkt.DataAndCRC[pkt.Length] = crc8ComputerPkt(&pkt);
        sendComputerPkt(&pkt);
        return -3;
    }
#endif
    return 0;
}

static int refreshMaxAioBoardCurrent(void)
{
    DmaUartProtocolPacket txPacket;
    EventBits_t uxBits;
    
    DmaUartProtocolPacketInit(&txPacket);
    txPacket.ID = (u8)PKT_ID_TDM_RESULT;
    txPacket.ACK = DMA_UART_PACKET_NACK;
    
    sendCoopMcuPkt(&txPacket, 0);
    
    uxBits = xEventGroupWaitBits(
            xCoopMCUPktAckEventGroup,   // The event group being tested.
            COOPMCU_PKT_ACK_BIT_TDM,    // The bits within the event group to wait for.
            pdTRUE,                     // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
            pdFALSE,                    // Don't wait for both bits, either bit will do.
            2000 / portTICK_PERIOD_MS );// Wait a maximum of for either bit to be set.
    if (uxBits & COOPMCU_PKT_ACK_BIT_TDM)
    {
        if (gpCoopMcuDev->testAioBoardCurrent > gAioBoardMaxCurrent)
        {
            gAioBoardMaxCurrent = gpCoopMcuDev->testAioBoardCurrent;
        }
//        INFO("refreshMaxAioBoardCurrent =%dmA!!!\r\n",gAioBoardMaxCurrent);
        return 0;
    }
    else //timeout
    {
        ERROR("testAIOBaordCurrent timeout!!!\r\n");
        return 1;
    }
}


int testAioBoardMaxCurrent(void)
{
    DmaUartProtocolPacket txPacket;
    int normal_current = 0;
    int i;
    EventBits_t uxBits;
    
    DmaUartProtocolPacketInit(&txPacket);
    txPacket.ID = (u8)PKT_ID_TDM_RESULT;
    txPacket.ACK = DMA_UART_PACKET_NACK;

    for (i = 0; i < 5; i++)
    {
        sendCoopMcuPkt(&txPacket, 1000);
        
        uxBits = xEventGroupWaitBits(
                xCoopMCUPktAckEventGroup,   // The event group being tested.
                COOPMCU_PKT_ACK_BIT_TDM,    // The bits within the event group to wait for.
                pdTRUE,                     // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
                pdFALSE,                    // Don't wait for both bits, either bit will do.
                1000 / portTICK_PERIOD_MS );// Wait a maximum of for either bit to be set.
        if (uxBits & COOPMCU_PKT_ACK_BIT_TDM)
        {
            normal_current += gpCoopMcuDev->testAioBoardCurrent;
        }
        else //timeout
        {
            ERROR("testAioBoardMaxCurrent timeout!!!\r\n");
            return 1;
        }
        vTaskDelay(100);
    }
    
    normal_current = normal_current / 5;

    
    if ((gAioBoardMaxCurrent > AIOBOARD_PUMP_ON_CURRENT_MAX) \
        ||(normal_current > AIOBOARD_PUMP_OFF_CURRENT_MAX))
    {
        ERROR("AIOBaordCurrent gAioBoardMaxCurrent = %dmA,normal_current=%dmA\r\n",
            gAioBoardMaxCurrent,normal_current);
        return 1;
    }
    INFO("AIOBaordCurrent gAioBoardMaxCurrent = %dmA,normal_current=%dmA\r\n",
        gAioBoardMaxCurrent,normal_current);
    return 0;
}

int getEcgSelfcheck(void)
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
    ERROR("getEcgSelfcheck timeout!!!\r\n");
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
            1000 / portTICK_PERIOD_MS );// Wait a maximum of for either bit to be set.
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
            if (info & PROBE_INFO_MASK_RL)
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
    
    //S2:Set Waveform Device
    if (WavefromCtrl(WF_CTRL_2Hz_16Vpp_SIN, NULL) < 0)
    {
        ERROR("testEcgProbeOff WavefromCtrl!!!\r\n");
        return -1;
    }
    
    //S3:delay
    vTaskDelay(ECG_PROBE_OFF_DELAY_MS/portTICK_PERIOD_MS);

    EcgDevCtrl(CMD_ECG_LL_OFF, CMD_VAL_OFF_LEADOFF);
    vTaskDelay(ECG_PROBE_OFF_DELAY_MS/portTICK_PERIOD_MS);
    if (waitAndCheckProbeInfo(CHECK_LL_OFF) < 0)
    {
        ERROR("testEcgProbeOff CHECK_LL_OFF!!!\r\n");
        error++;
    }
    EcgDevCtrl(CMD_ECG_LL_OFF, CMD_VAL_OFF_NORMAL);
    
    EcgDevCtrl(CMD_ECG_LA_OFF, CMD_VAL_OFF_LEADOFF);
    vTaskDelay(ECG_PROBE_OFF_DELAY_MS/portTICK_PERIOD_MS);
    if (waitAndCheckProbeInfo(CHECK_LA_OFF) < 0)
    {
        ERROR("testEcgProbeOff CHECK_LA_OFF!!!\r\n");
        error++;
    }
    EcgDevCtrl(CMD_ECG_LA_OFF, CMD_VAL_OFF_NORMAL);

    EcgDevCtrl(CMD_ECG_RA_OFF, CMD_VAL_OFF_LEADOFF);
    vTaskDelay(ECG_PROBE_OFF_DELAY_MS/portTICK_PERIOD_MS);
    if (waitAndCheckProbeInfo(CHECK_RA_OFF) < 0)
    {
        ERROR("testEcgProbeOff CHECK_RA_OFF!!!\r\n");
        error++;
    }
    EcgDevCtrl(CMD_ECG_RA_OFF, CMD_VAL_OFF_NORMAL);

    EcgDevCtrl(CMD_ECG_RL_OFF, CMD_VAL_OFF_LEADOFF);
    vTaskDelay(ECG_PROBE_OFF_DELAY_MS/portTICK_PERIOD_MS);
    if (waitAndCheckProbeInfo(CHECK_RL_OFF) < 0)
    {
        ERROR("testEcgProbeOff CHECK_RL_OFF!!!\r\n");
        error++;
    }
    EcgDevCtrl(CMD_ECG_RL_OFF, CMD_VAL_OFF_NORMAL);

    EcgDevCtrl(CMD_ECG_V_OFF, CMD_VAL_OFF_LEADOFF);
    vTaskDelay(ECG_PROBE_OFF_DELAY_MS/portTICK_PERIOD_MS);
    if (waitAndCheckProbeInfo(CHECK_V_OFF) < 0)
    {
        ERROR("testEcgProbeOff CHECK_V_OFF!!!\r\n");
        error++;
    }
    EcgDevCtrl(CMD_ECG_V_OFF, CMD_VAL_OFF_NORMAL);
    
    EcgDevCtrl(CMD_ECG_LL_LA_OFF, CMD_VAL_OFF_LEADOFF);
    vTaskDelay(ECG_PROBE_OFF_DELAY_MS/portTICK_PERIOD_MS);
    if (waitAndCheckProbeInfo(CHECK_LL_LA_OFF) < 0)
    {
        ERROR("testEcgProbeOff CHECK_LL_LA_OFF!!!\r\n");
        error++;
    }
    EcgDevCtrl(CMD_ECG_LL_LA_OFF, CMD_VAL_OFF_NORMAL);
    
    EcgDevCtrl(CMD_ECG_LA_RA_OFF, CMD_VAL_OFF_LEADOFF);
    vTaskDelay(ECG_PROBE_OFF_DELAY_MS/portTICK_PERIOD_MS);
    if (waitAndCheckProbeInfo(CHECK_LA_RA_OFF) < 0)
    {
        ERROR("testEcgProbeOff CHECK_LA_RA_OFF!!!\r\n");
        error++;
    }
    EcgDevCtrl(CMD_ECG_LA_RA_OFF, CMD_VAL_OFF_NORMAL);

    EcgDevCtrl(CMD_ECG_LL_RA_OFF, CMD_VAL_OFF_LEADOFF);
    vTaskDelay(ECG_PROBE_OFF_DELAY_MS/portTICK_PERIOD_MS);
    if (waitAndCheckProbeInfo(CHECK_LL_RA_OFF) < 0)
    {
        ERROR("testEcgProbeOff CHECK_LL_RA_OFF!!!\r\n");
        error++;
    }
    EcgDevCtrl(CMD_ECG_LL_RA_OFF, CMD_VAL_OFF_NORMAL);

    EcgDevCtrl(CMD_ECG_LL_LA_RA_OFF, CMD_VAL_OFF_LEADOFF);
    vTaskDelay(ECG_PROBE_OFF_DELAY_MS/portTICK_PERIOD_MS);
    if (waitAndCheckProbeInfo(CHECK_LL_LA_RA_OFF) < 0)
    {
        ERROR("testEcgProbeOff CHECK_LL_LA_RA_OFF!!!\r\n");
        error++;
    }
    EcgDevCtrl(CMD_ECG_LL_LA_RA_OFF, CMD_VAL_OFF_NORMAL);

    EcgDevCtrl(CMD_ECG_ALL_OFF, CMD_VAL_OFF_LEADOFF);
    vTaskDelay(ECG_PROBE_OFF_DELAY_MS/portTICK_PERIOD_MS);
    if (waitAndCheckProbeInfo(CHECK_ALL_OFF) < 0)
    {
        ERROR("testEcgProbeOff CHECK_ALL_OFF!!!\r\n");
        error++;
    }
    EcgDevCtrl(CMD_ECG_ALL_OFF, CMD_VAL_OFF_NORMAL);

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
        ERROR("checkPositive500mVPolarity CMD_ECG_LL_SEL\r\n");
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
        ERROR("checkPositive500mVPolarity CMD_ECG_RA_SEL\r\n");
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
        ERROR("checkPositive500mVPolarity CMD_ECG_LA_SEL\r\n");
        err_cnt++;
    }
    
    EcgDevCtrl(CMD_ECG_RL_SEL, CMD_VAL_SEL_CH2);
    EcgDevCtrl(CMD_ECG_RA_SEL, CMD_VAL_SEL_CH0);
    EcgDevCtrl(CMD_ECG_LA_SEL, CMD_VAL_SEL_CH0);
    EcgDevCtrl(CMD_ECG_LL_SEL, CMD_VAL_SEL_CH0);
    EcgDevCtrl(CMD_ECG_V_SEL, CMD_VAL_SEL_CH0);
    vTaskDelay(ECG_POLARITY_DELAY_MS/portTICK_PERIOD_MS);
    ret = waitAndCheckEcgOverLoad();
    if (alarm != ret)
    {
        ERROR("checkPositive500mVPolarity CMD_ECG_RL_SEL\r\n");
        err_cnt++;
    }
    
    EcgDevCtrl(CMD_ECG_V_SEL, CMD_VAL_SEL_CH2);
    EcgDevCtrl(CMD_ECG_RA_SEL, CMD_VAL_SEL_CH0);
    EcgDevCtrl(CMD_ECG_LA_SEL, CMD_VAL_SEL_CH0);
    EcgDevCtrl(CMD_ECG_RL_SEL, CMD_VAL_SEL_CH0);
    EcgDevCtrl(CMD_ECG_LL_SEL, CMD_VAL_SEL_CH0);
    vTaskDelay(ECG_POLARITY_DELAY_MS/portTICK_PERIOD_MS);
    ret = waitAndCheckEcgOverLoad();
    if (alarm != ret)
    {
        ERROR("checkPositive500mVPolarity CMD_ECG_V_SEL\r\n");
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

    if (checkPolarityByVolt(500, 0) < 0)
    {
        ERROR("testEcgPolarity checkPositive500mVPolarity!!!\r\n");
        error++;
    }
    
    if (checkPolarityByVolt(-500, 0) < 0)
    {
        ERROR("testEcgPolarity checkNegative500mVPolarity!!!\r\n");
        error++;
    }
    
    if (checkPolarityByVolt(540, 1) < 0)
    {
        ERROR("testEcgPolarity checkPositive540mVPolarity!!!\r\n");
        error++;
    }
    
    if (checkPolarityByVolt(-540, 1) < 0)
    {
        ERROR("testEcgPolarity checkNegative540mVPolarity!!!\r\n");
        error++;
    }
    
    if (error) return -1;
    else return 0;
}

int testEcgPace(void)
{
    return 0;
}

int testEcgQuickQRS(void)
{
    return 0;
}

int testRespAmplitudeBand(void)
{
    return 0;
}

int testTempSelfcheck(void)
{
    return 0;
}

int testTempProbeOff(void)
{
    return 0;
}

int testTempPrecision(void)
{
    return 0;
}

int testSpo2Uart(void)
{
    return 0;
}

static int sendAndWaitNibpStop(void)
{
    char buf = 0x01;
    EventBits_t uxBits = 0;

    xEventGroupClearBits(xDspPktAckEventGroup, DSP_PKT_ACK_BIT_NIBP_STOP);
    sendAioDspPktByID(AIO_NIBP_STOP_ID, &buf, 1, 0);
    
    uxBits = xEventGroupWaitBits(
            xDspPktAckEventGroup,   // The event group being tested.
            DSP_PKT_ACK_BIT_NIBP_STOP,    // The bits within the event group to wait for.
            pdTRUE,                     // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
            pdFALSE,                    // Don't wait for both bits, either bit will do.
            1000 / portTICK_PERIOD_MS );// Wait a maximum of for either bit to be set.
    if (uxBits & DSP_PKT_ACK_BIT_NIBP_STOP)
    {
        udprintf("sendAndWaitNibpStop success!!!\r\n");
        return 0;
    }
    udprintf("sendAndWaitNibpStop failed!!!\r\n");
    return -1;
}


int testNibpSelfcheck(void)
{
    return 0;
}


static int enterNibpVerify(void)
{
    int i;
    EventBits_t uxBits = 0;
    u8 val = 0xCC;
    
    for (i = 0; i < 3; i++)
    {
        sendAioDspPktByID(AIO_NIBP_VERIFY_ID, (char *)&val, 1, 0);
        
        uxBits = xEventGroupWaitBits(
                xDspPktAckEventGroup,   // The event group being tested.
                DSP_PKT_ACK_BIT_VERIFY,    // The bits within the event group to wait for.
                pdTRUE,                     // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
                pdFALSE,                    // Don't wait for both bits, either bit will do.
                1000 / portTICK_PERIOD_MS );// Wait a maximum of for either bit to be set.
        if (uxBits & DSP_PKT_ACK_BIT_VERIFY)
        {
            if (val == gpDspAckResult->u8DspAckVerifyVal)
            {
                break;
            }
            else //pkt return unvalied, continue
            {
                continue;
            }
        }
        else //timeout
        {
            continue;
        }
    }
    
    if (3 == i) //Enter verify failed
    {
        return -1;
    }
    return 0;
}

static int exitNibpVerify(void)
{
    int i;
    EventBits_t uxBits = 0;
    u8 val = 0xFF; //Exit Verfiy
    
    for (i = 0; i < 3; i++)
    {
        sendAioDspPktByID(AIO_NIBP_VERIFY_ID, (char *)&val, 1, 0);
        
        uxBits = xEventGroupWaitBits(
                xDspPktAckEventGroup,   // The event group being tested.
                DSP_PKT_ACK_BIT_VERIFY,    // The bits within the event group to wait for.
                pdTRUE,                     // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
                pdFALSE,                    // Don't wait for both bits, either bit will do.
                1000 / portTICK_PERIOD_MS );// Wait a maximum of for either bit to be set.
        if (uxBits & DSP_PKT_ACK_BIT_VERIFY)
        {
            if (val == gpDspAckResult->u8DspAckVerifyVal)
            {
                vTaskDelay(5000 / portTICK_PERIOD_MS);
                break;
            }
            else //pkt return unvalied, continue
            {
                continue;
            }
        }
        else //timeout
        {
            continue;
        }
    }
    
    if (3 == i) //Exit verify failed
    {
        return -1;
    }
    return 0;
}


static int getPressure(int *pGetVal)
{
    DmaUartProtocolPacket txPacket;
    EventBits_t uxBits;
    
    DmaUartProtocolPacketInit(&txPacket);
    txPacket.ID = (u8)PKT_ID_DPM_PRESSURE;
    txPacket.ACK = DMA_UART_PACKET_NACK;

    xEventGroupClearBits(xCoopMCUPktAckEventGroup, COOPMCU_PKT_ACK_BIT_DPM_PRESS);
    sendCoopMcuPkt(&txPacket, 0);
    
    uxBits = xEventGroupWaitBits(
            xCoopMCUPktAckEventGroup,   // The event group being tested.
            COOPMCU_PKT_ACK_BIT_DPM_PRESS,    // The bits within the event group to wait for.
            pdTRUE,                     // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
            pdFALSE,                    // Don't wait for both bits, either bit will do.
            2000 / portTICK_PERIOD_MS );// Wait a maximum of for either bit to be set.
    if (uxBits & COOPMCU_PKT_ACK_BIT_DPM_PRESS)
    {
        *pGetVal = gpCoopMcuDev->testDpmPressure;
        return 0;
    }
    return -1;
}

/*****************************************************************************
 Prototype    : verify310mmHgPoint
 Description  : verify NIBP 310mmHg
                1. Hold release; 
                2. pump on -> 320mmHg; 
                3. pump off and wait stable
                4. set and save.
 Input        : void  
 Output       : None
 Return Value : static
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2015/10/26
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
static int verify310mmHgPoint(void)
{
    u8 pBuf[2] = {0,};
    EventBits_t uxBits = 0;
    int press = 0;
    int ret = 0;
    int error_cnt = 0;

    INFO("=======================1. Hold release\r\n");
    pBuf[0] = NIBP_DEBUG_CID_RELEASE;
    pBuf[1] = (u8)BOTH_HOLD;
    xEventGroupClearBits(xDspPktAckEventGroup, DSP_PKT_ACK_BIT_NIBP_DEB);
    sendAioDspPktByID(AIO_RX_NIBP_Debug_ID, (char *)pBuf, 2, 0);
    
    uxBits = xEventGroupWaitBits(
            xDspPktAckEventGroup,   // The event group being tested.
            DSP_PKT_ACK_BIT_NIBP_DEB,    // The bits within the event group to wait for.
            pdTRUE,                     // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
            pdFALSE,                    // Don't wait for both bits, either bit will do.
            1000 / portTICK_PERIOD_MS );// Wait a maximum of for either bit to be set.
    if ((EventBits_t)0x00 == (uxBits & DSP_PKT_ACK_BIT_NIBP_DEB))
    {
        ERROR("BOTH_HOLD failed!!!\r\n");
        return -1;
    }
    
    INFO("=======================2. pump on -> 320mmHg\r\n");
    pBuf[0] = NIBP_DEBUG_CID_PUMP;
    pBuf[1] = 100; //100% percent
    xEventGroupClearBits(xDspPktAckEventGroup, DSP_PKT_ACK_BIT_NIBP_DEB);
    sendAioDspPktByID(AIO_RX_NIBP_Debug_ID, (char *)pBuf, 2, 0);
    
    uxBits = xEventGroupWaitBits(
            xDspPktAckEventGroup,   // The event group being tested.
            DSP_PKT_ACK_BIT_NIBP_DEB,    // The bits within the event group to wait for.
            pdTRUE,                     // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
            pdFALSE,                    // Don't wait for both bits, either bit will do.
            1000 / portTICK_PERIOD_MS );// Wait a maximum of for either bit to be set.
    if ((EventBits_t)0x00 == (uxBits & DSP_PKT_ACK_BIT_NIBP_DEB))
    {
        ERROR("PUMP ON failed!!!\r\n");
        return -2;
    }

    while(error_cnt < 5)
    {
        ret = getPressure(&press);
        if ((0 == ret) && press > 320000)
        {
            INFO("=======================3. pump off and wait stable\r\n");
            pBuf[0] = NIBP_DEBUG_CID_PUMP;
            pBuf[1] = 0x00; //PUMP OFF
            xEventGroupClearBits(xDspPktAckEventGroup, DSP_PKT_ACK_BIT_NIBP_DEB);
            sendAioDspPktByID(AIO_RX_NIBP_Debug_ID, (char *)pBuf, 2, 0);
            
            uxBits = xEventGroupWaitBits(
                    xDspPktAckEventGroup,   // The event group being tested.
                    DSP_PKT_ACK_BIT_NIBP_DEB,    // The bits within the event group to wait for.
                    pdTRUE,                     // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
                    pdFALSE,                    // Don't wait for both bits, either bit will do.
                    1000 / portTICK_PERIOD_MS );// Wait a maximum of for either bit to be set.
            if ((EventBits_t)0x00 == (uxBits & DSP_PKT_ACK_BIT_NIBP_DEB))
            {
                ERROR("PUMP OFF failed!!!\r\n");
                return -3;
            }
            else
            {
                break;
            }
        }
        if ((0 == ret) && (press > 280000) && (press < 300000))
        {
            refreshMaxAioBoardCurrent();
            INFO("=======================pump on PWM=50%\r\n");
            pBuf[0] = NIBP_DEBUG_CID_PUMP;
            pBuf[1] = 60;
            xEventGroupClearBits(xDspPktAckEventGroup, DSP_PKT_ACK_BIT_NIBP_DEB);
            sendAioDspPktByID(AIO_RX_NIBP_Debug_ID, (char *)pBuf, 2, 0);
        }
        if(0 != ret)
        {
            ERROR("getPressure failed!!!\r\n");
            error_cnt++;
        }
        vTaskDelay(NIBP_VERIFY_DELAY_MS/portTICK_PERIOD_MS);
    }

    if (5 == error_cnt)
    {
        pBuf[0] = NIBP_DEBUG_CID_RELEASE;
        pBuf[1] = (u8)BOTH_RELE;
        sendAioDspPktByID(AIO_RX_NIBP_Debug_ID, (char *)pBuf, 2, 0);
        ERROR("getPressure count = 5 failed!!!\r\n");
        return -4;
    }
    
    INFO("=======================4. set and save.\r\n");
    vTaskDelay(2000/portTICK_PERIOD_MS); //delay for stable
    error_cnt = 0;
    while(error_cnt < 5)
    {
        ret = getPressure(&press);
        if (0 == ret)
        {
            if ((press >= 300000) && (press <= 320000))
            {
                if ((press % 1000) > 750)
                {
                    press += 1000;
                    break;
                }
                else if ((press % 1000) < 250)
                {
                    break;
                }
            }
            else if (press > 320000)
            {
                ERROR("press > 320000 failed!!!\r\n");
//                return -4;

            }
            else if (press < 300000)
            {
                //failed!!
                ERROR("press < 300000 failed!!!\r\n");
                return -5;
            }
        }
        else
        {
            ERROR("getPressure failed!!!\r\n");
            error_cnt++;
        }
        vTaskDelay(NIBP_VERIFY_DELAY_MS/portTICK_PERIOD_MS);
    }

    if (5 == error_cnt)
    {
        pBuf[0] = NIBP_DEBUG_CID_RELEASE;
        pBuf[1] = (u8)BOTH_RELE;
        sendAioDspPktByID(AIO_RX_NIBP_Debug_ID, (char *)pBuf, 2, 0);
        ERROR("getPressure count = 5 failed!!!\r\n");
        return -4;
    }
    
    press = press/1000;
    pBuf[0] = (u8)((press >> 8) & 0xFF);
    pBuf[1] = (u8)(press & 0xFF);
    xEventGroupClearBits(xDspPktAckEventGroup, DSP_PKT_ACK_BIT_310MMHG);
    sendAioDspPktByID(AIO_NIBP_310MMHG_ID, (char *)pBuf, 2, 0);
    
    uxBits = xEventGroupWaitBits(
            xDspPktAckEventGroup,   // The event group being tested.
            DSP_PKT_ACK_BIT_310MMHG,    // The bits within the event group to wait for.
            pdTRUE,                     // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
            pdFALSE,                    // Don't wait for both bits, either bit will do.
            1000 / portTICK_PERIOD_MS );// Wait a maximum of for either bit to be set.
    if (uxBits & DSP_PKT_ACK_BIT_310MMHG)
    {
        if ((pBuf[0] == gpDspAckResult->u8DspAck310mmHgVal[0]) \
            &&(pBuf[1] == gpDspAckResult->u8DspAck310mmHgVal[1]))
        {
            return 0;
        }
    }
    ERROR("set 310mmHg failed!!!\r\n");
    return -6;
}

/*****************************************************************************
 Prototype    : verify150mmHgPoint
 Description  : verify 150mmHg
                1.slow release at 150mmHg
                2.wait for stable
                3.set and save
 Input        : void  
 Output       : None
 Return Value : static
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2015/10/26
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
static int verify150mmHgPoint(void)
{
    u8 pBuf[2] = {0,};
    EventBits_t uxBits = 0;
    int press = 0;
    int ret = 0;
    int error_cnt = 0;
    
    INFO("=======================1.slow release at 150mmHg\r\n");
    while(error_cnt < 5)
    {
        ret = getPressure(&press);
        if ((0 == ret) && press > 150000)
        {
            pBuf[0] = NIBP_DEBUG_CID_RELEASE;
            pBuf[1] = (u8)SLOW_RELE;
            xEventGroupClearBits(xDspPktAckEventGroup, DSP_PKT_ACK_BIT_NIBP_DEB);
            sendAioDspPktByID(AIO_RX_NIBP_Debug_ID, (char *)pBuf, 2, 0);
            
            uxBits = xEventGroupWaitBits(
                    xDspPktAckEventGroup,   // The event group being tested.
                    DSP_PKT_ACK_BIT_NIBP_DEB,    // The bits within the event group to wait for.
                    pdTRUE,                     // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
                    pdFALSE,                    // Don't wait for both bits, either bit will do.
                    1000 / portTICK_PERIOD_MS );// Wait a maximum of for either bit to be set.
            if ((EventBits_t)0x00 == (uxBits & DSP_PKT_ACK_BIT_NIBP_DEB))
            {
                ERROR("SLOW_RELE failed!!!\r\n");
            }
            else
            {
                INFO("SLOW_RELE success!!!\r\n");
            }
        }
        else if ((0 == ret) && press < 150000)
        {
            pBuf[0] = NIBP_DEBUG_CID_RELEASE;
            pBuf[1] = (u8)BOTH_HOLD;
            xEventGroupClearBits(xDspPktAckEventGroup, DSP_PKT_ACK_BIT_NIBP_DEB);
            sendAioDspPktByID(AIO_RX_NIBP_Debug_ID, (char *)pBuf, 2, 0);
            
            uxBits = xEventGroupWaitBits(
                    xDspPktAckEventGroup,   // The event group being tested.
                    DSP_PKT_ACK_BIT_NIBP_DEB,    // The bits within the event group to wait for.
                    pdTRUE,                     // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
                    pdFALSE,                    // Don't wait for both bits, either bit will do.
                    1000 / portTICK_PERIOD_MS );// Wait a maximum of for either bit to be set.
            if ((EventBits_t)0x00 == (uxBits & DSP_PKT_ACK_BIT_NIBP_DEB))
            {
                ERROR("BOTH_HOLD failed!!!\r\n");
            }
            else
            {
                INFO("BOTH_HOLD success!!!\r\n");
                break;
            }
        }
        if(0 != ret)
        {
            ERROR("getPressure failed!!!\r\n");
            error_cnt++;
        }
        vTaskDelay(NIBP_VERIFY_DELAY_MS/portTICK_PERIOD_MS);
    }
    
    if (5 == error_cnt)
    {
        pBuf[0] = NIBP_DEBUG_CID_RELEASE;
        pBuf[1] = (u8)BOTH_RELE;
        sendAioDspPktByID(AIO_RX_NIBP_Debug_ID, (char *)pBuf, 2, 0);
        ERROR("getPressure count = 5 failed!!!\r\n");
        return -4;
    }
    
    INFO("=======================2.wait for stable\r\n");
    vTaskDelay(2000/portTICK_PERIOD_MS); //delay for stable
    error_cnt = 0;
    while(error_cnt < 5)
    {
        ret = getPressure(&press);
        if (0 == ret)
        {
            if ((press >= 140000) && (press <= 160000))
            {
                if ((press % 1000) > 750)
                {
                    press += 1000;
                    break;
                }
                else if ((press % 1000) < 250)
                {
                    break;
                }
            }
            else if (press > 160000)
            {
                ERROR("press > 160000 failed!!!\r\n");
//                return -4;

            }
            else if (press < 140000)
            {
                //failed!!
                ERROR("press < 140000 failed!!!\r\n");
                return -5;
            }
        }
        else
        {
            ERROR("getPressure failed!!!\r\n");
            error_cnt++;
        }
        vTaskDelay(NIBP_VERIFY_DELAY_MS/portTICK_PERIOD_MS);
    }

    if (5 == error_cnt)
    {
        pBuf[0] = NIBP_DEBUG_CID_RELEASE;
        pBuf[1] = (u8)BOTH_RELE;
        sendAioDspPktByID(AIO_RX_NIBP_Debug_ID, (char *)pBuf, 2, 0);
        ERROR("getPressure count = 5 failed!!!\r\n");
        return -4;
    }
    
    press = press/1000;
    pBuf[0] = (u8)(press & 0xFF);
    xEventGroupClearBits(xDspPktAckEventGroup, DSP_PKT_ACK_BIT_150MMHG);
    sendAioDspPktByID(AIO_NIBP_150MMHG_ID, (char *)pBuf, 1, 0);
    
    uxBits = xEventGroupWaitBits(
            xDspPktAckEventGroup,   // The event group being tested.
            DSP_PKT_ACK_BIT_150MMHG,    // The bits within the event group to wait for.
            pdTRUE,                     // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
            pdFALSE,                    // Don't wait for both bits, either bit will do.
            1000 / portTICK_PERIOD_MS );// Wait a maximum of for either bit to be set.
    if (uxBits & DSP_PKT_ACK_BIT_150MMHG)
    {
        if (pBuf[0] == gpDspAckResult->u8DspAck150mmHgVal)
        {
            return 0;
        }
    }
    ERROR("set 150mmHg failed!!!\r\n");
    return -6;
}

int testNibpVerify(void)
{
    //S1 Verify Enter and 0mmHg
    if (enterNibpVerify() < 0)
    {
        ERROR("enterNibpVerify failed!!!\r\n");
        return -1;
    }

    //S2 Verify 300mmHg ~ 320mmHg
    if (verify310mmHgPoint() < 0)
    {
        ERROR("verify310mmHgPoint failed!!!\r\n");
        return -2;
    }

    //S3 Verify 140mmHg ~ 160mmHg
    if (verify150mmHgPoint() < 0)
    {
        ERROR("verify150mmHgPoint failed!!!\r\n");
        return -3;
    }

    //S4 Verify Exit
    if (exitNibpVerify() < 0)
    {
        ERROR("exitNibpVerify failed!!!\r\n");
        return -4;
    }
    
    return 0;
}

static int check280mmHgPoint(void)
{

    u8 pBuf[2] = {0,};
    EventBits_t uxBits = 0;
    int press = 0;
    int ret = 0;
    char error_cnt = 0;
    char check_cnt = 0;
    char over_cnt = 0;

    INFO("=======================1. Hold release\r\n");
    pBuf[0] = NIBP_DEBUG_CID_RELEASE;
    pBuf[1] = (u8)BOTH_HOLD;
    xEventGroupClearBits(xDspPktAckEventGroup, DSP_PKT_ACK_BIT_NIBP_DEB);
    sendAioDspPktByID(AIO_RX_NIBP_Debug_ID, (char *)pBuf, 2, 0);
    
    uxBits = xEventGroupWaitBits(
            xDspPktAckEventGroup,   // The event group being tested.
            DSP_PKT_ACK_BIT_NIBP_DEB,    // The bits within the event group to wait for.
            pdTRUE,                     // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
            pdFALSE,                    // Don't wait for both bits, either bit will do.
            1000 / portTICK_PERIOD_MS );// Wait a maximum of for either bit to be set.
    if ((EventBits_t)0x00 == (uxBits & DSP_PKT_ACK_BIT_NIBP_DEB))
    {
        ERROR("BOTH_HOLD failed!!!\r\n");
        return -1;
    }
    
    INFO("=======================2. pump on -> 320mmHg\r\n");
    pBuf[0] = NIBP_DEBUG_CID_PUMP;
    pBuf[1] = 100; //100% percent
    xEventGroupClearBits(xDspPktAckEventGroup, DSP_PKT_ACK_BIT_NIBP_DEB);
    sendAioDspPktByID(AIO_RX_NIBP_Debug_ID, (char *)pBuf, 2, 0);
    
    uxBits = xEventGroupWaitBits(
            xDspPktAckEventGroup,   // The event group being tested.
            DSP_PKT_ACK_BIT_NIBP_DEB,    // The bits within the event group to wait for.
            pdTRUE,                     // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
            pdFALSE,                    // Don't wait for both bits, either bit will do.
            1000 / portTICK_PERIOD_MS );// Wait a maximum of for either bit to be set.
    if ((EventBits_t)0x00 == (uxBits & DSP_PKT_ACK_BIT_NIBP_DEB))
    {
        ERROR("PUMP ON failed!!!\r\n");
        return -2;
    }
    
    while(error_cnt < 5)
    {
        ret = getPressure(&press);
        if ((0 == ret) && press > 260000)
        {
            INFO("=======================3. pump off and wait stable\r\n");
            pBuf[0] = NIBP_DEBUG_CID_PUMP;
            pBuf[1] = 0x00; //PUMP OFF
            xEventGroupClearBits(xDspPktAckEventGroup, DSP_PKT_ACK_BIT_NIBP_DEB);
            sendAioDspPktByID(AIO_RX_NIBP_Debug_ID, (char *)pBuf, 2, 0);
            
            uxBits = xEventGroupWaitBits(
                    xDspPktAckEventGroup,   // The event group being tested.
                    DSP_PKT_ACK_BIT_NIBP_DEB,    // The bits within the event group to wait for.
                    pdTRUE,                     // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
                    pdFALSE,                    // Don't wait for both bits, either bit will do.
                    1000 / portTICK_PERIOD_MS );// Wait a maximum of for either bit to be set.
            if ((EventBits_t)0x00 == (uxBits & DSP_PKT_ACK_BIT_NIBP_DEB))
            {
                ERROR("PUMP OFF failed!!!\r\n");
                return -3;
            }
            else
            {
                break;
            }
        }
        if ((0 == ret) && (press > 200000) && (press < 220000))
        {
            refreshMaxAioBoardCurrent();
            INFO("=======================pump on PWM=50%\r\n");
            pBuf[0] = NIBP_DEBUG_CID_PUMP;
            pBuf[1] = 60;
            xEventGroupClearBits(xDspPktAckEventGroup, DSP_PKT_ACK_BIT_NIBP_DEB);
            sendAioDspPktByID(AIO_RX_NIBP_Debug_ID, (char *)pBuf, 2, 0);
        }
        if(0 != ret)
        {
            ERROR("getPressure failed!!!\r\n");
            error_cnt++;
        }
        vTaskDelay(NIBP_VERIFY_DELAY_MS/portTICK_PERIOD_MS);
    }

    if (5 == error_cnt)
    {
        pBuf[0] = NIBP_DEBUG_CID_RELEASE;
        pBuf[1] = (u8)BOTH_RELE;
        sendAioDspPktByID(AIO_RX_NIBP_Debug_ID, (char *)pBuf, 2, 0);
        ERROR("getPressure count = 5 failed!!!\r\n");
        return -4;
    }
    
    vTaskDelay(2000/portTICK_PERIOD_MS); //delay for stable
    error_cnt = 0;
    check_cnt = 0;
    while(error_cnt < 5 && check_cnt < 5)
    {
        ret = getPressure(&press);
        if (0 == ret)
        {
            INFO("DPM=%d, AIO=%d\r\n",press, gpDspAckResult->u16DspAckMMHG);
            press = (press + 500) / 1000;
            if ((gpDspAckResult->u16DspAckMMHG > press+2) \
                ||(gpDspAckResult->u16DspAckMMHG < press-2))
            {
                ERROR("check280mmHgPoint over_range\r\n");
                over_cnt++;
            }
            check_cnt++;
        }
        else
        {
            ERROR("getPressure failed!!!\r\n");
            error_cnt++;
        }
        vTaskDelay(500/portTICK_PERIOD_MS);
    }
    
    if (5 == error_cnt)
    {
        pBuf[0] = NIBP_DEBUG_CID_RELEASE;
        pBuf[1] = (u8)BOTH_RELE;
        sendAioDspPktByID(AIO_RX_NIBP_Debug_ID, (char *)pBuf, 2, 0);
        ERROR("getPressure count = 5 failed!!!\r\n");
        return -5;
    }
    
    if (over_cnt > 2)
    {
        return -6;
    }
    return 0;
}

static int check80mmHgPoint(void)
{
    u8 pBuf[2] = {0,};
    EventBits_t uxBits = 0;
    int press = 0;
    int ret = 0;
    int error_cnt = 0;
    char check_cnt = 0;
    char over_cnt = 0;
    
    INFO("=======================1.slow release at 150mmHg\r\n");
    while(error_cnt < 5)
    {
        ret = getPressure(&press);
        if ((0 == ret) && press > 80000)
        {
            pBuf[0] = NIBP_DEBUG_CID_RELEASE;
            pBuf[1] = (u8)SLOW_RELE;
            xEventGroupClearBits(xDspPktAckEventGroup, DSP_PKT_ACK_BIT_NIBP_DEB);
            sendAioDspPktByID(AIO_RX_NIBP_Debug_ID, (char *)pBuf, 2, 0);
            
            uxBits = xEventGroupWaitBits(
                    xDspPktAckEventGroup,   // The event group being tested.
                    DSP_PKT_ACK_BIT_NIBP_DEB,    // The bits within the event group to wait for.
                    pdTRUE,                     // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
                    pdFALSE,                    // Don't wait for both bits, either bit will do.
                    1000 / portTICK_PERIOD_MS );// Wait a maximum of for either bit to be set.
            if ((EventBits_t)0x00 == (uxBits & DSP_PKT_ACK_BIT_NIBP_DEB))
            {
                ERROR("SLOW_RELE failed!!!\r\n");
            }
            else
            {
                INFO("SLOW_RELE success!!!\r\n");
            }
        }
        else if ((0 == ret) && press < 80000)
        {
            pBuf[0] = NIBP_DEBUG_CID_RELEASE;
            pBuf[1] = (u8)BOTH_HOLD;
            xEventGroupClearBits(xDspPktAckEventGroup, DSP_PKT_ACK_BIT_NIBP_DEB);
            sendAioDspPktByID(AIO_RX_NIBP_Debug_ID, (char *)pBuf, 2, 0);
            
            uxBits = xEventGroupWaitBits(
                    xDspPktAckEventGroup,   // The event group being tested.
                    DSP_PKT_ACK_BIT_NIBP_DEB,    // The bits within the event group to wait for.
                    pdTRUE,                     // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
                    pdFALSE,                    // Don't wait for both bits, either bit will do.
                    1000 / portTICK_PERIOD_MS );// Wait a maximum of for either bit to be set.
            if ((EventBits_t)0x00 == (uxBits & DSP_PKT_ACK_BIT_NIBP_DEB))
            {
                ERROR("BOTH_HOLD failed!!!\r\n");
            }
            else
            {
                INFO("BOTH_HOLD success!!!\r\n");
                break;
            }
        }
        if(0 != ret)
        {
            ERROR("getPressure failed!!!\r\n");
            error_cnt++;
        }
        vTaskDelay(NIBP_VERIFY_DELAY_MS/portTICK_PERIOD_MS);
    }
    
    if (5 == error_cnt)
    {
        pBuf[0] = NIBP_DEBUG_CID_RELEASE;
        pBuf[1] = (u8)BOTH_RELE;
        sendAioDspPktByID(AIO_RX_NIBP_Debug_ID, (char *)pBuf, 2, 0);
        ERROR("getPressure count = 5 failed!!!\r\n");
        return -4;
    }
    
    vTaskDelay(2000/portTICK_PERIOD_MS); //delay for stable
    error_cnt = 0;
    check_cnt = 0;
    while(error_cnt < 5 && check_cnt < 5)
    {
        ret = getPressure(&press);
        if (0 == ret)
        {
            INFO("DPM=%d, AIO=%d\r\n",press, gpDspAckResult->u16DspAckMMHG);
            press = (press + 500) / 1000;
            if ((gpDspAckResult->u16DspAckMMHG > press+2) \
                ||(gpDspAckResult->u16DspAckMMHG < press-2))
            {
                ERROR("check80mmHgPoint over_range\r\n");
                over_cnt++;
            }
            check_cnt++;
        }
        else
        {
            ERROR("getPressure failed!!!\r\n");
            error_cnt++;
        }
        vTaskDelay(500/portTICK_PERIOD_MS);
    }
    
    if (5 == error_cnt)
    {
        ERROR("getPressure count = 5 failed!!!\r\n");
        return -5;
    }
    if (over_cnt > 2)
    {
        return -6;
    }
    
    pBuf[0] = NIBP_DEBUG_CID_RELEASE;
    pBuf[1] = (u8)BOTH_RELE;
    sendAioDspPktByID(AIO_RX_NIBP_Debug_ID, (char *)pBuf, 2, 0);

    vTaskDelay(5000/portTICK_PERIOD_MS); //delay for stable
    error_cnt = 0;
    check_cnt = 0;
    over_cnt = 0;
    while(error_cnt < 5 && check_cnt < 5)
    {
        ret = getPressure(&press);
        if (0 == ret)
        {
            INFO("DPM=%d, AIO=%d\r\n",press, gpDspAckResult->u16DspAckMMHG);
            press = (press + 500) / 1000;
            if (gpDspAckResult->u16DspAckMMHG > press+2)
            {
                ERROR("check0mmHgPoint over_range\r\n");
                over_cnt++;
            }
            check_cnt++;
        }
        else
        {
            ERROR("getPressure failed!!!\r\n");
            error_cnt++;
        }
        vTaskDelay(500/portTICK_PERIOD_MS);
    }
    
    if (5 == error_cnt)
    {
        ERROR("getPressure count = 5 failed!!!\r\n");
        return -6;
    }
    if (over_cnt > 2)
    {
        return -7;
    }
    
    return 0;
}

/*****************************************************************************
 Prototype    : testNibpGasControl
 Description  : 
 Input        : void  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2015/10/27
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
int testNibpGasControl(void)
{
    if (enterNibpVerify() < 0)
    {
        ERROR("enterNibpVerify failed!!!\r\n");
        return -1;
    }
    
    if (check280mmHgPoint() < 0)
    {
        ERROR("check280mmHgPoint failed!!!\r\n");
        return -2;
    }
    
    if (check80mmHgPoint() < 0)
    {
        ERROR("check80mmHgPoint failed!!!\r\n");
        return -3;
    }
    
    if (exitNibpVerify() < 0)
    {
        ERROR("exitNibpVerify failed!!!\r\n");
        return -4;
    }
    return 0;
}

static int checkNibpDspProtect(void)
{
    u8 pBuf[2] = {0,};
    EventBits_t uxBits = 0;

    pBuf[0] = NIBP_DEBUG_CHECK_DSP_PROTECT;
    pBuf[1] = 70;
    xEventGroupClearBits(xDspPktAckEventGroup, DSP_PKT_ACK_BIT_NIBP_DEB);
    sendAioDspPktByID(AIO_RX_NIBP_Debug_ID, (char *)pBuf, 2, 0);
    
    uxBits = xEventGroupWaitBits(
            xDspPktAckEventGroup,   // The event group being tested.
            DSP_PKT_ACK_BIT_NIBP_DEB,    // The bits within the event group to wait for.
            pdTRUE,                     // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
            pdFALSE,                    // Don't wait for both bits, either bit will do.
            1000 / portTICK_PERIOD_MS );// Wait a maximum of for either bit to be set.
    if ((EventBits_t)0x00 == (uxBits & DSP_PKT_ACK_BIT_NIBP_DEB))
    {
        return -1;
    }

    xEventGroupClearBits(xDspPktAckEventGroup, 
                        DSP_PKT_ACK_BIT_NIBP_MMHG|DSP_PKT_ACK_BIT_NIBP_ALARM);
    while(1)
    {
        uxBits = xEventGroupWaitBits(
                xDspPktAckEventGroup,   // The event group being tested.
                DSP_PKT_ACK_BIT_NIBP_DEB,    // The bits within the event group to wait for.
                pdTRUE,                     // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
                pdFALSE,                    // Don't wait for both bits, either bit will do.
                1000 / portTICK_PERIOD_MS );// Wait a maximum of for either bit to be set.
        if (uxBits & DSP_PKT_ACK_BIT_NIBP_MMHG)
        {
            udprintf("Current Press:%dmmHg\r\n",gpDspAckResult->u16DspAckMMHG);
            if (gpDspAckResult->u16DspAckMMHG > 340)
            {
                udprintf("protect function failed!!!\r\n");
                break;
            }
        }
        if (uxBits & DSP_PKT_ACK_BIT_NIBP_ALARM)
        {
            if ((u8)OVER_PROTECT_PRESSURE == gpDspAckResult->u8DspAckNibpAlarmType)
            {
                udprintf("protect function success!!!\r\n");
                
                return 0;
            }
            else
            {
                udprintf("Nibp Alarm type=%d!!!\r\n", gpDspAckResult->u8DspAckNibpAlarmType);
                break;
            }
        }
    }
    sendAndWaitNibpStop();
    return -1;
}

static int checkNibpStmProtect(void)
{
    u8 pBuf[2] = {0,};
    EventBits_t uxBits = 0;

    pBuf[0] = NIBP_DEBUG_CHECK_STM_PROTECT;
    pBuf[1] = 70;
    xEventGroupClearBits(xDspPktAckEventGroup, DSP_PKT_ACK_BIT_NIBP_DEB);
    sendAioDspPktByID(AIO_RX_NIBP_Debug_ID, (char *)pBuf, 2, 0);
    
    uxBits = xEventGroupWaitBits(
            xDspPktAckEventGroup,   // The event group being tested.
            DSP_PKT_ACK_BIT_NIBP_DEB,    // The bits within the event group to wait for.
            pdTRUE,                     // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
            pdFALSE,                    // Don't wait for both bits, either bit will do.
            1000 / portTICK_PERIOD_MS );// Wait a maximum of for either bit to be set.
    if ((EventBits_t)0x00 == (uxBits & DSP_PKT_ACK_BIT_NIBP_DEB))
    {
        return -1;
    }
    
    xEventGroupClearBits(xDspPktAckEventGroup, 
                        DSP_PKT_ACK_BIT_NIBP_MMHG|DSP_PKT_ACK_BIT_NIBP_ALARM);
    while(1)
    {
        uxBits = xEventGroupWaitBits(
                xDspPktAckEventGroup,   // The event group being tested.
                DSP_PKT_ACK_BIT_NIBP_DEB,    // The bits within the event group to wait for.
                pdTRUE,                     // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
                pdFALSE,                    // Don't wait for both bits, either bit will do.
                1000 / portTICK_PERIOD_MS );// Wait a maximum of for either bit to be set.
        if (uxBits & DSP_PKT_ACK_BIT_NIBP_MMHG)
        {
            udprintf("Current Press:%dmmHg\r\n",gpDspAckResult->u16DspAckMMHG);
            if (gpDspAckResult->u16DspAckMMHG > 340)
            {
                udprintf("protect function failed!!!\r\n");
                break;
            }
        }
        if (uxBits & DSP_PKT_ACK_BIT_NIBP_ALARM)
        {
            if ((u8)OVER_PROTECT_PRESSURE == gpDspAckResult->u8DspAckNibpAlarmType)
            {
                udprintf("protect function success!!!\r\n");
                return 0;
            }
            else
            {
                udprintf("Nibp Alarm type=%d!!!\r\n", gpDspAckResult->u8DspAckNibpAlarmType);
                break;
            }
        }
    }
    sendAndWaitNibpStop();
    return -1;
}

int testNibpOverPress(void)
{
    if (checkNibpDspProtect() < 0)
    {
        udprintf("testNibpOverPress failed!\r\n");
        return -1;
    }

    if (checkNibpStmProtect() < 0)
    {
        udprintf("testNibpStmProtect failed!\r\n");
        return -2;
    }
    return 0;
}

int testIbpSelfcheck(void)
{
    return 0;
}

int testIbpProbeOff(void)
{
    return 0;
}

int testIbpAmplitudeBand(void)
{
    return 0;
}

