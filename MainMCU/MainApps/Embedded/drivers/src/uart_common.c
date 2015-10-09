/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : uart_common.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/8/26
  Last Modified :
  Description   : uart common handle
  Function List :
  History       :
  1.Date        : 2015/8/26
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#include "uart_common.h"
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

typedef struct
{
    USART_TypeDef   *UARTx;
    uint32_t        DataRegister;
    uint32_t        RCC_Periph_UARTx;
    uint32_t        RCC_Periph_UARTx_Tx;
    uint32_t        RCC_Periph_UARTx_Rx;
    uint32_t        RCC_Periph_UARTx_AFIO;
    GPIO_TypeDef    *PORT_UARTx_Tx;
    GPIO_TypeDef    *PORT_UARTx_Rx;
    uint16_t        PIN_UARTx_Tx;
    uint16_t        PIN_UARTx_Rx;
    uint8_t         NVIC_IRQChannel;
} UART_GPIO_DEF;

const UART_GPIO_DEF UART_GPIO[UART_NUM_MAX] = 
{
    //<!-- UART01 -->
    {
        USART1,
        0x40013804,
        RCC_APB2Periph_USART1,
        RCC_APB2Periph_GPIOA,
        RCC_APB2Periph_GPIOA,
        RCC_APB2Periph_AFIO,
        GPIOA,
        GPIOA,
        GPIO_Pin_9,
        GPIO_Pin_10,
        USART1_IRQn
    },
    
    //<!-- UART02 -->
    {
        USART2,
        0x40004404,
        RCC_APB1Periph_USART2,
        RCC_APB2Periph_GPIOA,
        RCC_APB2Periph_GPIOA,
        RCC_APB2Periph_AFIO,
        GPIOA,
        GPIOA,
        GPIO_Pin_2,
        GPIO_Pin_3,
        USART2_IRQn
    },
    
    //<!-- UART03 -->
    {
        USART3,
        0x40004804,
        RCC_APB1Periph_USART3,
        RCC_APB2Periph_GPIOB,
        RCC_APB2Periph_GPIOB,
        RCC_APB2Periph_AFIO,
        GPIOB,
        GPIOB,
        GPIO_Pin_10,
        GPIO_Pin_11,
        USART3_IRQn

    },
    
    //<!-- UART04 -->
    {
        UART4,
        0x40004C04,
        RCC_APB1Periph_UART4,
        RCC_APB2Periph_GPIOC,
        RCC_APB2Periph_GPIOC,
        RCC_APB2Periph_AFIO,
        GPIOC,
        GPIOC,
        GPIO_Pin_10,
        GPIO_Pin_11,
        UART4_IRQn
    },
    
    //<!-- UART05 -->
    {
        UART5,
        0x40005004,
        RCC_APB1Periph_UART5,
        RCC_APB2Periph_GPIOC,
        RCC_APB2Periph_GPIOD,
        RCC_APB2Periph_AFIO,
        GPIOC,
        GPIOD,
        GPIO_Pin_12,
        GPIO_Pin_2,
        UART5_IRQn
    },
};

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/


