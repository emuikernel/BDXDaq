// HallBFlexRegisters.cpp: implementation of the HallBFlexRegisters class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HallBFlexTriggerGUI.h"
#include "DebugDlg.h"
#include "HallBFlexRegisters.h"
#include "vm-usb_easyvme.h"

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
	m_iMORA_Delay = 0;
	m_iMORB_Delay = 0;
	memset(m_iST_Delay, 0, sizeof(m_iST_Delay));
	memset(m_iTOF_Delay, 0, sizeof(m_iTOF_Delay));
	memset(m_iECinE_Delay, 0, sizeof(m_iECinE_Delay));
	memset(m_iECtotE_Delay, 0, sizeof(m_iECtotE_Delay));
	memset(m_iECinP_Delay, 0, sizeof(m_iECinP_Delay));
	memset(m_iECtotP_Delay, 0, sizeof(m_iECtotP_Delay));
	memset(m_iCC_Delay, 0, sizeof(m_iCC_Delay));

	memset(m_iST_Scaler, 0, sizeof(m_iST_Scaler));
	memset(m_iSTS_Scaler, 0, sizeof(m_iSTS_Scaler));
	memset(m_iECE_Scaler, 0, sizeof(m_iECE_Scaler));
	memset(m_iSTOF_Scaler, 0, sizeof(m_iSTOF_Scaler));
	memset(m_iECP_Scaler, 0, sizeof(m_iECP_Scaler));
	memset(m_iECC_Scaler, 0, sizeof(m_iECC_Scaler));
	memset(m_iTOF_Scaler, 0, sizeof(m_iTOF_Scaler));
	memset(m_iECinP_Scaler, 0, sizeof(m_iECinP_Scaler));
	memset(m_iECinE_Scaler, 0, sizeof(m_iECinE_Scaler));
	memset(m_iECtotP_Scaler, 0, sizeof(m_iECtotP_Scaler));
	memset(m_iECtotE_Scaler, 0, sizeof(m_iECtotE_Scaler));
	memset(m_iCC_Scaler, 0, sizeof(m_iCC_Scaler));

	memset(m_iLUT_Scaler, 0, sizeof(m_iLUT_Scaler));
	memset(m_iLUTMOR_Scaler, 0, sizeof(m_iLUTMOR_Scaler));
	memset(m_iLUTMORST_Scaler, 0, sizeof(m_iLUTMORST_Scaler));
	memset(m_iTrigPersist_Scaler, 0, sizeof(m_iTrigPersist_Scaler));
	memset(m_iTrigPrescale_Scaler, 0, sizeof(m_iTrigPrescale_Scaler));

	m_iMult_Min = 1;
	m_iMult_Max = 24;

	m_iSTMult_Scaler = 0;
	m_iMORA_Scaler = 0;
	m_iMORB_Scaler = 0;
	m_iMOROr_Scaler = 0;

	m_iMORA_Delay = 0;
	m_iMORB_Delay = 0;

	m_iREF_Scaler = 0;

	int i;
	for(i = 0; i < 6; i++)
	{
		m_iSTOF_Mode[i] = STOF_AND;
		m_iECC_Mode[i] = ECC_AND;
	}

	for(i = 0; i < 12; i++)
	{
		m_iTrig_Persist[i] = 0;
		m_iTrig_Prescale[i] = 1;
		m_iMOR_Mode[i] = MOR_OR;
		m_iSTMult_Mode[i] = STMULT_AND;
	}
	udev = NULL;
	m_pDbgDlg = NULL;
	m_bConnected = FALSE;
	m_bDebugAll = FALSE;
}

HallBFlexRegisters::~HallBFlexRegisters()
{

}

void HallBFlexRegisters::SetDebugDlg(void *pDbgDlg)
{
	m_pDbgDlg = pDbgDlg;
}

void HallBFlexRegisters::IncSTMultMode(int trigger)
{
	switch(m_iSTMult_Mode[trigger])
	{
		case STMULT_AND: m_iSTMult_Mode[trigger] = STMULT_MOR; break;
		case STMULT_MOR: m_iSTMult_Mode[trigger] = STMULT_AND; break;
	}

	unsigned short regSTMult_DIS = 0;
	for(int i = 0; i < 12; i++)
	{
		if(m_iSTMult_Mode[trigger] == STMULT_MOR)
			regSTMult_DIS |= (1<<i);
	}
	VMEWrite16(TS_STMULT_DIS + V1495_VMEADDRESS, regSTMult_DIS);
}

void HallBFlexRegisters::IncSTOFMode(int sector)
{
	switch(m_iSTOF_Mode[sector])
	{
		case STOF_AND:	m_iSTOF_Mode[sector] = STOF_STS; break;
		case STOF_STS:	m_iSTOF_Mode[sector] = STOF_TOF; break;
		case STOF_TOF:	m_iSTOF_Mode[sector] = STOF_HI; break;
		case STOF_HI:	m_iSTOF_Mode[sector] = STOF_LO; break;
		case STOF_LO:	m_iSTOF_Mode[sector] = STOF_AND; break;
	}

	unsigned short regSTOF_EN = 0;
	unsigned short regTOF_DIS = 0;
	unsigned short regSTS_DIS = 0;
	for(int i = 0; i < 6; i++)
	{
		switch(m_iSTOF_Mode[i])
		{
			case STOF_STS:
				regSTOF_EN |= (1<<i);
				regTOF_DIS |= (1<<i);
				break;
			case STOF_TOF:
				regSTOF_EN |= (1<<i);
				regSTS_DIS |= (1<<i);
				break;
			case STOF_HI:
				regSTOF_EN |= (1<<i);
				regTOF_DIS |= (1<<i);
				regSTS_DIS |= (1<<i);
				break;
			case STOF_LO:
				break;
			case STOF_AND:
				regSTOF_EN |= (1<<i);
				break;
		}
	}
	VMEWrite16(TS_STOF_EN + V1495_VMEADDRESS, regSTOF_EN);
	VMEWrite16(TS_TOF_DIS + V1495_VMEADDRESS, regTOF_DIS);
	VMEWrite16(TS_STS_DIS + V1495_VMEADDRESS, regSTS_DIS);
}

