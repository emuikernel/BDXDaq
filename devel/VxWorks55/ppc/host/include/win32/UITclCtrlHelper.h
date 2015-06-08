/* UITclCtrlHelper.h - CUITclCtrlHelper class interface */

/* Copyright 1984-1995 Wind River Systems, Inc. */

/*
modification history
--------------------
01f,24sep98,ren  Exposed ::WindowLockUpdate to TCL
01e,31aug98,ren  Added ChooseFont().  Helps fix unix fonts.
01d,20jul98,c_s  fix callback type-safety issues
01c,01apr98,j_k implement handling of -fixed flag here.
01a,01nov95,j_k written.
*/

/*
DESCRIPTION

This file documents the class interface for CUITclCtrlHelper.

*/

#ifndef __INCUITclCtrlHelperh
#define __INCUITclCtrlHelperh

#pragma warning(disable: 4275)  // deriving exported class from non-exported
#pragma warning(disable: 4251)  // using non-exported as public in exported

/////////////////////////////////////////////////////////////////////////////
// CUITclCtrlHelper

typedef struct Tcl_Interp Tcl_Interp;
typedef CPtrList CServiceMapList;

#include "UITcl.h"

class AFX_EXT_CLASS CUITclCtrlHelper : public CUITclHelper
{
public:
	static int InitCommandOptionsTable();

//	DECLARE_DYNAMIC(CUITclCtrlHelper)
protected:
	CUITclCtrlHelper();

public:
	CUITclCtrlHelper(CWnd* pPrnt, Tcl_Interp* pTclInterp);
	~CUITclCtrlHelper();
	virtual void DeleteObject() = 0;

	UITclCtrlType& ControlType();
	void RouteCmdUICallback(CCmdUI* pCmdUI);

	void InitDefaultServiceTable();
	virtual int WindowCreate(CUITclCommmadInfo* pCmdInfo);

	virtual void UpdateControl(void* clientData) { ; }

 	virtual int ControlCmdUICallbackSet(CUITclCommmadInfo* pCmdInfo);
	virtual int ControlCmdUICallbackGet(CUITclCommmadInfo* pCmdInfo);

	STD_SVC_CALLBACK_IMPL (CUITclCtrlHelper, WindowCreate);
	STD_SVC_CALLBACK_IMPL (CUITclCtrlHelper, WindowTextSet);
	STD_SVC_CALLBACK_IMPL (CUITclCtrlHelper, WindowTextGet);
	STD_SVC_CALLBACK_IMPL (CUITclCtrlHelper, WindowSizeSet);
	STD_SVC_CALLBACK_IMPL (CUITclCtrlHelper, WindowSizeGet);
	STD_SVC_CALLBACK_IMPL (CUITclCtrlHelper, WindowPositionSet);
	STD_SVC_CALLBACK_IMPL (CUITclCtrlHelper, WindowPositionGet);
	STD_SVC_CALLBACK_IMPL (CUITclCtrlHelper, WindowHide);
	STD_SVC_CALLBACK_IMPL (CUITclCtrlHelper, WindowHidden);
	STD_SVC_CALLBACK_IMPL (CUITclCtrlHelper, WindowShow);
	STD_SVC_CALLBACK_IMPL (CUITclCtrlHelper, WindowVisible);
	STD_SVC_CALLBACK_IMPL (CUITclCtrlHelper, WindowEnable);
	STD_SVC_CALLBACK_IMPL (CUITclCtrlHelper, WindowEnabled);
	STD_SVC_CALLBACK_IMPL (CUITclCtrlHelper, WindowHandleGet);
	STD_SVC_CALLBACK_IMPL (CUITclCtrlHelper, WindowHandleSet);
	STD_SVC_CALLBACK_IMPL (CUITclCtrlHelper, WindowParentSet);
	STD_SVC_CALLBACK_IMPL (CUITclCtrlHelper, WindowParentGet);
	STD_SVC_CALLBACK_IMPL (CUITclCtrlHelper, WindowFocusSet);
	STD_SVC_CALLBACK_IMPL (CUITclCtrlHelper, WindowPropertySet);
	STD_SVC_CALLBACK_IMPL (CUITclCtrlHelper, WindowPropertyGet);
	STD_SVC_CALLBACK_IMPL (CUITclCtrlHelper, WindowHelpIDSet);
	STD_SVC_CALLBACK_IMPL (CUITclCtrlHelper, WindowEventGet);
	STD_SVC_CALLBACK_IMPL (CUITclCtrlHelper, WindowContextMenuShow);
	STD_SVC_CALLBACK_IMPL (CUITclCtrlHelper, WindowCallbackSet);
	STD_SVC_CALLBACK_IMPL (CUITclCtrlHelper, WindowCallbackGet);
	STD_SVC_CALLBACK_IMPL (CUITclCtrlHelper, ControlCmdUICallbackSet);
	STD_SVC_CALLBACK_IMPL (CUITclCtrlHelper, ControlCmdUICallbackGet);
	STD_SVC_CALLBACK_IMPL (CUITclCtrlHelper, WindowLockUpdate);
	STD_SVC_CALLBACK_IMPL (CUITclCtrlHelper, WindowUnlockUpdate);


public:
	CString& Tooltip();
	BOOL& AutoCreateSeparator();
	BOOL IsToolbarButton();
	operator CUITclHelper* () const;
	virtual CFont * ChooseFont( );

protected:
	CString			m_csToolTipText;	// Tooltip text if the control is a child of toolbar
	CString			m_csLinkVar;
	CString			m_csCmdUICallback;
	UITclCtrlType	m_ctCtrlType;
	TBBUTTON*		m_pTbButton;
	BOOL			m_bIsToolbarButton;
	BOOL			m_bAutoCreateSep;
	BOOL			m_bUseFixedFont;
	CFont           m_fontNormalText;

};
#endif	// __INCUITclCtrlHelperh
