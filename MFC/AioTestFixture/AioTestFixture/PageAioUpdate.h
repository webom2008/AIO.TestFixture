#pragma once

#include <map>
#include "Update.h"
#include "afxcmn.h"
#include "afxwin.h"


// CPageAioUpdate 对话框

class CPageAioUpdate : public CPropertyPage, public CUpdate
{
	DECLARE_DYNAMIC(CPageAioUpdate)

public:
	CPageAioUpdate();
	virtual ~CPageAioUpdate();

    int         initApplication(void);
    int         checkTarget(BYTE &CID, CString &path);

    static int WINAPI    PktHandleGetVersion(LPVOID pParam, UartProtocolPacket *pPacket);

// 对话框数据
	enum { IDD = IDD_DLG_UPDATE };
    
protected:
    BYTE        detectBinFile(void);
    void        displayProgressAIO(BYTE index, int value);
    void        addInfo2Display(CString str);
    void        cleanInfo2Display(void);
    
private:
    std::map<BYTE, CString> m_Target;
    BYTE        m_AioBinFileCount;
    // Thread
    bool                m_bUpdateThreadRun;
    static UINT	        UpdateThread(LPVOID pParam);
	CWinThread*	        m_UpdateThread;

protected:
    afx_msg LRESULT OnMsgAioVersionUpdate(WPARAM wParam, LPARAM lParam);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedBtnGetAioVer();
    afx_msg void OnBnClickedBtnAioUpdate();
    CEdit m_EditAIOVersion;
    CProgressCtrl m_ProgressAIO;
    CEdit m_EditUpdateDisplay;
    virtual BOOL OnInitDialog();
};
