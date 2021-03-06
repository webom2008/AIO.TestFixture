/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : defines.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/4/8
  Last Modified :
  Description   : defines.h header file
  Function List :
  History       :
  1.Date        : 2015/4/8
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#ifndef __DEFINES_H__
#define __DEFINES_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


#include "sysconfigs.h"

#include "FreeRTOS.h"
#include "task.h"








//PRIORITY DEFINES
#define TEST_DRIVERS_TASK_PRIORITY          (tskIDLE_PRIORITY + 1)
#define RUN_UTILITIES_TASK_PRIORITY         (tskIDLE_PRIORITY + 1)
#define MAIN_MCU_EXE_PKT_TASK_PRIORITY      (tskIDLE_PRIORITY + 2)
#define MAIN_MCU_TIMEOUT_PKT_TASK_PRIORITY  (tskIDLE_PRIORITY + 2)
#define AIOSTM_UPDATE_TASK_PRIORITY         (tskIDLE_PRIORITY + 3)
#define SPO2_UNPACK_TASK_PRIORITY           (configMAX_PRIORITIES - 2)
#define MAIN_MCU_UNPACK_TASK_PRIORITY       (configMAX_PRIORITIES - 2)
#define AIOSTM_UNPACK_TASK_PRIORITY		    (configMAX_PRIORITIES - 1)

/* QWB:20150411
 * 1.逻辑优先级低于configMAX_SYSCALL_INTERRUPT_PRIORITY可以使用OS_API函数,否则不能使用
 * 2.逻辑优先级configMAX_SYSCALL_INTERRUPT_PRIORITY与硬件优先级的关系
 * a).FreeRTOS配置硬件中断只用4bits嵌套模式,没有子中断
 * b).configMAX_SYSCALL_INTERRUPT_PRIORITY = 191 = 0xB0 高4bits=0x0B=11
 * c).需要用的OS_API的中断函数只能使用11~15 这些中断优先级且可嵌套,不可以使用12~15的优先级
 * d).使用硬件中断号为0~10  的,不会被内核延迟,并且可嵌套
 * e).不使用OS_API的硬件中断,不受限制,全部中断可以使用 0 ~ 15
 */
#define KERNEL_INTERRUPT_PRIORITY_BASE              (u8)(configMAX_SYSCALL_INTERRUPT_PRIORITY>>4)

//lower number, higher priority
//1.The follow Priority can use OS_API 
#define IRQPriority11Uart23         (KERNEL_INTERRUPT_PRIORITY_BASE)     //11
#define IRQPriority12unDefine       (KERNEL_INTERRUPT_PRIORITY_BASE + 1) //12
#define IRQPriority13unDefine       (KERNEL_INTERRUPT_PRIORITY_BASE + 2) //13
#define IRQPriority14Uart145        (KERNEL_INTERRUPT_PRIORITY_BASE + 3) //14
#define IRQPriority15unDefine       (KERNEL_INTERRUPT_PRIORITY_BASE + 4) //15

//2.The follow Priority can't use OS_API
#define IRQPriority10unDefine       (KERNEL_INTERRUPT_PRIORITY_BASE - 1) //10
#define IRQPriority09unDefine       (KERNEL_INTERRUPT_PRIORITY_BASE - 2) //9
#define IRQPriority08unDefine       (KERNEL_INTERRUPT_PRIORITY_BASE - 3) //8
#define IRQPriority07unDefine       (KERNEL_INTERRUPT_PRIORITY_BASE - 4) //7
#define IRQPriority06unDefine       (KERNEL_INTERRUPT_PRIORITY_BASE - 5) //6
#define IRQPriority05unDefine       (KERNEL_INTERRUPT_PRIORITY_BASE - 6) //5
#define IRQPriority04MyTimer        (KERNEL_INTERRUPT_PRIORITY_BASE - 7) //4
#define IRQPriority03TIM5           (KERNEL_INTERRUPT_PRIORITY_BASE - 8) //3
#define IRQPriority02unDefine       (KERNEL_INTERRUPT_PRIORITY_BASE - 9) //2
#define IRQPriority01unDefine       (KERNEL_INTERRUPT_PRIORITY_BASE - 10) //1
#define IRQPriority00unDefine       (KERNEL_INTERRUPT_PRIORITY_BASE - 11) //0





#define MyMini(a,b)                 (a)>(b) ? (b):(a)
#define MyMaxi(a,b)                 (a)>(b) ? (a):(b)

#define DELAY_NO_WAIT               ( TickType_t ) 0
#define DELAY_MAX_WAIT              portMAX_DELAY

#define SW_OFF                      (char)0x00
#define SW_ON                       (char)0x01
#define PIN_LOW                     (char)0x00
#define PIN_HIGH                    (char)0x01

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DEFINES_H__ */
