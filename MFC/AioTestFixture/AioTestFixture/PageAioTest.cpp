// PageAioTest.cpp : 实现文件
//

#include "stdafx.h"
#include "AioTestFixture.h"
#include "PageAioTest.h"
#include "afxdialogex.h"
#include "FileIO.h"


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
    STATE_DOWNLOAD_AIOSTM_BOOT,
    STATE_DOWNLOAD_AIODSP_APP,
    STATE_DOWNLOAD_AIOSTM_APP,

    
    STATE_PROCESS_SUCCESS,
    STATE_PROCESS_UNVALID,
} MainProcessState_Typedef;

static const char* MainProcessStateInfo[STATE_PROCESS_UNVALID];
static const char AIO_DSP_APP_BIN_NAME[] = "aPM12_AIO_DSPAPP*.ldr";
static const char AIO_STM_APP_BIN_NAME[] = "aPM12_AIO_STMAPP*.bin";


#ifdef _PAGE_TEST_INFO_
#define TEST_INFO(format,...)  printf(format, ##__VA_ARGS__)
#else
#define TEST_INFO(format,...) do { } while(0)
#endif





IMPLEMENT_DYNAMIC(CPageAioTest, CPropertyPage)

CPageAioTest::CPageAioTest()
	: CPropertyPage(CPageAioTest::IDD)
    ,initApplicationDone(false)
    ,m_pUpdate(NULL)
{
}

CPageAioTest::~CPageAioTest()
{
    initApplicationDone = false;
    if (NULL != m_pUpdate)
    {
        delete m_pUpdate;
    }
}

void CPageAioTest::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_DISPLAY, m_EditDisplay);
}

BEGIN_MESSAGE_MAP(CPageAioTest, CPropertyPage)
    ON_EN_SETFOCUS(IDC_EDIT_DISPLAY, &CPageAioTest::OnEnSetfocusEditDisplay)
    ON_BN_CLICKED(IDC_BTN_CLEAN, &CPageAioTest::OnBnClickedBtnClean)
    ON_MESSAGE(MSG_UPDATE_AIODSP_APP, &CPageAioTest::OnMsgUpdateAiodspApp)
    ON_MESSAGE(MSG_UPDATE_AIOSTM_APP, &CPageAioTest::OnMsgUpdateAiostmApp)
END_MESSAGE_MAP()


// CPageAioTest 消息处理程序