void HallBFlexRegisters::IncECCMode(int sector)
{
	switch(m_iECC_Mode[sector])
	{
		case ECC_AND:	m_iECC_Mode[sector] = ECC_ECE; break;
		case ECC_ECE:	m_iECC_Mode[sector] = ECC_CC; break;
		case ECC_CC:	m_iECC_Mode[sector] = ECC_HI; break;
		case ECC_HI:	m_iECC_Mode[sector] = ECC_LO; break;
		case ECC_LO:	m_iECC_Mode[sector] = ECC_AND; break;
	}

	unsigned short regECE_DIS = 0;
	unsigned short regCC_DIS = 0;
	unsigned short regECC_EN = 0;
	for(int i = 0; i < 6; i++)
	{
		switch(m_iECC_Mode[i])
		{
			case ECC_AND:
				regECC_EN |= (1<<i);
				break;
			case ECC_ECE:
				regCC_DIS |= (1<<i);
				regECC_EN |= (1<<i);
				break;
			case ECC_CC:
				regECE_DIS |= (1<<i);
				regECC_EN |= (1<<i);
				break;
			case ECC_HI:
				regECE_DIS |= (1<<i);
				regCC_DIS |= (1<<i);
				regECC_EN |= (1<<i);
				break;
			case ECC_LO:
				break;
		}
	}
	VMEWrite16(TS_ECE_DIS + V1495_VMEADDRESS, regECE_DIS);
	VMEWrite16(TS_CC_DIS + V1495_VMEADDRESS, regCC_DIS);
	VMEWrite16(TS_ECC_EN + V1495_VMEADDRESS, regECC_EN);
}

void HallBFlexRegisters::IncMORMode(int trigger)
{
	switch(m_iMOR_Mode[trigger])
	{
		case MOR_OR:	m_iMOR_Mode[trigger] = MOR_MORA; break;
		case MOR_MORA:	m_iMOR_Mode[trigger] = MOR_MORB; break;
		case MOR_MORB:	m_iMOR_Mode[trigger] = MOR_HI; break;
		case MOR_HI:	m_iMOR_Mode[trigger] = MOR_OR; break;
	}

	unsigned short regMORA_EN = 0;
	unsigned short regMORB_EN = 0;
	unsigned short regMOR_DIS = 0;
	for(int i = 0; i < 12; i++)
	{
		if( (m_iMOR_Mode[i] == MOR_MORA) || (m_iMOR_Mode[i] == MOR_OR) )
			regMORA_EN |= (1<<i);
		if( (m_iMOR_Mode[i] == MOR_MORB) || (m_iMOR_Mode[i] == MOR_OR) )
			regMORB_EN |= (1<<i);
		if(m_iMOR_Mode[i] == MOR_HI)
			regMOR_DIS |= (1<<i);
	}
	VMEWrite16(TS_MORA_EN + V1495_VMEADDRESS, regMORA_EN);
	VMEWrite16(TS_MORB_EN + V1495_VMEADDRESS, regMORB_EN);
	VMEWrite16(TS_MOR_DIS + V1495_VMEADDRESS, regMOR_DIS);
}

void HallBFlexRegisters::IncMultMin(int step)
{
	if(m_iMult_Min + step > MAX_STMULT)
		m_iMult_Min = MAX_STMULT;
	else if(m_iMult_Min + step < 0)
		m_iMult_Min = 0;
	else
		m_iMult_Min += step;

	VMEWrite16(TS_STMULT_THRESHOLD + V1495_VMEADDRESS, (m_iMult_Max<<5) | m_iMult_Min);
}

void HallBFlexRegisters::IncMultMax(int step)
{
	if(m_iMult_Max + step > MAX_STMULT)
		m_iMult_Max = MAX_STMULT;
	else if(m_iMult_Max + step < 0)
		m_iMult_Max = 0;
	else
		m_iMult_Max += step;

	VMEWrite16(TS_STMULT_THRESHOLD + V1495_VMEADDRESS, (m_iMult_Max<<5) | m_iMult_Min);
}

void HallBFlexRegisters::IncPersist(int trigger, int step)
{
	if(m_iTrig_Persist[trigger] + step > MAX_PERSIST)
		m_iTrig_Persist[trigger] = MAX_PERSIST;
	else if(m_iTrig_Persist[trigger] + step < 0)
		m_iTrig_Persist[trigger] = 0;
	else
		m_iTrig_Persist[trigger] += step;

	VMEWrite16(trigger * 2 + TS_PERSIST_TRIG1 + V1495_VMEADDRESS, m_iTrig_Persist[trigger]);
}

