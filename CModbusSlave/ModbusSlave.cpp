﻿#include "pch.h"
#include "framework.h"
#include "ModbusSlave.h"
#include "ModbusSlaveDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



BEGIN_MESSAGE_MAP(CModbusSlaveApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()



CModbusSlaveApp::CModbusSlaveApp()
{
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

}



CModbusSlaveApp theApp;



BOOL CModbusSlaveApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	CShellManager *pShellManager = new CShellManager;

	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	SetRegistryKey(_T("The local application generated by the application wizard."));

	CModbusSlaveDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{

	}
	else if (nResponse == IDCANCEL)
	{

	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Warning: Dialog creation fails and the application will terminate unexpectedly.\n");
		TRACE(traceAppMsg, 0, "Warning: If you use MFC controls on dialog boxes，You cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}

	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	return FALSE;
}
