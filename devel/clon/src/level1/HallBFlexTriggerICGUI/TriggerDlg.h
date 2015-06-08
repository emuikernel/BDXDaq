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
	void UpdateControls();
	int m_iTriggerNum;
	void SetTriggerNum(int i);
	TriggerDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(TriggerDlg)
	enum { IDD = IDD_TRIGGER_DIALOG };
	CButton	m_bLoadTRIG;
	CButton	m_bLoadECP;
	CButton	m_bLoadECE;
	CButton	m_bLoadECC;
	CSpinButtonCtrl	m_spTrig_Prescale;
	CString	m_stTrig_Prescale;
	CString	m_stTrigOut;
	CString	m_stMousePos;
	CString	m_stTrig_Scaler;
	CString	m_stVet3_Scaler;
	CString	m_stVet2_Scaler;
	CString	m_stVet1_Scaler;
	CString	m_stTot3_Scaler;
	CString	m_stTot2_Scaler;
	CString	m_stTot1_Scaler;
	CString	m_stEle3_Scaler;
	CString	m_stEle2_Scaler;
	CString	m_stEle1_Scaler;
	CString	m_stECPLUT_Scaler;
	CString	m_stECELUT_Scaler;
	CString	m_stECCLUT_Scaler;
	CString	m_stIC_Dly;
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
	afx_msg void OnDeltaposSpPrescale(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBLoadEcc();
	afx_msg void OnBLoadEce();
	afx_msg void OnBLoadEcp();
	afx_msg void OnBLoadTrig();
	afx_msg void OnDeltaposSpIcDly(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL bLoad;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRIGGERDLG_H__15B6E2AA_6BA4_4A22_B631_17F5A8FF6093__INCLUDED_)