void HallBFlexRegisters::IncPrescale(int trigger, int step)
{
	if(m_iTrig_Prescale[trigger] + step > MAX_PRESCALE)
		m_iTrig_Prescale[trigger] = MAX_PRESCALE;
	else if(m_iTrig_Prescale[trigger] + step < 0)
		m_iTrig_Prescale[trigger] = 0;
	else
		m_iTrig_Prescale[trigger] += step;

	VMEWrite16(trigger * 2 + TS_PRESCALE_TRIG1 + V1495_VMEADDRESS, m_iTrig_Prescale[trigger]);
}

void HallBFlexRegisters::IncSTDelay(int sector, int st, int step)
{
	if(m_iST_Delay[sector*4+st]+step > MAX_DELAY)
		m_iST_Delay[sector*4+st] = MAX_DELAY;
	else if(m_iST_Delay[sector*4+st]+step < 0)
		m_iST_Delay[sector*4+st] = 0;
	else
		m_iST_Delay[sector*4+st] += step;

	VMEWrite16(st * 2 + sector * 8 + TS_ST1_DELAY + V1495_VMEADDRESS, m_iST_Delay[sector*4+st]);
}

void HallBFlexRegisters::IncTOFDelay(int sector, int step)
{
	if(m_iTOF_Delay[sector]+step > MAX_DELAY)
		m_iTOF_Delay[sector] = MAX_DELAY;
	else if(m_iTOF_Delay[sector]+step < 0)
		m_iTOF_Delay[sector] = 0;
	else
		m_iTOF_Delay[sector] += step;

	VMEWrite16(sector * 2 + TS_TOF1_DELAY + V1495_VMEADDRESS, m_iTOF_Delay[sector]);
}

void HallBFlexRegisters::IncECinEDelay(int sector, int step)
{
	if(m_iECinE_Delay[sector]+step > MAX_DELAY)
		m_iECinE_Delay[sector] = MAX_DELAY;
	else if(m_iECinE_Delay[sector]+step < 0)
		m_iECinE_Delay[sector] = 0;
	else
		m_iECinE_Delay[sector] += step;

	VMEWrite16(sector * 2 + TS_ECin1E_DELAY + V1495_VMEADDRESS, m_iECinE_Delay[sector]);
}

void HallBFlexRegisters::IncECtotEDelay(int sector, int step)
{
	if(m_iECtotE_Delay[sector]+step > MAX_DELAY)
		m_iECtotE_Delay[sector] = MAX_DELAY;
	else if(m_iECtotE_Delay[sector]+step < 0)
		m_iECtotE_Delay[sector] = 0;
	else
		m_iECtotE_Delay[sector] += step;

	VMEWrite16(sector * 2 + TS_ECtot1E_DELAY + V1495_VMEADDRESS, m_iECtotE_Delay[sector]);
}

void HallBFlexRegisters::IncECinPDelay(int sector, int step)
{
	if(m_iECinP_Delay[sector]+step > MAX_DELAY)
		m_iECinP_Delay[sector] = MAX_DELAY;
	else if(m_iECinP_Delay[sector]+step < 0)
		m_iECinP_Delay[sector] = 0;
	else
		m_iECinP_Delay[sector] += step;

	VMEWrite16(sector * 2 + TS_ECin1P_DELAY + V1495_VMEADDRESS, m_iECinP_Delay[sector]);
}

void HallBFlexRegisters::IncECtotPDelay(int sector, int step)
{
	if(m_iECtotP_Delay[sector]+step > MAX_DELAY)
		m_iECtotP_Delay[sector] = MAX_DELAY;
	else if(m_iECtotP_Delay[sector]+step < 0)
		m_iECtotP_Delay[sector] = 0;
	else
		m_iECtotP_Delay[sector] += step;

	VMEWrite16(sector * 2 + TS_ECtot1P_DELAY + V1495_VMEADDRESS, m_iECtotP_Delay[sector]);
}

void HallBFlexRegisters::IncCCDelay(int sector, int step)
{
	if(m_iCC_Delay[sector]+step > MAX_DELAY)
		m_iCC_Delay[sector] = MAX_DELAY;
	else if(m_iCC_Delay[sector]+step < 0)
		m_iCC_Delay[sector] = 0;
	else
		m_iCC_Delay[sector] += step;

	VMEWrite16(sector * 2 + TS_CC1_DELAY + V1495_VMEADDRESS, m_iCC_Delay[sector]);
}

void HallBFlexRegisters::IncMORADelay(int step)
{
	if(m_iMORA_Delay+step > MAX_DELAY)
		m_iMORA_Delay = MAX_DELAY;
	else if(m_iMORA_Delay+step < 0)
		m_iMORA_Delay = 0;
	else
		m_iMORA_Delay += step;

	VMEWrite16(TS_MORA_DELAY + V1495_VMEADDRESS, m_iMORA_Delay);
}

void HallBFlexRegisters::IncMORBDelay(int step)
{
	if(m_iMORB_Delay+step > MAX_DELAY)
		m_iMORB_Delay = MAX_DELAY;
	else if(m_iMORB_Delay+step < 0)
		m_iMORB_Delay = 0;
	else
		m_iMORB_Delay += step;

	VMEWrite16(TS_MORB_DELAY + V1495_VMEADDRESS, m_iMORB_Delay);
}

