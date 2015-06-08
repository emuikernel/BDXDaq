// HallBFlexRegisters.cpp: implementation of the HallBFlexRegisters class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HallBFlexTriggerGUI.h"
#include "HallBFlexTriggerGUIDlg.h"
#include "DebugDlg.h"
#include "HallBFlexRegisters.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HallBFlexRegisters::HallBFlexRegisters()
{
	m_iREF_Scaler = 0;
	for(int i = 0; i < 12; i++)
	{
		if(i < 3)
		{
			m_iICElectron_Scaler[i] = 0;
			m_iICTotal_Scaler[i] = 0;
			m_iICVeto_Scaler[i] = 0;
		}
		if(i < 6)
		{
			m_iTOF_Delay[i] = 0;
			m_iECinE_Delay[i] = 0;
			m_iECtotE_Delay[i] = 0;
			m_iECinP_Delay[i] = 0;
			m_iECtotP_Delay[i] = 0;
			m_iCC_Delay[i] = 0;
			m_iE_ECinE_Delay[i] = 0;
			m_iE_ECtotE_Delay[i] = 0;
			m_iE_CC_Delay[i] = 0;

			m_iECC_Mode[i] = ECC_AND;

			m_iTOF_Scaler[i] = 0;
			m_iECinP_Scaler[i] = 0;
			m_iECtotP_Scaler[i] = 0;
			m_iECinE_Scaler[i] = 0;
			m_iECtotE_Scaler[i] = 0;
			m_iCC_Scaler[i] = 0;
			m_iECE_Scaler[i] = 0;
			m_iECP_Scaler[i] = 0;
			m_iECC_Scaler[i] = 0;
			m_iESector_Scaler[i] = 0;
		}
		if(i < 11)
		{
			m_iECCLUT_Scaler[i] = 0;
			m_iECPLUT_Scaler[i] = 0;
			m_iECELUT_Scaler[i] = 0;
		}
		if(i < 12)
		{
			m_iTrig_Prescale[i] = 0;
			m_iTRIG_Scaler[i] = 0;
		}
	}
	m_iIC_Delay = 0;
	m_pDbgDlg = NULL;
	m_bConnected = FALSE;
	m_bDebugAll = FALSE;

	m_pSocket = NULL;
}

HallBFlexRegisters::~HallBFlexRegisters()
{

}

CString HallBFlexRegisters::LoadLUT(int trig, int table, CString filename)
{
//	CHallBFlexTriggerGUIDlg *pFlexGUIDlg = (CHallBFlexTriggerGUIDlg *)GetParent();
	CFileDialog dlgFile(TRUE, "lut", "*.lut", OFN_OVERWRITEPROMPT, "*.lut");//, pFlexGUIDlg);
	CString file("not loaded");

	if(!VMEWrite16(TS_BOARD_ADDRESS + TS_TRIG_SEL, (1<<trig)))
		return file;

	int lutSize = 0;
	unsigned int addr = TS_BOARD_ADDRESS;
	switch(table)
	{
		case LUT_ECP:
			lutSize = 4096;
			addr = TS_BOARD_ADDRESS + TS_TRIG_LUT_ECP;
			break;
		case LUT_ECE:
			lutSize = 4096;
			addr = TS_BOARD_ADDRESS + TS_TRIG_LUT_ECE;
			break;
		case LUT_TRIG:
			lutSize = 4096;
			addr = TS_BOARD_ADDRESS + TS_TRIG_LUT_TRIG;
			break;
		case LUT_ECC:
			lutSize = 64;
			addr = TS_BOARD_ADDRESS + TS_TRIG_LUT_ECC;
			break;
		default:
			return file;
	}

	if(!filename.GetLength())
	{
		if(dlgFile.DoModal() == IDOK)
			filename = dlgFile.GetPathName();
		else
			return file;
	}

	CString str;
	CStdioFile loadFile;
	CFileException exception;

	if(!loadFile.Open(filename, CFile::modeRead, &exception))
	{
		str = "Error opening file: ";
		str += filename;
		str += "Reason: ";
		str += exception.m_cause;
		AfxMessageBox(str);
		return file;
	}

	unsigned short LUT[256];
	memset(LUT, 0, sizeof(LUT));

	int bit = 0;
	while(loadFile.ReadString(str))
	{
		str.TrimLeft(" \t");
		if(str[0] == '1' || str[0] == '0')
		{
			if(bit < lutSize)
			{
				if(str[0] == '1')
					LUT[bit>>4] |= (1<<(bit&0xF));

			}
			bit++;
		}
	}
	loadFile.Close();

	if(bit != lutSize)
	{
		AfxMessageBox("Error: Too many LUT bits in file");
		return file;
	}
/*		else
		pFlexGUIDlg->DebugMsg("SUCCES: LUT bit file ready okay");*/

//UpdateData(FALSE);

	for(int i = 0; i < (lutSize + 15) >> 4; i++)
	{
		if(!VMEWrite16(addr, LUT[i]))
			return file;
		addr+= 2;
	}
	file.Format(filename);

	switch(table)
	{
		case LUT_ECP:
			m_stECPLUTNames[trig] = filename;
			break;
		case LUT_ECE:
			m_stECELUTNames[trig] = filename;
			break;
		case LUT_TRIG:
			m_stTRIGLUTNames[trig] = filename;
			break;
		case LUT_ECC:
			m_stECCLUTNames[trig] = filename;
			break;
	}
	return file;
}

void HallBFlexRegisters::SetDebugDlg(void *pDbgDlg)
{
	m_pDbgDlg = pDbgDlg;
}

void HallBFlexRegisters::IncECCMode(int sector, BOOL change)
{
	if(change)
	{
		switch(m_iECC_Mode[sector])
		{
			case ECC_AND:	m_iECC_Mode[sector] = ECC_ECE; break;
			case ECC_ECE:	m_iECC_Mode[sector] = ECC_CC; break;
			case ECC_CC:	m_iECC_Mode[sector] = ECC_HI; break;
			case ECC_HI:	m_iECC_Mode[sector] = ECC_LO; break;
			case ECC_LO:	m_iECC_Mode[sector] = ECC_AND; break;
		}
	}
	VMEWrite16(TS_BOARD_ADDRESS + TS_SECTOR1_CFG + sector * 2, m_iECC_Mode[sector]);
}

