// DebugDlg.cpp : implementation file
//

#include "stdafx.h"
#include "HallBFlexTriggerGUI.h"
#include "DebugDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DebugDlg dialog


DebugDlg::DebugDlg(CWnd* pParent /*=NULL*/)
	: CDialog(DebugDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(DebugDlg)
	//}}AFX_DATA_INIT
}


void DebugDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DebugDlg)
	DDX_Control(pDX, IDC_CH_DEBUGENABLE, m_chDebugAll);
	DDX_Control(pDX, IDC_LIST_DEBUG, m_listDebug);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DebugDlg, CDialog)
	//{{AFX_MSG_MAP(DebugDlg)
	ON_BN_CLICKED(IDC_B_DBGCLEAR, OnBDbgclear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DebugDlg message handlers

void DebugDlg::DebugMsg(CString msg)
{
	if(m_listDebug.GetCount() >= 1000)
		m_listDebug.DeleteString(1000);
	m_listDebug.InsertString(0, msg);
}

void DebugDlg::OnBDbgclear() 
{
	while(m_listDebug.GetCount())
		m_listDebug.DeleteString(0);
}

