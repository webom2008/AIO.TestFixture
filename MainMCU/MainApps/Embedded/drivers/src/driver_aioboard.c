/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : driver_aioboard.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/16
  Last Modified :
  Description   : driver for aio board
  Function List :
  History       :
  1.Date        : 2015/9/16
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#include "includes.h"

/*----------------------------------------------*
 * external variables                           *
 *----------------------------------------------*/
extern EventGroupHandle_t xUart4RxEventGroup;

/*----------------------------------------------*
 * external routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * internal routine prototypes                  *
 *----------------------------------------------*/
static QueueHandle_t    xpReceiveQueueHandle = NULL;



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
#define PWR_CTL_PIN     GPIO_Pin_11
#define PWR_CTL_PORT    GPIOA
#define PWR_CTL_RCC     RCC_APB2Periph_GPIOA


#define RECEIVE_QUEUE_LENTGH        256


//#define _INFO_
#define _ERROR_

#ifdef _INFO_
#define INFO(fmt, arg...) udprintf("[MainMCU]Info: "fmt, ##arg)
#else
#define INFO(fmt, arg...) do{}while(0)
#endif
#ifdef _ERROR_
#define ERROR(fmt, arg...) udprintf("[MainMCU]Error: "fmt, ##arg)
#else
#define ERROR(fmt, arg...) do{}while(0)
#endif
/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

static void AioBoardGpioInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(PWR_CTL_RCC, ENABLE );
    
	/* Configure USARTx Rx as input floating */
	GPIO_InitStructure.GPIO_Pin = PWR_CTL_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(PWR_CTL_PORT, &GPIO_InitStructure );
}

int AioBoardInit(void)
{
    AioBoardGpioInit();
    xpReceiveQueueHandle = xQueueCreate(RECEIVE_QUEUE_LENTGH, sizeof(char));

    do {}while(NULL == xpReceiveQueueHandle);
    return 0;
}

int AioBoardOpen(void)
{
    return 0;
}

int AioBoardRead(char *pReadData, const int nDataLen)
{
    int i;
    
    for (i=0; i < nDataLen; i++)
    {
		if(pdPASS != xQueueReceive(xpReceiveQueueHandle, pReadData++, (TickType_t)10))
		{
            break;
		}
    }
    return i;
}

int AioBoardWrite(char *pWriteData, const int nDataLen)
{
    return Uart4Read(pWriteData, nDataLen);
}

int AioBoardCtrl(CTRL_CMD_AIOBOARD cmd, void *pData)
{
    char *pVal = (char *)pData;

    switch(cmd)
    {
    case CTRL_CMD_AIOBOARD_SET_POWER:
        if (SW_ON == *pVal)
        {
            GPIO_SetBits(PWR_CTL_PORT, PWR_CTL_PIN);
        }
        else
        {
            GPIO_ResetBits(PWR_CTL_PORT, PWR_CTL_PIN);
        }
        break;
    default:
        break;
    }
    return 0;
}

int AioBoardClose(void)
{
    return 0;
}


static void CoopAioBoardReadDriverTask(void *pvParameters)
{
    int rLen = 0;
    char rxBuf[UART4_RX_DMA_BUF_LEN];
    char *pChar = NULL;
    EventBits_t uxBits;
    
	/* Just to stop compiler warnings. */
	( void ) pvParameters;
    
    INFO("CoopAioBoardReadDriverTask running...\n");
    for (;;)
    {
        rLen = 0;
		uxBits = xEventGroupWaitBits(
					xUart4RxEventGroup,	// The event group being tested.
					UART_DMA_RX_COMPLETE_EVENT_BIT \
					| UART_DMA_RX_INCOMPLETE_EVENT_BIT,	// The bits within the event group to wait for.
					pdTRUE,			// BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
					pdFALSE,		// Don't wait for both bits, either bit will do.
					DELAY_MAX_WAIT );	// Wait a maximum of 100ms for either bit to be set.

        memset(rxBuf, 0x00, UART4_RX_DMA_BUF_LEN);
        if (0 != ( uxBits & UART_DMA_RX_COMPLETE_EVENT_BIT ) \
            || (0 != ( uxBits & UART_DMA_RX_COMPLETE_EVENT_BIT)))
		{
            rLen = Uart4Read(rxBuf, UART4_RX_DMA_BUF_LEN);
            pChar = &rxBuf[0];
            while(rLen--)
            {
                xQueueSendToBack(xpReceiveQueueHandle, (void *)pChar++, DELAY_NO_WAIT);
            }
		}
    }
}

int createAioBoardTask(void)
{
#ifndef CONFIG_DRIVER_TEST_UART4
    xTaskCreate(CoopAioBoardReadDriverTask,
                "CoopAioBoardReadDriverTask",
                configMINIMAL_STACK_SIZE,
                NULL,
                AIOBOARD_DRIVER_TASK_PRIORITY,
                NULL);
#endif /* CONFIG_DRIVER_TEST_UART3 */
    return 0;
}