void HallBFlexRegisters::IncPrescale(int trigger, int step)
{
	if(m_iTrig_Prescale[trigger] + step > MAX_PRESCALE)
		m_iTrig_Prescale[trigger] = MAX_PRESCALE;
	else if(m_iTrig_Prescale[trigger] + step < 0)
		m_iTrig_Prescale[trigger] = 0;
	else
		m_iTrig_Prescale[trigger] += step;

	VMEWrite16(trigger * 2 + TS_PRESCALE_TRIG1 + TS_BOARD_ADDRESS, m_iTrig_Prescale[trigger]);
}

void HallBFlexRegisters::IncTOFDelay(int sector, int step)
{
	if(m_iTOF_Delay[sector]+step > MAX_DELAY)
		m_iTOF_Delay[sector] = MAX_DELAY;
	else if(m_iTOF_Delay[sector]+step < 0)
		m_iTOF_Delay[sector] = 0;
	else
		m_iTOF_Delay[sector] += step;

	VMEWrite16(sector * 2 + TS_TOF1_DELAY + TS_BOARD_ADDRESS, m_iTOF_Delay[sector]);
}

void HallBFlexRegisters::IncECinEDelay(int sector, int step)
{
	if(m_iECinE_Delay[sector]+step > MAX_DELAY)
		m_iECinE_Delay[sector] = MAX_DELAY;
	else if(m_iECinE_Delay[sector]+step < 0)
		m_iECinE_Delay[sector] = 0;
	else
		m_iECinE_Delay[sector] += step;

	VMEWrite16(sector * 2 + TS_ECin1E_DELAY + TS_BOARD_ADDRESS, m_iECinE_Delay[sector]);
}

void HallBFlexRegisters::IncECtotEDelay(int sector, int step)
{
	if(m_iECtotE_Delay[sector]+step > MAX_DELAY)
		m_iECtotE_Delay[sector] = MAX_DELAY;
	else if(m_iECtotE_Delay[sector]+step < 0)
		m_iECtotE_Delay[sector] = 0;
	else
		m_iECtotE_Delay[sector] += step;

	VMEWrite16(sector * 2 + TS_ECtot1E_DELAY + TS_BOARD_ADDRESS, m_iECtotE_Delay[sector]);
}

void HallBFlexRegisters::IncECinPDelay(int sector, int step)
{
	if(m_iECinP_Delay[sector]+step > MAX_DELAY)
		m_iECinP_Delay[sector] = MAX_DELAY;
	else if(m_iECinP_Delay[sector]+step < 0)
		m_iECinP_Delay[sector] = 0;
	else
		m_iECinP_Delay[sector] += step;

	VMEWrite16(sector * 2 + TS_ECin1P_DELAY + TS_BOARD_ADDRESS, m_iECinP_Delay[sector]);
}

void HallBFlexRegisters::IncECtotPDelay(int sector, int step)
{
	if(m_iECtotP_Delay[sector]+step > MAX_DELAY)
		m_iECtotP_Delay[sector] = MAX_DELAY;
	else if(m_iECtotP_Delay[sector]+step < 0)
		m_iECtotP_Delay[sector] = 0;
	else
		m_iECtotP_Delay[sector] += step;

	VMEWrite16(sector * 2 + TS_ECtot1P_DELAY + TS_BOARD_ADDRESS, m_iECtotP_Delay[sector]);
}

void HallBFlexRegisters::IncCCDelay(int sector, int step)
{
	if(m_iCC_Delay[sector]+step > MAX_DELAY)
		m_iCC_Delay[sector] = MAX_DELAY;
	else if(m_iCC_Delay[sector]+step < 0)
		m_iCC_Delay[sector] = 0;
	else
		m_iCC_Delay[sector] += step;

	VMEWrite16(sector * 2 + TS_CC1_DELAY + TS_BOARD_ADDRESS, m_iCC_Delay[sector]);
}

void HallBFlexRegisters::IncE_CCDelay(int sector, int step)
{
	if(m_iE_CC_Delay[sector]+step > MAX_DELAY)
		m_iE_CC_Delay[sector] = MAX_DELAY;
	else if(m_iE_CC_Delay[sector]+step < 0)
		m_iE_CC_Delay[sector] = 0;
	else
		m_iE_CC_Delay[sector] += step;

	VMEWrite16(sector * 2 + TS_E_CC1_DELAY + TS_BOARD_ADDRESS, m_iE_CC_Delay[sector]);
}

void HallBFlexRegisters::IncE_ECinEDelay(int sector, int step)
{
	if(m_iE_ECinE_Delay[sector]+step > MAX_DELAY)
		m_iE_ECinE_Delay[sector] = MAX_DELAY;
	else if(m_iE_ECinE_Delay[sector]+step < 0)
		m_iE_ECinE_Delay[sector] = 0;
	else
		m_iE_ECinE_Delay[sector] += step;

	VMEWrite16(sector * 2 + TS_E_ECin1E_DELAY + TS_BOARD_ADDRESS, m_iE_ECinE_Delay[sector]);
}

void HallBFlexRegisters::IncE_ECtotEDelay(int sector, int step)
{
	if(m_iE_ECtotE_Delay[sector]+step > MAX_DELAY)
		m_iE_ECtotE_Delay[sector] = MAX_DELAY;
	else if(m_iE_ECtotE_Delay[sector]+step < 0)
		m_iE_ECtotE_Delay[sector] = 0;
	else
		m_iE_ECtotE_Delay[sector] += step;

	VMEWrite16(sector * 2 + TS_E_ECtot1E_DELAY + TS_BOARD_ADDRESS, m_iE_ECtotE_Delay[sector]);
}

void HallBFlexRegisters::IncICDelay(int step)
{
	if(m_iIC_Delay+step > MAX_DELAY)
		m_iIC_Delay = MAX_DELAY;
	else if(m_iIC_Delay+step < 0)
		m_iIC_Delay = 0;
	else
		m_iIC_Delay += step;

	VMEWrite16(TS_IC_DELAY + TS_BOARD_ADDRESS, m_iIC_Delay);
}


