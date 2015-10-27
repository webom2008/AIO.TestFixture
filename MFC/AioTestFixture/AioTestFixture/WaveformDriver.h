#pragma once
class CWaveformDriver
{
public:
    CWaveformDriver(void);
    ~CWaveformDriver(void);
    int     initApplication(void);
    
    int     openDevice(void);
    int     closeDevice(void);
};

