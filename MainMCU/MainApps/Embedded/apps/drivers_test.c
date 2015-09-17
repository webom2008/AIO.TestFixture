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

#ifdef CONFIG_DRIVER_TEST_UART2
#include "uart2_test.c"
#endif

#ifdef CONFIG_DRIVER_TEST_UART3
#include "uart3_test.c"
#endif

#ifdef CONFIG_DRIVER_TEST_UART4
#include "uart4_test.c"
#endif

#ifdef CONFIG_DRIVER_TEST_UART5
#include "uart5_test.c"
#endif



int drivers_test_start(void)
{
#ifdef CONFIG_DRIVER_TEST_UART1
    while (pdPASS != xTaskCreate(   uart1_driver_task,
                                    "uart1_driver_task",
                                    configMINIMAL_STACK_SIZE,
                                    NULL,
                                    TEST_DRIVERS_TASK_PRIORITY,
                                    NULL));
    while (pdPASS != xTaskCreate(   uart1_unpack_task,
                                    "uart1_unpack_task",
                                    configMINIMAL_STACK_SIZE,
                                    NULL,
                                    TEST_DRIVERS_TASK_PRIORITY,
                                    NULL));
#endif /* CONFIG_DRIVER_TEST_UART1 */


#ifdef CONFIG_DRIVER_TEST_UART2
    while (pdPASS != xTaskCreate(   uart2_driver_task,
                                    "uart2_driver_task",
                                    configMINIMAL_STACK_SIZE,
                                    NULL,
                                    TEST_DRIVERS_TASK_PRIORITY,
                                    NULL));
    while (pdPASS != xTaskCreate(   uart2_unpack_task,
                                    "uart2_unpack_task",
                                    configMINIMAL_STACK_SIZE,
                                    NULL,
                                    TEST_DRIVERS_TASK_PRIORITY,
                                    NULL));
#endif



#ifdef CONFIG_DRIVER_TEST_UART3
    while (pdPASS != xTaskCreate(   uart3_driver_task,
                                    "uart3_driver_task",
                                    configMINIMAL_STACK_SIZE,
                                    NULL,
                                    TEST_DRIVERS_TASK_PRIORITY,
                                    NULL));
    while (pdPASS != xTaskCreate(   uart3_unpack_task,
                                    "uart3_unpack_task",
                                    configMINIMAL_STACK_SIZE,
                                    NULL,
                                    TEST_DRIVERS_TASK_PRIORITY,
                                    NULL));
#endif



#ifdef CONFIG_DRIVER_TEST_UART4
    while (pdPASS != xTaskCreate(   uart4_driver_task,
                                    "uart4_driver_task",
                                    configMINIMAL_STACK_SIZE,
                                    NULL,
                                    TEST_DRIVERS_TASK_PRIORITY,
                                    NULL));
    while (pdPASS != xTaskCreate(   uart4_unpack_task,
                                    "uart4_unpack_task",
                                    configMINIMAL_STACK_SIZE,
                                    NULL,
                                    TEST_DRIVERS_TASK_PRIORITY,
                                    NULL));
#endif



#ifdef CONFIG_DRIVER_TEST_UART5
    while (pdPASS != xTaskCreate(   uart5_driver_task,
                                    "uart5_driver_task",
                                    configMINIMAL_STACK_SIZE,
                                    NULL,
                                    TEST_DRIVERS_TASK_PRIORITY,
                                    NULL));
    while (pdPASS != xTaskCreate(   uart5_unpack_task,
                                    "uart5_unpack_task",
                                    configMINIMAL_STACK_SIZE,
                                    NULL,
                                    TEST_DRIVERS_TASK_PRIORITY,
                                    NULL));
#endif

    return 0;
}

