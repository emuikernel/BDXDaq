/****************************************************************************
 *
 *  dsc2.h  -  Driver library header file for readout of a 
 *             New JLab 16-Channel Discriminator/Scaler VME Module
 *
 *  Author: Serguei Pozdniakov
 *          Jefferson Lab Online Group
 *          September 2009
 */

/*
Register		Description			Offset	Reset_State
---------------------------------------------------------------------------
A_THRESHOLD_CH0		Threshold Control Ch0		0x0000	0x0FFF0000
...			Threshold Control ChX		...	...
A_THRESHOLD_CH15	Threshold Control Ch15		0x003C	0x0FFF0000
A_PULSEWIDTH		Pulse Width Control		0x0080	0x01A501A5
A_CH_ENABLE		Channel Control			0x0088	0xFFFFFFFF
A_OR_MASK		OR Output Control		0x008C
A_DELAY			Input/Output Delays		0x0090	0x00080008
A_TEST			Test Input Control		0x0094	0x00000001
A_VME_LATCH		VME Scaler Latch		0x0098	0xXXXXXXXX
A_LATCH			Gated Scaler Latch		0x009C	0xXXXXXXXX
A_TRG_SCALER_CH0	Ext. Gate Trigger Scaler Ch0	0x0100	0xXXXXXXXX
...			Ext. Gate Trigger Scaler ChX	...	0xXXXXXXXX
A_TRG_SCALER_CH15	Ext. Gate Trigger Scaler Ch15	0x013C	0xXXXXXXXX
A_TDC_SCALER_CH0	Ext. Gate TDC Scaler Ch0	0x0140	0xXXXXXXXX
...			Ext. Gate TDC Scaler ChX	...	0xXXXXXXXX
A_TDC_SCALER_CH15	Ext. Gate TDC Scaler Ch15	0x017C	0xXXXXXXXX
A_TRG_VME_SCALER_CH0	VME Gate Trigger Scaler Ch0	0x0180	0xXXXXXXXX
...			VME Gate Trigger Scaler ChX	...	0xXXXXXXXX
A_TRG_VME_SCALER_CH15	VME Gate Trigger Scaler Ch15	0x01BC	0xXXXXXXXX
A_TDC_VME_SCALER_CH0	VME Gate TDC Scaler Ch0		0x01C0	0xXXXXXXXX
...			VME Gate TDC Scaler ChX		...	0xXXXXXXXX
A_TDC_VME_SCALER_CH15	VME Gate TDC Scaler Ch15	0x01FC	0xXXXXXXXX
A_REF_SCALER		VME Gate Ref Scaler		0x0200	0xXXXXXXXX
A_REF_SCALER_GATE	Ext. Gate Ref Scaler		0x0204	0xXXXXXXXX
A_FIRMWARE_REV		Firmware Revision		0x0400	0xXXXXXXXX
A_BOARDID		Board Identifier		0x0404	0x44534332 = "DSC2" in ASCII
*/

#ifndef __VMEDSC2__
#define __VMEDSC2__

