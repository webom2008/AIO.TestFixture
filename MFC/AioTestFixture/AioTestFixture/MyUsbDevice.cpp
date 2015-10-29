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
    BOOL bResult; // ȡ��һ����GUID��ص��豸��Ϣ�����
    LPGUID lpGuid = (LPGUID)&UsbClassGuid;

    strDevsInfo.RemoveAll();

    hDevInfoSet = ::SetupDiGetClassDevs(lpGuid, // class GUID
        NULL, // �޹ؼ���
        NULL, // ��ָ�������ھ��
        DIGCF_PRESENT | DIGCF_DEVICEINTERFACE); // Ŀǰ���ڵ��豸 // ʧ��...
    if (hDevInfoSet == INVALID_HANDLE_VALUE)
    {
        AfxMessageBox("��ȡ��ص��豸��Ϣ��GUID���ʧ�ܣ�");
    } // �����豸�ӿ����ݿռ�
    else
    {
        pDetail = (PSP_DEVICE_INTERFACE_DETAIL_DATA)::GlobalAlloc(LMEM_ZEROINIT, INTERFACE_DETAIL_SIZE);
        pDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
        nCount = 0;
        bResult = TRUE; // �豸���=0,1,2... ��һ�����豸�ӿڣ���ʧ��Ϊֹ
        while (bResult)
        {
            ifdata.cbSize = sizeof(ifdata); // ö�ٷ��ϸ�GUID���豸�ӿ�
            bResult = ::SetupDiEnumDeviceInterfaces( hDevInfoSet, // �豸��Ϣ�����
                NULL, // ���������豸����
                lpGuid, // GUID
                (ULONG)nCount, // �豸��Ϣ������豸���
                &ifdata); // �豸�ӿ���Ϣ
            if (bResult)
            {
                // ȡ�ø��豸�ӿڵ�ϸ��(�豸·��)
                bResult = SetupDiGetInterfaceDeviceDetail( hDevInfoSet, // �豸��Ϣ�����
                    &ifdata, // �豸�ӿ���Ϣ
                    pDetail, // �豸�ӿ�ϸ��(�豸·��)
                    INTERFACE_DETAIL_SIZE, // �����������С
                    NULL, // ������������������С(ֱ�����趨ֵ)
                    NULL); // ���������豸����
                if (bResult)
                {
                    // �����豸·�������������
                    Sdn = pDetail->DevicePath; 
                    // ��������ֵ
                    strDevsInfo.Add(Sdn);
//                    AfxMessageBox(Sdn);//���ڵ�����
                    nCount++;
                }
            }
        }
        // �ͷ��豸�ӿ����ݿռ�
        ::GlobalFree(pDetail); // �ر��豸��Ϣ�����
        ::SetupDiDestroyDeviceInfoList(hDevInfoSet);
    }
    return nCount;//�����豸ID���ַ���
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
