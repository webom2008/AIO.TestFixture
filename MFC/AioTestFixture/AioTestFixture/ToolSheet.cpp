// ToolSheet.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "AioTestFixture.h"
#include "ToolSheet.h"

#include <io.h>
#include <fcntl.h>
#include <stdio.h>

const char VERSION[] = "V0.0.1";

extern CSerialProtocol *g_pSerialProtocol;

IMPLEMENT_DYNAMIC(CToolSheet, CPropertySheet)

CToolSheet::CToolSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{

}

CToolSheet::CToolSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
    g_pSerialProtocol = new CSerialProtocol;
    g_pSerialProtocol->initApplication();
    
    this->m_psh.dwFlags |= PSH_NOAPPLYNOW;
    this->m_psh.dwFlags &= ~(PSH_HASHELP);
    
    m_PageSysCfg.m_psp.dwFlags &= ~(PSP_HASHELP);
    AddPage(&m_PageSysCfg);
    
    m_PageAioUpdate.m_psp.dwFlags &= ~(PSP_HASHELP);
    AddPage(&m_PageAioUpdate);

    m_PageAioTest.m_psp.dwFlags &= ~(PSP_HASHELP);
    AddPage(&m_PageAioTest);

#ifdef CONFIG_DEBUG_PAGE_SHOW
    m_PageDebug.m_psp.dwFlags &= ~(PSP_HASHELP);
    AddPage(&m_PageDebug);
#endif
//    m_PageDemo.m_psp.dwFlags &= ~(PSP_HASHELP);
//    AddPage(&m_PageDemo);
}

CToolSheet::~CToolSheet()
{
    Log2File("Close aPM12Tool \r\n");
    if (NULL != g_pSerialProtocol)
    {
        delete g_pSerialProtocol;
        g_pSerialProtocol = NULL;
    }
}


BEGIN_MESSAGE_MAP(CToolSheet, CPropertySheet)
END_MESSAGE_MAP()


// CToolSheet ��Ϣ�������
void CToolSheet::initApplication(void)
{
    m_PageSysCfg.initApplication();
    m_PageAioUpdate.initApplication();
    m_PageAioTest.initApplication();
    
#ifdef CONFIG_DEBUG_PAGE_SHOW
    m_PageDebug.initApplication();
#endif
}

void InitConsoleWindow(void)
{
    INFO("<!-- ========================================= -->\r\n");
    INFO("// Copyright (C) 2015 QiuWeibo <qiuweibo@cvte.com>\r\n");
    INFO("// �������� : ���Դ�ӡ����\r\n");
    INFO("// ����汾 : %s\r\n",VERSION);
    INFO("// ��    �� : ��ΰ��\r\n");
    INFO("// ʱ    �� : %s %s\r\n",__DATE__, __TIME__);
    INFO("<!-- ========================================= -->\r\n");
}


BOOL CToolSheet::OnInitDialog()
{
    BOOL bResult = CPropertySheet::OnInitDialog();

#ifndef CONFIG_CONSOLE_USED
    if (TRUE == PathFileExists("console"))
#endif
    {
        Logconsole_open();
        InitConsoleWindow();
        Log2File("Open aPM12Tool \r\n");
    }

    //ȥ����ȷ�����͡�ȡ������ť
    CWnd *pWnd = GetDlgItem(IDOK);
    if(pWnd && pWnd->GetSafeHwnd())
    {
        pWnd->ShowWindow(false);
    }
    pWnd = GetDlgItem(IDCANCEL); 
    if(pWnd && pWnd->GetSafeHwnd())
    {
        pWnd->ShowWindow(false); 
    }

    //ȥ���ĸ���ť�����������·��Ŀհ�
    GetDlgItem(IDOK)->ShowWindow(SW_HIDE);
    GetDlgItem(IDHELP)->ShowWindow(SW_HIDE);
    GetDlgItem(IDCANCEL)->ShowWindow(SW_HIDE);
    GetDlgItem(ID_APPLY_NOW)->ShowWindow(SW_HIDE);
    
    //��ȡ����ߴ� 
    CRect btnRect; 
    GetDlgItem(IDCANCEL)->GetWindowRect(&btnRect);
    CRect wdnRect; 
    GetWindowRect(&wdnRect); 
    //���������С 
    //::SetWindowPos(this->m_hWnd, HWND_TOP, 0,0,wdnRect.Width(),wdnRect.Height() - btnRect.Height(), SWP_NOMOVE | SWP_NOZORDER);
    ::SetWindowPos(this->m_hWnd, HWND_TOP, 0,0,640,480, SWP_NOMOVE | SWP_NOZORDER);
    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL)
    {
        pSysMenu->AppendMenu(MF_STRING, SC_MINIMIZE, "��С��");
        ModifyStyle( 0, WS_MINIMIZEBOX);
    }

    initApplication();
    return bResult;
}
