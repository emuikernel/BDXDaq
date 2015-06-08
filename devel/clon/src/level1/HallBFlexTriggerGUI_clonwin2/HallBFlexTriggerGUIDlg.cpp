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
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHallBFlexTriggerGUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHallBFlexTriggerGUIDlg)
	DDX_Control(pDX, IDC_SL_SCALERPERIOD, m_slScalerPeriod);
	DDX_Control(pDX, IDC_TAB_TRIGMAIN, m_tabTrigMain);
	DDX_Control(pDX, IDC_IPADDRESS, m_ipAddress);
	DDX_Control(pDX, IDC_CH_REMOTECONNECT, m_chRemoteConnect);
	DDX_Text(pDX, IDC_ST_SCALERPERIOD, m_stScalerPeriod);
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
	for(i = 0; i < 12; i++)
	{
		m_dlgTriggers[i].Create(IDD_TRIGGER_DIALOG);
		m_dlgTriggers[i].SetTriggerNum(i);
		str.Format("Trig %d", i+1);
		m_tabTrigMain.InsertItem(id, str);
		m_pCDialogs[id++] = &m_dlgTriggers[i];
	}

	m_tabTrigMain.InsertItem(id, "Testing");
	m_pCDialogs[id++] = &m_dlgTest;

	m_tabTrigMain.SetCurSel(0);

	OnSelchangeTabTrigmain(NULL, NULL);
	GetDlgItem(IDC_B_DISCONNECT)->EnableWindow(FALSE);

	m_ipAddress.SetAddress(129, 57, 68, 97);
	m_chRemoteConnect.SetCheck(TRUE);

	m_slScalerPeriod.SetRange(1, 60);
	m_slScalerPeriod.SetPos(10);

	m_HallBFlexRegisters.SetDebugDlg((void *)&m_dlgDebug);

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
	unsigned char b0,b1,b2,b3;
	m_ipAddress.GetAddress(b0,b1,b2,b3);

	CString ipAddr;
	ipAddr.Format("%d.%d.%d.%d", (int)b0, (int)b1, (int)b2, (int)b3);

	if(!m_HallBFlexRegisters.ConnectVME(m_chRemoteConnect.GetCheck(), ipAddr))
		return;

	GetDlgItem(IDC_B_DISCONNECT)->EnableWindow(TRUE);
	GetDlgItem(IDC_B_CONNECT)->EnableWindow(FALSE);
	GetDlgItem(IDC_IPADDRESS)->EnableWindow(FALSE);
	GetDlgItem(IDC_CH_REMOTECONNECT)->EnableWindow(FALSE);

	int i;
	for(i = 0; i < 6; i++)
		m_dlgSectors[i].UpdateControls();
	for(i = 0; i < 12; i++)
		m_dlgTriggers[i].UpdateControls();
}

void CHallBFlexTriggerGUIDlg::OnBDisconnect() 
{
	m_HallBFlexRegisters.DisconnectVME();

	GetDlgItem(IDC_B_DISCONNECT)->EnableWindow(FALSE);
	GetDlgItem(IDC_B_CONNECT)->EnableWindow(TRUE);
	GetDlgItem(IDC_IPADDRESS)->EnableWindow(TRUE);
	GetDlgItem(IDC_CH_REMOTECONNECT)->EnableWindow(TRUE);
}

