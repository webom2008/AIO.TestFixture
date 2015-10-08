/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : AppTaskUtilities.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/18
  Last Modified :
  Description   : task utilities
  Function List :
  History       :
  1.Date        : 2015/9/18
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

//#define _SEND_DEMO_PKT_
//#define _INFO_
#define _ERROR_

#ifdef _INFO_
#define INFO(fmt, arg...) udprintf("[Util]Info: "fmt, ##arg)
#else
#define INFO(fmt, arg...) do{}while(0)
#endif
#ifdef _ERROR_
#define ERROR(fmt, arg...) udprintf("[Util]Error: "fmt, ##arg)
#else
#define ERROR(fmt, arg...) do{}while(0)
#endif

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/
#ifdef CONFIG_FOR_DEBUG
#define FREEROTS_RUN_TIMER_PERIOD_US        100

static volatile uint32_t gFreeRtosRunTimeTick;
static char *pTaskListBuffer = NULL;
static const char TaskListInfo1[]   = "Task           State   Priority Stack   Num\r\n";
static const char TaskListInfo2[]   = "*******************************************\r\n";
static const char EndInfo[]         = "*******************************************\r\n\r\n";
static const char RunTimeStatsInfo1[]   = "Task           Abs Time          % Time\r\n";
static const char RunTimeStatsInfo2[]   = "****************************************\r\n";

extern void initFreeRTOSRunTimer(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    
    /* TIM5 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

    /* Time base configuration */ 
    TIM_DeInit(TIM5);
    TIM_InternalClockConfig(TIM5);

    //timer clock = 72Mhz/72 = 1MHz
    TIM_TimeBaseStructure.TIM_Prescaler = ((configCPU_CLOCK_HZ/1000000)-1); // 1MHz(1us)
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;   
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
    TIM_TimeBaseStructure.TIM_Period = FREEROTS_RUN_TIMER_PERIOD_US - 1;
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
    
    TIM_ClearFlag(TIM5, TIM_FLAG_Update);
    TIM_ARRPreloadConfig(TIM5, DISABLE);
    TIM_ITConfig(TIM5,TIM_IT_Update, ENABLE);
    
    /* Enable the TIM5 global Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = IRQPriority03TIM5;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    /* TIM5 enable counter */
    TIM_Cmd(TIM5, ENABLE);
}

void TIM5_IRQHandler(void)
{
    if(RESET != TIM_GetITStatus(TIM5, TIM_IT_Update))
    {
        TIM_ClearITPendingBit(TIM5 , TIM_FLAG_Update);
        gFreeRtosRunTimeTick++;
    }
}

extern uint32_t getFreeRTOSRunTimer(void)
{
    return gFreeRtosRunTimeTick;
}

static    int toWriteLen;
static    int wLen;
static    int offset;

static void Uart1WriteBigData(const char *pData, const int nLen)
{
    
    toWriteLen = nLen;
    wLen = 0;
    offset = 0;
    
    while(offset < nLen)
    {
        if (toWriteLen > UART1_TX_DMA_BUF_LEN )
        {
            wLen = Uart1Write((char *)&pData[offset], UART1_TX_DMA_BUF_LEN);
        }
        else
        {
            wLen = Uart1Write((char *)&pData[offset], toWriteLen);
        }
        
        if (wLen > 0)
        {
            offset += wLen;
            toWriteLen -= wLen;
        }
        else
        {
            vTaskDelay(5);
        }
    }
}


static void RuningUtilitiesTask(void *pvParameters)
{
    volatile TickType_t current_tick;
#ifndef CONFIG_FOR_DEBUG_BUTTON
    const TickType_t xTicksToWait = 5000 / portTICK_PERIOD_MS;
#endif
    UBaseType_t totalTaskNum;
    /* Just to stop compiler warnings. */
    ( void ) pvParameters;
    
    INFO("RuningUtilitiesTask start...\r\n");
#ifndef CONFIG_FOR_DEBUG_BUTTON
    for (;;)
#endif
    {
#ifndef CONFIG_FOR_DEBUG_BUTTON
        vTaskDelay(xTicksToWait);
#endif
        INFO("============== Utilities Start==================\r\n");
        current_tick = xTaskGetTickCount();
        totalTaskNum = uxTaskGetNumberOfTasks();
        INFO("current_tick      = %d\r\n",current_tick);
        INFO("MyTimerTick       = %d\r\n",getMyTimerTick());
        INFO("NumberOfTasks     = %d\r\n",totalTaskNum);
        INFO("HighWaterMark     = %d\r\n",uxTaskGetStackHighWaterMark(NULL));
        INFO("FreeHeapSize      = %d\r\n",xPortGetFreeHeapSize());

  #ifndef CONFIG_UART1_FOR_DPM2200
        pTaskListBuffer = pvPortMalloc(40*totalTaskNum);
        if (NULL != pTaskListBuffer)
        {
            memset(pTaskListBuffer, 0x00, 40*totalTaskNum);
            vTaskList(pTaskListBuffer);
            Uart1WriteBigData((char *)TaskListInfo1,strlen(TaskListInfo1));
            Uart1WriteBigData((char *)TaskListInfo2,strlen(TaskListInfo2));
            Uart1WriteBigData(pTaskListBuffer, strlen(pTaskListBuffer));
            Uart1WriteBigData((char *)EndInfo,strlen(EndInfo));

            memset(pTaskListBuffer, 0x00, 40*totalTaskNum);
            vTaskGetRunTimeStats(pTaskListBuffer);
            Uart1WriteBigData((char *)RunTimeStatsInfo1,strlen(RunTimeStatsInfo1));
            Uart1WriteBigData((char *)RunTimeStatsInfo2,strlen(RunTimeStatsInfo2));
            Uart1WriteBigData(pTaskListBuffer, strlen(pTaskListBuffer));
            Uart1WriteBigData((char *)EndInfo,strlen(EndInfo));
            
            vPortFree(pTaskListBuffer);
            pTaskListBuffer = NULL;
        }
  #endif /* #ifndef CONFIG_UART1_FOR_DPM2200 */
        INFO("============== Utilities End==================\r\n\r\n");
    }
    vTaskDelete(NULL);
}


#endif /* #ifdef CONFIG_FOR_DEBUG */


int AppTaskUtilitiesInit(void)
{
    return 0;
}

int AppTaskUtilitiesStart(void)
{
#ifdef CONFIG_FOR_DEBUG
    while (pdPASS != xTaskCreate(   RuningUtilitiesTask,
                                    "RuningUtilitiesTask",
                                    configMINIMAL_STACK_SIZE,
                                    NULL,
                                    RUN_UTILITIES_TASK_PRIORITY,
                                    NULL));
#endif
    return 0;
}



