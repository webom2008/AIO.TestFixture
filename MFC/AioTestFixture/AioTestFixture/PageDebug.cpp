// PageDebug.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "AioTestFixture.h"
#include "PageDebug.h"
#include "afxdialogex.h"

extern CSerialProtocol *g_pSerialProtocol;

// CPageDebug �Ի���

IMPLEMENT_DYNAMIC(CPageDebug, CPropertyPage)

CPageDebug::CPageDebug()
	: CPropertyPage(CPageDebug::IDD)
{

}

CPageDebug::~CPageDebug()
{
}

void CPageDebug::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageDebug, CPropertyPage)
    ON_BN_CLICKED(IDC_BTN_GET_TASK_UTILITES, &CPageDebug::OnBnClickedBtnGetTaskUtilites)
END_MESSAGE_MAP()


// CPageDebug ��Ϣ�������

int CPageDebug::initApplication(void)
{
    //g_pSerialProtocol->bindPaktFuncByID(AIO_TEST_FIXTURE_ID ,this, CPageAioTest::PktHandlePowerResult);

    return 0;
}

void CPageDebug::OnBnClickedBtnGetTaskUtilites()
{
    BYTE id;
    BYTE pBuf[1];

    id = AIO_TEST_FIXTURE_ID;
    pBuf[0] = (BYTE)COMP_ID_TASK_UTILITES;
    if (g_pSerialProtocol->isSerialOpen())
    {
        g_pSerialProtocol->sendOnePacket(id, 0, pBuf, 1);
    }
    else
    {
        MSG("��ȷ����ȷ���ô���\r\n");
    }
}
