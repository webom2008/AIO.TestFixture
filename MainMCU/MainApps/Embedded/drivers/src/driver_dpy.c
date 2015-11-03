/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : driver_dpy.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/10/14
  Last Modified :
  Description   : driver for LED display
  Function List :
  History       :
  1.Date        : 2015/10/14
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
const uint8_t dis_data_arry[10] = \
{0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/
#define DIG_SDA_PIN             GPIO_Pin_7
#define DIG_SDA_PORT            GPIOB
#define DIG_SDA_RCC             RCC_APB2Periph_GPIOB

#define DIG_SCL_PIN             GPIO_Pin_6
#define DIG_SCL_PORT            GPIOB
#define DIG_SCL_RCC             RCC_APB2Periph_GPIOB

#define SDA_HIGH                GPIO_SetBits(DIG_SDA_PORT, DIG_SDA_PIN)
#define SDA_LOW                 GPIO_ResetBits(DIG_SDA_PORT, DIG_SDA_PIN)

#define SCK_HIGH                GPIO_SetBits(DIG_SCL_PORT, DIG_SCL_PIN)
#define SCK_LOW                 GPIO_ResetBits(DIG_SCL_PORT, DIG_SCL_PIN)

#define DIS_DATA_CMD            0x48
#define LIGHT_LEVEL(x)          (x << 4)  //????
#define DIS_TYPE(x)             (x << 3)  //0 seven segment display, 1 eight segment display
#define DIS_ENABLE(x)           (x)       //1 enable, 0 disable
#define ADDR1                   0x68
#define ADDR2                   0x6A
#define ADDR3                   0x6C
#define ADDR4                   0x6E
/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

static void delay(uint32_t ns_data)
{
    int i = 0, j = 0;
    for(i = 0; i < ns_data; i++)
    {
        for(j = 0; j < 3000; j++);
    }
}

static void Send_Start(void)
{
    SCK_LOW;
    delay(1);

    SDA_HIGH;
    delay(1);

    SCK_HIGH;
    delay(1);

    SDA_LOW;
    delay(1);

    SCK_LOW;
    delay(1);
}

static void Send_Stop(void)
{
    SCK_LOW;
    delay(1);

    SDA_LOW;
    delay(1);

    SCK_HIGH;
    delay(1);

    SDA_HIGH;
    delay(1);
}

void Send_Byte(uint8_t data)
{
    int8_t i = 0;

    for (i = 8u; i != 0; i--) 
    {
    SCK_LOW;
    delay(1);

    (data & 0x80u)? (SDA_HIGH): (SDA_LOW);
    delay(1);

    SCK_HIGH;
    delay(1);

    data <<= 1;
    }
    SCK_LOW;
    delay(1);
    SCK_HIGH;
    delay(1);
    SCK_LOW;
}

int LedsDisplayInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(DIG_SDA_RCC | DIG_SCL_RCC,ENABLE);

    GPIO_InitStructure.GPIO_Pin = DIG_SDA_PIN;      
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(DIG_SDA_PORT, &GPIO_InitStructure);	

    GPIO_InitStructure.GPIO_Pin = DIG_SCL_PIN;  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DIG_SCL_PORT, &GPIO_InitStructure);
    return 0;
}

int LedsDisplayOpen(void)
{
    return 0;
}

int LedsDisplayRead(void)
{
    return 0;
}

int LedsDisplayWrite(int data)
{    
    Send_Start();
    Send_Byte(DIS_DATA_CMD);    //command1
    Send_Byte(LIGHT_LEVEL(1)| DIS_TYPE(0) | DIS_ENABLE(1));   //command2
    Send_Stop();
    delay(1);
    Send_Start();
    Send_Byte(ADDR1); Send_Byte(dis_data_arry[data/1000]); //one byte
    Send_Stop();
    delay(5);
    Send_Start();
    Send_Byte(ADDR2); Send_Byte(dis_data_arry[(data%1000)/100]); //second byte
    Send_Stop();
    delay(5);
    Send_Start();
    Send_Byte(ADDR3); Send_Byte(dis_data_arry[((data%1000)%100)/10]); //third byte
    Send_Stop();
    delay(5);
    Send_Start();
    Send_Byte(ADDR4); Send_Byte(dis_data_arry[((data%1000)%100)%10]); //fourth byte
    Send_Stop();
    return 0;
}

int LedsDisplayCtrl(void)
{
    return 0;
}

int LedsDisplayClose(void)
{
    return 0;
}