/* Define a Structure for access to Discriminator */
/* all addressing is A24/A32, all data D32 */
typedef struct dsc2_struct
{
  /*0x0000*/ volatile unsigned int threshold[16];    /* RW TRG=25-16  TDC=9-0 */
  /*0x0040*/ volatile unsigned int res1[16];
  /*0x0080*/ volatile unsigned int width;            /* RW TRGout=31-28  TRG=21-16  TDC=5-0 */
  /*0x0084*/ volatile unsigned int res2;
  /*0x0088*/ volatile unsigned int chMask;           /* RW TRGen15-0=31-16  TDCen15-0=15-0 */
  /*0x008C*/ volatile unsigned int orMask;           /* RW TRGen15-0=31-16  TDCen15-0=15-0 */
  /*0x0090*/ volatile unsigned int delay;            /* RW TRGout=22-16  Scaler=6-0 */
  /*0x0094*/ volatile unsigned int testCtrl;         /* RW 1=1 enable front-panel TestIn */
  /*0x0098*/ volatile unsigned int vmeScalerLatch;   /* WO D32 */
  /*0x009C*/ volatile unsigned int gatedScalerLatch; /* WO D32 */
  /*0x00A0*/ volatile unsigned int res3[24];
  /*0x0100*/ volatile unsigned int trgScaler[16];    /* RO D32 */
  /*0x0140*/ volatile unsigned int tdcScaler[16];    /* RO D32 */
  /*0x0180*/ volatile unsigned int trgVmeScaler[16]; /* RO D32 */
  /*0x01C0*/ volatile unsigned int tdcVmeScaler[16]; /* RO D32 */
  /*0x0200*/ volatile unsigned int refVmeScaler;     /* RO D32 */
  /*0x0204*/ volatile unsigned int refGatedScaler;   /* RO D32 */
  /*0x0208*/ volatile unsigned int res4[126];
  /*0x0400*/ volatile unsigned int firmware;         /* RO RevMAJ=15-8  RevMIN=7-0 */
  /*0x0404*/ volatile unsigned int boardID;          /* RO D,S,C,2=31-24,23-16,15-8,7-0 */
  /*0x0408*/          unsigned int res5[(0x500-0x408)/4];
  /*0x0500*/ volatile unsigned int clearFIFO;        /* WO D32 */
  /*0x0504*/ volatile unsigned int softTrigger;      /* RW D32 */
  /*0x0508*/          unsigned int readoutConfig;    /* RW D32 */
  /*0x050C*/          unsigned int res6[(0x8000-0x50C)/4];
  /*0x8000*/ volatile unsigned int calCmd;
  /*0x8004*/ volatile unsigned int calBuf[(0x8204-0x8004)/4];
  /*0x8204*/          unsigned int blank6[(0x9000-0x8204)/4];
  /*0x9000*/ volatile unsigned int calExe;
  /*
             volatile unsigned int mem[(0x8800-0x8000)/4];
                      unsigned int res7[(0x9000-0x8800)/4];
             volatile unsigned int memExe;
  */

} DSC2;

#define DSC2_BOARD_ID     0x44534332
#define DSC2_A32_ADDR     0x08000000


#define DSC2_TRG_THRESHOLD         0x03ff0000
#define DSC2_TDC_THRESHOLD         0x000003ff

#define DSC2_TRG_OUTPUT_WIDTH      0xf0000000
#define DSC2_TRG_PULSER_WIDTH      0x003f0000
#define DSC2_TDC_PULSER_WIDTH      0x0000003f

#define DSC2_TRG_MASK              0xffff0000
#define DSC2_TDC_MASK              0x0000ffff

#define DSC2_TRG_OUTPUT_DELAY      0x007f0000
#define DSC2_SCALER_INPUT_DELAY    0x0000007f

#define DSC2_FIRMWARE_REV_MAJ      0x0000ff00
#define DSC2_FIRMWARE_REV_MIN      0x000000ff


#define UINT32 unsigned int
#define UINT16 unsigned short
#define INT32  int


/**************/
/* from bryan */

#define DSC_BOARD_ID       0x44534332
#define DSC_MAX_BOARDS             40

/* Reference channel clock frequency - 128 MHz */
#define DSC_REFERENCE_RATE  125000000.0

/* Some definitions for "type" argument in routines */
#define TDCTRG      0  /* Both TDC and TRG */
#define TDC         1
#define TRG         2
#define TRGOUTPUT   3
#define SCALER      1
#define VMESCALER   0
#define GATEDSCALER 1

/* threshold register Masks */
#define DSC_THRESHOLD_TRG_MASK          0x03ff0000
#define DSC_THRESHOLD_TDC_MASK          0x000003ff

/* pulsewidth register Masks */
#define DSC_PULSEWIDTH_TRG_OUTPUT_MASK  0xf0000000
#define DSC_PULSEWIDTH_TRG_MASK         0x003f0000
#define DSC_PULSEWIDTH_TDC_MASK         0x0000003f

/* chEnable register Masks */
#define DSC_CHENABLE_TRG_MASK           0xffff0000
#define DSC_CHENABLE_TDC_MASK           0x0000ffff

/* orMask register Masks */
#define DSC_ORMASK_TRG_MASK             0xffff0000
#define DSC_ORMASK_TDC_MASK             0x0000ffff

