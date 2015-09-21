/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : driver_buzzer.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/21
  Last Modified :
  Description   : driver for buzzer
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
 * macros                                       *
 *----------------------------------------------*/
#define BUZZER_RCC      RCC_APB2Periph_GPIOE
#define BUZZER_PORT     GPIOE
#define BUZZER_PIN      GPIO_Pin_15

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
 * routines' implementations                    *
 *----------------------------------------------*/

int BuzzerInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(BUZZER_RCC, ENABLE );
	GPIO_InitStructure.GPIO_Pin = BUZZER_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(BUZZER_PORT, &GPIO_InitStructure );

    return 0;
}

int BuzzerOpen(void)
{
    return 0;
}

int BuzzerRead(void)
{
    return 0;
}

int BuzzerWrite(void)
{
    return 0;
}

int BuzzerCtrl(Buzzer_Mode mode)
{
    static char tigger = 0;
    if (BUZZER_ON == mode)
    {
        GPIO_SetBits(BUZZER_PORT, BUZZER_PIN);
    }
    else if (BUZZER_OFF == mode)
    {
        GPIO_ResetBits(BUZZER_PORT, BUZZER_PIN);
    }
    else if (BUZZER_TRIGGER == mode)
    {
        tigger ^= 0x01;
        if (tigger)
        {
            GPIO_ResetBits(BUZZER_PORT, BUZZER_PIN);
        }
        else
        {
            GPIO_SetBits(BUZZER_PORT, BUZZER_PIN);
        }
    }
    return 0;
}

int BuzzerClose(void)
{
    return 0;
}



