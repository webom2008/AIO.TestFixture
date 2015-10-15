/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : MainMcuProtocol.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/15
  Last Modified :
  Description   : MainMcuProtocol.c header file
  Function List :
  History       :
  1.Date        : 2015/9/15
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/

#ifndef __MAINMCUPROTOCOL_H__
#define __MAINMCUPROTOCOL_H__


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
    PKT_ID_TDM_RESULT,

    PKT_ID_UNVALID
} MainMCUPacketID_DEF;

int MainMcuProtocolInit(void);

int deleteMainMcuPkt(const u8 ID);

int checkAndResendMainMcuACKPkt(void);

int sendMainMcuPkt( DmaUartProtocolPacket *pTxPacket, const u32 timeout_ms);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __MAINMCUPROTOCOL_H__ */