/* delay register Masks */
#define DSC_DELAY_TRG_MASK              0x007f0000
#define DSC_DELAY_SCALER_MASK           0x0000007f

/* firmwareRev register Masks */
#define DSC_FIRMWAREREV_MASK            0xffff
#define DSC_FIRMWAREREV_MAJOR_MASK      0xff00
#define DSC_FIRMWAREREV_MINOR_MASK      0x00ff

/* Scaler readout definitions 
   These are bound to change in future firmware/driver revisions */
#define DSC_SCT_HEADER            0xdca00000 /* + (0-7) Readout flag in LSBs + (8-12) Module id */
#define DSC_READOUT_TRG                 1<<0
#define DSC_READOUT_TDC                 1<<1
#define DSC_READOUT_TRGVME              1<<2
#define DSC_READOUT_TDCVME              1<<3
#define DSC_READOUT_REF                 1<<4
#define DSC_READOUT_REFVME              1<<5
#define DSC_LATCH_VME_BEFORE_READOUT    1<<6
#define DSC_LATCH_GATED_BEFORE_READOUT  1<<7

/* Function Prototypes */
int  dsc2Status            (UINT32 id, int pflag);
int  dsc2SetThreshold      (UINT32 id, UINT16 chan,   UINT16 val, UINT16 type);
int  dsc2GetThreshold      (UINT32 id, UINT16 chan,   UINT16 type);
int  dsc2SetPulseWidth     (UINT32 id, UINT16 val,    UINT16 type);
int  dsc2GetPulseWidth     (UINT32 id, UINT16 type);
int  dsc2SetPulseWidthAll  (UINT16 tdcVal, UINT16 trgVal, UINT16 trgoutVal);
int  dsc2SetThresholdAll   (UINT16 tdcVal, UINT16 trgVal);
int  dsc2EnableChannel     (UINT32 id, UINT16 chan,   UINT16 type);
int  dsc2DisableChannel    (UINT32 id, UINT16 chan,   UINT16 type);
int  dsc2SetChannelMask    (UINT32 id, UINT16 chMask, UINT16 type);
int  dsc2GetChannelMask    (UINT32 id, UINT16 type);
int  dsc2EnableChannelOR   (UINT32 id, UINT16 chan,   UINT16 type);
int  dsc2DisableChannelOR  (UINT32 id, UINT16 chan,   UINT16 type);
int  dsc2SetChannelORMask  (UINT32 id, UINT16 chMask, UINT16 type);
int  dsc2GetChannelORMask  (UINT32 id, UINT16 type);
int  dsc2SetDelay          (UINT32 id, UINT16 val,    UINT16 type);
int  dsc2GetDelay          (UINT32 id, UINT16 type);
int  dsc2Test              (UINT32 id, UINT32 flag);
int  dsc2LatchScalers      (UINT32 id, UINT16 type);
int  dsc2ReadScalers       (UINT32 id, volatile UINT32 *data, int nwrds, int rflag);
int  dsc2PrintScalers      (UINT32 id, int rflag);
int  dsc2PrintScalerRates  (UINT32 id, int rflag);

/* from Ben Raydo: to update new Firmware */
#define SPIFLASH_ID_NUMONYX      0x20
#define SPIFLASH_ID_ATMEL        0x1F

#define CAL_ADDR_NUMONYX         0x1FE000
#define CAL_ADDR_ATMEL           0x3FC000

void dsc2FlashChipSelect   (UINT32 id, int select);
int  dsc2FlashGetId        (UINT32 id);
int  dsc2FlashPollStatus   (UINT32 id);
int  dsc2Calibrate         (UINT32 id);
int  dsc2CalibrateAll      (void);
int  dsc2ReloadFPGA        (UINT32 id);
int  dsc2UpdateFirmwareAll (const char *filename);
int  dsc2UpdateFirmware    (UINT32 id, const char *filename);
int  dsc2VerifyFirmware    (UINT32 id, const char *filename);
int  dsc2ReadFirmware      (UINT32 id, const char *filename);

#endif /* __VMEDSC2__ */