int CPageAioTest::initApplication(void)
{
    m_pUpdate = new CUpdate;
    if (NULL != m_pUpdate)
    {
        m_pUpdate->initApplication();
    }
    g_pSerialProtocol->bindPaktFuncByID(AIO_TEST_FIXTURE_ID ,this, CPageAioTest::PktHandlePowerResult);

    
    MainProcessStateInfo[STATE_AIOBOARD_START]          = ">>========STATE:开始！！\r\n";
    MainProcessStateInfo[STATE_AIOBOARD_DETECT_READY]   = ">>========STATE:在位检测！！\r\n";
    MainProcessStateInfo[STATE_AIOBOARD_POWERUP]        = ">>========STATE:板卡供电！！\r\n";
    MainProcessStateInfo[STATE_AIOBOARD_DETECT_CURRENT] = ">>========STATE:电流检测与控制！！\r\n";
    MainProcessStateInfo[STATE_DETECT_D3V3E_POWER]      = ">>========STATE:D3V3E电压检测！！\r\n";
    MainProcessStateInfo[STATE_DETECT_OTHER_POWER]      = ">>========STATE:其他电压检测！！\r\n";
    MainProcessStateInfo[STATE_DOWNLOAD_AIOSTM_BOOT]    = ">>========STATE:烧写AIOSTM_BOOT！！\r\n";
    MainProcessStateInfo[STATE_DOWNLOAD_AIODSP_APP]     = ">>========STATE:升级AIODSP-APP！！\r\n";
    MainProcessStateInfo[STATE_DOWNLOAD_AIOSTM_APP]     = ">>========STATE:升级AIOSTM-APP！！\r\n";
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
    case COMP_ID_AIOSTM_BOOT:{
    }break;
    case COMP_ID_AIODSP_APP:{
        //pDlgTest->PktHandleStartUpdateDspApp();
        HWND hWnd = pDlgTest->GetSafeHwnd(); 
        if (hWnd != NULL)
        {
            ::SendMessage(hWnd, MSG_UPDATE_AIODSP_APP, NULL, NULL);
        }
    }break;
    case COMP_ID_AIOSTM_APP:{
        //pDlgTest->PktHandleStartUpdateStmApp();
        HWND hWnd = pDlgTest->GetSafeHwnd(); 
        if (hWnd != NULL)
        {
            ::SendMessage(hWnd, MSG_UPDATE_AIOSTM_APP, NULL, NULL);
        }
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

int CPageAioTest::detectBinFile(const char *wildcard,CString &name)
{
    CStringArray strArrayFileNames;

    CFileIO::GetFileNamesInDir(strArrayFileNames, wildcard);
    if (strArrayFileNames.GetSize() > 0)
    {
        name = strArrayFileNames.GetAt(0); // 只获取第一个
        strArrayFileNames.RemoveAll();
        return 1;
    }
    return 0;
}

int CPageAioTest::PktHandleStartUpdateDspApp(void)
{
    CString bin_name;
    BYTE    id = SF_AIO_DSP_UPDATE;
    CString info;
    BYTE pBuf[2];

    if (detectBinFile(AIO_DSP_APP_BIN_NAME, bin_name) > 0)
    {
        if (updateTask(id, bin_name) < 0)
        {
            add2Display("错误:AIODSP-APP升级失败!!!\r\n");
            id = AIO_TEST_FIXTURE_ID;
            pBuf[0] = (BYTE)COMP_ID_AIODSP_APP;
            pBuf[1] = (BYTE)0x01;
            g_pSerialProtocol->sendOnePacket(id, 0, pBuf, 2);
        }
        else
        {
            add2Display("AIODSP-APP升级成功!!!\r\n");
            id = AIO_TEST_FIXTURE_ID;
            pBuf[0] = (BYTE)COMP_ID_AIODSP_APP;
            pBuf[1] = (BYTE)0x00;
            g_pSerialProtocol->sendOnePacket(id, 0, pBuf, 2);
        }
    }
    else
    {
        info.Format("错误:没有检测到升级文件%s\r\n", AIO_DSP_APP_BIN_NAME);
        add2Display(info);
        id = AIO_TEST_FIXTURE_ID;
        pBuf[0] = (BYTE)COMP_ID_AIODSP_APP;
        pBuf[1] = (BYTE)0x01;
        g_pSerialProtocol->sendOnePacket(id, 0, pBuf, 2);
    }
    return 0;
}

int CPageAioTest::PktHandleStartUpdateStmApp(void)
{
    CString bin_name;
    BYTE    id = SF_AIO_STM_UPDATE;
    CString info;
    BYTE pBuf[2];

    if (detectBinFile(AIO_STM_APP_BIN_NAME, bin_name) > 0)
    {
        if (updateTask(id, bin_name) < 0)
        {
            add2Display("错误:AIOSTM-APP升级失败!!!\r\n");
            id = AIO_TEST_FIXTURE_ID;
            pBuf[0] = (BYTE)COMP_ID_AIOSTM_APP;
            pBuf[1] = (BYTE)0x01;
            g_pSerialProtocol->sendOnePacket(id, 0, pBuf, 2);
        }
        else
        {
            add2Display("AIODSP-APP升级成功!!!\r\n");
            id = AIO_TEST_FIXTURE_ID;
            pBuf[0] = (BYTE)COMP_ID_AIOSTM_APP;
            pBuf[1] = (BYTE)0x00;
            g_pSerialProtocol->sendOnePacket(id, 0, pBuf, 2);
        }
    }
    else
    {
        info.Format("错误:没有检测到升级文件%s\r\n", AIO_STM_APP_BIN_NAME);
        add2Display(info);
        id = AIO_TEST_FIXTURE_ID;
        pBuf[0] = (BYTE)COMP_ID_AIOSTM_APP;
        pBuf[1] = (BYTE)0x01;
        g_pSerialProtocol->sendOnePacket(id, 0, pBuf, 2);
    }
    return 0;
}


int CPageAioTest::updateTask(BYTE &CID, CString &name)
{
    int i;
    unsigned int len;   //bin file lenght
    int result;
    CString str;
    
    INFO("正准备软件升级...\r\n");

    if (NULL == m_pUpdate)
    {
        add2Display(_T("错误:m_pUpdate初始化失败\r\n"));
        return -1;
    }
    if (CID != m_pUpdate->getPacketCID())
    {
        m_pUpdate->setPacketCID(CID);
    }
    if (m_pUpdate->SaveFiletoRAM(&len, name) < 0)
    {
        INFO("错误:文件拷贝至内存失败\r\n");
        add2Display(_T("错误:文件拷贝至内存失败\r\n"));
        return -2;
    }
    else
    {
        INFO("提示:文件长度 = %d  字节\r\n", len);
    }

    //>>>>>>>>S3:Send Update Tag, Target ready to download.
    i = 5;
    INFO("(1/5)等待目标板确认包...");
    while(--i)
    {
        if (m_pUpdate->SendResetAndUpdateTag()) break;
    }
    m_pUpdate->DisplayOKorError(i);
    if (i == 0)
    {
        add2Display(_T("错误:等待目标板确认包失败\r\n"));
        return -3;
    }

    //>>>>>>>>S4:Send UPDATE_SOL
    i = 5;
    INFO("(2/5)等待文件长度确认包...");
    while(--i)
    {
        result = m_pUpdate->SendUpdateStartOfLenght(len);
        if (0 != result) break;
    }
    if (-1 == result)
    {
        INFO("Error(文件大小超出Flash范围)\r\n");
        add2Display(_T("错误:文件大小超出Flash范围\r\n"));
        return -4;
    }
    m_pUpdate->DisplayOKorError(i);
    if (i == 0)
    {
        add2Display(_T("错误:等待目标板长度确认包失败\r\n"));
        return -5;
    }

    INFO("(3/5)正在传送数据:\r\n");
    do 
    {
        result = m_pUpdate->SendUpdateStartOfData(len);
        if (result > 0)
        {
            i = 5 + 90 * (len - result)/len;
            //pSmartUpdate->displayProgressAIO(index, i);
        }
    } while(result > 0);
        
    INFO("\r\n(3/5)正在传送数据结果:");
    if (0 == result)
    {
        m_pUpdate->DisplayOKorError(1);
    }
    else 
    {
        m_pUpdate->DisplayOKorError(0);
        if (-1 == result)
        {
            ERROR_INFO("目标板异常终止");
        }
        else if (-2 == result)
        {
            ERROR_INFO("等待响应超时!");
        }
        add2Display(_T("错误:数据传输失败\r\n"));
        return -6;
    }

    //>>>>>>>>S6:Send UPDATE_EOT
    i = 5;
    INFO("(4/5)等待文件结束确认包...");
    while(--i)
    {
        if (m_pUpdate->SendUpdateEndOfTransmit()) break;
    }
    m_pUpdate->DisplayOKorError(i);
    if (i == 0)
    {
        add2Display(_T("错误:等待文件结束确认包失败\r\n"));
        return -7;
    }
    //pSmartUpdate->displayProgressAIO(index, 97);

    INFO("(5/5)等待烧写进Flash:");
    for (i = 0; i < 1000; i++)
    {  
        result = m_pUpdate->WaitUpdateWrite2FlashDone();
        if (0 == result) 
        {
            INFO("OK.\r\n");
            break;
        }
        else if (-1 == result)
        {
            INFO("Error.\r\n");
            break;
        }
        INFO(".");
        if (i % 50 == 0)
        {
            INFO("\r\n");
        }
    }
    if (1000 == i)
    {
        ERROR_INFO("超时\r\n");
    }
    //pSmartUpdate->displayProgressAIO(index, 100);
    
    INFO("\r\n===========================");
    INFO("\r\n===========================");
    INFO("\r\n===========================");
    INFO("\r\n>>>>>>>>烧写已完成<<<<<<<<<\r\n");
    add2Display(_T("\r\n>>>>烧写成功<<<<\r\n"));
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
    PktHandleStartUpdateDspApp();
//    PktHandleStartUpdateStmApp();
}


afx_msg LRESULT CPageAioTest::OnMsgUpdateAiodspApp(WPARAM wParam, LPARAM lParam)
{
    INFO("========================OnMsgUpdateAiodspApp\r\n");
    //PktHandleStartUpdateDspApp();
    return 0;
}


afx_msg LRESULT CPageAioTest::OnMsgUpdateAiostmApp(WPARAM wParam, LPARAM lParam)
{
    INFO("========================OnMsgUpdateAiostmApp\r\n");
    PktHandleStartUpdateStmApp();
    return 0;
}
