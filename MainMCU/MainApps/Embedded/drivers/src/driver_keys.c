/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : driver_keys.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/21
  Last Modified :
  Description   : driver for keys
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

/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

typedef struct
{
    uint32_t        RCCx_Periph;
    GPIO_TypeDef    *PORTx;
    uint16_t        PINx;

} KEYS_GPIO_DEF;

const KEYS_GPIO_DEF KEYS_GPIO[KEYS_TYPE_MAX] = 
{
    // KEY1
    {
        RCC_APB2Periph_GPIOC,
        GPIOC,
        GPIO_Pin_13
    },

    // KEY2
    {
        RCC_APB2Periph_GPIOC,
        GPIOC,
        GPIO_Pin_14
    },
    
    // KEY3
    {
        RCC_APB2Periph_GPIOC,
        GPIOC,
        GPIO_Pin_15
    },
};

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

int KeysInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(KEYS_GPIO[KEY_1].RCCx_Periph, ENABLE );
	GPIO_InitStructure.GPIO_Pin = KEYS_GPIO[KEY_1].PINx;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(KEYS_GPIO[KEY_1].PORTx, &GPIO_InitStructure );

    
    RCC_APB2PeriphClockCmd(KEYS_GPIO[KEY_2].RCCx_Periph, ENABLE );
	GPIO_InitStructure.GPIO_Pin = KEYS_GPIO[KEY_2].PINx;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(KEYS_GPIO[KEY_2].PORTx, &GPIO_InitStructure );

    
    RCC_APB2PeriphClockCmd(KEYS_GPIO[KEY_3].RCCx_Periph, ENABLE );
	GPIO_InitStructure.GPIO_Pin = KEYS_GPIO[KEY_3].PINx;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(KEYS_GPIO[KEY_3].PORTx, &GPIO_InitStructure );
    
    return 0;
}

int KeysOpen(void)
{
    return 0;
}

int KeysRead(Keys_Type type, Keys_Val *pVal)
{
    if (type >= KEYS_TYPE_MAX) return -1;
    
    if ((uint8_t)Bit_RESET == GPIO_ReadInputDataBit(KEYS_GPIO[type].PORTx, KEYS_GPIO[type].PINx))
    {
        *pVal = KEY_PRESSED;
    }
    else
    {
        *pVal = KEY_IDLE;
    }
    return 0;
}

int KeysWrite(void)
{
    return 0;
}

int KeysCtrl(void)
{
    return 0;
}

int KeysClose(void)
{
    return 0;
}

