/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : driver_dac.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/22
  Last Modified :
  Description   : driver for dac8568
  Function List :
  History       :
  1.Date        : 2015/9/22
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
static int spi_write_byte(const u8 byte);

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

//GENERAL DATA FORMAT FOR 12-BIT DAC7568
//	 			 |					 |				 	 |			 DB16 |	DB15
//DB31 	DB30-DB28 DB27 DB26 DB25 DB24 DB23 DB22 DB21 DB20 DB19 DB18 DB17  DB16-DB10  DB9 DB8 DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
//	0 		NC     C3    C2  C1   C0   A3   A2   A1   A0   D12 D11  D10    D9-D3      D2  D1  X   X   X   X  F3  F2  F1  F0

//GENERAL DATA FORMAT FOR 14-BIT DAC8168
//	0	    NC	   C3    C2  C1   C0   A3   A2   A1   A0   D14 D13  D12    D11-D5 	  D4  D3  D2  D1  X   X  F3  F2  F1  F0

//GENERAL DATA FORMAT FOR 16-BIT DAC8568
//	0       NC     C3    C2  C1   C0   A3   A2   A1   A0   D16 D15  D14    D13-D7 	  D6  D5  D4  D3  D2  D1 F3  F2  F1  F0

#define DAC_FRAME_FEATURE_OFFSET        ((uint8_t) 0)
#define DAC_FRAME_ADDR_OFFSET           ((uint8_t) 20)
#define DAC_FRAME_CTRL_OFFSET           ((uint8_t) 24)
#define DAC_FRAME_PREFIX_OFFSET         ((uint8_t) 28)

#define DAC_CTRL_WRITE_INPUT_REG        ((uint8_t) 0x00)
#define DAC_CTRL_UPDATE_REG             ((uint8_t) 0x01)
#define DAC_CTRL_W_UPDATE_ALL_REG       ((uint8_t) 0x02) //Write to Selected DAC Input Register and Update All DAC Registers
#define DAC_CTRL_W_UPDATE_SPEC_REG      ((uint8_t) 0x03) //Write to Selected DAC Input Register and Update Respective DAC Register
#define DAC_CTRL_WRITE_CLEAR_CODE_REG   ((uint8_t) 0x05)
#define DAC_CTRL_WRITE_LDAC             ((uint8_t) 0x06)
#define DAC_CTRL_SF_RESET               ((uint8_t) 0x07)

#ifdef _DAC7568_
#define DAC_VAL_MAX                     ((uint16_t) 0x0FFF)
#define DAC_FRAME_DATA_OFFSET           ((uint8_t) 8)
#endif
#ifdef _DAC8168_
#define DAC_VAL_MAX                     ((uint16_t) 0x3FFF)
#define DAC_FRAME_DATA_OFFSET           ((uint8_t) 6)
#endif
#ifdef _DAC8568_
#define DAC_VAL_MAX                     ((uint16_t) 0xFFFF)
#define DAC_FRAME_DATA_OFFSET           ((uint8_t) 4)
#endif  

#define DAC_MAX_VOLT_MV                 (5000.0f)

#define DAC_VAL_MID                     ((uint16_t) (DAC_VAL_MAX / 2))
#define DAC_VAL_MIN                     ((uint16_t) 0)

//SPI1 interrfaces
#define DAC_8568_SPIx           SPI1
#define DAC_8568_SPIx_RCC       RCC_APB2Periph_SPI1
#define DAC_8568_SCLK_RCC       RCC_APB2Periph_GPIOA
#define DAC_8568_SCLK_PORT      GPIOA
#define DAC_8568_SCLK_PIN       GPIO_Pin_5
#define DAC_8568_MOSI_RCC       RCC_APB2Periph_GPIOA
#define DAC_8568_MOSI_PORT      GPIOA
#define DAC_8568_MOSI_PIN       GPIO_Pin_7

