/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : driver_tdm.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/17
  Last Modified :
  Description   : driver_tdm.c header file
  Function List :
  History       :
  1.Date        : 2015/9/17
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#ifndef __DRIVER_TDM_H__
#define __DRIVER_TDM_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define TDMX_UART_PKT_HEADER1           0xAA
#define TDMX_UART_PKT_HEADER2           0x55

typedef struct _TDMXUARTPKT_
{
    u8 header1;
    u8 header2;
    u8 cmd_len;
    u8 cmd;
    u8 cmd_val_checksum[8]; //date + 2bytes checksum
} TDMxUartPkt;


//部分控制及应答指令
#define CMD_CONNECT         0xF1    /* 控制连接(适用于连续读取方式) */
#define CMD_UNCONNECT       0xF2    /* 控制断开 */
#define CMD_ACK             0xF3    /* 控制应答 */
#define CMD_GET_INFO        0xF4    /* 读取量程信息 */
#define CMD_RET_INFO        0xF5    /* 量程信息返回 */
#define CMD_RET_DATA        0xF6    /* 测量数据返回 */
#define CMD_SET_DP_SEL      0xF7    /* 小数点位置设置 */
#define CMD_SET_SPS         0xF8    /* 采样速率设置 */
#define CMD_SET_BAUD        0xF9    /* 更改通信波特率(重新上电后生效) */
#define CMD_GET_DATA        0xFE    /* 读取测量值(适用于单次读取方式) */
#define CMD_SET_RANGE       0xA1    /* 更改换算量程(仅限于 75mV 分流器专配表和三位半全量程电阻表) */

//量程码
#define RANGE_1KHZ          0x7D
#define RANGE_10KHZ         0x7E
#define RANGE_100KHZ        0x7F
#define RANGE_2MOhm         0xA8
#define RANGE_200KOhm       0xA9
#define RANGE_20KOhm        0xAA
#define RANGE_2KOhm         0xAB
#define RANGE_200Ohm        0xAC
#define RANGE_1000A         0xAD
#define RANGE_900A          0xAE
#define RANGE_800A          0xAF
#define RANGE_750A          0xB0
#define RANGE_600A          0xB1
#define RANGE_500A          0xB2
#define RANGE_400A          0xB3
#define RANGE_300A          0xB4
#define RANGE_100A          0xB5
#define RANGE_10A           0xB6
#define RANGE_30A           0xB7
#define RANGE_40A           0xB8
#define RANGE_50A           0xB9
#define RANGE_60A           0xBA
#define RANGE_75A           0xBB
#define RANGE_80A           0xBC
#define RANGE_90A           0xBD
#define RANGE_20A           0xBE
#define RANGE_200A          0xBF
#define RANGE_2V            0xC1
#define RANGE_20V           0xC2
#define RANGE_20mV          0xC3
#define RANGE_200V          0xC4
#define RANGE_200mV         0xC5
#define RANGE_4V            0xC6
#define RANGE_40V           0xC7
#define RANGE_40mV          0xC8
#define RANGE_400V          0xC9
#define RANGE_400mV         0xCA
#define RANGE_5V            0xCB
#define RANGE_50V           0xCC
#define RANGE_50mV          0xCD
#define RANGE_500V          0xCE
#define RANGE_500mV         0xCF
#define RANGE_6V            0xD0
#define RANGE_60V           0xD1
#define RANGE_60mV          0xD2
#define RANGE_600V          0xD3
#define RANGE_600mV         0xD4
#define RANGE_2A            0xD5
#define RANGE_2mA           0xD6
#define RANGE_20mA          0xD7
#define RANGE_200mA         0xD8
#define RANGE_200uA         0xD9
#define RANGE_4mA           0xDA
#define RANGE_40mA          0xDB
#define RANGE_400mA         0xDC
#define RANGE_400uA         0xDD
#define RANGE_5mA           0xDE
#define RANGE_50mA          0xDF
#define RANGE_500mA         0xE0
#define RANGE_500uA         0xE1
#define RANGE_6mA           0xE2
#define RANGE_60mA          0xE3
#define RANGE_600mA         0xE4
#define RANGE_600uA         0xE5
#define RANGE_4A            0xE6
#define RANGE_5A            0xE7
#define RANGE_6A            0xE8
#define RANGE_2KV           0xE9
#define RANGE_NKV           0xEA
#define RANGE_2mV           0xEB
#define RANGE_20uA          0xEB
#define RANGE_2KA           0xED
#define RANGE_NKA           0xEE
#define RANGE_700V          0xEF

//类别码
#define CLASS_DC_FOUR_HALF  0x11 /* 直流四位半表头 */
#define CLASS_DC_THREE_HALF 0x12 /* 直流三位半表头 */
#define CLASS_AC_FOUR_HALF  0x21 /* 交流四位半表头 */
#define CLASS_AC_THREE_HALF 0x22 /* 交流三位半表头 */



typedef enum
{
    TDMxCTRL_GET_DATA   = 0,

    TDMxCTRL_RESERVED             //Reserved 
} TDMxCtrlCmd_TypeDef;





int TDMxInit(void);
int TDMxOpen(void);
int TDMxWrite(const u8 cmd, const u8 *pCmdVal2Write, const u8 nCmdValLen);
int TDMxCtrl(const TDMxCtrlCmd_TypeDef cmd, void *arg);
int TDMxClose(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DRIVER_TDM_H__ */
