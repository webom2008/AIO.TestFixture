#pragma once


// CPageDemo �Ի���

class CPageDemo : public CPropertyPage
{
	DECLARE_DYNAMIC(CPageDemo)

public:
	CPageDemo();
	virtual ~CPageDemo();

// �Ի�������
	enum { IDD = IDD_DLG_DEMO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
