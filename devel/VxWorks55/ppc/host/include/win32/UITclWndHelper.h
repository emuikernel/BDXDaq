/* UITclWndHelper.h - CUITclWndHelper class interface */

/* Copyright 1984-1995 Wind River Systems, Inc. */

/*
modification history
--------------------
01g,26feb99,j_k  Centralize ShowHelp function into CUITclHelper.
01f,18feb99,jak  add SetWndAppHelpFile
01e,24sep98,ren  Exposed ::WindowLockUpdate to TCL
01d,03sep98,ren  Added SetChildrenFont to fix font problems on UNIX
01c,20jul98,c_s  fix callback type-safety issues
01b,14apr98,jak removed WindowPropertyGet&Set defs
01a,01nov95,j_k written.
*/

/*
DESCRIPTION

This file documents the class interface for CUITclWndHelper.

*/

#ifndef __INCUITclWndHelperh
#define __INCUITclWndHelperh

#ifdef _MSC_VER
#pragma warning(disable: 4275)
#pragma warning(disable: 4251)
#endif // _MSC_VER

// Forward declarations
#include "afxtempl.h"
#include "tcl.h"

#include "UITcl.h"
#include "UITclHelper.h"

typedef struct Tcl_Interp Tcl_Interp;

class CUITclMenu;
class CUITclToolBar;
class CUITclCtrlHelper;
class CUITclCommmadInfo;

/////////////////////////////////////////////////////////////////////////////
// CUITclWndHelper
typedef CList<CUITclHelper*, CUITclHelper*> CCtrlList;

class AFX_EXT_CLASS CUITclWndHelper : public CUITclHelper
{
public:
	static int InitCommandOptionsTable();

	// Runtime class checking helpers
#ifndef PARISC_HPUX10
	static const AFX_DATA CRuntimeClass classCUITclWndHelper;
#endif
	static const CRuntimeClass* PASCAL _GetBaseClass();
	CRuntimeClass* GetRuntimeClass() const;
	BOOL IsA(const CRuntimeClass* pClass) const;

	operator CUITclWndHelper*()
	{
		return this;
	}

public:
	CUITclWndHelper(CWnd* pPrnt = NULL, Tcl_Interp* pTclInterp = 0);
	~CUITclWndHelper();
	virtual void DeleteObject() = 0;

        CUITclWndHelper* GetTclWndHelper(HWND hWnd);
	static CUITclWndHelper* GetTclWndHelper(LPCTSTR lpszTclHelper, Tcl_Interp* pInterp = 0);
	static void RemoveFromTclHelpers(CUITclWndHelper* pTclHelper);

	virtual void RouteCallback(LPCTSTR lpszEventName);

	void InitDefaultServiceTable();
	virtual int WindowCreate(CUITclCommmadInfo* pCmdInfo);

	virtual BOOL SetIconFile(LPCTSTR lpszIconFile);
	LPCTSTR  GetIconFile();
	
	void AddToolbar(CUITclWndHelper* pToolbar);
	void RemoveToolbar(CUITclWndHelper* pToolbar);

	BOOL SetWndAppHelpFile(const char* lpszHelpFile);

	CString& ShowCallback();
	CString& InitCallback();
	CString& ExitCallback();
	CString& ActivateCallback();
	CString& QueryCloseCallback();

	operator CUITclHelper* () const;

public:
	virtual int WindowClientSizeGet(CUITclCommmadInfo* pCmdInfo);

	virtual int WindowMenuGet(CUITclCommmadInfo* pCmdInfo);
	virtual int WindowMenuSet(CUITclCommmadInfo* pCmdInfo);

	virtual int WindowInitCallbackSet(CUITclCommmadInfo* pCmdInfo);
	virtual int WindowInitCallbackGet(CUITclCommmadInfo* pCmdInfo);

	virtual int WindowExitCallbackSet(CUITclCommmadInfo* pCmdInfo);
	virtual int WindowExitCallbackGet(CUITclCommmadInfo* pCmdInfo);

	virtual int WindowShowCallbackSet(CUITclCommmadInfo* pCmdInfo);
	virtual int WindowShowCallbackGet(CUITclCommmadInfo* pCmdInfo);

	virtual int WindowSizeCallbackSet(CUITclCommmadInfo* pCmdInfo);
	virtual int WindowSizeCallbackGet(CUITclCommmadInfo* pCmdInfo);

	virtual int WindowActivateCallbackSet(CUITclCommmadInfo* pCmdInfo);
	virtual int WindowActivateCallbackGet(CUITclCommmadInfo* pCmdInfo);

	virtual int WindowQueryCloseCallbackSet(CUITclCommmadInfo* pCmdInfo);
	virtual int WindowQueryCloseCallbackGet(CUITclCommmadInfo* pCmdInfo);

	virtual int WindowTitleGet(CUITclCommmadInfo* pCmdInfo);
	virtual int WindowTitleSet(CUITclCommmadInfo* pCmdInfo);

    virtual int WindowShow(CUITclCommmadInfo* pCmdInfo);
    virtual int WindowActiveGet(CUITclCommmadInfo* pCmdInfo);
    virtual int WindowActiveSet(CUITclCommmadInfo* pCmdInfo);

	int WindowControlDestroy(CUITclCommmadInfo* pCmdInfo);
	int AllControlsDestroy(CUITclCommmadInfo* pCmdInfo);
	
