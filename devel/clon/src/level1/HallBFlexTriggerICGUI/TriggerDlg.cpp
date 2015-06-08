// TriggerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "HallBFlexTriggerGUI.h"
#include "TriggerDlg.h"
#include "HallBFlexRegisters.h"
#include "HallBFlexTriggerGUIDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// TriggerDlg dialog


TriggerDlg::TriggerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(TriggerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(TriggerDlg)
	m_stTrig_Prescale = _T("");
	m_stTrigOut = _T("");
	m_stMousePos = _T("");
	m_stTrig_Scaler = _T("");
	m_stVet3_Scaler = _T("");
	m_stVet2_Scaler = _T("");
	m_stVet1_Scaler = _T("");
	m_stTot3_Scaler = _T("");
	m_stTot2_Scaler = _T("");
	m_stTot1_Scaler = _T("");
	m_stEle3_Scaler = _T("");
	m_stEle2_Scaler = _T("");
	m_stEle1_Scaler = _T("");
	m_stECPLUT_Scaler = _T("");
	m_stECELUT_Scaler = _T("");
	m_stECCLUT_Scaler = _T("");
	m_stIC_Dly = _T("");
	//}}AFX_DATA_INIT
}


void TriggerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TriggerDlg)
	DDX_Control(pDX, IDC_B_LOAD_TRIG, m_bLoadTRIG);
	DDX_Control(pDX, IDC_B_LOAD_ECP, m_bLoadECP);
	DDX_Control(pDX, IDC_B_LOAD_ECE, m_bLoadECE);
	DDX_Control(pDX, IDC_B_LOAD_ECC, m_bLoadECC);
	DDX_Control(pDX, IDC_SP_PRESCALE, m_spTrig_Prescale);
	DDX_Text(pDX, IDC_ST_PRESCALE, m_stTrig_Prescale);
	DDX_Text(pDX, IDC_ST_TRIGOUT, m_stTrigOut);
	DDX_Text(pDX, IDC_ST_MOUSEPOS, m_stMousePos);
	DDX_Text(pDX, IDC_ST_TRIGGER_SCALER, m_stTrig_Scaler);
	DDX_Text(pDX, IDC_IC_VET3_SCALER, m_stVet3_Scaler);
	DDX_Text(pDX, IDC_IC_VET2_SCALER, m_stVet2_Scaler);
	DDX_Text(pDX, IDC_IC_VET1_SCALER, m_stVet1_Scaler);
	DDX_Text(pDX, IDC_IC_TOT3_SCALER, m_stTot3_Scaler);
	DDX_Text(pDX, IDC_IC_TOT2_SCALER, m_stTot2_Scaler);
	DDX_Text(pDX, IDC_IC_TOT1_SCALER, m_stTot1_Scaler);
	DDX_Text(pDX, IDC_IC_ELE3_SCALER, m_stEle3_Scaler);
	DDX_Text(pDX, IDC_IC_ELE2_SCALER, m_stEle2_Scaler);
	DDX_Text(pDX, IDC_IC_ELE1_SCALER, m_stEle1_Scaler);
	DDX_Text(pDX, IDC_ECP_LUT_SCALER, m_stECPLUT_Scaler);
	DDX_Text(pDX, IDC_ECE_LUT_SCALER, m_stECELUT_Scaler);
	DDX_Text(pDX, IDC_ECC_LUT_SCALER, m_stECCLUT_Scaler);
	DDX_Text(pDX, IDC_ST_IC_DLY, m_stIC_Dly);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(TriggerDlg, CDialog)
	//{{AFX_MSG_MAP(TriggerDlg)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SP_PRESCALE, OnDeltaposSpPrescale)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_B_LOAD_ECC, OnBLoadEcc)
	ON_BN_CLICKED(IDC_B_LOAD_ECE, OnBLoadEce)
	ON_BN_CLICKED(IDC_B_LOAD_ECP, OnBLoadEcp)
	ON_BN_CLICKED(IDC_B_LOAD_TRIG, OnBLoadTrig)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SP_IC_DLY, OnDeltaposSpIcDly)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TriggerDlg message handlers

