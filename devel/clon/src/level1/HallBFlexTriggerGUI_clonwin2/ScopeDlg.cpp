// ScopeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "HallBFlexTriggerGUI.h"
#include "HallBFlexTriggerGUIDlg.h"
#include "ScopeDlg.h"
#include "HallBFlexRegisters.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TRIGPATTERN_0		0
#define TRIGPATTERN_1		1
#define TRIGPATTERN_X		2

#define TRIGGER_CAP_LEN		128

#define	TEXT_HEIGHT			12

#define TRIG_POLLRATE_MS	100
#define TRIG_LOOPS			1000
#define SCOPE_MIN_X			125
#define SCOPE_MAX_X			677

#define SIG_LOW				0x1
#define SIG_HIGH			0x2
#define SIG_TRANS			0x4

/////////////////////////////////////////////////////////////////////////////
// ScopeDlg dialog


ScopeDlg::ScopeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ScopeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(ScopeDlg)
	m_stCursorTime = _T("");
	//}}AFX_DATA_INIT
}


void ScopeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ScopeDlg)
	DDX_Control(pDX, IDC_CH_PERSIST, m_chPersist);
	DDX_Control(pDX, IDC_SCROLL_SCOPE, m_scrollScope);
	DDX_Text(pDX, IDC_ST_CURSORTIME, m_stCursorTime);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ScopeDlg, CDialog)
	//{{AFX_MSG_MAP(ScopeDlg)
	ON_WM_VSCROLL()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_B_SCOPECONTINUOUS, OnBScopecontinuous)
	ON_BN_CLICKED(IDC_B_SCOPESINGLE, OnBScopesingle)
	ON_BN_CLICKED(IDC_B_SCOPESTOP, OnBScopestop)
	ON_BN_CLICKED(IDC_B_SCOPECLEAR, OnBScopeclear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ScopeDlg message handlers

void ScopeDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int CurPos = m_scrollScope.GetScrollPos();

	// Determine the new position of scroll box.
	switch (nSBCode)
	{
	case SB_LEFT:      // Scroll to far left.
		CurPos = 0;
		break;

	case SB_RIGHT:      // Scroll to far right.
		CurPos = 63;
		break;

	case SB_ENDSCROLL:   // End scroll.
		break;

	case SB_LINELEFT:      // Scroll left.
		if (CurPos > 0)
			CurPos--;
		break;

	case SB_LINERIGHT:   // Scroll right.
		if (CurPos < 63)
			CurPos++;
		break;

	case SB_PAGELEFT:    // Scroll one page left.
		{
			// Get the page size. 
			SCROLLINFO   info;
			m_scrollScope.GetScrollInfo(&info, SIF_ALL);
   
			if (CurPos > 0)
				CurPos = max(0, CurPos - (int) info.nPage);
		}
		break;

	case SB_PAGERIGHT:      // Scroll one page right
		{
			// Get the page size. 
			SCROLLINFO   info;
			m_scrollScope.GetScrollInfo(&info, SIF_ALL);

			if (CurPos < 63)
				CurPos = min(63, CurPos + (int) info.nPage);
		}
		break;

	case SB_THUMBPOSITION: // Scroll to absolute position. nPos is the position
		CurPos = nPos;      // of the scroll box at the end of the drag operation.
		break;

	case SB_THUMBTRACK:   // Drag scroll box to specified position. nPos is the
		CurPos = nPos;     // position that the scroll box has been dragged to.
		break;
	}

	// Set the new position of the thumb (scroll box).
	m_scrollScope.SetScrollPos(CurPos);
	Invalidate();

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void ScopeDlg::OnTimer(UINT nIDEvent) 
{
	KillTimer(0);

	unsigned short status;
	HallBFlexRegisters *pFlexRegs = &(((CHallBFlexTriggerGUIDlg *)GetParent())->m_HallBFlexRegisters);
	if(pFlexRegs->VMERead16(V1495_VMEADDRESS + TS_TRIG_STATUS, &status, FALSE))
	{
		if(status & 0x2)
		{
			ReadoutScope();
			if(m_bTriggerContinuous)
			{
				pFlexRegs->VMEWrite16(V1495_VMEADDRESS + TS_TRIG_STATUS, 0x0000, FALSE);
				pFlexRegs->VMEWrite16(V1495_VMEADDRESS + TS_TRIG_STATUS, 0x0001, FALSE);
				SetTimer(0, TRIG_POLLRATE_MS, NULL);
			}
			else
			{
				GetDlgItem(IDC_B_SCOPECONTINUOUS)->EnableWindow(TRUE);
				GetDlgItem(IDC_B_SCOPESINGLE)->EnableWindow(TRUE);
				GetDlgItem(IDC_B_SCOPESTOP)->EnableWindow(FALSE);
			}
			DrawScope(m_chPersist.GetCheck());
		}
		else
			SetTimer(0, TRIG_POLLRATE_MS, NULL);
	}
	else
	{
		GetDlgItem(IDC_B_SCOPECONTINUOUS)->EnableWindow(TRUE);
		GetDlgItem(IDC_B_SCOPESINGLE)->EnableWindow(TRUE);
		GetDlgItem(IDC_B_SCOPESTOP)->EnableWindow(FALSE);
		pFlexRegs->DebugMsg("ERROR: Scope Trigger Failed to Read Status");
	}
	
	CDialog::OnTimer(nIDEvent);
}

void ScopeDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(nFlags & MK_LBUTTON)
		UpdateCursor(point.x, point.y, TRUE, TRUE);
	
	CDialog::OnMouseMove(nFlags, point);
}

void ScopeDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if( (point.x > 40) && (point.x < 87) && (point.y > 50) && (point.y < 464) )
	{
		int TrigPatternHit = (point.y - 50) / (TEXT_HEIGHT+1) + m_scrollScope.GetScrollPos();
		m_iTriggerPattern[TrigPatternHit] = (m_iTriggerPattern[TrigPatternHit] + 1) % 3;

		CFont scopeText;
		scopeText.CreateFont(TEXT_HEIGHT, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 0,
							 OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
							 DEFAULT_PITCH | FF_ROMAN, "Times New Roman");

		CClientDC dc(this);

		dc.SelectObject(&scopeText);
		dc.SetBkColor(::GetSysColor(COLOR_3DFACE));

		CString ch;
		if(m_iTriggerPattern[TrigPatternHit] == TRIGPATTERN_0)
			ch = "0 ";
		else if(m_iTriggerPattern[TrigPatternHit] == TRIGPATTERN_1)
			ch = "1 ";
		else
			ch = "X ";

		dc.TextOut(65, 51+(TrigPatternHit-m_scrollScope.GetScrollPos())*(TEXT_HEIGHT+1), ch);
		scopeText.DeleteObject();

		UpdateTriggerMasks();
	}

	if(nFlags & MK_LBUTTON)
		UpdateCursor(point.x, point.y, TRUE, TRUE);
	
	CDialog::OnLButtonDown(nFlags, point);
}

