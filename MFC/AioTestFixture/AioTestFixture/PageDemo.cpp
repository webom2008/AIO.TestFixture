// PageDemo.cpp : 实现文件
//

#include "stdafx.h"
#include "AioTestFixture.h"
#include "PageDemo.h"
#include "afxdialogex.h"


// CPageDemo 对话框

IMPLEMENT_DYNAMIC(CPageDemo, CPropertyPage)

CPageDemo::CPageDemo()
	: CPropertyPage(CPageDemo::IDD)
{

}

CPageDemo::~CPageDemo()
{
}

void CPageDemo::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageDemo, CPropertyPage)
END_MESSAGE_MAP()


// CPageDemo 消息处理程序