//GPIO interrfaces
#define DAC_8568_CLR_RCC        RCC_APB2Periph_GPIOC
#define DAC_8568_CLR_PORT       GPIOC
#define DAC_8568_CLR_PIN        GPIO_Pin_5
#define DAC_8568_LDAC_RCC       RCC_APB2Periph_GPIOC
#define DAC_8568_LDAC_PORT      GPIOC
#define DAC_8568_LDAC_PIN       GPIO_Pin_4
#define DAC_8568_SYNC_RCC       RCC_APB2Periph_GPIOA
#define DAC_8568_SYNC_PORT      GPIOA
#define DAC_8568_SYNC_PIN       GPIO_Pin_4

#define DAC_8568_SYNC_LOW()     GPIO_ResetBits(DAC_8568_SYNC_PORT, DAC_8568_SYNC_PIN)
#define DAC_8568_SYNC_HIGH()    GPIO_SetBits(DAC_8568_SYNC_PORT, DAC_8568_SYNC_PIN)
#define DAC_8568_CLR_LOW()      GPIO_ResetBits(DAC_8568_CLR_PORT, DAC_8568_CLR_PIN)
#define DAC_8568_CLR_HIGH()     GPIO_SetBits(DAC_8568_CLR_PORT, DAC_8568_CLR_PIN)
#define DAC_8568_LDAC_LOW()     GPIO_ResetBits(DAC_8568_LDAC_PORT, DAC_8568_LDAC_PIN)
#define DAC_8568_LDAC_HIGH()    GPIO_SetBits(DAC_8568_LDAC_PORT, DAC_8568_LDAC_PIN)

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

static __INLINE void delay_ns(const u32 ns)
{
    u32 i;
    for(i= 0; i < ns; i++);
}

static void SPIx_ConfigureInit(void)
{
    SPI_InitTypeDef  SPI_InitStructure;
    
    SPI_Cmd(DAC_8568_SPIx, DISABLE);
    
    SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                           //Note@: software NSS
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256; //72MHz /256 = 281Khz, 50MHz max
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(DAC_8568_SPIx, &SPI_InitStructure);
    
    /* Enable SPI_MASTER */
    SPI_Cmd(DAC_8568_SPIx, ENABLE);

    DAC_8568_SYNC_HIGH();
}