BOOL ScopeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	int i, sig = 0;
	for(i = 1; i <= 24; i++) m_sSignalNames[sig++].Format("ST%d", i);
	for(i = 1; i <= 6; i++) m_sSignalNames[sig++].Format("TOF%d", i);
	for(i = 1; i <= 6; i++) m_sSignalNames[sig++].Format("ECinP%d", i);
	for(i = 1; i <= 6; i++) m_sSignalNames[sig++].Format("ECtotP%d", i);
	for(i = 1; i <= 6; i++) m_sSignalNames[sig++].Format("ECinE%d", i);
	for(i = 1; i <= 6; i++) m_sSignalNames[sig++].Format("ECtotE%d", i);
	for(i = 1; i <= 6; i++) m_sSignalNames[sig++].Format("CC%d", i);
	m_sSignalNames[sig++] = "MORA";
	m_sSignalNames[sig++] = "MORB";
	for(i = 1; i <= 6; i++) m_sSignalNames[sig++].Format("STOF%d", i);
	for(i = 1; i <= 6; i++) m_sSignalNames[sig++].Format("ECP%d", i);
	for(i = 1; i <= 6; i++) m_sSignalNames[sig++].Format("ECC%d", i);
	m_sSignalNames[sig++] = "STMULT";
	for(i = 1; i <= 12; i++) m_sSignalNames[sig++].Format("TRIG%d", i);

	for(i = 0; i < 96; i++) m_iTriggerPattern[i] = TRIGPATTERN_X;
	
	m_scrollScope.SetScrollRange(0, 63);
	m_scrollScope.SetScrollPos(0);

	memset(m_iTriggerBuffer, 0, sizeof(m_iTriggerBuffer));

	m_iCursorSample = TRIGGER_CAP_LEN / 2;
	m_stCursorTime.Format("Cursor = %dns", (int)((m_iCursorSample - TRIGGER_CAP_LEN / 2) * CLOCK_PERIOD_NS));
	UpdateData(FALSE);

	GetDlgItem(IDC_B_SCOPESTOP)->EnableWindow(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void ScopeDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CFont scopeText;
	int i;

	scopeText.CreateFont(TEXT_HEIGHT, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 0,
	                     OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
						 DEFAULT_PITCH | FF_ROMAN, "Times New Roman");


	DrawScope();

	dc.SelectObject(&scopeText);
	COLORREF bkcolor = ::GetSysColor(COLOR_3DFACE);

	dc.SetBkColor(bkcolor);

	int p = 0;
	for(i = m_scrollScope.GetScrollPos(); i < m_scrollScope.GetScrollPos()+32; i++)
		dc.TextOut(10, 51+(p++)*(TEXT_HEIGHT+1), m_sSignalNames[i]);

	p = 0;
	for(i = m_scrollScope.GetScrollPos(); i < m_scrollScope.GetScrollPos()+32; i++)
	{
		CString ch;
		if(m_iTriggerPattern[i] == TRIGPATTERN_0)
			ch = "0";
		else if(m_iTriggerPattern[i] == TRIGPATTERN_1)
			ch = "1";
		else
			ch = "X";

		dc.TextOut(65, 51+(p++)*(TEXT_HEIGHT+1), ch);
	}

	for(i = 0; i < 32; i++)
	{
		dc.MoveTo(0, 50+i*(TEXT_HEIGHT+1));
		dc.LineTo(125, 50+i*(TEXT_HEIGHT+1)); 
	}
	dc.MoveTo(50, 20); dc.LineTo(50, 466);
	dc.MoveTo(84, 20); dc.LineTo(84, 466);

	scopeText.DeleteObject();
}

