// Trigger12Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "HallBFlexTriggerGUI.h"
#include "HallBFlexTriggerGUIDlg.h"
#include "HallBFlexRegisters.h"
#include "Trigger12Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Trigger12Dlg dialog


Trigger12Dlg::Trigger12Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(Trigger12Dlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(Trigger12Dlg)
	m_stTrig_Scaler = _T("");
	m_stTrig_Prescale = _T("");
	//}}AFX_DATA_INIT
}


void Trigger12Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Trigger12Dlg)
	DDX_Control(pDX, IDC_SP_PRESCALE, m_spTrig_Prescale);
	DDX_Text(pDX, IDC_ST_TRIGGER_SCALER, m_stTrig_Scaler);
	DDX_Text(pDX, IDC_ST_PRESCALE, m_stTrig_Prescale);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Trigger12Dlg, CDialog)
	//{{AFX_MSG_MAP(Trigger12Dlg)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SP_PRESCALE, OnDeltaposSpPrescale)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Trigger12Dlg message handlers

void Trigger12Dlg::UpdateControls()
{
	HallBFlexRegisters *flexRegs = &(((CHallBFlexTriggerGUIDlg *)GetParent())->m_HallBFlexRegisters);

	float elapsedTime;

	if(!flexRegs->m_iREF_Scaler)
		elapsedTime = 10e9;
	else
		elapsedTime = flexRegs->m_iREF_Scaler * 25.0f / 1000000000.0f;

	m_stTrig_Scaler.Format("Scaler = %d\nRate = %.3fHz", flexRegs->m_iTRIG_Scaler[11], (float)flexRegs->m_iTRIG_Scaler[11] / elapsedTime);
	m_stTrig_Prescale.Format("Prescale = %d", flexRegs->m_iTrig_Prescale[11]);

	UpdateData(FALSE);
}

BOOL Trigger12Dlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	UpdateControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void Trigger12Dlg::OnDeltaposSpPrescale(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HallBFlexRegisters *flexRegs = &(((CHallBFlexTriggerGUIDlg *)GetParent())->m_HallBFlexRegisters);
	flexRegs->IncPrescale(11, -((NM_UPDOWN*)pNMHDR)->iDelta);
	flexRegs->ReadBoardRegisters();
	UpdateControls();
	*pResult = 0;
}
