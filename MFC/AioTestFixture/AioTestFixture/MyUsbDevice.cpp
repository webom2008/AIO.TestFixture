#include "stdafx.h"
#include "MyUsbDevice.h"


#include <winioctl.h>
#include <initguid.h>

extern "C" {
    // Declare the C libraries used
#include <setupapi.h>  // Must link in setupapi.lib
}

#pragma comment(lib, "SetupAPI32.Lib")

DEFINE_GUID(UsbClassGuid, 0xa5dcbf10L, 0x6530, 0x11d2, 0x90, 0x1f, 0x00, 0xc0, 0x4f, 0xb9, 0x51, 0xed);
#define INTERFACE_DETAIL_SIZE (1024)

CMyUsbDevice::CMyUsbDevice(void)
{
}


CMyUsbDevice::~CMyUsbDevice(void)
{
}

int CMyUsbDevice::getUsbDeviceInfo(CStringArray &strDevsInfo)
{
    int nCount;
    CString Sdn;
    HDEVINFO hDevInfoSet;
    SP_DEVICE_INTERFACE_DATA ifdata;
    PSP_DEVICE_INTERFACE_DETAIL_DATA pDetail;
    BOOL bResult; // 取得一个该GUID相关的设备信息集句柄
    LPGUID lpGuid = (LPGUID)&UsbClassGuid;

    strDevsInfo.RemoveAll();

    hDevInfoSet = ::SetupDiGetClassDevs(lpGuid, // class GUID
        NULL, // 无关键字
        NULL, // 不指定父窗口句柄
        DIGCF_PRESENT | DIGCF_DEVICEINTERFACE); // 目前存在的设备 // 失败...
    if (hDevInfoSet == INVALID_HANDLE_VALUE)
    {
        AfxMessageBox("获取相关的设备信息集GUID句柄失败！");
    } // 申请设备接口数据空间
    else
    {
        pDetail = (PSP_DEVICE_INTERFACE_DETAIL_DATA)::GlobalAlloc(LMEM_ZEROINIT, INTERFACE_DETAIL_SIZE);
        pDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
        nCount = 0;
        bResult = TRUE; // 设备序号=0,1,2... 逐一测试设备接口，到失败为止
        while (bResult)
        {
            ifdata.cbSize = sizeof(ifdata); // 枚举符合该GUID的设备接口
            bResult = ::SetupDiEnumDeviceInterfaces( hDevInfoSet, // 设备信息集句柄
                NULL, // 不需额外的设备描述
                lpGuid, // GUID
                (ULONG)nCount, // 设备信息集里的设备序号
                &ifdata); // 设备接口信息
            if (bResult)
            {
                // 取得该设备接口的细节(设备路径)
                bResult = SetupDiGetInterfaceDeviceDetail( hDevInfoSet, // 设备信息集句柄
                    &ifdata, // 设备接口信息
                    pDetail, // 设备接口细节(设备路径)
                    INTERFACE_DETAIL_SIZE, // 输出缓冲区大小
                    NULL, // 不需计算输出缓冲区大小(直接用设定值)
                    NULL); // 不需额外的设备描述
                if (bResult)
                {
                    // 复制设备路径到输出缓冲区
                    Sdn = pDetail->DevicePath; 
                    // 调整计数值
                    strDevsInfo.Add(Sdn);
//                    AfxMessageBox(Sdn);//用于调试用
                    nCount++;
                }
            }
        }
        // 释放设备接口数据空间
        ::GlobalFree(pDetail); // 关闭设备信息集句柄
        ::SetupDiDestroyDeviceInfoList(hDevInfoSet);
    }
    return nCount;//包含设备ID的字符串
}

/*"\\?\usb#vid_04f2&pid_b39a#5&42c634&0&12#{a5dcbf10-6530-11d2-901f-00c04fb951ed}"*/
int CMyUsbDevice::getUsbDeviceName(CString &strDevsInfo, CString &name)
{
    int offset = 0;
    int ret = 0;
    int cut_start_offset = 0;
    int cut_end_offset = 0;

    ret = strDevsInfo.Find('#',0); //find first '#'
    if (ret > 0)
    {
        offset = ret+1;
        ret = strDevsInfo.Find('#',offset); //second first '#'
        if (ret > 0) cut_start_offset = ret+1;
        ret = strDevsInfo.Find('#',cut_start_offset); //third first '#'
        if (ret > 0) cut_end_offset = ret;
    }

    ret = cut_end_offset - cut_start_offset;
    if (ret > 0)
    {
        name = strDevsInfo.Mid(cut_start_offset, ret);
        return 0;
    }
    return -1;
}


//const char DEVICE_NAME[] = "USB0::0x0957::0x2C07::MY52802529::0::INSTR";
/*"\\?\usb#vid_04f2&pid_b39a#5&42c634&0&12#{a5dcbf10-6530-11d2-901f-00c04fb951ed}"*/
int CMyUsbDevice::formatVisaUsbDeviceName(CString &strDevsInfo, CString &name)
{
    CString info;
    CString id;
    CString vid;
    CString pid;
    int offset = 0;
    int first_offset = 0;
    int second_offset = 0;
    int third_offset = 0;
    int ret = 0;

    ret = strDevsInfo.Find('#',0); //find first '#'
    if (ret > 0)
    {
        first_offset = ret+1;
        ret = strDevsInfo.Find('#',first_offset); //second first '#'
        if (ret > 0) second_offset = ret+1;
        ret = strDevsInfo.Find('#',second_offset); //third first '#'
        if (ret > 0) third_offset = ret;
    }

    
    ret = second_offset - first_offset;
    if (ret > 0)
    {
        id = strDevsInfo.Mid(first_offset, ret-1);
        //vid_04f2&pid_b39a
    }

    ret = third_offset - second_offset;
    if (ret > 0)
    {
        info = strDevsInfo.Mid(second_offset, ret);
    }

    //id = "vid_04f2&pid_b39a"
    ret = id.Find('_',0); //find first '_'
    if (ret > 0)
    {
        first_offset = ret+1;
        ret = id.Find('&',first_offset); //find first '_'
        if (ret > 0) 
        {
            second_offset = ret;
            ret = second_offset - first_offset;
            if (ret > 0)
            {
                vid = id.Mid(first_offset, ret);

            }
        }
        ret = id.Find('_',second_offset); //second  '_'
        if (ret > 0)
        {
            first_offset = ret+1;
            pid = id.Mid(first_offset);
        }
    }

    //"USB0::0x0957::0x2C07::MY52802529::0::INSTR"
    name = "USB0::0x"+vid+"::0x"+pid+"::"+info+"::0::INSTR";
    return -1;
}