void ScopeDlg::DrawScope(BOOL bDrawTraceOnly)
{
	CClientDC dc(this);
	int i, y = 50;

	if(!bDrawTraceOnly)
	{
		for(i = 0; i < 16; i++)
		{
			if(m_scrollScope.GetScrollPos() & 0x1)
			{
				dc.FillSolidRect(125, y, SCOPE_MAX_X-SCOPE_MIN_X, TEXT_HEIGHT+1, RGB(0,64,0)); y+= TEXT_HEIGHT+1;
				dc.FillSolidRect(125, y, SCOPE_MAX_X-SCOPE_MIN_X, TEXT_HEIGHT+1, RGB(0,0,0)); y+= TEXT_HEIGHT+1;
			}
			else
			{
				dc.FillSolidRect(125, y, SCOPE_MAX_X-SCOPE_MIN_X, TEXT_HEIGHT+1, RGB(0,0,0)); y+= TEXT_HEIGHT+1;
				dc.FillSolidRect(125, y, SCOPE_MAX_X-SCOPE_MIN_X, TEXT_HEIGHT+1, RGB(0,64,0)); y+= TEXT_HEIGHT+1;
			}
		}
	}

	CPen traceG;
	traceG.CreatePen(PS_SOLID, 1, RGB(0,255,0));
	dc.SelectObject(&traceG);

	y = 0;
	for(i = m_scrollScope.GetScrollPos(); i < m_scrollScope.GetScrollPos() + 32; i++)
	{
		for(int j = 0; j < TRIGGER_CAP_LEN; j++)
		{
			if(m_iTriggerBuffer[i][j] & SIG_LOW)
			{
				dc.MoveTo(SCOPE_MIN_X+j*(SCOPE_MAX_X-SCOPE_MIN_X)/(TRIGGER_CAP_LEN-1), 51+y*(TEXT_HEIGHT+1)+TEXT_HEIGHT-2);
				dc.LineTo(SCOPE_MIN_X+(j+1)*(SCOPE_MAX_X-SCOPE_MIN_X)/(TRIGGER_CAP_LEN-1), 51+y*(TEXT_HEIGHT+1)+TEXT_HEIGHT-2);
			}
			if(m_iTriggerBuffer[i][j] & SIG_HIGH)
			{
				dc.MoveTo(SCOPE_MIN_X+j*(SCOPE_MAX_X-SCOPE_MIN_X)/(TRIGGER_CAP_LEN-1), 51+y*(TEXT_HEIGHT+1));
				dc.LineTo(SCOPE_MIN_X+(j+1)*(SCOPE_MAX_X-SCOPE_MIN_X)/(TRIGGER_CAP_LEN-1), 51+y*(TEXT_HEIGHT+1));
			}
			if(m_iTriggerBuffer[i][j] & SIG_TRANS)
			{
				dc.MoveTo(SCOPE_MIN_X+j*(SCOPE_MAX_X-SCOPE_MIN_X)/(TRIGGER_CAP_LEN-1), 51+y*(TEXT_HEIGHT+1)+TEXT_HEIGHT-2);
				dc.LineTo(SCOPE_MIN_X+j*(SCOPE_MAX_X-SCOPE_MIN_X)/(TRIGGER_CAP_LEN-1), 51+y*(TEXT_HEIGHT+1));
			}
		}
		y++;
	}
	if(!bDrawTraceOnly)
	{
		CPen traceR;
		traceR.CreatePen(PS_SOLID, 1, RGB(255,0,0));
		dc.SelectObject(&traceR);
		float cursorX = SCOPE_MIN_X+(TRIGGER_CAP_LEN/2.0f+0.5f)*(SCOPE_MAX_X-SCOPE_MIN_X)/(TRIGGER_CAP_LEN-1);
		dc.MoveTo((int)cursorX, 51); dc.LineTo((int)cursorX, 464); 

		UpdateCursor(0, 0, FALSE, FALSE);
	}
}

void ScopeDlg::UpdateCursor(int x, int y, BOOL updatePosition, BOOL eraseold)
{
	int i;
	CClientDC dc(this);
	float cursorX = SCOPE_MIN_X+(m_iCursorSample+0.5f)*(SCOPE_MAX_X-SCOPE_MIN_X)/(TRIGGER_CAP_LEN-1);
	CFont scopeText;
	scopeText.CreateFont(TEXT_HEIGHT, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 0,
	                     OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
						 DEFAULT_PITCH | FF_ROMAN, "Times New Roman");

	dc.SelectObject(&scopeText);
	COLORREF bkcolor = ::GetSysColor(COLOR_3DFACE);
	dc.SetBkColor(bkcolor);
	CString str;

	if(eraseold)
	{
		for(i = 50; i <= 466; i++)
			dc.SetPixel((int)cursorX, i, dc.GetPixel((int)cursorX, i) ^ RGB(0,0,255));
	}

	if( updatePosition && (x > SCOPE_MIN_X) && (x < SCOPE_MAX_X) && (y > 50) && (y < 464) )
		m_iCursorSample = (x - SCOPE_MIN_X) * TRIGGER_CAP_LEN / (SCOPE_MAX_X - SCOPE_MIN_X);

	cursorX = SCOPE_MIN_X+(m_iCursorSample+0.5f)*(SCOPE_MAX_X-SCOPE_MIN_X)/(TRIGGER_CAP_LEN-1);
	for(i = 50; i <= 466; i++)
		dc.SetPixel((int)cursorX, i, dc.GetPixel((int)cursorX, i) ^ RGB(0,0,255));

	int line = 0;
	for(i = m_scrollScope.GetScrollPos(); i < m_scrollScope.GetScrollPos() + 32; i++)
	{
		if( (m_iTriggerBuffer[i][m_iCursorSample] & SIG_LOW) && (m_iTriggerBuffer[i][m_iCursorSample] & SIG_HIGH))
			str = "0/1 ";
		else if(m_iTriggerBuffer[i][m_iCursorSample] & SIG_HIGH)
			str = "1    ";
		else if(m_iTriggerBuffer[i][m_iCursorSample] & SIG_LOW)
			str = "0    ";
		else
			str = "X    ";
		dc.TextOut(90, 51+line*(TEXT_HEIGHT+1), str);
		line++;
	}
	m_stCursorTime.Format("Cursor = %dns", (int)((m_iCursorSample - TRIGGER_CAP_LEN / 2) * CLOCK_PERIOD_NS));
	UpdateData(FALSE);
}

