#pragma once


// CPageDebug �Ի���

class CPageDebug : public CPropertyPage
{
	DECLARE_DYNAMIC(CPageDebug)

public:
	CPageDebug();
	virtual ~CPageDebug();
    
    int        initApplication(void);
// �Ի�������
	enum { IDD = IDD_DLG_DEBUG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedBtnGetTaskUtilites();
};
