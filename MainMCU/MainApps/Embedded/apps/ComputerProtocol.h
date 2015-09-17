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

int initComputerProtocol(void);

int sendComputerPkt(AioDspProtocolPkt *pAioDspPkt);

int createComputerUnpackTask(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __COMPUTERPROTOCOL_H__ */
