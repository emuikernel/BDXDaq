#if !defined(AFX_TESTDIALOG_H__00F95179_CC63_4313_B60A_DE4A0C4284CC__INCLUDED_)
#define AFX_TESTDIALOG_H__00F95179_CC63_4313_B60A_DE4A0C4284CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TestDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// TestDialog dialog

class TestDialog : public CDialog
{
// Construction
public:
	TestDialog(CWnd* pParent = NULL);   // standard constructor
	unsigned int StrToHex(CString val);
// Dialog Data
	//{{AFX_DATA(TestDialog)
	enum { IDD = IDD_TEST_DIALOG };
	CString	m_csPattern;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TestDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(TestDialog)
	afx_msg void OnBPattern();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTDIALOG_H__00F95179_CC63_4313_B60A_DE4A0C4284CC__INCLUDED_)