static int UartCommonGPIOInit(const UART_NUM_DEF uart_num)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    if (uart_num > UART_NUM_MAX)
    {
        return -1;
    }
    
	/* Enable USARTx clock */
    if (UART_NUM01 == uart_num)
    {
    	RCC_APB2PeriphClockCmd( UART_GPIO[uart_num].RCC_Periph_UARTx,ENABLE );
    }
    else
    {
        RCC_APB1PeriphClockCmd( UART_GPIO[uart_num].RCC_Periph_UARTx,ENABLE );
    }
	RCC_APB2PeriphClockCmd( UART_GPIO[uart_num].RCC_Periph_UARTx_Tx \
	                        | UART_GPIO[uart_num].RCC_Periph_UARTx_Rx \
	                        | UART_GPIO[uart_num].RCC_Periph_UARTx_AFIO,
	                        ENABLE );	

	/* Configure USARTx Rx as input floating */
	GPIO_InitStructure.GPIO_Pin = UART_GPIO[uart_num].PIN_UARTx_Rx;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init( UART_GPIO[uart_num].PORT_UARTx_Rx, &GPIO_InitStructure );
	
	/* Configure USARTx Tx s alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = UART_GPIO[uart_num].PIN_UARTx_Tx;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init( UART_GPIO[uart_num].PORT_UARTx_Tx, &GPIO_InitStructure );
    return 0;
}


static int UartCommonConfig(const UART_DEVICE_TypeDef *pUartDevice)
{
    USART_InitTypeDef USART_InitStructure;
    
    if (pUartDevice->num > UART_NUM_MAX)
    {
        return -1;
    }
    
	USART_InitStructure.USART_BaudRate = pUartDevice->baudrate;
    if (PARITY_EVEN == pUartDevice->ParityType)
    {
    	USART_InitStructure.USART_WordLength = USART_WordLength_9b;
    	USART_InitStructure.USART_Parity = USART_Parity_Even;
    }
    else if (PARITY_ODD == pUartDevice->ParityType)
    {
    	USART_InitStructure.USART_WordLength = USART_WordLength_9b;
    	USART_InitStructure.USART_Parity = USART_Parity_Odd;
    }
    else
    {
    	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    	USART_InitStructure.USART_Parity = USART_Parity_No;
    }
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	USART_Init(UART_GPIO[pUartDevice->num].UARTx, &USART_InitStructure );

    if (PARITY_NONE != pUartDevice->ParityType)
    {
        //enable parity error interrupt
        USART_ITConfig(UART_GPIO[pUartDevice->num].UARTx, USART_IT_PE , ENABLE);
    }

    if (UART_INTERRUPT_MODE == pUartDevice->mode)
    {
        USART_Cmd(UART_GPIO[pUartDevice->num].UARTx, ENABLE);
    }
    else
    {
        //enable idle interrupt
        USART_ITConfig(UART_GPIO[pUartDevice->num].UARTx, USART_IT_IDLE , ENABLE);
        
        USART_Cmd(UART_GPIO[pUartDevice->num].UARTx, ENABLE);
        
        //fixed CPU bugs: if send data after uart config, the first char don't send.
        USART_ClearFlag(UART_GPIO[pUartDevice->num].UARTx, USART_FLAG_TC);
    }

    return 0;
}


static int UartCommonNVICInit(const UART_DEVICE_TypeDef *pUartDevice)
{
    uint8_t Uartx_Rx_DMA_IRQn;
    uint8_t Uartx_Tx_DMA_IRQn;
    NVIC_InitTypeDef NVIC_InitStructure;

    // Enable the USARTx Interrupt
    NVIC_InitStructure.NVIC_IRQChannel = UART_GPIO[pUartDevice->num].NVIC_IRQChannel;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = pUartDevice->IRQPriority;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init( &NVIC_InitStructure );
    
    if (UART_INTERRUPT_MODE == pUartDevice->mode)
    {
        USART_ITConfig(UART_GPIO[pUartDevice->num].UARTx, USART_IT_RXNE, ENABLE);
    }
    else
    {
        switch (pUartDevice->num)
        {
        case UART_NUM01:
            Uartx_Tx_DMA_IRQn = DMA1_Channel4_IRQn;
            Uartx_Rx_DMA_IRQn = DMA1_Channel5_IRQn;
            break;
        case UART_NUM02:
            Uartx_Tx_DMA_IRQn = DMA1_Channel7_IRQn;
            Uartx_Rx_DMA_IRQn = DMA1_Channel6_IRQn;
            break;
        case UART_NUM03:
            Uartx_Tx_DMA_IRQn = DMA1_Channel2_IRQn;
            Uartx_Rx_DMA_IRQn = DMA1_Channel3_IRQn;
            break;
        case UART_NUM04:
            Uartx_Tx_DMA_IRQn = DMA2_Channel4_5_IRQn; // -QWB-:For STM32F10X_HD
            Uartx_Rx_DMA_IRQn = DMA2_Channel3_IRQn;
            break;
        default:
            return -1;
        }

        NVIC_InitStructure.NVIC_IRQChannel = Uartx_Tx_DMA_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = pUartDevice->IRQPriority;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);

        NVIC_InitStructure.NVIC_IRQChannel = Uartx_Rx_DMA_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = pUartDevice->IRQPriority;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
    }
    return 0;
}

static int UartCommonDMAConfig(const UART_DEVICE_TypeDef *pUartDevice)
{
    DMA_Channel_TypeDef *UARTx_Tx_DMA;
    DMA_Channel_TypeDef *UARTx_Rx_DMA;
    DMA_InitTypeDef DMA_InitStruct;
    if ((NULL == pUartDevice->pTxDMABuffer) \
        || (NULL == pUartDevice->pRxDMABuffer) \
        || (NULL == pUartDevice->pTxDMABuffer->pPingPongBuff1) \
        || (NULL == pUartDevice->pTxDMABuffer->pPingPongBuff2) \
        || (NULL == pUartDevice->pRxDMABuffer->pPingPongBuff1)\
        || (NULL == pUartDevice->pRxDMABuffer->pPingPongBuff2))
    {
        return -1;
    }
    
    /* DMA clock enable */
    if (pUartDevice->num < UART_NUM04)
    {
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);//DMA1
    }
    else if (UART_NUM04 == pUartDevice->num)
    {
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);//DMA2
    }
    else
    {
        return -1;
    }
    
    switch (pUartDevice->num)
    {
    case UART_NUM01:
        UARTx_Tx_DMA = DMA1_Channel4;
        UARTx_Rx_DMA = DMA1_Channel5;
        break;
    case UART_NUM02:
        UARTx_Tx_DMA = DMA1_Channel7;
        UARTx_Rx_DMA = DMA1_Channel6;
        break;
    case UART_NUM03:
        UARTx_Tx_DMA = DMA1_Channel2;
        UARTx_Rx_DMA = DMA1_Channel3;
        break;
    case UART_NUM04:
        UARTx_Tx_DMA = DMA2_Channel5;
        UARTx_Rx_DMA = DMA2_Channel3;
        break;
    default:
        return -1;
    }
    
    DMA_DeInit(UARTx_Tx_DMA);
    DMA_StructInit(&DMA_InitStruct);
    DMA_InitStruct.DMA_PeripheralBaseAddr = UART_GPIO[pUartDevice->num].DataRegister;
    DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)pUartDevice->pTxDMABuffer->pPingPongBuff1;
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStruct.DMA_BufferSize = pUartDevice->pTxDMABuffer->nBuff1MaxLength;
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;//send once  //DMA_Mode_Normal, DMA_Mode_Circular
    DMA_InitStruct.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(UARTx_Tx_DMA, &DMA_InitStruct);
    
    DMA_ITConfig(UARTx_Tx_DMA, DMA_IT_TC, ENABLE);
