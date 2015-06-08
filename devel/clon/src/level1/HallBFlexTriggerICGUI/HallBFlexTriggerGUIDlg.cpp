// HallBFlexTriggerGUIDlg.cpp : implementation file
//

#include "stdafx.h"
#include "HallBFlexTriggerGUI.h"
#include "HallBFlexTriggerGUIDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHallBFlexTriggerGUIDlg dialog

CHallBFlexTriggerGUIDlg::CHallBFlexTriggerGUIDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHallBFlexTriggerGUIDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHallBFlexTriggerGUIDlg)
	m_stScalerPeriod = _T("");
	m_stServerAddress = _T("");
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHallBFlexTriggerGUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHallBFlexTriggerGUIDlg)
	DDX_Control(pDX, IDC_SL_SCALERPERIOD, m_slScalerPeriod);
	DDX_Control(pDX, IDC_TAB_TRIGMAIN, m_tabTrigMain);
	DDX_Text(pDX, IDC_ST_SCALERPERIOD, m_stScalerPeriod);
	DDX_Text(pDX, IDC_E_SERVERADDR, m_stServerAddress);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CHallBFlexTriggerGUIDlg, CDialog)
	//{{AFX_MSG_MAP(CHallBFlexTriggerGUIDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_B_CONNECT, OnBConnect)
	ON_BN_CLICKED(IDC_B_DISCONNECT, OnBDisconnect)
	ON_BN_CLICKED(IDC_B_LOADSETTINGS, OnBLoadsettings)
	ON_BN_CLICKED(IDC_B_SAVESETTINGS, OnBSavesettings)
	ON_BN_CLICKED(IDC_B_SCOPE, OnBScope)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_TRIGMAIN, OnSelchangeTabTrigmain)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SL_SCALERPERIOD, OnCustomdrawSlScalerperiod)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_B_FIRMWARE, OnBFirmware)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHallBFlexTriggerGUIDlg message handlers

BOOL CHallBFlexTriggerGUIDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	CString str;
	int i, id = 0;

	m_dlgTest.Create(IDD_TEST_DIALOG);
	m_ScopeDlg.Create(IDD_SCOPE_DIALOG);
	m_dlgDebug.Create(IDD_DEBUG_DIALOG);
	m_dlgTrigger12.Create(IDD_TRIGGER12_DIALOG);
	m_tabTrigMain.InsertItem(id, "Debug");
	m_pCDialogs[id++] = &m_dlgDebug;

	for(i = 0; i < 6; i++)
	{
		m_dlgSectors[i].Create(IDD_SECTOR_DIALOG);
		m_dlgSectors[i].SetSectorNum(i);
		str.Format("Sect %d", i+1);
		m_tabTrigMain.InsertItem(id, str);
		m_pCDialogs[id++] = &m_dlgSectors[i];
	}
	for(i = 0; i < 11; i++)
	{
		m_dlgTriggers[i].Create(IDD_TRIGGER_DIALOG);
		m_dlgTriggers[i].SetTriggerNum(i);
		str.Format("Trig %d", i+1);
		m_tabTrigMain.InsertItem(id, str);
		m_pCDialogs[id++] = &m_dlgTriggers[i];
	}
	m_tabTrigMain.InsertItem(id, "Trig 12");
	m_pCDialogs[id++] = &m_dlgTrigger12;

	m_tabTrigMain.InsertItem(id, "Testing");
	m_pCDialogs[id++] = &m_dlgTest;

	m_tabTrigMain.SetCurSel(0);

	OnSelchangeTabTrigmain(NULL, NULL);
	GetDlgItem(IDC_B_DISCONNECT)->EnableWindow(FALSE);

	m_slScalerPeriod.SetRange(1, 60);
	m_slScalerPeriod.SetPos(10);

	m_HallBFlexRegisters.SetDebugDlg((void *)&m_dlgDebug);

	m_stServerAddress.Format("clastrig2");
	UpdateData(FALSE);

	DebugMsg("HallB Flex Trigger GUI Initialized. Built: "__DATE__" " __TIME__);
	return TRUE;
}

void CHallBFlexTriggerGUIDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CHallBFlexTriggerGUIDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CHallBFlexTriggerGUIDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CHallBFlexTriggerGUIDlg::OnBConnect() 
{
	UpdateData(TRUE);
	if(!m_HallBFlexRegisters.ConnectVME(m_stServerAddress))
		return;

	GetDlgItem(IDC_B_DISCONNECT)->EnableWindow(TRUE);
	GetDlgItem(IDC_B_CONNECT)->EnableWindow(FALSE);

	UpdateTabs();
}

void CHallBFlexTriggerGUIDlg::OnBDisconnect() 
{
	m_HallBFlexRegisters.DisconnectVME();

	GetDlgItem(IDC_B_DISCONNECT)->EnableWindow(FALSE);
	GetDlgItem(IDC_B_CONNECT)->EnableWindow(TRUE);
}

void CHallBFlexTriggerGUIDlg::OnBLoadsettings() 
{
	CFileDialog dlgFile(TRUE, "dat", "*.dat", 0, "*.dat", this);

	if(dlgFile.DoModal() == IDOK)
	{
		if(!m_HallBFlexRegisters.LoadRegisterSettings(dlgFile.GetPathName()))
		{
			CString errorMsg;

			errorMsg.Format("ERROR: unable to load file %s", dlgFile.GetPathName());
			MessageBox(errorMsg);
		}
		else
			UpdateTabs();
	}
}

void CHallBFlexTriggerGUIDlg::OnBSavesettings() 
{
	CFileDialog dlgFile(FALSE, "dat", "*.dat", OFN_OVERWRITEPROMPT, "*.dat", this);

	if(dlgFile.DoModal() == IDOK)
	{
		if(!m_HallBFlexRegisters.SaveRegisterSettings(dlgFile.GetPathName()))
		{
			CString errorMsg;

			errorMsg.Format("ERROR: unable to save file %s", dlgFile.GetPathName());
			MessageBox(errorMsg);
		}
	}
}

void CHallBFlexTriggerGUIDlg::OnBScope() 
{
	m_ScopeDlg.ShowWindow(SW_SHOW);
}

void CHallBFlexTriggerGUIDlg::OnSelchangeTabTrigmain(NMHDR* pNMHDR, LRESULT* pResult)
{
	static int lastDialog = -1;

	if(pResult)
		*pResult = 0;
	else
	{
		CRect l_rectClient;
		CRect l_rectWnd;

		m_tabTrigMain.GetClientRect(l_rectClient);
		m_tabTrigMain.AdjustRect(FALSE,l_rectClient);
		m_tabTrigMain.GetWindowRect(l_rectWnd);
		ScreenToClient(l_rectWnd);
		l_rectClient.OffsetRect(l_rectWnd.left,l_rectWnd.top);

		for(int i = 0; i < sizeof(m_pCDialogs)/sizeof(m_pCDialogs[0]); i++)
			m_pCDialogs[i]->SetWindowPos(&wndTop, l_rectClient.left, l_rectClient.top, l_rectClient.Width(), l_rectClient.Height(), SWP_HIDEWINDOW);
	}

	if(lastDialog >= 0)
	{
		m_pCDialogs[lastDialog]->EnableWindow(FALSE);
		m_pCDialogs[lastDialog]->ShowWindow(SW_HIDE);
	}
	lastDialog = m_tabTrigMain.GetCurSel();
	m_pCDialogs[lastDialog]->EnableWindow(TRUE);
	m_pCDialogs[lastDialog]->ShowWindow(SW_SHOW);
}

void CHallBFlexTriggerGUIDlg::DebugMsg(CString str)
{
	m_dlgDebug.DebugMsg(str);	
}

