/* UITclHelper.h - CUITclHelper class interface */

/* Copyright 1984-1995 Wind River Systems, Inc. */

/*
modification history
--------------------
01l,28may99,j_k  Fix for Visual C++ 6.0.
01k,01apr99,c_s  fix warnings
01j,02mar99,c_s  removed dynamic_cast (RTTI thus no longer needed.)
01i,26feb99,j_k  Delete GetName function because there is one already,
                 StringId which behaves correctly.
01h,11feb99,jak  remove ctrl-Ms from macros
		note:  ensure that there are no ctrl-Ms in macros while saving
01g,01feb99,jak  Add GenName()
01f,24sep98,ren  Exposed ::WindowLockUpdate to TCL
01e,20jul98,c_s  fix callback type-safety issues
01d,16jul98,c_s  port to MainWin continues (added type-safety to callbacks)
01c,06jun98,j_k setting of font info for controls.
01b,14apr97,jak added WindowPropertySet&Get
				removed WindowAttributeSet
01a,01nov95,j_k written.
*/

/*
DESCRIPTION

This file documents the class interface for CUITclHelper.

*/

#ifndef __INCUITclHelperh
#define __INCUITclHelperh

#ifdef _MSC_VER
#pragma warning(disable: 4275)  // deriving exported class from non-exported
#pragma warning(disable: 4251)  // using non-exported as public in exported
#endif // _MSC_VER

/////////////////////////////////////////////////////////////////////////////
// CUITclHelper

// include files

#include "afxtempl.h"
#include "UITcl.h"
#include <map>
#define STD_SVC_CALLBACK_IMPL(ClassName, Callback)                      \
    static int ss_##Callback (CUITclHelper * pH, CUITclCommmadInfo *pI) \
        {                                                               \
        ClassName *pX;                                                  \
        if ((pX = static_cast <ClassName *> (pH)))                 	\
            return pX->Callback (pI);                               	\
        else                                                            \
            return 0;                                                   \
        }

