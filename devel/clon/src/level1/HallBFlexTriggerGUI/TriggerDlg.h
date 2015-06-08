#if !defined(AFX_TRIGGERDLG_H__15B6E2AA_6BA4_4A22_B631_17F5A8FF6093__INCLUDED_)
#define AFX_TRIGGERDLG_H__15B6E2AA_6BA4_4A22_B631_17F5A8FF6093__INCLUDED_

#include "HallBFlexRegisters.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TriggerDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// TriggerDlg dialog

class TriggerDlg : public CDialog
{
// Construction
public:
	HallBFlexRegisters *m_flexRegs;
	void UpdateControls();
	int m_iTriggerNum;
	void SetTriggerNum(int i);
	TriggerDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(TriggerDlg)
	enum { IDD = IDD_TRIGGER_DIALOG };
	CSpinButtonCtrl	m_spTrig_Prescale;
	CSpinButtonCtrl	m_spTrig_Persist;
	CSpinButtonCtrl	m_spSTMult_Min;
	CSpinButtonCtrl	m_spSTMult_Max;
	CSpinButtonCtrl	m_spMORB_Dly;
	CSpinButtonCtrl	m_spMORA_Dly;
	CString	m_stLUTMOR_Scaler;
	CString	m_stLUTMORST_Scaler;
	CString	m_stLUT_Scaler;
	CString	m_stMOR_Mode;
	CString	m_stMORA_Dly;
	CString	m_stMORB_Dly;
	CString	m_stMult_MinMax;
	CString	m_stTrig_Persist;
	CString	m_stTrigPersist_Scaler;
	CString	m_stTrig_Prescale;
	CString	m_stTrigPrescale_Scaler;
	CString	m_stSTMult_Scaler;
	CString	m_stTrigOut;
	CString	m_stMORA_Scaler;
	CString	m_stMORB_Scaler;
	CString	m_stMOROr_Scaler;
	CString	m_stMousePos;
	CString	m_csECCLutFile;
	CString	m_csECPLutFile;
	CString	m_stSTMult_Mode;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TriggerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(TriggerDlg)
	afx_msg void OnBLoadStofEcc();
	afx_msg void OnBLoadStofEcp();
	afx_msg void OnDeltaposSpMoraDly(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpMorbDly(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpMultmax(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpMultmin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpPersist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpPrescale(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnStMorMode();
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnStStmultMode();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRIGGERDLG_H__15B6E2AA_6BA4_4A22_B631_17F5A8FF6093__INCLUDED_)
