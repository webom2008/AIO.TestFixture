// PageAioUpdate.cpp : 实现文件
//

#include "stdafx.h"
#include "AioTestFixture.h"
#include "PageAioUpdate.h"
#include "afxdialogex.h"


// CPageAioUpdate 对话框

IMPLEMENT_DYNAMIC(CPageAioUpdate, CPropertyPage)

CPageAioUpdate::CPageAioUpdate()
	: CPropertyPage(CPageAioUpdate::IDD)
{

}

CPageAioUpdate::~CPageAioUpdate()
{
}

void CPageAioUpdate::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageAioUpdate, CPropertyPage)
END_MESSAGE_MAP()


// CPageAioUpdate 消息处理程序
