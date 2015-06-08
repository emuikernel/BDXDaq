// HallBFlexTriggerGUIDlg.h : header file
//

#if !defined(AFX_HALLBFLEXTRIGGERGUIDLG_H__522984F9_4DF5_4D83_94AD_BDF4DC93375C__INCLUDED_)
#define AFX_HALLBFLEXTRIGGERGUIDLG_H__522984F9_4DF5_4D83_94AD_BDF4DC93375C__INCLUDED_

#include "ScopeDlg.h"	// Added by ClassView
#include "HallBFlexRegisters.h"	// Added by ClassView
#include "SectorDlg.h"	// Added by ClassView
#include "TriggerDlg.h"	// Added by ClassView
#include "DebugDlg.h"	// Added by ClassView
#include "V1495VMERemote.h"
#include "TestDialog.h"	// Added by ClassView
#include "Trigger12Dlg.h"	// Added by ClassView

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CHallBFlexTriggerGUIDlg dialog

class CHallBFlexTriggerGUIDlg : public CDialog
{
// Construction
public:
	void UpdateTabs();
	Trigger12Dlg m_dlgTrigger12;
	TestDialog m_dlgTest;
	unsigned int ReadScaler16(unsigned int addr);
	unsigned int ReadScaler32(unsigned int addr);
	void DebugMsg(CString str);
	ScopeDlg m_ScopeDlg;
	HallBFlexRegisters m_HallBFlexRegisters;
	CDialog *m_pCDialogs[20];
	DebugDlg m_dlgDebug;
	TriggerDlg m_dlgTriggers[11];
	SectorDlg m_dlgSectors[6];
	CHallBFlexTriggerGUIDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CHallBFlexTriggerGUIDlg)
	enum { IDD = IDD_HALLBFLEXTRIGGERGUI_DIALOG };
	CSliderCtrl	m_slScalerPeriod;
	CTabCtrl	m_tabTrigMain;
	CButton	m_chRemoteConnect;
	CString	m_stScalerPeriod;
	CString	m_stServerAddress;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHallBFlexTriggerGUIDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CHallBFlexTriggerGUIDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBConnect();
	afx_msg void OnBDisconnect();
	afx_msg void OnBLoadsettings();
	afx_msg void OnBSavesettings();
	afx_msg void OnBScope();
	afx_msg void OnSelchangeTabTrigmain(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomdrawSlScalerperiod(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBFirmware();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HALLBFLEXTRIGGERGUIDLG_H__522984F9_4DF5_4D83_94AD_BDF4DC93375C__INCLUDED_)
