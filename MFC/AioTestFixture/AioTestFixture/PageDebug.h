#pragma once


// CPageDebug 对话框

class CPageDebug : public CPropertyPage
{
	DECLARE_DYNAMIC(CPageDebug)

public:
	CPageDebug();
	virtual ~CPageDebug();
    
    int        initApplication(void);
// 对话框数据
	enum { IDD = IDD_DLG_DEBUG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedBtnGetTaskUtilites();
    afx_msg void OnBnClickedBtnResetDlCnt();
    afx_msg void OnBnClickedBtnDecDlCnt();
    afx_msg void OnBnClickedBtnResetFlash();
    afx_msg void OnBnClickedBtnWave1();
    afx_msg void OnBnClickedBtnWave2();
};