void CHallBFlexTriggerGUIDlg::OnCustomdrawSlScalerperiod(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if(m_slScalerPeriod.GetPos() == 60)
	{
		m_stScalerPeriod.Format("Scaler Sample: disabled");
	}
	else
		m_stScalerPeriod.Format("Scaler Sample Period: %ds", m_slScalerPeriod.GetPos());
	UpdateData(FALSE);

	KillTimer(0);
	SetTimer(0, m_slScalerPeriod.GetPos() * 1000, NULL);

	*pResult = 0;
}

void CHallBFlexTriggerGUIDlg::OnTimer(UINT nIDEvent) 
{
	KillTimer(0);

	if(!GetDlgItem(IDC_B_CONNECT)->IsWindowEnabled() && (m_slScalerPeriod.GetPos() != 60) && m_HallBFlexRegisters.m_bConnected)
	{
		DebugMsg("Reading scalers...");
		m_HallBFlexRegisters.VMEWrite16(TS_BOARD_ADDRESS + TS_ENABLE_SCALERS, 0x0000, FALSE);

		m_HallBFlexRegisters.m_iTOF_Scaler[0] = ReadScaler32(TS_TOF1_SCALER_H);
		m_HallBFlexRegisters.m_iTOF_Scaler[1] = ReadScaler32(TS_TOF2_SCALER_H);
		m_HallBFlexRegisters.m_iTOF_Scaler[2] = ReadScaler32(TS_TOF3_SCALER_H);
		m_HallBFlexRegisters.m_iTOF_Scaler[3] = ReadScaler32(TS_TOF4_SCALER_H);
		m_HallBFlexRegisters.m_iTOF_Scaler[4] = ReadScaler32(TS_TOF5_SCALER_H);
		m_HallBFlexRegisters.m_iTOF_Scaler[5] = ReadScaler32(TS_TOF6_SCALER_H);
		m_HallBFlexRegisters.m_iECinP_Scaler[0] = ReadScaler32(TS_ECin1P_SCALER_H);
		m_HallBFlexRegisters.m_iECinP_Scaler[1] = ReadScaler32(TS_ECin2P_SCALER_H);
		m_HallBFlexRegisters.m_iECinP_Scaler[2] = ReadScaler32(TS_ECin3P_SCALER_H);
		m_HallBFlexRegisters.m_iECinP_Scaler[3] = ReadScaler32(TS_ECin4P_SCALER_H);
		m_HallBFlexRegisters.m_iECinP_Scaler[4] = ReadScaler32(TS_ECin5P_SCALER_H);
		m_HallBFlexRegisters.m_iECinP_Scaler[5] = ReadScaler32(TS_ECin6P_SCALER_H);
		m_HallBFlexRegisters.m_iECinE_Scaler[0] = ReadScaler32(TS_ECin1E_SCALER_H);
		m_HallBFlexRegisters.m_iECinE_Scaler[1] = ReadScaler32(TS_ECin2E_SCALER_H);
		m_HallBFlexRegisters.m_iECinE_Scaler[2] = ReadScaler32(TS_ECin3E_SCALER_H);
		m_HallBFlexRegisters.m_iECinE_Scaler[3] = ReadScaler32(TS_ECin4E_SCALER_H);
		m_HallBFlexRegisters.m_iECinE_Scaler[4] = ReadScaler32(TS_ECin5E_SCALER_H);
		m_HallBFlexRegisters.m_iECinE_Scaler[5] = ReadScaler32(TS_ECin6E_SCALER_H);
		m_HallBFlexRegisters.m_iECtotP_Scaler[0] = ReadScaler32(TS_ECtot1P_SCALER_H);
		m_HallBFlexRegisters.m_iECtotP_Scaler[1] = ReadScaler32(TS_ECtot2P_SCALER_H);
		m_HallBFlexRegisters.m_iECtotP_Scaler[2] = ReadScaler32(TS_ECtot3P_SCALER_H);
		m_HallBFlexRegisters.m_iECtotP_Scaler[3] = ReadScaler32(TS_ECtot4P_SCALER_H);
		m_HallBFlexRegisters.m_iECtotP_Scaler[4] = ReadScaler32(TS_ECtot5P_SCALER_H);
		m_HallBFlexRegisters.m_iECtotP_Scaler[5] = ReadScaler32(TS_ECtot6P_SCALER_H);
		m_HallBFlexRegisters.m_iECtotE_Scaler[0] = ReadScaler32(TS_ECtot1E_SCALER_H);
		m_HallBFlexRegisters.m_iECtotE_Scaler[1] = ReadScaler32(TS_ECtot2E_SCALER_H);
		m_HallBFlexRegisters.m_iECtotE_Scaler[2] = ReadScaler32(TS_ECtot3E_SCALER_H);
		m_HallBFlexRegisters.m_iECtotE_Scaler[3] = ReadScaler32(TS_ECtot4E_SCALER_H);
		m_HallBFlexRegisters.m_iECtotE_Scaler[4] = ReadScaler32(TS_ECtot5E_SCALER_H);
		m_HallBFlexRegisters.m_iECtotE_Scaler[5] = ReadScaler32(TS_ECtot6E_SCALER_H);
		m_HallBFlexRegisters.m_iCC_Scaler[0] = ReadScaler32(TS_CC1_SCALER_H);
		m_HallBFlexRegisters.m_iCC_Scaler[1] = ReadScaler32(TS_CC2_SCALER_H);
		m_HallBFlexRegisters.m_iCC_Scaler[2] = ReadScaler32(TS_CC3_SCALER_H);
		m_HallBFlexRegisters.m_iCC_Scaler[3] = ReadScaler32(TS_CC4_SCALER_H);
		m_HallBFlexRegisters.m_iCC_Scaler[4] = ReadScaler32(TS_CC5_SCALER_H);
		m_HallBFlexRegisters.m_iCC_Scaler[5] = ReadScaler32(TS_CC6_SCALER_H);
		m_HallBFlexRegisters.m_iECE_Scaler[0] = ReadScaler32(TS_ECE1_SCALER_H);
		m_HallBFlexRegisters.m_iECE_Scaler[1] = ReadScaler32(TS_ECE2_SCALER_H);
		m_HallBFlexRegisters.m_iECE_Scaler[2] = ReadScaler32(TS_ECE3_SCALER_H);
		m_HallBFlexRegisters.m_iECE_Scaler[3] = ReadScaler32(TS_ECE4_SCALER_H);
		m_HallBFlexRegisters.m_iECE_Scaler[4] = ReadScaler32(TS_ECE5_SCALER_H);
		m_HallBFlexRegisters.m_iECE_Scaler[5] = ReadScaler32(TS_ECE6_SCALER_H);
		m_HallBFlexRegisters.m_iECP_Scaler[0] = ReadScaler32(TS_ECP1_SCALER_H);
		m_HallBFlexRegisters.m_iECP_Scaler[1] = ReadScaler32(TS_ECP2_SCALER_H);
		m_HallBFlexRegisters.m_iECP_Scaler[2] = ReadScaler32(TS_ECP3_SCALER_H);
		m_HallBFlexRegisters.m_iECP_Scaler[3] = ReadScaler32(TS_ECP4_SCALER_H);
		m_HallBFlexRegisters.m_iECP_Scaler[4] = ReadScaler32(TS_ECP5_SCALER_H);
		m_HallBFlexRegisters.m_iECP_Scaler[5] = ReadScaler32(TS_ECP6_SCALER_H);
		m_HallBFlexRegisters.m_iECC_Scaler[0] = ReadScaler32(TS_ECC1_SCALER_H);
		m_HallBFlexRegisters.m_iECC_Scaler[1] = ReadScaler32(TS_ECC2_SCALER_H);
		m_HallBFlexRegisters.m_iECC_Scaler[2] = ReadScaler32(TS_ECC3_SCALER_H);
		m_HallBFlexRegisters.m_iECC_Scaler[3] = ReadScaler32(TS_ECC4_SCALER_H);
		m_HallBFlexRegisters.m_iECC_Scaler[4] = ReadScaler32(TS_ECC5_SCALER_H);
		m_HallBFlexRegisters.m_iECC_Scaler[5] = ReadScaler32(TS_ECC6_SCALER_H);
		m_HallBFlexRegisters.m_iESector_Scaler[0] = ReadScaler32(TS_E_SECTOR1_SCALER_H);
		m_HallBFlexRegisters.m_iESector_Scaler[1] = ReadScaler32(TS_E_SECTOR2_SCALER_H);
		m_HallBFlexRegisters.m_iESector_Scaler[2] = ReadScaler32(TS_E_SECTOR3_SCALER_H);
		m_HallBFlexRegisters.m_iESector_Scaler[3] = ReadScaler32(TS_E_SECTOR4_SCALER_H);
		m_HallBFlexRegisters.m_iESector_Scaler[4] = ReadScaler32(TS_E_SECTOR5_SCALER_H);
		m_HallBFlexRegisters.m_iESector_Scaler[5] = ReadScaler32(TS_E_SECTOR6_SCALER_H);
		m_HallBFlexRegisters.m_iTRIG_Scaler[0] = ReadScaler32(TS_TRIG1_TRIGLUT_SCALER_H);
		m_HallBFlexRegisters.m_iTRIG_Scaler[1] = ReadScaler32(TS_TRIG2_TRIGLUT_SCALER_H);
		m_HallBFlexRegisters.m_iTRIG_Scaler[2] = ReadScaler32(TS_TRIG3_TRIGLUT_SCALER_H);
		m_HallBFlexRegisters.m_iTRIG_Scaler[3] = ReadScaler32(TS_TRIG4_TRIGLUT_SCALER_H);
		m_HallBFlexRegisters.m_iTRIG_Scaler[4] = ReadScaler32(TS_TRIG5_TRIGLUT_SCALER_H);
		m_HallBFlexRegisters.m_iTRIG_Scaler[5] = ReadScaler32(TS_TRIG6_TRIGLUT_SCALER_H);
		m_HallBFlexRegisters.m_iTRIG_Scaler[6] = ReadScaler32(TS_TRIG7_TRIGLUT_SCALER_H);
		m_HallBFlexRegisters.m_iTRIG_Scaler[7] = ReadScaler32(TS_TRIG8_TRIGLUT_SCALER_H);
		m_HallBFlexRegisters.m_iTRIG_Scaler[8] = ReadScaler32(TS_TRIG9_TRIGLUT_SCALER_H);
		m_HallBFlexRegisters.m_iTRIG_Scaler[9] = ReadScaler32(TS_TRIG10_TRIGLUT_SCALER_H);
		m_HallBFlexRegisters.m_iTRIG_Scaler[10] = ReadScaler32(TS_TRIG11_TRIGLUT_SCALER_H);
		m_HallBFlexRegisters.m_iTRIG_Scaler[11] = ReadScaler32(TS_E_TRIG_SCALER_H);
		m_HallBFlexRegisters.m_iICElectron_Scaler[0] = ReadScaler32(TS_SCALER_ELECTRON0_H);
		m_HallBFlexRegisters.m_iICElectron_Scaler[1] = ReadScaler32(TS_SCALER_ELECTRON1_H);
		m_HallBFlexRegisters.m_iICElectron_Scaler[2] = ReadScaler32(TS_SCALER_ELECTRON2_H);
		m_HallBFlexRegisters.m_iICTotal_Scaler[0] = ReadScaler32(TS_SCALER_TOTAL0_H);
		m_HallBFlexRegisters.m_iICTotal_Scaler[1] = ReadScaler32(TS_SCALER_TOTAL1_H);
		m_HallBFlexRegisters.m_iICTotal_Scaler[2] = ReadScaler32(TS_SCALER_TOTAL2_H);
		m_HallBFlexRegisters.m_iICVeto_Scaler[0] = ReadScaler32(TS_SCALER_VETO0_H);
		m_HallBFlexRegisters.m_iICVeto_Scaler[1] = ReadScaler32(TS_SCALER_VETO1_H);
		m_HallBFlexRegisters.m_iICVeto_Scaler[2] = ReadScaler32(TS_SCALER_VETO2_H);

		m_HallBFlexRegisters.m_iECPLUT_Scaler[0] = ReadScaler32(TS_TRIG1_ECPLUT_SCALER_H);
		m_HallBFlexRegisters.m_iECPLUT_Scaler[1] = ReadScaler32(TS_TRIG2_ECPLUT_SCALER_H);
		m_HallBFlexRegisters.m_iECPLUT_Scaler[2] = ReadScaler32(TS_TRIG3_ECPLUT_SCALER_H);
		m_HallBFlexRegisters.m_iECPLUT_Scaler[3] = ReadScaler32(TS_TRIG4_ECPLUT_SCALER_H);
		m_HallBFlexRegisters.m_iECPLUT_Scaler[4] = ReadScaler32(TS_TRIG5_ECPLUT_SCALER_H);
		m_HallBFlexRegisters.m_iECPLUT_Scaler[5] = ReadScaler32(TS_TRIG6_ECPLUT_SCALER_H);
		m_HallBFlexRegisters.m_iECPLUT_Scaler[6] = ReadScaler32(TS_TRIG7_ECPLUT_SCALER_H);
		m_HallBFlexRegisters.m_iECPLUT_Scaler[7] = ReadScaler32(TS_TRIG8_ECPLUT_SCALER_H);
		m_HallBFlexRegisters.m_iECPLUT_Scaler[8] = ReadScaler32(TS_TRIG9_ECPLUT_SCALER_H);
		m_HallBFlexRegisters.m_iECPLUT_Scaler[9] = ReadScaler32(TS_TRIG10_ECPLUT_SCALER_H);
		m_HallBFlexRegisters.m_iECPLUT_Scaler[10] = ReadScaler32(TS_TRIG11_ECPLUT_SCALER_H);

		m_HallBFlexRegisters.m_iECELUT_Scaler[0] = ReadScaler32(TS_TRIG1_ECELUT_SCALER_H);
		m_HallBFlexRegisters.m_iECELUT_Scaler[1] = ReadScaler32(TS_TRIG2_ECELUT_SCALER_H);
		m_HallBFlexRegisters.m_iECELUT_Scaler[2] = ReadScaler32(TS_TRIG3_ECELUT_SCALER_H);
		m_HallBFlexRegisters.m_iECELUT_Scaler[3] = ReadScaler32(TS_TRIG4_ECELUT_SCALER_H);
		m_HallBFlexRegisters.m_iECELUT_Scaler[4] = ReadScaler32(TS_TRIG5_ECELUT_SCALER_H);
		m_HallBFlexRegisters.m_iECELUT_Scaler[5] = ReadScaler32(TS_TRIG6_ECELUT_SCALER_H);
		m_HallBFlexRegisters.m_iECELUT_Scaler[6] = ReadScaler32(TS_TRIG7_ECELUT_SCALER_H);
		m_HallBFlexRegisters.m_iECELUT_Scaler[7] = ReadScaler32(TS_TRIG8_ECELUT_SCALER_H);
		m_HallBFlexRegisters.m_iECELUT_Scaler[8] = ReadScaler32(TS_TRIG9_ECELUT_SCALER_H);
		m_HallBFlexRegisters.m_iECELUT_Scaler[9] = ReadScaler32(TS_TRIG10_ECELUT_SCALER_H);
		m_HallBFlexRegisters.m_iECELUT_Scaler[10] = ReadScaler32(TS_TRIG11_ECELUT_SCALER_H);

		m_HallBFlexRegisters.m_iECCLUT_Scaler[0] = ReadScaler32(TS_TRIG1_ECCLUT_SCALER_H);
		m_HallBFlexRegisters.m_iECCLUT_Scaler[1] = ReadScaler32(TS_TRIG2_ECCLUT_SCALER_H);
		m_HallBFlexRegisters.m_iECCLUT_Scaler[2] = ReadScaler32(TS_TRIG3_ECCLUT_SCALER_H);
		m_HallBFlexRegisters.m_iECCLUT_Scaler[3] = ReadScaler32(TS_TRIG4_ECCLUT_SCALER_H);
		m_HallBFlexRegisters.m_iECCLUT_Scaler[4] = ReadScaler32(TS_TRIG5_ECCLUT_SCALER_H);
		m_HallBFlexRegisters.m_iECCLUT_Scaler[5] = ReadScaler32(TS_TRIG6_ECCLUT_SCALER_H);
		m_HallBFlexRegisters.m_iECCLUT_Scaler[6] = ReadScaler32(TS_TRIG7_ECCLUT_SCALER_H);
		m_HallBFlexRegisters.m_iECCLUT_Scaler[7] = ReadScaler32(TS_TRIG8_ECCLUT_SCALER_H);
		m_HallBFlexRegisters.m_iECCLUT_Scaler[8] = ReadScaler32(TS_TRIG9_ECCLUT_SCALER_H);
		m_HallBFlexRegisters.m_iECCLUT_Scaler[9] = ReadScaler32(TS_TRIG10_ECCLUT_SCALER_H);
		m_HallBFlexRegisters.m_iECCLUT_Scaler[10] = ReadScaler32(TS_TRIG11_ECCLUT_SCALER_H);
		
		m_HallBFlexRegisters.m_iREF_Scaler = ReadScaler32(TS_REFCLK_SCALER_H);

		m_HallBFlexRegisters.VMEWrite16(TS_BOARD_ADDRESS + TS_ENABLE_SCALERS, 0x0001, FALSE);
		DebugMsg("done.");

		UpdateTabs();
	}

	SetTimer(0, m_slScalerPeriod.GetPos() * 1000, NULL);
	CDialog::OnTimer(nIDEvent);
}

