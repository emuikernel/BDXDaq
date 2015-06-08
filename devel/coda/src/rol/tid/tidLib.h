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
 *     Primitive trigger control for VME CPUs using the TJNAF Trigger
 *     Interface Distribution (TID) card
 *
 * SVN: $Rev: 492 $
 *
 *----------------------------------------------------------------------------*/
#ifndef TIDLIB_H
#define TIDLIB_H

#include <sdLib.h>
#include <ctpLib.h>
#include <pthread.h>

pthread_mutex_t tidISR_mutex=PTHREAD_MUTEX_INITIALIZER;

#ifdef VXWORKS
#define INTLOCK {				\
    if(pthread_mutex_lock(&tidISR_mutex)<0)	\
      perror("pthread_mutex_lock");		\
}

#define INTUNLOCK {				\
    if(pthread_mutex_unlock(&tidISR_mutex)<0)	\
      perror("pthread_mutex_unlock");		\
}
#else
#define INTLOCK {				\
    vmeBusLock();				\
}
#define INTUNLOCK {				\
    vmeBusUnlock();				\
}
#endif

struct TID_A24RegStruct
{
  /* 0x00000 */ volatile unsigned int dataSetup;  /* BM - Name could change */
  /* 0x00004 */ volatile unsigned int enableBits; /* BM - Name could change */
  /* 0x00008 */ volatile unsigned int intSetup;   /* BM - Formerly syncDelay */
  /* 0x0000C */ volatile unsigned int trigDelay;
  /* 0x00010 */ volatile unsigned int adr32;
  /* 0x00014 */ volatile unsigned int daqStatus;  /* BM - Want to change name */
  /* 0x00018 */ volatile unsigned int fiberLatencyMeasurement;
  /* 0x0001C */ volatile unsigned int fiberSyncAlignment;
  /* 0x00020 */ volatile unsigned int liveTimer;
  /* 0x00024 */ volatile unsigned int busyTimer;
  /* 0x00028 */ volatile unsigned int monitor;
  /* 0x0002C */ volatile unsigned int oport;
  /* 0x00030 */ volatile unsigned int GTPStatusA;
  /* 0x00034 */ volatile unsigned int GTPStatusB;
  /* 0x00038 */ volatile unsigned int triggerDataBufferLength;
  /* 0x0003C */ volatile unsigned int TSInputCounter;
  /* 0x00040 */ volatile unsigned int blockStatus[(0x54-0x40)/4];
  /* 0x00054 */ volatile unsigned int boardID;
  /* 0x00058 */ volatile unsigned int blank0[(0x100-0x58)/4]; /* BM - Future Status/Timing regs */
  /* 0x00100 */ volatile unsigned int reset;
  /* 0x00104 */          unsigned int blank1[(0x800-0x104)/4];
  /* 0x00800 */ volatile unsigned int triggerCmdCode;
  /* 0x00804 */ volatile unsigned int softTrig;
  /* 0x00808 */ volatile unsigned int randomTrig;
  /* 0x0080C */          unsigned int blank2[(0x8C0-0x80C)/4];
  /* 0x008C0 */ volatile unsigned int trigTable[(0x900-0x8C0)/4];
  /* 0x00900 */ volatile unsigned int vmeSyncLoad;
  /* 0x00904 */ volatile unsigned int vmeSyncDelay;
  /* 0x00908 */          unsigned int blank4[(0xFFFC-0x908)/4];
  /* 0x0FFFC */ volatile unsigned int eJTAGLoad;
  /* 0x10000 */ volatile unsigned int JTAGPROMBase[(0x20000-0x10000)/4];
  /* 0x20000 */ volatile unsigned int JTAGFPGABase[(0x30000-0x20000)/4];
  /* 0x30000 */ struct   TID_CTPStruct ctp;
  /* 0x40000 */ struct   TID_SDStruct sd;
  /* 0x50000 */ volatile unsigned int i2cVMEP2Base[(0x60000-0x50000)/4];
  /* 0x60000 */ volatile unsigned int serialFlashBase[(0x70000-0x60000)/4];
};

/* Define TID Modes of operation:    Ext trigger - Interrupt mode   0
                                     TS  trigger - Interrupt mode   1
                                     Ext trigger - polling  mode    2 
                                     TS  trigger - polling  mode    3  */
#define TID_READOUT_EXT_INT    0
#define TID_READOUT_TS_INT     1
#define TID_READOUT_EXT_POLL   2
#define TID_READOUT_TS_POLL    3

