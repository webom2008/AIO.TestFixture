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
 * macros                                       *
 *----------------------------------------------*/

#define PC_RX_BUFF_LEN_MAX        256

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
static char PCSerialRxBuf[PC_RX_BUFF_LEN_MAX] = {0,};
static int PCSerialRxOffset = 0;
/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

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
        || (0 != ( uxBits & UART_DMA_RX_INCOMPLETE_EVENT_BIT)))
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

u8 crc8ComputerPkt(const AioDspProtocolPkt *pPacket)
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
            if (TEST_ADDR == pPacket->DR_Addr){
                mStatus = WaitSrcAddr;
            }
        }
            break;
        case WaitSrcAddr:{
            pPacket->SR_Addr = pBuf[i++];
            if (PC_ADDR == pPacket->SR_Addr){
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
            if (pPacket->DataAndCRC[pPacket->Length] == crc8ComputerPkt(pPacket)){
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
        udprintf("PC-PKT CRC error !\r\n");
        return 0;
    }
    
    if ((i == len) &&(WaitDstAddr == mStatus)){
        memset(pBuf, 0x00, len);
        *pBufLen = 0;
    }
    return 0;
}

static void ComputerUnpackTask(void *pvParameters)
{
    int rLen = 0;
    const TickType_t xTicksToWait = 5 / portTICK_PERIOD_MS;
    int tryCountinue = 1;
    /* Just to stop compiler warnings. */
    ( void ) pvParameters;
    
    INFO("ComputerUnpackTask running...\n");
    for (;;)
    {
        rLen = ComputerRead(&PCSerialRxBuf[PCSerialRxOffset],
                            PC_RX_BUFF_LEN_MAX-PCSerialRxOffset);
        if (rLen > 0)
        {
            PCSerialRxOffset += rLen;
        }
        tryCountinue = 1;
        while ((PCSerialRxOffset > PACKET_FIXED_LENGHT) && (1 == tryCountinue))
        {
            tryCountinue = tryUnpack(PCSerialRxBuf, &PCSerialRxOffset, &gCurrentRxPkt);
            if (tryCountinue)
            {
                exePacket(&gCurrentRxPkt);
            }
        }
        
        vTaskDelay(xTicksToWait);
    }
}

int initComputerProtocol(void)
{
    xpReceiveQueueHandle = xQueueCreate(PC_RX_BUFF_LEN_MAX, sizeof(char));

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
    case COMP_ID_TASK_UTILITES:{
        AppTaskUtilitiesStart();
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
//        udprintf("PC:exePacket ID=0X%02X,CID=0X%02X\r\n",id,pPacket->DataAndCRC[0]);
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

