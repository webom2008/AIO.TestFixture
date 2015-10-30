/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : sysconfigs.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/3
  Last Modified :
  Description   : sysconfigs.h header file
  Function List :
  History       :
  1.Date        : 2015/9/3
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/

#ifndef __SYSCONFIGS_H__
#define __SYSCONFIGS_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */



//#define CONFIG_UART1_DMA_MODE
#define CONFIG_UART1_INT_MODE

#define CONFIG_UART4_DMA_MODE
//#define CONFIG_UART4_INT_MODE

// config for drivers test task 
//#define CONFIG_DRIVER_TEST_UART1
//#define CONFIG_DRIVER_TEST_UART2
//#define CONFIG_DRIVER_TEST_UART3
//#define CONFIG_DRIVER_TEST_UART4
//#define CONFIG_DRIVER_TEST_UART5


//#define CONFIG_BTN_TASK_UTILITIES




#define SKIP_CHECK_DPM2200_CONNECT

#define SKIP_STATE_DETECT_OTHER_POWER
#define SKIP_STATE_DOWNLOAD_AIOSTM_BOOT
#define SKIP_STATE_DOWNLOAD_AIODSP_APP
#define SKIP_STATE_DOWNLOAD_AIOSTM_APP
#define SKIP_STATE_NIBP_VERIFY
#define SKIP_STATE_AIOBOARD_MAX_CURRENT
#define SKIP_STATE_NIBP_GAS_CONTROL


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __SYSCONFIGS_H__ */