BOOL HallBFlexRegisters::VMEBlkRead16(unsigned int addr, unsigned short count, unsigned short *vals)
{
	if(!m_bConnected)
	{
		DebugMsg("ERROR: VME not initialized");
		return FALSE;
	}

	if(m_bUseRemote)
	{
		Cmd_BlkRead16 *pCmdBlkRead16 = (Cmd_BlkRead16 *)&m_VMERemote.Msgs[1];

		m_VMERemote.Msgs[0] = V1495_CMD_BLKREAD16;
		pCmdBlkRead16->Offset = htons(addr & 0xFFFF);
		pCmdBlkRead16->Count = htons(count);

		if(!m_Socket.Send((void *)&m_VMERemote, sizeof(V1495VMERemote)))//sizeof(Cmd_BlkRead16)+1))
		{
			DebugMsg("ERROR: remote VMERead16 send() failed");
			DisconnectVME();
			return FALSE;
		}

		if( (m_Socket.Receive((void *)&m_VMERemote, sizeof(V1495VMERemote)) != 2*count+1) ||
			(m_VMERemote.Msgs[0] != CMD_RSP(V1495_CMD_BLKREAD16)) )
		{
			DebugMsg("ERROR: remote VMEBLKRead16 failed");
			DisconnectVME();
			return FALSE;
		}

		Cmd_BlkRead16_Rsp *pCmdBlkRead16_Rsp = (Cmd_BlkRead16_Rsp *)&m_VMERemote.Msgs[1];
		int i = 0;

		while(count--)
			*vals++ = ntohs(pCmdBlkRead16_Rsp->Values[i++]);
	}
	else
	{
		while(count--)
		{
			if(VMERead16(addr, vals++, FALSE) < 0)
			{
				DebugMsg("ERROR: blkread16 failed");
				return FALSE;
			}
			addr += 2;
		}
	}
	return TRUE;
}

BOOL HallBFlexRegisters::VMEBlkWrite16(unsigned int addr, unsigned short count, unsigned short *vals, BOOL verify)
{
	if(!m_bConnected)
	{
		DebugMsg("ERROR: VME not initialized");
		return FALSE;
	}

	if(m_bUseRemote)
	{
		Cmd_BlkWrite16 *pCmdBlkWrite16 = (Cmd_BlkWrite16 *)&m_VMERemote.Msgs[1];

		m_VMERemote.Msgs[0] = V1495_CMD_BLKWRITE16;
		pCmdBlkWrite16->Offset = htons(addr & 0xFFFF);
		pCmdBlkWrite16->Count = htons(count);
		pCmdBlkWrite16->Verify = (unsigned short)verify;
		for(int i = 0; i < count; i++)
			pCmdBlkWrite16->Values[i] = htons(vals[i]);

		if(!m_Socket.Send((void *)&m_VMERemote, sizeof(V1495VMERemote)))//sizeof(Cmd_BlkWrite16)-(BLK_MAX_LEN-count)+1))
		{
			DebugMsg("ERROR: remote BlkWrite16 send() failed");
			DisconnectVME() ;
			return FALSE;
		}
		unsigned int dummy; m_Socket.Receive(&dummy, sizeof(dummy));
/*
		if(verify)
		{
			if( (m_Socket.Receive((void *)&m_VMERemote, sizeof(V1495VMERemote)) != sizeof(Cmd_BlkWrite16_Rsp)+1) ||
				(m_VMERemote.Msgs[0] != CMD_RSP(V1495_CMD_BLKWRITE16)) )
			{
				DebugMsg("ERROR: remote BLKWrite16 failed");
				return FALSE;
			}
			
			Cmd_BlkWrite16_Rsp *pBlkWrite16_Rsp = (Cmd_BlkWrite16_Rsp *)&m_VMERemote.Msgs[1];
			if(pBlkWrite16_Rsp->Verified != V1495_VERIFY_OK)
			{
				DebugMsg("ERROR: remote BLKWrite16 Verify failed");
				return FALSE;
			}
		}*/
	}
	else
	{
		unsigned short rdval;

		for(int i = 0; i < count; i++)
		{
			if(VMEWrite16(addr, *vals, FALSE) < 0)
				return FALSE;

			if(verify)
			{
				if( (VMERead16(addr, &rdval, FALSE) < 0) || (rdval != *vals++) )
				{
					DebugMsg("ERROR: Write Failed to Verify");
					return FALSE;
				}
			}
			addr+= 2;
			vals++;
		}
	}
	return TRUE;
}

