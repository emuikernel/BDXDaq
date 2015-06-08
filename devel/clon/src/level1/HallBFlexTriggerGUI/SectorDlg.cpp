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
	m_stST_Dly1 = _T("");
	m_stST_Dly2 = _T("");
	m_stST_Dly3 = _T("");
	m_stST_Dly4 = _T("");
	m_stST_Scaler1 = _T("");
	m_stST_Scaler2 = _T("");
	m_stST_Scaler3 = _T("");
	m_stST_Scaler4 = _T("");
	m_stST1 = _T("");
	m_stST2 = _T("");
	m_stST3 = _T("");
	m_stST4 = _T("");
	m_stSTOF_Mode = _T("");
	m_stSTOF_Scaler = _T("");
	m_stSTS_Scaler = _T("");
	m_stTOF = _T("");
	m_stTOF_Dly = _T("");
	m_stTOF_Scaler = _T("");
	m_stECC = _T("");
	m_stECE = _T("");
	m_stECP = _T("");
	m_stSTOF = _T("");
	m_stSTS = _T("");
	m_stCC = _T("");
	m_stMousePosSec = _T("");
	//}}AFX_DATA_INIT
}


void SectorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SectorDlg)
	DDX_Control(pDX, IDC_SP_TOF_DLY, m_spinTOF_Dly);
	DDX_Control(pDX, IDC_SP_ST_DLY4, m_spinST_Dly4);
	DDX_Control(pDX, IDC_SP_ST_DLY3, m_spinST_Dly3);
	DDX_Control(pDX, IDC_SP_ST_DLY2, m_spinST_Dly2);
	DDX_Control(pDX, IDC_SP_ST_DLY1, m_spinST_Dly1);
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
	DDX_Text(pDX, IDC_ST_ST_DLY1, m_stST_Dly1);
	DDX_Text(pDX, IDC_ST_ST_DLY2, m_stST_Dly2);
	DDX_Text(pDX, IDC_ST_ST_DLY3, m_stST_Dly3);
	DDX_Text(pDX, IDC_ST_ST_DLY4, m_stST_Dly4);
	DDX_Text(pDX, IDC_ST_ST_SCALER1, m_stST_Scaler1);
	DDX_Text(pDX, IDC_ST_ST_SCALER2, m_stST_Scaler2);
	DDX_Text(pDX, IDC_ST_ST_SCALER3, m_stST_Scaler3);
	DDX_Text(pDX, IDC_ST_ST_SCALER4, m_stST_Scaler4);
	DDX_Text(pDX, IDC_ST_ST1, m_stST1);
	DDX_Text(pDX, IDC_ST_ST2, m_stST2);
	DDX_Text(pDX, IDC_ST_ST3, m_stST3);
	DDX_Text(pDX, IDC_ST_ST4, m_stST4);
	DDX_Text(pDX, IDC_ST_STOF_MODE, m_stSTOF_Mode);
	DDX_Text(pDX, IDC_ST_STOF_SCALER, m_stSTOF_Scaler);
	DDX_Text(pDX, IDC_ST_STS_SCALER, m_stSTS_Scaler);
	DDX_Text(pDX, IDC_ST_TOF, m_stTOF);
	DDX_Text(pDX, IDC_ST_TOF_DLY, m_stTOF_Dly);
	DDX_Text(pDX, IDC_ST_TOF_SCALER, m_stTOF_Scaler);
	DDX_Text(pDX, IDC_ST_ECC, m_stECC);
	DDX_Text(pDX, IDC_ST_ECE, m_stECE);
	DDX_Text(pDX, IDC_ST_ECP, m_stECP);
	DDX_Text(pDX, IDC_ST_STOF, m_stSTOF);
	DDX_Text(pDX, IDC_ST_STS, m_stSTS);
	DDX_Text(pDX, IDC_ST_CC, m_stCC);
	DDX_Text(pDX, IDC_ST_MOUSEPOS_SEC, m_stMousePosSec);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SectorDlg, CDialog)
	//{{AFX_MSG_MAP(SectorDlg)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SP_CC_DLY, OnDeltaposSpCcDly)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SP_ECINE_DLY, OnDeltaposSpEcineDly)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SP_ECINP_DLY, OnDeltaposSpEcinpDly)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SP_ECTOTE_DLY, OnDeltaposSpEctoteDly)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SP_ECTOTP_DLY, OnDeltaposSpEctotpDly)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SP_ST_DLY1, OnDeltaposSpStDly1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SP_ST_DLY2, OnDeltaposSpStDly2)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SP_ST_DLY3, OnDeltaposSpStDly3)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SP_ST_DLY4, OnDeltaposSpStDly4)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SP_TOF_DLY, OnDeltaposSpTofDly)
	ON_BN_CLICKED(IDC_ST_ECC_MODE, OnStEccMode)
	ON_BN_CLICKED(IDC_ST_STOF_MODE, OnStStofMode)
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SectorDlg message handlers

