#pragma once


// CPageAioTest 对话框

class CPageAioTest : public CPropertyPage
{
	DECLARE_DYNAMIC(CPageAioTest)

public:
	CPageAioTest();
	virtual ~CPageAioTest();
    
    static int WINAPI   PktHandlePowerResult(LPVOID pParam, UartProtocolPacket *pPacket);

    int        initApplication(void);
// 对话框数据
	enum { IDD = IDD_DLG_TEST };
private:
    void refreshPowerAlarmStatus(LPVOID pWnd, void *param);
    bool initApplicationDone;

    CBitmapButton m_BtnStatusD3V3E;
    CBitmapButton m_BtnStatusD5V6N;
    CBitmapButton m_BtnStatusD3V3N;
    CBitmapButton m_BtnStatusD5VAN;
    CBitmapButton m_BtnStatusD5VSPO2;
    CBitmapButton m_BtnStatusD5VNIBP;
    CBitmapButton m_BtnStatusREF2V5N;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
protected:
    afx_msg LRESULT OnPowerAlramMsg(WPARAM wParam, LPARAM lParam);
};
