/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : Spo2Protocol.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/17
  Last Modified :
  Description   : spo2 uart protocol
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
static Spo2ProtocolPkt gCurrentRxPkt;

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
#define INFO(fmt, arg...) udprintf("[SPO2]Info: "fmt, ##arg)
#else
#define INFO(fmt, arg...) do{}while(0)
#endif
#ifdef _ERROR_
#define ERROR(fmt, arg...) udprintf("[SPO2]Error: "fmt, ##arg)
#else
#define ERROR(fmt, arg...) do{}while(0)
#endif
/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/
static int exePacket(Spo2ProtocolPkt *pPacket);


static u8 crc8(const u8 *ptr, u8 len)
{
	u8 crc;
	u8 i;
	crc = 0;
	while(len--)
	{
		crc ^= *ptr++;
		for(i = 0; i < 8; i++)
		{
			if(crc&0x01)
			{
				crc = (crc >> 1) ^ 0x8C;
			}	
			else
			{
				crc >>= 1;
			}
		}	
	}
	return crc;
}


static u8 crc8AioDspPkt(const Spo2ProtocolPkt *pPacket)
{
    u8 len = 3 + (pPacket->len);//3(PacketNum,PacketID,Length)+Data length
    u8 crc = crc8(&pPacket->pkt_seq, len);
    return crc;
}


static int tryUnpack(char *pBuf, int *pBufLen, Spo2ProtocolPkt *pPacket)
{
    int dataLen, len = *pBufLen, tempLen = 0, i;
    static int mStatus = WaitDstAddr;
    
    for (i=0; i<len;)
    {
        switch(mStatus)
        {
            case WaitDstAddr:
            {
                if (SPO2_ADDR == pBuf[i])
                {
                    pPacket->dst_addr = pBuf[i];
                    mStatus = WaitSrcAddr;
                }
                i++;
            }
                break;
            case WaitSrcAddr:
            {
                if (AIO_ADDR == pBuf[i])
                {
                    pPacket->src_addr = pBuf[i];
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
                pPacket->pkt_seq = pBuf[i];
                mStatus = WaitType;
                i++;
            }
                break;
            case WaitType:
            {
                pPacket->pkt_id = pBuf[i];
                mStatus = WaitLen;
                i++;
            }
                break;
            case WaitLen:
            {
                pPacket->len= pBuf[i];
                if(pPacket->len < sizeof(pPacket->data_crc))
                {
                    dataLen = 0;
                    if(pPacket->len > 0)
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
                if(dataLen < pPacket->len)
                {
                    tempLen = (len-i >= pPacket->len-dataLen) ? (pPacket->len-dataLen) : (len-i);
                    memcpy(pPacket->data_crc+dataLen, pBuf+i, tempLen);
                    dataLen += tempLen;
                    i += tempLen;
                }
                if(dataLen >= pPacket->len)
                {
                    mStatus = WaitCRC;
                }
            }
                break;
            case WaitCRC:
            {
                pPacket->data_crc[pPacket->len] = pBuf[i];
                i++;

                mStatus = WaitDstAddr;
                if(pPacket->data_crc[pPacket->len] == crc8AioDspPkt(pPacket))
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


static void Spo2TryUnpackTask(void *pvParameters)
{
    int rLen = 0, rxOffset = 0;
    char rxBuf[256] = {0,};
    const TickType_t xTicksToWait = 50 / portTICK_PERIOD_MS;
    
	/* Just to stop compiler warnings. */
	( void ) pvParameters;
    
    INFO("Spo2TryUnpackTask running...\n");
    for (;;)
    {
        rLen = Uart5Read(&rxBuf[rxOffset], sizeof(rxBuf)-rxOffset);
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

static void InitSpo2Packet(Spo2ProtocolPkt *pTxPacket)
{
    memset(pTxPacket, 0, sizeof(Spo2ProtocolPkt));

    pTxPacket->dst_addr = AIO_ADDR;
    pTxPacket->src_addr = SPO2_ADDR;
}


int sendAioDspPkt(Spo2ProtocolPkt *pAioDspPkt)
{
    int res = 0;
    res = Uart5Write((char *)pAioDspPkt, PACKET_TOTAL_LENGHT);
    return res;
}

static void getSpo2Verison(void)
{
    const char verion[] = "VER:SPO2-TEST";
    static u8 count = 0;
    Spo2ProtocolPkt pkt;
    InitSpo2Packet(&pkt);

    pkt.pkt_seq = count++;
    pkt.pkt_id = SpO2_MODEL_VERSION_ID;
    pkt.len = strlen(verion);
    memcpy(pkt.data_crc, verion, pkt.len);
    pkt.data_crc[pkt.len] = crc8AioDspPkt(&pkt);
    sendAioDspPkt(&pkt);
}

static int exePacket(Spo2ProtocolPkt *pPacket)
{
    UART_PacketID id = (UART_PacketID)pPacket->pkt_id;
    
    if (SpO2_MODEL_VERSION_ID == id)
    {
        getSpo2Verison();
    }
    else //do nothing...
    {

    }
    return 0;
}


int createSpo2ProtocolTask(void)
{
#ifndef CONFIG_DRIVER_TEST_UART5
    while (pdPASS != xTaskCreate(   Spo2TryUnpackTask,
                                    "Spo2TryUnpackTask",
                                    configMINIMAL_STACK_SIZE,
                                    NULL,
                                    SPO2_UNPACK_TASK_PRIORITY,
                                    NULL));
#endif /* CONFIG_DRIVER_TEST_UART5 */
    return 0;
}

