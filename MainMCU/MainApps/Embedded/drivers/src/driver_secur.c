/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : driver_secur.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/10/14
  Last Modified :
  Description   : secur function
  Function List :
  History       :
  1.Date        : 2015/10/14
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
static SecurInfo_TypeDef gSecurInfo;

/*----------------------------------------------*
 * module-wide global variables                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/
static const uint32_t INFO_PAGE_START  = 0x0807f800;
//static const uint32_t INFO_PAGE_END    = 0x0807ffff; //2Kbytes
static const int DOWNLOAD_COUNT_MAX    = 2000;
/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/

//#define _INFO_
#define _ERROR_

#ifdef _INFO_
#define INFO(fmt, arg...) udprintf("[Secur]Info: "fmt, ##arg)
#else
#define INFO(fmt, arg...) do{}while(0)
#endif
#ifdef _ERROR_
#define ERROR(fmt, arg...) udprintf("[Secur]Error: "fmt, ##arg)
#else
#define ERROR(fmt, arg...) do{}while(0)
#endif
/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

int SecurFlashInit(void)
{
    u8 crc;
    int len;
    
    gSecurInfo = *(SecurInfo_TypeDef *)INFO_PAGE_START;
    
    len = (u32)&gSecurInfo.crc - (u32)&gSecurInfo;
    //check crc
    crc = crc8((u8 *)&gSecurInfo, (u8)len);

    INFO("SecurFlashInit crc8=0x%02x read=0x%02x len=%d\r\n",
        crc, gSecurInfo.crc, len);
    
    if ((crc == gSecurInfo.crc) && (SECURINFO_HEAD == gSecurInfo.info_head))
    {
        gSecurInfo.flag = S32_FLAG_TRUE;
    }
    else
    {
        memset(&gSecurInfo, 0x00, sizeof(SecurInfo_TypeDef));
        gSecurInfo.flag = S32_FLAG_FALT;
    }

    SecurFlashCtrl(SECUR_CTRL_R_DOWNLOAD_CNT, &len);
    return 0;
}

int SecurFlashOpen(void)
{
    return 0;
}

int SecurFlashRead(SecurInfo_TypeDef *pInfo)
{
    *pInfo = gSecurInfo;
    return 0;
}

int SecurFlashWrite(SecurInfo_TypeDef *pInfo)
{
    int         i       = 0;
    uint16_t    *pData  = (uint16_t *)pInfo;
    uint32_t    start   = INFO_PAGE_START;

    RCC_HSICmd(ENABLE);
    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
    FLASH_ErasePage(INFO_PAGE_START);

    for (i = 0; i < sizeof(SecurInfo_TypeDef)/2; i++,pData++)
    {
        start = INFO_PAGE_START+i*2;
        FLASH_ProgramHalfWord(start, *pData);
        INFO("[0x%08x]0x%04x \r\n", start, *pData);
    }
    
    FLASH_Lock();
    INFO("SecurFlashWrite count=%d\r\n",pInfo->download_cnt);
    return 0;
}

static void resetSecureFlashAll(void)
{
    u32 len;
    SecurInfo_TypeDef info;
    SecurFlashRead(&info);
    
    info.info_head = SECURINFO_HEAD;
    info.flag = S32_FLAG_TRUE;
    info.download_cnt = DOWNLOAD_COUNT_MAX;
    info.crc_reserved[0] = 0xFF;
    info.crc_reserved[1] = 0xFF;
    info.crc_reserved[2] = 0xFF;

    //calc crc
    len = (u32)&info.crc - (u32)&info;
    info.crc = crc8((u8 *)&info, (u8)len);

    INFO("resetSecureFlashAll crc8=0x%02x len=%d\r\n",info.crc, len);
    
    SecurFlashWrite(&info);

    SecurFlashInit();
}

static void saveDownloadCount(const u16 count)
{
    u32 len;
    SecurInfo_TypeDef info;
    
    SecurFlashRead(&info);
    
    info.download_cnt = count;
    //calc crc
    len = (u32)&info.crc - (u32)&info;
    info.crc = crc8((u8 *)&info, (u8)len);

    INFO("saveDownloadCount crc8=0x%02x len=%d\r\n",info.crc, len);
    
    SecurFlashWrite(&info);
    
    SecurFlashInit();
}

static void decreaseDownloadCount(void)
{
    u16 count_cur = gSecurInfo.download_cnt;
    if (count_cur > 0)
    {
        count_cur--;
    }
    else
    {
        count_cur = 0;
    }
    saveDownloadCount(count_cur);
}

static void resetDownloadCount(void)
{
    saveDownloadCount(DOWNLOAD_COUNT_MAX);
}

static int readDownloadCount(int *pCountLeft)
{
    if (U8_FLAG_TRUE == gSecurInfo.flag)
    {
        *pCountLeft = gSecurInfo.download_cnt;
        LedsDisplayWrite(gSecurInfo.download_cnt);
        return 0;
    }
    return -1;
}

int SecurFlashCtrl(const SecurCmd_TypeDef cmd, void *arg)
{
    int ret = 0;
    switch (cmd)
    {
    case SECUR_CTRL_W_RESET_ALL:{
        resetSecureFlashAll();
    }
        break;
    case SECUR_CTRL_W_RESET_DOWNLOAD_CNT:{
        resetDownloadCount();
    }
        break;
    case SECUR_CTRL_W_DEC_DOWNLOAD_CNT:{
        decreaseDownloadCount();
    }
        break;
    case SECUR_CTRL_R_DOWNLOAD_CNT:{
        readDownloadCount((int *)arg);
    }
        break;
    default:
        break;
    }
    return ret;
}

int SecurFlashClose(void)
{
    return 0;
}

int exeSecurFlashPkt(void *arg)
{
    AioDspProtocolPkt *pPacket = (AioDspProtocolPkt *)arg;
    switch ((SecurCmd_TypeDef)pPacket->DataAndCRC[1])
    {
    case SECUR_CTRL_W_RESET_ALL:
    case SECUR_CTRL_W_RESET_DOWNLOAD_CNT:
    case SECUR_CTRL_W_DEC_DOWNLOAD_CNT:{
        SecurFlashCtrl((SecurCmd_TypeDef)pPacket->DataAndCRC[1], NULL);
        INFO("exeSecurFlashPkt...\r\n");
    }
        break;
    default :
        break;
    }
    return 0;
}

