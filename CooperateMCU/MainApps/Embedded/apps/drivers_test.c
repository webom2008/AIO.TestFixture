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



#ifdef CONFIG_DRIVER_TEST_UART1_DPM2200
static int cur_press;
static void uart1_dpm2200_driver_task(void *pvParameters)
{
    int val;
    const TickType_t xTicksToWait = 1000 / portTICK_PERIOD_MS; //delay 1s
    
    /* Just to stop compiler warnings. */
    ( void ) pvParameters;
    for (;;)
    {
        val = 0;
        DPM2200Ctrl(DPMCTRL_R_PRESS, &val);
        cur_press = val;
        vTaskDelay(xTicksToWait);
        val = PressUnits_PSI;
        DPM2200Ctrl(DPMCTRL_W_UNITS, &val);
        vTaskDelay(xTicksToWait);
    }
}

#endif

int drivers_test_start(void)
{
#ifdef CONFIG_DRIVER_TEST_UART1
    xTaskCreate(uart1_driver_task,
                "uart1_driver_task",
                configMINIMAL_STACK_SIZE,
                NULL,
                TEST_DRIVERS_TASK_PRIORITY,
                NULL);
    xTaskCreate(uart1_unpack_task,
                "uart1_unpack_task",
                configMINIMAL_STACK_SIZE,
                NULL,
                TEST_DRIVERS_TASK_PRIORITY,
                NULL);
#endif /* CONFIG_DRIVER_TEST_UART1 */


#ifdef CONFIG_DRIVER_TEST_UART2
    xTaskCreate(uart2_driver_task,
                "uart2_driver_task",
                configMINIMAL_STACK_SIZE,
                NULL,
                TEST_DRIVERS_TASK_PRIORITY,
                NULL);
    xTaskCreate(uart2_unpack_task,
                "uart2_unpack_task",
                configMINIMAL_STACK_SIZE,
                NULL,
                TEST_DRIVERS_TASK_PRIORITY,
                NULL);
#endif



#ifdef CONFIG_DRIVER_TEST_UART3
    xTaskCreate(uart3_driver_task,
                "uart3_driver_task",
                configMINIMAL_STACK_SIZE,
                NULL,
                TEST_DRIVERS_TASK_PRIORITY,
                NULL);
    xTaskCreate(uart3_unpack_task,
                "uart3_unpack_task",
                configMINIMAL_STACK_SIZE,
                NULL,
                TEST_DRIVERS_TASK_PRIORITY,
                NULL);
#endif



#ifdef CONFIG_DRIVER_TEST_UART4
    xTaskCreate(uart4_driver_task,
                "uart4_driver_task",
                configMINIMAL_STACK_SIZE,
                NULL,
                TEST_DRIVERS_TASK_PRIORITY,
                NULL);
    xTaskCreate(uart4_unpack_task,
                "uart4_unpack_task",
                configMINIMAL_STACK_SIZE,
                NULL,
                TEST_DRIVERS_TASK_PRIORITY,
                NULL);
#endif



#ifdef CONFIG_DRIVER_TEST_UART5
    xTaskCreate(uart5_driver_task,
                "uart5_driver_task",
                configMINIMAL_STACK_SIZE,
                NULL,
                TEST_DRIVERS_TASK_PRIORITY,
                NULL);
    xTaskCreate(uart5_unpack_task,
                "uart5_unpack_task",
                configMINIMAL_STACK_SIZE,
                NULL,
                TEST_DRIVERS_TASK_PRIORITY,
                NULL);
#endif

#ifdef CONFIG_DRIVER_TEST_UART1_DPM2200
    xTaskCreate(uart1_dpm2200_driver_task,
                "uart1_dpm2200_driver_task",
                configMINIMAL_STACK_SIZE,
                NULL,
                TEST_DRIVERS_TASK_PRIORITY,
                NULL);
#endif
    return 0;
}