int Dac8568Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // GPIO Init
    RCC_APB2PeriphClockCmd( DAC_8568_CLR_RCC \
                            | DAC_8568_LDAC_RCC \
                            | DAC_8568_SYNC_RCC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = DAC_8568_CLR_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_Init(DAC_8568_CLR_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = DAC_8568_LDAC_PIN;
    GPIO_Init(DAC_8568_LDAC_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = DAC_8568_SYNC_PIN;
    GPIO_Init(DAC_8568_SYNC_PORT, &GPIO_InitStructure);

    //SPI1 IO Init
    RCC_APB2PeriphClockCmd( DAC_8568_SPIx_RCC, ENABLE);
    RCC_APB2PeriphClockCmd( DAC_8568_SCLK_RCC \
                            | DAC_8568_MOSI_RCC \
                            | RCC_APB2Periph_AFIO, ENABLE);
    GPIO_InitStructure.GPIO_Pin = DAC_8568_SCLK_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
    GPIO_Init(DAC_8568_SCLK_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = DAC_8568_MOSI_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
    GPIO_Init(DAC_8568_MOSI_PORT, &GPIO_InitStructure);

    SPIx_ConfigureInit();
    
    return 0;
}

static void Dac8568PowerupInit(void)
{
    uint32_t val = 0;
//    uint8_t u8Val = 0;
    OpDacRegister_TypeDef OpDacReg;

    DAC_8568_LDAC_HIGH();
    DAC_8568_CLR_HIGH();
    Dac8568Ctrl(DAC_CTRL_CLR_PULSE, NULL);
    Dac8568Ctrl(DAC_CTRL_W_SF_RESET, NULL);
    delay_ns(10000);
    
    OpDacReg.channel = DAC_CH_ALL;
    OpDacReg.val = PWRDOWN_CMD_UP;
    Dac8568Ctrl(DAC_CTRL_W_PWRDOWN, &OpDacReg); //enable DAC out

    val = RefPowerUpDAC_StaticMode;
    Dac8568Ctrl(DAC_CTRL_W_INT_REF, &val);//DAC reference enable

//    u8Val = DAC_CLEAR_IGNORE_PIN;
//    Dac8568Ctrl(DAC_CTRL_W_CLEAR_CODE_REG, &u8Val);
//    u8Val = DAC_CLEAR_TO_MID;
//    Dac8568Ctrl(DAC_CTRL_W_CLEAR_CODE_REG, &u8Val);

    OpDacReg.channel = DAC_CH_ALL;
    OpDacReg.val = DAC_VAL_MID;
    Dac8568Ctrl(DAC_CTRL_W_IN_UP_RES, &OpDacReg);
}

int Dac8568Open(void)
{
    Dac8568PowerupInit();
    return 0;
}

//static int Dac8568Read(void)
//{
//    return 0;
//}

static int Dac8568Write(const uint32_t oneFrame)
{
    DAC_8568_SYNC_LOW();
    delay_ns(13);           //t5 at least 13ns
    spi_write_byte((u8)((oneFrame>>24)&0xFF));
    spi_write_byte((u8)((oneFrame>>16)&0xFF));
    spi_write_byte((u8)((oneFrame>>8)&0xFF));
    spi_write_byte((u8)(oneFrame&0xFF));
    
    delay_ns(500);          //@note: Wait for SPI send last byte
    delay_ns(10);           //t8 at least 10ns 
    DAC_8568_SYNC_HIGH();
    delay_ns(80);           // Minimum SYNC HIGH time at least 80ns 
    return 0;
}


int Dac8568Ctrl(const DacCtrlCmd_TypeDef cmd, void *paramter)
{
    OpDacRegister_TypeDef *pOpDacReg = (OpDacRegister_TypeDef *)paramter;
    uint32_t *pU32Val = (uint32_t *)paramter;
    uint8_t *pU8Val = (uint8_t *)paramter;
    uint32_t oneFrame = 0;
    
    switch (cmd)
    {
    case DAC_CTRL_W_INPUT_REG:{
        pOpDacReg = (OpDacRegister_TypeDef *)paramter;
        if (NULL == pOpDacReg) return -1;
        oneFrame = (uint32_t)(DAC_CTRL_WRITE_INPUT_REG << DAC_FRAME_CTRL_OFFSET);
        oneFrame |= (uint32_t)(pOpDacReg->channel << DAC_FRAME_ADDR_OFFSET);
        oneFrame |= (uint32_t)((pOpDacReg->val&0xFFFF) << DAC_FRAME_DATA_OFFSET);
        Dac8568Write(oneFrame);
        DAC_8568_LDAC_LOW();
        delay_ns(80);       //LDAC pulse width LOW time,t12 at least 80ns 
        DAC_8568_LDAC_HIGH();
    }
        break;
    case DAC_CTRL_W_UPDATE_REG:{
        if (NULL == pOpDacReg) return -1;
        oneFrame = (uint32_t)(DAC_CTRL_UPDATE_REG << DAC_FRAME_CTRL_OFFSET);
        oneFrame |= (uint32_t)(pOpDacReg->channel << DAC_FRAME_ADDR_OFFSET);
        oneFrame |= (uint32_t)((pOpDacReg->val&0xFFFF) << DAC_FRAME_DATA_OFFSET);
        Dac8568Write(oneFrame);
        DAC_8568_LDAC_LOW();
        delay_ns(80);       //LDAC pulse width LOW time,t12 at least 80ns 
        DAC_8568_LDAC_HIGH();
    }
        break;
    case DAC_CTRL_W_CLEAR_CODE_REG:{
        if (NULL == pU8Val) return -1;
        oneFrame = (uint32_t)(DAC_CTRL_WRITE_CLEAR_CODE_REG << DAC_FRAME_CTRL_OFFSET);
        oneFrame |= (uint32_t)((*pU8Val & 0x03) << DAC_FRAME_FEATURE_OFFSET);
        Dac8568Write(oneFrame);
    }
        break;
    case DAC_CTRL_W_LDAC_REG:{
        //TODO
    }
        break;
    case DAC_CTRL_W_SF_RESET:{
        oneFrame = (uint32_t)(DAC_CTRL_SF_RESET << DAC_FRAME_CTRL_OFFSET);
        Dac8568Write(oneFrame);
    }
        break;
    case DAC_CTRL_W_IN_UP_ALL:{
        if (NULL == pOpDacReg) return -1;
        oneFrame = (uint32_t)(DAC_CTRL_W_UPDATE_ALL_REG << DAC_FRAME_CTRL_OFFSET);
        oneFrame |= (uint32_t)(pOpDacReg->channel << DAC_FRAME_ADDR_OFFSET);
        oneFrame |= (uint32_t)((pOpDacReg->val & 0xFFFF) << DAC_FRAME_DATA_OFFSET);
        Dac8568Write(oneFrame);
        DAC_8568_LDAC_LOW();
        delay_ns(80);       //LDAC pulse width LOW time,t12 at least 80ns 
        DAC_8568_LDAC_HIGH();

    }
        break;
    case DAC_CTRL_W_IN_UP_RES:{
        if (NULL == pOpDacReg) return -1;
        oneFrame = (uint32_t)(DAC_CTRL_W_UPDATE_SPEC_REG << DAC_FRAME_CTRL_OFFSET);
        oneFrame |= (uint32_t)(pOpDacReg->channel << DAC_FRAME_ADDR_OFFSET);
        oneFrame |= (uint32_t)((pOpDacReg->val&0xFFFF) << DAC_FRAME_DATA_OFFSET);
        Dac8568Write(oneFrame);
        DAC_8568_LDAC_LOW();
        delay_ns(80);       //LDAC pulse width LOW time,t12 at least 80ns 
        DAC_8568_LDAC_HIGH();

    }
        break;
    case DAC_CTRL_W_PWRDOWN:{
        if (NULL == pOpDacReg) return -1;
        oneFrame = pOpDacReg->val;
        if (DAC_CH_ALL ==  pOpDacReg->channel)
        {
            oneFrame |= (uint32_t)0x000000FF;
        }
        else
        {
            oneFrame |= (uint32_t)(0x01 << pOpDacReg->channel);
        }
        Dac8568Write(oneFrame);

    }
        break;
    case DAC_CTRL_W_INT_REF:{
        if (NULL == pU32Val) return -1;
        oneFrame = *pU32Val;
        Dac8568Write(oneFrame);

    }
        break;
    case DAC_CTRL_CLR_PULSE:{
        DAC_8568_CLR_LOW();
        delay_ns(80);           //CLR pulse width LOW time,t15 at least 80ns 
        DAC_8568_CLR_HIGH();
    }
        break;
    default:
        break;
    }
    return 0;
}

int Dac8568Close(void)
{
    /* Enable SPI_MASTER */
    SPI_Cmd(DAC_8568_SPIx, DISABLE);
    return 0;
}

//0xFFFF -- 5V
//0x0000 -- 0V

uint16_t Dac8568mV2Dac(uint16_t mV)
{
    uint32_t dac;
    if (mV > (uint16_t)DAC_MAX_VOLT_MV)
    {
        mV = (uint16_t)DAC_MAX_VOLT_MV;
    }
    dac = (uint32_t)(DAC_VAL_MAX / DAC_MAX_VOLT_MV *mV);
    return (uint16_t)dac;
}

//SPI interrface
static int spi_write_byte(const u8 byte)
{
    /*!< Loop while DR register in not emplty */
    while (RESET == SPI_I2S_GetFlagStatus(DAC_8568_SPIx, SPI_I2S_FLAG_TXE));

    /*!< Send byte through the SPI1 peripheral */
    SPI_I2S_SendData(DAC_8568_SPIx, byte);

    /*!< Loop while DR register in not emplty */
    while (RESET == SPI_I2S_GetFlagStatus(DAC_8568_SPIx, SPI_I2S_FLAG_TXE));
    
    return 0;
//    //SPI_Direction_1Line_Tx not need to receive
//    /*!< Wait to receive a byte */
//    while (RESET == SPI_I2S_GetFlagStatus(DAC_8568_SPIx, SPI_I2S_FLAG_RXNE));

//    /*!< Return the byte read from the SPI bus */
//    return SPI_I2S_ReceiveData(DAC_8568_SPIx);
}

