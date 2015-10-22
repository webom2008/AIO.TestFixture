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
const uint32_t BinInFlashAddr= 0x0801C000;
const uint32_t BinLen       = 0x4000;
const uint8_t Start_CMD     = 0x7F;
const uint8_t Get_CMD[2]    = {0x00, 0xFF};
const uint8_t ER_ALL_CMD[2] = {0xFF, 0x00};

#define SIZE_WRITE          128

static uint32_t Write_Address      = 0x08000000 - SIZE_WRITE;
static uint32_t Read_Address       = 0x08000000 - SIZE_WRITE;
static uint8_t Go_CMD[2]           = {0x21, 0xDE};
static uint8_t WR_CMD[2]           = {0x31, 0xCE};
static uint8_t RD_CMD[2]           = {0x11, 0xEE};
static uint8_t ER_CMD[2]           = {0x43, 0xBC};

static uint8_t Write_Data[SIZE_WRITE] = {0};
static uint8_t Read_Data[SIZE_WRITE]  = {0};

/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/
enum
{
    STATE_UPDATE_WAIT_SINGLE,
    STATE_UPDATE_PREPEARE,
    STATE_UPDATE_START_CMD,
    STATE_UPDATE_GET_CMD,
    STATE_UPDATE_ERASE_CMD,
    STATE_UPDATE_DOWNLOAD,
    STATE_UPDATE_READ_CHECK,
    STATE_UPDATE_GO_CMD,
    STATE_UPDATE_SUCCESS,
    STATE_UPDATE_ERROR,
    
    STATE_UPDATE_UNVALID,
};

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/

