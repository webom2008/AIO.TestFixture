// PageDemo.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "AioTestFixture.h"
#include "PageDemo.h"
#include "afxdialogex.h"


// CPageDemo �Ի���

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


// CPageDemo ��Ϣ�������
