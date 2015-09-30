#pragma once


// CPageAioUpdate 对话框

class CPageAioUpdate : public CPropertyPage
{
	DECLARE_DYNAMIC(CPageAioUpdate)

public:
	CPageAioUpdate();
	virtual ~CPageAioUpdate();

// 对话框数据
	enum { IDD = IDD_DLG_UPDATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
