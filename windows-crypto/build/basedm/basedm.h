
// basedm.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CbasedmApp:
// �йش����ʵ�֣������ basedm.cpp
//

class CbasedmApp : public CWinApp
{
public:
	CbasedmApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CbasedmApp theApp;