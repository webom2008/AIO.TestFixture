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
#define INFO(fmt, arg...) udprintf("[MainMCU]Info: "fmt, ##arg)
#else
#define INFO(fmt, arg...) do{}while(0)
#endif
#ifdef _ERROR_
#define ERROR(fmt, arg...) udprintf("[MainMCU]Error: "fmt, ##arg)
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
    int dataLen, len = *pBufLen, tempLen = 0, i;
    static int mStatus = WaitDstAddr;
    
    for (i=0; i<len;)
    {
        switch(mStatus)
        {
            case WaitDstAddr:
            {
                if (MPU_ADDR == pBuf[i])
                {
                    pPacket->DR_Addr = pBuf[i];
                    mStatus = WaitSrcAddr;
                }
                i++;
            }
                break;
            case WaitSrcAddr:
            {
                if (AIO_ADDR == pBuf[i])
                {
                    pPacket->SR_Addr = pBuf[i];
                    mStatus = WaitSN;
                    i++;
                }
                else
                {
                    mStatus = WaitDstAddr;
                }
            }
                break;
            case WaitSN:
            {
                pPacket->PacketNum = pBuf[i];
                mStatus = WaitType;
                i++;
            }
                break;
            case WaitType:
            {
                pPacket->PacketID = pBuf[i];
                mStatus = WaitLen;
                i++;
            }
                break;
            case WaitLen:
            {
                pPacket->Length = pBuf[i];
                if(pPacket->Length < PACKET_DATA_LEN_MAX)
                {
                    dataLen = 0;
                    if(pPacket->Length > 0)
                    {
                        mStatus = WaitData;
                    }
                    else
                    {
                        mStatus = WaitCRC;
                    }
                    i++;
                }
                else
                {
                    mStatus = WaitDstAddr;
                }
            }
                break;
            case WaitData:
            {
                if(dataLen < pPacket->Length)
                {
                    tempLen = (len-i >= pPacket->Length-dataLen) ? (pPacket->Length-dataLen) : (len-i);
                    memcpy(pPacket->DataAndCRC+dataLen, pBuf+i, tempLen);
                    dataLen += tempLen;
                    i += tempLen;
                }
                if(dataLen >= pPacket->Length)
                {
                    mStatus = WaitCRC;
                }
            }
                break;
            case WaitCRC:
            {
                pPacket->DataAndCRC[pPacket->Length] = pBuf[i];
                i++;

                mStatus = WaitDstAddr;
                if(pPacket->DataAndCRC[pPacket->Length] == crc8AioDspPkt(pPacket))
                {
                    *pBufLen = *pBufLen - i; 

                    //del used data
                    memcpy(pBuf, &pBuf[i], *pBufLen);
                    memset(&pBuf[i], 0x00, i);
                    return 1;
                }
            }
                break;
            default:
                mStatus = WaitDstAddr;
                break;
        }
    }
    return 0;
}

static void AioDspTryUnpackTask(void *pvParameters)
{
    int rLen = 0, rxOffset = 0;
    char rxBuf[256] = {0,};
    const TickType_t xTicksToWait = 5 / portTICK_PERIOD_MS;
    
	/* Just to stop compiler warnings. */
	( void ) pvParameters;
    
    INFO("TestedAIOTryUnpackTask running...\r\n");
    for (;;)
    {
        rLen = AioBoardRead(&rxBuf[rxOffset], sizeof(rxBuf)-rxOffset);
        if (rLen > 0)
        {
            rxOffset += rLen;
        }

        while (rxOffset > PACKET_FIXED_LENGHT)
        {
            if (tryUnpack(rxBuf, &rxOffset, &gCurrentRxPkt))
            {
                exePacket(&gCurrentRxPkt);
            }
        }
        
        vTaskDelay(xTicksToWait);
    }
}

static void InitAioDspPkt(AioDspProtocolPkt *pTxPacket)
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
    
    InitAioDspPkt(&packet);
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
    else //do nothing...
    {

    }
    return 0;
}
