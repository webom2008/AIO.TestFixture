/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : AioDspProtocol.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/17
  Last Modified :
  Description   : AIO-DSP uart potocol
  Function List :
  History       :
  1.Date        : 2015/9/17
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
static AioDspProtocolPkt gCurrentRxPkt;
/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/
enum AnalysisStatus{
    WaitDstAddr,
    WaitSrcAddr,
    WaitSN,
    WaitType,
    WaitLen,
    WaitData,
    WaitCRC
};

//#define _INFO_
#define _ERROR_

#ifdef _INFO_
#define INFO(fmt, arg...) udprintf("[AioDSP]Info: "fmt, ##arg)
#else
#define INFO(fmt, arg...) do{}while(0)
#endif
#ifdef _ERROR_
#define ERROR(fmt, arg...) udprintf("[AioDSP]Error: "fmt, ##arg)
#else
#define ERROR(fmt, arg...) do{}while(0)
#endif
/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/
static int exePacket(AioDspProtocolPkt *pPacket);

u8 crc8AioDspPkt(const AioDspProtocolPkt *pPacket)
{
    u8 len = 3 + (pPacket->Length);//3(PacketNum,PacketID,Length)+Data length
    u8 crc = crc8(&pPacket->PacketNum, len);
    return crc;
}

static int tryUnpack(char *pBuf, int *pBufLen, AioDspProtocolPkt *pPacket)
{
    int i = 0, nTempLen = 0, len = *pBufLen;
    int mStatus = WaitDstAddr;
    char crc_ok = 0;
    
    memset((char *)pPacket, 0, sizeof(AioDspProtocolPkt));
    
    while ((i < len) && (0 == crc_ok))
    {
        switch (mStatus)
        {
        case WaitDstAddr:{
            pPacket->DR_Addr = pBuf[i++];
            if (MPU_ADDR == pPacket->DR_Addr){
                mStatus = WaitSrcAddr;
            }
        }
            break;
        case WaitSrcAddr:{
            pPacket->SR_Addr = pBuf[i++];
            if (AIO_ADDR == pPacket->SR_Addr){
                mStatus = WaitSN;
            }else{
                mStatus = WaitDstAddr;
            }
        }
            break;
        case WaitSN:{
            pPacket->PacketNum = pBuf[i++];
            mStatus = WaitType;
        }
            break;
        case WaitType:{
            pPacket->PacketID = pBuf[i++];
            mStatus = WaitLen;
        }
            break;
        case WaitLen:{
            pPacket->Length = pBuf[i++];
            if ((pPacket->Length > 0) && (pPacket->Length <= PACKET_DATA_LEN_MAX)){
                nTempLen = 0;
                mStatus = WaitData;
            } else if (pPacket->Length > PACKET_DATA_LEN_MAX){
                mStatus = WaitDstAddr;
            }
            else {
                mStatus = WaitCRC;
            }
        }
            break;
        case WaitData:{
            if (nTempLen < pPacket->Length){
                pPacket->DataAndCRC[nTempLen++] = pBuf[i++];
            }else{
                mStatus = WaitCRC;
            }
        }
            break;
        case WaitCRC:{
            pPacket->DataAndCRC[pPacket->Length] = pBuf[i++];
            mStatus = WaitDstAddr;
            if (pPacket->DataAndCRC[pPacket->Length] == crc8AioDspPkt(pPacket)){
                crc_ok = 1;
            }else{
                crc_ok = 0;
            }
        }
            break;
        default:
            break;

        } // End of switch (mStatus)
    } // End of while ( && (0 == crc_ok))

    
    if (crc_ok)
    {
        //moving data
        nTempLen = len - i;
        memcpy(pBuf, &pBuf[i], nTempLen);
        memset(&pBuf[nTempLen], 0x00, i);
        *pBufLen = nTempLen; 
        return 1;
    }

    if ((0 == crc_ok) && (i < len))
    {
        //moving data
        nTempLen = len - i;
        memcpy(pBuf, &pBuf[i], nTempLen);
        memset(&pBuf[nTempLen], 0x00, i);
        *pBufLen = nTempLen; 
        udprintf("AIO-PKT CRC error !\r\n");
        return 0;
    }
    
    if ((i == len) &&(WaitDstAddr == mStatus)){
            memset(pBuf, 0x00, len);
            *pBufLen = 0;
    }
    return 0;
}

