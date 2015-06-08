// SectorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "HallBFlexTriggerGUI.h"
#include "SectorDlg.h"
#include "HallBFlexRegisters.h"
#include "HallBFlexTriggerGUIDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SectorDlg dialog


SectorDlg::SectorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SectorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(SectorDlg)
	m_stCC_Dly = _T("");
	m_stCC_Scaler = _T("");
	m_stECC_Mode = _T("");
	m_stECC_Scaler = _T("");
	m_stECE_Scaler = _T("");
	m_stECEtotE = _T("");
	m_stECinE = _T("");
	m_stECinE_Dly = _T("");
	m_stECinE_Scaler = _T("");
	m_stECinP = _T("");
	m_stECinP_Dly = _T("");
	m_stECinP_Scaler = _T("");
	m_stECP_Scaler = _T("");
	m_stECtotE = _T("");
	m_stECtotE_Dly = _T("");
	m_stECtotE_Scaler = _T("");
	m_stECtotP = _T("");
	m_stECtotP_Dly = _T("");
	m_stECtotP_Scaler = _T("");
	m_stTOF = _T("");
	m_stTOF_Dly = _T("");
	m_stTOF_Scaler = _T("");
	m_stECC = _T("");
	m_stECE = _T("");
	m_stECP = _T("");
	m_stCC = _T("");
	m_stMousePosSec = _T("");
	m_stTOF2 = _T("");
	m_stE_CC = _T("");
	m_stE_CC_Dly = _T("");
	m_stE_ECC = _T("");
	m_stE_ECinE = _T("");
	m_stE_ECinE_Dly = _T("");
	m_stE_ECtotE = _T("");
	m_stE_ECtotE_Dly = _T("");
	m_stE_ECC_Scaler = _T("");
	//}}AFX_DATA_INIT
}


void SectorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SectorDlg)
	DDX_Control(pDX, IDC_SP_E_ECTOTE_DLY, m_spinE_ECtotE_Dly);
	DDX_Control(pDX, IDC_SP_E_ECINE_DLY, m_spinE_ECinE_Dly);
	DDX_Control(pDX, IDC_SP_E_CC_DLY, m_spinE_CC_Dly);
	DDX_Control(pDX, IDC_SP_TOF_DLY, m_spinTOF_Dly);
	DDX_Control(pDX, IDC_SP_ECTOTP_DLY, m_spinECtotP_Dly);
	DDX_Control(pDX, IDC_SP_ECTOTE_DLY, m_spinECtotE_Dly);
	DDX_Control(pDX, IDC_SP_ECINP_DLY, m_spinECinP_Dly);
	DDX_Control(pDX, IDC_SP_ECINE_DLY, m_spinECinE_Dly);
	DDX_Control(pDX, IDC_SP_CC_DLY, m_spinCC_Dly);
	DDX_Text(pDX, IDC_ST_CC_DLY, m_stCC_Dly);
	DDX_Text(pDX, IDC_ST_CC_SCALER, m_stCC_Scaler);
	DDX_Text(pDX, IDC_ST_ECC_MODE, m_stECC_Mode);
	DDX_Text(pDX, IDC_ST_ECC_SCALER, m_stECC_Scaler);
	DDX_Text(pDX, IDC_ST_ECE_SCALER, m_stECE_Scaler);
	DDX_Text(pDX, IDC_ST_ECINE, m_stECinE);
	DDX_Text(pDX, IDC_ST_ECINE_DLY, m_stECinE_Dly);
	DDX_Text(pDX, IDC_ST_ECINE_SCALER, m_stECinE_Scaler);
	DDX_Text(pDX, IDC_ST_ECINP, m_stECinP);
	DDX_Text(pDX, IDC_ST_ECINP_DLY, m_stECinP_Dly);
	DDX_Text(pDX, IDC_ST_ECINP_SCALER, m_stECinP_Scaler);
	DDX_Text(pDX, IDC_ST_ECP_SCALER, m_stECP_Scaler);
	DDX_Text(pDX, IDC_ST_ECTOTE, m_stECtotE);
	DDX_Text(pDX, IDC_ST_ECTOTE_DLY, m_stECtotE_Dly);
	DDX_Text(pDX, IDC_ST_ECTOTE_SCALER, m_stECtotE_Scaler);
	DDX_Text(pDX, IDC_ST_ECTOTP, m_stECtotP);
	DDX_Text(pDX, IDC_ST_ECTOTP_DLY, m_stECtotP_Dly);
	DDX_Text(pDX, IDC_ST_ECTOTP_SCALER, m_stECtotP_Scaler);
	DDX_Text(pDX, IDC_ST_TOF, m_stTOF);
	DDX_Text(pDX, IDC_ST_TOF_DLY, m_stTOF_Dly);
	DDX_Text(pDX, IDC_ST_TOF_SCALER, m_stTOF_Scaler);
	DDX_Text(pDX, IDC_ST_ECC, m_stECC);
	DDX_Text(pDX, IDC_ST_ECE, m_stECE);
	DDX_Text(pDX, IDC_ST_ECP, m_stECP);
	DDX_Text(pDX, IDC_ST_CC, m_stCC);
	DDX_Text(pDX, IDC_ST_MOUSEPOS_SEC, m_stMousePosSec);
	DDX_Text(pDX, IDC_ST_TOF2, m_stTOF2);
	DDX_Text(pDX, IDC_ST_E_CC, m_stE_CC);
	DDX_Text(pDX, IDC_ST_E_CC_DLY, m_stE_CC_Dly);
	DDX_Text(pDX, IDC_ST_E_ECC, m_stE_ECC);
	DDX_Text(pDX, IDC_ST_E_ECINE, m_stE_ECinE);
	DDX_Text(pDX, IDC_ST_E_ECINE_DLY, m_stE_ECinE_Dly);
	DDX_Text(pDX, IDC_ST_E_ECTOTE, m_stE_ECtotE);
	DDX_Text(pDX, IDC_ST_E_ECTOTE_DLY, m_stE_ECtotE_Dly);
	DDX_Text(pDX, IDC_ST_E_ECC_SCALER, m_stE_ECC_Scaler);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SectorDlg, CDialog)
	//{{AFX_MSG_MAP(SectorDlg)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SP_CC_DLY, OnDeltaposSpCcDly)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SP_ECINE_DLY, OnDeltaposSpEcineDly)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SP_ECINP_DLY, OnDeltaposSpEcinpDly)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SP_ECTOTE_DLY, OnDeltaposSpEctoteDly)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SP_ECTOTP_DLY, OnDeltaposSpEctotpDly)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SP_TOF_DLY, OnDeltaposSpTofDly)
	ON_BN_CLICKED(IDC_ST_ECC_MODE, OnStEccMode)
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SP_E_CC_DLY, OnDeltaposSpECcDly)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SP_E_ECINE_DLY, OnDeltaposSpEEcineDly)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SP_E_ECTOTE_DLY, OnDeltaposSpEEctoteDly)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SectorDlg message handlers

void SectorDlg::OnDeltaposSpECcDly(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_flexRegs->IncE_CCDelay(m_iSectorNum, -((NM_UPDOWN*)pNMHDR)->iDelta);
	m_flexRegs->ReadBoardRegisters();
	UpdateControls(FALSE);
	*pResult = 0;
}

void SectorDlg::OnDeltaposSpEEcineDly(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_flexRegs->IncE_ECinEDelay(m_iSectorNum, -((NM_UPDOWN*)pNMHDR)->iDelta);
	m_flexRegs->ReadBoardRegisters();
	UpdateControls(FALSE);
	*pResult = 0;
}

void SectorDlg::OnDeltaposSpEEctoteDly(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_flexRegs->IncE_ECtotEDelay(m_iSectorNum, -((NM_UPDOWN*)pNMHDR)->iDelta);
	m_flexRegs->ReadBoardRegisters();
	UpdateControls(FALSE);
	*pResult = 0;
}

void SectorDlg::OnDeltaposSpCcDly(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_flexRegs->IncCCDelay(m_iSectorNum, -((NM_UPDOWN*)pNMHDR)->iDelta);
	m_flexRegs->ReadBoardRegisters();
	UpdateControls(FALSE);
	*pResult = 0;
}

void SectorDlg::OnDeltaposSpEcineDly(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_flexRegs->IncECinEDelay(m_iSectorNum, -((NM_UPDOWN*)pNMHDR)->iDelta);
	m_flexRegs->ReadBoardRegisters();
	UpdateControls(FALSE);
	*pResult = 0;
}

