/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : AppAIOStm32.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/12
  Last Modified :
  Description   : communicate with AIO-STM
  Function List :
  History       :
  1.Date        : 2015/9/12
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#include "includes.h"
#include "AioStmUpdate.h"

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
#define _INFO_
#define _ERROR_

#ifdef _INFO_
#define INFO(fmt, arg...) udprintf("[MainMCU]Info: "fmt, ##arg)
#else
#define INFO(fmt, arg...) do{}while(0)
#endif
#ifdef _ERROR_
#define ERROR(fmt, arg...) udprintf("[MainMCU]Error: "fmt, ##arg)
#else
#define ERROR(fmt, arg...) do{}while(0)
#endif

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/
 
static void AioStmUnpackTask(void *pvParameters)
{
    int rLen = 0, i;
    char rxBuf[128];
    const TickType_t xTicksToWait = 5 / portTICK_PERIOD_MS;
    
	/* Just to stop compiler warnings. */
	( void ) pvParameters;
    
    INFO("AioStmUnpackTask running...\n");
    for (;;)
    {
        rLen = 0;
        memset(rxBuf, 0x00, sizeof(rxBuf));
        rLen = Uart3Read(rxBuf, sizeof(rxBuf));
        if (rLen > 0)
        {
            for(i=0; i < rLen; i++)
            {
                xQueueSendToBack(gpAioStmDev->pAioStmUpdateRxQueue,
                                (void *)&rxBuf[i],
                                DELAY_NO_WAIT);
            }
        }
        vTaskDelay(xTicksToWait);
    }
}

int AppAioStmInit(void)
{
    int ret = 0;
    
    ret = AioStmUpdateInit();
    
    while (ret < 0); //error hanppen
    return 0;
}

int AppAioStmStart(void)
{
#ifndef CONFIG_DRIVER_TEST_UART3
    xTaskCreate(AioStmUnpackTask,
                "AioStmUnpackTask",
                2*configMINIMAL_STACK_SIZE,
                NULL,
                AIOSTM_UNPACK_TASK_PRIORITY,
                NULL);
    xTaskCreate(AioStmUpdateTask,
                "AioStmUpdateTask",
                configMINIMAL_STACK_SIZE,
                NULL,
                AIOSTM_UPDATE_TASK_PRIORITY,
                NULL);
#endif /* CONFIG_DRIVER_TEST_UART2 */
    return 0;
}
