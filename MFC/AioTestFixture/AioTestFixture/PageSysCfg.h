#pragma once
#include "afxwin.h"


// CPageSysCfg �Ի���

class CPageSysCfg : public CPropertyPage
{
	DECLARE_DYNAMIC(CPageSysCfg)

public:
	CPageSysCfg();
	virtual ~CPageSysCfg();

    int initApplication(void);
// �Ի�������
	enum { IDD = IDD_DLG_CONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    CComboBox m_SerialNumbSel;
    CComboBox m_SerialBaudSel;
    CButton m_SerialOpenCtrl;
    afx_msg void OnBnClickedBtnSerialOpen();
};
