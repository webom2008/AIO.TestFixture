/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : driver_interAdc.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/22
  Last Modified :
  Description   : inter adc driver
  Function List :
  History       :
  1.Date        : 2015/9/22
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#include "includes.h"

//#define ADC_DMA_INTERRUPT_USED

/*----------------------------------------------*
 * external variables                           *
 *----------------------------------------------*/

/*----------------------------------------------*
 * external routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * internal routine prototypes                  *
 *----------------------------------------------*/
static void InterAdcCfg(void);
static void InterAdcCfgDMA(void);

/*----------------------------------------------*
 * project-wide global variables                *
 *----------------------------------------------*/

/*----------------------------------------------*
 * module-wide global variables                 *
 *----------------------------------------------*/
#ifdef ADC_DMA_INTERRUPT_USED
static xSemaphoreHandle xUpdateResultOpLock     = NULL; //update result operate lock
#endif

/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/

#define ADC1_DR_Address             ((uint32_t)0x4001244C)


typedef struct
{
    uint8_t         ch;
    uint8_t         source;
    uint16_t        pin;
    uint32_t        rcc;
    GPIO_TypeDef    *port;
} AdcChannels_TypeDef ;


static const AdcChannels_TypeDef InterAdcChannel[INTER_ADC_TOTAL] =
{
{   //INTER_ADC_D3V3E: ADC123_IN10
    ADC_Channel_10,
    GPIO_PinSource0,
    GPIO_Pin_0,
    RCC_APB2Periph_GPIOC,
    GPIOC,
},
{   //INTER_ADC_5V6N: ADC123_IN11
    ADC_Channel_11,
    GPIO_PinSource1,
    GPIO_Pin_1,
    RCC_APB2Periph_GPIOC,
    GPIOC,
},
{   //INTER_ADC_D3V3N: ADC123_IN12
    ADC_Channel_12,
    GPIO_PinSource2,
    GPIO_Pin_2,
    RCC_APB2Periph_GPIOC,
    GPIOC,
},
{   //INTER_ADC_5VAN: ADC123_IN13
    ADC_Channel_13,
    GPIO_PinSource3,
    GPIO_Pin_3,
    RCC_APB2Periph_GPIOC,
    GPIOC,
},
{   //INTER_ADC_5V_SPO2: ADC123_IN0
    ADC_Channel_0,
    GPIO_PinSource0,
    GPIO_Pin_0,
    RCC_APB2Periph_GPIOA,
    GPIOA,
},
{   //INTER_ADC_5V_NIBP: ADC123_IN1
    ADC_Channel_1,
    GPIO_PinSource1,
    GPIO_Pin_1,
    RCC_APB2Periph_GPIOA,
    GPIOA,
},
{   //INTER_ADC_REF2V5N: ADC12_IN6
    ADC_Channel_6,
    GPIO_PinSource6,
    GPIO_Pin_6,
    RCC_APB2Periph_GPIOA,
    GPIOA,
},
};


/*----------------------------------------------*
 * project-wide global variables                *
 *----------------------------------------------*/

/*----------------------------------------------*
 * internal variables                           *
 *----------------------------------------------*/
static __IO uint16_t RegularConvData_Tab[INTER_ADC_TOTAL];
static uint16_t gInterAdcResult[INTER_ADC_TOTAL];

