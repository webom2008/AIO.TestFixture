/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : ComputerProtocol.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/17
  Last Modified :
  Description   : computer protocol
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
extern EventGroupHandle_t xUart2RxEventGroup;

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
static QueueHandle_t    xpReceiveQueueHandle = NULL;

/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/

#define RECEIVE_QUEUE_LENTGH        256

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




static void ComputerReadDriverTask(void *pvParameters)
{
    int rLen = 0;
    char rxBuf[UART2_RX_DMA_BUF_LEN];
    char *pChar = NULL;
    EventBits_t uxBits;

    /* Just to stop compiler warnings. */
    ( void ) pvParameters;

    INFO("ComputerReadDriverTask running...\n");
    for (;;)
    {
        rLen = 0;
        uxBits = xEventGroupWaitBits(
                                    xUart2RxEventGroup,                 // The event group being tested.
                                    UART_DMA_RX_COMPLETE_EVENT_BIT \
                                    | UART_DMA_RX_INCOMPLETE_EVENT_BIT, // The bits within the event group to wait for.
                                    pdTRUE,                             // BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
                                    pdFALSE,                            // Don't wait for both bits, either bit will do.
                                    DELAY_MAX_WAIT );                   // Wait a maximum of 100ms for either bit to be set.

        memset(rxBuf, 0x00, UART2_RX_DMA_BUF_LEN);
        if (0 != ( uxBits & UART_DMA_RX_COMPLETE_EVENT_BIT ) \
        || (0 != ( uxBits & UART_DMA_RX_COMPLETE_EVENT_BIT)))
        {
            rLen = Uart2Read(rxBuf, UART4_RX_DMA_BUF_LEN);
            pChar = &rxBuf[0];
            while(rLen--)
            {
                xQueueSendToBack(xpReceiveQueueHandle, (void *)pChar++, DELAY_NO_WAIT);
            }
        }
    }
}

static int ComputerRead(char *pReadData, const int nDataLen)
{
    int i;
    
    for (i=0; i < nDataLen; i++)
    {
        if(pdPASS != xQueueReceive(xpReceiveQueueHandle, pReadData++, (TickType_t)10))
        {
            break;
        }
    }
    return i;
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
                if (TEST_ADDR == pBuf[i])
                {
                    pPacket->DR_Addr = pBuf[i];
                    mStatus = WaitSrcAddr;
                }
                i++;
            }
                break;
            case WaitSrcAddr:
            {
                if (PC_ADDR == pBuf[i])
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
                if(pPacket->DataAndCRC[pPacket->Length] == crc8ComputerPkt(pPacket))
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

static void ComputerUnpackTask(void *pvParameters)
{
    int rLen = 0, rxOffset = 0;
    char rxBuf[256] = {0,};
    const TickType_t xTicksToWait = 5 / portTICK_PERIOD_MS;

    /* Just to stop compiler warnings. */
    ( void ) pvParameters;
    
    INFO("ComputerUnpackTask running...\n");
    for (;;)
    {
        rLen = ComputerRead(&rxBuf[rxOffset], sizeof(rxBuf)-rxOffset);
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

int initComputerProtocol(void)
{
    xpReceiveQueueHandle = xQueueCreate(RECEIVE_QUEUE_LENTGH, sizeof(char));

    do {}while(NULL == xpReceiveQueueHandle);
    
    return 0;
}

int createComputerUnpackTask(void)
{
    while (pdPASS != xTaskCreate(   ComputerReadDriverTask,
                                    "ComputerReadDriverTask",
                                    configMINIMAL_STACK_SIZE,
                                    NULL,
                                    COMPUTER_DRIVER_TASK_PRIORITY,
                                    NULL));
    while (pdPASS != xTaskCreate(   ComputerUnpackTask,
                                    "ComputerUnpackTask",
                                    configMINIMAL_STACK_SIZE,
                                    NULL,
                                    COMPUTER_UNPACK_TASK_PRIORITY,
                                    NULL));
    return 0;
}

u8 crc8ComputerPkt(const AioDspProtocolPkt *pPacket)
{
    u8 len = 3 + (pPacket->Length);//3(PacketNum,PacketID,Length)+Data length
    u8 crc = crc8(&pPacket->PacketNum, len);
    return crc;
}

int sendComputerPkt(AioDspProtocolPkt *pAioDspPkt)
{
    int res = 0;
    res = Uart2Write((char *)pAioDspPkt, pAioDspPkt->Length + PACKET_FIXED_LENGHT);
    return res;
}

void initComputerPkt(AioDspProtocolPkt *pTxPacket)
{
    memset(pTxPacket, 0, sizeof(AioDspProtocolPkt));

    pTxPacket->DR_Addr = PC_ADDR;
    pTxPacket->SR_Addr = TEST_ADDR;
    pTxPacket->PacketID = AIO_TEST_FIXTURE_ID;
}


static int exeAioTestFixturePkt(AioDspProtocolPkt *pPacket)
{
    COMPUTER_PKT_CID cid = (COMPUTER_PKT_CID)pPacket->DataAndCRC[0];
    switch (cid)
    {
    case COMP_ID_VERSION:{

    }
        break;
    case COMP_ID_PWR_ALARM:{

    }
        break;
    default:
        break;
    }
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
        pPacket->DR_Addr = AIO_ADDR;
        pPacket->SR_Addr = MPU_ADDR;
        sendAioDspPkt(pPacket);
    }
    else if (AIO_TEST_FIXTURE_ID == id)
    {
        exeAioTestFixturePkt(pPacket);
    }
    else //do nothing...
    {

    }
    return 0;
}

