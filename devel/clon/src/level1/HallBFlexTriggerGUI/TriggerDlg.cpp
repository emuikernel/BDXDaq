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
	m_stLUTMOR_Scaler = _T("");
	m_stLUTMORST_Scaler = _T("");
	m_stLUT_Scaler = _T("");
	m_stMOR_Mode = _T("");
	m_stMORA_Dly = _T("");
	m_stMORB_Dly = _T("");
	m_stMult_MinMax = _T("");
	m_stTrig_Persist = _T("");
	m_stTrigPersist_Scaler = _T("");
	m_stTrig_Prescale = _T("");
	m_stTrigPrescale_Scaler = _T("");
	m_stSTMult_Scaler = _T("");
	m_stTrigOut = _T("");
	m_stMORA_Scaler = _T("");
	m_stMORB_Scaler = _T("");
	m_stMOROr_Scaler = _T("");
	m_stMousePos = _T("");
	m_csECCLutFile = _T("");
	m_csECPLutFile = _T("");
	m_stSTMult_Mode = _T("");
	//}}AFX_DATA_INIT
}


void TriggerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TriggerDlg)
	DDX_Control(pDX, IDC_SP_PRESCALE, m_spTrig_Prescale);
	DDX_Control(pDX, IDC_SP_PERSIST, m_spTrig_Persist);
	DDX_Control(pDX, IDC_SP_MULTMIN, m_spSTMult_Min);
	DDX_Control(pDX, IDC_SP_MULTMAX, m_spSTMult_Max);
	DDX_Control(pDX, IDC_SP_MORB_DLY, m_spMORB_Dly);
	DDX_Control(pDX, IDC_SP_MORA_DLY, m_spMORA_Dly);
	DDX_Text(pDX, IDC_ST_LUT_MOR_SCALER, m_stLUTMOR_Scaler);
	DDX_Text(pDX, IDC_ST_LUT_MOR_ST_SCALER, m_stLUTMORST_Scaler);
	DDX_Text(pDX, IDC_ST_LUT_SCALER, m_stLUT_Scaler);
	DDX_Text(pDX, IDC_ST_MOR_MODE, m_stMOR_Mode);
	DDX_Text(pDX, IDC_ST_MORA_DLY, m_stMORA_Dly);
	DDX_Text(pDX, IDC_ST_MORB_DLY, m_stMORB_Dly);
	DDX_Text(pDX, IDC_ST_MULTMINMAX, m_stMult_MinMax);
	DDX_Text(pDX, IDC_ST_PERSIST, m_stTrig_Persist);
	DDX_Text(pDX, IDC_ST_PERSIST_SCALER, m_stTrigPersist_Scaler);
	DDX_Text(pDX, IDC_ST_PRESCALE, m_stTrig_Prescale);
	DDX_Text(pDX, IDC_ST_PRESCALE_SCALER, m_stTrigPrescale_Scaler);
	DDX_Text(pDX, IDC_ST_STMULT_SCALER, m_stSTMult_Scaler);
	DDX_Text(pDX, IDC_ST_TRIGOUT, m_stTrigOut);
	DDX_Text(pDX, IDC_ST_MORA_SCALER, m_stMORA_Scaler);
	DDX_Text(pDX, IDC_ST_MORB_SCALER, m_stMORB_Scaler);
	DDX_Text(pDX, IDC_ST_MOROR_SCALER, m_stMOROr_Scaler);
	DDX_Text(pDX, IDC_ST_MOUSEPOS, m_stMousePos);
	DDX_Text(pDX, IDC_ST_ECCLUT_FILE, m_csECCLutFile);
	DDX_Text(pDX, IDC_ST_ECPLUT_FILE, m_csECPLutFile);
	DDX_Text(pDX, IDC_ST_STMULT_MODE, m_stSTMult_Mode);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(TriggerDlg, CDialog)
	//{{AFX_MSG_MAP(TriggerDlg)
	ON_BN_CLICKED(IDC_B_LOAD_STOF_ECC, OnBLoadStofEcc)
	ON_BN_CLICKED(IDC_B_LOAD_STOF_ECP, OnBLoadStofEcp)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SP_MORA_DLY, OnDeltaposSpMoraDly)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SP_MORB_DLY, OnDeltaposSpMorbDly)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SP_MULTMAX, OnDeltaposSpMultmax)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SP_MULTMIN, OnDeltaposSpMultmin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SP_PERSIST, OnDeltaposSpPersist)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SP_PRESCALE, OnDeltaposSpPrescale)
	ON_BN_CLICKED(IDC_ST_MOR_MODE, OnStMorMode)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_ST_STMULT_MODE, OnStStmultMode)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TriggerDlg message handlers

