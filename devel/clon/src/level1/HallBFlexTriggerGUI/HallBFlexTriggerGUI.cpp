// HallBFlexTriggerGUI.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "HallBFlexTriggerGUI.h"
#include "HallBFlexTriggerGUIDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHallBFlexTriggerGUIApp

BEGIN_MESSAGE_MAP(CHallBFlexTriggerGUIApp, CWinApp)
	//{{AFX_MSG_MAP(CHallBFlexTriggerGUIApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHallBFlexTriggerGUIApp construction

CHallBFlexTriggerGUIApp::CHallBFlexTriggerGUIApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CHallBFlexTriggerGUIApp object

CHallBFlexTriggerGUIApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CHallBFlexTriggerGUIApp initialization

BOOL CHallBFlexTriggerGUIApp::InitInstance()
{
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	// Standard initialization

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CHallBFlexTriggerGUIDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
