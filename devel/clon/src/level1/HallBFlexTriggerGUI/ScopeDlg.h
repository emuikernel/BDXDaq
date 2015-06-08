#if !defined(AFX_SCOPEDLG_H__08339D82_0641_423A_8F68_4171E28027A3__INCLUDED_)
#define AFX_SCOPEDLG_H__08339D82_0641_423A_8F68_4171E28027A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScopeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ScopeDlg dialog

class ScopeDlg : public CDialog
{
// Construction
public:
	BOOL ReadoutScope(void);
	BOOL UpdateTriggerMasks();
	BOOL m_bTriggerContinuous;
	int m_iTriggerBuffer[96][128];
	void DrawScope(BOOL bDrawTraceOnly = FALSE);
	void UpdateCursor(int x, int y, BOOL updatePosition, BOOL eraseold);
	int m_iTriggerPattern[96];
	int m_iCursorSample;
	CString m_sSignalNames[96];
	ScopeDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(ScopeDlg)
	enum { IDD = IDD_SCOPE_DIALOG };
	CButton	m_chPersist;
	CScrollBar	m_scrollScope;
	CString	m_stCursorTime;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ScopeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ScopeDlg)
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBScopecontinuous();
	afx_msg void OnBScopesingle();
	afx_msg void OnBScopestop();
	afx_msg void OnBScopeclear();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCOPEDLG_H__08339D82_0641_423A_8F68_4171E28027A3__INCLUDED_)