/* Busy Sources, used by tidSetBusySource */
#define TID_BUSY_SWB           (1<<0)
#define TID_BUSY_P2            (1<<1)
#define TID_BUSY_FP            (1<<2)
#define TID_BUSY_LOOPBACK      (1<<3)
#define TID_BUSY_HFBR1         (1<<4)
#define TID_BUSY_HFBR2         (1<<5)
#define TID_BUSY_HFBR3         (1<<6)
#define TID_BUSY_HFBR4         (1<<7)
#define TID_BUSY_HFBR5         (1<<8)
#define TID_BUSY_HFBR6         (1<<9)
#define TID_BUSY_HFBR7         (1<<10)
#define TID_BUSY_HFBR8         (1<<11)

/* Trigger Sources, used by tidSetTriggerSource  */
#define TID_TRIGGER_P0        0
#define TID_TRIGGER_HFBR1     1
#define TID_TRIGGER_FPTRG     2
#define TID_TRIGGER_TSINPUTS  3
#define TID_TRIGGER_TSREV2    4
#define TID_TRIGGER_RANDOM    5

/* Sync Sources, used by tidSetSyncSource */
#define TID_SYNC_P0           0
#define TID_SYNC_HFBR1        1
#define TID_SYNC_HFBR5        2
#define TID_SYNC_FP           3
#define TID_SYNC_LOOPBACK     4

/* Clock Sources, used by tidSetClockSource */
#define TID_CLOCK_ONBOARD     0
#define TID_CLOCK_HFBR1       1
#define TID_CLOCK_HFBR5       2
#define TID_CLOCK_FP          3

/* TS Inputs, used by tidEnableTSInput/tidDisableTSInput */
#define TID_TSINPUT_1         (1<<0)
#define TID_TSINPUT_2         (1<<1)
#define TID_TSINPUT_3         (1<<2)
#define TID_TSINPUT_4         (1<<3)
#define TID_TSINPUT_5         (1<<4)
#define TID_TSINPUT_6         (1<<5)

/* Define default Interrupt vector and level */
#define TID_INT_VEC      0xb7
#define TID_INT_LEVEL    3

/* i2c data masks - 16bit data default */
#define TID_I2C_DATA_MASK             0x0000ffff
#define TID_I2C_8BIT_DATA_MASK        0x000000ff

/* dataSetup bits and masks */
#define TID_DATASETUP_CRATEID_MASK     0x000000FF
#define TID_DATASETUP_BLOCKSIZE_MASK   0x0000FF00
#define TID_DATASETUP_TRIGSRC_MASK     0x00FF0000
#define TID_DATASETUP_TRIGSRC_P0       (1<<16)
#define TID_DATASETUP_TRIGSRC_HFBR1    (1<<17)
#define TID_DATASETUP_TRIGSRC_LOOPBACK (1<<18)
#define TID_DATASETUP_TRIGSRC_FPTRG    (1<<19)
#define TID_DATASETUP_TRIGSRC_VME      (1<<20)
#define TID_DATASETUP_TRIGSRC_TSINPUTS (1<<21)
#define TID_DATASETUP_TRIGSRC_TSrev2   (1<<22)
#define TID_DATASETUP_TRIGSRC_RND      (1<<23)
#define TID_DATASETUP_SYNCSRC_MASK     0x1F000000
#define TID_DATASETUP_SYNCSRC_P0       (1<<24)
#define TID_DATASETUP_SYNCSRC_HFBR1    (1<<25)
#define TID_DATASETUP_SYNCSRC_HFBR5    (1<<26)
#define TID_DATASETUP_SYNCSRC_FP       (1<<27)
#define TID_DATASETUP_SYNCSRC_LOOPBACK (1<<28)
#define TID_DATASETUP_TWOBLOCK_DUMMY   (1<<29) 
#define TID_DATASETUP_EVENTFORMAT_MASK 0xC0000000
#define TID_DATASETUP_EVENTFORMAT_1    0x00000000
#define TID_DATASETUP_EVENTFORMAT_2    0x40000000
#define TID_DATASETUP_EVENTFORMAT_3    0x80000000
#define TID_DATASETUP_EVENTFORMAT_4    0xC0000000