void SectorDlg::OnDeltaposSpEcinpDly(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_flexRegs->IncECinPDelay(m_iSectorNum, -((NM_UPDOWN*)pNMHDR)->iDelta);
	m_flexRegs->ReadBoardRegisters();
	UpdateControls(FALSE);
	*pResult = 0;
}

void SectorDlg::OnDeltaposSpEctoteDly(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_flexRegs->IncECtotEDelay(m_iSectorNum, -((NM_UPDOWN*)pNMHDR)->iDelta);
	m_flexRegs->ReadBoardRegisters();
	UpdateControls(FALSE);
	*pResult = 0;
}

void SectorDlg::OnDeltaposSpEctotpDly(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_flexRegs->IncECtotPDelay(m_iSectorNum, -((NM_UPDOWN*)pNMHDR)->iDelta);
	m_flexRegs->ReadBoardRegisters();
	UpdateControls(FALSE);
	*pResult = 0;
}

void SectorDlg::OnDeltaposSpTofDly(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_flexRegs->IncTOFDelay(m_iSectorNum, -((NM_UPDOWN*)pNMHDR)->iDelta);
	m_flexRegs->ReadBoardRegisters();
	UpdateControls(FALSE);
	*pResult = 0;
}

void SectorDlg::OnStEccMode() 
{
	m_flexRegs->IncECCMode(m_iSectorNum);
	UpdateControls(FALSE);
}

void SectorDlg::SetSectorNum(int i)
{
	m_iSectorNum = i;

	m_stE_ECC.Format("-> E_ECC%d", i+1);
	m_stE_ECinE.Format("ECinE%d ->", i+1);
	m_stE_ECtotE.Format("ECtotE%d ->", i+1);
	m_stE_CC.Format("CC%d ->", i+1);

	m_stECinE.Format("ECinE%d ->", i+1);
	m_stECinP.Format("ECinP%d ->", i+1);
	m_stECtotE.Format("ECtotE%d ->", i+1);
	m_stECtotP.Format("ECtotP%d ->", i+1);
	m_stTOF.Format("TOF%d ->", i+1);
	m_stTOF2.Format("-> TOF%d", i+1);
	m_stECC.Format("-> ECC%d", i+1);
	m_stECE.Format("-> ECE%d", i+1);
	m_stECP.Format("-> ECP%d", i+1);
	m_stCC.Format("CC%d ->", i+1);
	UpdateData(FALSE);
}

