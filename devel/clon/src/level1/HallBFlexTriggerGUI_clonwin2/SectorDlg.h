#if !defined(AFX_SECTORDLG_H__F7917945_910B_490B_9E0A_EB6FFD86ED1E__INCLUDED_)
#define AFX_SECTORDLG_H__F7917945_910B_490B_9E0A_EB6FFD86ED1E__INCLUDED_

#include "HallBFlexRegisters.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SectorDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SectorDlg dialog

class SectorDlg : public CDialog
{
// Construction
public:
	HallBFlexRegisters *m_flexRegs;
	int m_iSectorNum;
	void UpdateControls();
	void SetSectorNum(int i);
	SectorDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(SectorDlg)
	enum { IDD = IDD_SECTOR_DIALOG };
	CSpinButtonCtrl	m_spinTOF_Dly;
	CSpinButtonCtrl	m_spinST_Dly4;
	CSpinButtonCtrl	m_spinST_Dly3;
	CSpinButtonCtrl	m_spinST_Dly2;
	CSpinButtonCtrl	m_spinST_Dly1;
	CSpinButtonCtrl	m_spinECtotP_Dly;
	CSpinButtonCtrl	m_spinECtotE_Dly;
	CSpinButtonCtrl	m_spinECinP_Dly;
	CSpinButtonCtrl	m_spinECinE_Dly;
	CSpinButtonCtrl	m_spinCC_Dly;
	CString	m_stCC_Dly;
	CString	m_stCC_Scaler;
	CString	m_stECC_Mode;
	CString	m_stECC_Scaler;
	CString	m_stECE_Scaler;
	CString	m_stECEtotE;
	CString	m_stECinE;
	CString	m_stECinE_Dly;
	CString	m_stECinE_Scaler;
	CString	m_stECinP;
	CString	m_stECinP_Dly;
	CString	m_stECinP_Scaler;
	CString	m_stECP_Scaler;
	CString	m_stECtotE;
	CString	m_stECtotE_Dly;
	CString	m_stECtotE_Scaler;
	CString	m_stECtotP;
	CString	m_stECtotP_Dly;
	CString	m_stECtotP_Scaler;
	CString	m_stST_Dly1;
	CString	m_stST_Dly2;
	CString	m_stST_Dly3;
	CString	m_stST_Dly4;
	CString	m_stST_Scaler1;
	CString	m_stST_Scaler2;
	CString	m_stST_Scaler3;
	CString	m_stST_Scaler4;
	CString	m_stST1;
	CString	m_stST2;
	CString	m_stST3;
	CString	m_stST4;
	CString	m_stSTOF_Mode;
	CString	m_stSTOF_Scaler;
	CString	m_stSTS_Scaler;
	CString	m_stTOF;
	CString	m_stTOF_Dly;
	CString	m_stTOF_Scaler;
	CString	m_stECC;
	CString	m_stECE;
	CString	m_stECP;
	CString	m_stSTOF;
	CString	m_stSTS;
	CString	m_stCC;
	CString	m_stMousePosSec;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SectorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SectorDlg)
	afx_msg void OnDeltaposSpCcDly(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpEcineDly(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpEcinpDly(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpEctoteDly(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpEctotpDly(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpStDly1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpStDly2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpStDly3(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpStDly4(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpTofDly(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnStEccMode();
	afx_msg void OnStStofMode();
	virtual BOOL OnInitDialog();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SECTORDLG_H__F7917945_910B_490B_9E0A_EB6FFD86ED1E__INCLUDED_)