#define SVC_METHOD_SET(__Svc, __Callback)                      		\
    do { SetServiceCallback (this, __Svc, ss_##__Callback); } while (0);

// forward declarations

class UITclHelper;

// typedefs

class CServiceMapEntry
    {
    public:
    CServiceMapEntry ()
	: m_pAssocHelper (0),
	  m_ctSvcType (0),
	  m_cbProc (0)
        {}   

    CServiceMapEntry (CUITclHelper * pHelper, UITclCmdType ctSvcType, SvcCbProcWrapper cbProc)
	: m_pAssocHelper (pHelper),
	  m_ctSvcType (ctSvcType),
	  m_cbProc (cbProc)
        {}   

    CUITclHelper *	    m_pAssocHelper;
    UITclCmdType 	    m_ctSvcType;
    SvcCbProcWrapper	    m_cbProc;
    };

typedef CList <CUITclHelper*, CUITclHelper*>			CCtrlList;
#ifdef __SUNPRO_CC
typedef map <UITclCmdType, CServiceMapEntry, less <UITclCmdType> > CServiceMap;
#else
#ifdef PARISC_HPUX10
typedef map <UITclCmdType, CServiceMapEntry> 		CServiceMap;
#else
typedef std::map <UITclCmdType, CServiceMapEntry> 		CServiceMap;
#endif /* PARISC_HPUX10 */
#endif

class AFX_EXT_CLASS CUITclHelper
    {
public:
    // Runtime class checking helpers
    static const AFX_DATA CRuntimeClass classCUITclHelper;
    static CRuntimeClass* PASCAL _GetBaseClass();
    CRuntimeClass* GetRuntimeClass() const;
    BOOL IsA(const CRuntimeClass* pClass) const;
    
    UINT GetNextAvailableCtrlID();
    
protected:
    CUITclHelper();
    
public:
    CUITclHelper(CWnd* pPrnt, Tcl_Interp* pTclInterp);
    ~CUITclHelper();
    virtual void DeleteObject() = 0;
    
    int SetServiceCallback (CUITclHelper * pHelper, LPCTSTR pService, SvcCbProcWrapper cbProc);
    int SetServiceCallback (CUITclHelper * pHelper, UITclCmdType cmdType, SvcCbProcWrapper cbProc)
        {
	m_svcCallbackMap [cmdType] = CServiceMapEntry (pHelper, cmdType, cbProc);
	return 1;
	}
    
    int RouteRequest(UITclCmdType cmdType, CUITclCommmadInfo* pCmdInfo);
    int RouteRequest(const char* pszSvc, CUITclCommmadInfo* pCmdInfo);
    int RouteRequest(const char* pszSvc, int nElems, char** ppData);
    
    virtual void RouteCallback(LPCTSTR lpszEventName);
    
    static BOOL ExtractIntValue(CUITclCommmadInfo* pCmdInfo, const char* lpszOption,
        int* piReturnValue);
    static BOOL ExtractIntValue(CUITclCommmadInfo* pCmdInfo, UITclOptnType optOptionType,
        int* piReturnValue);
    
    static BOOL ExtractBoolValue(CUITclCommmadInfo* pCmdInfo, const char* lpszOption,
        BOOL* pbReturnValue);
    static BOOL ExtractBoolValue(CUITclCommmadInfo* pCmdInfo, UITclOptnType optOptionType,
        BOOL* pbReturnValue);
    
    static BOOL ExtractStringValue(CUITclCommmadInfo* pCmdInfo, const char* lpszOption,
        CString* pcsReturnValue);
    static BOOL ExtractStringValue(CUITclCommmadInfo* pCmdInfo, UITclOptnType optOptionType,
        CString* pcsReturnValue);
    
    void InitDefaultServiceTable ();
    
    virtual int WindowCreate(CUITclCommmadInfo* pCmdInfo);
    virtual int WindowClose(CUITclCommmadInfo* pCmdInfo)
        {return 1;};
    
    virtual int WindowTextSet(CUITclCommmadInfo* pCmdInfo);
    virtual int WindowTextGet(CUITclCommmadInfo* pCmdInfo);
    
    virtual int WindowSizeSet(CUITclCommmadInfo* pCmdInfo);
    virtual int WindowSizeGet(CUITclCommmadInfo* pCmdInfo);
    
    virtual int WindowPositionSet(CUITclCommmadInfo* pCmdInfo);
    virtual int WindowPositionGet(CUITclCommmadInfo* pCmdInfo);
    
    virtual int WindowEnable(CUITclCommmadInfo* pCmdInfo);
    virtual int WindowEnabled(CUITclCommmadInfo* pCmdInfo);
    
    virtual int WindowHide(CUITclCommmadInfo* pCmdInfo);
    virtual int WindowHidden(CUITclCommmadInfo* pCmdInfo);
    
    virtual int WindowShow(CUITclCommmadInfo* pCmdInfo);
    virtual int WindowVisible(CUITclCommmadInfo* pCmdInfo);
    
    virtual int WindowCallbackSet(CUITclCommmadInfo* pCmdInfo);
    virtual int WindowCallbackGet(CUITclCommmadInfo* pCmdInfo);
    
    virtual int WindowHelpIDSet(CUITclCommmadInfo* pCmdInfo);
    virtual int WindowHelpIDGet(CUITclCommmadInfo* pCmdInfo);
    
    virtual int WindowHandleSet(CUITclCommmadInfo* pCmdInfo);
    virtual int WindowHandleGet(CUITclCommmadInfo* pCmdInfo);
    
    virtual int WindowParentSet(CUITclCommmadInfo* pCmdInfo);
    virtual int WindowParentGet(CUITclCommmadInfo* pCmdInfo);
    
    virtual int WindowTimerGet(CUITclCommmadInfo* pCmdInfo);
    virtual int WindowTimerSet(CUITclCommmadInfo* pCmdInfo);
    
    virtual int WindowEventGet(CUITclCommmadInfo* pCmdInfo);
    virtual int WindowFocusSet(CUITclCommmadInfo* pCmdInfo);
    
    virtual int WindowContextMenuShow(CUITclCommmadInfo* pCmdInfo);
    
    virtual int WindowPropertySet(CUITclCommmadInfo* pCmdInfo);
    virtual int WindowPropertyGet(CUITclCommmadInfo* pCmdInfo);

	virtual int WindowLockUpdate(CUITclCommmadInfo* pCmdInfo);
	virtual int WindowUnlockUpdate(CUITclCommmadInfo* pCmdInfo);

    
public:
    // virtual operations. Windows or Controls could override for proper behavior
    virtual void SetWindowSize(int cx, int cy, BOOL bInPixels = FALSE);		// sets the window size provided in dialog units
    virtual CSize GetWindowSize(BOOL bInPixels = FALSE);					// get the window size in dialog units
    
    virtual void SetWindowPosition(int x, int y, BOOL bInPixels = FALSE);	// sets the window position provided in dialog units
    virtual CPoint GetWindowPosition(BOOL bInPixels = FALSE);				// gets the window position in dialog units
    
    virtual void tSetFont(CFont* pFont, BOOL bRedraw);
    virtual void tSetFocus();
    
    virtual BOOL HandleContextMenu(CWnd* pWnd, CPoint point);
    
    BOOL AssertValidWindow();
    
    CUITclHelper* GetControl(LPCTSTR lpcszName);
    CUITclHelper* GetControl(UINT uId);
    CUITclHelper* GetControl(HWND hwnd);
    CUITclHelper* GetControl(POSITION ctrlPos);
    CUITclHelper* GetControl(CPoint point);
    
    CUITclHelper* GetFocusControl();
    UINT ControlHelpIDFromPoint(CPoint point);
    POSITION GetControlPosition(LPCTSTR lpszStr);
    
    BOOL SetFontInfo(Tcl_Interp* pInterp, CString& csFontInfo);
    
    virtual HWND& ControlHWND();
    virtual CString& StringId();
    virtual Tcl_Interp*& TclInterpreter();
    virtual CUITclHelper*& ParentHelper();
    virtual HWND& AssociatedWindow();
    virtual void*& AssociatedCWnd();
    virtual HWND& ParentWindow();
    virtual UINT& ControlID();
    virtual UINT& HelpID();
    virtual CString& HelpFile();
    virtual CString& Callback();
    virtual CString& SizeCallback();
    virtual CString& TimerCallback();
    CSize GetOriginalWndSize();
    CPoint GetOriginalWndPosition();
    
    virtual BOOL ShowHelp(UINT uHelpId);
    CString& WindowMenuName();
    
    DWORD GetDefaultInitialStyles();
    void GetEventInfo(Tcl_DString& tdsEventInfo);
    
    virtual void RemoveControl(LPCTSTR lpcszCtrlName, BOOL bDeleteAlso = FALSE);
    virtual void RemoveAllControls(BOOL bDeleteAlso = FALSE);

public:
    virtual void TclOnTimer(UINT nIDEvent);
    virtual void TclOnSetFocus(CWnd* pOldWnd);
    virtual void TclOnPaletteChanged(CWnd* pOldWnd);
    virtual void TclOnSize(UINT nType, int cx, int cy);
    virtual void TclOnShowWindow(BOOL bShow, UINT nStatus);
    virtual void TclOnContextMenu(CWnd* pWnd, CPoint point);
    
	virtual CPalette * GetPalette();
    virtual void SetPalette();
	virtual void AdjustPalette(CWnd* pFocusWnd);
    
protected:
    // CUITclMapStringToPtr	m_svcDefCallbackMap;
    CServiceMap			m_svcCallbackMap;
    
    Tcl_Interp*			m_pTclInterp;		// Associated tcl interpreter
    void*			m_pAssocCWnd;
    HWND			m_hwndAssocWnd;		// Associated Window object
    HWND			m_hwndParentWin;	// Handle of the parent Window
    CUITclHelper*	        m_pParentHelper;
    CString			m_csLinkVar;		// Tcl variable that is linked to the value of this control
    
    CPoint			m_cpWndPos;			// Position of the window on the parent's client in pixels
    CSize			m_cWndSz;			// Size of the window in pixels
    CPoint			m_cpLastMenuLoc;
    
    UINT			m_uCtrlID;
    UINT			m_uHelpId;
	DWORD			m_dwHelpContextType; // gets set in constructor of Wnd/Ctrl Helper
    CFont*			m_pWndFont;
    
    DWORD			m_dwCmnStyle;		// Common styles that can be set at creation time
    
    CString			m_csWndId;			// String Id that is used to identify the window
    CString			m_csHelpFile;
    CString			m_csTooltip;
    
    CString			m_csCallback;		// Callback script for the control.
    CString			m_csSizeCb;
    CString			m_csTimerCb;
    
    CString			m_csAssocMenuName;
    
    Tcl_DString		        m_tdsEventName;		// last event (window message) that has occured on a child control
    
    CCtrlList*		        m_pCtrlList;		// list of pointer to all the controls on the window
    UINT			m_uNextCtrlId;		// next control id available for ControlCrete function.
    UINT			m_uTimerId;			// current timer id if one is started
    UINT			m_uNextTimerId;		// next timer id that is available for use
   
    };

/////////////////////////////////////////////////////////////////////////////
#endif	// __INCUITclHelperh
