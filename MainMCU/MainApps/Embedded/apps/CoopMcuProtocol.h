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


int CoopMcuProtocolInit(void);

int deleteCoopMcuPkt(const u8 ID);

int checkAndResendCoopMcuACKPkt(void);

int sendCoopMcuPkt(     const u8 ID, 
                        const u8 *pSendData, 
                        const u8 DataLen, 
                        const u8 Ack, 
                        const u32 timeout_ms);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __COOPMCUPROTOCOL_H__ */