BOOL HallBFlexRegisters::VMEBlkRead16(unsigned int addr, unsigned short count, unsigned short *vals)
{
	if(!m_bConnected)
	{
		DebugMsg("ERROR: VME not initialized");
		return FALSE;
	}
	Cmd_BlkRead16 *pCmd_BlkRead16;
	OutgoingMsg.Length = htonl(REMOTE_MSG_SIZE(Cmd_BlkRead16)-4);
	OutgoingMsg.BoardType = htonl(BOARD_TYPE_DVCSQ);
	OutgoingMsg.MsgType = htonl(REMOTE_CMD_BLKREAD16);
	pCmd_BlkRead16 = (Cmd_BlkRead16 *)OutgoingMsg.Msg;
	pCmd_BlkRead16->Address = htonl(addr);
	pCmd_BlkRead16->Count = htons(count);
	if(!m_pSocket->Send((void *)&OutgoingMsg, REMOTE_MSG_SIZE(Cmd_BlkRead16)))//sizeof(Cmd_BlkRead16)+1))
	{
		DebugMsg("ERROR: remote VMERead16 send() failed");
		DisconnectVME();
		return FALSE;
	}

	if( (m_pSocket->Receive((void *)&IncomingMsg.Length, 4) == 4) &&
		(ntohl(IncomingMsg.Length) <= sizeof(RemoteMsgStruct)) &&
		((unsigned long)m_pSocket->Receive((void *)&IncomingMsg.BoardType, ntohl(IncomingMsg.Length)) == ntohl(IncomingMsg.Length)) )
	{
		IncomingMsg.Length = ntohl(IncomingMsg.Length);
		IncomingMsg.BoardType = ntohl(IncomingMsg.BoardType);
		IncomingMsg.MsgType = ntohl(IncomingMsg.MsgType);
		if(IncomingMsg.MsgType == CMD_RSP(REMOTE_CMD_BLKREAD16))
		{
			unsigned short *p = (unsigned short *)IncomingMsg.Msg;
			for(int i = 0; i < count; i++)
			{
				*vals++ = ntohs(*p);
				p++;
			}
			return TRUE;
		}
	}
	DebugMsg("ERROR: remote VMEBLKRead16 failed");
	DisconnectVME();
	return FALSE;
}

BOOL HallBFlexRegisters::VMEWrite16(unsigned int addr, unsigned short val, BOOL bDebug, BOOL bIgnoreReady)
{
	if(!bIgnoreReady && !m_bConnected)
	{
		DebugMsg("ERROR: VME not initialized");
		return FALSE;
	}
	Cmd_Write16 *pCmd_Write16;
	OutgoingMsg.Length = htonl(REMOTE_MSG_SIZE(Cmd_Write16)-4);
	OutgoingMsg.BoardType = htonl(BOARD_TYPE_DVCSQ);
	OutgoingMsg.MsgType = htonl(REMOTE_CMD_WRITE16);
	pCmd_Write16 = (Cmd_Write16 *)OutgoingMsg.Msg;
	pCmd_Write16->Address = htonl(addr);
	pCmd_Write16->Value = htons(val);
	if(!m_pSocket->Send((void *)&OutgoingMsg, REMOTE_MSG_SIZE(Cmd_Write16)))
	{
		DebugMsg("ERROR: remote VMEWrite16 send() failed");
		DisconnectVME();
		return FALSE;
	}
	if(bDebug || (m_pDbgDlg && ((DebugDlg *)m_pDbgDlg)->m_chDebugAll.GetCheck()))
	{
		CString str;
		str.Format("SUCCESS: Write 0x%04X => 0x%08X", (unsigned int)val, addr);
		DebugMsg(str);
	}
	return TRUE;
}

BOOL HallBFlexRegisters::VMERead16(unsigned int addr, unsigned short *val, BOOL bDebug, BOOL bIgnoreReady)
{
	if(!bIgnoreReady && !m_bConnected)
	{
		DebugMsg("ERROR: VME not initialized");
		return FALSE;
	}

	Cmd_Read16 *pCmd_Read16;
	OutgoingMsg.Length = htonl(REMOTE_MSG_SIZE(Cmd_Read16)-4);
	OutgoingMsg.BoardType = htonl(BOARD_TYPE_DVCSQ);
	OutgoingMsg.MsgType = htonl(REMOTE_CMD_READ16);
	pCmd_Read16 = (Cmd_Read16 *)OutgoingMsg.Msg;
	pCmd_Read16->Address = htonl(addr);
	if(!m_pSocket->Send((void *)&OutgoingMsg, REMOTE_MSG_SIZE(Cmd_Read16)))
	{
		DebugMsg("ERROR: remote VMERead16 send() failed");
		DisconnectVME();
		return FALSE;
	}

	if( (m_pSocket->Receive((void *)&IncomingMsg.Length, 4) == 4) &&
		(ntohl(IncomingMsg.Length) <= sizeof(RemoteMsgStruct)) &&
		((unsigned long)m_pSocket->Receive((void *)&IncomingMsg.BoardType, ntohl(IncomingMsg.Length)) == ntohl(IncomingMsg.Length)) )
	{
		IncomingMsg.Length = ntohl(IncomingMsg.Length);
		IncomingMsg.BoardType = ntohl(IncomingMsg.BoardType);
		IncomingMsg.MsgType = ntohl(IncomingMsg.MsgType);
		if(IncomingMsg.MsgType == CMD_RSP(REMOTE_CMD_READ16))
		{
			unsigned short *p = (unsigned short *)IncomingMsg.Msg;
			*val = ntohs(*p);

			if(bDebug || (m_pDbgDlg && ((DebugDlg *)m_pDbgDlg)->m_chDebugAll.GetCheck()))
			{
				CString str;
				str.Format("SUCCESS: Read 0x%04X <= 0x%08X", (unsigned int)*val, addr);
				DebugMsg(str);
			}
			return TRUE;
		}
	}
	DebugMsg("ERROR: remote VMERead16 failed");
	DisconnectVME();
	return FALSE;
}

BOOL HallBFlexRegisters::ConnectVME(CString addr)
{
	m_bConnected = FALSE;

	m_pSocket = new CSocket;

	if(!m_pSocket->Create())
	{
		AfxMessageBox("Failed to create socket");
		PostQuitMessage(0);
	}

	if(!m_pSocket->Connect(addr, VMESERVER_PORT))
	{
		char buf[100];
		sprintf(buf, "ERROR: failed to connect to host: %50s", addr);
		DebugMsg(buf);
		return FALSE;
	}
	m_bConnected = TRUE;

	ReadBoardRegisters();

	unsigned short val;
	CString ver;
	VMERead16(TS_BOARD_ADDRESS + TS_REVISION, &val);
	ver.Format("HallBFlexTrigger Firmware Revision: V%u.%u", (unsigned int)(val>>8), (unsigned int)(val & 0xFF));
	DebugMsg(ver);

	return TRUE;
}

void HallBFlexRegisters::DisconnectVME()
{
	m_pSocket->Close();
	delete m_pSocket;
	m_pSocket = NULL;
	m_bConnected = FALSE;
}

void HallBFlexRegisters::DebugMsg(CString msg)
{
	if(m_pDbgDlg)
		((DebugDlg *)m_pDbgDlg)->DebugMsg(msg);
}