	virtual void SetInitialControls(const char* lpszCtrls);
	virtual void ControlsCreate(const char* pszCtrlsInfo);
	virtual BOOL ControlCreate(char* pszCtrlInfo);
	virtual BOOL ControlCreate(UITclCtrlType ctrlType, CUITclCommmadInfo* pCmdInfo);

	LPCTSTR GetToolTipFor(int id);
	UINT ControlHelpIDFromPoint(CPoint point);

	virtual BOOL SetWindowMenu(CUITclMenu* pNewMenu);
	virtual HMENU GetWindowMenu();

	virtual int WindowClose(CUITclCommmadInfo* pCmdInfo);
	virtual void WindowActivate();

        virtual CString ActiveWindowGet();
        virtual int ActiveWindowSet(CString csWndName);

	virtual BOOL QueryClose();
	virtual BOOL TclOnHelp();
	virtual void TclOnClose();
	virtual void TclOnDestroy();
	virtual void TclDoDataExchange(CDataExchange* pDX);
	virtual void TclOnInitialUpdate();
	virtual void TclOnActivate(UINT nState, BOOL bMinimized);
	virtual BOOL TclOnCommandHelp(WPARAM wParam, LPARAM lParam);

	void TclOnUpdateEditMenu(CCmdUI* pCmdUI);
	void TclOnEditCopy();
	void TclOnEditCut();
	void TclOnEditPaste();
	void TclOnEditClear();
	void TclOnEditSelectAll();
	void TclOnEditUndo();

	virtual void LoadState();
	virtual void SaveState();

	virtual CSize GetClientSize(BOOL bInPixels);

	virtual void SetWindowTitle(LPCTSTR pszNewTitle);
	virtual LPCTSTR GetWindowTitle();

	BOOL OnUserCommandID(UINT nID);
	BOOL OnUpdateUserCommandID(CCmdUI* pCmdUI);
	
	const char* CreateToolbar(CString pName, DWORD dwStyle);
	CUITclToolBar* CreateToolbar(CFrameWnd* pParentFrm, Tcl_Interp* pInterp,
											   CString csName, DWORD dwExtraStyle, UINT uID);

	virtual void PreDestroyWindow();
	virtual CFont * ChooseFont( );

protected:
	BOOL m_bVertScrollBar;			// state of the vertical scroll bar
	BOOL m_bHorzScrollBar;			// state of the horizontal scroll bar
	HICON m_hWinIcon;				// icon associated with the window
	HMENU m_hWinMenu;				// menu associated with the window
	char* m_pCtrlsStrList;			// string list of controls created initially
	BOOL m_bPersistent;
	BOOL m_bDestroyCalled;

	CString m_csExecScript;
	CString m_csTitleStr;
	CString m_csWndIconFile;
	CString m_csShowCb;
	CString m_csInitCb;				// tcl callback when the window is being initialized on the display
	CString m_csExitCb;				// tcl callback when the window is being removed from the display
	CString m_csQueryCloseCb;		// tcl callback when the window is about to be closed
	CString m_csContextMenuCb;		// tcl callback when the context menu button is pressed by user
	CString m_csActivateCb;			// tcl callback when the window is being activated either by 
									//     by using the mouse or otherwise
	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowCreate);
	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowTextSet);
	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowTextGet);

	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowSizeSet);
	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowSizeGet);

	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowClientSizeGet);

	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowPositionSet);
	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowPositionGet);

	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowHide);
	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowHidden);

	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowShow);
	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowVisible);

	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowActiveGet);
	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowActiveSet);

	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowEnable);
	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowEnabled);

	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowHandleGet);
	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowHandleSet);

	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowParentSet);
	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowParentGet);

	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowMenuSet);
	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowMenuGet);

	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowTitleSet);
	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowTitleGet);

	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowFocusSet);

	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowHelpIDSet);

	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowClose);
	
	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowEventGet);

	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowContextMenuShow);

	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowInitCallbackSet);
	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowInitCallbackGet);

	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowExitCallbackSet);
	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowExitCallbackGet);

	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowShowCallbackSet);
	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowShowCallbackGet);

	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowSizeCallbackSet);
	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowSizeCallbackGet);

	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowTimerSet);
	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowTimerGet);

	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowActivateCallbackSet);
	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowActivateCallbackGet);

	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowQueryCloseCallbackSet);
	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowQueryCloseCallbackGet);

	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowControlDestroy);
	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, AllControlsDestroy);

	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowPropertySet);
	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowPropertyGet);
	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowLockUpdate);
	STD_SVC_CALLBACK_IMPL (CUITclWndHelper, WindowUnlockUpdate);

protected:
	CPtrList m_lTclFrameBars;
	BOOL m_bTooltips;
	CFont           m_fontNormalText;

	CUITclCtrlHelper* TclOnCtrlHitTest(CPoint point, CPoint* pCenter);
	BOOL TclPreTranslateMessage(MSG *pMsg) {return 0L;}
	BOOL TclOnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
	UINT TclOnCmdHitTest(CPoint point, CPoint* pCenter);
	void SetChildrenFont( CWnd * pwndParent, CFont * pfont);
private:
	void idst1 ();
	void idst2 ();
};

/////////////////////////////////////////////////////////////////////////////

#endif	// __INCUITclWndHelperh
