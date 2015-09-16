/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : CoopMcuProtocol.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/16
  Last Modified :
  Description   : CoopMcuProtocol.c header file
  Function List :
  History       :
  1.Date        : 2015/9/16
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#ifndef __COOPMCUPROTOCOL_H__
#define __COOPMCUPROTOCOL_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define ACK_PKT_TIMEOUT_TRY_MAX         3

typedef enum
{
    PKT_ID_RESERVED         = 0x00,
    PKT_ID_DRIVER_TEST,
    PKT_ID_AIOSTM_UPDATE_START,
    PKT_ID_AIOSTM_UPDATE_ERROR,
    PKT_ID_AIOSTM_UPDATE_BOOT,
    PKT_ID_AIOSTM_UPDATE_END,

    PKT_ID_UNVALID
} CoopMCUPacketID_DEF;


#define DMA_UART_START_HEADER_TAG       (u8)0xA5
#define DMA_UART_END_HEADER_TAG         (u8)0x5A
#define DMA_UART_PACKET_NACK            (u8)0x00
#define DMA_UART_PACKET_ACK             (u8)0x01
#define DMA_UART_PACKET_PARITY_OK       (u8)0x00
#define DMA_UART_PACKET_PARITY_ERR      (u8)0x01


typedef struct 
{ 
    u8 StartHeader;     //Start Header Tag
    u8 ID;              //Packet ID
    u8 DataLen;         //Packet Data Length
    u8 Data[26];        //Packet Data
    u8 ACK;             //Tag for ack or nack
    u8 ParityTag;       //Tage for Parity result OK or Error
    u8 EndHeader;       //End Header Tag
} DmaUartProtocolPacket;

int DmaUartProtocolPacketInit(DmaUartProtocolPacket *pPacket);

int CoopMcuProtocolInit(void);

int deleteCoopMcuAckPkt(const u8 ID);

int checkAndResendCoopMcuACKPkt(void);

int sendCoopMcuPkt(DmaUartProtocolPacket *pTxPacket, const u32 timeout_ms);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __COOPMCUPROTOCOL_H__ */
