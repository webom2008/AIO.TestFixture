/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : driver_dac.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/22
  Last Modified :
  Description   : driver_dac.c header file
  Function List :
  History       :
  1.Date        : 2015/9/22
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/

#ifndef __DRIVER_DAC_H__
#define __DRIVER_DAC_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


//#define _DAC7568_
//#define _DAC8168_
#define _DAC8568_


#define DAC_CH_A        ((uint8_t) 0x00)
#define DAC_CH_B        ((uint8_t) 0x01)
#define DAC_CH_C        ((uint8_t) 0x02)
#define DAC_CH_D        ((uint8_t) 0x03)
#define DAC_CH_E        ((uint8_t) 0x04)
#define DAC_CH_F        ((uint8_t) 0x05)
#define DAC_CH_G        ((uint8_t) 0x06)
#define DAC_CH_H        ((uint8_t) 0x07)
#define DAC_CH_ALL      ((uint8_t) 0x0F)


#define DAC_CLEAR_TO_ZERO       ((uint8_t) 0x00)
#define DAC_CLEAR_TO_MID        ((uint8_t) 0x01)
#define DAC_CLEAR_TO_FULL       ((uint8_t) 0x02)
#define DAC_CLEAR_IGNORE_PIN    ((uint8_t) 0x03)

#define PWRDOWN_CMD_UP          ((uint32_t)((0x04 << 24)|(0x00 << 8)))
#define PWRDOWN_CMD_1K_GND      ((uint32_t)((0x04 << 24)|(0x01 << 8)))
#define PWRDOWN_CMD_100K_GND    ((uint32_t)((0x04 << 24)|(0x02 << 8)))
#define PWRDOWN_CMD_Z_GND       ((uint32_t)((0x04 << 24)|(0x03 << 8)))

//Internal Reference Commands
#define RefPowerDown_StaticMode     0x08000000//Power down internal reference - static mode (default), must use external reference to operate device
#define RefPowerUpDAC_StaticMode    0x08000001//Power up internal reference - static mode,when all DACs power down,the reference power down,when  any dac  power up,the reference  power up
#define RefPowerUpDAC_FlexibleMode  0x09080000//Power up internal reference - flexible mode,when all DACs power down,the reference power down,when  any dac  power up,the reference  power up
#define RefPowerUp_FlexibleMode     0x090A0000//Power up internal reference all the time regardless of state of DACs - flexible mode,
#define RefPowerDown_FlexibleMode   0x090C0000//Power down internal reference all the time regardless of state of DACs - flexible mode,must use external reference to operate device
#define RefFlexibleToStatic         0x09000000//switching internal reference mode from flexible  mode to static  mode

typedef enum
{
    DAC_CTRL_W_INPUT_REG   = 0,     //Write to Selected DAC Input Register
    DAC_CTRL_W_UPDATE_REG,          //Update Selected DAC Registers
    DAC_CTRL_W_CLEAR_CODE_REG,      //Write to Clear Code Register
    DAC_CTRL_W_LDAC_REG,            //Write to LDAC Register
    DAC_CTRL_W_SF_RESET,            //Software Reset
    DAC_CTRL_W_IN_UP_ALL,           //Write to Selected DAC Input Register and Update All DAC Registers
    DAC_CTRL_W_IN_UP_RES,           //Write to Selected DAC Input Register and Update Respective DAC Register
    DAC_CTRL_W_PWRDOWN,             //Power-Down Commands
    DAC_CTRL_W_INT_REF,             //Internal Reference Commands
    DAC_CTRL_CLR_PULSE,             //CLR pin pulse width LOW  time

    DAC_CTRL_W_RESERVED             //Reserved 
} DacCtrlCmd_TypeDef;

typedef struct
{
    uint8_t     channel;
    uint32_t    val;
} OpDacRegister_TypeDef;

int Dac8568Init(void);
int Dac8568Open(void);
/*int Dac8568Read(void);*/
/*int Dac8568Write(void);*/
int Dac8568Ctrl(const DacCtrlCmd_TypeDef cmd, void *paramter);
int Dac8568Close(void);
uint16_t Dac8568mV2Dac(uint16_t mV);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DRIVER_DAC_H__ */
