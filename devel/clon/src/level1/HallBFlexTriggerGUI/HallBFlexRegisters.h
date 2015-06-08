// HallBFlexRegisters.h: interface for the HallBFlexRegisters class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HALLBFLEXREGISTERS_H__7C79B9D6_1DEE_486F_858B_01861B42AC0D__INCLUDED_)
#define AFX_HALLBFLEXREGISTERS_H__7C79B9D6_1DEE_486F_858B_01861B42AC0D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "v1495VMEFlexRemote.h"
#include "xxusbdll.h"

enum { STOF_AND, STOF_STS, STOF_TOF, STOF_HI, STOF_LO };
enum { ECC_AND, ECC_ECE, ECC_CC, ECC_HI, ECC_LO };
enum { MOR_OR, MOR_MORA, MOR_MORB, MOR_HI };
enum { STMULT_AND, STMULT_MOR };

class HallBFlexRegisters  
{
public:
	BOOL SaveRegisterSettings(CString filename);
	BOOL ReadScope(unsigned short *buf);
	void ReadBoardRegisters();
	BOOL m_bDebugAll;
	V1495VMERemote m_VMERemote;
	void DebugMsg(CString msg);
	void DisconnectVME();
	BOOL VMERead16(unsigned int addr, unsigned short *val, BOOL bDebug = TRUE, BOOL bIgnoreReady = FALSE);
	BOOL VMEWrite16(unsigned int addr, unsigned short val, BOOL bDebug = TRUE, BOOL bIgnoreReady = FALSE);
	BOOL VMEBlkWrite16(unsigned int addr, unsigned short count, unsigned short *vals, BOOL verify);
	BOOL VMEBlkRead16(unsigned int addr, unsigned short count, unsigned short *vals);
	void SetDebugDlg(void *pDbgDlg);
	CSocket m_Socket;
	BOOL m_bUseRemote;
	BOOL ConnectVME(BOOL bUseTCP, CString ipAddr);
	BOOL m_bConnected;
	void IncMORBDelay(int step);
	void IncMORADelay(int step);
	void IncCCDelay(int sector, int step);
	void IncECtotPDelay(int sector, int step);
	void IncECinPDelay(int sector, int step);
	void IncECtotEDelay(int sector, int step);
	void IncECinEDelay(int sector, int step);
	void IncTOFDelay(int sector, int step);
	void IncSTDelay(int sector, int st, int step);

	void IncSTMultMode(int trigger);
	void IncMORMode(int trigger);
	void IncECCMode(int sector);
	void IncSTOFMode(int sector);
	void IncMultMin(int step);
	void IncMultMax(int step);
	void IncPersist(int trigger, int step);
	void IncPrescale(int trigger, int step);

	void *m_pDbgDlg;
	int m_iMOR_Mode[12];
	int m_iSTMult_Mode[12];
	int m_iMORA_Delay;
	int m_iMORB_Delay;
	int m_iST_Delay[24];
	int m_iTOF_Delay[6];
	int m_iECinE_Delay[6];
	int m_iECtotE_Delay[6];
	int m_iECinP_Delay[6];
	int m_iECtotP_Delay[6];
	int m_iCC_Delay[6];

	int m_iTrig_Persist[12];
	int m_iTrig_Prescale[12];

	int m_iMult_Min;
	int m_iMult_Max;

	unsigned int m_iST_Scaler[24];
	unsigned int m_iTOF_Scaler[6];
	unsigned int m_iECinP_Scaler[6];
	unsigned int m_iECtotP_Scaler[6];
	unsigned int m_iECinE_Scaler[6];
	unsigned int m_iECtotE_Scaler[6];
	unsigned int m_iCC_Scaler[6];
	unsigned int m_iSTS_Scaler[6];
	unsigned int m_iECE_Scaler[6];
	unsigned int m_iSTOF_Scaler[6];
	unsigned int m_iECP_Scaler[6];
	unsigned int m_iECC_Scaler[6];

	unsigned int m_iLUT_Scaler[12];
	unsigned int m_iLUTMOR_Scaler[12];
	unsigned int m_iLUTMORST_Scaler[12];
	unsigned int m_iTrigPersist_Scaler[12];
	unsigned int m_iTrigPrescale_Scaler[12];
	unsigned int m_iSTMult_Scaler;

	unsigned int m_iMORA_Scaler;
	unsigned int m_iMORB_Scaler;
	unsigned int m_iMOROr_Scaler;

	unsigned int m_iREF_Scaler;

	int m_iSTOF_Mode[6];
	int m_iECC_Mode[6];

	HallBFlexRegisters();
	virtual ~HallBFlexRegisters();

private:
	usb_dev_handle *udev;

};

#endif // !defined(AFX_HALLBFLEXREGISTERS_H__7C79B9D6_1DEE_486F_858B_01861B42AC0D__INCLUDED_)
