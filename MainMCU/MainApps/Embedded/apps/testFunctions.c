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

//#define _INFO_
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
    gAioBoardMaxCurrent = 0;
    
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
        refreshMaxAioBoardCurrent();
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
            INFO("=======================pump on PWM=50%\r\n");
            pBuf[0] = NIBP_DEBUG_CID_PUMP;
            pBuf[1] = 50;
            xEventGroupClearBits(xDspPktAckEventGroup, DSP_PKT_ACK_BIT_NIBP_DEB);
            sendAioDspPktByID(AIO_RX_NIBP_Debug_ID, (char *)pBuf, 2, 0);
        }
        if(0 != ret)
        {
            ERROR("getPressure failed!!!\r\n");
            error_cnt++;
        }
        vTaskDelay(200/portTICK_PERIOD_MS);
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
        vTaskDelay(200/portTICK_PERIOD_MS);
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
        vTaskDelay(50/portTICK_PERIOD_MS);
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
        vTaskDelay(200/portTICK_PERIOD_MS);
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