void SectorDlg::OnDeltaposSpCcDly(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_flexRegs->IncCCDelay(m_iSectorNum, -((NM_UPDOWN*)pNMHDR)->iDelta);
	UpdateControls();
	*pResult = 0;
}

void SectorDlg::OnDeltaposSpEcineDly(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_flexRegs->IncECinEDelay(m_iSectorNum, -((NM_UPDOWN*)pNMHDR)->iDelta);
	UpdateControls();
	*pResult = 0;
}

void SectorDlg::OnDeltaposSpEcinpDly(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_flexRegs->IncECinPDelay(m_iSectorNum, -((NM_UPDOWN*)pNMHDR)->iDelta);
	UpdateControls();
	*pResult = 0;
}

void SectorDlg::OnDeltaposSpEctoteDly(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_flexRegs->IncECtotEDelay(m_iSectorNum, -((NM_UPDOWN*)pNMHDR)->iDelta);
	UpdateControls();
	*pResult = 0;
}

void SectorDlg::OnDeltaposSpEctotpDly(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_flexRegs->IncECtotPDelay(m_iSectorNum, -((NM_UPDOWN*)pNMHDR)->iDelta);
	UpdateControls();
	*pResult = 0;
}

void SectorDlg::OnDeltaposSpStDly1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_flexRegs->IncSTDelay(m_iSectorNum, 0, -((NM_UPDOWN*)pNMHDR)->iDelta);
	UpdateControls();
	*pResult = 0;
}

void SectorDlg::OnDeltaposSpStDly2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_flexRegs->IncSTDelay(m_iSectorNum, 1, -((NM_UPDOWN*)pNMHDR)->iDelta);
	UpdateControls();
	*pResult = 0;
}

void SectorDlg::OnDeltaposSpStDly3(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_flexRegs->IncSTDelay(m_iSectorNum, 2, -((NM_UPDOWN*)pNMHDR)->iDelta);
	UpdateControls();
	*pResult = 0;
}

void SectorDlg::OnDeltaposSpStDly4(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_flexRegs->IncSTDelay(m_iSectorNum, 3, -((NM_UPDOWN*)pNMHDR)->iDelta);
	UpdateControls();
	*pResult = 0;
}

void SectorDlg::OnDeltaposSpTofDly(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_flexRegs->IncTOFDelay(m_iSectorNum, -((NM_UPDOWN*)pNMHDR)->iDelta);
	UpdateControls();
	*pResult = 0;
}

void SectorDlg::OnStEccMode() 
{
	m_flexRegs->IncECCMode(m_iSectorNum);
	UpdateControls();
}

void SectorDlg::OnStStofMode()
{
	m_flexRegs->IncSTOFMode(m_iSectorNum);
	UpdateControls();
}

