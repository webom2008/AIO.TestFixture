#pragma once


// CPageAioUpdate �Ի���

class CPageAioUpdate : public CPropertyPage
{
	DECLARE_DYNAMIC(CPageAioUpdate)

public:
	CPageAioUpdate();
	virtual ~CPageAioUpdate();

// �Ի�������
	enum { IDD = IDD_DLG_UPDATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
