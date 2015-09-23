/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : AppGeneralIO.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/21
  Last Modified :
  Description   : generally gpio task
  Function List :
  History       :
  1.Date        : 2015/9/21
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
EventGroupHandle_t xKeysEventGroup      = NULL;

/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/
#define GENERAL_GPIO_TASK_DELAY_MS      50
#define KEYS_PRESSED_DELAY_MS           200

//#define _INFO_
#define _ERROR_

#ifdef _INFO_
#define INFO(fmt, arg...) udprintf("[GPIO]Info: "fmt, ##arg)
#else
#define INFO(fmt, arg...) do{}while(0)
#endif
#ifdef _ERROR_
#define ERROR(fmt, arg...) udprintf("[GPIO]Error: "fmt, ##arg)
#else
#define ERROR(fmt, arg...) do{}while(0)
#endif
/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/
static int exeKeyPressed(const Keys_Type key)
{
    if (KEY_1 == key)
    {
        InterAdcCtrl(INTER_ADC_CMD_START,NULL);
    }
    else if (KEY_2 == key)
    {
        InterAdcCtrl(INTER_ADC_CMD_STOP,NULL);
    }
    else if (KEY_3 == key)
    {

    }
    return 0;
}

static void GeneralIOTask(void *pvParameters)
{
    const TickType_t xTicksToWait = GENERAL_GPIO_TASK_DELAY_MS / portTICK_PERIOD_MS;
    const u8 keyDelayMaxCnt = KEYS_PRESSED_DELAY_MS / GENERAL_GPIO_TASK_DELAY_MS;
    const u8 ledDelayMaxCnt = 500 / GENERAL_GPIO_TASK_DELAY_MS;
    TickType_t xLastWakeTime;
    Keys_Type key = KEY_1;
    Keys_Val keyVal = KEY_IDLE;
    u8 keyCount[KEYS_TYPE_MAX] = {0,};
    u8 ledcount = 0;
    u32 u32MyTimerNextTick = getMyTimerTick();
	/* Just to stop compiler warnings. */
	( void ) pvParameters;

    xLastWakeTime = xTaskGetTickCount();
    
    INFO("GeneralIOTask running...\n");
    for (;;)
    {
        //keys handle
        for (key = KEY_1; key < KEYS_TYPE_MAX; key++)
        {
            KeysRead(key, &keyVal);
            if (KEY_PRESSED == keyVal)
            {
                keyCount[key]++;
            }
            else
            {
                keyCount[key] = 0;
            }
            
            if (keyCount[key] >= keyDelayMaxCnt)
            {
                keyCount[key] = 0;
                xEventGroupSetBits( xKeysEventGroup, KEYS_EVENT_GROUP_BIT(key));
                BuzzerCtrl(BUZZER_ON);
                vTaskDelay(5);
                BuzzerCtrl(BUZZER_OFF);
                INFO("Key%d Pressed!\r\n", key+1);
                exeKeyPressed(key);
            }
        }

        ledcount++;
        if (ledcount >= ledDelayMaxCnt)
        {
            ledcount = 0;
            LedsCtrl(LED_RED, LED_TRIGGER);
            LedsCtrl(LED_GREEN, LED_TRIGGER);
        }

        // 1s task
        if (IsMyTimerOnTime(u32MyTimerNextTick))
        {
            u32MyTimerNextTick += (1000 / MY_TIM_TICK_PERIOD_MS);
            alarmPowerDetect();
        }
        vTaskDelayUntil(&xLastWakeTime, xTicksToWait);
    }
}



int AppGeneralIOInit(void)
{
	xKeysEventGroup  = xEventGroupCreate();
	do{} while (NULL == xKeysEventGroup);
    
    return 0;
}


int AppGeneralIOStart(void)
{
    while (pdPASS != xTaskCreate(   GeneralIOTask,
                                    "GeneralIOTask",
                                    configMINIMAL_STACK_SIZE,
                                    NULL,
                                    GENERAL_GPIO_TASK_PRIORITY,
                                    NULL));
    return 0;
}