//    DMA_ITConfig(UARTx_Tx_DMA, DMA_IT_TE, ENABLE);
    
    USART_DMACmd(UART_GPIO[pUartDevice->num].UARTx, USART_DMAReq_Tx, ENABLE);
    DMA_Cmd(UARTx_Tx_DMA, DISABLE); //NOTE: Start by usr when need send data
    pUartDevice->pTxDMABuffer->IsDMAWroking = 0;
    
    DMA_DeInit(UARTx_Rx_DMA);
    DMA_StructInit(&DMA_InitStruct);
    DMA_InitStruct.DMA_PeripheralBaseAddr = UART_GPIO[pUartDevice->num].DataRegister;
    DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)pUartDevice->pRxDMABuffer->pPingPongBuff1;
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStruct.DMA_BufferSize = pUartDevice->pRxDMABuffer->nBuff1MaxLength;
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;//received once  //DMA_Mode_Normal, DMA_Mode_Circular
    DMA_InitStruct.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(UARTx_Rx_DMA, &DMA_InitStruct);
    DMA_ITConfig(UARTx_Rx_DMA, DMA_IT_TC, ENABLE);
//    DMA_ITConfig(UARTx_Rx_DMA, DMA_IT_TE, ENABLE);

    /* Enable USARTx DMA RX request */
    USART_DMACmd(UART_GPIO[pUartDevice->num].UARTx, USART_DMAReq_Rx, ENABLE);
    DMA_Cmd(UARTx_Rx_DMA, ENABLE);
    pUartDevice->pRxDMABuffer->IsDMAWroking = 1;
    return 0;
}


int UartDeviceDefaultInit(UART_DEVICE_TypeDef *pUartDevice)
{
    pUartDevice->IsDeviceOpen   = false;
    pUartDevice->num            = UART_NUM01;
    pUartDevice->mode           = UART_INTERRUPT_MODE;
    pUartDevice->baudrate       = B115200;
    pUartDevice->ParityType     = PARITY_NONE;
    pUartDevice->pRxDMABuffer   = NULL;
    pUartDevice->pTxDMABuffer   = NULL;
    pUartDevice->IRQPriority    = 15; //lowest level
    pUartDevice->IsDeviceOpen   = false;
    return 0;
}

int UartCommonInit(const UART_DEVICE_TypeDef *pUartDevice)
{
    if (UartCommonGPIOInit(pUartDevice->num) < 0)
    {
        return -1;
    }

    if (UartCommonNVICInit(pUartDevice) < 0)
    {
        return -1;
    }
    
    if (UART_DMA_MODE == pUartDevice->mode)
    {
        if (UartCommonDMAConfig(pUartDevice) < 0)
        {
            return -1;
        }
    }
    
    if (UartCommonConfig(pUartDevice) < 0)
    {
        return -1;
    }
    return 0;
}

