
// AioTestFixture.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


//Add by QWB
#include "logfile.h"
#include "DefPrintf.h"
#include "SerialProtocol.h"


#define POWER_ALRAM_MSG                         WM_USER+1 


// CAioTestFixtureApp:
// �йش����ʵ�֣������ AioTestFixture.cpp
//

class CAioTestFixtureApp : public CWinApp
{
public:
	CAioTestFixtureApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CAioTestFixtureApp theApp;