void TriggerDlg::OnBLoadStofEcc() 
{
	CFileDialog dlgFile(TRUE, "lut", "*.lut", OFN_OVERWRITEPROMPT, "*.lut", this);
	CHallBFlexTriggerGUIDlg *pFlexGUIDlg = (CHallBFlexTriggerGUIDlg *)GetParent();

	if(dlgFile.DoModal() == IDOK)
	{
		CString str;
		CStdioFile loadFile;
		CFileException exception;

		if(!loadFile.Open(dlgFile.GetPathName(), CFile::modeRead, &exception))
		{
			str = "Error opening file: ";
			str += dlgFile.GetPathName();
			str += "Reason: ";
			str += exception.m_cause;
			MessageBox(str);
			return;
		}

		unsigned short LUT[256];
		memset(LUT, 0, sizeof(LUT));

		int bit = 0;
		int word = 0;
		while(loadFile.ReadString(str))
		{
			str.TrimLeft(" \t");
			if(str[0] == '1' || str[0] == '0')
			{
				if(word < 256)
				{
					if(str[0] == '1')
						LUT[word] |= (1<<bit);

				}
				bit = (bit + 1) % 16;
				if(!bit)
					word++;
			}
		}
		loadFile.Close();

		if( (word != 256) || (bit != 0) )
		{
			pFlexGUIDlg->DebugMsg("Error: Too many LUT bits in file");
			return;
		}
		else
			pFlexGUIDlg->DebugMsg("SUCCESS: LUT bit file ready okay");

		m_csECCLutFile.Format(dlgFile.GetFileName());
		UpdateData(FALSE);

		HallBFlexRegisters *flexRegs = &(((CHallBFlexTriggerGUIDlg *)GetParent())->m_HallBFlexRegisters);
		unsigned short offset = TS_CFG_LUT_BASE + (m_iTriggerNum<<9);
		flexRegs->VMEBlkWrite16(V1495_VMEADDRESS + offset, 256, LUT, FALSE);
	}
}

void TriggerDlg::OnBLoadStofEcp() 
{
	CFileDialog dlgFile(TRUE, "lut", "*.lut", OFN_OVERWRITEPROMPT, "*.lut", this);
	CHallBFlexTriggerGUIDlg *pFlexGUIDlg = (CHallBFlexTriggerGUIDlg *)GetParent();

	if(dlgFile.DoModal() == IDOK)
	{
		CString str;
		CStdioFile loadFile;
		CFileException exception;

		if(!loadFile.Open(dlgFile.GetPathName(), CFile::modeRead, &exception))
		{
			str = "Error opening file: ";
			str += dlgFile.GetPathName();
			str += "Reason: ";
			str += exception.m_cause;
			MessageBox(str);
			return;
		}

		unsigned short LUT[256];
		memset(LUT, 0, sizeof(LUT));

		int bit = 0;
		int word = 0;
		while(loadFile.ReadString(str))
		{
			str.TrimLeft(" \t");
			if(str[0] == '1' || str[0] == '0')
			{
				if(word < 256)
				{
					if(str[0] == '1')
						LUT[word] |= (1<<bit);

				}
				bit = (bit + 1) % 16;
				if(!bit)
					word++;
			}
		}
		loadFile.Close();

		if( (word != 256) || (bit != 0) )
		{
			pFlexGUIDlg->DebugMsg("Error: Too many LUT bits in file");
			return;
		}
		else
			pFlexGUIDlg->DebugMsg("SUCCES: LUT bit file ready okay");

		m_csECPLutFile.Format(dlgFile.GetFileName());
		UpdateData(FALSE);

		HallBFlexRegisters *flexRegs = &(((CHallBFlexTriggerGUIDlg *)GetParent())->m_HallBFlexRegisters);
		unsigned short offset = TS_CFG_LUT_BASE + ((m_iTriggerNum+12)<<9);
		flexRegs->VMEBlkWrite16(V1495_VMEADDRESS + offset, 256, LUT, FALSE);
	}
}

void TriggerDlg::OnDeltaposSpMoraDly(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_flexRegs->IncMORADelay(-((NM_UPDOWN*)pNMHDR)->iDelta);
	UpdateControls();
	*pResult = 0;
}

void TriggerDlg::OnDeltaposSpMorbDly(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_flexRegs->IncMORBDelay(-((NM_UPDOWN*)pNMHDR)->iDelta);
	UpdateControls();
	*pResult = 0;
}

void TriggerDlg::OnDeltaposSpMultmax(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_flexRegs->IncMultMax(-((NM_UPDOWN*)pNMHDR)->iDelta);
	UpdateControls();
	*pResult = 0;
}

void TriggerDlg::OnDeltaposSpMultmin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_flexRegs->IncMultMin(-((NM_UPDOWN*)pNMHDR)->iDelta);
	UpdateControls();
	*pResult = 0;
}

void TriggerDlg::OnDeltaposSpPersist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_flexRegs->IncPersist(m_iTriggerNum, -((NM_UPDOWN*)pNMHDR)->iDelta);
	UpdateControls();
	*pResult = 0;
}

void TriggerDlg::OnDeltaposSpPrescale(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_flexRegs->IncPrescale(m_iTriggerNum, -((NM_UPDOWN*)pNMHDR)->iDelta);
	UpdateControls();
	*pResult = 0;
}

