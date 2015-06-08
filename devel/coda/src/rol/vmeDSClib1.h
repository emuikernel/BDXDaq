/*modified by Sergey */

/*----------------------------------------------------------------------------*
 *  Copyright (c) 2010        Southeastern Universities Research Association, *
 *                            Thomas Jefferson National Accelerator Facility  *
 *                                                                            *
 *    This software was developed under a United States Government license    *
 *    described in the NOTICE file included as part of this distribution.     *
 *                                                                            *
 *    Authors: Bryan Moffit                                                   *
 *             moffit@jlab.org                   Jefferson Lab, MS-12B3       *
 *             Phone: (757) 269-5660             12000 Jefferson Ave.         *
 *             Fax:   (757) 269-5800             Newport News, VA 23606       *
 *                                                                            *
 *----------------------------------------------------------------------------*
 *
 * Description:
 *     Header file for the TJNAF VME 16 Channel Discriminator/Scaler 
 *
 *----------------------------------------------------------------------------*/

#ifndef __VMEDSC__
#define __VMEDSC__

#define DSC_BOARD_ID       0x44534332
#define DSC_MAX_BOARDS             20
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

/* Define DSC Memory structure */
struct dsc_struct {
  /* 0x0000 */ volatile unsigned int threshold[16];
  /* 0x0040 */          unsigned int blank0[(0x80-0x40)/4];
  /* 0x0080 */ volatile unsigned int pulsewidth;
  /* 0x0084 */          unsigned int blank1;
  /* 0x0088 */ volatile unsigned int chEnable;
  /* 0x008C */ volatile unsigned int orMask;
  /* 0x0090 */ volatile unsigned int delay;
  /* 0x0094 */ volatile unsigned int testCtrl;
  /* 0x0098 */ volatile unsigned int vmeScalerLatch;
  /* 0x009C */ volatile unsigned int gatedScalerLatch;
  /* 0x00A0 */          unsigned int blank2[(0x100-0xA0)/4];
  /* 0x0100 */ volatile unsigned int triggerScaler[16];
  /* 0x0140 */ volatile unsigned int tdcScaler[16];
  /* 0x0180 */ volatile unsigned int triggerVmeScaler[16];
  /* 0x01C0 */ volatile unsigned int tdcVmeScaler[16];
  /* 0x0200 */ volatile unsigned int refVmeScaler;
  /* 0x0204 */ volatile unsigned int refGatedScaler;
  /* 0x0208 */          unsigned int blank3[(0x400-0x208)/4];
  /* 0x0400 */ volatile unsigned int firmwareRev;
  /* 0x0404 */ volatile unsigned int boardID;
  /* 0x0408 */          unsigned int blank4[(0x500-0x408)/4];
  /* 0x0500 */ volatile unsigned int clearFIFO;
  /* 0x0504 */ volatile unsigned int softTrigger;
  /* 0x0508 */          unsigned int blank5[(0x8000-0x508)/4];
  /* 0x8000 */ volatile unsigned int calCmd;
  /* 0x8004 */ volatile unsigned int calBuf[(0x8204-0x8004)/4];
  /* 0x8204 */          unsigned int blank6[(0x9000-0x8204)/4];
  /* 0x9000 */ volatile unsigned int calExe;
};

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
int  dscInit(unsigned int addr, unsigned int addr_incr, int ndsc);
int  dscStatus(UINT32 id, int pflag);
int  dscSetThreshold(UINT32 id, UINT16 chan, UINT16 val, UINT16 type);
int  dscGetThreshold(UINT32 id, UINT16 chan, UINT16 type);
int  dscSetPulseWidth(UINT32 id, UINT16 val, UINT16 type);
int  dscGetPulseWidth(UINT32 id, UINT16 type);
int  dscEnableChannel(UINT32 id, UINT16 chan, UINT16 type);
int  dscDisableChannel(UINT32 id, UINT16 chan, UINT16 type);
int  dscSetChannelMask(UINT32 id, UINT16 chMask, UINT16 type);
int  dscGetChannelMask(UINT32 id, UINT16 type);
int  dscEnableChannelOR(UINT32 id, UINT16 chan, UINT16 type);
int  dscDisableChannelOR(UINT32 id, UINT16 chan, UINT16 type);
int  dscSetChannelORMask(UINT32 id, UINT16 chMask, UINT16 type);
int  dscGetChannelORMask(UINT32 id, UINT16 type);
int  dscSetDelay(UINT32 id, UINT16 val, UINT16 type);
int  dscGetDelay(UINT32 id, UINT16 type);
int  dscTest(UINT32 id, UINT32 flag);
int  dscLatchScalers(UINT32 id, UINT16 type);
int  dscReadScalers(UINT32 id, volatile UINT32 *data, int nwrds, int rflag);
int  dscPrintScalers(UINT32 id, int rflag);
int  dscPrintScalerRates(UINT32 id, int rflag);

#endif /* __VMEDSC__ */
