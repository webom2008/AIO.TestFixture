/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : ComputerProtocol.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/17
  Last Modified :
  Description   : ComputerProtocol.c header file
  Function List :
  History       :
  1.Date        : 2015/9/17
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/

#ifndef __COMPUTERPROTOCOL_H__
#define __COMPUTERPROTOCOL_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

typedef enum
{
    COMP_ID_VERSION         = 0x00,
    COMP_ID_PWR_ALARM       = 0x01,
    COMP_ID_TASK_UTILITES   = 0x02,
    COMP_ID_DOWNLOAD_CNT    = 0x03,
    COMP_ID_ERROR_INFO      = 0x04,

    COMP_ID_UNVALID,
} COMPUTER_PKT_CID;


typedef enum
{
    ERR_INFO_ID_TEST_END        = 0x00,
    ERR_INFO_ID_D3V3E_PWR       = 0x01,
    ERR_INFO_ID_OTHER_PWR       = 0x02,

    ERR_INFO_ID_UNVALID,
} ERROR_INFO_CID;

int initComputerProtocol(void);

void initComputerPkt(AioDspProtocolPkt *pTxPacket);

u8 crc8ComputerPkt(const AioDspProtocolPkt *pPacket);

int sendComputerPkt(AioDspProtocolPkt *pAioDspPkt);

int createComputerUnpackTask(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __COMPUTERPROTOCOL_H__ */
