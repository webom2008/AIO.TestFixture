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

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/
#define AIOBOARD_PUMP_ON_CURRENT_MAX        ((int)500) //mA
#define AIOBOARD_PUMP_OFF_CURRENT_MAX       ((int)150) //mA

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

int refreshMaxAioBoardCurrent(void)
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

