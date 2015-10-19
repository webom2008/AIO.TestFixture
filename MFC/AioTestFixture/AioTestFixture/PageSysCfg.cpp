// PageSysCfg.cpp : 实现文件
//

#include "stdafx.h"
#include "AioTestFixture.h"
#include "PageSysCfg.h"
#include "afxdialogex.h"

extern CSerialProtocol *g_pSerialProtocol;

// CPageSysCfg 对话框

IMPLEMENT_DYNAMIC(CPageSysCfg, CPropertyPage)

CPageSysCfg::CPageSysCfg()
	: CPropertyPage(CPageSysCfg::IDD)
{

}

CPageSysCfg::~CPageSysCfg()
{
}

void CPageSysCfg::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_SERIAL_NUM, m_SerialNumbSel);
    DDX_Control(pDX, IDC_COMBO_SERIAL_BAUDRATE, m_SerialBaudSel);
    DDX_Control(pDX, IDC_BTN_SERIAL_OPEN, m_SerialOpenCtrl);
}


BEGIN_MESSAGE_MAP(CPageSysCfg, CPropertyPage)
    ON_BN_CLICKED(IDC_BTN_SERIAL_OPEN, &CPageSysCfg::OnBnClickedBtnSerialOpen)
END_MESSAGE_MAP()


// CPageSysCfg 消息处理程序


BOOL CPageSysCfg::OnInitDialog()
{
    CPropertyPage::OnInitDialog();

    int i;
	char SerialPortName[10][80];
    int count = g_pSerialProtocol->getSerialPortsReg(SerialPortName);
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