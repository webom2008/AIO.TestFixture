#pragma once


// CPageDemo 对话框

class CPageDemo : public CPropertyPage
{
	DECLARE_DYNAMIC(CPageDemo)

public:
	CPageDemo();
	virtual ~CPageDemo();

// 对话框数据
	enum { IDD = IDD_DLG_DEMO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