void HallBFlexRegisters::ReadBoardRegisters()
{
	int i;
	unsigned short val;

	if(!m_bConnected)
	{
		DebugMsg("ERROR: VME not initialized");
		return;
	}

	for(i = 0; i < 6; i++)
	{
		VMERead16(TS_BOARD_ADDRESS + TS_TOF1_DELAY + i * 2, &val, FALSE); m_iTOF_Delay[i] = val;
		VMERead16(TS_BOARD_ADDRESS + TS_ECin1P_DELAY + i * 2, &val, FALSE); m_iECinP_Delay[i] = val;
		VMERead16(TS_BOARD_ADDRESS + TS_ECin1E_DELAY + i * 2, &val, FALSE); m_iECinE_Delay[i] = val;
		VMERead16(TS_BOARD_ADDRESS + TS_ECtot1P_DELAY + i * 2, &val, FALSE); m_iECtotP_Delay[i] = val;
		VMERead16(TS_BOARD_ADDRESS + TS_ECtot1E_DELAY + i * 2, &val, FALSE); m_iECtotE_Delay[i] = val;
		VMERead16(TS_BOARD_ADDRESS + TS_CC1_DELAY + i * 2, &val, FALSE); m_iCC_Delay[i] = val;

		VMERead16(TS_BOARD_ADDRESS + TS_E_ECin1E_DELAY + i * 2, &val, FALSE); m_iE_ECinE_Delay[i] = val;
		VMERead16(TS_BOARD_ADDRESS + TS_E_ECtot1E_DELAY + i * 2, &val, FALSE); m_iE_ECtotE_Delay[i] = val;
		VMERead16(TS_BOARD_ADDRESS + TS_E_CC1_DELAY + i * 2, &val, FALSE); m_iE_CC_Delay[i] = val;

		VMERead16(TS_BOARD_ADDRESS + TS_SECTOR1_CFG + i * 2, &val, FALSE);
		if(val & 0x8)
			m_iECC_Mode[i] = ECC_LO;
		else if(val & 0x4)
			m_iECC_Mode[i] = ECC_HI;
		else if((val & 0x3) == 0x3)
			m_iECC_Mode[i] = ECC_AND;
		else if(val & 0x2)
			m_iECC_Mode[i] = ECC_CC;
		else if(val & 0x1)
			m_iECC_Mode[i] = ECC_ECE;
		else
			m_iECC_Mode[i] = ECC_LO;
	}
	VMERead16(TS_BOARD_ADDRESS + TS_IC_DELAY, &val, FALSE); m_iIC_Delay = val;
	for(i = 0; i < 12; i++)
	{
		VMERead16(TS_BOARD_ADDRESS + TS_PRESCALE_TRIG1 + i * 2, &val, FALSE);
		m_iTrig_Prescale[i] = val;
	}
}

BOOL HallBFlexRegisters::ReadScope(unsigned short *buf)
{
/*	unsigned int addr = TS_BOARD_ADDRESS + TS_TRIG_BUFFER3_H;
	for(int i = 0; i < 1024; i++)
	{
		if(!VMERead16(addr, &buf[i], FALSE))
			return FALSE;
		addr += 2;
	}
	return TRUE;
	*/
	return VMEBlkRead16(TS_BOARD_ADDRESS + TS_TRIG_BUFFER3_H, 1024, buf);
}

BOOL HallBFlexRegisters::ProgramModule(CString filename, unsigned int base)
{
	CStdioFile readFile;
	CFileException exception;
	CString str;

	if(!m_bConnected)
	{
		DebugMsg("ERROR: VME not initialized");
		return FALSE;
	}

	if(!readFile.Open(filename, CFile::typeBinary | CFile::modeRead, &exception))
	{
		str = "Error opening file: ";
		str += filename;
		str += "Reason: ";
		str += exception.m_cause;
		AfxMessageBox(str);

		return FALSE;
	}

	Cmd_WritePage *pCmd_WritePage = (Cmd_WritePage *)OutgoingMsg.Msg;
	pCmd_WritePage->Address = htonl(base);

	OutgoingMsg.Length = htonl(REMOTE_MSG_SIZE(Cmd_WritePage)-4);
	OutgoingMsg.BoardType = htonl(BOARD_TYPE_DVCSQ);
	OutgoingMsg.MsgType = htonl(REMOTE_CMD_WRITEPAGE);

	int page = 0;
	int nbytes = 0;
	bool quit = false;
	while(!quit)
	{
		int ch;
		if(readFile.Read(&ch, 1) != 1)
			quit = true;
		else
		{
			pCmd_WritePage->Values[nbytes] = 0;
			for(int i = 0; i < 8; i++)
			{
				if(ch & (1<<i))
					pCmd_WritePage->Values[nbytes] |= 1<<(7-i);
			}
			nbytes++;
		}
		if( (quit && nbytes) || (nbytes == PAGE_SIZE) )
		{
			pCmd_WritePage->Page = htonl(page+48);
			m_pSocket->Send((void *)&OutgoingMsg, REMOTE_MSG_SIZE(Cmd_WritePage));

			if( (m_pSocket->Receive((void *)&IncomingMsg.Length, 4) != 4) ||
				(ntohl(IncomingMsg.Length) > sizeof(RemoteMsgStruct)) ||
				((unsigned long)m_pSocket->Receive((void *)&IncomingMsg.BoardType, ntohl(IncomingMsg.Length)) != ntohl(IncomingMsg.Length)) )
			{
				AfxMessageBox("Progamming failed...");
				DisconnectVME();
				return FALSE;
			}
			IncomingMsg.Length = ntohl(IncomingMsg.Length);
			IncomingMsg.BoardType = ntohl(IncomingMsg.BoardType);
			IncomingMsg.MsgType = ntohl(IncomingMsg.MsgType);
			if(IncomingMsg.MsgType == CMD_RSP(REMOTE_CMD_WRITEPAGE))
			{
				Cmd_WritePage_Rsp *pCmd_WritePage_Rsp = (Cmd_WritePage_Rsp *)IncomingMsg.Msg;
				if(!pCmd_WritePage_Rsp->Verified)
				{
					char buf[100];
					sprintf(buf, "Programming Failed - verify error at page %d\n", page);
					AfxMessageBox(buf);
					return FALSE;
				}
			}
			page++;
			nbytes = 0;
		}
	}
	readFile.Close();

	AfxMessageBox("Progamming Completed and Verified");
	return TRUE;
}

