/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : AioDspProtocol.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/17
  Last Modified :
  Description   : AioDspProtocol.c header file
  Function List :
  History       :
  1.Date        : 2015/9/17
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#ifndef __AIODSPPROTOCOL_H__
#define __AIODSPPROTOCOL_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */



typedef enum
{
  
    /*----------- SpO2 Start ------------*/
    SpO2_MODEL_VERSION_ID	    = 0x10,
    SpO2_ALARM_INFO_ID		    = 0x11,
    SpO2_SELF_CHECK_ID		    = 0x12,
    SpO2_PATIENT_SPEED_ID       = 0x13,
    SpO2_WORK_MODE_ID           = 0x14,
    SpO2_MODEL_LOWPOWER_ID      = 0x16,//only for debug
    SpO2_POWER_DETECT_ID  	    = 0x17,
    SpO2_SPO2_REALTIME_ID  	    = 0x19,//only for debug
    SPO2_NORMALIZED_ID          = 0x1A,
    SPO2_CALC_RESULT_ID 	 	= 0x1B,
    SPO2_DEBUG_INTERFACE_ID 	= 0x1F,
    /*----------- SpO2 Stop ------------*/

    /*----------- AIO Start ------------*/
    AIO_TX_ECG_REALTIME_ID       = 0xA0,
    AIO_TX_ECG_POLAR_VOLTAGE_ID  = 0xA1,//polarization voltage
    AIO_TX_ECG_HR_RR_ID          = 0xA2,
    AIO_TX_ARRHYTHMIA_RESULT_ID  = 0xA3,
    AIO_TX_ECG_PVCs_ID           = 0xA4,
    AIO_TX_ECG_ST_OFFSET_ID      = 0xA5,
    AIO_TX_ECG_Alarm_ID          = 0xA6,
    AIO_TX_ECG_LEAD_INFO_ID      = 0xA7,
    AIO_TX_ECG_OVERLOAD_ID       = 0xA8,//20150124 detele by qwb
    AIO_TX_ECG_ANALYZE_CHANNEL_ID = 0xA9,
    AIO_TX_ECG_ANALYZE_STATUS_ID = 0xAB,
    AIO_TX_ECG_HEARTBEAT_INFO_ID = 0xAC,
    
    AIO_ECG12_CHANNEL_ID	     = 0x20,
    AIO_ECG_ST_TEMPLATE_ID	     = 0x21,
    AIO_ECG_ALGO_PRINTF_ID	     = 0x22,
    AIO_ECG_ANALYZE_STUDY_ID	 = 0x23,
    AIO_ECG_ST_SW_ID	         = 0x25,
    AIO_ECG_ARRHYTHMIA_SW_ID	 = 0x26,
    AIO_ECG_ST_MEASURE_ID	     = 0x27,
    AIO_ECG_NOTCH_SW_ID	         = 0x28,
    AIO_ECG_CAL_MODE_ID          = 0x29,
    AIO_ECG_PROBE_MODE_ID	     = 0x2A,
    AIO_ECG_PACE_SW_ID	         = 0x2B,
    AIO_ECG_PACE_CHANNEL_ID	     = 0x2C,
    AIO_ECG_PACE_OVERSHOOT_SW_ID = 0x2D,
    AIO_RX_ECG_Debug_ID          = 0x2F,//Add for Debug ECG Module
        
    AIO_TX_RESP_REALTIME_ID      = 0xB0,
    AIO_TX_RESP_ASPHYXIA_ID      = 0xB1,
    AIO_TX_RESP_CVA_ID           = 0xB2,
    AIO_TX_RESP_OTHER_ALARM_ID   = 0xB3,

    AIO_RX_RESP_UPLOAD_TYPE_ID   = 0x30,
    AIO_RX_RESP_THRESHOLD_ID     = 0x31,
    AIO_RESP_CHANNEL_SEL_ID      = 0x32,
    AIO_RESP_CARRIER_SW_ID       = 0x33,
    AIO_RESP_ASPHYXIA_TIME_ID    = 0x34,
    AIO_RX_RESP_Debug_ID         = 0x3E,//Add for Debug RESP Module
        
    AIO_TX_TEMP_REALTIME_ID      = 0xB8,
    AIO_TX_TEMP_ALARM_ID         = 0xB9,
    AIO_TX_TEMP_CAL_SW_ID        = 0xBA,
    AIO_RX_TEMP_Debug_ID         = 0x3F,//Add for Debug TEMP Module
        
    AIO_NIBP_RESLULT_ID          = 0xC0,
    AIO_TX_NIBP_REALTIME_ID      = 0xC1,
    AIO_NIBP_VERIFY_STATE_ID     = 0xC2,
    AIO_TX_NIBP_ALARM_ID         = 0xC4,
    AIO_TX_NIBP_MMHG_ID          = 0xC5,
    AIO_TX_NIBP_COUNT_DOWN_S_ID  = 0xC7,

    AIO_NIBP_START_ID            = 0x40,
    AIO_NIBP_STOP_ID             = 0x41,
    AIO_NIBP_CYCLE_ID            = 0x42,
    AIO_NIBP_VERIFY_ID           = 0x43,
    AIO_NIBP_150MMHG_ID          = 0x44,
    AIO_NIBP_310MMHG_ID          = 0x45,
    AIO_NIBP_RESET_ID            = 0x46,
    AIO_NIBP_PREPROCESS_PRESS_ID = 0x47,
    AIO_NIBP_STATIC_PRESS_ID     = 0x48,
    AIO_NIBP_VENIPUNCTURE_ID     = 0x49,
    AIO_NIBP_CONTINUED_ID        = 0x4C,
    AIO_NIBP_GAS_LEAK_ID         = 0x4D,
    AIO_RX_NIBP_Debug_ID         = 0x4F,//Add for Debug NIBP Module

    AIO_STM_DEBUG_ID             = 0x50,
    AIO_EEPROM_DEBUG_ID          = 0x51,

    /*----------- common Start ------------*/
    COM_SOFTWARE_VERSION_ID      = 0xE0,
    COM_SELF_CHECK_ID            = 0xE1,
    COM_TX_PowerStatus_ID        = 0xE2,
    COM_TX_AbnormalReset_ID      = 0xE3,
    COM_PATIENT_TYPE_ID          = 0xE4,
    COM_PM_WORK_MODE_ID          = 0xE5,
    COM_ASK_CONFIGURE_ID         = 0xE6,
    
    AIO_TEST_FIXTURE_ID          = 0xEF, // Add for QWB for AIO.test fixture
    /*----------- common Stop ------------*/
    
    /*----------- AIO Stop ------------*/

    SF_SPO2_UPDATE               = 0xD1,
    SF_AIO_STM_UPDATE            = 0xD2,
    SF_AIO_DSP_UPDATE            = 0xD3,

    /*----------- Error Info Start ------------*/
    ERR_LICENSE_FAILED           = 0xF0,//U-Boot used
    SYSTEM_ERR_ID                = 0xF1,
    /*----------- Error Info Stop ------------*/
    
} UART_PacketID;

