/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : driver_ecg.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/10/30
  Last Modified :
  Description   : ecg module driver
  Function List :
  History       :
  1.Date        : 2015/10/30
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
#define V_LEAD_SEL_S0_PIN       GPIO_Pin_0
#define V_LEAD_SEL_S0_PORT      GPIOB
#define V_LEAD_SEL_S0_RCC       RCC_APB2Periph_GPIOB
#define V_LEAD_SEL_S0_HIGH      GPIO_SetBits(V_LEAD_SEL_S0_PORT, V_LEAD_SEL_S0_PIN)
#define V_LEAD_SEL_S0_LOW       GPIO_ResetBits(V_LEAD_SEL_S0_PORT, V_LEAD_SEL_S0_PIN)

#define V_LEAD_SEL_S1_PIN       GPIO_Pin_1
#define V_LEAD_SEL_S1_PORT      GPIOB
#define V_LEAD_SEL_S1_RCC       RCC_APB2Periph_GPIOB
#define V_LEAD_SEL_S1_HIGH      GPIO_SetBits(V_LEAD_SEL_S1_PORT, V_LEAD_SEL_S1_PIN)
#define V_LEAD_SEL_S1_LOW       GPIO_ResetBits(V_LEAD_SEL_S1_PORT, V_LEAD_SEL_S1_PIN)

#define RA_LEAD_SEL_S0_PIN      GPIO_Pin_7
#define RA_LEAD_SEL_S0_PORT     GPIOE
#define RA_LEAD_SEL_S0_RCC      RCC_APB2Periph_GPIOE
#define RA_LEAD_SEL_S0_HIGH     GPIO_SetBits(RA_LEAD_SEL_S0_PORT, RA_LEAD_SEL_S0_PIN)
#define RA_LEAD_SEL_S0_LOW      GPIO_ResetBits(RA_LEAD_SEL_S0_PORT, RA_LEAD_SEL_S0_PIN)

#define RA_LEAD_SEL_S1_PIN      GPIO_Pin_8
#define RA_LEAD_SEL_S1_PORT     GPIOE
#define RA_LEAD_SEL_S1_RCC      RCC_APB2Periph_GPIOE
#define RA_LEAD_SEL_S1_HIGH     GPIO_SetBits(RA_LEAD_SEL_S1_PORT, RA_LEAD_SEL_S1_PIN)
#define RA_LEAD_SEL_S1_LOW      GPIO_ResetBits(RA_LEAD_SEL_S1_PORT, RA_LEAD_SEL_S1_PIN)

#define LL_LEAD_SEL_S0_PIN      GPIO_Pin_9
#define LL_LEAD_SEL_S0_PORT     GPIOE
#define LL_LEAD_SEL_S0_RCC      RCC_APB2Periph_GPIOE
#define LL_LEAD_SEL_S0_HIGH     GPIO_SetBits(LL_LEAD_SEL_S0_PORT, LL_LEAD_SEL_S0_PIN)
#define LL_LEAD_SEL_S0_LOW      GPIO_ResetBits(LL_LEAD_SEL_S0_PORT, LL_LEAD_SEL_S0_PIN)

#define LL_LEAD_SEL_S1_PIN      GPIO_Pin_10
#define LL_LEAD_SEL_S1_PORT     GPIOE
#define LL_LEAD_SEL_S1_RCC      RCC_APB2Periph_GPIOE
#define LL_LEAD_SEL_S1_HIGH     GPIO_SetBits(LL_LEAD_SEL_S1_PORT, LL_LEAD_SEL_S1_PIN)
#define LL_LEAD_SEL_S1_LOW      GPIO_ResetBits(LL_LEAD_SEL_S1_PORT, LL_LEAD_SEL_S1_PIN)

#define RL_LEAD_SEL_S0_PIN      GPIO_Pin_11
#define RL_LEAD_SEL_S0_PORT     GPIOE
#define RL_LEAD_SEL_S0_RCC      RCC_APB2Periph_GPIOE
#define RL_LEAD_SEL_S0_HIGH     GPIO_SetBits(RL_LEAD_SEL_S0_PORT, RL_LEAD_SEL_S0_PIN)
#define RL_LEAD_SEL_S0_LOW      GPIO_ResetBits(RL_LEAD_SEL_S0_PORT, RL_LEAD_SEL_S0_PIN)

