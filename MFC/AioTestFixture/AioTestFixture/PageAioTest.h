#pragma once
#include <map>
#include "Update.h"
#include "afxwin.h"

// CPageAioTest 对话框

class CPageAioTest : public CPropertyPage
{
	DECLARE_DYNAMIC(CPageAioTest)

public:
	CPageAioTest();
	virtual ~CPageAioTest();
    
    static int WINAPI   PktHandlePowerResult(LPVOID pParam, UartProtocolPacket *pPacket);
    void    add2Display(CString &str);
    void    add2Display(LPCSTR pStr);
    void    clearDisplay(void);
    int     initApplication(void);
    void    createAioDspAppUpdateThread(void);
    void    createAioDspStmUpdateThread(void);

// 对话框数据
	enum { IDD = IDD_DLG_TEST };
private:
    bool initApplicationDone;
    CBitmapButton m_BtnTestStatus;
    std::map<BYTE, CString> m_Target;
    CUpdate                 *m_pUpdate;

    void    setTestResultFlag(void);
    void    clearTestResultFlag(void);
    int     PktHandleErrorInfo(UartProtocolPacket *pPacket);
    int     PktHandleProcessState(UartProtocolPacket *pPacket);
    int     PktHandleConnectted(UartProtocolPacket *pPacket);
    int     PktHandleWaveformComm(UartProtocolPacket *pPacket);
    int     PktHandleWaveformConnect(UartProtocolPacket *pPacket);
    int     checkAndPrintPowerInfo(void *arg);
    int     detectBinFile(const char *wildcard,CString &name);
    int     updateTask(BYTE &CID, CString &name);
    
    static UINT AioDspAppUpdateThread(LPVOID pParam);
    static UINT AioDspStmUpdateThread(LPVOID pParam);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    
	DECLARE_MESSAGE_MAP()
public:
    CEdit m_EditDisplay;
    
    afx_msg void OnEnSetfocusEditDisplay();
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedBtnClean();
};
