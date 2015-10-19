// PageAioTest.cpp : 实现文件
//

#include "stdafx.h"
#include "AioTestFixture.h"
#include "PageAioTest.h"
#include "afxdialogex.h"
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

typedef enum
{
    STATE_AIOBOARD_START,
    STATE_AIOBOARD_DETECT_READY,
    STATE_AIOBOARD_POWERUP,
    STATE_AIOBOARD_DETECT_CURRENT,
    STATE_DETECT_D3V3E_POWER,
    STATE_DETECT_OTHER_POWER,

    
    STATE_PROCESS_SUCCESS,
    STATE_PROCESS_UNVALID,
} MainProcessState_Typedef;
static const char* MainProcessStateInfo[STATE_PROCESS_UNVALID];



#ifdef _PAGE_TEST_INFO_
#define TEST_INFO(format,...)  printf(format, ##__VA_ARGS__)
#else
#define TEST_INFO(format,...) do { } while(0)
#endif





IMPLEMENT_DYNAMIC(CPageAioTest, CPropertyPage)

CPageAioTest::CPageAioTest()
	: CPropertyPage(CPageAioTest::IDD)
    ,initApplicationDone(false)
{
}

CPageAioTest::~CPageAioTest()
{
    initApplicationDone = false;
}

void CPageAioTest::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_DISPLAY, m_EditDisplay);
}

BEGIN_MESSAGE_MAP(CPageAioTest, CPropertyPage)
    ON_EN_SETFOCUS(IDC_EDIT_DISPLAY, &CPageAioTest::OnEnSetfocusEditDisplay)
    ON_BN_CLICKED(IDC_BTN_CLEAN, &CPageAioTest::OnBnClickedBtnClean)
END_MESSAGE_MAP()


// CPageAioTest 消息处理程序

int CPageAioTest::initApplication(void)
{
    g_pSerialProtocol->bindPaktFuncByID(AIO_TEST_FIXTURE_ID ,this, CPageAioTest::PktHandlePowerResult);

    
    MainProcessStateInfo[STATE_AIOBOARD_START]          = ">>========STATE:开始！！\r\n";
    MainProcessStateInfo[STATE_AIOBOARD_DETECT_READY]   = ">>========STATE:在位检测！！\r\n";
    MainProcessStateInfo[STATE_AIOBOARD_POWERUP]        = ">>========STATE:板卡供电！！\r\n";
    MainProcessStateInfo[STATE_AIOBOARD_DETECT_CURRENT] = ">>========STATE:电流检测与控制！！\r\n";
    MainProcessStateInfo[STATE_DETECT_D3V3E_POWER]      = ">>========STATE:D3V3E电压检测！！\r\n";
    MainProcessStateInfo[STATE_DETECT_OTHER_POWER]      = ">>========STATE:其他电压检测！！\r\n";
    MainProcessStateInfo[STATE_PROCESS_SUCCESS]         = ">>========STATE:测试成功！！\r\n";
    initApplicationDone = true;
    return 0;
}

int CPageAioTest::PktHandlePowerResult(LPVOID pParam, UartProtocolPacket *pPacket)
{
    CPageAioTest *pDlgTest = (CPageAioTest*)pParam;

    switch(pPacket->DataAndCRC[0])
    {
    case COMP_ID_VERSION:{
                         
    }break;

    case COMP_ID_PWR_ALARM:{

    }break;
        
    case COMP_ID_TASK_UTILITES:{
                         
    }break;

    case COMP_ID_DOWNLOAD_CNT:{

    }break;
    case COMP_ID_ERROR_INFO:{
        pDlgTest->PktHandleErrorInfo(pPacket);
    }break;
    case COMP_ID_PROCESS_STATE:{
        pDlgTest->PktHandleProcessState(pPacket);
    }break;

    default:
        break;
    }
    return 0;
}


BOOL CPageAioTest::OnInitDialog()
{
    CPropertyPage::OnInitDialog();
    m_BtnTestStatus.LoadBitmaps(IDB_BITMAP_RED);
    m_BtnTestStatus.SubclassDlgItem(IDC_BTN_TEST_STATUS, this);
    m_BtnTestStatus.SizeToContent();
    m_BtnTestStatus.Invalidate();
    return TRUE;
}

void CPageAioTest::setTestResultFlag(void)
{
    m_BtnTestStatus.LoadBitmaps(IDB_BITMAP_RED);
    m_BtnTestStatus.Invalidate();
}
void CPageAioTest::clearTestResultFlag(void)
{
    m_BtnTestStatus.LoadBitmaps(IDB_BITMAP_NORMAL);
    m_BtnTestStatus.Invalidate();
}

