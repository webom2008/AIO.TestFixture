// PageAioTest.cpp : 实现文件
//

#include "stdafx.h"
#include "AioTestFixture.h"
#include "PageAioTest.h"
#include "afxdialogex.h"
#include "Redirect.h"
#include <iostream>

extern CSerialProtocol *g_pSerialProtocol;

typedef enum
{
    INTER_ADC_D3V3E     = 0,
    INTER_ADC_5V6N,
    INTER_ADC_D3V3N,
    INTER_ADC_5VAN,
    INTER_ADC_5V_SPO2,
    INTER_ADC_5V_NIBP,
    INTER_ADC_REF2V5N,
    
    INTER_ADC_TOTAL
} INTER_ADC_CH;

#define PWR_BIT_D3V3E_MASK      ((WORD) (0x0001 << INTER_ADC_D3V3E))
#define PWR_BIT_5V6N_MASK       ((WORD) (0x0001 << INTER_ADC_5V6N))
#define PWR_BIT_D3V3N_MASK      ((WORD) (0x0001 << INTER_ADC_D3V3N))
#define PWR_BIT_5VAN_MASK       ((WORD) (0x0001 << INTER_ADC_5VAN))
#define PWR_BIT_5V_SPO2_MASK    ((WORD) (0x0001 << INTER_ADC_5V_SPO2))
#define PWR_BIT_5V_NIBP_MASK    ((WORD) (0x0001 << INTER_ADC_5V_NIBP))
#define PWR_BIT_REF2V5N_MASK    ((WORD) (0x0001 << INTER_ADC_REF2V5N))

typedef struct
{
    WORD flag;
    UINT u32AdcResultmV[INTER_ADC_TOTAL];
} ALARM_PWR_RESULT;









#ifdef _PAGE_TEST_INFO_
#define TEST_INFO(format,...)  printf(format, ##__VA_ARGS__)
#else
#define TEST_INFO(format,...) do { } while(0)
#endif





IMPLEMENT_DYNAMIC(CPageAioTest, CPropertyPage)

CPageAioTest::CPageAioTest()
	: CPropertyPage(CPageAioTest::IDD)
    ,initApplicationDone(false)
    ,m_pRedirect(NULL)
{

}

CPageAioTest::~CPageAioTest()
{
    if (NULL != m_pRedirect)
    {
        m_pRedirect->Stop();
        delete m_pRedirect;
        m_pRedirect = NULL;
    }
    initApplicationDone = false;
}

void CPageAioTest::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_DISPLAY, m_EditDisplay);
}


BEGIN_MESSAGE_MAP(CPageAioTest, CPropertyPage)
    ON_MESSAGE(MSG_POWER_ALRAM, &CPageAioTest::OnPowerAlramMsg)
    ON_EN_SETFOCUS(IDC_EDIT_DISPLAY, &CPageAioTest::OnEnSetfocusEditDisplay)
    ON_BN_CLICKED(IDC_BUTTON1, &CPageAioTest::OnBnClickedButton1)
END_MESSAGE_MAP()


// CPageAioTest 消息处理程序

int CPageAioTest::initApplication(void)
{
    g_pSerialProtocol->bindPaktFuncByID(AIO_TEST_FIXTURE_ID ,this, CPageAioTest::PktHandlePowerResult);

    m_pRedirect = new CRedirect("C:\\Temp\\sample.bat", &m_EditDisplay);
    if (NULL != m_pRedirect)
    {
        m_pRedirect->Run();
    }
    initApplicationDone = true;
    return 0;
}

int CPageAioTest::PktHandlePowerResult(LPVOID pParam, UartProtocolPacket *pPacket)
{
    CPageAioTest *pDlgTest = (CPageAioTest*)pParam;

    switch(pPacket->DataAndCRC[0])
    {
    case COMP_ID_VERSION:
    {
                         
    }
        break;
    case COMP_ID_PWR_ALARM:
    {
        pDlgTest->refreshPowerAlarmStatus(pParam, (void *)&pPacket->DataAndCRC[1]);
    }
        break;
    default:
        break;
    }
    return 0;
}


BOOL CPageAioTest::OnInitDialog()
{
    CPropertyPage::OnInitDialog();
    
    m_BtnStatusD3V3E.LoadBitmaps(IDB_BITMAP_RED);
    m_BtnStatusD3V3E.SubclassDlgItem(IDC_BTN_D3V3E_STATUS, this);
    m_BtnStatusD3V3E.SizeToContent();
    m_BtnStatusD3V3E.Invalidate();
    m_BtnStatusD5V6N.LoadBitmaps(IDB_BITMAP_RED);
    m_BtnStatusD5V6N.SubclassDlgItem(IDC_BTN_D5V6N_STATUS, this);
    m_BtnStatusD5V6N.SizeToContent();
    m_BtnStatusD5V6N.Invalidate();
    m_BtnStatusD3V3N.LoadBitmaps(IDB_BITMAP_RED);
    m_BtnStatusD3V3N.SubclassDlgItem(IDC_BTN_D3V3N_STATUS, this);
    m_BtnStatusD3V3N.SizeToContent();
    m_BtnStatusD3V3N.Invalidate();
    m_BtnStatusD5VAN.LoadBitmaps(IDB_BITMAP_RED);
    m_BtnStatusD5VAN.SubclassDlgItem(IDC_BTN_D5VAN_STATUS, this);
    m_BtnStatusD5VAN.SizeToContent();
    m_BtnStatusD5VAN.Invalidate();
    m_BtnStatusD5VSPO2.LoadBitmaps(IDB_BITMAP_RED);
    m_BtnStatusD5VSPO2.SubclassDlgItem(IDC_BTN_D5V_SPO2_STATUS, this);
    m_BtnStatusD5VSPO2.SizeToContent();
    m_BtnStatusD5VSPO2.Invalidate();
    m_BtnStatusD5VNIBP.LoadBitmaps(IDB_BITMAP_RED);
    m_BtnStatusD5VNIBP.SubclassDlgItem(IDC_BTN_D5V_NIBP_STATUS, this);
    m_BtnStatusD5VNIBP.SizeToContent();
    m_BtnStatusD5VNIBP.Invalidate();
    m_BtnStatusREF2V5N.LoadBitmaps(IDB_BITMAP_RED);
    m_BtnStatusREF2V5N.SubclassDlgItem(IDC_BTN_REF2V5N_STATUS, this);
    m_BtnStatusREF2V5N.SizeToContent();
    m_BtnStatusREF2V5N.Invalidate();


    return TRUE;
}

