
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


#define MSG_POWER_ALRAM                         WM_USER+1 
#define MSG_AIO_VERSION_UPDATE                  WM_USER+2 
#define MSG_UPDATE_AIODSP_APP                   WM_USER+3 
#define MSG_UPDATE_AIOSTM_APP                   WM_USER+4 


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