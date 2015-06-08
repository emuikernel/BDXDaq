#if !defined(AFX_TRIGGER12DLG_H__00A37914_9C67_4A48_8FAE_BA56C7C28882__INCLUDED_)
#define AFX_TRIGGER12DLG_H__00A37914_9C67_4A48_8FAE_BA56C7C28882__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Trigger12Dlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Trigger12Dlg dialog

class Trigger12Dlg : public CDialog
{
// Construction
public:
	void UpdateControls();
	Trigger12Dlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(Trigger12Dlg)
	enum { IDD = IDD_TRIGGER12_DIALOG };
	CSpinButtonCtrl	m_spTrig_Prescale;
	CString	m_stTrig_Scaler;
	CString	m_stTrig_Prescale;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Trigger12Dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Trigger12Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDeltaposSpPrescale(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRIGGER12DLG_H__00A37914_9C67_4A48_8FAE_BA56C7C28882__INCLUDED_)
