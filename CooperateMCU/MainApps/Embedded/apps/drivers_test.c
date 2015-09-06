/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : drivers_test.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/3
  Last Modified :
  Description   : drivers test app
  Function List :
  History       :
  1.Date        : 2015/9/3
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

#ifdef CONFIG_DRIVER_TEST_UART1
#include "uart1_test.c"
#endif


int drivers_test_start(void)
{
#ifdef CONFIG_DRIVER_TEST_UART1
    xTaskCreate(uart_driver_task,
                "uart_driver_task",
                configMINIMAL_STACK_SIZE,
                NULL,
                TEST_DRIVERS_TASK_PRIORITY,
                NULL);
 #ifdef CONFIG_UART1_DMA_MODE
    xTaskCreate(uart1_unpack_task,
                "uart1_unpack_task",
                configMINIMAL_STACK_SIZE,
                NULL,
                TEST_DRIVERS_TASK_PRIORITY,
                NULL);
 #endif /* CONFIG_UART1_DMA_MODE */
#endif /* CONFIG_DRIVER_TEST_UART1 */
    return 0;
}

