/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : MainMcuProtocol.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/15
  Last Modified :
  Description   : main mcu uart protocol
  Function List :
  History       :
  1.Date        : 2015/9/15
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

/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/
typedef struct _NeedAckPkt_StructDef_
{
    u8 idle_flag;
    u8 try_cnt;
    u32 timeoutTick;
    DmaUartProtocolPacket pkt;
} NeedAckPkt_StructDef;

#define ACK_PKT_BUFFER_MAX_SIZE         20     
NeedAckPkt_StructDef gNeedAckPktBuffer[ACK_PKT_BUFFER_MAX_SIZE];

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/
#define IDLE_FLAG_USED          (u8)0x01
#define IDLE_FLAG_UNUSED        (u8)0x00

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

int MainMcuProtocolInit(void)
{
    int ret = 0;
    memset(gNeedAckPktBuffer, 0x00, sizeof(gNeedAckPktBuffer));
    return ret;
}

int deleteMainMcuAckPkt(const u8 ID)
{
    int i;
    for (i = 0; i < ACK_PKT_BUFFER_MAX_SIZE; i++)
    {
        if ((IDLE_FLAG_USED == gNeedAckPktBuffer[i].idle_flag) \
            && (ID == gNeedAckPktBuffer[i].pkt.ID))
        {
            gNeedAckPktBuffer[i].try_cnt = 0;
            gNeedAckPktBuffer[i].idle_flag = IDLE_FLAG_UNUSED;
            gNeedAckPktBuffer[i].pkt.ID = PKT_ID_UNVALID;
        }
    }
    return 0;
}

static int addMainMcuAckPkt(    const DmaUartProtocolPacket *pPacket, 
                                const u32 timeoutTick)
{
    int i;
    NeedAckPkt_StructDef AckPkt;
    AckPkt.idle_flag = IDLE_FLAG_USED;
    AckPkt.pkt = *pPacket;
    AckPkt.timeoutTick = timeoutTick;

    // Packet had in buffer yet. change try count
    for (i = 0; i < ACK_PKT_BUFFER_MAX_SIZE; i++)
    {
        if ((pPacket->ID == gNeedAckPktBuffer[i].pkt.ID) \
            && (IDLE_FLAG_USED == gNeedAckPktBuffer[i].idle_flag))
        {
            gNeedAckPktBuffer[i].try_cnt++;
            if (gNeedAckPktBuffer[i].try_cnt > ACK_PKT_TIMEOUT_TRY_MAX)
            {
                //NOTE: timout over max try count, delte timeout pkt
                deleteMainMcuAckPkt(pPacket->ID);
            }
            return 0;
        }
    }

    // Packet hadn't in buffer yet. create a new one
    for (i = 0; i < ACK_PKT_BUFFER_MAX_SIZE; i++)
    {
        if (IDLE_FLAG_UNUSED == gNeedAckPktBuffer[i].idle_flag)
        {
            gNeedAckPktBuffer[i] = AckPkt;
            return 0;
        }
    }

    // Not enough buffer space
    return -1;
}

int checkAndResendMainMcuACKPkt(void)
{
    int i;
    for (i = 0; i < ACK_PKT_BUFFER_MAX_SIZE; i++)
    {
        if ((IDLE_FLAG_USED == gNeedAckPktBuffer[i].idle_flag) \
            && (1 == IsMyTimerOnTime(gNeedAckPktBuffer[i].timeoutTick)))
        {
            sendMainMcuPkt(gNeedAckPktBuffer[i].pkt.ID,
                gNeedAckPktBuffer[i].pkt.Data,
                gNeedAckPktBuffer[i].pkt.DataLen,
                gNeedAckPktBuffer[i].pkt.ACK,
                gNeedAckPktBuffer[i].timeoutTick);
        }
    }
    return 0;
}

int sendMainMcuPkt( const u8 ID,
                    const u8 *pSendData,
                    const u8 DataLen,
                    const u8 Ack,
                    const u32 timeout_ms)
{
    DmaUartProtocolPacket txPacket;
    u32 tick;
    
    if (DataLen > sizeof(txPacket.Data))
    {
        return -1;
    }
    
    DmaUartProtocolPacketInit(&txPacket);
    txPacket.ID = ID;
    txPacket.DataLen = DataLen;
    txPacket.ACK = Ack;
    if (DataLen > 0)
    {
        memcpy(txPacket.Data, pSendData, DataLen);
    }
    
    Uart2Write((char *)&txPacket, sizeof(DmaUartProtocolPacket));
    if (DMA_UART_PACKET_ACK == Ack)
    {
        tick = timeout_ms / MY_TIM_TICK_PERIOD_MS;
        tick = MyMaxi(tick, 2);
        tick = tick + getMyTimerTick();
        addMainMcuAckPkt(&txPacket, tick);
    }
    return 0;
}

