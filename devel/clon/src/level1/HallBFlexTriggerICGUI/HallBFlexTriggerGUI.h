// HallBFlexTriggerGUI.h : main header file for the HALLBFLEXTRIGGERGUI application
//

#if !defined(AFX_HALLBFLEXTRIGGERGUI_H__73970707_D600_4B42_B472_AE7FB17D8DF8__INCLUDED_)
#define AFX_HALLBFLEXTRIGGERGUI_H__73970707_D600_4B42_B472_AE7FB17D8DF8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CHallBFlexTriggerGUIApp:
// See HallBFlexTriggerGUI.cpp for the implementation of this class
//

class CHallBFlexTriggerGUIApp : public CWinApp
{
public:
	CHallBFlexTriggerGUIApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHallBFlexTriggerGUIApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CHallBFlexTriggerGUIApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HALLBFLEXTRIGGERGUI_H__73970707_D600_4B42_B472_AE7FB17D8DF8__INCLUDED_)