void CPageAioTest::OnEnSetfocusEditDisplay()
{
    m_EditDisplay.HideCaret();
}


void CPageAioTest::add2Display(CString &str)
{
	m_EditDisplay.SetSel(-1, -1);
	m_EditDisplay.ReplaceSel(str);
    m_EditDisplay.Invalidate();
    Log2File(str.GetBuffer(str.GetLength()));
}

void CPageAioTest::add2Display(LPCSTR pStr)
{
    CString str(pStr);
	m_EditDisplay.SetSel(-1, -1);
	m_EditDisplay.ReplaceSel(str);
    m_EditDisplay.Invalidate();
    Log2File(str.GetBuffer(str.GetLength()));
}

void CPageAioTest::clearDisplay(void)
{
	m_EditDisplay.SetWindowText("");
    m_EditDisplay.SetSel(-1);
    m_EditDisplay.Invalidate();
}


int CPageAioTest::PktHandleErrorInfo(UartProtocolPacket *pPacket)
{
    switch(pPacket->DataAndCRC[1])
    {
    case ERR_INFO_ID_TEST_END:{
        add2Display(_T(">>******************************\r\n"));
        add2Display(_T(">>========STATE:测量异常结束！！\r\n"));
        add2Display(_T(">>******************************\r\n"));
        setTestResultFlag();
    }break;

    case ERR_INFO_ID_D3V3E_PWR:{
        add2Display(_T("E04-01:板卡故障，电源可能异常，请送修!!!\r\n"));
    }break;
        
    case ERR_INFO_ID_OTHER_PWR:{
        checkAndPrintPowerInfo(&pPacket->DataAndCRC[2]);
    }break;

    default:
        break;
    }
    return 0;
}

int CPageAioTest::PktHandleProcessState(UartProtocolPacket *pPacket)
{
    add2Display(MainProcessStateInfo[pPacket->DataAndCRC[1]]);
    if((BYTE)STATE_AIOBOARD_START == pPacket->DataAndCRC[1])
    {
        clearTestResultFlag();
    }
    return 0;
}

int CPageAioTest::checkAndPrintPowerInfo(void *arg)
{
    char buf[100] = {0};
    ALARM_PWR_RESULT *pPwrInfo = (ALARM_PWR_RESULT *)arg;
    if (pPwrInfo->flag & PWR_BIT_5V6N_MASK)
    {
        memset(buf,0x00, sizeof(buf));
        sprintf_s(buf,sizeof(buf), "E04-02:5V6N异常,实测: %d mV\r\n",pPwrInfo->u32AdcResultmV[INTER_ADC_5V6N]);
        add2Display(buf);
    }

    if (pPwrInfo->flag & PWR_BIT_D3V3N_MASK)
    {
        memset(buf,0x00, sizeof(buf));
        sprintf_s(buf,sizeof(buf), "E04-02:D3V3N异常,实测: %d mV\r\n",pPwrInfo->u32AdcResultmV[INTER_ADC_D3V3N]);
        add2Display(buf);
    }

    if (pPwrInfo->flag & PWR_BIT_5VAN_MASK)
    {
        memset(buf,0x00, sizeof(buf));
        sprintf_s(buf,sizeof(buf), "E04-02:+5VAN异常,实测: %d mV\r\n",pPwrInfo->u32AdcResultmV[INTER_ADC_5VAN]);
        add2Display(buf);
    }

    if (pPwrInfo->flag & PWR_BIT_5V_SPO2_MASK)
    {
        memset(buf,0x00, sizeof(buf));
        sprintf_s(buf,sizeof(buf), "E04-02:+5V_SPO2异常,实测: %d mV\r\n",pPwrInfo->u32AdcResultmV[INTER_ADC_5V_SPO2]);
        add2Display(buf);
    }

    if (pPwrInfo->flag & PWR_BIT_5V_NIBP_MASK)
    {
        memset(buf,0x00, sizeof(buf));
        sprintf_s(buf,sizeof(buf), "E04-02:5V_NIBP异常,实测: %d mV\r\n",pPwrInfo->u32AdcResultmV[INTER_ADC_5V_NIBP]);
        add2Display(buf);
    }

    if (pPwrInfo->flag & PWR_BIT_REF2V5N_MASK)
    {
        memset(buf,0x00, sizeof(buf));
        sprintf_s(buf,sizeof(buf), "E04-02:REF_2V5N异常,实测: %d mV\r\n",pPwrInfo->u32AdcResultmV[INTER_ADC_REF2V5N]);
        add2Display(buf);
    }
    return 0;
}






void CPageAioTest::OnBnClickedBtnClean()
{
    clearDisplay();
}
