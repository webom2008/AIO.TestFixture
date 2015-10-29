#pragma once
class CMyUsbDevice
{
public:
    CMyUsbDevice(void);
    ~CMyUsbDevice(void);
    
int     getUsbDeviceInfo(CStringArray &strDevsInfo);
int     getUsbDeviceName(CString &strDevsInfo, CString &name);
int     formatVisaUsbDeviceName(CString &strDevsInfo, CString &name);

};

