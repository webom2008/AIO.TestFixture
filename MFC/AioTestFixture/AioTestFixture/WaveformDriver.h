#pragma once


#include "visa.h"

#define WF_COMM_CID_SET_SIN_10Hz1Vpp        ((BYTE)0x00)
#define WF_COMM_CID_SET_SIN_0P5Hz1Vpp       ((BYTE)0x01)
#define WF_COMM_CID_SET_SIN_150Hz1Vpp       ((BYTE)0x02)
#define WF_COMM_CID_SET_SIN_2Hz16Vpp        ((BYTE)0x03)
#define WF_COMM_CID_SET_SIN_2Hz16mVpp       ((BYTE)0x04)
#define WF_COMM_CID_SET_PACE_A              ((BYTE)0x05)
#define WF_COMM_CID_SET_PACE_B              ((BYTE)0x06)
#define WF_COMM_CID_SET_PACE_C              ((BYTE)0x07)
#define WF_COMM_CID_SET_PACE_D              ((BYTE)0x08)
#define WF_COMM_CID_SET_PACE_E              ((BYTE)0x09)
#define WF_COMM_CID_SET_PACE_F              ((BYTE)0x0A)
#define WF_COMM_CID_SET_PACE_G              ((BYTE)0x0B)
#define WF_COMM_CID_SET_PACE_H              ((BYTE)0x0C)
#define WF_COMM_CID_SET_QRS_A               ((BYTE)0x0D)
#define WF_COMM_CID_SET_ARB                 ((BYTE)0xFF)

//USB[board]::manufacturer ID::model code::serial number[::USB interface number][::INSTR]
const char DEVICE_NAME[] = "USB0::0x0957::0x2C07::MY52802529::0::INSTR";

const char PATH_PACE_A[] = "\"USB:\\PACE_A.BARB\"";
const char PATH_PACE_B[] = "\"USB:\\PACE_B.BARB\"";
const char PATH_PACE_C[] = "\"USB:\\PACE_C.BARB\"";
const char PATH_PACE_D[] = "\"USB:\\PACE_D.BARB\"";
const char PATH_PACE_E[] = "\"USB:\\PACE_E.BARB\"";
const char PATH_PACE_F[] = "\"USB:\\PACE_F.BARB\"";
const char PATH_PACE_G[] = "\"USB:\\PACE_G.BARB\"";
const char PATH_PACE_H[] = "\"USB:\\PACE_H.BARB\"";
const char PATH_QRS_A[] = "\"USB:\\QRS_A.BARB\"";


enum 
{
    PACE_A,
    PACE_B,
    PACE_C,
    PACE_D,
    PACE_E,
    PACE_F,
    PACE_G,
    PACE_H,
};

class CWaveformDriver
{
public:
    CWaveformDriver(void);
    ~CWaveformDriver(void);

    int     initApplication(void);
    
    bool    checkIsDeviceOpen(void);
    int     openDevice(const char *pUsbDevName = DEVICE_NAME);
    int     closeDevice(void);
    
    int     rset2Default(void);
    int     getDeviceIDN(void);
    
    int     setPaceByEnum(int type);

    int     setFuncSin(UINT8 channel, float freq_hz, float amp_V, float high_V = 0, float low_V = 0, float offset_V = 0);
    int     setFuncPULSe (UINT8 channel, float freq_hz, float volt_V, float lead_s, float tra_s, float widt_s, float offset_V = 0);
    int     setFuncARB (UINT8 channel, const char *pathName);

    //<!-- Debug_Interface_Start -->
    int     testSampleCh1(void);
    int     testSampleCh2(void);
    int     exampleARBFuncCh1InternalFile(void);
    int     exampleARBFuncCh2USBDeviceFile(void);
    //<!-- Debug_Interface_End -->
protected:

private:
    bool        m_bIsDeviceOpen;
    ViSession   m_ViSessionRM;  
    ViSession   m_ViSession33522B;
};