void CPageAioTest::refreshPowerAlarmStatus(LPVOID pWnd, void *param)
{
    ALARM_PWR_RESULT *pPwrResult = (ALARM_PWR_RESULT *)param;
    CPageAioTest *pDlgTest = (CPageAioTest*)pWnd;

    if (NULL == pPwrResult || NULL == pDlgTest) return;
    
    if (pPwrResult->flag){
        TEST_INFO("Power:flag = 0X%04X\r\n",pPwrResult->flag);
        if (pPwrResult->flag & PWR_BIT_D3V3E_MASK){
            m_BtnStatusD3V3E.LoadBitmaps(IDB_BITMAP_RED);
            TEST_INFO("========D3V3E     = %dmV\r\n",pPwrResult->u32AdcResultmV[INTER_ADC_D3V3E]);
        }else{
            m_BtnStatusD3V3E.LoadBitmaps(IDB_BITMAP_NORMAL);
        }
        
        if (pPwrResult->flag & PWR_BIT_5V6N_MASK){
            m_BtnStatusD5V6N.LoadBitmaps(IDB_BITMAP_RED);
            TEST_INFO("========D5V6N     = %dmV\r\n",pPwrResult->u32AdcResultmV[INTER_ADC_5V6N]);
        }else{
            m_BtnStatusD5V6N.LoadBitmaps(IDB_BITMAP_NORMAL);
        }
        
        if (pPwrResult->flag & PWR_BIT_D3V3N_MASK){
            m_BtnStatusD3V3N.LoadBitmaps(IDB_BITMAP_RED);
            TEST_INFO("========D3V3N     = %dmV\r\n",pPwrResult->u32AdcResultmV[INTER_ADC_D3V3N]);
        }else{
            m_BtnStatusD3V3N.LoadBitmaps(IDB_BITMAP_NORMAL);
        }
        
        if (pPwrResult->flag & PWR_BIT_5VAN_MASK){
            m_BtnStatusD5VAN.LoadBitmaps(IDB_BITMAP_RED);
            TEST_INFO("========D5VAN     = %dmV\r\n",pPwrResult->u32AdcResultmV[INTER_ADC_5VAN]);
        }else{
            m_BtnStatusD5VAN.LoadBitmaps(IDB_BITMAP_NORMAL);
        }
        
        if (pPwrResult->flag & PWR_BIT_5V_SPO2_MASK){
            m_BtnStatusD5VSPO2.LoadBitmaps(IDB_BITMAP_RED);
            TEST_INFO("========D5VSPO2   = %dmV\r\n",pPwrResult->u32AdcResultmV[INTER_ADC_5V_SPO2]);
        }else{
            m_BtnStatusD5VSPO2.LoadBitmaps(IDB_BITMAP_NORMAL);
        }
        
        if (pPwrResult->flag & PWR_BIT_5V_NIBP_MASK){
            m_BtnStatusD5VNIBP.LoadBitmaps(IDB_BITMAP_RED);
            TEST_INFO("========D5VNIBP   = %dmV\r\n",pPwrResult->u32AdcResultmV[INTER_ADC_5V_NIBP]);
        }else{
            m_BtnStatusD5VNIBP.LoadBitmaps(IDB_BITMAP_NORMAL);
        }
        
        if (pPwrResult->flag & PWR_BIT_REF2V5N_MASK){
            m_BtnStatusREF2V5N.LoadBitmaps(IDB_BITMAP_RED);
            TEST_INFO("========REF2V5N   = %dmV\r\n",pPwrResult->u32AdcResultmV[INTER_ADC_REF2V5N]);
        }else{
            m_BtnStatusREF2V5N.LoadBitmaps(IDB_BITMAP_NORMAL);
        }
    }
    HWND hWnd = pDlgTest->GetSafeHwnd(); 
    if (hWnd != NULL)
    {
        ::SendMessage(hWnd, MSG_POWER_ALRAM, NULL, NULL);
    }
}

afx_msg LRESULT CPageAioTest::OnPowerAlramMsg(WPARAM wParam, LPARAM lParam)
{
    if (initApplicationDone)
    {
        m_BtnStatusD3V3E.Invalidate();
        m_BtnStatusD5V6N.Invalidate();
        m_BtnStatusD3V3N.Invalidate();
        m_BtnStatusD5VAN.Invalidate();
        m_BtnStatusD5VSPO2.Invalidate();
        m_BtnStatusD5VNIBP.Invalidate();
        m_BtnStatusREF2V5N.Invalidate();
    }
    return 0;
}


void CPageAioTest::OnEnSetfocusEditDisplay()
{
    m_EditDisplay.HideCaret();
}


void CPageAioTest::OnBnClickedButton1()
{
    // TODO: 在此添加控件通知处理程序代码
//    m_EditDisplay.SetWindowTextA("OnBnClickedButton1");
    if (NULL != m_pRedirect)
    {
        std::cout<<"OnBnClickedButton1"<<std::endl;
    }
}
