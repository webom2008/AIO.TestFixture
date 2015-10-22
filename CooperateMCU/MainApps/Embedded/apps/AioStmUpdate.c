/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : AioStmUpdate.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/14
  Last Modified :
  Description   : file for update AioStm32
  Function List :
  History       :
  1.Date        : 2015/9/14
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#include "includes.h"
#include "AioStmUpdate.h"

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
static AioStmDev_TypeDef gAioStmDev;
AioStmDev_TypeDef *gpAioStmDev = &gAioStmDev;


const uint8_t ACK           = 0x79;
const uint8_t NACK          = 0x1F;

#define TICKTOWAIT          1000
#define SIZE_WRITE          128

#define Start_Address       0x0801C004
uint32_t Write_Address      = 0x08000000 - SIZE_WRITE;
uint32_t Read_Address       = 0x08000000 - SIZE_WRITE;
uint8_t Start_CMD           = 0x7F;
uint8_t Get_CMD[2]          = {0x00, 0xFF};
uint8_t Go_CMD[2]           = {0x21, 0xDE};
uint8_t WR_CMD[2]           = {0x31, 0xCE};
uint8_t RD_CMD[2]           = {0x11, 0xEE};
uint8_t ER_CMD[2]           = {0x43, 0xBC};
uint8_t ER_ALL_CMD[2]       = {0xFF, 0x00};

uint8_t Write_Data[SIZE_WRITE] = {0};
uint8_t Read_Data[SIZE_WRITE]  = {0};

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








#define _INFO_
#define _ERROR_

#ifdef _INFO_
#define INFO(fmt, arg...) udprintf("[AioStm]Info: "fmt, ##arg)
#else
#define INFO(fmt, arg...) do{}while(0)
#endif
#ifdef _ERROR_
#define ERROR(fmt, arg...) udprintf("[AioStm]Error: "fmt, ##arg)
#else
#define ERROR(fmt, arg...) do{}while(0)
#endif

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/