//#define _INFO_
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
static int isGetACKSignal(void)
{
    uint8_t Rev_Data = 0;
    Uart3Read((char *)&Rev_Data,1);
//    Uart3Ctrl(Uart3CTRL_ClearRxQueue, NULL);
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

static int downloadBin(void)
{
    uint8_t temp = 0;
    uint32_t i = 0,j = 0;
    uint8_t CheckSum = 0;
    uint8_t Count = 0;
    uint8_t Address[5] = {0}; // 4 Bytes Address + 1 Bytes checksum
    
    Write_Address = 0x08000000 - SIZE_WRITE;
    for(i = 0; i < BinLen; i++)
    {
        Write_Data[Count++] = *(uint8_t *)(BinInFlashAddr+i);
        if((SIZE_WRITE == Count) || ((BinLen -1) == i)) //a frame or end
        {
            Write_Address += SIZE_WRITE;
            Count = 0;
            /****---------------------------------------------------***/
            Uart3Write((char *)WR_CMD,2); //send command
            if(!isGetACKSignal())
            {
                ERROR("WR_CMD error\r\n");
                return -1;
            }

            Address[0] = (uint8_t)(Write_Address >> 24);
            Address[1] = (uint8_t)(Write_Address >> 16);
            Address[2] = (uint8_t)(Write_Address >> 8);
            Address[3] = (uint8_t)(Write_Address >> 0);
            Address[4] = (Address[0]^Address[1]^Address[2]^Address[3]);

            Uart3Write((char *)Address,5); //send address
            if(!isGetACKSignal())
            {
                ERROR("Send Addr error\r\n");
                return -2;
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
                ERROR("Write_Data error\r\n");
                return -3;
            }
            memset(Write_Data, 0xFF, sizeof(Write_Data));
            /*****------------------------------------------------***/
        }
    }
    if(i < BinLen) //write failed,erase it
    {
        ERROR("write-total error\r\n");
        Erase_Operation();
        return -4;
    }
    return 0;
}

static int readAndCheckBin(void)
{
    uint8_t Count = 0;
    uint8_t temp = 0;
    uint32_t i = 0;
    uint8_t CheckSum = 0;
    uint8_t Address[5] = {0}; // 4 Bytes Address + 1 Bytes checksum
    
    Read_Address = 0x08000000 - SIZE_WRITE;
    while(i < BinLen)
    {
        Read_Address += SIZE_WRITE;
        Count = 0;
        /****---------------------------------------------------***/
        Uart3Ctrl(Uart3CTRL_ClearRxQueue, NULL);
        Uart3Write((char *)RD_CMD,2); //send  command
        if(!isGetACKSignal())
        {
            ERROR("RD_CMD error\r\n");
            return -1;
        }

        Address[0] = (uint8_t)(Read_Address >> 24);
        Address[1] = (uint8_t)(Read_Address >> 16);
        Address[2] = (uint8_t)(Read_Address >> 8);
        Address[3] = (uint8_t)(Read_Address >> 0);
        Address[4] = (Address[0]^Address[1]^Address[2]^Address[3]);

        Uart3Write((char *)Address,5); //send address
        if(!isGetACKSignal())
        {
            ERROR("Send Addr error\r\n");
            return -2;
        }

        CheckSum = 0xFF ^ (SIZE_WRITE -1);
        temp = SIZE_WRITE -1;
        Uart3Write((char *)&temp, 1);//send byte to read
        Uart3Write((char *)&CheckSum, 1);//send checksum

        if(!isGetACKSignal())
        {
            ERROR("Send len and checksum error\r\n");
            return -3;
        }

        Count = Uart3Read((char *)&Read_Data, SIZE_WRITE);
        if (SIZE_WRITE != Count)
        {
            ERROR("read data timeout\r\n");
            return -4;
        }

        for(Count = 0; Count < SIZE_WRITE; Count++)
        {
            if(Read_Data[Count] != *(uint8_t *)(BinInFlashAddr+i))
            {
                break;//failed 
            }
            i++;
        }
        
        if(SIZE_WRITE != Count)
        {
            break; //failed vertify
        }
        else
        {
            INFO("vertify %d OK\r\n",i);
        }

        memset(Read_Data, 0xFF, sizeof(Read_Data));
        /*****------------------------------------------------***/
    }
    if(i < BinLen) //vertify failed,erase it
    {
        ERROR("vertify failed,erase it\r\n");
        Erase_Operation();
        return -5;//failed reback
    }
    INFO("Read success\r\n");
    return 0;
}

void AioStmUpdateTask(void *pvParameter)
{
    char s8Val;
    uint8_t Count = 0;
    uint8_t Get_Info_Data[15] = {0};
    uint8_t Address[5] = {0}; // 4 Bytes Address + 1 Bytes checksum
    int state = STATE_UPDATE_WAIT_SINGLE;
    DmaUartProtocolPacket txPacket;

    INFO("AioStmUpdateTask running...\r\n");
    while(1)
    {
        switch(state)
        {
        case STATE_UPDATE_WAIT_SINGLE:{
            if (pdTRUE == xSemaphoreTake(gpAioStmDev->pStartUpdateSemaphore, DELAY_MAX_WAIT))
            {
                state = STATE_UPDATE_PREPEARE;
            }
        }break;
        case STATE_UPDATE_PREPEARE:{
            s8Val = PIN_HIGH;
            AioStmCtrl(AIO_STM_CTRL_CMD_SET_UART, &s8Val);
            vTaskDelay(1000);//delay for aio_stm_ctrl
            INFO("pStartUpdateSemaphore==start\r\n");
            //clear queue is important here. a bug will happan if not do it.
            Uart3Ctrl(Uart3CTRL_ClearRxQueue, NULL);
            state = STATE_UPDATE_START_CMD;
        }break;
        case STATE_UPDATE_START_CMD:{
            Uart3Write((char *)&Start_CMD,1);   //to select AIO-STM bootloader USARTx mode
            if(!isGetACKSignal())
            {
                state = STATE_UPDATE_ERROR;
                ERROR("Start_CMD error\r\n");
            }
            else
            {
                state = STATE_UPDATE_GET_CMD;
            }
        }break;
        case STATE_UPDATE_GET_CMD:{
            Uart3Ctrl(Uart3CTRL_ClearRxQueue, NULL);
            Uart3Write((char *)Get_CMD,2);
            Count = Uart3Read((char *)Get_Info_Data, sizeof(Get_Info_Data));
            if(sizeof(Get_Info_Data) != Count)
            {
                state = STATE_UPDATE_ERROR;
                ERROR("Get_CMD timeout\r\n");
                break;  //Note@ debug wait time
            }
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
                INFO("get cmd success\r\n");
                state = STATE_UPDATE_ERASE_CMD;
            }
            else
            {
                state = STATE_UPDATE_ERROR;
                ERROR("Get_CMD error\r\n");
            }
        }break;
        case STATE_UPDATE_ERASE_CMD:{
            if(0 != Erase_Operation())
            {
                INFO("erase success\r\n");
                state = STATE_UPDATE_DOWNLOAD;
            }
            else
            {
                ERROR("erase error\r\n");
                state = STATE_UPDATE_ERROR;
            }
        }break;
        case STATE_UPDATE_DOWNLOAD:{
            if(downloadBin() < 0)
            {
                ERROR("download_bin error\r\n");
                state = STATE_UPDATE_ERROR;
            }
            else
            {
                INFO("download_bin success\r\n");
                state = STATE_UPDATE_READ_CHECK;
            }
        }break;
        case STATE_UPDATE_READ_CHECK:{
            if(readAndCheckBin() < 0)
            {
                ERROR("readAndCheckBin error\r\n");
                state = STATE_UPDATE_ERROR;
            }
            else
            {
                INFO("readAndCheckBin success\r\n");
                state = STATE_UPDATE_GO_CMD;
            }
        }break;
        case STATE_UPDATE_GO_CMD:{
            Uart3Write((char *)Go_CMD,2);
            if(!isGetACKSignal())
            {
                ERROR("Go_CMD error\r\n");
                state = STATE_UPDATE_ERROR;
                break;
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
                ERROR("Go_CMD Address error\r\n");
                state = STATE_UPDATE_ERROR;
            }
            else
            {
                INFO("go command success!\r\n");
                state = STATE_UPDATE_SUCCESS;
            }
        }break;
        case STATE_UPDATE_SUCCESS:{
            DmaUartProtocolPacketInit(&txPacket);
            txPacket.ID = PKT_ID_AIOSTM_UPDATE_END;
            txPacket.DataLen = 0;
            txPacket.ACK = DMA_UART_PACKET_NACK;
            sendMainMcuPkt(&txPacket, MY_TIM_DEFAULT_TIMEOUT_MS);
            s8Val = PIN_LOW;
            AioStmCtrl(AIO_STM_CTRL_CMD_SET_UART, &s8Val);
            state = STATE_UPDATE_WAIT_SINGLE;
        }break;
        case STATE_UPDATE_ERROR:{
            //Error Happend, send pkt to tell Main MCU know
            DmaUartProtocolPacketInit(&txPacket);
            txPacket.ID = PKT_ID_AIOSTM_UPDATE_ERROR;
            txPacket.DataLen = 1;
            txPacket.Data[0] = 0x00; //reserved
            txPacket.ACK = DMA_UART_PACKET_NACK;
            sendMainMcuPkt(&txPacket, 0);
            s8Val = PIN_LOW;
            AioStmCtrl(AIO_STM_CTRL_CMD_SET_UART, &s8Val);
            state = STATE_UPDATE_WAIT_SINGLE;

        }break;
        default:{

        } break;
        }
    }
}


int AioStmUpdateInit(void)
{
    int ret = 0;
    
    ret = AioStmOpen();
    gpAioStmDev->pStartUpdateSemaphore = xSemaphoreCreateBinary();
    
    if (NULL == gpAioStmDev->pStartUpdateSemaphore)
    {
        ret = -1;
    }
    
    while (ret < 0); //error hanppen
    return 0;
}

