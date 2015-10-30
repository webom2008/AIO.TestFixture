// PageDebug.cpp : 实现文件
//

#include "stdafx.h"
#include "AioTestFixture.h"
#include "PageDebug.h"
#include "afxdialogex.h"
#include "WaveformDriver.h"

extern CSerialProtocol *g_pSerialProtocol;
extern CWaveformDriver *gpWaveformDev;

// CPageDebug 对话框

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
    ON_BN_CLICKED(IDC_BTN_RESET_DL_CNT, &CPageDebug::OnBnClickedBtnResetDlCnt)
    ON_BN_CLICKED(IDC_BTN_DEC_DL_CNT, &CPageDebug::OnBnClickedBtnDecDlCnt)
    ON_BN_CLICKED(IDC_BTN_RESET_FLASH, &CPageDebug::OnBnClickedBtnResetFlash)
    ON_BN_CLICKED(IDC_BTN_WAVE1, &CPageDebug::OnBnClickedBtnWave1)
    ON_BN_CLICKED(IDC_BTN_WAVE2, &CPageDebug::OnBnClickedBtnWave2)
END_MESSAGE_MAP()


// CPageDebug 消息处理程序

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
        MSG("请确保正确配置串口\r\n");
    }
}

void CPageDebug::OnBnClickedBtnResetDlCnt()
{
    BYTE id;
    BYTE pBuf[2];

    id = AIO_TEST_FIXTURE_ID;
    pBuf[0] = (BYTE)COMP_ID_DOWNLOAD_CNT;
    pBuf[1] = 0x01; //SECUR_CTRL_W_RESET_DOWNLOAD_CNT
    if (g_pSerialProtocol->isSerialOpen())
    {
        g_pSerialProtocol->sendOnePacket(id, 0, pBuf, sizeof(pBuf));
    }
    else
    {
        MSG("请确保正确配置串口\r\n");
    }
}

void CPageDebug::OnBnClickedBtnDecDlCnt()
{
    BYTE id;
    BYTE pBuf[2];

    id = AIO_TEST_FIXTURE_ID;
    pBuf[0] = (BYTE)COMP_ID_DOWNLOAD_CNT;
    pBuf[1] = 0x02; //SECUR_CTRL_W_DEC_DOWNLOAD_CNT
    if (g_pSerialProtocol->isSerialOpen())
    {
        g_pSerialProtocol->sendOnePacket(id, 0, pBuf, sizeof(pBuf));
    }
    else
    {
        MSG("请确保正确配置串口\r\n");
    }
}

void CPageDebug::OnBnClickedBtnResetFlash()
{
    BYTE id;
    BYTE pBuf[2];

    id = AIO_TEST_FIXTURE_ID;
    pBuf[0] = (BYTE)COMP_ID_DOWNLOAD_CNT;
    pBuf[1] = 0x00; //SECUR_CTRL_W_RESET_ALL
    if (g_pSerialProtocol->isSerialOpen())
    {
        g_pSerialProtocol->sendOnePacket(id, 0, pBuf, sizeof(pBuf));
    }
    else
    {
        MSG("请确保正确配置串口\r\n");
    }
}


void CPageDebug::OnBnClickedBtnWave1()
{
    gpWaveformDev->getDeviceIDN();
}


void CPageDebug::OnBnClickedBtnWave2()
{
    gpWaveformDev->rset2Default();

    //test 7.1.2.1 passed
    gpWaveformDev->setFuncSin(1,10.0f, 1.0f, 0.5f, -0.5f);
    gpWaveformDev->setFuncSin(1,0.5f, 1.0f, 0.5f, -0.5f);
    gpWaveformDev->setFuncSin(1,150.0f, 1.0f, 0.5f, -0.5f);

    //test 7.1.3.1 passed
    gpWaveformDev->setFuncSin(1,2.0f, 16.0f, 8.0f, -8.0f);

    //gpWaveformDev->setPaceByEnum(PACE_A);
    //gpWaveformDev->setPaceByEnum(PACE_B);
    //gpWaveformDev->setPaceByEnum(PACE_C);
    //gpWaveformDev->setPaceByEnum(PACE_D);
    //gpWaveformDev->setPaceByEnum(PACE_E);
    //gpWaveformDev->setPaceByEnum(PACE_F);
    //gpWaveformDev->setPaceByEnum(PACE_G);
    //gpWaveformDev->setPaceByEnum(PACE_H);
}