#define RL_LEAD_SEL_S1_PIN      GPIO_Pin_12
#define RL_LEAD_SEL_S1_PORT     GPIOE
#define RL_LEAD_SEL_S1_RCC      RCC_APB2Periph_GPIOE
#define RL_LEAD_SEL_S1_HIGH     GPIO_SetBits(RL_LEAD_SEL_S1_PORT, RL_LEAD_SEL_S1_PIN)
#define RL_LEAD_SEL_S1_LOW      GPIO_ResetBits(RL_LEAD_SEL_S1_PORT, RL_LEAD_SEL_S1_PIN)

#define LA_LEAD_SEL_S0_PIN      GPIO_Pin_13
#define LA_LEAD_SEL_S0_PORT     GPIOE
#define LA_LEAD_SEL_S0_RCC      RCC_APB2Periph_GPIOE
#define LA_LEAD_SEL_S0_HIGH     GPIO_SetBits(LA_LEAD_SEL_S0_PORT, LA_LEAD_SEL_S0_PIN)
#define LA_LEAD_SEL_S0_LOW      GPIO_ResetBits(LA_LEAD_SEL_S0_PORT, LA_LEAD_SEL_S0_PIN)

#define LA_LEAD_SEL_S1_PIN      GPIO_Pin_14
#define LA_LEAD_SEL_S1_PORT     GPIOE
#define LA_LEAD_SEL_S1_RCC      RCC_APB2Periph_GPIOE
#define LA_LEAD_SEL_S1_HIGH     GPIO_SetBits(LA_LEAD_SEL_S1_PORT, LA_LEAD_SEL_S1_PIN)
#define LA_LEAD_SEL_S1_LOW      GPIO_ResetBits(LA_LEAD_SEL_S1_PORT, LA_LEAD_SEL_S1_PIN)

#define RA_LEAD_OFF_PIN         GPIO_Pin_13
#define RA_LEAD_OFF_PORT        GPIOD
#define RA_LEAD_OFF_RCC         RCC_APB2Periph_GPIOD
#define RA_LEAD_OFF_HIGH        GPIO_SetBits(RA_LEAD_OFF_PORT, RA_LEAD_OFF_PIN)
#define RA_LEAD_OFF_LOW         GPIO_ResetBits(RA_LEAD_OFF_PORT, RA_LEAD_OFF_PIN)

#define RL_LEAD_OFF_PIN         GPIO_Pin_14
#define RL_LEAD_OFF_PORT        GPIOD
#define RL_LEAD_OFF_RCC         RCC_APB2Periph_GPIOD
#define RL_LEAD_OFF_HIGH        GPIO_SetBits(RL_LEAD_OFF_PORT, RL_LEAD_OFF_PIN)
#define RL_LEAD_OFF_LOW         GPIO_ResetBits(RL_LEAD_OFF_PORT, RL_LEAD_OFF_PIN)

#define V_LEAD_OFF_PIN          GPIO_Pin_15
#define V_LEAD_OFF_PORT         GPIOD
#define V_LEAD_OFF_RCC          RCC_APB2Periph_GPIOD
#define V_LEAD_OFF_HIGH         GPIO_SetBits(V_LEAD_OFF_PORT, V_LEAD_OFF_PIN)
#define V_LEAD_OFF_LOW          GPIO_ResetBits(V_LEAD_OFF_PORT, V_LEAD_OFF_PIN)

#define LA_LEAD_OFF_PIN         GPIO_Pin_6
#define LA_LEAD_OFF_PORT        GPIOC
#define LA_LEAD_OFF_RCC         RCC_APB2Periph_GPIOC
#define LA_LEAD_OFF_HIGH        GPIO_SetBits(LA_LEAD_OFF_PORT, LA_LEAD_OFF_PIN)
#define LA_LEAD_OFF_LOW         GPIO_ResetBits(LA_LEAD_OFF_PORT, LA_LEAD_OFF_PIN)