typedef enum
{
    SYS_ERR_MCU_UART_TX_FULL      = 0x00,
    SYS_ERR_UART_DMA_ERR,
    SYS_ERR_SPO2_UART_RX,
    SYS_ERR_SPO2_UART_RX_FULL,
    
} SysErrSubID;

#define PACKET_DATA_LEN_MAX         (132)
typedef struct 
{ 
    u8 DR_Addr;
    u8 SR_Addr;
    u8 PacketNum; 
    u8 PacketID;
    u8 Length;
    u8 DataAndCRC[PACKET_DATA_LEN_MAX+1];
} AioDspProtocolPkt;
#define PACKET_FIXED_LENGHT    6

#define MPU_ADDR                    0xAA
#define AIO_ADDR                    0x55
#define KEY_ADDR                    0xEF
#define BCK_ADDR                    0xEE
#define RECORDER_ADDR               0XBB
#define SPO2_ADDR                   0xCC
#define TEST_ADDR                   0x33
#define PC_ADDR                     0x99



typedef struct
{
    u8 u8DspAckVerifyVal;
    u8 u8DspAck150mmHgVal;
    u8 u8DspAck310mmHgVal[2];
    u8 u8DspAckVenipunctureVal;
    u16 u16DspAckMMHG;
} DspAckResult_Typedef;

#define DSP_PKT_ACK_BIT_VERIFY      ((EventBits_t)(1<<0))
#define DSP_PKT_ACK_BIT_150MMHG     ((EventBits_t)(1<<1))
#define DSP_PKT_ACK_BIT_310MMHG     ((EventBits_t)(1<<2))
#define DSP_PKT_ACK_BIT_NIBP_DEB    ((EventBits_t)(1<<3))
#define DSP_PKT_ACK_BIT_NIBP_ALARM  ((EventBits_t)(1<<4))
#define DSP_PKT_ACK_BIT_VENIPUNCTURE ((EventBits_t)(1<<5))

extern DspAckResult_Typedef    *gpDspAckResult;

int createAioDspUnpackTask(void);

int initAioDspResource(void);

void initAioDspPkt(AioDspProtocolPkt *pTxPacket);

u8 crc8AioDspPkt(const AioDspProtocolPkt *pPacket);

int sendAioDspPkt(AioDspProtocolPkt *pAioDspPkt);

int sendAioDspPktByID(const UART_PacketID id, char* pData, const u8 lenght, const u8 number);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __AIODSPPROTOCOL_H__ */