int AioStmOpen(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(STM_UART_CTL_RCC | STM_BOOT0_CTL_RCC,ENABLE);

    GPIO_InitStructure.GPIO_Pin = STM_UART_CTL_PIN;      
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
    GPIO_Init(STM_UART_CTL_PORT, &GPIO_InitStructure);	

    GPIO_InitStructure.GPIO_Pin = STM_BOOT0_CTL_PIN;
    GPIO_Init(STM_BOOT0_CTL_PORT, &GPIO_InitStructure);	
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


static void clearUartRxQueue(void)
{
    int rLen = 0;
    
    do
    {
        rLen = 0;
        rLen = Uart3Read((char *)Read_Data, sizeof(Read_Data));
    } while(rLen > 0);
        
    xQueueReset(gpAioStmDev->pAioStmUpdateRxQueue);
}

static int isGetACKSignal(void)
{
    uint8_t Rev_Data = 0;
    xQueueReceive(gpAioStmDev->pAioStmUpdateRxQueue,&Rev_Data,TICKTOWAIT);
    clearUartRxQueue();
    vTaskDelay(1000);
    if(ACK != Rev_Data)
    {
        return 0; //failed reback
    }
    return 1;
}

/*****************************************************************************
 Prototype    : Erase_Operation
 Description  : erase operation for AIO-STM
 Input        : void  
 Output       : None
 Return Value : 0 :failed       , 1 : success
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2015/9/15
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
static int Erase_Operation(void)
{
    Uart3Write((char *)ER_CMD,2);
    if(!isGetACKSignal())
    {
        return 0; //failed reback
    }

    Uart3Write((char *)ER_ALL_CMD,2);
    if(!isGetACKSignal())
    {
        return 0; //failed reback
    }

    return 1;
}

void AioStmUpdateTask(void *pvParameter)
{
    uint8_t temp = 0;
    uint32_t length = 0;
    uint32_t i = 0,j = 0;
    uint8_t Count = 0;
    uint8_t CheckSum = 0;
    uint8_t Get_Info_Data[15] = {0};
    uint8_t Address[5] = {0}; // 4 Bytes Address + 1 Bytes checksum
    uint8_t isError = 0;
    DmaUartProtocolPacket txPacket;

    INFO("AioStmUpdateTask running...\r\n");
    while(1)
    {
        if (0 != isError) //Error Happend, send pkt to tell Main MCU know
        {
            DmaUartProtocolPacketInit(&txPacket);
            txPacket.ID = PKT_ID_AIOSTM_UPDATE_ERROR;
            txPacket.DataLen = 1;
            txPacket.Data[0] = isError;
            txPacket.ACK = DMA_UART_PACKET_NACK;
            sendMainMcuPkt(&txPacket, 0);
            isError = 0;
        }
        
        if (pdTRUE != xSemaphoreTake(gpAioStmDev->pStartUpdateSemaphore, DELAY_MAX_WAIT))
        {
            //timeout, nothing to do!
            continue;
        }

        INFO("pStartUpdateSemaphore==start\r\n");
        clearUartRxQueue();//clear queue is important here. a bug will happan if not do it.

        //S1: ==========>start
        Uart3Write((char *)&Start_CMD,1);   //to select AIO-STM bootloader USARTx mode
        if(!isGetACKSignal())
        {
            isError = 1;
            ERROR("start error\r\n");
            continue; //failed, reback
        }

        //S2: ==========>get command
        Uart3Write((char *)Get_CMD,2);
        Count = 0;
        while(Count != 15)
        {
            xQueueReceive(gpAioStmDev->pAioStmUpdateRxQueue,&Get_Info_Data[Count++],TICKTOWAIT);
        }
        clearUartRxQueue();
        if((ACK == Get_Info_Data[0]) && (ACK == Get_Info_Data[14]))
        {
            RD_CMD[0] = Get_Info_Data[6];
            RD_CMD[1] = 0xFF - RD_CMD[0];
            Go_CMD[0] = Get_Info_Data[7];
            Go_CMD[1] = 0xFF - Go_CMD[0];
            WR_CMD[0] = Get_Info_Data[8];
            WR_CMD[1] = 0xFF - WR_CMD[0];
            ER_CMD[0] = Get_Info_Data[9];
            ER_CMD[1] = 0xFF - ER_CMD[0];
        }
        else
        {
            isError = 2;
            ERROR("get cmd error\r\n");
            continue; //faild reback
        }
        INFO("get cmd success\r\n");

        //S3: ==========>erase command
        if(Erase_Operation() == 0)
        {
            ERROR("erase error\r\n");
            continue;//failed reback
        }
        INFO("erase success\r\n");

        //S4: ==========>write command
        Count = 0;
        Write_Address = 0x08000000 - SIZE_WRITE;
        length = *(uint32_t *)(Start_Address-4);
        for(i = 0; i < length; i++)
        {
            Write_Data[Count++] = *(uint8_t *)(Start_Address+i);
            if(Count == SIZE_WRITE || (i == (length -1))) //a frame or end
            {
                Write_Address += SIZE_WRITE;
                Count = 0;
                /****---------------------------------------------------***/
                Uart3Write((char *)WR_CMD,2); //send command
                if(!isGetACKSignal())
                {
                    isError = 4;
                    ERROR("WR_CMD error\r\n");
                    break; //failed reback
                }

                Address[0] = (uint8_t)(Write_Address >> 24);
                Address[1] = (uint8_t)(Write_Address >> 16);
                Address[2] = (uint8_t)(Write_Address >> 8);
                Address[3] = (uint8_t)(Write_Address >> 0);
                Address[4] = (Address[0]^Address[1]^Address[2]^Address[3]);

                Uart3Write((char *)Address,5); //send address
                if(!isGetACKSignal())
                {
                    isError = 5;
                    ERROR("Send Addr error\r\n");
                    break; //failed reback
                }


                CheckSum = SIZE_WRITE -1;
                for(j = 0; j < SIZE_WRITE; j++)
                    CheckSum = (CheckSum ^ Write_Data[j]);

                temp = SIZE_WRITE - 1;
                Uart3Write((char *)&temp, 1); //send byte to write
                Uart3Write((char *)Write_Data, SIZE_WRITE);
                Uart3Write((char *)&CheckSum, 1);
                if(!isGetACKSignal())
                {
                    isError = 6;
                    ERROR("Write_Data error\r\n");
                    break; //failed reback
                }
                memset(Write_Data, 0xFF, sizeof(Write_Data));
                /*****------------------------------------------------***/
            }
        }
        if(i < length) //write failed,erase it
        {
            ERROR("write-total error\r\n");
            Erase_Operation();
            isError = 7;
            continue;//failed reback
        }
        INFO("write-total success\r\n");

        //S4: ==========>read command for vertify
        Count = 0;
        Read_Address = 0x08000000 - SIZE_WRITE;
        length = *(uint32_t *)(Start_Address-4);
        i = 0;
        while(i < length)
        {
            Read_Address += SIZE_WRITE;
            Count = 0;
            /****---------------------------------------------------***/
            Uart3Write((char *)RD_CMD,2); //send  command
            if(!isGetACKSignal())
            {
                isError = 6;
                ERROR("RD_CMD error\r\n");
                break; //failed reback
            }

            Address[0] = (uint8_t)(Read_Address >> 24);
            Address[1] = (uint8_t)(Read_Address >> 16);
            Address[2] = (uint8_t)(Read_Address >> 8);
            Address[3] = (uint8_t)(Read_Address >> 0);
            Address[4] = (Address[0]^Address[1]^Address[2]^Address[3]);

            Uart3Write((char *)Address,5); //send address
            if(!isGetACKSignal())
            {
                isError = 5;
                ERROR("Send Addr error\r\n");
                break; //failed reback
            }

            CheckSum = 0xFF ^ (SIZE_WRITE -1);
            temp = SIZE_WRITE -1;
            Uart3Write((char *)&temp, 1);//send byte to read
            Uart3Write((char *)&CheckSum, 1);//send checksum

            if(!isGetACKSignal())
            {
                isError = 5;
                ERROR("Send Addr error\r\n");
                break; //failed reback
            }

            while(Count != SIZE_WRITE)
            {
                xQueueReceive(gpAioStmDev->pAioStmUpdateRxQueue,&Read_Data[Count++],TICKTOWAIT);
            }

            for(Count = 0; Count < SIZE_WRITE; Count++)
            {
                if(Read_Data[Count] != *(uint8_t *)(Start_Address+i))
                break;//failed 
                i++;
            }
            if(Count != SIZE_WRITE)
            break; //failed vertify

            memset(Read_Data, 0xFF, sizeof(Read_Data));
            /*****------------------------------------------------***/
        }
        if(i < length) //vertify failed,erase it
        {
            ERROR("Read error\r\n");
            Erase_Operation();
            isError = 5;
            continue;//failed reback
        }
        INFO("Read success\r\n");

        //S5: ==========>go command
        Uart3Write((char *)Go_CMD,2);
        if(!isGetACKSignal())
        {
            isError = 5;
            ERROR("go command\r\n");
            continue; //failed reback
        }

        Write_Address = 0x08000000;
        Address[0] = (uint8_t)(Write_Address >> 24);
        Address[1] = (uint8_t)(Write_Address >> 16);
        Address[2] = (uint8_t)(Write_Address >> 8);
        Address[3] = (uint8_t)(Write_Address >> 0);
        Address[4] = (Address[0]^Address[1]^Address[2]^Address[3]);

        Uart3Write((char *)Address,5); //send address
        if(!isGetACKSignal())
        {
            isError = 5;
            ERROR("go command\r\n");
            continue; //failed reback
        }
        INFO("go command\r\n");

        DmaUartProtocolPacketInit(&txPacket);
        txPacket.ID = PKT_ID_AIOSTM_UPDATE_END;
        txPacket.DataLen = 0;
        txPacket.ACK = DMA_UART_PACKET_ACK;
        sendMainMcuPkt(&txPacket, MY_TIM_DEFAULT_TIMEOUT_MS);
    }
}


int AioStmUpdateInit(void)
{
    int ret = 0;
    
    ret = AioStmOpen();
    gpAioStmDev->pAioStmUpdateRxQueue = xQueueCreate(256, sizeof(char));
    gpAioStmDev->pStartUpdateSemaphore = xSemaphoreCreateBinary();
    
    if ((NULL == gpAioStmDev->pAioStmUpdateRxQueue) \
        || (NULL == gpAioStmDev->pStartUpdateSemaphore))
    {
        ERROR("Create pAioStmUpdateRxQueue Failed!");
        ret = -1;
    }
    
    while (ret < 0); //error hanppen
    return 0;
}

