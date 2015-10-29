// PageSysCfg.cpp : 实现文件
//

#include "stdafx.h"
#include "AioTestFixture.h"
#include "PageSysCfg.h"
#include "afxdialogex.h"
#include "WaveformDriver.h"

extern CSerialProtocol *g_pSerialProtocol;
extern CWaveformDriver *gpWaveformDev;

// CPageSysCfg 对话框

IMPLEMENT_DYNAMIC(CPageSysCfg, CPropertyPage)

CPageSysCfg::CPageSysCfg()
	: CPropertyPage(CPageSysCfg::IDD)
    ,m_pMyUsbDev(NULL)
{

}

CPageSysCfg::~CPageSysCfg()
{
    if (NULL != m_pMyUsbDev)
    {
        delete m_pMyUsbDev;
        m_pMyUsbDev = NULL;
    }

}

void CPageSysCfg::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_SERIAL_NUM, m_SerialNumbSel);
    DDX_Control(pDX, IDC_COMBO_SERIAL_BAUDRATE, m_SerialBaudSel);
    DDX_Control(pDX, IDC_BTN_SERIAL_OPEN, m_SerialOpenCtrl);
    DDX_Control(pDX, IDC_BTN_WAVE_CONNECT, m_BtnWaveConnnetCtrl);
    DDX_Control(pDX, IDC_COMBO_USB_SEL, m_ComboUSBSel);
    DDX_Control(pDX, IDC_EDIT_USB_INFO, m_EditUsbInfo);
}


BEGIN_MESSAGE_MAP(CPageSysCfg, CPropertyPage)
    ON_BN_CLICKED(IDC_BTN_SERIAL_OPEN, &CPageSysCfg::OnBnClickedBtnSerialOpen)
    ON_BN_CLICKED(IDC_BTN_WAVE_CONNECT, &CPageSysCfg::OnBnClickedBtnWaveConnect)
    ON_CBN_SELCHANGE(IDC_COMBO_USB_SEL, &CPageSysCfg::OnCbnSelchangeComboUsbSel)
END_MESSAGE_MAP()


// CPageSysCfg 消息处理程序


BOOL CPageSysCfg::OnInitDialog()
{
    CPropertyPage::OnInitDialog();

    int i;
	char SerialPortName[10][80];
    int count = g_pSerialProtocol->getSerialPortsReg(SerialPortName);
    CString str1;
    CString str2;

    if (NULL == m_pMyUsbDev)
    {
        m_pMyUsbDev = new CMyUsbDevice;
        if (NULL == m_pMyUsbDev)
        {
            AfxMessageBox("new CMyUsbDevice失败！");
        }
    }

    for (i = 0; i < count; i++)
    {
        m_SerialNumbSel.InsertString(i,SerialPortName[i]);
    }
    if (count > 0)
    {
        m_SerialNumbSel.SetCurSel(0);
    }

    m_SerialBaudSel.InsertString(0, "19200");
    m_SerialBaudSel.InsertString(1, "115200");
    m_SerialBaudSel.InsertString(2, "230400");
    m_SerialBaudSel.InsertString(3, "460800");
    m_SerialBaudSel.SetCurSel(2);

    m_pMyUsbDev->getUsbDeviceInfo(m_StrArrayUSBInfo);
    for (i = 0; i < m_StrArrayUSBInfo.GetCount(); i++)
    {
        str1 = m_StrArrayUSBInfo.GetAt(i);
        m_pMyUsbDev->getUsbDeviceName(str1, str2);
        m_ComboUSBSel.InsertString(i, str2);
        TRACE("%d:%s\r\n",i,str1);
    }
    if (m_StrArrayUSBInfo.GetCount() > 0)
    {
        m_ComboUSBSel.SetCurSel(0);
    }
    return TRUE;  // return TRUE unless you set the focus to a control
}



void CPageSysCfg::OnBnClickedBtnSerialOpen()
{
    int index = 0, offset = 0;
    UINT portNr = 0;
	CString str,strPortNr;
    int nBaudrate = 0;
    
    if (NULL == g_pSerialProtocol) return;

    str.Empty();
	index = m_SerialBaudSel.GetCurSel();
	m_SerialBaudSel.GetLBText( index, str); 
	nBaudrate = atoi(str);
    
    str.Empty();
	index = m_SerialNumbSel.GetCurSel();
	m_SerialNumbSel.GetLBText( index, str);
    offset = str.ReverseFind('M');// 从右往左边开始查找第一个'\\'，获取左边字符串的长度
    strPortNr = str.Right(str.GetLength() - offset -1);
    portNr = atoi(strPortNr);

    if(g_pSerialProtocol->isSerialOpen())    
    {
        MSG("关闭串口");
        if (0 == g_pSerialProtocol->closeDevice())
        {
            INFO(":OK\r\n");
            m_SerialOpenCtrl.SetWindowText("打开串口");
        }
        else
        {
            INFO(":FAIL\r\n");
        }
    }   
    else    
    { 
        MSG("打开串口");
        if (0 == g_pSerialProtocol->openDevice(this, portNr, nBaudrate))
        {
            INFO(":OK\r\n");
            m_SerialOpenCtrl.SetWindowText("关闭串口");
        }
        else
        {
            INFO(":FAIL\r\n");
        }
    }
}

int CPageSysCfg::initApplication(void)
{
#ifdef CONFIG_CHECK_PKT_ID_LOST_USED
    g_pSerialProtocol->setPacketNumValidCheck(true);
#endif
    return 0;
}

void CPageSysCfg::OnBnClickedBtnWaveConnect()
{
    TCHAR visa_dev_name[100];
    if (gpWaveformDev->checkIsDeviceOpen())
    {
        gpWaveformDev->closeDevice();
        m_BtnWaveConnnetCtrl.SetWindowText("建立连接");
    }
    else
    {
        memset(visa_dev_name, 0x00, sizeof(visa_dev_name));
        m_EditUsbInfo.GetWindowTextA((LPTSTR)visa_dev_name, sizeof(visa_dev_name));
        if (0 == gpWaveformDev->openDevice(visa_dev_name))
        {
            m_BtnWaveConnnetCtrl.SetWindowText("断开连接");
        }
        else
        {
            ERROR_INFO("连接信号发生器失败!!!\r\n");
        }
    }
}


void CPageSysCfg::OnCbnSelchangeComboUsbSel()
{
    CString str1;
    CString str2;
    
    str1 = m_StrArrayUSBInfo.GetAt(m_ComboUSBSel.GetCurSel());
    m_pMyUsbDev->formatVisaUsbDeviceName(str1, str2); //debug
    m_EditUsbInfo.SetWindowText(str2);
}
