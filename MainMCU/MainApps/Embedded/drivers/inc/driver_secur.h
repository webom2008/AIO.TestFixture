/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : driver_secur.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/10/14
  Last Modified :
  Description   : driver_secur.c header file
  Function List :
  History       :
  1.Date        : 2015/10/14
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/


#ifndef __DRIVER_SECUR_H__
#define __DRIVER_SECUR_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define SECURINFO_HEAD      ((int)(0xA5A5A5A5))

//注意:大小端对齐问题
typedef struct
{
    int info_head;      //0xA5A5A5A5
    int flag;           //1:valid   0:unvalid
    int download_cnt; 
    
    u8  crc_reserved[3];
    u8  crc;            //CRC8
} SecurInfo_TypeDef;


typedef enum
{
    /*Write Operation */
    SECUR_CTRL_W_RESET_ALL   = 0,
    SECUR_CTRL_W_RESET_DOWNLOAD_CNT,
    SECUR_CTRL_W_DEC_DOWNLOAD_CNT,      //Decrease download count
    
    /*Read Operation */
    SECUR_CTRL_R_DOWNLOAD_CNT,
    
    SECUR_CTRL_W_RESERVED               //Reserved 
} SecurCmd_TypeDef;

int SecurFlashInit(void);
int SecurFlashOpen(void);
int SecurFlashRead(SecurInfo_TypeDef *pInfo);
int SecurFlashWrite(SecurInfo_TypeDef *pInfo);
int SecurFlashCtrl(const SecurCmd_TypeDef cmd, void *arg);
int SecurFlashClose(void);

int exeSecurFlashPkt(void *arg);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DRIVER_SECUR_H__ */
