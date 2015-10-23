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

static int testDPM2200Connect(void)
{
    DmaUartProtocolPacket txPacket;
    EventBits_t uxBits;
    
    DmaUartProtocolPacketInit(&txPacket);
    txPacket.ID = (u8)PKT_ID_DPM_UNITS;
    txPacket.ACK = DMA_UART_PACKET_NACK;

    sendCoopMcuPkt(&txPacket, 0);
    
    uxBits = xEventGroupWaitBits(
            xCoopMCUPktAckEventGroup,   // The event group being tested.
            COOPMCU_PKT_ACK_BIT_DPM_UNITS,    // The bits within the event group to wait for.
            pdTRUE,                     // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
            pdFALSE,                    // Don't wait for both bits, either bit will do.
            1000 / portTICK_PERIOD_MS );// Wait a maximum of for either bit to be set.
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
    if (testComputerConnect() < 0)
    {
        return -1;
    }
    if (testDPM2200Connect() < 0)
    {
        return -2;
    }
    return 0;
}

int testAioBoardMaxCurrent(void)
{
    DmaUartProtocolPacket txPacket;
    int normal_current = 0;
    int pump_on_current = 0;
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
            ERROR("testAIOBaordCurrent timeout!!!\r\n");
            return 1;
        }
        vTaskDelay(100);
    }
    
    normal_current = normal_current / 5;

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
            pump_on_current += gpCoopMcuDev->testAioBoardCurrent;
        }
        else //timeout
        {
            ERROR("testAIOBaordCurrent timeout!!!\r\n");
            return 1;
        }
        vTaskDelay(100);
    }
    
    pump_on_current = pump_on_current / 5;
    
    if ((pump_on_current > AIOBOARD_PUMP_ON_CURRENT_MAX) \
        ||(normal_current > AIOBOARD_PUMP_OFF_CURRENT_MAX))
    {
        ERROR("AIOBaordCurrent pump_on_current= %d,normal_current=%d\r\n",
            pump_on_current,normal_current);
        return 1;
    }
    INFO("AIOBaordCurrent pump_on_current= %d,normal_current=%d\r\n",
        pump_on_current,normal_current);
    return 0;
}

int getEcgSelfcheck(void)
{
    return 0;
}

int testEcgAmplitudeBand(void)
{
    return 0;
}

int testEcgProbeOff(void)
{
    return 0;
}

int testEcgPolarity(void)
{
    return 0;
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

int testNibpSelfcheck(void)
{
    return 0;
}

int testNibpVerify(void)
{
    return 0;
}

int testNibpGasControl(void)
{
    return 0;
}

int testNibpOverPress(void)
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