BOOL HallBFlexRegisters::VMEWrite16(unsigned int addr, unsigned short val, BOOL bDebug, BOOL bIgnoreReady)
{
	if(!bIgnoreReady && !m_bConnected)
	{
		DebugMsg("ERROR: VME not initialized");
		return FALSE;
	}

	if(m_bUseRemote)
	{
		Cmd_Write16 *pCmdWrite16 = (Cmd_Write16 *)&m_VMERemote.Msgs[1];

		m_VMERemote.Msgs[0] = V1495_CMD_WRITE16;
		pCmdWrite16->Offset = htons(addr & 0xFFFF);
		pCmdWrite16->Value = htons(val);

		if(!m_Socket.Send((void *)&m_VMERemote, sizeof(V1495VMERemote)))//sizeof(Cmd_Write16)+1))
		{
			DebugMsg("ERROR: remote VMEWrite16 send() failed");
			DisconnectVME();
			return FALSE;
		}

		unsigned int dummy; m_Socket.Receive(&dummy, sizeof(dummy));
	}
	else
	{
		if(vme_write_16(udev, 0x09, addr, val) < 0)
		{
			DebugMsg("ERROR: vme_write_16 failed");
			DisconnectVME();
			return FALSE;
		}
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

	if(m_bUseRemote)
	{
		Cmd_Read16 *pCmdRead16 = (Cmd_Read16 *)&m_VMERemote.Msgs[1];

		m_VMERemote.Msgs[0] = V1495_CMD_READ16;
		pCmdRead16->Offset = htons(addr & 0xFFFF);

		if(!m_Socket.Send((void *)&m_VMERemote, sizeof(V1495VMERemote)))//sizeof(Cmd_Read16)+1))
		{
			DebugMsg("ERROR: remote VMERead16 send() failed");
			DisconnectVME();
			return FALSE;
		}

		int len = m_Socket.Receive((void *)&m_VMERemote, sizeof(V1495VMERemote));

		if(len)
		{
			if(m_VMERemote.Msgs[0] == CMD_ERR(V1495_CMD_READ16))
			{
				CString err;
				err.Format("ERR: %s", m_VMERemote.Msgs[1]);
				return FALSE;
			}
			else if((m_VMERemote.Msgs[0] != CMD_RSP(V1495_CMD_READ16)) )
			{
				CString err;

				err.Format("MSG: 0x%02X, LEN = %d", (int)m_VMERemote.Msgs[0], (int)len);
				DebugMsg("ERROR: remote VMERead16 failed");
				DisconnectVME();
				return FALSE;
			}
		}
		else
		{
			DebugMsg("ERROR: remote VMERead16 failed (len = 0)");
			DisconnectVME();
			return FALSE;
		}
		Cmd_Read16_Rsp *pCmdRead16_Rsp = (Cmd_Read16_Rsp *)&m_VMERemote.Msgs[1];
		*val = ntohs(pCmdRead16_Rsp->Value);
	}
	else
	{
		if(!udev)
		{
			DebugMsg("ERROR: VM-USB not initialized");
			return FALSE;
		}
		long rd;
		if(vme_read_16(udev, 0x09, addr, &rd) < 0)
		{
			DebugMsg("ERROR: vme_read_16 failed");
			return FALSE;
		}
		*val = rd;
	}
	if(bDebug || (m_pDbgDlg && ((DebugDlg *)m_pDbgDlg)->m_chDebugAll.GetCheck()))
	{
		CString str;
		str.Format("SUCCESS: Read 0x%04X <= 0x%08X", (unsigned int)*val, addr);
		DebugMsg(str);
	}
	return TRUE;
}

BOOL HallBFlexRegisters::ConnectVME(BOOL bUseTCP, CString ipAddr)
{
	m_bUseRemote = bUseTCP;
	m_bConnected = FALSE;

	if(m_bUseRemote)
	{
		if(!m_Socket.Create() || !m_Socket.Connect(ipAddr, VMESERVER_PORT))
		{
			DebugMsg("ERROR: remote connect() failed");
			return FALSE;
		}
	}
	else
	{
		struct usb_device *dev;
		xxusb_device_type xxusbDev[32];
		int DevFound = xxusb_devices_find(xxusbDev);

		if(!DevFound)
		{
			DebugMsg("ERROR: No valid device found");
			return FALSE;
		}
		else if(DevFound < 0)
		{
			DebugMsg("ERROR: Cannot Open Handle to a XX-USB Device");
			return FALSE;
		}

		for(int i = 0; i < DevFound; i++)
		{
			dev = xxusbDev[i].usbdev;
			CString str;
			str.Format("%d: ID = %d, SN = %s", (i+1), dev->descriptor.idProduct, xxusbDev[i].SerialString);
			DebugMsg(str);
		}
		udev = xxusb_device_open(dev);
		if(!udev)
		{
			DebugMsg("ERROR: Failed to open VM-USB");
			return FALSE;
		}
		if(dev->descriptor.idProduct != XXUSB_VMUSB_PRODUCT_ID)
		{
			DebugMsg("ERROR: Device Product ID Error");
			xxusb_device_close(udev);
			return FALSE;
		}
		DebugMsg("XXUSB_VMUSB_PRODUCT_ID found");
	}

	if(m_bUseRemote)
	{
		Cmd_SetBase *pCmd_SetBase = (Cmd_SetBase *)&m_VMERemote.Msgs[1];

		m_VMERemote.Msgs[0] = V1495_CMD_SETBASE;
		pCmd_SetBase->Address = htonl(V1495_VMEADDRESS & 0xFFFF0000);

		if(!m_Socket.Send((void *)&m_VMERemote, sizeof(V1495VMERemote)))//sizeof(Cmd_SetBase)+1))
		{
			DebugMsg("ERROR: remote Cmd_SetBase send() failed");
			return FALSE;
		}
		unsigned int dummy;
		m_Socket.Receive(&dummy, sizeof(dummy));
	}
	m_bConnected = TRUE;

	ReadBoardRegisters();

	unsigned short val;
	CString ver;
	VMERead16(V1495_VMEADDRESS + TS_REVISION, &val);
	ver.Format("HallBFlexTrigger Firmware Revision: V%u.%u", (unsigned int)(val>>8), (unsigned int)(val & 0xFF));
	DebugMsg(ver);
	
	return TRUE;
}

void HallBFlexRegisters::DisconnectVME()
{
	if(m_bUseRemote)
		m_Socket.Close();
	else
	{
		if(udev)
		{
			xxusb_device_close(udev);
			udev = NULL;
		}
	}
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

	VMERead16(V1495_VMEADDRESS + TS_MORA_DELAY, &val); m_iMORA_Delay = val;
	VMERead16(V1495_VMEADDRESS + TS_MORB_DELAY, &val); m_iMORB_Delay = val;
	for(i = 0; i < 6; i++)
	{
		VMERead16(V1495_VMEADDRESS + TS_TOF1_DELAY + i * 2, &val, FALSE); m_iTOF_Delay[i] = val;
		VMERead16(V1495_VMEADDRESS + TS_ECin1P_DELAY + i * 2, &val, FALSE); m_iECinP_Delay[i] = val;
		VMERead16(V1495_VMEADDRESS + TS_ECin1E_DELAY + i * 2, &val, FALSE); m_iECinE_Delay[i] = val;
		VMERead16(V1495_VMEADDRESS + TS_ECtot1P_DELAY + i * 2, &val, FALSE); m_iECtotP_Delay[i] = val;
		VMERead16(V1495_VMEADDRESS + TS_ECtot1E_DELAY + i * 2, &val, FALSE); m_iECtotE_Delay[i] = val;
		VMERead16(V1495_VMEADDRESS + TS_CC1_DELAY + i * 2, &val, FALSE); m_iCC_Delay[i] = val;
	}
	for(i = 0; i < 24; i++)
	{
		VMERead16(V1495_VMEADDRESS + TS_ST1_DELAY + i * 2, &val, FALSE); m_iST_Delay[i] = val;
	}
	for(i = 0; i < 12; i++)
	{
		VMERead16(V1495_VMEADDRESS + TS_PERSIST_TRIG1 + i * 2, &val, FALSE); m_iTrig_Persist[i] = val;
		VMERead16(V1495_VMEADDRESS + TS_PRESCALE_TRIG1 + i * 2, &val, FALSE); m_iTrig_Prescale[i] = val;
	}
	VMERead16(V1495_VMEADDRESS + TS_STMULT_THRESHOLD, &val, FALSE); m_iMult_Min = (val & 0x1F); m_iMult_Max = val >> 5;

	unsigned short regSTOF_EN, regTOF_DIS, regSTS_DIS;
	VMERead16(V1495_VMEADDRESS + TS_STOF_EN, &regSTOF_EN, FALSE);
	VMERead16(V1495_VMEADDRESS + TS_TOF_DIS, &regTOF_DIS, FALSE);
	VMERead16(V1495_VMEADDRESS + TS_STS_DIS, &regSTS_DIS, FALSE);
	for(i = 0; i < 6; i++)
	{
		int stof_en = regSTOF_EN & (1<<i);
		int tof_dis = regTOF_DIS & (1<<i);
		int sts_dis = regSTS_DIS & (1<<i);

		if(stof_en && tof_dis && !sts_dis) m_iSTOF_Mode[i] = STOF_STS;
		else if(stof_en && !tof_dis && sts_dis) m_iSTOF_Mode[i] = STOF_TOF;
		else if(stof_en && tof_dis && sts_dis) m_iSTOF_Mode[i] = STOF_HI;
		else if(stof_en && !tof_dis && !sts_dis) m_iSTOF_Mode[i] = STOF_AND;
		else m_iSTOF_Mode[i] = STOF_LO;
	}

	unsigned short regECC_EN, regCC_DIS, regECE_DIS;
	VMERead16(V1495_VMEADDRESS + TS_ECE_DIS, &regECE_DIS, FALSE);
	VMERead16(V1495_VMEADDRESS + TS_CC_DIS, &regCC_DIS, FALSE);
	VMERead16(V1495_VMEADDRESS + TS_ECC_EN, &regECC_EN, FALSE);
	for(i = 0; i < 6; i++)
	{
		int ecc_en = regECC_EN & (1<<i);
		int cc_dis = regCC_DIS & (1<<i);
		int ece_dis = regECE_DIS & (1<<i);

		if(ecc_en && !cc_dis && !ece_dis) m_iECC_Mode[i] = ECC_AND;
		else if(ecc_en && cc_dis && !ece_dis) m_iECC_Mode[i] = ECC_ECE;
		else if(ecc_en && !cc_dis && ece_dis) m_iECC_Mode[i] = ECC_CC;
		else if(ecc_en && !cc_dis && !ece_dis) m_iECC_Mode[i] = ECC_HI;
		else m_iECC_Mode[i] = ECC_LO;
	}

	unsigned short regMORA_EN, regMORB_EN, regMOR_DIS;
	VMERead16(V1495_VMEADDRESS + TS_MORA_EN, &regMORA_EN, FALSE);
	VMERead16(V1495_VMEADDRESS + TS_MORB_EN, &regMORB_EN, FALSE);
	VMERead16(V1495_VMEADDRESS + TS_MOR_DIS, &regMOR_DIS, FALSE);
	for(i = 0; i < 12; i++)
	{
		int mora_en = regMORA_EN & (1<<i);
		int morb_en = regMORB_EN & (1<<i);
		int mor_dis = regMOR_DIS & (1<<i);

		if(mora_en && !morb_en && !mor_dis) m_iMOR_Mode[i] = MOR_MORA;
		else if(!mora_en && morb_en && !mor_dis) m_iMOR_Mode[i] = MOR_MORB;
		else if(mora_en && morb_en && !mor_dis) m_iMOR_Mode[i] = MOR_OR;
		else m_iMOR_Mode[i] = MOR_HI;
	}

	unsigned short regSTMult_DIS;
	VMERead16(V1495_VMEADDRESS + TS_STMULT_DIS, &regSTMult_DIS, FALSE);
	for(i = 0; i < 12; i++)
	{
		if(regSTMult_DIS & (1<<i))
			m_iSTMult_Mode[i] = STMULT_MOR;
		else
			m_iSTMult_Mode[i] = STMULT_AND;
	}
}

BOOL HallBFlexRegisters::ReadScope(unsigned short *buf)
{
	if(!m_bConnected)
	{
		DebugMsg("ERROR: VME not initialized");
		return FALSE;
	}

	if(m_bUseRemote)
	{
		m_VMERemote.Msgs[0] = V1495_CMD_GETSCOPEBUF;

		if(!m_Socket.Send((void *)&m_VMERemote, sizeof(V1495VMERemote)))
		{
			DebugMsg("ERROR: remote GetScope - send() failed");
			DisconnectVME();
			return FALSE;
		}

		
		int len = m_Socket.Receive((void *)&m_VMERemote, sizeof(V1495VMERemote));
/*
		if(len)
		{
			if(m_VMERemote.Msgs[0] == CMD_ERR(V1495_CMD_GETSCOPEBUF))
			{
				CString err;
				err.Format("ERR: %s", m_VMERemote.Msgs[1]);
				return FALSE;
			}
			else if((m_VMERemote.Msgs[0] != CMD_RSP(V1495_CMD_GETSCOPEBUF)) )
			{
				CString err;

				err.Format("MSG: 0x%02X, LEN = %d", (int)m_VMERemote.Msgs[0], (int)len);
				DebugMsg("ERROR: remote GetScope failed");
				DisconnectVME();
				return FALSE;
			}
		}
		else
		{
			DebugMsg("ERROR: remote GetScope failed (len = 0)");
			DisconnectVME();
			return FALSE;
		}
		memcpy(buf, &m_VMERemote.Msgs[1], 768*2);
		*/
	}
	else
	{
		for(int i = 0; i < 128; i++)
		{
			if( (vme_read_16(udev, 0x09, V1495_VMEADDRESS + TS_TRIG_BUFFER2_H, (long *)buf++) < 0) ||
				(vme_read_16(udev, 0x09, V1495_VMEADDRESS + TS_TRIG_BUFFER2_L, (long *)buf++) < 0) ||
				(vme_read_16(udev, 0x09, V1495_VMEADDRESS + TS_TRIG_BUFFER1_H, (long *)buf++) < 0) ||
				(vme_read_16(udev, 0x09, V1495_VMEADDRESS + TS_TRIG_BUFFER1_L, (long *)buf++) < 0) ||
				(vme_read_16(udev, 0x09, V1495_VMEADDRESS + TS_TRIG_BUFFER0_H, (long *)buf++) < 0) ||
				(vme_read_16(udev, 0x09, V1495_VMEADDRESS + TS_TRIG_BUFFER0_L, (long *)buf++) < 0) )
			{
				DebugMsg("ERROR: read scope failed");
				DisconnectVME();
				return FALSE;
			}
		}
	}
//	if(m_pDbgDlg && ((DebugDlg *)m_pDbgDlg)->m_chDebugAll.GetCheck())
//		DebugMsg("SUCCESS: ReadScope()");

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
		/*
		str = "Error opening file: ";
		str += filename;
		str += "Reason: ";
		str += exception.m_cause;
		MessageBox(str);
		*/
		return FALSE;
	}

	CTime t = CTime::GetCurrentTime();
	saveFile.WriteString("# G12 Hall B Flex Trigger Module Configuration Register Settings\n");
	saveFile.WriteString(t.Format("# Last Updated: %A, %B %d, %Y at %I:%M:%S%p\n\n"));

	str.Format("TS_MORA_DELAY             %6d\n", m_iMORA_Delay); saveFile.WriteString(str);
	str.Format("TS_MORB_DELAY             %6d\n", m_iMORB_Delay); saveFile.WriteString(str);

	for(i = 0; i < 6; i++) {
	str.Format("TS_TOF%d_DELAY             %6d\n", i+1, m_iTOF_Delay[i]); saveFile.WriteString(str); }
	for(i = 0; i < 6; i++) {
	str.Format("TS_ECin%dP_DELAY           %6d\n", i+1, m_iECinP_Delay[i]); saveFile.WriteString(str); }
	for(i = 0; i < 6; i++) {
	str.Format("TS_ECin%dE_DELAY           %6d\n", i+1, m_iECinE_Delay[i]); saveFile.WriteString(str); }
	for(i = 0; i < 6; i++) {
	str.Format("TS_ECtot%dP_DELAY          %6d\n", i+1, m_iECtotP_Delay[i]); saveFile.WriteString(str); }
	for(i = 0; i < 6; i++) {
	str.Format("TS_ECtot%dE_DELAY          %6d\n", i+1, m_iECtotE_Delay[i]); saveFile.WriteString(str); }
	for(i = 0; i < 6; i++) {
	str.Format("TS_CC%d_DELAY              %6d\n", i+1, m_iCC_Delay[i]); saveFile.WriteString(str); }
	for(i = 0; i < 24; i++) {
	str.Format("TS_ST%d_DELAY              %6d\n", i+1, m_iST_Delay[i]); saveFile.WriteString(str); }
	for(i = 0; i < 12; i++) {
	str.Format("TS_PERSIST_TRIG%d          %6d\n", i+1, m_iTrig_Persist[i]); saveFile.WriteString(str); }
	for(i = 0; i < 12; i++) {
	str.Format("TS_PRESCALE_TRIG%d         %6d\n", i+1, m_iTrig_Prescale[i]); saveFile.WriteString(str); }

	int stof_en;
	for(i = 0; i < 6; i++)
	{
		switch(m_iSTOF_Mode[i])
		{
			case STOF_STS: stof_en = 1; break;
			case STOF_TOF: stof_en = 1;	break;
			case STOF_HI:  stof_en = 1; break;
			case STOF_AND: stof_en = 1; break;
			case STOF_LO:  stof_en = 0; break;
		}
	str.Format("TS_STOF_EN_SEC%d           %6d\n", i+1, stof_en); saveFile.WriteString(str);
	}

	int tof_dis;
	for(i = 0; i < 6; i++)
	{
		switch(m_iSTOF_Mode[i])
		{
			case STOF_STS: tof_dis = 1; break;
			case STOF_TOF: tof_dis = 0;	break;
			case STOF_HI:  tof_dis = 1; break;
			case STOF_AND: tof_dis = 0; break;
			case STOF_LO:  tof_dis = 0; break;
		}
	str.Format("TS_TOF_DIS_SEC%d           %6d\n", i+1, tof_dis); saveFile.WriteString(str);
	}

	int sts_dis;
	for(i = 0; i < 6; i++)
	{
		switch(m_iSTOF_Mode[i])
		{
			case STOF_STS: sts_dis = 0; break;
			case STOF_TOF: sts_dis = 1;	break;
			case STOF_HI:  sts_dis = 1; break;
			case STOF_AND: sts_dis = 0; break;
			case STOF_LO:  sts_dis = 0; break;
		}
	str.Format("TS_STS_DIS_SEC%d           %6d\n", i+1, sts_dis); saveFile.WriteString(str);
	}

	int ece_dis;
	for(i = 0; i < 6; i++)
	{
		switch(m_iECC_Mode[i])
		{
			case ECC_AND: ece_dis = 0; break;
			case ECC_ECE: ece_dis = 0; break;
			case ECC_CC:  ece_dis = 1; break;
			case ECC_HI:  ece_dis = 1; break;
			case ECC_LO:  ece_dis = 0; break;
		}
	str.Format("TS_ECE_DIS_SEC%d          %6d\n", i+1, ece_dis); saveFile.WriteString(str);
	}

	int cc_dis;
	for(i = 0; i < 6; i++)
	{
		switch(m_iECC_Mode[i])
		{
			case ECC_AND: cc_dis = 0; break;
			case ECC_ECE: cc_dis = 1; break;
			case ECC_CC:  cc_dis = 0; break;
			case ECC_HI:  cc_dis = 1; break;
			case ECC_LO:  cc_dis = 0; break;
		}
	str.Format("TS_CC_DIS_SEC%d          %6d\n", i+1, cc_dis); saveFile.WriteString(str);
	}

	int ecc_en;
	for(i = 0; i < 6; i++)
	{
		switch(m_iECC_Mode[i])
		{
			case ECC_AND: ecc_en = 1; break;
			case ECC_ECE: ecc_en = 1; break;
			case ECC_CC:  ecc_en = 1; break;
			case ECC_HI:  ecc_en = 1; break;
			case ECC_LO:  ecc_en = 0; break;
		}
	str.Format("TS_ECC_EN_SEC%d          %6d\n", i+1, ecc_en); saveFile.WriteString(str);
	}

	int mora_en;
	for(i = 0; i < 12; i++)
	{
		switch(m_iMOR_Mode[i])
		{
			case MOR_MORA: mora_en = 1; break;
			case MOR_MORB: mora_en = 0; break;
			case MOR_OR:   mora_en = 1; break;
			case MOR_HI:   mora_en = 0; break;
		}
	str.Format("TS_MORA_EN_TRIG%d        %6d\n", i+1, mora_en); saveFile.WriteString(str);
	}

	int morb_en;
	for(i = 0; i < 12; i++)
	{
		switch(m_iMOR_Mode[i])
		{
			case MOR_MORA: morb_en = 0; break;
			case MOR_MORB: morb_en = 1; break;
			case MOR_OR:   morb_en = 1; break;
			case MOR_HI:   morb_en = 0; break;
		}
	str.Format("TS_MORB_EN_TRIG%d        %6d\n", i+1, morb_en); saveFile.WriteString(str);
	}

	int mor_dis;
	for(i = 0; i < 12; i++)
	{
		switch(m_iMOR_Mode[i])
		{
			case MOR_MORA: mor_dis = 0; break;
			case MOR_MORB: mor_dis = 0; break;
			case MOR_OR:   mor_dis = 0; break;
			case MOR_HI:   mor_dis = 1; break;
		}
	str.Format("TS_MOR_DIS_TRIG%d        %6d\n", i+1, mor_dis); saveFile.WriteString(str);
	}

	str.Format("TS_ST_MULT_MAX            %6d\n", m_iMult_Max); saveFile.WriteString(str);
	str.Format("TS_ST_MULT_MIN            %6d\n", m_iMult_Min); saveFile.WriteString(str);

	int stmult_dis;
	for(i = 0; i < 12; i++)
	{
		switch(m_iSTMult_Mode[i])
		{
			case STMULT_AND: stmult_dis = 0; break;
			case STMULT_MOR: stmult_dis = 1; break;
		}
		str.Format("TS_STMULT_DIS_TRIG%d        %6d\n", i+1, stmult_dis); saveFile.WriteString(str);
	}

	saveFile.Close();
	return TRUE;
}