BOOL SectorDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_flexRegs = &(((CHallBFlexTriggerGUIDlg *)GetParent())->m_HallBFlexRegisters);
	SetSectorNum(0);
	UpdateControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SectorDlg::UpdateControls(BOOL bUpdateScalers)
{
	m_stE_CC_Dly.Format("Delay = %dns", m_flexRegs->m_iE_CC_Delay[m_iSectorNum] * CLOCK_PERIOD_NS);
	m_stE_ECinE_Dly.Format("Delay = %dns", m_flexRegs->m_iE_ECinE_Delay[m_iSectorNum] * CLOCK_PERIOD_NS);
	m_stE_ECtotE_Dly.Format("Delay = %dns", m_flexRegs->m_iE_ECtotE_Delay[m_iSectorNum] * CLOCK_PERIOD_NS);
	m_stCC_Dly.Format("Delay = %dns", m_flexRegs->m_iCC_Delay[m_iSectorNum] * CLOCK_PERIOD_NS);
	m_stECinE_Dly.Format("Delay = %dns", m_flexRegs->m_iECinE_Delay[m_iSectorNum] * CLOCK_PERIOD_NS);
	m_stECinP_Dly.Format("Delay = %dns", m_flexRegs->m_iECinP_Delay[m_iSectorNum] * CLOCK_PERIOD_NS);
	m_stECtotE_Dly.Format("Delay = %dns", m_flexRegs->m_iECtotE_Delay[m_iSectorNum] * CLOCK_PERIOD_NS);
	m_stECtotP_Dly.Format("Delay = %dns", m_flexRegs->m_iECtotP_Delay[m_iSectorNum] * CLOCK_PERIOD_NS);
	m_stTOF_Dly.Format("Delay = %dns", m_flexRegs->m_iTOF_Delay[m_iSectorNum] * CLOCK_PERIOD_NS);

	if(bUpdateScalers)
	{
		float elapsedTime;

		if(!m_flexRegs->m_iREF_Scaler)
			elapsedTime = 10e9;
		else
			elapsedTime = m_flexRegs->m_iREF_Scaler * 25.0f / 1000000000.0f;

		m_stE_ECC_Scaler.Format("Scaler = %d\nRate = %.3fHz", m_flexRegs->m_iESector_Scaler[m_iSectorNum], (float)m_flexRegs->m_iESector_Scaler[m_iSectorNum] / elapsedTime);
		m_stCC_Scaler.Format("Scaler = %d\nRate = %.3fHz", m_flexRegs->m_iCC_Scaler[m_iSectorNum], (float)m_flexRegs->m_iCC_Scaler[m_iSectorNum] / elapsedTime);
		m_stECtotE_Scaler.Format("Scaler = %d\nRate = %.3fHz", m_flexRegs->m_iECtotE_Scaler[m_iSectorNum], (float)m_flexRegs->m_iECtotE_Scaler[m_iSectorNum] / elapsedTime);
		m_stECinE_Scaler.Format("Scaler = %d\nRate = %.3fHz", m_flexRegs->m_iECinE_Scaler[m_iSectorNum], (float)m_flexRegs->m_iECinE_Scaler[m_iSectorNum] / elapsedTime);
		m_stECtotP_Scaler.Format("Scaler = %d\nRate = %.3fHz", m_flexRegs->m_iECtotP_Scaler[m_iSectorNum], (float)m_flexRegs->m_iECtotP_Scaler[m_iSectorNum] / elapsedTime);
		m_stECinP_Scaler.Format("Scaler = %d\nRate = %.3fHz", m_flexRegs->m_iECinP_Scaler[m_iSectorNum], (float)m_flexRegs->m_iECinP_Scaler[m_iSectorNum] / elapsedTime);
		m_stTOF_Scaler.Format("Scaler = %d\nRate = %.3fHz", m_flexRegs->m_iTOF_Scaler[m_iSectorNum], (float)m_flexRegs->m_iTOF_Scaler[m_iSectorNum] / elapsedTime);
		m_stECE_Scaler.Format("Scaler = %d\nRate = %.3fHz", m_flexRegs->m_iECE_Scaler[m_iSectorNum], (float)m_flexRegs->m_iECE_Scaler[m_iSectorNum] / elapsedTime);
		m_stECP_Scaler.Format("Scaler = %d\nRate = %.3fHz", m_flexRegs->m_iECP_Scaler[m_iSectorNum], (float)m_flexRegs->m_iECP_Scaler[m_iSectorNum] / elapsedTime);
		m_stECC_Scaler.Format("Scaler = %d\nRate = %.3fHz", m_flexRegs->m_iECC_Scaler[m_iSectorNum], (float)m_flexRegs->m_iECC_Scaler[m_iSectorNum] / elapsedTime);
	}

	switch(m_flexRegs->m_iECC_Mode[m_iSectorNum])
	{
		case ECC_AND:	m_stECC_Mode.Format("and"); break;
		case ECC_ECE:	m_stECC_Mode.Format("ECE"); break;
		case ECC_CC:	m_stECC_Mode.Format("CC"); break;
		case ECC_HI:	m_stECC_Mode.Format("1"); break;
		case ECC_LO:	m_stECC_Mode.Format("0"); break;
	}
	UpdateData(FALSE);
}


void SectorDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	m_stMousePosSec.Format("X=%d,Y=%d", point.x, point.y);
	UpdateData(FALSE);
	
	CDialog::OnMouseMove(nFlags, point);
}

void SectorDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting	
	CPen pen;

	pen.CreatePen(PS_SOLID, 3, RGB(0,0,0));
	dc.SelectObject(&pen);
	dc.MoveTo(277, 169); dc.LineTo(507, 169);

	dc.MoveTo(314, 220); dc.LineTo(397, 220);

	dc.MoveTo(314, 286); dc.LineTo(397, 286);// dc.LineTo(450, 313); dc.LineTo(340, 313); dc.LineTo(340, 333); dc.LineTo(362, 333);
	dc.MoveTo(340, 286); dc.LineTo(340, 333); dc.LineTo(360, 333);
	dc.MoveTo(362, 345); dc.LineTo(277, 345);
	dc.MoveTo(207, 78); dc.LineTo(385, 78);
}

