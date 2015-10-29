#pragma once
#include "afxwin.h"


#include "MyUsbDevice.h"
// CPageSysCfg 对话框

class CPageSysCfg : public CPropertyPage
{
	DECLARE_DYNAMIC(CPageSysCfg)

public:
	CPageSysCfg();
	virtual ~CPageSysCfg();

    int initApplication(void);
// 对话框数据
	enum { IDD = IDD_DLG_CONFIG };

private:
    CMyUsbDevice        *m_pMyUsbDev;
    CStringArray        m_StrArrayUSBInfo;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    CComboBox       m_SerialNumbSel;
    CComboBox       m_SerialBaudSel;
    CButton         m_SerialOpenCtrl;
    CButton         m_BtnWaveConnnetCtrl;
    CComboBox       m_ComboUSBSel;
    CEdit           m_EditUsbInfo;

    virtual BOOL    OnInitDialog();
    afx_msg void    OnBnClickedBtnSerialOpen();
    afx_msg void    OnBnClickedBtnWaveConnect();
    afx_msg void OnCbnSelchangeComboUsbSel();
};
