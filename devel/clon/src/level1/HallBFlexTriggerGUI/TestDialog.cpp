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

/////////////////////////////////////////////////////////////////////////////
// TestDialog dialog


TestDialog::TestDialog(CWnd* pParent /*=NULL*/)
	: CDialog(TestDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(TestDialog)
	m_csPattern = _T("");
	//}}AFX_DATA_INIT
}


void TestDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TestDialog)
	DDX_Text(pDX, IDC_EDIT_PATTERN, m_csPattern);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(TestDialog, CDialog)
	//{{AFX_MSG_MAP(TestDialog)
	ON_BN_CLICKED(IDC_B_PATTERN, OnBPattern)
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

#define V1495_TESTADDRESS	0x20000000
#define A_PATTERN_L			0x0016
#define A_PATTERN_H			0x0018

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
