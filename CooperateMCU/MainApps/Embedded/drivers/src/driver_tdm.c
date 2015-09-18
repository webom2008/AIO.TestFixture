/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : driver_tdm.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/17
  Last Modified :
  Description   : tdm driver
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

/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/


/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

int TDMxInit(void)
{
    return 0;
}

int TDMxOpen(void)
{
    return 0;
}


static void initPkt(TDMxUartPkt *pPacket)
{
    memset((void *)pPacket, 0x00, sizeof(TDMxUartPkt));
    pPacket->header1 = TDMX_UART_PKT_HEADER1;
    pPacket->header2 = TDMX_UART_PKT_HEADER2;
}


static u16 calcPktChecksum(TDMxUartPkt *pPacket)
{
    int i;
    u16 checksum = 0;
    u8 *pStart = &pPacket->cmd_len;
    
    for (i = 0; i < pPacket->cmd_len; i++)
    {
        checksum += *pStart;
        pStart++;
    }
    return checksum;
}

static void cleanUartRxBuffer(void)
{
    int rLen = 0;
    char rBuf[10];
    do
    {
        rLen = Uart4Read(rBuf, sizeof(rBuf));
    }while(0 != rLen);
}

int TDMxRead(u8 *pCmdVal2Read, u8 *pCmdValLen)
{
    TDMxUartPkt pkt;
    int i, len = 0;
    char *pStart = (char *)pkt.cmd_val_checksum;
    u16 checksum = 0;
    char tmp;

    if (1 != Uart4Read(&pkt.header1, 1)) return -1;
    if (1 != Uart4Read(&pkt.header2, 1)) return -1;

    if ((TDMX_UART_PKT_HEADER1 == pkt.header1) \
        && (TDMX_UART_PKT_HEADER2 == pkt.header2))
    {
        if (1 != Uart4Read(&pkt.cmd_len, 1)) return -1;
        if (1 != Uart4Read(&pkt.cmd, 1)) return -1;

        len = MyMini(pkt.cmd_len - 2, TDMX_UART_CMD_VALUE_MAX);
        for (i = 0; i < len; )
        {
            if (1 != Uart4Read(pStart++, 1)) return -1;
        }

        if (1 != Uart4Read(&tmp, 1)) return -1;
        checksum = (tmp << 8);
        if (1 != Uart4Read(&tmp, 1)) return -1;
        checksum += tmp;

        if (checksum == calcPktChecksum(&pkt))
        {
            //Pkt OK
            *pCmdValLen = pkt.cmd_len - 2;
            memcpy(pCmdVal2Read, pkt.cmd_val_checksum, *pCmdValLen);
            return 0;
        }
    }
    else
    {
        return -2;
    }
}

int TDMxWrite(const u8 cmd, const u8 *pCmdVal2Write, const u8 nCmdValLen)
{
    int i;
    TDMxUartPkt pkt;
    u8 len = MyMini(nCmdValLen, TDMX_UART_CMD_VALUE_MAX);
    u16 checksum = 0;
    
    initPkt(&pkt);
    pkt.cmd = cmd;
    memcpy(pkt.cmd_val_checksum, pCmdVal2Write, len);
    pkt.cmd_len = len + 2;                              //1byte(cmd_len)+1byte(cmd)+Nbytes(cmd_val)
    checksum = calcPktChecksum(&pkt);
    pkt.cmd_val_checksum[len] = (u8)(checksum >> 8);    //MSB
    pkt.cmd_val_checksum[len+1] = (u8)checksum;         //LSB

    cleanUartRxBuffer();
    Uart4Write((char *)&pkt, pkt.cmd_len + 4);          //2bytes header + 2bytes checksum
    return 0;
}

int TDMxCtrl(void)
{
    return 0;
}

int TDMxClose(void)
{
    return 0;
}