#define AIO_DSP_RX_BUFF_LEN_MAX         256
static char AioDspRxBuf[AIO_DSP_RX_BUFF_LEN_MAX] = {0,};
static int AioDspRxOffset = 0;

static void AioDspTryUnpackTask(void *pvParameters)
{
    int rLen = 0;
    const TickType_t xTicksToWait = 5 / portTICK_PERIOD_MS;
    int tryCountinue = 1;
    /* Just to stop compiler warnings. */
    ( void ) pvParameters;
    
    INFO("TestedAIOTryUnpackTask running...\r\n");
    for (;;)
    {
        rLen = AioBoardRead(&AioDspRxBuf[AioDspRxOffset], AIO_DSP_RX_BUFF_LEN_MAX-AioDspRxOffset);
        if (rLen > 0)
        {
            AioDspRxOffset += rLen;
        }
        tryCountinue = 1;
        while ((AioDspRxOffset > PACKET_FIXED_LENGHT) && (1 == tryCountinue))
        {
            tryCountinue = tryUnpack(AioDspRxBuf, &AioDspRxOffset, &gCurrentRxPkt);
            if (tryCountinue)
            {
                exePacket(&gCurrentRxPkt);
            }
        }
        
        vTaskDelay(xTicksToWait);
    }
}

void initAioDspPkt(AioDspProtocolPkt *pTxPacket)
{
    memset(pTxPacket, 0, sizeof(AioDspProtocolPkt));

    pTxPacket->DR_Addr = AIO_ADDR;
    pTxPacket->SR_Addr = MPU_ADDR;
}


int sendAioDspPkt(AioDspProtocolPkt *pAioDspPkt)
{
    int res = 0;
    res = AioBoardWrite((char *)pAioDspPkt, pAioDspPkt->Length + PACKET_FIXED_LENGHT);
    return res;
}

int sendAioDspPktByID(const UART_PacketID id, char* pData, const u8 lenght, const u8 number)
{
    AioDspProtocolPkt packet;
    int res = 0;
    
    initAioDspPkt(&packet);
    packet.PacketID = id;
    if ((NULL != pData) && (lenght <= PACKET_DATA_LEN_MAX))
    {
        packet.Length = lenght;
        memcpy(packet.DataAndCRC, pData, lenght);
    }
    packet.PacketNum = number;
    packet.DataAndCRC[packet.Length] = crc8AioDspPkt(&packet);
    
    res = AioBoardWrite((char *)&packet, packet.Length + PACKET_FIXED_LENGHT);
    return res;
}

int createAioDspUnpackTask(void)
{
    while (pdPASS != xTaskCreate(   AioDspTryUnpackTask,
                                    "AioDspTryUnpackTask",
                                    configMINIMAL_STACK_SIZE,
                                    NULL,
                                    AIOBOARD_UNPACK_TASK_PRIORITY,
                                    NULL));
    return 0;
}

static int exePacket(AioDspProtocolPkt *pPacket)
{
    UART_PacketID id = (UART_PacketID)pPacket->PacketID;

    
    if ((SF_SPO2_UPDATE == id) \
        || (SF_AIO_STM_UPDATE == id) \
        || (SF_AIO_DSP_UPDATE == id)\
        || (COM_SOFTWARE_VERSION_ID == id))
    {
        sendComputerPkt(pPacket);
    }
    else if (AIO_TX_ECG_LEAD_INFO_ID == id)//do nothing...
    {
        udprintf("PktID=0x%02X,PktNum=%d\r\n",id,pPacket->PacketNum); 
    }
    else
    {

    }
    return 0;
}
