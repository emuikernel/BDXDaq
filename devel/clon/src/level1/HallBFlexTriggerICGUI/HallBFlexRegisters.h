// HallBFlexRegisters.h: interface for the HallBFlexRegisters class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HALLBFLEXREGISTERS_H__7C79B9D6_1DEE_486F_858B_01861B42AC0D__INCLUDED_)
#define AFX_HALLBFLEXREGISTERS_H__7C79B9D6_1DEE_486F_858B_01861B42AC0D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "V1495VMERemote.h"
#include "HallBTriggerBoardRegs.h"

#define ECC_AND		0x3
#define ECC_ECE		0x1
#define ECC_CC		0x2
#define ECC_HI		0x4
#define ECC_LO		0x8

enum { LUT_ECP, LUT_ECE, LUT_ECC, LUT_TRIG };

class HallBFlexRegisters  
{
public:
	BOOL LoadRegisterSettings(CString filename);
	BOOL SaveRegisterSettings(CString filename);
	BOOL ReadScope(unsigned short *buf);
	void ReadBoardRegisters();
	BOOL m_bDebugAll;
	void DebugMsg(CString msg);
	void DisconnectVME();
	BOOL VMERead16(unsigned int addr, unsigned short *val, BOOL bDebug = FALSE, BOOL bIgnoreReady = FALSE);
	BOOL VMEWrite16(unsigned int addr, unsigned short val, BOOL bDebug = FALSE, BOOL bIgnoreReady = FALSE);
	BOOL VMEBlkRead16(unsigned int addr, unsigned short count, unsigned short *vals);
	void SetDebugDlg(void *pDbgDlg);
	CSocket *m_pSocket;
	BOOL ConnectVME(CString addr);
	BOOL m_bConnected;
	CString LoadLUT(int trig, int table, CString filename = "");
	void IncCCDelay(int sector, int step);
	void IncECtotPDelay(int sector, int step);
	void IncECinPDelay(int sector, int step);
	void IncECtotEDelay(int sector, int step);
	void IncECinEDelay(int sector, int step);
	void IncTOFDelay(int sector, int step);
	void IncICDelay(int step);
	void IncE_CCDelay(int sector, int step);
	void IncE_ECtotEDelay(int sector, int step);
	void IncE_ECinEDelay(int sector, int step);
	BOOL ProgramModule(CString filename, unsigned int base = TS_BOARD_ADDRESS);

	void IncECCMode(int sector, BOOL change = TRUE);
	void IncPrescale(int trigger, int step);

	void *m_pDbgDlg;
	int m_iTOF_Delay[6];
	int m_iECinE_Delay[6];
	int m_iECtotE_Delay[6];
	int m_iECinP_Delay[6];
	int m_iECtotP_Delay[6];
	int m_iCC_Delay[6];
	int m_iE_ECinE_Delay[6];
	int m_iE_ECtotE_Delay[6];
	int m_iE_CC_Delay[6];
	int m_iIC_Delay;

	int m_iTrig_Prescale[12];

	unsigned int m_iTOF_Scaler[6];
	unsigned int m_iECinP_Scaler[6];
	unsigned int m_iECtotP_Scaler[6];
	unsigned int m_iECinE_Scaler[6];
	unsigned int m_iECtotE_Scaler[6];
	unsigned int m_iCC_Scaler[6];
	unsigned int m_iECE_Scaler[6];
	unsigned int m_iECP_Scaler[6];
	unsigned int m_iECC_Scaler[6];
	unsigned int m_iESector_Scaler[6];
	unsigned int m_iTRIG_Scaler[12];
	unsigned int m_iICElectron_Scaler[3];
	unsigned int m_iICTotal_Scaler[3];
	unsigned int m_iICVeto_Scaler[3];
	unsigned int m_iECPLUT_Scaler[11];
	unsigned int m_iECELUT_Scaler[11];
	unsigned int m_iECCLUT_Scaler[11];

	unsigned int m_iREF_Scaler;

	CString m_stECPLUTNames[11];
	CString m_stECELUTNames[11];
	CString m_stECCLUTNames[11];
	CString m_stTRIGLUTNames[11];

	int m_iECC_Mode[6];

	HallBFlexRegisters();
	virtual ~HallBFlexRegisters();

private:
	RemoteMsgStruct		IncomingMsg;
	RemoteMsgStruct		OutgoingMsg;
};

#endif // !defined(AFX_HALLBFLEXREGISTERS_H__7C79B9D6_1DEE_486F_858B_01861B42AC0D__INCLUDED_)
