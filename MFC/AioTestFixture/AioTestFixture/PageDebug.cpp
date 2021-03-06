/******************************************************************************
*
*                版权所有 (C), 2001-2015, 广州视源电子科技股份有限公司
*
*******************************************************************************
* 文 件 名   : 
* 版 本 号   : 
* 生成日期   : 2015年11月12日
* 最近修改   :
* 功能描述   : 
*
* 修改历史   :
* 1.日    期   : 
*   作    者   : 
*   修改内容   : 
*
******************************************************************************/

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

/******************************************************************************
* 函数名称：
* 功    能：
* 参    数：
* 返 回 值：
* 作    者：QiuWeibo
* 电子邮箱：qiuweibo@cvte.com
* 日    期：2015年11月12日
******************************************************************************/
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


/*******************************************************************
* 函数名称：
* 功    能：
* 参    数：
* 返 回 值：
* 作    者：QiuWeibo
* 电子邮箱：qiuweibo@cvte.com
* 日    期：2015年11月12日
*******************************************************************/
void CPageDebug::OnBnClickedBtnWave2()
{
    gpWaveformDev->rset2Default();

    //test 7.1.2.1 passed
    //gpWaveformDev->setFuncSin(1,10.0f, 1.0f, 0.5f, -0.5f);
    //gpWaveformDev->setFuncSin(1,0.5f, 1.0f, 0.5f, -0.5f);
    //gpWaveformDev->setFuncSin(1,150.0f, 1.0f, 0.5f, -0.5f);

    //test 7.1.3.1 passed
    //gpWaveformDev->setFuncSin(1,2.0f, 16.0f, 8.0f, -8.0f);

    //gpWaveformDev->setPaceByEnum(PACE_A);
    //gpWaveformDev->setPaceByEnum(PACE_B);
    //gpWaveformDev->setPaceByEnum(PACE_C);
    //gpWaveformDev->setPaceByEnum(PACE_D);
    //gpWaveformDev->setPaceByEnum(PACE_E);
    //gpWaveformDev->setPaceByEnum(PACE_F);
    //gpWaveformDev->setPaceByEnum(PACE_G);
    //gpWaveformDev->setPaceByEnum(PACE_H);

    //gpWaveformDev->setFuncARB(2, PATH_QRS_1Hz1Vpp);
    /*gpWaveformDev->setDISPlay("PACE_A running.Please do not touch.");
    gpWaveformDev->setFuncARBByRemoteFile(1, "AIOTEST/PACE_A.arb");
    gpWaveformDev->setDISPlay("PACE_B running.Please do not touch.");
    gpWaveformDev->setFuncARBByRemoteFile(2, "AIOTEST/PACE_B.arb");
    gpWaveformDev->setDISPlay("PACE_C running.Please do not touch.");
    gpWaveformDev->setFuncARBByRemoteFile(1, "AIOTEST/PACE_C.arb");
    gpWaveformDev->setDISPlay("PACE_D running.Please do not touch.");
    gpWaveformDev->setFuncARBByRemoteFile(2, "AIOTEST/PACE_D.arb");
    gpWaveformDev->setDISPlay("PACE_E running.Please do not touch.");
    gpWaveformDev->setFuncARBByRemoteFile(1, "AIOTEST/PACE_E.arb");
    gpWaveformDev->setDISPlay("PACE_F running.Please do not touch.");
    gpWaveformDev->setFuncARBByRemoteFile(2, "AIOTEST/PACE_F.arb");
    gpWaveformDev->setDISPlay("PACE_G running.Please do not touch.");
    gpWaveformDev->setFuncARBByRemoteFile(1, "AIOTEST/PACE_G.arb");
    gpWaveformDev->setDISPlay("PACE_H running.Please do not touch.");
    gpWaveformDev->setFuncARBByRemoteFile(2, "AIOTEST/PACE_H.arb");*/
    //gpWaveformDev->setDISPlay("QRS_1Hz1Vpp running.Please do not touch.");
    //gpWaveformDev->setFuncARBByRemoteFile(1, "AIOTEST/QRS_1Hz1Vpp.arb");
    gpWaveformDev->setDISPlay("QRS_A running.Please do not touch.");
    gpWaveformDev->setFuncARBByRemoteFile(1, "AIOTEST/QRS_A.arb");//Sleep(10000);
    gpWaveformDev->clearDISPlay();
}