/* enableBits bits and mask */
#define TID_ENABLEBITS_HFBR_MASK         0x000000FF
#define TID_ENABLEBITS_I2C_DEV_HACK      (1<<8)
#define TID_ENABLEBITS_IRQ_ENABLE        (1<<13)
#define TID_ENABLEBITS_BUSY_INPUT_MASK   0xFFFF0000
#define TID_ENABLEBITS_BUSY_ENABLE_SWA   (1<<16)
#define TID_ENABLEBITS_BUSY_ENABLE_SWB   (1<<17)
#define TID_ENABLEBITS_BUSY_ENABLE_VMEP2 (1<<18)
#define TID_ENABLEBITS_BUSY_ENABLE_TDC   (1<<19)
#define TID_ENABLEBITS_BUSY_ENABLE_ADC   (1<<20)
#define TID_ENABLEBITS_BUSY_ENABLE_FP    (1<<21)
/* #define TID_ENABLEBITS_BUSY_ENABLE_VME   ((1<<22)|(1<<23)) */
/* This is for TD function only -- as a debug/test */
#define TID_ENABLEBITS_BUSY_ENABLE_P2TRIG  (1<<22)
#define TID_ENABLEBITS_BUSY_ENABLE_LOOPBACK (1<<23)
#define TID_ENABLEBITS_BUSY_ENABLE_HFBR1 (1<<24)
#define TID_ENABLEBITS_BUSY_ENABLE_HFBR2 (1<<25)
#define TID_ENABLEBITS_BUSY_ENABLE_HFBR3 (1<<26)
#define TID_ENABLEBITS_BUSY_ENABLE_HFBR4 (1<<27)
#define TID_ENABLEBITS_BUSY_ENABLE_HFBR5 (1<<28)
#define TID_ENABLEBITS_BUSY_ENABLE_HFBR6 (1<<29)
#define TID_ENABLEBITS_BUSY_ENABLE_HFBR7 (1<<30)
#define TID_ENABLEBITS_BUSY_ENABLE_HFBR8 (1<<31)

/* intSetup bits and masks */
#define TID_INTSETUP_LEVEL_MASK       0x00000700
#define TID_INTSETUP_VEC_MASK         0x000000FF
#define TID_INTSETUP_BLOCK_WAIT_THRES 0xFF000000
#define TID_INTSETUP_GTPINPUT_MASK    0x003F0000
#define TID_INTSETUP_CLK_SOURCE_MASK  0x00003000
#define TID_INTSETUP_CLK_ONBOARD      (0)
#define TID_INTSETUP_CLK_HFBR1        (1<<13)
#define TID_INTSETUP_CLK_HFBR5        (1<<12)
#define TID_INTSETUP_CLK_FP           ((1<<12)|(1<<13))

/* trigDelay bits and masks */
#define TID_TRIGDELAY_TRIG1_DELAY_MASK 0x000000FF
#define TID_TRIGDELAY_TRIG1_WIDTH_MASK 0x00001F00
#define TID_TRIGDELAY_TRIG2_DELAY_MASK 0x00FF0000
#define TID_TRIGDELAY_TRIG2_WIDTH_MASK 0x0F000000
#define TID_TRIGDELAY_PRESCALE_MASK    0xF0000000

/* softTrig bits and masks */
#define TID_SOFTTRIG_NEVENTS_MASK     0x0000FFFF
#define TID_SOFTTRIG_PERIOD_MASK      0xFFFF0000

/* adr32 bits and masks */
#define TID_ADR32_ENABLE_BERR    (1<<0)
#define TID_ADR32_ENABLE         (1<<4)
#define TID_ADR32_A32_BASE_MASK  0xFF000000

/* daqStatus bits and masks */
#define TID_DAQSTATUS_BLOCKS_AVAILABLE_MASK 0xFF000000
#define TID_DAQSTATUS_EVENTS_IN_BLOCK_MASK  0x00FF0000
#define TID_DAQSTATUS_BLOCKS_IN_DAQ_MASK    0x0000FFFF

/* reset bits and masks */
#define TID_RESET_VMEREGS      (1<<4)
#define TID_RESET_BLOCK_ACK    (1<<7)
#define TID_RESET_DCM_250      (1<<8)
#define TID_RESET_DCM_125      (1<<9)

/* blockStatus bits and masks */
#define TID_BLOCKSTATUS_NBLOCKS_READY0    0x000000FF
#define TID_BLOCKSTATUS_NBLOCKS_NEEDACK0  0x0000FF00
#define TID_BLOCKSTATUS_NBLOCKS_READY1    0x00FF0000
#define TID_BLOCKSTATUS_NBLOCKS_NEEDACK1  0xFF000000

/* Signals generated from vmeSyncLoad */
#define TID_CLOCK_DCM_RESET    0x11
#define TID_250_CLOCK_RESET    0x22
#define TID_GTP_STATUSB_RESET  0x44
#define TID_TRIGLINK_ENABLE    0x55
#define TID_TRIGLINK_DISABLE   0x77
#define TID_SYNC_RESET         0xDD


