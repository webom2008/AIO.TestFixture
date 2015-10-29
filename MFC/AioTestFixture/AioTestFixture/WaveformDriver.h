#pragma once


#include "visa.h"

//USB[board]::manufacturer ID::model code::serial number[::USB interface number][::INSTR]
const char DEVICE_NAME[] = "USB0::0x0957::0x2C07::MY52802529::0::INSTR";

class CWaveformDriver
{
public:
    CWaveformDriver(void);
    ~CWaveformDriver(void);

    int     initApplication(void);
    
    bool    checkIsDeviceOpen(void);
    int     openDevice(const char *pUsbDevName = DEVICE_NAME);
    int     closeDevice(void);
    
    int     getDeviceIDN(void);

    int     testSampleCh1(void);
    int     testSampleCh2(void);
    int     exampleARBFuncCh1InternalFile(void);
    int     exampleARBFuncCh2USBDeviceFile(void);

private:
    bool        m_bIsDeviceOpen;
    ViSession   m_ViSessionRM;  
    ViSession   m_ViSession33522B;
};

