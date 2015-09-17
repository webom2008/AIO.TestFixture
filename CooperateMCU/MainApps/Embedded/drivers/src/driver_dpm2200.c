/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : driver_dpm2200.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/17
  Last Modified :
  Description   : driver for High percision Digital Pressure Meters
  Function List :
  History       :
  1.Date        : 2015/9/17
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
 * macros                                       *
 *----------------------------------------------*/
#define WRITE_CMD_BUFFER_LEN            9
#define READ_CMD_BUFFER_LEN             4

/*----------------------------------------------*
 * module-wide global variables                 *
 *----------------------------------------------*/
static char cmd_write_buffer[WRITE_CMD_BUFFER_LEN];         //exp. W0600004<CR>
static char cmd_write_echo_buufer[WRITE_CMD_BUFFER_LEN+1];  //exp. W0600004<CR><LF>
static char cmd_read_buffer[READ_CMD_BUFFER_LEN];           //exp. R08<CR>
static char cmd_read_echo_buufer[READ_CMD_BUFFER_LEN+1];    //exp. R08<CR><LF>

static const char SEND_END_TAG[] = {0x0D};            // <CR>
static const char RESPONSE_END_TAG[] = {0x0D,0x0A}; // <CR><LF>

/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/
int DPM2200Init(void)
{
    return 0;
}

int DPM2200Open(void)
{
    return 0;
}

static void cleanUartRxBuffer(void)
{
    int rLen = 0;
    char rBuf[10];
    do
    {
        rLen = Uart1Read(rBuf, sizeof(rBuf));
    }while(0 != rLen);
}

/*****************************************************************************
 Prototype    : DPM2200Read
 Description  : read DPM2200
 Input        : const int location  
                char *pReadData     
                const int nDataLen  
 Output       : None
 Return Value : 0 : OK,     <0 : Error happen
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2015/9/17
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
int DPM2200Read(const int location, char *pReadData, const int nDataLen)
{
    int len = 0, fail_cnt = 0;
    char respon = 0;
    
    sprintf(cmd_read_buffer, "R%02d", location);
    cmd_read_buffer[READ_CMD_BUFFER_LEN-1] = SEND_END_TAG[0];

    cleanUartRxBuffer();
    Uart1Write(cmd_read_buffer, READ_CMD_BUFFER_LEN);

    //read echoed value
    memset(cmd_read_echo_buufer, 0x00, sizeof(cmd_read_echo_buufer));
    for(len = 0, fail_cnt = 0; ((len < sizeof(cmd_read_echo_buufer)) && (fail_cnt < 10)); )
    {
        if (1 == Uart1Read(&respon, 1))
        {
            cmd_read_echo_buufer[len++] = respon;
        }
        else
        {
            fail_cnt++;
        }
    }
    if ((RESPONSE_END_TAG[0] == cmd_read_echo_buufer[READ_CMD_BUFFER_LEN-1]) \
        && (RESPONSE_END_TAG[1] == cmd_read_echo_buufer[READ_CMD_BUFFER_LEN]))
    {
        if (0 == memcmp(cmd_read_buffer, cmd_read_echo_buufer, WRITE_CMD_BUFFER_LEN))
        {
            // read data return
            for(len = 0, fail_cnt = 0, respon = 0;\
                ((len < nDataLen) && (RESPONSE_END_TAG[1] != respon)&& (fail_cnt < 100)); )
            {
                if (1 == Uart1Read(&respon, 1))
                {
                    pReadData[len++] = respon;
                }
                else
                {
                    fail_cnt++;
                }
            }
            if (RESPONSE_END_TAG[1] == respon) return 0;
            else return -3;
        }
        else
        {
            return -2;
        }
    }
    return -1;
}

/*****************************************************************************
 Prototype    : DPM2200Write
 Description  : write DPM2200
 Input        : const int location  
                const int data      
 Output       : None
 Return Value : 0 : sucess      <0 : error
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2015/9/17
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
int DPM2200Write(const int location, const int data)
{
    int len = 0, fail_cnt = 0;
    char respon = 0;
    
    sprintf(cmd_write_buffer, "W%02d%05d", location, data);
    cmd_write_buffer[WRITE_CMD_BUFFER_LEN-1] = SEND_END_TAG[0];

    cleanUartRxBuffer();
    Uart1Write(cmd_write_buffer, WRITE_CMD_BUFFER_LEN);

    memset(cmd_write_echo_buufer, 0x00, sizeof(cmd_write_echo_buufer));
    for(len = 0, fail_cnt = 0; ((len < sizeof(cmd_write_echo_buufer)) && (fail_cnt < 20)); )
    {
        if (1 == Uart1Read(&respon, 1))
        {
            cmd_write_echo_buufer[len++] = respon;
        }
        else
        {
            fail_cnt++;
        }
    }
    if ((RESPONSE_END_TAG[0] == cmd_write_echo_buufer[WRITE_CMD_BUFFER_LEN-1]) \
        && (RESPONSE_END_TAG[1] == cmd_write_echo_buufer[WRITE_CMD_BUFFER_LEN]))
    {
        if (0 == memcmp(cmd_write_buffer, cmd_write_echo_buufer, WRITE_CMD_BUFFER_LEN))
        {
            return 0;
        }
    }
    return -1;
}

int DPM2200Ctrl(void)
{
    return 0;
}

int DPM2200Close(void)
{
    return 0;
}

