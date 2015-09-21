/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : driver_leds.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/21
  Last Modified :
  Description   : leds driver
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

/*----------------------------------------------*
 * internal routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * project-wide global variables                *
 *----------------------------------------------*/

/*----------------------------------------------*
 * module-wide global variables                 *
 *----------------------------------------------*/
static char gLedsStatus[LEDS_TYPE_MAX] = {0,};


/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

typedef struct
{
    uint32_t        RCCx_Periph;
    GPIO_TypeDef    *PORTx;
    uint16_t        PINx;
} LEDS_GPIO_DEF;

const LEDS_GPIO_DEF LEDS_GPIO[LEDS_TYPE_MAX] = 
{
    // LED_RED
    {
        RCC_APB2Periph_GPIOB,
        GPIOB,
        GPIO_Pin_12
    },

    // LED_GREEN
    {
        RCC_APB2Periph_GPIOB,
        GPIOB,
        GPIO_Pin_13
    },
};
/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/




int LedsInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(LEDS_GPIO[LED_RED].RCCx_Periph, ENABLE );
	GPIO_InitStructure.GPIO_Pin = LEDS_GPIO[LED_RED].PINx;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(LEDS_GPIO[LED_RED].PORTx, &GPIO_InitStructure );

    
    RCC_APB2PeriphClockCmd(LEDS_GPIO[LED_GREEN].RCCx_Periph, ENABLE );
	GPIO_InitStructure.GPIO_Pin = LEDS_GPIO[LED_GREEN].PINx;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(LEDS_GPIO[LED_GREEN].PORTx, &GPIO_InitStructure );
    
    return 0;
}

int LedsOpen(void)
{
    LedsCtrl(LED_RED, LED_ON);
    LedsCtrl(LED_GREEN, LED_ON);
    return 0;
}

int LedsRead(char *pReadData, const int nDataLen)
{
    return 0;
}

int LedsWrite(char *pWriteData, const int nDataLen)
{
    return 0;
}

int LedsCtrl(Leds_Type type, Leds_Mode mode)
{
    if (type >= LEDS_TYPE_MAX) return -1;

    if (LED_ON == mode)
    {
        GPIO_SetBits(LEDS_GPIO[type].PORTx, LEDS_GPIO[type].PINx);
    }
    else if (LED_OFF == mode)
    {
        GPIO_ResetBits(LEDS_GPIO[type].PORTx, LEDS_GPIO[type].PINx);
    }
    else if (LED_TRIGGER == mode)
    {
        gLedsStatus[type] ^= 0x01;
        if (gLedsStatus[type])
        {
            GPIO_SetBits(LEDS_GPIO[type].PORTx, LEDS_GPIO[type].PINx);
        }
        else
        {
            GPIO_ResetBits(LEDS_GPIO[type].PORTx, LEDS_GPIO[type].PINx);
        }
    }
    return 0;
}

int LedsClose(void)
{
    return 0;
}

