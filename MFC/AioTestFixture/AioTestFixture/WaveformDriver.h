#pragma once


#include "visa.h"

//USB[board]::manufacturer ID::model code::serial number[::USB interface number][::INSTR]
const char DEVICE_NAME[] = "USB0::0x0957::0x2C07::MY52802529::0::INSTR";

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