void ScopeDlg::OnBScopecontinuous() 
{
	m_bTriggerContinuous = TRUE;
	if(UpdateTriggerMasks())
	{
		GetDlgItem(IDC_B_SCOPECONTINUOUS)->EnableWindow(FALSE);
		GetDlgItem(IDC_B_SCOPESINGLE)->EnableWindow(FALSE);
		GetDlgItem(IDC_B_SCOPESTOP)->EnableWindow(TRUE);

		HallBFlexRegisters *pFlexRegs = &(((CHallBFlexTriggerGUIDlg *)GetParent())->m_HallBFlexRegisters);
		pFlexRegs->VMEWrite16(V1495_VMEADDRESS + TS_TRIG_STATUS, 0x0000, FALSE);
		pFlexRegs->VMEWrite16(V1495_VMEADDRESS + TS_TRIG_STATUS, 0x0001, FALSE);

		SetTimer(0, TRIG_POLLRATE_MS, NULL);
	}
}

void ScopeDlg::OnBScopesingle() 
{
	m_bTriggerContinuous = FALSE;
	if(UpdateTriggerMasks())
	{
		GetDlgItem(IDC_B_SCOPECONTINUOUS)->EnableWindow(FALSE);
		GetDlgItem(IDC_B_SCOPESINGLE)->EnableWindow(FALSE);
		GetDlgItem(IDC_B_SCOPESTOP)->EnableWindow(TRUE);

		HallBFlexRegisters *pFlexRegs = &(((CHallBFlexTriggerGUIDlg *)GetParent())->m_HallBFlexRegisters);
		pFlexRegs->VMEWrite16(V1495_VMEADDRESS + TS_TRIG_STATUS, 0x0000, FALSE);
		pFlexRegs->VMEWrite16(V1495_VMEADDRESS + TS_TRIG_STATUS, 0x0001, FALSE);

		SetTimer(0, TRIG_POLLRATE_MS, NULL);
	}
}

void ScopeDlg::OnBScopestop()
{
	GetDlgItem(IDC_B_SCOPECONTINUOUS)->EnableWindow(TRUE);
	GetDlgItem(IDC_B_SCOPESINGLE)->EnableWindow(TRUE);
	GetDlgItem(IDC_B_SCOPESTOP)->EnableWindow(FALSE);
	KillTimer(0);
}

void ScopeDlg::OnBScopeclear() 
{
	memset(m_iTriggerBuffer, 0, sizeof(m_iTriggerBuffer));
	Invalidate();
}

