// TestDialog.cpp : implementation file
//

#include "stdafx.h"
#include "HallBFlexTriggerGUI.h"
#include "HallBFlexTriggerGUIDlg.h"
#include "HallBFlexRegisters.h"
#include "TestDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define V1495_TESTADDRESS	0x08520000
#define A_PATTERN_L			0x0016
#define A_PATTERN_H			0x0018

/////////////////////////////////////////////////////////////////////////////
// TestDialog dialog


TestDialog::TestDialog(CWnd* pParent /*=NULL*/)
	: CDialog(TestDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(TestDialog)
	m_csPattern = _T("");
	m_stTestStatus = _T("");
	//}}AFX_DATA_INIT
}


void TestDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TestDialog)
	DDX_Text(pDX, IDC_EDIT_PATTERN, m_csPattern);
	DDX_Text(pDX, IDC_ST_TESTSTATUS, m_stTestStatus);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(TestDialog, CDialog)
	//{{AFX_MSG_MAP(TestDialog)
	ON_BN_CLICKED(IDC_B_PATTERN, OnBPattern)
	ON_BN_CLICKED(IDC_B_PROG, OnBProg)
	ON_BN_CLICKED(IDC_B_START, OnBStart)
	ON_BN_CLICKED(IDC_B_STOP, OnBStop)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TestDialog message handlers

void TestDialog::OnBPattern() 
{
	HallBFlexRegisters *pFlexRegs = (HallBFlexRegisters *)&((CHallBFlexTriggerGUIDlg *)GetParent())->m_HallBFlexRegisters;

	if(pFlexRegs->m_bConnected)
	{
		UpdateData(TRUE);
		unsigned int pattern = StrToHex(m_csPattern);

		pFlexRegs->VMEWrite16(V1495_TESTADDRESS + A_PATTERN_L, pattern & 0xFFFF);
		pFlexRegs->VMEWrite16(V1495_TESTADDRESS + A_PATTERN_H, pattern >> 16);
	}
}

unsigned int TestDialog::StrToHex(CString val)
{
	unsigned int v = 0;
	int len = val.GetLength();

	for(int i = 0; i < len; i++)
	{
		char ch = val[len-1-i];
		unsigned int digit = 0;

		if( (ch >= '0') && (ch <= '9') )
			digit = ch - '0';
		else if( (toupper(ch) >= 'A') && (toupper(ch) <= 'F') )
			digit = 10 + toupper(ch) - 'A';

		v += digit*(1<<(i*4));
	}
	return v;
}

void TestDialog::OnBProg() 
{
	HallBFlexRegisters *pFlexRegs = (HallBFlexRegisters *)&((CHallBFlexTriggerGUIDlg *)GetParent())->m_HallBFlexRegisters;
	CFileDialog dlgFile(TRUE, "dat", "*.rbf", OFN_OVERWRITEPROMPT, "*.rbf", this);

	if(dlgFile.DoModal() == IDOK)
	{
		if(!pFlexRegs->ProgramModule(dlgFile.GetPathName(), V1495_TESTADDRESS))
		{
			CString errorMsg;

			errorMsg.Format("ERROR: unable to program file %s", dlgFile.GetPathName());
			MessageBox(errorMsg);
		}
		else
			pFlexRegs->VMEWrite16(V1495_TESTADDRESS + 0x8016, 1);
	}
}

void TestDialog::OnBStart() 
{
	HallBFlexRegisters *pFlexRegs = (HallBFlexRegisters *)&((CHallBFlexTriggerGUIDlg *)GetParent())->m_HallBFlexRegisters;
	pFlexRegs->VMEWrite16(V1495_TESTADDRESS + 0x0012, 0);
	SetTimer(1, 500, NULL);
}

void TestDialog::OnBStop() 
{
	KillTimer(1);
}

void TestDialog::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == 1)
	{
		KillTimer(1);
		
		HallBFlexRegisters *pFlexRegs = (HallBFlexRegisters *)&((CHallBFlexTriggerGUIDlg *)GetParent())->m_HallBFlexRegisters;
		pFlexRegs->VMEWrite16(V1495_TESTADDRESS + 0x0010, 0);

		m_stTestStatus.Format(	"TRIG 1 Count: %10u\tTRIG 1 Bad: %10u\n"\
								"TRIG 2 Count: %10u\tTRIG 2 Bad: %10u\n"\
								"TRIG 3 Count: %10u\tTRIG 3 Bad: %10u\n"\
								"TRIG 4 Count: %10u\tTRIG 4 Bad: %10u\n"\
								"TRIG 5 Count: %10u\tTRIG 5 Bad: %10u\n"\
								"TRIG 6 Count: %10u\tTRIG 6 Bad: %10u\n"\
								"TRIG 7 Count: %10u\tTRIG 7 Bad: %10u\n"\
								"TRIG 8 Count: %10u\tTRIG 8 Bad: %10u\n"\
								"TRIG 9 Count: %10u\tTRIG 9 Bad: %10u\n"\
								"TRIG 10 Count: %10u\tTRIG 10 Bad: %10u\n"\
								"TRIG 11 Count: %10u\tTRIG 11 Bad: %10u\n"\
								"TRIG 12 Count: %10u\tTRIG 12 Bad: %10u\n"\
								"TestCount: %10u\n",
								ReadTest32(0x100C), ReadTest32(0x1010), 
								ReadTest32(0x1014), ReadTest32(0x1018), 
								ReadTest32(0x100C), ReadTest32(0x1020), 
								ReadTest32(0x1024), ReadTest32(0x1028), 
								ReadTest32(0x102C), ReadTest32(0x1030), 
								ReadTest32(0x1034), ReadTest32(0x1038), 
								ReadTest32(0x103C), ReadTest32(0x1040), 
								ReadTest32(0x1044), ReadTest32(0x1048), 
								ReadTest32(0x104C), ReadTest32(0x1050), 
								ReadTest32(0x1054), ReadTest32(0x1058), 
								ReadTest32(0x105C), ReadTest32(0x1060), 
								ReadTest32(0x1064), ReadTest32(0x1068), 
								ReadTest32(0x1000));
		UpdateData(FALSE);
		SetTimer(1, 500, NULL);
	}
	CDialog::OnTimer(nIDEvent);
}

unsigned int TestDialog::ReadTest32(unsigned int offset)
{
	HallBFlexRegisters *pFlexRegs = (HallBFlexRegisters *)&((CHallBFlexTriggerGUIDlg *)GetParent())->m_HallBFlexRegisters;
	unsigned int result;
	unsigned short *p = (unsigned short *)&result;

	pFlexRegs->VMERead16(V1495_TESTADDRESS + offset, &p[0]);
	pFlexRegs->VMERead16(V1495_TESTADDRESS + offset + 2, &p[1]);

	return result;
}