void SectorDlg::SetSectorNum(int i)
{
	m_iSectorNum = i;

	m_stST1.Format("ST%d ->", 1+i*4);
	m_stST2.Format("ST%d ->", 2+i*4);
	m_stST3.Format("ST%d ->", 3+i*4);
	m_stST4.Format("ST%d ->", 4+i*4);
	m_stECinE.Format("ECin%dE ->", i+1);
	m_stECinP.Format("ECin%dP ->", i+1);
	m_stECtotE.Format("ECtot%dE ->", i+1);
	m_stECtotP.Format("ECtot%dP ->", i+1);
	m_stTOF.Format("TOF%d ->", i+1);
	m_stECC.Format("ECC%d ->", i+1);
	m_stECE.Format("EC%dE ->", i+1);
	m_stECP.Format("EC%dP ->", i+1);
	m_stSTOF.Format("STOF%d ->", i+1);
	m_stSTS.Format("STS%d ->", i+1);
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

void SectorDlg::UpdateControls()
{
	m_stCC_Dly.Format("Delay = %dns", m_flexRegs->m_iCC_Delay[m_iSectorNum] * CLOCK_PERIOD_NS);
	m_stECinE_Dly.Format("Delay = %dns", m_flexRegs->m_iECinE_Delay[m_iSectorNum] * CLOCK_PERIOD_NS);
	m_stECinP_Dly.Format("Delay = %dns", m_flexRegs->m_iECinP_Delay[m_iSectorNum] * CLOCK_PERIOD_NS);
	m_stECtotE_Dly.Format("Delay = %dns", m_flexRegs->m_iECtotE_Delay[m_iSectorNum] * CLOCK_PERIOD_NS);
	m_stECtotP_Dly.Format("Delay = %dns", m_flexRegs->m_iECtotP_Delay[m_iSectorNum] * CLOCK_PERIOD_NS);
	m_stST_Dly1.Format("Delay = %dns", m_flexRegs->m_iST_Delay[m_iSectorNum*4] * CLOCK_PERIOD_NS);
	m_stST_Dly2.Format("Delay = %dns", m_flexRegs->m_iST_Delay[m_iSectorNum*4+1] * CLOCK_PERIOD_NS);
	m_stST_Dly3.Format("Delay = %dns", m_flexRegs->m_iST_Delay[m_iSectorNum*4+2] * CLOCK_PERIOD_NS);
	m_stST_Dly4.Format("Delay = %dns", m_flexRegs->m_iST_Delay[m_iSectorNum*4+3] * CLOCK_PERIOD_NS);
	m_stTOF_Dly.Format("Delay = %dns", m_flexRegs->m_iTOF_Delay[m_iSectorNum] * CLOCK_PERIOD_NS);

	float elapsedTime;

	if(!m_flexRegs->m_iREF_Scaler)
		elapsedTime = 10e9;
	else
		elapsedTime = m_flexRegs->m_iREF_Scaler * 25.0f / 1000000000.0f;

	m_stCC_Scaler.Format("Scaler = %d\nRate = %.3fHz", m_flexRegs->m_iCC_Scaler[m_iSectorNum], (float)m_flexRegs->m_iCC_Scaler[m_iSectorNum] / elapsedTime);
	m_stECtotE_Scaler.Format("Scaler = %d\nRate = %.3fHz", m_flexRegs->m_iECtotE_Scaler[m_iSectorNum], (float)m_flexRegs->m_iECtotE_Scaler[m_iSectorNum] / elapsedTime);
	m_stECinE_Scaler.Format("Scaler = %d\nRate = %.3fHz", m_flexRegs->m_iECinE_Scaler[m_iSectorNum], (float)m_flexRegs->m_iECinE_Scaler[m_iSectorNum] / elapsedTime);
	m_stECtotP_Scaler.Format("Scaler = %d\nRate = %.3fHz", m_flexRegs->m_iECtotP_Scaler[m_iSectorNum], (float)m_flexRegs->m_iECtotP_Scaler[m_iSectorNum] / elapsedTime);
	m_stECinP_Scaler.Format("Scaler = %d\nRate = %.3fHz", m_flexRegs->m_iECinP_Scaler[m_iSectorNum], (float)m_flexRegs->m_iECinP_Scaler[m_iSectorNum] / elapsedTime);
	m_stTOF_Scaler.Format("Scaler = %d\nRate = %.3fHz", m_flexRegs->m_iTOF_Scaler[m_iSectorNum], (float)m_flexRegs->m_iTOF_Scaler[m_iSectorNum] / elapsedTime);
	m_stST_Scaler1.Format("Scaler = %d\nRate = %.3fHz", m_flexRegs->m_iST_Scaler[m_iSectorNum*4], (float)m_flexRegs->m_iST_Scaler[m_iSectorNum*4] / elapsedTime);
	m_stST_Scaler2.Format("Scaler = %d\nRate = %.3fHz", m_flexRegs->m_iST_Scaler[m_iSectorNum*4+1], (float)m_flexRegs->m_iST_Scaler[m_iSectorNum*4+1] / elapsedTime);
	m_stST_Scaler3.Format("Scaler = %d\nRate = %.3fHz", m_flexRegs->m_iST_Scaler[m_iSectorNum*4+2], (float)m_flexRegs->m_iST_Scaler[m_iSectorNum*4+2] / elapsedTime);
	m_stST_Scaler4.Format("Scaler = %d\nRate = %.3fHz", m_flexRegs->m_iST_Scaler[m_iSectorNum*4+3], (float)m_flexRegs->m_iST_Scaler[m_iSectorNum*4+3] / elapsedTime);
	m_stSTS_Scaler.Format("Scaler = %d\nRate = %.3fHz", m_flexRegs->m_iSTS_Scaler[m_iSectorNum], (float)m_flexRegs->m_iSTS_Scaler[m_iSectorNum] / elapsedTime);
	m_stECE_Scaler.Format("Scaler = %d\nRate = %.3fHz", m_flexRegs->m_iECE_Scaler[m_iSectorNum], (float)m_flexRegs->m_iECE_Scaler[m_iSectorNum] / elapsedTime);
	m_stSTOF_Scaler.Format("Scaler = %d\nRate = %.3fHz", m_flexRegs->m_iSTOF_Scaler[m_iSectorNum], (float)m_flexRegs->m_iSTOF_Scaler[m_iSectorNum] / elapsedTime);
	m_stECP_Scaler.Format("Scaler = %d\nRate = %.3fHz", m_flexRegs->m_iECP_Scaler[m_iSectorNum], (float)m_flexRegs->m_iECP_Scaler[m_iSectorNum] / elapsedTime);
	m_stECC_Scaler.Format("Scaler = %d\nRate = %.3fHz", m_flexRegs->m_iECC_Scaler[m_iSectorNum], (float)m_flexRegs->m_iECC_Scaler[m_iSectorNum] / elapsedTime);

	switch(m_flexRegs->m_iSTOF_Mode[m_iSectorNum])
	{
		case STOF_AND:	m_stSTOF_Mode.Format("and"); break;
		case STOF_STS:	m_stSTOF_Mode.Format("STS"); break;
		case STOF_TOF:	m_stSTOF_Mode.Format("TOF"); break;
		case STOF_HI:	m_stSTOF_Mode.Format("1"); break;
		case STOF_LO:	m_stSTOF_Mode.Format("0"); break;
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
	dc.MoveTo(429, 82); dc.LineTo(467, 82); dc.LineTo(467, 118); dc.LineTo(334, 118); dc.LineTo(334, 162); dc.LineTo(359, 162);
	dc.MoveTo(277, 173); dc.LineTo(359, 173);

	dc.MoveTo(314, 220); dc.LineTo(397, 220);

	dc.MoveTo(428, 286); dc.LineTo(450, 286); dc.LineTo(450, 313); dc.LineTo(340, 313); dc.LineTo(340, 333); dc.LineTo(362, 333);
	dc.MoveTo(362, 345); dc.LineTo(277, 345);
}