void TriggerDlg::OnDeltaposSpPrescale(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HallBFlexRegisters *flexRegs = &(((CHallBFlexTriggerGUIDlg *)GetParent())->m_HallBFlexRegisters);
	flexRegs->IncPrescale(m_iTriggerNum, -((NM_UPDOWN*)pNMHDR)->iDelta);
	flexRegs->ReadBoardRegisters();
	UpdateControls();
	*pResult = 0;
}

void TriggerDlg::SetTriggerNum(int i)
{
	m_iTriggerNum = i;

	m_stTrigOut.Format("-> TRIG%d", i+1);
	UpdateData(FALSE);
}

BOOL TriggerDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	bLoad = TRUE;
	SetTriggerNum(1);
	UpdateControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void TriggerDlg::UpdateControls()
{
	HallBFlexRegisters *flexRegs = &(((CHallBFlexTriggerGUIDlg *)GetParent())->m_HallBFlexRegisters);

	float elapsedTime;

	if(!flexRegs->m_iREF_Scaler)
		elapsedTime = 10e9;
	else
		elapsedTime = flexRegs->m_iREF_Scaler * 25.0f / 1000000000.0f;

	m_stTrig_Prescale.Format("Prescale = %d", flexRegs->m_iTrig_Prescale[m_iTriggerNum]);
	m_stTrig_Scaler.Format("Scaler = %d\nRate = %.3fHz", flexRegs->m_iTRIG_Scaler[m_iTriggerNum], (float)flexRegs->m_iTRIG_Scaler[m_iTriggerNum] / elapsedTime);
	m_stVet1_Scaler.Format("Scaler = %d\nRate = %.3fHz", flexRegs->m_iICVeto_Scaler[0], (float)flexRegs->m_iICVeto_Scaler[0] / elapsedTime);
	m_stVet2_Scaler.Format("Scaler = %d\nRate = %.3fHz", flexRegs->m_iICVeto_Scaler[1], (float)flexRegs->m_iICVeto_Scaler[1] / elapsedTime);
	m_stVet3_Scaler.Format("Scaler = %d\nRate = %.3fHz", flexRegs->m_iICVeto_Scaler[2], (float)flexRegs->m_iICVeto_Scaler[2] / elapsedTime);
	m_stTot1_Scaler.Format("Scaler = %d\nRate = %.3fHz", flexRegs->m_iICTotal_Scaler[0], (float)flexRegs->m_iICTotal_Scaler[0] / elapsedTime);
	m_stTot2_Scaler.Format("Scaler = %d\nRate = %.3fHz", flexRegs->m_iICTotal_Scaler[1], (float)flexRegs->m_iICTotal_Scaler[1] / elapsedTime);
	m_stTot3_Scaler.Format("Scaler = %d\nRate = %.3fHz", flexRegs->m_iICTotal_Scaler[2], (float)flexRegs->m_iICTotal_Scaler[2] / elapsedTime);
	m_stEle1_Scaler.Format("Scaler = %d\nRate = %.3fHz", flexRegs->m_iICElectron_Scaler[0], (float)flexRegs->m_iICElectron_Scaler[0] / elapsedTime);
	m_stEle2_Scaler.Format("Scaler = %d\nRate = %.3fHz", flexRegs->m_iICElectron_Scaler[1], (float)flexRegs->m_iICElectron_Scaler[1] / elapsedTime);
	m_stEle3_Scaler.Format("Scaler = %d\nRate = %.3fHz", flexRegs->m_iICElectron_Scaler[2], (float)flexRegs->m_iICElectron_Scaler[2] / elapsedTime);
	m_stECPLUT_Scaler.Format("Scaler = %d\nRate = %.3fHz", flexRegs->m_iECPLUT_Scaler[m_iTriggerNum], (float)flexRegs->m_iECPLUT_Scaler[m_iTriggerNum] / elapsedTime);
	m_stECELUT_Scaler.Format("Scaler = %d\nRate = %.3fHz", flexRegs->m_iECELUT_Scaler[m_iTriggerNum], (float)flexRegs->m_iECELUT_Scaler[m_iTriggerNum] / elapsedTime);
	m_stECCLUT_Scaler.Format("Scaler = %d\nRate = %.3fHz", flexRegs->m_iECCLUT_Scaler[m_iTriggerNum], (float)flexRegs->m_iECCLUT_Scaler[m_iTriggerNum] / elapsedTime);
	m_stIC_Dly.Format("Delay = %dns", flexRegs->m_iIC_Delay);
	UpdateData(FALSE);

	bLoad = FALSE;
	OnBLoadEcc();
	OnBLoadEce();
	OnBLoadEcp();
	OnBLoadTrig();
	bLoad = TRUE;
}

void TriggerDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CPen pen;

	pen.CreatePen(PS_SOLID, 3, RGB(0,0,0));
	dc.SelectObject(&pen);
/*	dc.MoveTo(162, 123); dc.LineTo(250, 123); dc.LineTo(250, 80); dc.LineTo(344, 80);
	dc.MoveTo(492, 71); dc.LineTo(510, 71); dc.LineTo(510, 114); dc.LineTo(315, 114); dc.LineTo(315, 140); dc.LineTo(344, 140);
	dc.MoveTo(104, 171); dc.LineTo(344, 171);
	dc.MoveTo(492, 156); dc.LineTo(510, 156); dc.LineTo(510, 197); dc.LineTo(50, 197); dc.LineTo(50, 225); dc.LineTo(65, 225);*/

	UpdateControls();
}

void TriggerDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	m_stMousePos.Format("X=%d,Y=%d", point.x, point.y);
	UpdateData(FALSE);
	CDialog::OnMouseMove(nFlags, point);
}

void TriggerDlg::OnBLoadEcc()
{
	HallBFlexRegisters *flexRegs = &(((CHallBFlexTriggerGUIDlg *)GetParent())->m_HallBFlexRegisters);

	if(bLoad)
		flexRegs->LoadLUT(m_iTriggerNum, LUT_ECC);
	
	CString str = "ECC LUT(6:1)\n FILE: " + flexRegs->m_stECCLUTNames[m_iTriggerNum];
	SetDlgItemText(IDC_B_LOAD_ECC, str);
}

void TriggerDlg::OnBLoadEce() 
{
	HallBFlexRegisters *flexRegs = &(((CHallBFlexTriggerGUIDlg *)GetParent())->m_HallBFlexRegisters);
	
	if(bLoad)
		flexRegs->LoadLUT(m_iTriggerNum, LUT_ECE);
	
	CString str = "ECE LUT(12:1)\n FILE: " + flexRegs->m_stECELUTNames[m_iTriggerNum];
	SetDlgItemText(IDC_B_LOAD_ECE, str);
}

void TriggerDlg::OnBLoadEcp() 
{
	HallBFlexRegisters *flexRegs = &(((CHallBFlexTriggerGUIDlg *)GetParent())->m_HallBFlexRegisters);

	if(bLoad)
		flexRegs->LoadLUT(m_iTriggerNum, LUT_ECP);
	
	CString str = "ECP LUT(12:1)\n FILE: " + flexRegs->m_stECPLUTNames[m_iTriggerNum];
	SetDlgItemText(IDC_B_LOAD_ECP, str);
}

void TriggerDlg::OnBLoadTrig() 
{
	HallBFlexRegisters *flexRegs = &(((CHallBFlexTriggerGUIDlg *)GetParent())->m_HallBFlexRegisters);

	if(bLoad)
		flexRegs->LoadLUT(m_iTriggerNum, LUT_TRIG);
	
	CString str = "TRIG LUT(12:1)\n FILE: " + flexRegs->m_stTRIGLUTNames[m_iTriggerNum];
	SetDlgItemText(IDC_B_LOAD_TRIG, str);
}

void TriggerDlg::OnDeltaposSpIcDly(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HallBFlexRegisters *flexRegs = &(((CHallBFlexTriggerGUIDlg *)GetParent())->m_HallBFlexRegisters);
	flexRegs->IncICDelay(-((NM_UPDOWN*)pNMHDR)->iDelta);
	flexRegs->ReadBoardRegisters();
	UpdateControls();
	*pResult = 0;
}