/*----------------------------------------------*
 * internal routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

#ifdef ADC_DMA_INTERRUPT_USED
static void NVIC_Configuration(const FunctionalState state)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = IRQPriority13InterADC;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = state;
    NVIC_Init(&NVIC_InitStructure);
}
#endif

int InterAdcInit(void)
{
    GPIO_InitTypeDef    GPIO_InitStructure;
    
    /* ADCCLK 14MHz Max */
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);//72MHz/8=9MHz

    RCC_APB2PeriphClockCmd(InterAdcChannel[INTER_ADC_D3V3E].rcc \
                          | InterAdcChannel[INTER_ADC_5V6N].rcc\
                          | InterAdcChannel[INTER_ADC_D3V3N].rcc\
                          | InterAdcChannel[INTER_ADC_5VAN].rcc\
                          | InterAdcChannel[INTER_ADC_5V_SPO2].rcc\
                          | InterAdcChannel[INTER_ADC_5V_NIBP].rcc\
                          | InterAdcChannel[INTER_ADC_REF2V5N].rcc\
                          | RCC_APB2Periph_AFIO, ENABLE);

    /* Enable ADC1 clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    /* Enable DMA1 clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
    

    GPIO_InitStructure.GPIO_Pin     = InterAdcChannel[INTER_ADC_D3V3E].pin ;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AIN;	
    GPIO_Init(InterAdcChannel[INTER_ADC_D3V3E].port, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin     = InterAdcChannel[INTER_ADC_5V6N].pin ;
    GPIO_Init(InterAdcChannel[INTER_ADC_5V6N].port, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin     = InterAdcChannel[INTER_ADC_D3V3N].pin ;
    GPIO_Init(InterAdcChannel[INTER_ADC_D3V3N].port, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin     = InterAdcChannel[INTER_ADC_5VAN].pin ;
    GPIO_Init(InterAdcChannel[INTER_ADC_5VAN].port, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin     = InterAdcChannel[INTER_ADC_5V_SPO2].pin ;
    GPIO_Init(InterAdcChannel[INTER_ADC_5V_SPO2].port, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin     = InterAdcChannel[INTER_ADC_5V_NIBP].pin ;
    GPIO_Init(InterAdcChannel[INTER_ADC_5V_NIBP].port, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin     = InterAdcChannel[INTER_ADC_REF2V5N].pin ;
    GPIO_Init(InterAdcChannel[INTER_ADC_REF2V5N].port, &GPIO_InitStructure);
    
    InterAdcCfgDMA();
    InterAdcCfg();
    
#ifdef ADC_DMA_INTERRUPT_USED
    NVIC_Configuration(ENABLE);
    xUpdateResultOpLock = xSemaphoreCreateMutex();
    do{} while (NULL == xUpdateResultOpLock);
#endif
    
    return 0;
}

#ifndef ADC_DMA_INTERRUPT_USED
static void WaitInterAdcDone(void)
{
    /* Test DMA1 TC flag */
    while(RESET == (DMA_GetFlagStatus(DMA1_FLAG_TC1)));
    
    /* Clear DMA TC flag */
    DMA_ClearFlag(DMA1_FLAG_TC1);
}
#endif

int InterAdcOpen(void)
{
//    InterAdcCtrl(INTER_ADC_CMD_START,NULL);
    return 0;
}

int InterAdcRead(uint32_t *pResultArray, const int totalNums)
{
    uint32_t *pBuf = pResultArray;
    uint8_t index;
    
    if ((INTER_ADC_TOTAL != totalNums) || (NULL == pBuf)) return -1;
#ifdef ADC_DMA_INTERRUPT_USED
    if( pdTRUE != xSemaphoreTake( xUpdateResultOpLock, ( TickType_t ) 5 ))
    {
        return -2;
    }
#endif

#ifndef ADC_DMA_INTERRUPT_USED
    WaitInterAdcDone(); //Test
    memcpy((void *)gInterAdcResult, (void *)RegularConvData_Tab, sizeof(RegularConvData_Tab));
#endif

    for(index = 0; index < INTER_ADC_TOTAL; index++)
    {
        pBuf[index] = ((gInterAdcResult[index]* 3300) / 0xFFF); //mv
    }
#ifdef ADC_DMA_INTERRUPT_USED
    xSemaphoreGive( xUpdateResultOpLock);
#endif
    return 0;
}

int InterAdcWrite(char *pWriteData, const int nDataLen)
{
    return 0;
}

int InterAdcCtrl(const INTER_ADC_CTRL_CMD cmd, void *parameter)
{
    switch (cmd)
    {
    case INTER_ADC_CMD_START:{
        /* Start ADC1 Software Conversion */ 
        ADC_SoftwareStartConvCmd(ADC1, ENABLE);//Start ADC1 Software Conversion
    }
        break;
    case INTER_ADC_CMD_STOP:{
        ADC_SoftwareStartConvCmd(ADC1, DISABLE);
    }
        break;
    default:
        break;
    }
    return 0;
}

