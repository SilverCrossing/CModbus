#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含 'pch.h' 以生成 PCH"
#endif

#include "resource.h"


class CModbusMasterApp : public CWinApp
{
public:
	CModbusMasterApp();

public:
	virtual BOOL InitInstance();


	DECLARE_MESSAGE_MAP()
};

extern CModbusMasterApp theApp;