BOOL HallBFlexRegisters::SaveRegisterSettings(CString filename)
{
	CStdioFile saveFile;
	CFileException exception;
	CString str;
	int i;

	if(!saveFile.Open(filename, CFile::modeCreate | CFile::modeWrite, &exception))
	{
		str = "Error opening file: ";
		str += filename;
		str += "Reason: ";
		str += exception.m_cause;
		AfxMessageBox(str);

		return FALSE;
	}

	CTime t = CTime::GetCurrentTime();
	saveFile.WriteString("# Hall B IC Flex Trigger Module Configuration Register Settings\n");
	saveFile.WriteString(t.Format("# Last Updated: %A, %B %d, %Y at %I:%M:%S%p\n\n"));

	for(i = 0; i < 6; i++) {	str.Format("TS_TOF%d_DELAY             %6d\n", i+1, m_iTOF_Delay[i]); saveFile.WriteString(str); }
	for(i = 0; i < 6; i++) {	str.Format("TS_ECin%dP_DELAY           %6d\n", i+1, m_iECinP_Delay[i]); saveFile.WriteString(str); }
	for(i = 0; i < 6; i++) {	str.Format("TS_ECin%dE_DELAY           %6d\n", i+1, m_iECinE_Delay[i]); saveFile.WriteString(str); }
	for(i = 0; i < 6; i++) {	str.Format("TS_ECtot%dP_DELAY          %6d\n", i+1, m_iECtotP_Delay[i]); saveFile.WriteString(str); }
	for(i = 0; i < 6; i++) {	str.Format("TS_ECtot%dE_DELAY          %6d\n", i+1, m_iECtotE_Delay[i]); saveFile.WriteString(str); }
	for(i = 0; i < 6; i++) {	str.Format("TS_CC%d_DELAY              %6d\n", i+1, m_iCC_Delay[i]); saveFile.WriteString(str); }
	for(i = 0; i < 12; i++) {	str.Format("TS_PRESCALE_TRIG%d         %6d\n", i+1, m_iTrig_Prescale[i]); saveFile.WriteString(str); }
	for(i = 0; i < 6; i++) {	str.Format("TS_E_ECin%dE_DELAY           %6d\n", i+1, m_iE_ECinE_Delay[i]); saveFile.WriteString(str); }
	for(i = 0; i < 6; i++) {	str.Format("TS_E_ECtot%dE_DELAY          %6d\n", i+1, m_iE_ECtotE_Delay[i]); saveFile.WriteString(str); }
	for(i = 0; i < 6; i++) {	str.Format("TS_E_CC%d_DELAY              %6d\n", i+1, m_iE_CC_Delay[i]); saveFile.WriteString(str); }
	str.Format("TS_IC_DELAY              %6d\n", m_iIC_Delay); saveFile.WriteString(str);

	for(i = 0; i < 6; i++)
	{
		str.Format("TS_SECTOR%d_CFG_FORCE0       %6d\n", i+1, (m_iECC_Mode[i]>>3) & 0x1); saveFile.WriteString(str);
		str.Format("TS_SECTOR%d_CFG_FORCE1       %6d\n", i+1, (m_iECC_Mode[i]>>2) & 0x1); saveFile.WriteString(str);
		str.Format("TS_SECTOR%d_CFG_ECE          %6d\n", i+1, (m_iECC_Mode[i]>>0) & 0x1); saveFile.WriteString(str);
		str.Format("TS_SECTOR%d_CFG_CC           %6d\n", i+1, (m_iECC_Mode[i]>>1) & 0x1); saveFile.WriteString(str);
	}

	for(i = 0; i < 11; i++)
	{
		if(m_stECPLUTNames[i].GetLength())	{ str.Format("TS_TRIG%d_ECPLUTFILE           \"%s\"\n", i+1, m_stECPLUTNames[i]); saveFile.WriteString(str); }
		if(m_stECELUTNames[i].GetLength())	{ str.Format("TS_TRIG%d_ECELUTFILE           \"%s\"\n", i+1, m_stECELUTNames[i]); saveFile.WriteString(str); }
		if(m_stTRIGLUTNames[i].GetLength())	{ str.Format("TS_TRIG%d_TRIGLUTFILE          \"%s\"\n", i+1, m_stTRIGLUTNames[i]); saveFile.WriteString(str); }
		if(m_stECCLUTNames[i].GetLength())	{ str.Format("TS_TRIG%d_ECCLUTFILE           \"%s\"\n", i+1, m_stECCLUTNames[i]); saveFile.WriteString(str); }
	}

	saveFile.Close();
	return TRUE;
}

