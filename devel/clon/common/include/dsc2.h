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


/* Define a Structure for access to Discriminator */
/* all addressing is A24/A32, all data D32 */
typedef struct dsc2_struct
{
  /*0x0000*/ volatile unsigned int threshold[16];    /* RW TRG=25-16  TDC=9-0 */
  /*0x0040*/ volatile unsigned int res1[16];
  /*0x0080*/ volatile unsigned int width;            /* RW TRGout=31-28  TRG=21-16  TDC=5-0 */
  /*0x0084*/ volatile unsigned int res2;
  /*0x0088*/ volatile unsigned int CHmask;           /* RW TRGen15-0=31-16  TDCen15-0=15-0 */
  /*0x008C*/ volatile unsigned int ORmask;           /* RW TRGen15-0=31-16  TDCen15-0=15-0 */
  /*0x0090*/ volatile unsigned int delay;            /* RW TRGout=22-16  Scaler=6-0 */
  /*0x0094*/ volatile unsigned int res3;
  /*0x0098*/ volatile unsigned int VMElatch;         /* WO D32 */
  /*0x009C*/ volatile unsigned int GTDlatch;         /* WO D32 */
  /*0x00A0*/ volatile unsigned int res4[24];
  /*0x0100*/ volatile unsigned int TRGscaler[16];    /* RO D32 */
  /*0x0140*/ volatile unsigned int TDCscaler[16];    /* RO D32 */
  /*0x0180*/ volatile unsigned int TRGscalerVME[16]; /* RO D32 */
  /*0x01C0*/ volatile unsigned int TDCscalerVME[16]; /* RO D32 */
  /*0x0200*/ volatile unsigned int REFscalerVME;     /* RO D32 */
  /*0x0204*/ volatile unsigned int REFscaler;        /* RO D32 */
  /*0x0208*/ volatile unsigned int res5[126];
  /*0x0400*/ volatile unsigned int firmware;         /* RO RevMAJ=15-8  RevMIN=7-0 */
  /*0x0404*/ volatile unsigned int boardID;          /* RO D,S,C,2=31-24,23-16,15-8,7-0 */

} DSC2;


#define DSC2_TRG_THRESHOLD         0x03ff0000
#define DSC2_TDC_THRESHOLD         0x000003ff

#define DSC2_TRG_OUTPUT_WIDTH      0xf0000000
#define DSC2_TRG_PULSER_WIDTH      0x003f0000
#define DSC2_TDC_PULSER_WIDTH      0x0000003f

#define DSC2_TRG_MASK              0xffff0000
#define DSC2_ADC_MASK              0x0000ffff

#define DSC2_TRG_OUTPUT_DELAY      0x007f0000
#define DSC2_SCALER_INPUT_DELAY    0x0000007f

#define DSC2_FIRMWARE_REV_MAJ      0x0000ff00
#define DSC2_FIRMWARE_REV_MIN      0x000000ff


#define UINT32 unsigned int
#define UINT16 unsigned short
#define INT32  int