void TriggerDlg::OnStMorMode() 
{
	HallBFlexRegisters *flexRegs = &(((CHallBFlexTriggerGUIDlg *)GetParent())->m_HallBFlexRegisters);
	flexRegs->IncMORMode(m_iTriggerNum);
	UpdateControls();
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

	m_flexRegs = &(((CHallBFlexTriggerGUIDlg *)GetParent())->m_HallBFlexRegisters);
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

	m_stLUT_Scaler.Format("Scaler = %d\nRate = %.3fHz", flexRegs->m_iLUT_Scaler[m_iTriggerNum], (float)flexRegs->m_iLUT_Scaler[m_iTriggerNum] / elapsedTime);
	m_stLUTMOR_Scaler.Format("Scaler = %d\nRate = %.3fHz", flexRegs->m_iLUTMOR_Scaler[m_iTriggerNum], (float)flexRegs->m_iLUTMOR_Scaler[m_iTriggerNum] / elapsedTime);
	m_stLUTMORST_Scaler.Format("Scaler = %d\nRate = %.3fHz", flexRegs->m_iLUTMORST_Scaler[m_iTriggerNum], (float)flexRegs->m_iLUTMORST_Scaler[m_iTriggerNum] / elapsedTime);
	m_stSTMult_Scaler.Format("Scaler = %d\nRate = %.3fHz", flexRegs->m_iSTMult_Scaler, (float)flexRegs->m_iSTMult_Scaler / elapsedTime);
	m_stTrigPersist_Scaler.Format("Scaler = %d\nRate = %.3fHz", flexRegs->m_iTrigPersist_Scaler[m_iTriggerNum], (float)flexRegs->m_iTrigPersist_Scaler[m_iTriggerNum] / elapsedTime);
	m_stTrigPrescale_Scaler.Format("Scaler = %d\nRate = %.3fHz", flexRegs->m_iTrigPrescale_Scaler[m_iTriggerNum], (float)flexRegs->m_iTrigPrescale_Scaler[m_iTriggerNum] / elapsedTime);

	m_stMORA_Scaler.Format("Scaler = %d\nRate = %.3fHz", flexRegs->m_iMORA_Scaler, (float)flexRegs->m_iMORA_Scaler / elapsedTime);
	m_stMORB_Scaler.Format("Scaler = %d\nRate = %.3fHz", flexRegs->m_iMORB_Scaler, (float)flexRegs->m_iMORB_Scaler / elapsedTime);
	m_stMOROr_Scaler.Format("Scaler = %d\nRate = %.3fHz", flexRegs->m_iMOROr_Scaler, (float)flexRegs->m_iMOROr_Scaler / elapsedTime);

	m_stMORA_Dly.Format("Delay = %dns", flexRegs->m_iMORA_Delay * CLOCK_PERIOD_NS);
	m_stMORB_Dly.Format("Delay = %dns", flexRegs->m_iMORB_Delay * CLOCK_PERIOD_NS);

	m_stTrig_Persist.Format("Persist = %dns", flexRegs->m_iTrig_Persist[m_iTriggerNum] * CLOCK_PERIOD_NS);
	m_stTrig_Prescale.Format("Prescale = %d", flexRegs->m_iTrig_Prescale[m_iTriggerNum]);

	m_stMult_MinMax.Format("Mult Min = %d\nMult Max = %d", flexRegs->m_iMult_Min, flexRegs->m_iMult_Max);

	switch(flexRegs->m_iMOR_Mode[m_iTriggerNum])
	{
		case MOR_OR:	m_stMOR_Mode.Format("or"); break;
		case MOR_MORA:	m_stMOR_Mode.Format("MORA"); break;
		case MOR_MORB:	m_stMOR_Mode.Format("MORB"); break;
		case MOR_HI:	m_stMOR_Mode.Format("1"); break;
	}
	switch(flexRegs->m_iSTMult_Mode[m_iTriggerNum])
	{
		case STMULT_AND: m_stSTMult_Mode.Format("and"); break;
		case STMULT_MOR: m_stSTMult_Mode.Format("MOR"); break;
	}
	UpdateData(FALSE);
}

void TriggerDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CPen pen;

	pen.CreatePen(PS_SOLID, 3, RGB(0,0,0));
	dc.SelectObject(&pen);
	dc.MoveTo(162, 123); dc.LineTo(250, 123); dc.LineTo(250, 80); dc.LineTo(344, 80);
	dc.MoveTo(492, 71); dc.LineTo(510, 71); dc.LineTo(510, 114); dc.LineTo(315, 114); dc.LineTo(315, 140); dc.LineTo(344, 140);
	dc.MoveTo(104, 171); dc.LineTo(344, 171);
	dc.MoveTo(492, 156); dc.LineTo(510, 156); dc.LineTo(510, 197); dc.LineTo(50, 197); dc.LineTo(50, 225); dc.LineTo(65, 225);

	UpdateControls();
}

void TriggerDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	m_stMousePos.Format("X=%d,Y=%d", point.x, point.y);
	UpdateData(FALSE);
	CDialog::OnMouseMove(nFlags, point);
}

void TriggerDlg::OnStStmultMode() 
{
	HallBFlexRegisters *flexRegs = &(((CHallBFlexTriggerGUIDlg *)GetParent())->m_HallBFlexRegisters);
	flexRegs->IncSTMultMode(m_iTriggerNum);
	UpdateControls();
}