BOOL HallBFlexRegisters::LoadRegisterSettings(CString filename)
{
	CStdioFile loadFile;
	CFileException exception;
	CString str;
	int i;

	if(!loadFile.Open(filename, CFile::modeRead, &exception))
	{
		str = "Error opening file: ";
		str += filename;
		str += "Reason: ";
		str += exception.m_cause;
		AfxMessageBox(str);

		return FALSE;
	}

	while(loadFile.ReadString(str))
	{
		BOOL bLastWasSpace = TRUE;
		int token = 0;
		CString token0, token1;

		str.TrimLeft(" \t");
		while(!str.IsEmpty())
		{
			if((str[0] == ' ') || (str[0] == '\t'))
			{
				if(!bLastWasSpace)
					token++;

				bLastWasSpace = TRUE;
			}
			else
			{
				bLastWasSpace = FALSE;
				if(token == 0)
					token0 += str[0];
				else if(token == 1)
					token1 += str[0];
			}
			str.Delete(0);
		}

		if(!token0.IsEmpty() && !token0.IsEmpty() && token0[0] != '#')
		{
			unsigned int val = atoi(token1);

			while(1)
			{
				if(!token0.Compare("TS_TOF1_DELAY")) { m_iTOF_Delay[0] = val; break; }
				if(!token0.Compare("TS_TOF2_DELAY")) { m_iTOF_Delay[1] = val; break; }
				if(!token0.Compare("TS_TOF3_DELAY")) { m_iTOF_Delay[2] = val; break; }
				if(!token0.Compare("TS_TOF4_DELAY")) { m_iTOF_Delay[3] = val; break; }
				if(!token0.Compare("TS_TOF5_DELAY")) { m_iTOF_Delay[4] = val; break; }
				if(!token0.Compare("TS_TOF6_DELAY")) { m_iTOF_Delay[5] = val; break; }
				if(!token0.Compare("TS_ECin1P_DELAY")) { m_iECinP_Delay[0] = val; break; }
				if(!token0.Compare("TS_ECin2P_DELAY")) { m_iECinP_Delay[1] = val; break; }
				if(!token0.Compare("TS_ECin3P_DELAY")) { m_iECinP_Delay[2] = val; break; }
				if(!token0.Compare("TS_ECin4P_DELAY")) { m_iECinP_Delay[3] = val; break; }
				if(!token0.Compare("TS_ECin5P_DELAY")) { m_iECinP_Delay[4] = val; break; }
				if(!token0.Compare("TS_ECin6P_DELAY")) { m_iECinP_Delay[5] = val; break; }
				if(!token0.Compare("TS_ECin1E_DELAY")) { m_iECinE_Delay[0] = val; break; }
				if(!token0.Compare("TS_ECin2E_DELAY")) { m_iECinE_Delay[1] = val; break; }
				if(!token0.Compare("TS_ECin3E_DELAY")) { m_iECinE_Delay[2] = val; break; }
				if(!token0.Compare("TS_ECin4E_DELAY")) { m_iECinE_Delay[3] = val; break; }
				if(!token0.Compare("TS_ECin5E_DELAY")) { m_iECinE_Delay[4] = val; break; }
				if(!token0.Compare("TS_ECin6E_DELAY")) { m_iECinE_Delay[5] = val; break; }
				if(!token0.Compare("TS_ECtot1P_DELAY")) { m_iECtotP_Delay[0] = val; break; }
				if(!token0.Compare("TS_ECtot2P_DELAY")) { m_iECtotP_Delay[1] = val; break; }
				if(!token0.Compare("TS_ECtot3P_DELAY")) { m_iECtotP_Delay[2] = val; break; }
				if(!token0.Compare("TS_ECtot4P_DELAY")) { m_iECtotP_Delay[3] = val; break; }
				if(!token0.Compare("TS_ECtot5P_DELAY")) { m_iECtotP_Delay[4] = val; break; }
				if(!token0.Compare("TS_ECtot6P_DELAY")) { m_iECtotP_Delay[5] = val; break; }
				if(!token0.Compare("TS_ECtot1E_DELAY")) { m_iECtotE_Delay[0] = val; break; }
				if(!token0.Compare("TS_ECtot2E_DELAY")) { m_iECtotE_Delay[1] = val; break; }
				if(!token0.Compare("TS_ECtot3E_DELAY")) { m_iECtotE_Delay[2] = val; break; }
				if(!token0.Compare("TS_ECtot4E_DELAY")) { m_iECtotE_Delay[3] = val; break; }
				if(!token0.Compare("TS_ECtot5E_DELAY")) { m_iECtotE_Delay[4] = val; break; }
				if(!token0.Compare("TS_ECtot6E_DELAY")) { m_iECtotE_Delay[5] = val; break; }
				if(!token0.Compare("TS_CC1_DELAY")) { m_iCC_Delay[0] = val; break; }
				if(!token0.Compare("TS_CC2_DELAY")) { m_iCC_Delay[1] = val; break; }
				if(!token0.Compare("TS_CC3_DELAY")) { m_iCC_Delay[2] = val; break; }
				if(!token0.Compare("TS_CC4_DELAY")) { m_iCC_Delay[3] = val; break; }
				if(!token0.Compare("TS_CC5_DELAY")) { m_iCC_Delay[4] = val; break; }
				if(!token0.Compare("TS_CC6_DELAY")) { m_iCC_Delay[5] = val; break; }
				if(!token0.Compare("TS_PRESCALE_TRIG1")) { m_iTrig_Prescale[0] = val; break; }
				if(!token0.Compare("TS_PRESCALE_TRIG2")) { m_iTrig_Prescale[1] = val; break; }
				if(!token0.Compare("TS_PRESCALE_TRIG3")) { m_iTrig_Prescale[2] = val; break; }
				if(!token0.Compare("TS_PRESCALE_TRIG4")) { m_iTrig_Prescale[3] = val; break; }
				if(!token0.Compare("TS_PRESCALE_TRIG5")) { m_iTrig_Prescale[4] = val; break; }
				if(!token0.Compare("TS_PRESCALE_TRIG6")) { m_iTrig_Prescale[5] = val; break; }
				if(!token0.Compare("TS_PRESCALE_TRIG7")) { m_iTrig_Prescale[6] = val; break; }
				if(!token0.Compare("TS_PRESCALE_TRIG8")) { m_iTrig_Prescale[7] = val; break; }
				if(!token0.Compare("TS_PRESCALE_TRIG9")) { m_iTrig_Prescale[8] = val; break; }
				if(!token0.Compare("TS_PRESCALE_TRIG10")) { m_iTrig_Prescale[9] = val; break; }
				if(!token0.Compare("TS_PRESCALE_TRIG11")) { m_iTrig_Prescale[10] = val; break; }
				if(!token0.Compare("TS_PRESCALE_TRIG12")) { m_iTrig_Prescale[11] = val; break; }
				if(!token0.Compare("TS_E_ECin1E_DELAY")) { m_iE_ECinE_Delay[0] = val; break; }
				if(!token0.Compare("TS_E_ECin2E_DELAY")) { m_iE_ECinE_Delay[1] = val; break; }
				if(!token0.Compare("TS_E_ECin3E_DELAY")) { m_iE_ECinE_Delay[2] = val; break; }
				if(!token0.Compare("TS_E_ECin4E_DELAY")) { m_iE_ECinE_Delay[3] = val; break; }
				if(!token0.Compare("TS_E_ECin5E_DELAY")) { m_iE_ECinE_Delay[4] = val; break; }
				if(!token0.Compare("TS_E_ECin6E_DELAY")) { m_iE_ECinE_Delay[5] = val; break; }
				if(!token0.Compare("TS_E_ECtot1E_DELAY")) { m_iE_ECtotE_Delay[0] = val; break; }
				if(!token0.Compare("TS_E_ECtot2E_DELAY")) { m_iE_ECtotE_Delay[1] = val; break; }
				if(!token0.Compare("TS_E_ECtot3E_DELAY")) { m_iE_ECtotE_Delay[2] = val; break; }
				if(!token0.Compare("TS_E_ECtot4E_DELAY")) { m_iE_ECtotE_Delay[3] = val; break; }
				if(!token0.Compare("TS_E_ECtot5E_DELAY")) { m_iE_ECtotE_Delay[4] = val; break; }
				if(!token0.Compare("TS_E_ECtot6E_DELAY")) { m_iE_ECtotE_Delay[5] = val; break; }
				if(!token0.Compare("TS_E_CC1_DELAY")) { m_iE_CC_Delay[0] = val; break; }
				if(!token0.Compare("TS_E_CC2_DELAY")) { m_iE_CC_Delay[1] = val; break; }
				if(!token0.Compare("TS_E_CC3_DELAY")) { m_iE_CC_Delay[2] = val; break; }
				if(!token0.Compare("TS_E_CC4_DELAY")) { m_iE_CC_Delay[3] = val; break; }
				if(!token0.Compare("TS_E_CC5_DELAY")) { m_iE_CC_Delay[4] = val; break; }
				if(!token0.Compare("TS_E_CC6_DELAY")) { m_iE_CC_Delay[5] = val; break; }
				if(!token0.Compare("TS_IC_DELAY")) { m_iIC_Delay = val; break; }
				if(!token0.Compare("TS_SECTOR1_CFG_FORCE0")) { m_iECC_Mode[0] = (m_iECC_Mode[0] & ~ECC_LO) | ((val & 0x1)<<3); break; }
				if(!token0.Compare("TS_SECTOR1_CFG_FORCE1")) { m_iECC_Mode[0] = (m_iECC_Mode[0] & ~ECC_HI) | ((val & 0x1)<<2); break; }
				if(!token0.Compare("TS_SECTOR1_CFG_ECE")) { m_iECC_Mode[0] = (m_iECC_Mode[0] & ~ECC_ECE) | ((val & 0x1)<<0); break; }
				if(!token0.Compare("TS_SECTOR1_CFG_CC")) { m_iECC_Mode[0] = (m_iECC_Mode[0] & ~ECC_CC) | ((val & 0x1)<<1); break; }
				if(!token0.Compare("TS_SECTOR2_CFG_FORCE0")) { m_iECC_Mode[1] = (m_iECC_Mode[1] & ~ECC_LO) | ((val & 0x1)<<3); break; }
				if(!token0.Compare("TS_SECTOR2_CFG_FORCE1")) { m_iECC_Mode[1] = (m_iECC_Mode[1] & ~ECC_HI) | ((val & 0x1)<<2); break; }
				if(!token0.Compare("TS_SECTOR2_CFG_ECE")) { m_iECC_Mode[1] = (m_iECC_Mode[1] & ~ECC_ECE) | ((val & 0x1)<<0); break; }
				if(!token0.Compare("TS_SECTOR2_CFG_CC")) { m_iECC_Mode[1] = (m_iECC_Mode[1] & ~ECC_CC) | ((val & 0x1)<<1); break; }
				if(!token0.Compare("TS_SECTOR3_CFG_FORCE0")) { m_iECC_Mode[2] = (m_iECC_Mode[2] & ~ECC_LO) | ((val & 0x1)<<3); break; }
				if(!token0.Compare("TS_SECTOR3_CFG_FORCE1")) { m_iECC_Mode[2] = (m_iECC_Mode[2] & ~ECC_HI) | ((val & 0x1)<<2); break; }
				if(!token0.Compare("TS_SECTOR3_CFG_ECE")) { m_iECC_Mode[2] = (m_iECC_Mode[2] & ~ECC_ECE) | ((val & 0x1)<<0); break; }
				if(!token0.Compare("TS_SECTOR3_CFG_CC")) { m_iECC_Mode[2] = (m_iECC_Mode[2] & ~ECC_CC) | ((val & 0x1)<<1); break; }
				if(!token0.Compare("TS_SECTOR4_CFG_FORCE0")) { m_iECC_Mode[3] = (m_iECC_Mode[3] & ~ECC_LO) | ((val & 0x1)<<3); break; }
				if(!token0.Compare("TS_SECTOR4_CFG_FORCE1")) { m_iECC_Mode[3] = (m_iECC_Mode[3] & ~ECC_HI) | ((val & 0x1)<<2); break; }
				if(!token0.Compare("TS_SECTOR4_CFG_ECE")) { m_iECC_Mode[3] = (m_iECC_Mode[3] & ~ECC_ECE) | ((val & 0x1)<<0); break; }
				if(!token0.Compare("TS_SECTOR4_CFG_CC")) { m_iECC_Mode[3] = (m_iECC_Mode[3] & ~ECC_CC) | ((val & 0x1)<<1); break; }
				if(!token0.Compare("TS_SECTOR5_CFG_FORCE0")) { m_iECC_Mode[4] = (m_iECC_Mode[4] & ~ECC_LO) | ((val & 0x1)<<3); break; }
				if(!token0.Compare("TS_SECTOR5_CFG_FORCE1")) { m_iECC_Mode[4] = (m_iECC_Mode[4] & ~ECC_HI) | ((val & 0x1)<<2); break; }
				if(!token0.Compare("TS_SECTOR5_CFG_ECE")) { m_iECC_Mode[4] = (m_iECC_Mode[4] & ~ECC_ECE) | ((val & 0x1)<<0); break; }
				if(!token0.Compare("TS_SECTOR5_CFG_CC")) { m_iECC_Mode[4] = (m_iECC_Mode[4] & ~ECC_CC) | ((val & 0x1)<<1); break; }
				if(!token0.Compare("TS_SECTOR6_CFG_FORCE0")) { m_iECC_Mode[5] = (m_iECC_Mode[5] & ~ECC_LO) | ((val & 0x1)<<3); break; }
				if(!token0.Compare("TS_SECTOR6_CFG_FORCE1")) { m_iECC_Mode[5] = (m_iECC_Mode[5] & ~ECC_HI) | ((val & 0x1)<<2); break; }
				if(!token0.Compare("TS_SECTOR6_CFG_ECE")) { m_iECC_Mode[5] = (m_iECC_Mode[5] & ~ECC_ECE) | ((val & 0x1)<<0); break; }
				if(!token0.Compare("TS_SECTOR6_CFG_CC")) { m_iECC_Mode[5] = (m_iECC_Mode[5] & ~ECC_CC) | ((val & 0x1)<<1); break; }
				if(!token0.Compare("TS_TRIG1_ECPLUTFILE")) { LoadLUT(0, LUT_ECP, token1); break; }
				if(!token0.Compare("TS_TRIG2_ECPLUTFILE")) { LoadLUT(1, LUT_ECP, token1); break; }
				if(!token0.Compare("TS_TRIG3_ECPLUTFILE")) { LoadLUT(2, LUT_ECP, token1); break; }
				if(!token0.Compare("TS_TRIG4_ECPLUTFILE")) { LoadLUT(3, LUT_ECP, token1); break; }
				if(!token0.Compare("TS_TRIG5_ECPLUTFILE")) { LoadLUT(4, LUT_ECP, token1); break; }
				if(!token0.Compare("TS_TRIG6_ECPLUTFILE")) { LoadLUT(5, LUT_ECP, token1); break; }
				if(!token0.Compare("TS_TRIG7_ECPLUTFILE")) { LoadLUT(6, LUT_ECP, token1); break; }
				if(!token0.Compare("TS_TRIG8_ECPLUTFILE")) { LoadLUT(7, LUT_ECP, token1); break; }
				if(!token0.Compare("TS_TRIG9_ECPLUTFILE")) { LoadLUT(8, LUT_ECP, token1); break; }
				if(!token0.Compare("TS_TRIG10_ECPLUTFILE")) { LoadLUT(9, LUT_ECP, token1); break; }
				if(!token0.Compare("TS_TRIG11_ECPLUTFILE")) { LoadLUT(10, LUT_ECP, token1); break; }
				if(!token0.Compare("TS_TRIG1_ECELUTFILE")) { LoadLUT(0, LUT_ECE, token1); break; }
				if(!token0.Compare("TS_TRIG2_ECELUTFILE")) { LoadLUT(1, LUT_ECE, token1); break; }
				if(!token0.Compare("TS_TRIG3_ECELUTFILE")) { LoadLUT(2, LUT_ECE, token1); break; }
				if(!token0.Compare("TS_TRIG4_ECELUTFILE")) { LoadLUT(3, LUT_ECE, token1); break; }
				if(!token0.Compare("TS_TRIG5_ECELUTFILE")) { LoadLUT(4, LUT_ECE, token1); break; }
				if(!token0.Compare("TS_TRIG6_ECELUTFILE")) { LoadLUT(5, LUT_ECE, token1); break; }
				if(!token0.Compare("TS_TRIG7_ECELUTFILE")) { LoadLUT(6, LUT_ECE, token1); break; }
				if(!token0.Compare("TS_TRIG8_ECELUTFILE")) { LoadLUT(7, LUT_ECE, token1); break; }
				if(!token0.Compare("TS_TRIG9_ECELUTFILE")) { LoadLUT(8, LUT_ECE, token1); break; }
				if(!token0.Compare("TS_TRIG10_ECELUTFILE")) { LoadLUT(9, LUT_ECE, token1); break; }
				if(!token0.Compare("TS_TRIG11_ECELUTFILE")) { LoadLUT(10, LUT_ECE, token1); break; }
				if(!token0.Compare("TS_TRIG1_ECCLUTFILE")) { LoadLUT(0, LUT_ECC, token1); break; }
				if(!token0.Compare("TS_TRIG2_ECCLUTFILE")) { LoadLUT(1, LUT_ECC, token1); break; }
				if(!token0.Compare("TS_TRIG3_ECCLUTFILE")) { LoadLUT(2, LUT_ECC, token1); break; }
				if(!token0.Compare("TS_TRIG4_ECCLUTFILE")) { LoadLUT(3, LUT_ECC, token1); break; }
				if(!token0.Compare("TS_TRIG5_ECCLUTFILE")) { LoadLUT(4, LUT_ECC, token1); break; }
				if(!token0.Compare("TS_TRIG6_ECCLUTFILE")) { LoadLUT(5, LUT_ECC, token1); break; }
				if(!token0.Compare("TS_TRIG7_ECCLUTFILE")) { LoadLUT(6, LUT_ECC, token1); break; }
				if(!token0.Compare("TS_TRIG8_ECCLUTFILE")) { LoadLUT(7, LUT_ECC, token1); break; }
				if(!token0.Compare("TS_TRIG9_ECCLUTFILE")) { LoadLUT(8, LUT_ECC, token1); break; }
				if(!token0.Compare("TS_TRIG10_ECCLUTFILE")) { LoadLUT(9, LUT_ECC, token1); break; }
				if(!token0.Compare("TS_TRIG11_ECCLUTFILE")) { LoadLUT(10, LUT_ECC, token1); break; }
				if(!token0.Compare("TS_TRIG1_TRIGLUTFILE")) { LoadLUT(0, LUT_TRIG, token1); break; }
				if(!token0.Compare("TS_TRIG2_TRIGLUTFILE")) { LoadLUT(1, LUT_TRIG, token1); break; }
				if(!token0.Compare("TS_TRIG3_TRIGLUTFILE")) { LoadLUT(2, LUT_TRIG, token1); break; }
				if(!token0.Compare("TS_TRIG4_TRIGLUTFILE")) { LoadLUT(3, LUT_TRIG, token1); break; }
				if(!token0.Compare("TS_TRIG5_TRIGLUTFILE")) { LoadLUT(4, LUT_TRIG, token1); break; }
				if(!token0.Compare("TS_TRIG6_TRIGLUTFILE")) { LoadLUT(5, LUT_TRIG, token1); break; }
				if(!token0.Compare("TS_TRIG7_TRIGLUTFILE")) { LoadLUT(6, LUT_TRIG, token1); break; }
				if(!token0.Compare("TS_TRIG8_TRIGLUTFILE")) { LoadLUT(7, LUT_TRIG, token1); break; }
				if(!token0.Compare("TS_TRIG9_TRIGLUTFILE")) { LoadLUT(8, LUT_TRIG, token1); break; }
				if(!token0.Compare("TS_TRIG10_TRIGLUTFILE")) { LoadLUT(9, LUT_TRIG, token1); break; }
				if(!token0.Compare("TS_TRIG11_TRIGLUTFILE")) { LoadLUT(10, LUT_TRIG, token1); break; }

				str = "ERROR: invalid Register/Value: " + token0 + "/" + token1;
				DebugMsg(str);
				loadFile.Close();
				return FALSE;
			}
			str = "SUCCESS: Set Register: " + token0 + " = " + token1;
			DebugMsg(str);
		}

	}
	loadFile.Close();

	for(i = 0; i < 6; i++)
	{
		IncICDelay(0);
		IncCCDelay(i, 0);
		IncECtotPDelay(i, 0);
		IncECinPDelay(i, 0);
		IncECtotEDelay(i, 0);
		IncECinEDelay(i, 0);
		IncTOFDelay(i, 0);
		IncE_CCDelay(i, 0);
		IncE_ECtotEDelay(i, 0);
		IncE_ECinEDelay(i, 0);
		IncECCMode(i, FALSE);
	}
	for(i = 0; i < 12; i++)
		IncPrescale(i, 0);
 
	ReadBoardRegisters();
	return TRUE;
}