int InterAdcClose(void)
{
    ADC_SoftwareStartConvCmd(ADC1, DISABLE);
    ADC_Cmd(ADC1, DISABLE);
    ADC_DMACmd(ADC1, DISABLE);
    DMA_Cmd(DMA1_Channel1, DISABLE);
    
#ifdef ADC_DMA_INTERRUPT_USED
    NVIC_Configuration(DISABLE);
#endif

    return 0;
}

static void InterAdcCfg(void)
{
    ADC_InitTypeDef     ADC_InitStructure;

    /* ADC1 DeInit */  
    ADC_DeInit(ADC1);

    //ADC1 configuration
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = INTER_ADC_TOTAL;
    ADC_Init(ADC1, &ADC_InitStructure);

    /* Convert the ADC1 Channel11 and channel10 with 55.5 Cycles as sampling time */ 
    ADC_RegularChannelConfig(ADC1, InterAdcChannel[INTER_ADC_D3V3E].ch,   1,  ADC_SampleTime_55Cycles5);  
    ADC_RegularChannelConfig(ADC1, InterAdcChannel[INTER_ADC_5V6N].ch,    2,  ADC_SampleTime_55Cycles5);  
    ADC_RegularChannelConfig(ADC1, InterAdcChannel[INTER_ADC_D3V3N].ch,   3,  ADC_SampleTime_55Cycles5);  
    ADC_RegularChannelConfig(ADC1, InterAdcChannel[INTER_ADC_5VAN].ch,    4,  ADC_SampleTime_55Cycles5);  
    ADC_RegularChannelConfig(ADC1, InterAdcChannel[INTER_ADC_5V_SPO2].ch, 5,  ADC_SampleTime_55Cycles5);  
    ADC_RegularChannelConfig(ADC1, InterAdcChannel[INTER_ADC_5V_NIBP].ch, 6,  ADC_SampleTime_55Cycles5);  
    ADC_RegularChannelConfig(ADC1, InterAdcChannel[INTER_ADC_REF2V5N].ch, 7,  ADC_SampleTime_55Cycles5);

    //Enable ADC1 DMA
    ADC_DMACmd(ADC1, ENABLE);

    //Enable ADC1
    ADC_Cmd(ADC1, ENABLE);

    //Enable ADC1 reset calibration register  
    ADC_ResetCalibration(ADC1);
    //Check the end of ADC1 reset calibration register
    while(ADC_GetResetCalibrationStatus(ADC1));

    //Start ADC1 calibration
    ADC_StartCalibration(ADC1);
    //Check the end of ADC1 calibration
    while(ADC_GetCalibrationStatus(ADC1));
}

static void InterAdcCfgDMA(void)
{
    DMA_InitTypeDef   DMA_InitStructure;

    /* DMA1 Channel1 Config */
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)RegularConvData_Tab;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = INTER_ADC_TOTAL;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
    
#ifdef ADC_DMA_INTERRUPT_USED
    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
#endif

    /* Enable DMA1 channel1 */
    DMA_Cmd(DMA1_Channel1, ENABLE);
}

#ifdef ADC_DMA_INTERRUPT_USED
void DMA1_Channel1_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken, xResult;

    // xHigherPriorityTaskWoken must be initialised to pdFALSE.
    xHigherPriorityTaskWoken = pdFALSE;

    if(RESET != DMA_GetITStatus(DMA1_IT_TC1))
    {
        DMA_ClearITPendingBit(DMA1_IT_GL1);
        if( pdTRUE == xSemaphoreTakeFromISR( xUpdateResultOpLock, &xHigherPriorityTaskWoken))
        {
            memcpy((void *)gInterAdcResult, (void *)RegularConvData_Tab, sizeof(RegularConvData_Tab));
            xResult =xSemaphoreGiveFromISR( xUpdateResultOpLock ,&xHigherPriorityTaskWoken);
        }
    }
    if( xResult == pdPASS )
    {
        // If xHigherPriorityTaskWoken is now set to pdTRUE then a context
        // switch should be requested.  The macro used is port specific and 
        // will be either portYIELD_FROM_ISR() or portEND_SWITCHING_ISR() - 
        // refer to the documentation page for the port being used.
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }
}
#endif