BOOL ScopeDlg::UpdateTriggerMasks()
{
	unsigned short bit_masks[6], ignore_masks[6];

	memset(bit_masks, 0, sizeof(bit_masks));
	memset(ignore_masks, 0, sizeof(ignore_masks));
	for(int i = 0; i < 96; i++)
	{
		int word = i / 16;
		int bit = i % 16;

		if(m_iTriggerPattern[i] == TRIGPATTERN_X)
			ignore_masks[word] |= 1<<bit;
		else if(m_iTriggerPattern[i] == TRIGPATTERN_1)
			bit_masks[word] |= 1<<bit;
	}

	HallBFlexRegisters *pFlexRegs = &(((CHallBFlexTriggerGUIDlg *)GetParent())->m_HallBFlexRegisters);
	
	if(!pFlexRegs->VMEWrite16(V1495_VMEADDRESS + TS_TRIG_VALUE2_H, bit_masks[5])) return FALSE;
	if(!pFlexRegs->VMEWrite16(V1495_VMEADDRESS + TS_TRIG_VALUE2_L, bit_masks[4])) return FALSE;
	if(!pFlexRegs->VMEWrite16(V1495_VMEADDRESS + TS_TRIG_VALUE1_H, bit_masks[3])) return FALSE;
	if(!pFlexRegs->VMEWrite16(V1495_VMEADDRESS + TS_TRIG_VALUE1_L, bit_masks[2])) return FALSE;
	if(!pFlexRegs->VMEWrite16(V1495_VMEADDRESS + TS_TRIG_VALUE0_H, bit_masks[1])) return FALSE;
	if(!pFlexRegs->VMEWrite16(V1495_VMEADDRESS + TS_TRIG_VALUE0_L, bit_masks[0])) return FALSE;

	if(!pFlexRegs->VMEWrite16(V1495_VMEADDRESS + TS_TRIG_INGORE2_H, ignore_masks[5])) return FALSE;
	if(!pFlexRegs->VMEWrite16(V1495_VMEADDRESS + TS_TRIG_INGORE2_L, ignore_masks[4])) return FALSE;
	if(!pFlexRegs->VMEWrite16(V1495_VMEADDRESS + TS_TRIG_INGORE1_H, ignore_masks[3])) return FALSE;
	if(!pFlexRegs->VMEWrite16(V1495_VMEADDRESS + TS_TRIG_INGORE1_L, ignore_masks[2])) return FALSE;
	if(!pFlexRegs->VMEWrite16(V1495_VMEADDRESS + TS_TRIG_INGORE0_H, ignore_masks[1])) return FALSE;
	if(!pFlexRegs->VMEWrite16(V1495_VMEADDRESS + TS_TRIG_INGORE0_L, ignore_masks[0])) return FALSE;

	return TRUE;
}

BOOL ScopeDlg::ReadoutScope()
{
	unsigned short bits[6], lastbits[6];
	HallBFlexRegisters *pFlexRegs = &(((CHallBFlexTriggerGUIDlg *)GetParent())->m_HallBFlexRegisters);

	unsigned short ScopeTraces[768];
	unsigned short *pScopeTraces = ScopeTraces;

	for(int i = 0; i < 128; i++)
		pFlexRegs->VMEBlkRead16(V1495_VMEADDRESS + TS_TRIG_BUFFER2_H, 6, &ScopeTraces[i*6]);

//	if(pFlexRegs->ReadScope(ScopeTraces))
	{
		for(int i = 0; i < 128; i++)
		{
			bits[5] = *pScopeTraces++;
			bits[4] = *pScopeTraces++;
			bits[3] = *pScopeTraces++;
			bits[2] = *pScopeTraces++;
			bits[1] = *pScopeTraces++;
			bits[0] = *pScopeTraces++;

			for(int j = 0; j < 96; j++)
			{
				int signal, lastsignal;
				int bit = j % 16;

				signal = bits[j/16] & (1<<bit);
				if(i)
					lastsignal = lastbits[j/16] & (1<<bit);

				if(!m_chPersist.GetCheck())
					m_iTriggerBuffer[j][i] = 0;

				if(signal)
				{
					m_iTriggerBuffer[j][i] |= SIG_HIGH;
					if(i && !lastsignal)
						m_iTriggerBuffer[j][i] |= SIG_TRANS;
				}
				else
				{
					m_iTriggerBuffer[j][i] |= SIG_LOW;
					if(i && lastsignal)
						m_iTriggerBuffer[j][i] |= SIG_TRANS;
				}
			}
			memcpy(lastbits, bits, sizeof(bits));
		}
		return TRUE;
	}
	return FALSE;
}
