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
const char NORMAL_ACK[6] = {0xAA,0x55,0x02,0xF3,0x00,0xF5};

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/
#define TDMxUARTx                   UART4
#define TDMxRCC_Periph_UARTx        RCC_APB1Periph_UART4
#define TDMxRCC_Periph_UARTx_Tx     RCC_APB2Periph_GPIOC
#define TDMxRCC_Periph_UARTx_Rx     RCC_APB2Periph_GPIOC
#define TDMxRCC_Periph_UARTx_AFIO   RCC_APB2Periph_AFIO
#define TDMxPIN_UARTx_Tx            GPIO_Pin_10
#define TDMxPORT_UARTx_Tx           GPIOC
#define TDMxPIN_UARTx_Rx            GPIO_Pin_11
#define TDMxPORT_UARTx_Rx           GPIOC
#define TDMxBAUDRATE                115200

#define TRY_UART_MAX_COUNT          1000
/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

/*****************************************************************************
 Prototype    : TDMxInit
 Description  : init TDMx uart
 Input        : void  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2015/10/15
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
int TDMxInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB1PeriphClockCmd( TDMxRCC_Periph_UARTx,ENABLE );
    RCC_APB2PeriphClockCmd( TDMxRCC_Periph_UARTx_Tx \
                            | TDMxRCC_Periph_UARTx_Rx \
                            | TDMxRCC_Periph_UARTx_AFIO,
                            ENABLE );	

    /* Configure USARTx Rx as input floating */
    GPIO_InitStructure.GPIO_Pin = TDMxPIN_UARTx_Rx;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(TDMxPORT_UARTx_Rx, &GPIO_InitStructure );

    /* Configure USARTx Tx s alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = TDMxPIN_UARTx_Tx;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init( TDMxPORT_UARTx_Tx, &GPIO_InitStructure );
    return 0;
}

/*****************************************************************************
 Prototype    : TDMxOpen
 Description  : open TDMx uart
 Input        : void  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2015/10/15
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
int TDMxOpen(void)
{
    USART_InitTypeDef USART_InitStructure;
    
    USART_InitStructure.USART_BaudRate = TDMxBAUDRATE;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(TDMxUARTx, &USART_InitStructure );
    USART_Cmd(TDMxUARTx, ENABLE);
    return 0;
}

__inline static void clearUartRxFlag(void)
{
    uint16_t tmp;
    tmp = TDMxUARTx->SR;
    tmp = TDMxUARTx->DR;
    tmp = tmp;
}

static int sendAndWaitUartBuffer(const char *pBuf, const int nLen)
{
    int i = 0;

    clearUartRxFlag();
    for (i = 0; i < nLen; i++)
    {
        USART_SendData(TDMxUARTx, (uint8_t) pBuf[i]);
        while (RESET == USART_GetFlagStatus(TDMxUARTx, USART_FLAG_TC));
    }
    return i;
}

static void delay_us(const u32 delay)
{
    int i,j;
    for(i = 0; i < delay; i++)
    {
        for(j = 0; j < 10; j++){}
    }
}

static int getAndWaitUartBuffer(char *pBuf, const int nLen)
{
    int i = 0, try_cnt = 0;
    
//    if (SET == USART_GetFlagStatus(TDMxUARTx, USART_FLAG_ORE))
//    {
//        USART_ReceiveData(TDMxUARTx);
//        return -1;
//    }
    
    for (i = 0; i < nLen; i++)
    {
        while(RESET == USART_GetFlagStatus(TDMxUARTx, USART_FLAG_RXNE)\
            && (try_cnt < TRY_UART_MAX_COUNT))
        {
            delay_us(1);
            try_cnt++;
        }
        
        if (TRY_UART_MAX_COUNT == try_cnt) break;   //timeout
        else try_cnt = 0;
        
        pBuf[i] = (char)USART_ReceiveData(TDMxUARTx);   //read and clear flag by UART_DR
    }
    return i;
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

/*****************************************************************************
 Prototype    : waitForTDMxAck
 Description  : wait for TDMx Ack
 Input        : void  
 Output       : None
 Return Value : 0 : sucess, <0 : fail
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2015/9/18
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/

static int waitForTDMxAck(void)
{
    char ack[6] = {0,};
    int ret;

    ret = getAndWaitUartBuffer(ack, sizeof(ack));
    if ((sizeof(ack) == ret)&&(0 == memcmp(ack, NORMAL_ACK, 6)))
    {
        return 1;
    }
    return 0;
}


/*****************************************************************************
 Prototype    : TDMxWrite
 Description  : write cmd to TDMx
 Input        : const u8 cmd             
                const u8 *pCmdVal2Write  
                const u8 nCmdValLen      
 Output       : None
 Return Value : 0: success, <0:fail
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2015/9/18
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
int TDMxWrite(const u8 cmd, const u8 *pCmdVal2Write, const u8 nCmdValLen)
{
    TDMxUartPkt pkt;
    u8 len;
    u16 checksum = 0;
    
    initPkt(&pkt);
    pkt.cmd = cmd;
    if (len > 0)
    {
        memcpy(pkt.cmd_val_checksum, pCmdVal2Write, len);
    }
    pkt.cmd_len = len + 2;                              //1byte(cmd_len)+1byte(cmd)+Nbytes(cmd_val)
    checksum = calcPktChecksum(&pkt);
    pkt.cmd_val_checksum[len] = (u8)(checksum >> 8);    //MSB
    pkt.cmd_val_checksum[len+1] = (u8)checksum;         //LSB

    sendAndWaitUartBuffer((char *)&pkt, pkt.cmd_len + 4);          //2bytes header + 2bytes checksum

    if ((CMD_CONNECT == cmd) \
        || (CMD_UNCONNECT == cmd) \
        || (CMD_SET_DP_SEL == cmd) \
        || (CMD_SET_SPS == cmd) \
        || (CMD_SET_BAUD == cmd)\
        || (CMD_SET_RANGE == cmd))
    {
        return waitForTDMxAck();
    }
    
    return 0; // normal return
}

static int getCurrentData(int *pData)
{
    int ret = 0;
    TDMxUartPkt ack;
    const char GET_CUR_DATA_FRAME[6] = {0xAA, 0x55, 0x02, 0xFE, 0x01, 0x00};
    u16 checksum = 0;

    sendAndWaitUartBuffer(GET_CUR_DATA_FRAME, sizeof(GET_CUR_DATA_FRAME));
    ret = getAndWaitUartBuffer((char *)&ack, 8);
    if (8 == ret)
    {
        checksum = (u16)((ack.cmd_val_checksum[2] << 8) | ack.cmd_val_checksum[3]);
        if (checksum == calcPktChecksum(&ack))
        {
            *pData = (int)((ack.cmd_val_checksum[1] << 8) | ack.cmd_val_checksum[0]);
            return 0;
        }
    }
    return -1;
}

int TDMxCtrl(const TDMxCtrlCmd_TypeDef cmd, void *arg)
{
    int ret = 0;
    switch (cmd)
    {
    case TDMxCTRL_GET_DATA:{
        ret = getCurrentData((int *)arg);
    } break;
    default:
        break;
    }
    return ret;
}

int TDMxClose(void)
{
    USART_Cmd(TDMxUARTx, DISABLE);
    return 0;
}

