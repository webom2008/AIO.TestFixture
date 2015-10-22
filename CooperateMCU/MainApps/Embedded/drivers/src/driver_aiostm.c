/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : driver_aiostm.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/10/22
  Last Modified :
  Description   : AioStm32 driver
  Function List :
  History       :
  1.Date        : 2015/10/22
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

#define STM_UART_CTL_PORT       GPIOB
#define STM_UART_CTL_PIN        GPIO_Pin_0
#define STM_UART_CTL_RCC        RCC_APB2Periph_GPIOB


#define STM_BOOT0_CTL_PORT      GPIOB
#define STM_BOOT0_CTL_PIN       GPIO_Pin_1
#define STM_BOOT0_CTL_RCC       RCC_APB2Periph_GPIOB


/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/


int AioStmOpen(void)
{
    char pin = PIN_LOW;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(STM_UART_CTL_RCC | STM_BOOT0_CTL_RCC,ENABLE);

    GPIO_InitStructure.GPIO_Pin = STM_UART_CTL_PIN;      
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
    GPIO_Init(STM_UART_CTL_PORT, &GPIO_InitStructure);  

    GPIO_InitStructure.GPIO_Pin = STM_BOOT0_CTL_PIN;
    GPIO_Init(STM_BOOT0_CTL_PORT, &GPIO_InitStructure); 

    AioStmCtrl(AIO_STM_CTRL_CMD_SET_BOOT0, &pin);
//    pin = PIN_HIGH;
    AioStmCtrl(AIO_STM_CTRL_CMD_SET_UART, &pin);
    return 0;
}


int AioStmCtrl(AioStmCtrlCMD cmd, void *pParameter)
{
    char *pVal = (char *)pParameter;
    if (cmd >= AIO_STM_CTRL_CMD_UNVALID) return -1;
    switch (cmd)
    {
    case AIO_STM_CTRL_CMD_SET_BOOT0:
        if (PIN_LOW == *pVal)
        {
            GPIO_ResetBits(STM_BOOT0_CTL_PORT, STM_BOOT0_CTL_PIN);
        }
        else
        {
            GPIO_SetBits(STM_BOOT0_CTL_PORT, STM_BOOT0_CTL_PIN);
        }
        break;
    case AIO_STM_CTRL_CMD_GET_BOOT0:
        if (GPIO_ReadOutputDataBit(STM_BOOT0_CTL_PORT, STM_BOOT0_CTL_PIN))
        {
            *pVal = PIN_HIGH;
        }
        else
        {
            *pVal = PIN_LOW;
        }
        break;
    case AIO_STM_CTRL_CMD_SET_UART:
        if (PIN_LOW == *pVal)
        {
            GPIO_ResetBits(STM_UART_CTL_PORT, STM_UART_CTL_PIN);
        }
        else
        {
            GPIO_SetBits(STM_UART_CTL_PORT, STM_UART_CTL_PIN);
        }
        break;
    case AIO_STM_CTRL_CMD_GET_UART:
        if (GPIO_ReadOutputDataBit(STM_UART_CTL_PORT, STM_UART_CTL_PIN))
        {
            *pVal = PIN_HIGH;
        }
        else
        {
            *pVal = PIN_LOW;
        }
        break;
    default:
        break;
    }
    return 0;
}


int AioStmClose(void)
{
    return 0;
}