static void UartCommonNVICTerminate(const UART_DEVICE_TypeDef *pUartDevice)
{
    uint8_t Uartx_Rx_DMA_IRQn;
    uint8_t Uartx_Tx_DMA_IRQn;
    NVIC_InitTypeDef NVIC_InitStructure;

    // Disable the USARTx Interrupt
    NVIC_InitStructure.NVIC_IRQChannel = UART_GPIO[pUartDevice->num].NVIC_IRQChannel;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = pUartDevice->IRQPriority;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init( &NVIC_InitStructure );
    
    if (UART_INTERRUPT_MODE == pUartDevice->mode)
    {
        USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
    }
    else
    {
        switch (pUartDevice->num)
        {
        case UART_NUM01:
            Uartx_Tx_DMA_IRQn = DMA1_Channel4_IRQn;
            Uartx_Rx_DMA_IRQn = DMA1_Channel5_IRQn;
            break;
        case UART_NUM02:
            Uartx_Tx_DMA_IRQn = DMA1_Channel7_IRQn;
            Uartx_Rx_DMA_IRQn = DMA1_Channel6_IRQn;
            break;
        case UART_NUM03:
            Uartx_Tx_DMA_IRQn = DMA1_Channel2_IRQn;
            Uartx_Rx_DMA_IRQn = DMA1_Channel3_IRQn;
            break;
        case UART_NUM04:
            Uartx_Tx_DMA_IRQn = DMA2_Channel4_5_IRQn; // -QWB-:For STM32F10X_HD
            Uartx_Rx_DMA_IRQn = DMA2_Channel3_IRQn;
            break;
        default:
            return ;
        }

        NVIC_InitStructure.NVIC_IRQChannel = Uartx_Tx_DMA_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = pUartDevice->IRQPriority;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
        NVIC_Init(&NVIC_InitStructure);

        NVIC_InitStructure.NVIC_IRQChannel = Uartx_Rx_DMA_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = pUartDevice->IRQPriority;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
        NVIC_Init(&NVIC_InitStructure);
    }
}

static void UartCommonDMATerminate(const UART_DEVICE_TypeDef *pUartDevice)
{
    DMA_Channel_TypeDef *UARTx_Tx_DMA;
    DMA_Channel_TypeDef *UARTx_Rx_DMA;
    
    /* DMA clock enable */
    if (pUartDevice->num < UART_NUM04)
    {
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, DISABLE);//DMA1
    }
    else if (UART_NUM04 == pUartDevice->num)
    {
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, DISABLE);//DMA2
    }
    else
    {
        return ;
    }
    
    switch (pUartDevice->num)
    {
    case UART_NUM01:
        UARTx_Tx_DMA = DMA1_Channel4;
        UARTx_Rx_DMA = DMA1_Channel5;
        break;
    case UART_NUM02:
        UARTx_Tx_DMA = DMA1_Channel7;
        UARTx_Rx_DMA = DMA1_Channel6;
        break;
    case UART_NUM03:
        UARTx_Tx_DMA = DMA1_Channel2;
        UARTx_Rx_DMA = DMA1_Channel3;
        break;
    case UART_NUM04:
        UARTx_Tx_DMA = DMA2_Channel5;
        UARTx_Rx_DMA = DMA2_Channel3;
        break;
    default:
        return;
    }
    
    DMA_DeInit(UARTx_Tx_DMA);
    DMA_ITConfig(UARTx_Tx_DMA, DMA_IT_TC, DISABLE);
    DMA_ITConfig(UARTx_Tx_DMA, DMA_IT_TE, DISABLE);
    USART_DMACmd(UART_GPIO[pUartDevice->num].UARTx, USART_DMAReq_Tx, DISABLE);
    DMA_Cmd(UARTx_Tx_DMA, DISABLE);
    
    DMA_DeInit(UARTx_Rx_DMA);
    DMA_ITConfig(UARTx_Rx_DMA, DMA_IT_TC, DISABLE);
    DMA_ITConfig(UARTx_Rx_DMA, DMA_IT_TE, DISABLE);
    USART_DMACmd(UART_GPIO[pUartDevice->num].UARTx, USART_DMAReq_Rx, DISABLE);
    DMA_Cmd(UARTx_Rx_DMA, DISABLE);
}

static void UartCommonUARTTerminate(const UART_DEVICE_TypeDef *pUartDevice)
{
    if (PARITY_NONE != pUartDevice->ParityType)
    {
        //enable parity error interrupt
        USART_ITConfig(UART_GPIO[pUartDevice->num].UARTx, USART_IT_PE , DISABLE);
    }
    
    if (UART_INTERRUPT_MODE == pUartDevice->mode)
    {
        USART_Cmd(UART_GPIO[pUartDevice->num].UARTx, DISABLE);
    }
    else
    {
        USART_ITConfig(UART_GPIO[pUartDevice->num].UARTx, USART_IT_IDLE , DISABLE); //enable idle interrupt
        USART_Cmd(UART_GPIO[pUartDevice->num].UARTx, DISABLE);
    }
}

int UartCommonTerminate(const UART_DEVICE_TypeDef *pUartDevice)
{
    UartCommonUARTTerminate(pUartDevice);
    if (UART_DMA_MODE == pUartDevice->mode)
    {
        UartCommonDMATerminate(pUartDevice);
    }
    UartCommonNVICTerminate(pUartDevice);
    return 0;
}

