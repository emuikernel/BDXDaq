#if !defined(AFX_DEBUGDLG_H__BAAEB786_C2DC_421C_8F4F_A9491BF6D2DA__INCLUDED_)
#define AFX_DEBUGDLG_H__BAAEB786_C2DC_421C_8F4F_A9491BF6D2DA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DebugDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// DebugDlg dialog

class DebugDlg : public CDialog
{
// Construction
public:
	void DebugMsg(CString msg);
	DebugDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(DebugDlg)
	enum { IDD = IDD_DEBUG_DIALOG };
	CButton	m_chDebugAll;
	CListBox	m_listDebug;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DebugDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(DebugDlg)
	afx_msg void OnBDbgclear();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEBUGDLG_H__BAAEB786_C2DC_421C_8F4F_A9491BF6D2DA__INCLUDED_)
