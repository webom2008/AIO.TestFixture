#pragma once

#include "configs.h"
#include "PageDemo.h"
#include "PageSysCfg.h"
#include "PageAioTest.h"
#include "PageAioUpdate.h"

// CToolSheet

class CToolSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CToolSheet)

public:
	CToolSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CToolSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CToolSheet();

    void initApplication(void);

private:
    CPageDemo           m_PageDemo;
    CPageSysCfg         m_PageSysCfg;
    CPageAioUpdate      m_PageAioUpdate;
    CPageAioTest        m_PageAioTest;

protected:
	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
};