/* boardID bits and masks */
#define TID_BOARDID_TYPE            0x71D5
#define TID_BOARDID_TYPE_MASK       0xFFFF0000
#define TID_BOARDID_SLOTID_MASK     0x00007C00

/* Data buffer bits and masks */
#define TID_DUMMY_DATA                      0xDECEA5ED
#define TID_EMPTY_FIFO                      0xF0BAD0F0
#define TID_BLOCK_HEADER_CRATEID_MASK       0xFF000000
#define TID_BLOCK_HEADER_SLOTID_MASK        0x001F0000
#define TID_BLOCK_TRAILER_CRATEID_MASK      0x00FF0000
#define TID_BLOCK_TRAILER_SLOTID_MASK       0x1F000000
#define TID_DATA_BLKNUM_MASK                0x0000FF00
#define TID_DATA_BLKLEVEL_MASK              0x000000FF

/* VXS Payload Port to VME Slot map */
#define MAX_VME_SLOTS 21    /* This is either 20 or 21 */
unsigned short PayloadPort[MAX_VME_SLOTS+1] =
  {
    0,     /* Filler for mythical VME slot 0 */ 
#if MAX_VME_SLOTS == 21
    0,     /* VME Controller */
#endif
    17, 15, 13, 11, 9, 7, 5, 3, 1,  
    0,     /* Switch Slot A - SD */
    0,     /* Switch Slot B - CTP/GTP */
    2, 4, 6, 8, 10, 12, 14, 16, 
    18     /* VME Slot Furthest to the Right - TID */ 
  };

/* Function prototypes */
int  tidInit(unsigned int tAddr, unsigned int mode, int force);
void tidStatus();
int  tidGetFirmwareVersion();
int  tidReset();
int  tidReload();
int  tidClockResync();
int  tidSetCrateID(unsigned int crateID);
int  tidSetBlockLevel(unsigned int blockLevel);
int  tidSetTriggerSource(int trig);
int  tidSetTriggerSourceMask(int trigmask);
int  tidEnableTriggerSource();
int  tidDisableTriggerSource(int trig);
int  tidSetSyncSource(int sync);
int  tidSetEventFormat(int format);
int  tidSoftTrig(unsigned int nevents, unsigned int period);
int  tidSetRandomTrigger();
int  tidDisableRandomTrigger();
int  tidReadBlock(volatile unsigned int *data, int nwrds, int rflag);
int  tidEnableFiber(unsigned int fiber);
int  tidDisableFiber(unsigned int fiber);
int  tidSetBusySource(unsigned int sourcemask, int rFlag);
int  tidSetBusyInputMask(unsigned int mask);
void tidEnableI2CDevHack();
void tidDisableI2CDevHack();
void tidEnableBusError();
void tidDisableBusError();
int  tidPayloadPort2VMESlot(int payloadport);
int  tidPayloadPortMask2VMESlotMask(int payloadport_mask);
int  tidVMESlot2PayloadPort(int vmeslot);
int  tidVMESlotMask2PayloadPortMask(int vmeslot_mask);
int  tidSetPrescale(int prescale);
int  tidGetPrescale();
int  tidSetTriggerPulse(int trigger, int delay, int width);
void tidTrigLinkReset();
void tidSetSyncDelayWidth(unsigned int delay, unsigned int width);
void tidSyncReset();
void tidClockReset();

int  tidIntPoll();
unsigned int  tidGetIntCount();
int  tidIntConnect(unsigned int vector, VOIDFUNCPTR routine, unsigned int arg);
int  tidIntDisconnect();
int  tidAckConnect(VOIDFUNCPTR routine, unsigned int arg);
void tidIntAck();
int  tidIntEnable(int iflag);
void tidIntDisable();
unsigned int  tidBReady();
int  tidSetBlockAckThreshold(unsigned int thres);
int  tidSetupFiberLoopback();
int  tidLoadTriggerTable();
unsigned int tidGetDaqStatus();
int  tidEnableTSInput(unsigned int inpMask);
int  tidDisableTSInput(unsigned int inpMask);
int  tidSetOutputPort(unsigned int set1, unsigned int set2, unsigned int set3, unsigned int set4);
int  tidSetClockSource(unsigned int source);
int  tidVmeTrigger1();
int  tidVmeTrigger2();
void tidSetFiberDelay(unsigned int number, unsigned int offset);
int  tidAddSlave(unsigned int fiber);

#endif /* TIDLIB_H */