void CHallBFlexTriggerGUIDlg::OnBLoadsettings() 
{
	/*
	#define V895_VMEADDRESS1	0x60000000
	#define V895_VMEADDRESS2	0x70000000

	m_HallBFlexRegisters.VMEWrite16(V895_VMEADDRESS1 + 0x4A, 0xFFFF);	// Enable Channels
	m_HallBFlexRegisters.VMEWrite16(V895_VMEADDRESS2 + 0x4A, 0xFFFF);	// Enable Channels
	for(int i = 0; i < 16; i++)
	{
		m_HallBFlexRegisters.VMEWrite16(V895_VMEADDRESS1 + i*2, 128);	// Threshold ~-128mV
		m_HallBFlexRegisters.VMEWrite16(V895_VMEADDRESS2 + i*2, 128);	// Threshold ~-128mV
	}
	m_HallBFlexRegisters.VMEWrite16(V895_VMEADDRESS1 + 0x40, 0xFF);		// Pulse Width
	m_HallBFlexRegisters.VMEWrite16(V895_VMEADDRESS2 + 0x40, 0xFF);		// Pulse Width
	m_HallBFlexRegisters.VMEWrite16(V895_VMEADDRESS1 + 0x42, 0xFF);		// Pulse Width
	m_HallBFlexRegisters.VMEWrite16(V895_VMEADDRESS2 + 0x42, 0xFF);		// Pulse Width
	*/
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
		m_HallBFlexRegisters.VMEWrite16(V1495_VMEADDRESS + TS_ENABLE_SCALERS, 0x0000, FALSE);

		m_HallBFlexRegisters.m_iMORA_Scaler = ReadScaler32(TS_MORA_SCALER_H);
		m_HallBFlexRegisters.m_iMORB_Scaler = ReadScaler32(TS_MORB_SCALER_H);
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
		m_HallBFlexRegisters.m_iST_Scaler[0] = ReadScaler32(TS_ST1_SCALER_H);
		m_HallBFlexRegisters.m_iST_Scaler[1] = ReadScaler32(TS_ST2_SCALER_H);
		m_HallBFlexRegisters.m_iST_Scaler[2] = ReadScaler32(TS_ST3_SCALER_H);
		m_HallBFlexRegisters.m_iST_Scaler[3] = ReadScaler32(TS_ST4_SCALER_H);
		m_HallBFlexRegisters.m_iST_Scaler[4] = ReadScaler32(TS_ST5_SCALER_H);
		m_HallBFlexRegisters.m_iST_Scaler[5] = ReadScaler32(TS_ST6_SCALER_H);
		m_HallBFlexRegisters.m_iST_Scaler[6] = ReadScaler32(TS_ST7_SCALER_H);
		m_HallBFlexRegisters.m_iST_Scaler[7] = ReadScaler32(TS_ST8_SCALER_H);
		m_HallBFlexRegisters.m_iST_Scaler[8] = ReadScaler32(TS_ST9_SCALER_H);
		m_HallBFlexRegisters.m_iST_Scaler[9] = ReadScaler32(TS_ST10_SCALER_H);
		m_HallBFlexRegisters.m_iST_Scaler[10] = ReadScaler32(TS_ST11_SCALER_H);
		m_HallBFlexRegisters.m_iST_Scaler[11] = ReadScaler32(TS_ST12_SCALER_H);
		m_HallBFlexRegisters.m_iST_Scaler[12] = ReadScaler32(TS_ST13_SCALER_H);
		m_HallBFlexRegisters.m_iST_Scaler[13] = ReadScaler32(TS_ST14_SCALER_H);
		m_HallBFlexRegisters.m_iST_Scaler[14] = ReadScaler32(TS_ST15_SCALER_H);
		m_HallBFlexRegisters.m_iST_Scaler[15] = ReadScaler32(TS_ST16_SCALER_H);
		m_HallBFlexRegisters.m_iST_Scaler[16] = ReadScaler32(TS_ST17_SCALER_H);
		m_HallBFlexRegisters.m_iST_Scaler[17] = ReadScaler32(TS_ST18_SCALER_H);
		m_HallBFlexRegisters.m_iST_Scaler[18] = ReadScaler32(TS_ST19_SCALER_H);
		m_HallBFlexRegisters.m_iST_Scaler[19] = ReadScaler32(TS_ST20_SCALER_H);
		m_HallBFlexRegisters.m_iST_Scaler[20] = ReadScaler32(TS_ST21_SCALER_H);
		m_HallBFlexRegisters.m_iST_Scaler[21] = ReadScaler32(TS_ST22_SCALER_H);
		m_HallBFlexRegisters.m_iST_Scaler[22] = ReadScaler32(TS_ST23_SCALER_H);
		m_HallBFlexRegisters.m_iST_Scaler[23] = ReadScaler32(TS_ST24_SCALER_H);
		m_HallBFlexRegisters.m_iLUT_Scaler[0] = ReadScaler16(TS_LUT_SCALER_TRIG1);
		m_HallBFlexRegisters.m_iLUT_Scaler[1] = ReadScaler16(TS_LUT_SCALER_TRIG2);
		m_HallBFlexRegisters.m_iLUT_Scaler[2] = ReadScaler16(TS_LUT_SCALER_TRIG3);
		m_HallBFlexRegisters.m_iLUT_Scaler[3] = ReadScaler16(TS_LUT_SCALER_TRIG4);
		m_HallBFlexRegisters.m_iLUT_Scaler[4] = ReadScaler16(TS_LUT_SCALER_TRIG5);
		m_HallBFlexRegisters.m_iLUT_Scaler[5] = ReadScaler16(TS_LUT_SCALER_TRIG6);
		m_HallBFlexRegisters.m_iLUT_Scaler[6] = ReadScaler16(TS_LUT_SCALER_TRIG7);
		m_HallBFlexRegisters.m_iLUT_Scaler[7] = ReadScaler16(TS_LUT_SCALER_TRIG8);
		m_HallBFlexRegisters.m_iLUT_Scaler[8] = ReadScaler16(TS_LUT_SCALER_TRIG9);
		m_HallBFlexRegisters.m_iLUT_Scaler[9] = ReadScaler16(TS_LUT_SCALER_TRIG10);
		m_HallBFlexRegisters.m_iLUT_Scaler[10] = ReadScaler16(TS_LUT_SCALER_TRIG11);
		m_HallBFlexRegisters.m_iLUT_Scaler[11] = ReadScaler16(TS_LUT_SCALER_TRIG12);
		m_HallBFlexRegisters.m_iLUTMOR_Scaler[0] = ReadScaler16(TS_LUT_MOR_SCALER_TRIG1);
		m_HallBFlexRegisters.m_iLUTMOR_Scaler[1] = ReadScaler16(TS_LUT_MOR_SCALER_TRIG2);
		m_HallBFlexRegisters.m_iLUTMOR_Scaler[2] = ReadScaler16(TS_LUT_MOR_SCALER_TRIG3);
		m_HallBFlexRegisters.m_iLUTMOR_Scaler[3] = ReadScaler16(TS_LUT_MOR_SCALER_TRIG4);
		m_HallBFlexRegisters.m_iLUTMOR_Scaler[4] = ReadScaler16(TS_LUT_MOR_SCALER_TRIG5);
		m_HallBFlexRegisters.m_iLUTMOR_Scaler[5] = ReadScaler16(TS_LUT_MOR_SCALER_TRIG6);
		m_HallBFlexRegisters.m_iLUTMOR_Scaler[6] = ReadScaler16(TS_LUT_MOR_SCALER_TRIG7);
		m_HallBFlexRegisters.m_iLUTMOR_Scaler[7] = ReadScaler16(TS_LUT_MOR_SCALER_TRIG8);
		m_HallBFlexRegisters.m_iLUTMOR_Scaler[8] = ReadScaler16(TS_LUT_MOR_SCALER_TRIG9);
		m_HallBFlexRegisters.m_iLUTMOR_Scaler[9] = ReadScaler16(TS_LUT_MOR_SCALER_TRIG10);
		m_HallBFlexRegisters.m_iLUTMOR_Scaler[10] = ReadScaler16(TS_LUT_MOR_SCALER_TRIG11);
		m_HallBFlexRegisters.m_iLUTMOR_Scaler[11] = ReadScaler16(TS_LUT_MOR_SCALER_TRIG12);
		m_HallBFlexRegisters.m_iTrigPersist_Scaler[0] = ReadScaler16(TS_TRIG_PERSIST_SCALER_TRIG1);
		m_HallBFlexRegisters.m_iTrigPersist_Scaler[1] = ReadScaler16(TS_TRIG_PERSIST_SCALER_TRIG2);
		m_HallBFlexRegisters.m_iTrigPersist_Scaler[2] = ReadScaler16(TS_TRIG_PERSIST_SCALER_TRIG3);
		m_HallBFlexRegisters.m_iTrigPersist_Scaler[3] = ReadScaler16(TS_TRIG_PERSIST_SCALER_TRIG4);
		m_HallBFlexRegisters.m_iTrigPersist_Scaler[4] = ReadScaler16(TS_TRIG_PERSIST_SCALER_TRIG5);
		m_HallBFlexRegisters.m_iTrigPersist_Scaler[5] = ReadScaler16(TS_TRIG_PERSIST_SCALER_TRIG6);
		m_HallBFlexRegisters.m_iTrigPersist_Scaler[6] = ReadScaler16(TS_TRIG_PERSIST_SCALER_TRIG7);
		m_HallBFlexRegisters.m_iTrigPersist_Scaler[7] = ReadScaler16(TS_TRIG_PERSIST_SCALER_TRIG8);
		m_HallBFlexRegisters.m_iTrigPersist_Scaler[8] = ReadScaler16(TS_TRIG_PERSIST_SCALER_TRIG9);
		m_HallBFlexRegisters.m_iTrigPersist_Scaler[9] = ReadScaler16(TS_TRIG_PERSIST_SCALER_TRIG10);
		m_HallBFlexRegisters.m_iTrigPersist_Scaler[10] = ReadScaler16(TS_TRIG_PERSIST_SCALER_TRIG11);
		m_HallBFlexRegisters.m_iTrigPersist_Scaler[11] = ReadScaler16(TS_TRIG_PERSIST_SCALER_TRIG12);
		m_HallBFlexRegisters.m_iTrigPrescale_Scaler[0] = ReadScaler16(TS_TRIG_PRESCALE_SCALER_TRIG1);
		m_HallBFlexRegisters.m_iTrigPrescale_Scaler[1] = ReadScaler16(TS_TRIG_PRESCALE_SCALER_TRIG2);
		m_HallBFlexRegisters.m_iTrigPrescale_Scaler[2] = ReadScaler16(TS_TRIG_PRESCALE_SCALER_TRIG3);
		m_HallBFlexRegisters.m_iTrigPrescale_Scaler[3] = ReadScaler16(TS_TRIG_PRESCALE_SCALER_TRIG4);
		m_HallBFlexRegisters.m_iTrigPrescale_Scaler[4] = ReadScaler16(TS_TRIG_PRESCALE_SCALER_TRIG5);
		m_HallBFlexRegisters.m_iTrigPrescale_Scaler[5] = ReadScaler16(TS_TRIG_PRESCALE_SCALER_TRIG6);
		m_HallBFlexRegisters.m_iTrigPrescale_Scaler[6] = ReadScaler16(TS_TRIG_PRESCALE_SCALER_TRIG7);
		m_HallBFlexRegisters.m_iTrigPrescale_Scaler[7] = ReadScaler16(TS_TRIG_PRESCALE_SCALER_TRIG8);
		m_HallBFlexRegisters.m_iTrigPrescale_Scaler[8] = ReadScaler16(TS_TRIG_PRESCALE_SCALER_TRIG9);
		m_HallBFlexRegisters.m_iTrigPrescale_Scaler[9] = ReadScaler16(TS_TRIG_PRESCALE_SCALER_TRIG10);
		m_HallBFlexRegisters.m_iTrigPrescale_Scaler[10] = ReadScaler16(TS_TRIG_PRESCALE_SCALER_TRIG11);
		m_HallBFlexRegisters.m_iTrigPrescale_Scaler[11] = ReadScaler16(TS_TRIG_PRESCALE_SCALER_TRIG12);
		m_HallBFlexRegisters.m_iSTS_Scaler[0] = ReadScaler32(TS_STS1_SCALER_H);
		m_HallBFlexRegisters.m_iSTS_Scaler[1] = ReadScaler32(TS_STS2_SCALER_H);
		m_HallBFlexRegisters.m_iSTS_Scaler[2] = ReadScaler32(TS_STS3_SCALER_H);
		m_HallBFlexRegisters.m_iSTS_Scaler[3] = ReadScaler32(TS_STS4_SCALER_H);
		m_HallBFlexRegisters.m_iSTS_Scaler[4] = ReadScaler32(TS_STS5_SCALER_H);
		m_HallBFlexRegisters.m_iSTS_Scaler[5] = ReadScaler32(TS_STS6_SCALER_H);
		m_HallBFlexRegisters.m_iSTOF_Scaler[0] = ReadScaler16(TS_STOF1_SCALER);
		m_HallBFlexRegisters.m_iSTOF_Scaler[1] = ReadScaler16(TS_STOF2_SCALER);
		m_HallBFlexRegisters.m_iSTOF_Scaler[2] = ReadScaler16(TS_STOF3_SCALER);
		m_HallBFlexRegisters.m_iSTOF_Scaler[3] = ReadScaler16(TS_STOF4_SCALER);
		m_HallBFlexRegisters.m_iSTOF_Scaler[4] = ReadScaler16(TS_STOF5_SCALER);
		m_HallBFlexRegisters.m_iSTOF_Scaler[5] = ReadScaler16(TS_STOF6_SCALER);
		m_HallBFlexRegisters.m_iECP_Scaler[0] = ReadScaler32(TS_EC1P_SCALER_H);
		m_HallBFlexRegisters.m_iECP_Scaler[1] = ReadScaler32(TS_EC2P_SCALER_H);
		m_HallBFlexRegisters.m_iECP_Scaler[2] = ReadScaler32(TS_EC3P_SCALER_H);
		m_HallBFlexRegisters.m_iECP_Scaler[3] = ReadScaler32(TS_EC4P_SCALER_H);
		m_HallBFlexRegisters.m_iECP_Scaler[4] = ReadScaler32(TS_EC5P_SCALER_H);
		m_HallBFlexRegisters.m_iECP_Scaler[5] = ReadScaler32(TS_EC6P_SCALER_H);
		m_HallBFlexRegisters.m_iECE_Scaler[0] = ReadScaler32(TS_EC1E_SCALER_H);
		m_HallBFlexRegisters.m_iECE_Scaler[1] = ReadScaler32(TS_EC2E_SCALER_H);
		m_HallBFlexRegisters.m_iECE_Scaler[2] = ReadScaler32(TS_EC3E_SCALER_H);
		m_HallBFlexRegisters.m_iECE_Scaler[3] = ReadScaler32(TS_EC4E_SCALER_H);
		m_HallBFlexRegisters.m_iECE_Scaler[4] = ReadScaler32(TS_EC5E_SCALER_H);
		m_HallBFlexRegisters.m_iECE_Scaler[5] = ReadScaler32(TS_EC6E_SCALER_H);
		m_HallBFlexRegisters.m_iECC_Scaler[0] = ReadScaler16(TS_ECC1_SCALER);
		m_HallBFlexRegisters.m_iECC_Scaler[1] = ReadScaler16(TS_ECC2_SCALER);
		m_HallBFlexRegisters.m_iECC_Scaler[2] = ReadScaler16(TS_ECC3_SCALER);
		m_HallBFlexRegisters.m_iECC_Scaler[3] = ReadScaler16(TS_ECC4_SCALER);
		m_HallBFlexRegisters.m_iECC_Scaler[4] = ReadScaler16(TS_ECC5_SCALER);
		m_HallBFlexRegisters.m_iECC_Scaler[5] = ReadScaler16(TS_ECC6_SCALER);
		m_HallBFlexRegisters.m_iREF_Scaler = ReadScaler32(TS_REF_SCALER_H);
		m_HallBFlexRegisters.m_iMOROr_Scaler = ReadScaler32(TS_MORAB_SCALER_H);

		m_HallBFlexRegisters.VMEWrite16(V1495_VMEADDRESS + TS_ENABLE_SCALERS, 0x0001, FALSE);
		DebugMsg("done.");

		int i;
		for(i = 0; i < 6; i++) m_dlgSectors[i].UpdateControls();
		for(i = 0; i < 12; i++) m_dlgTriggers[i].UpdateControls();
	}

	SetTimer(0, m_slScalerPeriod.GetPos() * 1000, NULL);
	CDialog::OnTimer(nIDEvent);
}

unsigned int CHallBFlexTriggerGUIDlg::ReadScaler32(unsigned int addr)
{
	unsigned short valh, vall;
	m_HallBFlexRegisters.VMERead16(V1495_VMEADDRESS + addr, &valh, FALSE);
	m_HallBFlexRegisters.VMERead16(V1495_VMEADDRESS + addr+2, &vall, FALSE);
	return (valh<<16)|vall;
}

unsigned int CHallBFlexTriggerGUIDlg::ReadScaler16(unsigned int addr)
{
	unsigned short val;
	m_HallBFlexRegisters.VMERead16(V1495_VMEADDRESS + addr, &val, FALSE);
	return val;
}
