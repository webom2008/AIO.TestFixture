/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : MyTimer.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/15
  Last Modified :
  Description   : My timer
  Function List :
  History       :
  1.Date        : 2015/9/15
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
static volatile u32 g100msTick;

/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/


/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/


int MyTimerInit(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    
    /* TIM2 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    /* Time base configuration */ 
    TIM_DeInit(TIM2);
    TIM_InternalClockConfig(TIM2);

    //timer clock = 72Mhz/7200 = 10kHz
    TIM_TimeBaseStructure.TIM_Prescaler = ((configCPU_CLOCK_HZ/10000)-1); // 10kHz(0.1ms)
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;   
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
    TIM_TimeBaseStructure.TIM_Period = MY_TIM_TICK_PERIOD_MS*10 - 1;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    TIM_ARRPreloadConfig(TIM2, DISABLE);
    TIM_ITConfig(TIM2,TIM_IT_Update, ENABLE);
    
    /* Enable the TIM2 global Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = IRQPriority04MyTimer;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    return 0;
}

int MyTimerStart(void)
{
    /* TIM2 enable counter */
    TIM_Cmd(TIM2, ENABLE);
    return 0;
}

void TIM2_IRQHandler(void)
{
    if(RESET != TIM_GetITStatus(TIM2, TIM_IT_Update))
    {
        TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update);
        g100msTick++;
//        if (g100msTick % 10 == 0) udprintf("tick=%d\r\n",g100msTick);
    }
}

u32 getMyTimerTick(void)
{
    return g100msTick;
}

static int compareTick(const u32 u32Tick1, const u32 u32Tick2) // Interval must less than 0x80000000
{
    if(u32Tick1 == u32Tick2)
    {
        return 0;       // ==
    }
    else if(u32Tick1 < u32Tick2)
    {
        if((u32Tick2 - u32Tick1) < 0x80000000)
        {
            return -1;  // <
        }
        else            // Tick Out
        {
            return 1;   // >
        }
    }
    else    // u32Tick1 > u32Tick2
    {
        if((u32Tick1 - u32Tick2) < 0x80000000)
        {
            return 1;   // >
        }
        else            // Tick Out
        {
            return -1;  // <
        }
    }
}

int IsMyTimerOnTime(const u32 u32Target)
{
    if(compareTick(g100msTick, u32Target) == -1) 
    {
        return 0;
    }

    return 1;
}

int IsMyTimerOverTime(const u32 u32Base, const u32 u32Duration)    
{
    u32 u32Target;

    u32Target = u32Base + u32Duration;

    if(compareTick(g100msTick, u32Target) == 1)
    {
        return 1;
    }

    return 0;
}