#define LL_LEAD_OFF_PIN         GPIO_Pin_7
#define LL_LEAD_OFF_PORT        GPIOC
#define LL_LEAD_OFF_RCC         RCC_APB2Periph_GPIOC
#define LL_LEAD_OFF_HIGH        GPIO_SetBits(LL_LEAD_OFF_PORT, LL_LEAD_OFF_PIN)
#define LL_LEAD_OFF_LOW         GPIO_ResetBits(LL_LEAD_OFF_PORT, LL_LEAD_OFF_PIN)

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/


int EcgDevInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd( V_LEAD_SEL_S0_RCC \
                            | V_LEAD_SEL_S1_RCC \
                            | RA_LEAD_SEL_S0_RCC \
                            | RA_LEAD_SEL_S1_RCC \
                            | LL_LEAD_SEL_S0_RCC \
                            | LL_LEAD_SEL_S1_RCC \
                            | RL_LEAD_SEL_S0_RCC \
                            | RL_LEAD_SEL_S1_RCC \
                            | LA_LEAD_SEL_S0_RCC \
                            | LA_LEAD_SEL_S1_RCC \
                            | RA_LEAD_OFF_RCC \
                            | RL_LEAD_OFF_RCC \
                            | V_LEAD_OFF_RCC \
                            | LA_LEAD_OFF_RCC \
                            | LL_LEAD_OFF_RCC,
                            ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_InitStructure.GPIO_Pin = V_LEAD_SEL_S0_PIN;      
    GPIO_Init(V_LEAD_SEL_S0_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = V_LEAD_SEL_S1_PIN;      
    GPIO_Init(V_LEAD_SEL_S1_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = RA_LEAD_SEL_S0_PIN;      
    GPIO_Init(RA_LEAD_SEL_S0_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = RA_LEAD_SEL_S1_PIN;      
    GPIO_Init(RA_LEAD_SEL_S1_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = LL_LEAD_SEL_S0_PIN;      
    GPIO_Init(LL_LEAD_SEL_S0_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = LL_LEAD_SEL_S1_PIN;      
    GPIO_Init(LL_LEAD_SEL_S1_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = LA_LEAD_SEL_S0_PIN;      
    GPIO_Init(LA_LEAD_SEL_S0_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = LA_LEAD_SEL_S1_PIN;      
    GPIO_Init(LA_LEAD_SEL_S1_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = RL_LEAD_SEL_S0_PIN;      
    GPIO_Init(RL_LEAD_SEL_S0_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = RL_LEAD_SEL_S1_PIN;      
    GPIO_Init(RL_LEAD_SEL_S1_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = RA_LEAD_OFF_PIN;      
    GPIO_Init(RA_LEAD_OFF_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = RL_LEAD_OFF_PIN;      
    GPIO_Init(RL_LEAD_OFF_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = V_LEAD_OFF_PIN;      
    GPIO_Init(V_LEAD_OFF_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = LA_LEAD_OFF_PIN;      
    GPIO_Init(LA_LEAD_OFF_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = LL_LEAD_OFF_PIN;      
    GPIO_Init(LL_LEAD_OFF_PORT, &GPIO_InitStructure);
    
    return 0;
}

int EcgDevCtrl(const ECG_CTRL_CMD cmd, const ECG_CTRL_CMD_VAL val)
{
    switch (cmd)
    {
    case CMD_ECG_RA_SEL:{
        if (CMD_VAL_SEL_CH0 == val)
        {
            RA_LEAD_SEL_S1_LOW;
            RA_LEAD_SEL_S0_LOW;
        }
        else if (CMD_VAL_SEL_CH1 == val)
        {
            RA_LEAD_SEL_S1_LOW;
            RA_LEAD_SEL_S0_HIGH;
        }
        else if (CMD_VAL_SEL_CH2 == val)
        {
            RA_LEAD_SEL_S1_HIGH;
            RA_LEAD_SEL_S0_LOW;
        }
    }break;
    case CMD_ECG_RL_SEL:{
        if (CMD_VAL_SEL_CH0 == val)
        {
            RL_LEAD_SEL_S1_LOW;
            RL_LEAD_SEL_S0_LOW;
        }
        else if (CMD_VAL_SEL_CH1 == val)
        {
            RL_LEAD_SEL_S1_LOW;
            RL_LEAD_SEL_S0_HIGH;
        }
        else if (CMD_VAL_SEL_CH2 == val)
        {
            RL_LEAD_SEL_S1_HIGH;
            RL_LEAD_SEL_S0_LOW;
        }
    }break;
    case CMD_ECG_LA_SEL:{
        if (CMD_VAL_SEL_CH0 == val)
        {
            LA_LEAD_SEL_S1_LOW;
            LA_LEAD_SEL_S0_LOW;
        }
        else if (CMD_VAL_SEL_CH1 == val)
        {
            LA_LEAD_SEL_S1_LOW;
            LA_LEAD_SEL_S0_HIGH;
        }
        else if (CMD_VAL_SEL_CH2 == val)
        {
            LA_LEAD_SEL_S1_HIGH;
            LA_LEAD_SEL_S0_LOW;
        }
    }break;
    case CMD_ECG_LL_SEL:{
        if (CMD_VAL_SEL_CH0 == val)
        {
            LL_LEAD_SEL_S1_LOW;
            LL_LEAD_SEL_S0_LOW;
        }
        else if (CMD_VAL_SEL_CH1 == val)
        {
            LL_LEAD_SEL_S1_LOW;
            LL_LEAD_SEL_S0_HIGH;
        }
        else if (CMD_VAL_SEL_CH2 == val)
        {
            LL_LEAD_SEL_S1_HIGH;
            LL_LEAD_SEL_S0_LOW;
        }
    }break;
    case CMD_ECG_V_SEL:{
        if (CMD_VAL_SEL_CH0 == val)
        {
            V_LEAD_SEL_S1_LOW;
            V_LEAD_SEL_S0_LOW;
        }
        else if (CMD_VAL_SEL_CH1 == val)
        {
            V_LEAD_SEL_S1_LOW;
            V_LEAD_SEL_S0_HIGH;
        }
        else if (CMD_VAL_SEL_CH2 == val)
        {
            V_LEAD_SEL_S1_HIGH;
            V_LEAD_SEL_S0_LOW;
        }
    }break;
    case CMD_ECG_RA_OFF:{
        if (CMD_VAL_OFF_NORMAL == val)
        {
            RA_LEAD_OFF_LOW;
        }
        else if (CMD_VAL_OFF_LEADOFF == val)
        {
            RA_LEAD_OFF_HIGH;
        }
    }break;
    case CMD_ECG_RL_OFF:{
        if (CMD_VAL_OFF_NORMAL == val)
        {
            RL_LEAD_OFF_LOW;
        }
        else if (CMD_VAL_OFF_LEADOFF == val)
        {
            RL_LEAD_OFF_HIGH;
        }
    }break;
    case CMD_ECG_LA_OFF:{
        if (CMD_VAL_OFF_NORMAL == val)
        {
            LA_LEAD_OFF_LOW;
        }
        else if (CMD_VAL_OFF_LEADOFF == val)
        {
            LA_LEAD_OFF_HIGH;
        }
    }break;
    case CMD_ECG_LL_OFF:{
        if (CMD_VAL_OFF_NORMAL == val)
        {
            LL_LEAD_OFF_LOW;
        }
        else if (CMD_VAL_OFF_LEADOFF == val)
        {
            LL_LEAD_OFF_HIGH;
        }
    }break;
    case CMD_ECG_V_OFF:{
        if (CMD_VAL_OFF_NORMAL == val)
        {
            V_LEAD_OFF_LOW;
        }
        else if (CMD_VAL_OFF_LEADOFF == val)
        {
            V_LEAD_OFF_HIGH;
        }
    }break;
    case CMD_ECG_ALL_SEL_ECGOUT:{
        EcgDevCtrl(CMD_ECG_RA_SEL, CMD_VAL_SEL_CH0);
        EcgDevCtrl(CMD_ECG_RL_SEL, CMD_VAL_SEL_CH0);
        EcgDevCtrl(CMD_ECG_LA_SEL, CMD_VAL_SEL_CH0);
        EcgDevCtrl(CMD_ECG_LL_SEL, CMD_VAL_SEL_CH0);
        EcgDevCtrl(CMD_ECG_V_SEL, CMD_VAL_SEL_CH0);
    }break;
    case CMD_ECG_ALL_SEL_PACE:{
        EcgDevCtrl(CMD_ECG_RA_SEL, CMD_VAL_SEL_CH1);
        EcgDevCtrl(CMD_ECG_RL_SEL, CMD_VAL_SEL_CH1);
        EcgDevCtrl(CMD_ECG_LA_SEL, CMD_VAL_SEL_CH1);
        EcgDevCtrl(CMD_ECG_LL_SEL, CMD_VAL_SEL_CH1);
        EcgDevCtrl(CMD_ECG_V_SEL, CMD_VAL_SEL_CH1);
        
    }break;
    case CMD_ECG_ALL_SEL_OFFSET:{
        EcgDevCtrl(CMD_ECG_RA_SEL, CMD_VAL_SEL_CH2);
        EcgDevCtrl(CMD_ECG_RL_SEL, CMD_VAL_SEL_CH2);
        EcgDevCtrl(CMD_ECG_LA_SEL, CMD_VAL_SEL_CH2);
        EcgDevCtrl(CMD_ECG_LL_SEL, CMD_VAL_SEL_CH2);
        EcgDevCtrl(CMD_ECG_V_SEL, CMD_VAL_SEL_CH2);
        
    }break;
    case CMD_ECG_LL_LA_OFF:{
        if (CMD_VAL_OFF_NORMAL == val)
        {
            LL_LEAD_OFF_LOW;
            LA_LEAD_OFF_LOW;
        }
        else if (CMD_VAL_OFF_LEADOFF == val)
        {
            LL_LEAD_OFF_HIGH;
            LA_LEAD_OFF_HIGH;
        }
    }break;
    case CMD_ECG_LA_RA_OFF:{
        if (CMD_VAL_OFF_NORMAL == val)
        {
            RA_LEAD_OFF_LOW;
            LA_LEAD_OFF_LOW;
        }
        else if (CMD_VAL_OFF_LEADOFF == val)
        {
            RA_LEAD_OFF_HIGH;
            LA_LEAD_OFF_HIGH;
        }
    }break;
    case CMD_ECG_LL_RA_OFF:{
        if (CMD_VAL_OFF_NORMAL == val)
        {
            LL_LEAD_OFF_LOW;
            RA_LEAD_OFF_LOW;
        }
        else if (CMD_VAL_OFF_LEADOFF == val)
        {
            LL_LEAD_OFF_HIGH;
            RA_LEAD_OFF_HIGH;
        }
    }break;
    case CMD_ECG_LL_LA_RA_OFF:{
        if (CMD_VAL_OFF_NORMAL == val)
        {
            LL_LEAD_OFF_LOW;
            LA_LEAD_OFF_LOW;
            RA_LEAD_OFF_LOW;
        }
        else if (CMD_VAL_OFF_LEADOFF == val)
        {
            LL_LEAD_OFF_HIGH;
            LA_LEAD_OFF_HIGH;
            RA_LEAD_OFF_HIGH;
        }
    }break;
    case CMD_ECG_ALL_OFF:{
        if (CMD_VAL_OFF_NORMAL == val)
        {
            LL_LEAD_OFF_LOW;
            LA_LEAD_OFF_LOW;
            RA_LEAD_OFF_LOW;
            RL_LEAD_OFF_LOW;
            V_LEAD_OFF_LOW;
        }
        else if (CMD_VAL_OFF_LEADOFF == val)
        {
            LL_LEAD_OFF_HIGH;
            LA_LEAD_OFF_HIGH;
            RA_LEAD_OFF_HIGH;
            RL_LEAD_OFF_HIGH;
            V_LEAD_OFF_HIGH;
        }
    }break;
    default:{

    }break;
    }
    return 0;
}