unsigned int CHallBFlexTriggerGUIDlg::ReadScaler32(unsigned int addr)
{
	unsigned short valh, vall;
	m_HallBFlexRegisters.VMERead16(TS_BOARD_ADDRESS + addr, &valh, FALSE);
	m_HallBFlexRegisters.VMERead16(TS_BOARD_ADDRESS + addr+2, &vall, FALSE);
	return (valh<<16)|vall;
}

unsigned int CHallBFlexTriggerGUIDlg::ReadScaler16(unsigned int addr)
{
	unsigned short val;
	m_HallBFlexRegisters.VMERead16(TS_BOARD_ADDRESS + addr, &val, FALSE);
	return val;
}

void CHallBFlexTriggerGUIDlg::OnBFirmware() 
{
	CFileDialog dlgFile(TRUE, "dat", "*.rbf", OFN_OVERWRITEPROMPT, "*.rbf", this);

	if(dlgFile.DoModal() == IDOK)
	{
		if(!m_HallBFlexRegisters.ProgramModule(dlgFile.GetPathName()))
		{
			CString errorMsg;

			errorMsg.Format("ERROR: unable to program file %s", dlgFile.GetPathName());
			MessageBox(errorMsg);
		}
		else
		{
			m_HallBFlexRegisters.VMEWrite16(TS_BOARD_ADDRESS + 0x8016, 1);
			Sleep(2000);
			m_HallBFlexRegisters.ReadBoardRegisters();

			UpdateTabs();
		}
	}
}

void CHallBFlexTriggerGUIDlg::UpdateTabs()
{
	int i;
	for(i = 0; i < 6; i++)
		m_dlgSectors[i].UpdateControls();
	for(i = 0; i < 11; i++)
		m_dlgTriggers[i].UpdateControls();
	m_dlgTrigger12.UpdateControls();
}
