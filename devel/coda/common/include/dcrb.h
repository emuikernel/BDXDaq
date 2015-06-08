
/* dcrb.h */

#ifndef DCRB_H
#define DCRB_H

#define DATA_TYPE_BLKHDR                0x00
#define DATA_TYPE_BLKTLR                0x01
#define DATA_TYPE_EVTHDR                0x02
#define DATA_TYPE_TRGTIME               0x03
#define DATA_TYPE_TDCEVT                0x08
#define DATA_TYPE_DNV                   0x0E
#define DATA_TYPE_FILLER                0x0F

typedef struct
{
/* 0x0000-0x0003 */ volatile unsigned int FirmwareRev;
/* 0x0004-0x0007 */ volatile unsigned int BoardID;
/* 0x0008-0x0013 */ volatile unsigned int BLDError[3];
/* 0x0014-0x001F */ volatile unsigned int BLDBusy[3];
/* 0x0020-0x0023 */ volatile unsigned int LookBack;
/* 0x0024-0x0027 */ volatile unsigned int WindowWidth;
/* 0x0028-0x002B */ volatile unsigned int BlockConfig;
/* 0x002C-0x002F */ volatile unsigned int TDCConfig;
/* 0x0030-0x0033 */ volatile unsigned int ClockConfig;
/* 0x0034-0x0037 */ volatile unsigned int TestPulseConfig;
/* 0x0038-0x003B */ volatile unsigned int DACConfig;
/* 0x003C-0x003F */ volatile unsigned int TriggerBusyThreshold;
/* 0x0040-0x0043 */ volatile unsigned int TriggerSource;
/* 0x0044-0x0047 */ volatile unsigned int ADR32;
/* 0x0048-0x004B */ volatile unsigned int Interrupt;
/* 0x004C-0x004F */ volatile unsigned int InterruptAck;
/* 0x0050-0x0053 */ volatile unsigned int Geo;
/* 0x0054-0x0057 */ volatile unsigned int FifoWordCnt;
/* 0x0058-0x005B */ volatile unsigned int FifoEventCnt;
/* 0x005C-0x005F */ volatile unsigned int ReadoutConfig;
/* 0x0060-0x0063 */ volatile unsigned int SRAMDebugAddr;
/* 0x0064-0x0067 */ volatile unsigned int SRAMDebugData;
/* 0x0068-0x006B */ volatile unsigned int Reset;
/* 0x006C-0x0077 */ volatile unsigned int ChDisable[3];
} DCRB_regs;


void DCRBSync();
void DCRBTrig();
void DCRBEnableTestPulse(unsigned char fce_mask, unsigned char dce_mask);
void DCRBUpdateDAC(unsigned short val);
void DCRBInit(int threshold);
int  DCRBReadoutFIFO(unsigned int *gReadoutBuf, int *rlenbuf);
void DCRBStat();
int  DCRBFifoEventCount();


#endif
