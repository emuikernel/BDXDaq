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
 * SVN: $Rev: 469 $
 *
 *----------------------------------------------------------------------------*/

#define DEVEL
#define CHECKUSERCODE

#ifdef VXWORKS
#include <vxWorks.h>
#include <logLib.h>
#include <taskLib.h>
#include <intLib.h>
#include <iv.h>
#include <semLib.h>
#include <vxLib.h>
#else 
#include "jvme.h"
#endif
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "tidLib.h"

/* Mutex to guard TID read/writes */
pthread_mutex_t   tidMutex = PTHREAD_MUTEX_INITIALIZER;
#define TIDLOCK     if(pthread_mutex_lock(&tidMutex)<0) perror("pthread_mutex_lock");
#define TIDUNLOCK   if(pthread_mutex_unlock(&tidMutex)<0) perror("pthread_mutex_unlock");

/* Register Read/Write prototypes - not static during devel */
#ifndef DEVEL 
static
#endif
unsigned int tidRead(volatile unsigned int *addr);
#ifndef DEVEL 
static
#endif
void tidWrite(volatile unsigned int *addr, unsigned int val);

/* Global Variables */
volatile struct TID_A24RegStruct *TIDp=NULL;  /* pointer to TID memory map */
volatile        unsigned int     *TIDpd=NULL; /* pointer to TID data FIFO */
int tidA24Offset=0;                           /* Difference in CPU A24 Base and VME A24 Base */
int tidA32Base  =0x08000000;                  /* Minimum VME A32 Address for use by TID */
int tidA32Offset=0;                           /* Difference in CPU A32 Base and VME A32 Base */
int tidCrateID=0x59;                          /* Crate ID */
int tidBlockLevel=0;                          /* Block level for TID */
unsigned int TID_DATA_BLOCK_HEADER=0;         /* Block header info, constucted at tidInit */
unsigned int TID_DATA_BLOCK_TRAILER=0;        /* Block trailer info, constucted at tidInit */
unsigned int        tidIntCount    = 0;
unsigned int        tidDaqCount    = 0;       /* Block count from previous update (in daqStatus) */
unsigned int        tidReadoutMode = 0;
unsigned int        tidTriggerSource  = 0;
int                 tidDoAck       = 0;
int                 tidNeedAck     = 0;
static BOOL         tidIntRunning  = FALSE;   /* running flag */
static VOIDFUNCPTR  tidIntRoutine  = NULL;    /* user intererrupt service routine */
static int          tidIntArg      = 0;       /* arg to user routine */
static unsigned int tidIntLevel    = TID_INT_LEVEL;       /* VME Interrupt level */
static unsigned int tidIntVec      = TID_INT_VEC;  /* default interrupt vector */
static VOIDFUNCPTR  tidAckRoutine  = NULL;    /* user trigger acknowledge routine */
static int          tidAckArg      = 0;       /* arg to user trigger ack routine */


/* Interrupt/Polling routine prototypes (static) */
static void tidInt(void);
static void tidPoll(void);
/* static void tidStartPollingThread(void); */
void tidStartPollingThread(void);

/* polling thread pthread and pthread_attr */
pthread_attr_t tidpollthread_attr;
pthread_t      tidpollthread;


/* Library of routines for the SD */
#include "sdLib.c"

/* Library of routines for the CTP */
#include "ctpLib.c"

/*
  tidInit - Initialize the TIDp register space into local memory,
  and setup registers given user input

  ARGs: tSlot  - Slot number of the TID
  mode   - Readout Mode
  0: Polling
  1: Interrupt
  source - Trigger source
  0: Front panel
  1: TS - Fiber
  2: TS2 - Ribbon
  3: Software Regular trigger (requires fiber loopback)
  4: Software Random trigger (requires fiber loopback)

  iFlag   0: Initialize the TID (default behavior)
  1: Do not initialize the board, just setup the pointers
  to the registers
		    

  RETURNS: OK if successful, otherwise ERROR.

*/

int
tidInit(unsigned int tAddr, unsigned int mode, unsigned int source, int iFlag)
{
  unsigned int laddr;
  unsigned int rval, boardID;
#ifdef CHECKUSERCODE
  unsigned int fpgaUserCode;
#endif
  int stat, res;
  int noBoardInit=0;
  int a32Enabled=0;

  /* Check VME address */
  if(tAddr<0 || tAddr>0xffffff)
    {
      printf("%s: ERROR: Invalid VME Address (%d)\n",__FUNCTION__,
	     tAddr);
    }
  if(tAddr==0)
    {
      /* Assume 0 means to use default from GEO (slot 20 or 21, whichever = MAX_VME_SLOTS) */
      tAddr=(MAX_VME_SLOTS)<<19;
    }

  noBoardInit = iFlag&(0x1);

  /* Form VME base address from slot number */
#ifdef VXWORKS
  stat = sysBusToLocalAdrs(0x39,(char *)tAddr,(char **)&laddr);
  if (stat != 0) 
    {
      printf("%s: ERROR: Error in sysBusToLocalAdrs res=%d \n",__FUNCTION__,stat);
      return ERROR;
    } 
  else 
    {
      printf("TID address = 0x%x\n",laddr);
    }
#else
/*   stat = vmeBusToLocalAdrs(0x39,(char *)tAddr,(char **)&laddr); */
  stat = vmeBusToLocalAdrs(0x39,(char *)tAddr,(char **)&laddr);
  if (stat != 0) 
    {
      printf("%s: ERROR: Error in vmeBusToLocalAdrs res=%d \n",__FUNCTION__,stat);
      return ERROR;
    } 
  else 
    {
      printf("TID VME (USER) address = 0x%.8x (0x%.8x)\n",tAddr,laddr);
    }
#endif
  tidA24Offset = laddr-tAddr;

  /* Set Up pointer */
  TIDp = (struct TID_A24RegStruct *)laddr;

  /* Check if TITD board is readable */
#ifdef VXWORKS
  stat = vxMemProbe((char *)(&TIDp->boardID),0,4,(char *)&rval);
#else
  stat = vmeMemProbe((char *)(&TIDp->boardID),4,(char *)&rval);
#endif

  if (stat != 0) 
    {
      printf("%s: ERROR: TID card not addressable\n",__FUNCTION__);
      return(-1);
    }
  else
    {
      /* Check that it is a TID */
#ifdef CHECKID
      if(((rval&TID_BOARDID_TYPE_MASK)>>16) != TID_BOARDID_TYPE) 
	{
	  printf(" ERROR: Invalid Board ID: 0x%x\n",(rval&TID_BOARDID_TYPE_MASK)>>16);
	  return(ERROR);
	}
      /* Check if this is board has a valid slot number */
      boardID =  (rval&TID_BOARDID_SLOTID_MASK)>>10;
      if((boardID <= 0)||(boardID >21)) 
	{
	  printf(" ERROR: Board Slot ID is not in range: %d\n",boardID);
	  return(ERROR);
	}
#endif
    }
  
  /* Get the FPGA User Code and print out some details */
#ifdef CHECKUSERCODE
  fpgaUserCode = tidReadFPGAUserCode();
  if(fpgaUserCode>0)
    {
      printf("  0x%08x",
	     fpgaUserCode);
      printf("  Board type: %d \tFirmware version.revision: %d.%d\n",
	     (fpgaUserCode&0xF000)>>12, (fpgaUserCode&0xFF0)>>4, fpgaUserCode&0xF);
    }
  else
    return ERROR;
#endif

  /* Check if we should exit here, or initialize some board defaults */
  if(noBoardInit)
    {
      return OK;
    }

  /* Reset I2C engine */
  tidWrite(&TIDp->reset,(1<<1));

  /* Set Default Block Level to 1, and default crateID */
  /* Set two block placeholder words and Event format 2 */
  tidBlockLevel=1;
  tidWrite(&TIDp->dataSetup,
	   (tidBlockLevel<<8) | tidCrateID | 
	   TID_DATASETUP_TWOBLOCK_DUMMY | TID_DATASETUP_EVENTFORMAT_2);

/*   if(source==TID_SOURCE_EXT) /\* FIXME: Must be TIMaster *\/ */
/*     { */
/*       tidSetClockSource(0); */
/*     } */
/*   else */
/*     { */
/*       tidSetClockSource(1); /\* FIXME: Must be TIslave *\/ */
/*     } */

  /* Set tidReadoutMode */
  if(mode>1)
    {
      printf("%s: Invalid TID Mode %d\n",
	     __FUNCTION__,mode);
      return ERROR;
    }
  else
    tidReadoutMode = mode;

  /* Set tidTriggerSource */
  if(source>4)
    {
      printf("%s: Invalid TID Trigger Source %d\n",
	     __FUNCTION__,source);
      return ERROR;
    }
  else
    tidTriggerSource = source;

  /* Setup A32 data buffer with library default */
  tidWrite(&TIDp->adr32,
	   (tidA32Base) | (TID_ADR32_ENABLE) );

  a32Enabled = tidRead(&TIDp->adr32)&(TID_ADR32_ENABLE);
  if(!a32Enabled)
    {
      printf("%s: ERROR: Failed to enable A32 Address\n",__FUNCTION__);
      TIDUNLOCK;
      return ERROR;
    }

#ifdef VXWORKS
  res = sysBusToLocalAdrs(0x09,(char *)tidA32Base,(char **)&laddr);
  if (res != 0) 
    {
      printf("%s: ERROR in sysBusToLocalAdrs(0x09,0x%x,&laddr) \n",
	     __FUNCTION__,tidA32Base);
      TIDUNLOCK;
      return(ERROR);
    }
#else
  res = vmeBusToLocalAdrs(0x09,(char *)tidA32Base,(char **)&laddr);
  if (res != 0) 
    {
      printf("%s: ERROR in vmeBusToLocalAdrs(0x09,0x%x,&laddr) \n",
	     __FUNCTION__,tidA32Base);
      TIDUNLOCK;
      return(ERROR);
    }
#endif
  tidA32Offset = laddr - tidA32Base;
  TIDpd = (unsigned int *)(laddr);  /* Set a pointer to the FIFO */

  /* Enable Bus Errors on Block Transfer Terminiation, by default */
  tidEnableBusError();

  /* Set prescale factor to 1, by default */
  tidSetPrescale(0);

  /* Set this to 1, by default... for now */
  tidSetBlockWaitingThreshold(1);

  return OK;
}

/*******************************************************************************
 *
 * tidStatus - Print some status information of the TID to standard out
 *
 * RETURNS: OK if successful, ERROR otherwise
 *
 */

void
tidStatus()
{
  unsigned int dataSetup, enableBits, intSetup,
    adr32, daqStatus, liveTimer, busyTimer, monitor;
  unsigned int blocksReady, triggerSync, tsCounter;

  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return;
    }

  TIDLOCK;
  dataSetup    = tidRead(&TIDp->dataSetup);
  enableBits   = tidRead(&TIDp->enableBits);
  intSetup     = tidRead(&TIDp->intSetup);

  adr32        = tidRead(&TIDp->adr32);
  daqStatus    = tidRead(&TIDp->daqStatus);
  liveTimer    = tidRead(&TIDp->liveTimer);
  busyTimer    = tidRead(&TIDp->busyTimer);
  monitor      = tidRead(&TIDp->monitor);

  blocksReady  = tidRead(&TIDp->blocksReady);
  triggerSync  = tidRead(&TIDp->TSTriggerInfo);
  tsCounter    = tidRead(&TIDp->TSInputCounter);
  TIDUNLOCK;
  
#ifdef VXWORKS
  printf("STATUS for TID at base address 0x%08x \n",
	 (unsigned int) TIDp);
#else
  printf("STATUS for TID at VME (USER) base address 0x%08x (0x%08x) \n",
	 (unsigned int) TIDp - tidA24Offset, (unsigned int) TIDp);
#endif
  printf("A32 Data buffer ");
  if(adr32&TID_ADR32_ENABLE)
    {
      printf("ENABLED at ");
#ifdef VXWORKS
      printf("base address 0x%08x\n",
	     TIDpd);
#else
      printf("VME (USER) base address 0x%08x (0x%08x)\n",
	     (unsigned int)TIDpd - tidA32Offset, (unsigned int)TIDpd);
#endif
    }
  else
    printf("DISABLED\n");


  printf("----------------------------------------- \n");
  printf("Readout Count: %d\n",tidIntCount);
  printf("Registers:\n");
  printf(" dataSetup   = 0x%08x\t", dataSetup);
  printf(" enableBits  = 0x%08x\n", enableBits);
  printf(" intSetup    = 0x%08x\t", intSetup);
  printf(" adr32       = 0x%08x\n", adr32);
  printf(" daqStatus   = 0x%08x\t", daqStatus);
  printf(" liveTimer   = 0x%08x\n", liveTimer);
  printf(" busyTimer   = 0x%08x\t", busyTimer);
  printf(" monitor     = 0x%08x\n", monitor);
  printf("\n");

  printf("Crate ID = 0x%02x\n",(dataSetup&TID_DATASETUP_CRATEID_MASK));
  printf("Block size = %d\n",(dataSetup&TID_DATASETUP_BLOCKSIZE_MASK)>>8);
  printf("Trigger input source = ");
  switch(tidTriggerSource)
    {
    case TID_SOURCE_EXT:
      printf("Front Panel\n");
      break;

    case TID_SOURCE_TS:
      printf("Fiber #1\n");
      break;

    case TID_SOURCE_TS2:
      printf("Trigger Supervisor (rev2)\n");
      break;

    case TID_SOURCE_SOFT:
      printf("Software regular frequency\n");
      break;

    case TID_SOURCE_RANDOM:
      printf("Software random frequency\n");
      break;

    default:
      printf("UNDEFINED\n");
    }
  printf("Event Format = %d\n",(dataSetup&TID_DATASETUP_EVENTFORMAT_MASK)>>30);

  if(enableBits&TID_ENABLEBITS_IRQ_ENABLE)
    printf("Interrupts ENABLED\n");
  else
    printf("Interrupts DISABLED\n");
  printf("  Level = %d   Vector = 0x%02x\n",
	 (intSetup&TID_INTSETUP_LEVEL_MASK)>>8, (intSetup&TID_INTSETUP_VEC_MASK));
  
  if(adr32&TID_ADR32_ENABLE_BERR)
    printf("Bus Errors Enabled\n");

  printf("Blocks ready for readout: %d\n",(blocksReady&0xff)>>24);

  printf("Fiber 5: Blocks to be readout / missing:  %d / %d\n", (triggerSync&0xff),
	 (triggerSync&0xff00)>>8);
  printf("Fiber 6: Blocks to be readout / missing:  %d / %d\n", (triggerSync&0xff0000)>>16,
	 (triggerSync&0xff000000)>>24);
  
  printf("TS input counter %d\n",tsCounter);

  printf("\n\n");

}

/*******************************************************************************
 *
 * tidReadFPGAUserCode - Read the FPGA User Code from the TID
 *
 * RETURNS: User Code if successful, ERROR otherwise
 *
 *  FIXME: Problem with the firmware with this implementation (1Apr2011)
 *
 */

int
tidReadFPGAUserCode()
{
  unsigned int rval=0;
  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }

  TIDLOCK;
  /* reset the VME_to_JTAG engine logic */
  tidWrite(&TIDp->reset,(1<<2));

  /* Reset FPGA JTAG to "reset_idle" state */
  tidWrite(&TIDp->JTAGFPGABase[(0x003C)>>2],0);

  /* enable the user_code readback */
  tidWrite(&TIDp->JTAGFPGABase[(0x092C)>>2],0x3c8);

  /* shift in 32-bit to FPGA JTAG */
  tidWrite(&TIDp->JTAGFPGABase[(0x1F1C)>>2],0);
  
  rval = tidRead(&TIDp->JTAGFPGABase[(0x1F1C)>>2]);
  TIDUNLOCK;

  return rval;
}

/*******************************************************************************
 *
 * tidReload - Reload the firmware on the FPGA
 *
 * RETURNS: OK if successful, ERROR otherwise
 *
 */

int
tidReload()
{
  unsigned int tidBase=0, prombase=0,other1,other2;
  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }

#ifdef DEBUG
  tidBase  = (unsigned int)TIDp;
  prombase = (unsigned int)&TIDp->JTAGPROMBase;
  other1   = (unsigned int)&TIDp->JTAGPROMBase[(0x3c)>>2];
  other2   = (unsigned int)&TIDp->JTAGPROMBase[(0xf2c)>>2];
  printf("%s: tidBase = 0x%08x  prombase = 0x%08x   diff = 0x%08x  other1 = 0x%08x  other2 = 0x%08x\n",
	 __FUNCTION__,
	 tidBase, prombase, prombase-tidBase, other1-tidBase, other2-tidBase);
#endif
  
  TIDLOCK;
  tidWrite(&TIDp->reset,0x04);
  tidWrite(&TIDp->JTAGPROMBase[(0x3c)>>2],0);
  tidWrite(&TIDp->JTAGPROMBase[(0xf2c)>>2],0xEE);
  TIDUNLOCK;

  printf ("%s: \n FPGA Re-Load ! \n",__FUNCTION__);
  return OK;
  
}

/*******************************************************************************
 *
 * tidClockResync - Resync the 250 MHz Clock
 *
 * RETURNS: OK if successful, ERROR otherwise
 *
 */

int
tidClockResync()
{
  unsigned int tidBase=0, prombase=0,other1,other2;
  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }

  
  TIDLOCK;
  tidWrite(&TIDp->vmeSyncLoad,0x33); 
  TIDUNLOCK;

  printf ("%s: \n 250MHz Clock ReSync ! \n",__FUNCTION__);
  return OK;
  
}

int
tidReset()
{
  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }
  
  TIDLOCK;
  tidWrite(&TIDp->reset,TID_RESET_VMEREGS);
  TIDUNLOCK;
  return OK;
}

/*******************************************************************************
 *
 * tidSetCrateID - Set the crate ID that shows up in the data fifo
 *
 * RETURNS: OK if successful, ERROR otherwise
 *
 */

int
tidSetCrateID(unsigned int crateID)
{
  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }

  if( (crateID>0xff) || (crateID==0) )
    {
      printf("%s: ERROR: Invalid crate id (0x%x)\n",__FUNCTION__,crateID);
      return ERROR;
    }
  
  TIDLOCK;
  tidWrite(&TIDp->dataSetup,
	   (tidRead(&TIDp->dataSetup) & ~TID_DATASETUP_CRATEID_MASK)  | crateID);
  tidCrateID = crateID;
  TIDUNLOCK;

  return OK;
  
}

/*******************************************************************************
 *
 * tidSetBlockLevel - Set the number of events per block
 *
 * RETURNS: OK if successful, ERROR otherwise
 *
 */

int
tidSetBlockLevel(unsigned int blockLevel)
{
  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }

  if( (blockLevel>0xff) || (blockLevel==0) )
    {
      printf("%s: ERROR: Invalid Block Level (0x%x)\n",__FUNCTION__,blockLevel);
      return ERROR;
    }

  TIDLOCK;
  tidWrite(&TIDp->dataSetup,
	   (tidRead(&TIDp->dataSetup) & ~TID_DATASETUP_BLOCKSIZE_MASK) | (blockLevel<<8));
  tidBlockLevel = blockLevel;
  TIDUNLOCK;
  return OK;

}

/*******************************************************************************
 *
 * tidSetTriggerSource - Set the trigger source
 *
 *  trig - integer indicating the trigger source
 *         0: P0
 *         1: HFBR#1
 *         2: HFBR#5
 *         3: Front Panel
 *         4: VME (software trigger)
 *         5: Front Panel Trigger Codes (as Supervisor)
 *         6: TS (rev2) 
 *         7: Random
 *
 *  flag - decision to clear previous trigger bits
 *         0: Clear previous bits
 *         1: Do not clear previous bits
 *
 * RETURNS: OK if successful, ERROR otherwise
 *
 */

int
tidSetTriggerSource(int trig,int flag)
{
  unsigned int datasetup=0;

  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }

  if( (trig>7) || (trig<0) )
    {
      printf("%s: ERROR: Invalid Trigger Source (%d).  Must be between 0 and 7.\n",
	     __FUNCTION__,trig);
      return ERROR;
    }

  if( (flag<0) || (flag>1) )
    {
      printf("%s: ERROR: Invalid flag (%d).  Must be between 0 and 1.\n",
	     __FUNCTION__,flag);
      return ERROR;
    }

  TIDLOCK;
  if(flag==0)
    datasetup = tidRead(&TIDp->dataSetup) & ~TID_DATASETUP_TRIGSRC_MASK;
  else
    datasetup = tidRead(&TIDp->dataSetup);

  switch(trig)
    {
    case 0:
      datasetup |= TID_DATASETUP_TRIGSRC_P0;
      break;

    case 1:
      datasetup |= TID_DATASETUP_TRIGSRC_FBR1;
      datasetup |= TID_DATASETUP_TRIGSRC_VME;
      break;

    case 2:
/*       datasetup |= TID_DATASETUP_TRIGSRC_FBR5; */
      // FIXME: remove this option.
      printf("%s: ERROR... don't use this one (%d)\n",__FUNCTION__,trig);
      break;

    case 3:
/*       datasetup |= TID_DATASETUP_TRIGSRC_FP; */
      datasetup |= TID_DATASETUP_TRIGSRC_FPcode;
      datasetup |= TID_DATASETUP_TRIGSRC_VME;
      datasetup |= TID_DATASETUP_TRIGSRC_LOOPBACK;
      break;

    case 4:
      datasetup |= TID_DATASETUP_TRIGSRC_VME | TID_DATASETUP_TRIGSRC_FBR1;
      break;

    case 5:
      datasetup |= TID_DATASETUP_TRIGSRC_FPcode;
      break;

    case 6:
      datasetup |= TID_DATASETUP_TRIGSRC_TSrev2;
      break;

    case 7:
      datasetup |= TID_DATASETUP_TRIGSRC_RND | TID_DATASETUP_TRIGSRC_FBR1;
      break;

    }

  tidWrite(&TIDp->dataSetup,datasetup);
  TIDUNLOCK;

  return OK;
}

/*******************************************************************************
 *
 * tidDisableTriggerSource - Disable trigger sources
 *
 *  trig - integer indicating the trigger source
 *         4Apr2011 - For now... any value disables all
 *
 * RETURNS: OK if successful, ERROR otherwise
 *
 */
int
tidDisableTriggerSource(int trig)
{
  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }

  TIDLOCK;
  if(trig) /* Assume TS mode... disable FP input */
    {
      tidWrite(&TIDp->dataSetup,
	       tidRead(&TIDp->dataSetup) & ~TID_DATASETUP_TRIGSRC_FP);
    }
  else
    {
      tidWrite(&TIDp->dataSetup,
	       tidRead(&TIDp->dataSetup) & ~TID_DATASETUP_TRIGSRC_MASK);
    }
  TIDUNLOCK;

  return OK;

}

/*******************************************************************************
 *
 * tidSetSyncSouce - Set the Sync souce
 *
 *  sync - integer number indicating the sync source
 *         0: P0
 *         1: HFBR#1
 *         2: HFBR#5
 *         3: Front Panel
 *         4: TS Loopback
 *
 * RETURNS: OK if successful, ERROR otherwise
 *
 */

int
tidSetSyncSource(int sync)
{
  unsigned int datasetup=0;

  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }

  if( (sync>4) || (sync<0) )
    {
      printf("%s: ERROR: Invalid Sync Source (%d).  Must be between 0 and 4.\n",
	     __FUNCTION__,sync);
      return ERROR;
    }

  TIDLOCK;
  datasetup = tidRead(&TIDp->dataSetup) & ~TID_DATASETUP_SYNCSRC_MASK;

  switch(sync)
    {
    case 0:
      datasetup |= TID_DATASETUP_SYNCSRC_P0;
      break;

    case 1:
      datasetup |= TID_DATASETUP_SYNCSRC_FBR1;
      break;

    case 2:
      datasetup |= TID_DATASETUP_SYNCSRC_FBR5;
      break;

    case 3:
      datasetup |= TID_DATASETUP_SYNCSRC_FP;
      break;

    case 4:
      datasetup |= TID_DATASETUP_SYNCSRC_LOOPBACK;
      break;

    }

  tidWrite(&TIDp->dataSetup,datasetup);
  TIDUNLOCK;

  return OK;
}

/*******************************************************************************
 *
 * tidSetEventFormat - Set the event format
 *
 *  format - integer number indicating the event format
 *
 *           Description (21Mar2011)
 *           0: Shortest words per trigger
 *           1: TI Timing word enabled
 *           2: TI Status word enabled
 *           3: TI Timing and Status words enabled
 *
 *           Description (7Mar2011) (FIXME)
 *           0: Word 1 - 16bit time, 16bit event number
 *              Word 2 - Status
 *           1: Word 1 - 32bit event number
 *              Word 2 - Status
 *           2: Word 1 - 32bit event number
 *              Word 2 - 32bit time
 *           3: Word 1 - 32bit event number
 *              Word 2 - 16bit time, 16bit status
 *
 * RETURNS: OK if successful, ERROR otherwise
 *
 */

int
tidSetEventFormat(int format)
{
  unsigned int datasetup=0;

  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }

  if( (format>3) || (format<0) )
    {
      printf("%s: ERROR: Invalid Event Format (%d).  Must be between 0 and 3.\n",
	     __FUNCTION__,format);
      return ERROR;
    }

  TIDLOCK;
  datasetup = tidRead(&TIDp->dataSetup) & ~TID_DATASETUP_EVENTFORMAT_MASK;

  switch(format)
    {
    case 0:
      datasetup |= TID_DATASETUP_EVENTFORMAT_1;
      break;

    case 1:
      datasetup |= TID_DATASETUP_EVENTFORMAT_2;
      break;

    case 2:
      datasetup |= TID_DATASETUP_EVENTFORMAT_3;
      break;

    case 3:
      datasetup |= TID_DATASETUP_EVENTFORMAT_4;
      break;

    }

  tidWrite(&TIDp->dataSetup,datasetup);
  datasetup = tidRead(&TIDp->dataSetup);

  TIDUNLOCK;

  return OK;
}

/*******************************************************************************
 *
 * tidSoftTrig - Set and enable the "software" trigger
 *
 *  nevents - integer number of events to trigger
 *  period  - integer input time
 *
 * RETURNS: OK if successful, ERROR otherwise
 *
 */

int
tidSoftTrig(unsigned int nevents, unsigned int period)
{
  unsigned int periodMax=0xFFFF;
  int time=0;

  if(TIDp==NULL)
    {
      logMsg("\ntidSoftTrig: ERROR: TID not initialized\n",1,2,3,4,5,6);
      return ERROR;
    }

  if(nevents>0xFFFF)
    {
      logMsg("\ntidSoftTrig: ERROR: nevents must be less than %d\n",0xFFFF,2,3,4,5,6);
      return ERROR;
    }
  if(period>0xFFFF)
    {
      logMsg("\ntidSoftTrig: ERROR: period must be less than %d ns\n",
	     periodMax,2,3,4,5,6);
      return ERROR;
    }

  time = (80+160*(period<<2)*2048);

  logMsg("\ntidSoftTrig: INFO: Setting software trigger for %d nevents with period of 0x%x (%d)\n",
	 nevents,period,time,4,5,6);
  logMsg("\n softTrig = 0x%08x\n",(period<<16) | (nevents),2,3,4,5,6);

  TIDLOCK;
  tidWrite(&TIDp->softTrig,
	   (period<<16) | (nevents));
  TIDUNLOCK;

  return OK;

}

/*******************************************************************************
 *
 * tidSetRandomTrigger - Set the parameters of the random internal trigger
 *
 * RETURNS: OK if successful, ERROR otherwise.
 *
 */

int
tidSetRandomTrigger()
{
  unsigned int word = (1<<3) | (7); // ~4kHz, enable

  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }

  TIDLOCK;
  tidWrite(&TIDp->randomTrig,word | (word<<4));
  TIDUNLOCK;
  return OK;
}

int
tidDisableRandomTrigger()
{
  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }

  TIDLOCK;
  tidWrite(&TIDp->randomTrig,0);
  TIDUNLOCK;
  return OK;
}



/*******************************************************************************
 *
 * tidReadBlock - Read a block of events from the TID
 *
 *    data  - local memory address to place data
 *    nwrds - Max number of words to transfer
 *    rflag - Readout Flag
 *              0 - programmed I/O from the specified board
 *              1 - DMA transfer using Universe/Tempe DMA Engine 
 *                    (DMA VME transfer Mode must be setup prior)
 *
 * RETURNS: Number of words transferred to data if successful, ERROR otherwise
 *
 */

int
tidReadBlock(volatile unsigned int *data, int nwrds, int rflag)
{
  int ii, dummy=0;
  int dCnt, retVal, xferCount;// blknum;
  volatile unsigned int *laddr;
  unsigned int vmeAdr, val;// bhead;

  if(TIDp==NULL)
    {
      logMsg("\ntidReadBlock: ERROR: TID not initialized\n",1,2,3,4,5,6);
      return ERROR;
    }

  if(TIDpd==NULL)
    {
      logMsg("\ntidReadBlock: ERROR: TID A32 not initialized\n",1,2,3,4,5,6);
      return ERROR;
    }

  if(data==NULL) 
    {
      logMsg("\ntidReadBlock: ERROR: Invalid Destination address\n",0,0,0,0,0,0);
      return(ERROR);
    }

  TIDLOCK;
  if(rflag >= 1)
    { /* Block transfer */

      /* Assume that the DMA programming is already setup. 
	 Don't Bother checking if there is valid data - that should be done prior
	 to calling the read routine */
      
      /* Check for 8 byte boundary for address - insert dummy word (Slot 0 FADC Dummy DATA)*/
      if((unsigned long) (data)&0x7) 
	{
#ifdef VXWORKS
	  *data = TID_DUMMY_DATA;
#else
	  *data = LSWAP(TID_DUMMY_DATA);
#endif
	  dummy = 1;
	  laddr = (data + 1);
	} 
      else 
	{
	  dummy = 0;
	  laddr = data;
	}
      
      vmeAdr = (unsigned int)(TIDpd) - tidA32Offset;

#ifdef VXWORKS
      retVal = sysVmeDmaSend((UINT32)laddr, vmeAdr, (nwrds<<2), 0);
#else
      retVal = vmeDmaSend((UINT32)laddr, vmeAdr, (nwrds<<2));
#endif
      if(retVal |= 0) 
	{
	  logMsg("\ntidReadBlock: ERROR in DMA transfer Initialization 0x%x\n",retVal,0,0,0,0,0);
	  TIDUNLOCK;
	  return(retVal);
	}

      /* Wait until Done or Error */
#ifdef VXWORKS
      retVal = sysVmeDmaDone(10000,1);
#else
      retVal = vmeDmaDone();
#endif

      if(retVal > 0)
	{
#ifdef VXWORKS
	  xferCount = (nwrds - (retVal>>2) + dummy); /* Number of longwords transfered */
#else
	  xferCount = ((retVal>>2) + dummy); /* Number of longwords transfered */
#endif
	  TIDUNLOCK;
	  return(xferCount);
	}
      else if (retVal == 0) 
	{
#ifdef VXWORKS
	  logMsg("\ntidReadBlock: WARN: DMA transfer terminated by word count 0x%x\n",
		 nwrds,0,0,0,0,0);
#else
	  logMsg("\ntidReadBlock: WARN: DMA transfer returned zero word count 0x%x\n",
		 nwrds,0,0,0,0,0,0);
#endif
	  TIDUNLOCK;
	  return(nwrds);
	}
      else 
	{  /* Error in DMA */
#ifdef VXWORKS
	  logMsg("\ntidReadBlock: ERROR: sysVmeDmaDone returned an Error\n",
		 0,0,0,0,0,0);
#else
	  logMsg("\ntidReadBlock: ERROR: vmeDmaDone returned an Error\n",
		 0,0,0,0,0,0);
#endif
	  TIDUNLOCK;
	  return(retVal>>2);
	  
	}
    }
  else
    { /* Programmed IO */
      dCnt = 0;
      /* Read Block Header - should be first word */
      /*       bhead = (unsigned int) *TIDpd;  */
      /* #ifndef VXWORKS */
      /*       bhead = LSWAP(bhead); */
      /* #endif */
      /*       if( ((bhead&TID_BLOCK_HEADER_CRATEID_MASK)>>24==tidCrateID) && */
      /* 	  ((bhead&TID_BLOCK_HEADER_SLOTID_MASK)>>16==tidSlotID))  */
      /* 	{ */
      /* 	  blknum = bhead&TID_DATA_BLKNUM_MASK; */

      /* #ifdef VXWORKS */
      /* 	  data[dCnt] = bhead; */
      /* #else */
      /* 	  data[dCnt] = LSWAP(bhead); /\* Swap back to little-endian *\/ */
      /* #endif */
      /* 	  dCnt++; */
      /* 	} */
      /*       else */
      /* 	{ */
      /* 	  /\* We got bad data - Check if there is any data at all *\/ */
      /* 	  if( bhead == TID_EMPTY_FIFO )  */
      /* 	    { */
      /* 	      logMsg("\ntidReadBlock: FIFO Empty (0x%08x)\n",bhead,0,0,0,0,0); */
      /* 	      TIDUNLOCK; */
      /* 	      return(0); */
      /* 	    }  */
      /* 	  else  */
      /* 	    { */
      /* 	      logMsg("\ntidReadBlock: ERROR: Invalid Header Word 0x%08x\n",bhead,0,0,0,0,0); */
      /* 	      TIDUNLOCK; */
      /* 	      return(ERROR); */
      /* 	    } */
      /*     } */

      ii=0;
      while(ii<nwrds) 
	{
	  val = (unsigned int) *TIDpd;
#ifndef VXWORKS
	  val = LSWAP(val);
#endif
	  /* 	  if( ((val&TID_BLOCK_TRAILER_CRATEID_MASK)>>16==tidCrateID) && */
	  /* 	      ((val&TID_BLOCK_TRAILER_SLOTID_MASK)>>24==tidSlotID))  */
	  if(val==TID_EMPTY_FIFO)
	    break;
#ifndef VXWORKS
	  val = LSWAP(val);
#endif
	  data[ii] = val;
	  ii++;
	}
      ii++;
      dCnt += ii;

      TIDUNLOCK;
      return(dCnt);
    }

  TIDUNLOCK;
  return OK; /* OK?  Are you OK?  How did you get here? */
}

/* tidClearDataBuffer()
 *  - temporary routine to clear the a32 data buffer 
 */

void
tidClearDataBuffer()
{
  volatile unsigned int data=0;
  int xtra_words=0;
  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return;
    }
  
  if(TIDpd==NULL)
    {
      printf("%s: ERROR: TID A32 not initialized\n",__FUNCTION__);
      return;
    }
  
  while(data!=0xf0bad0f0)
    {
#ifdef VXWORKS
      data = (unsigned int) *TIDpd;
#else
      data = (unsigned int) TIDpd[xtra_words];
      data = LSWAP(data);
#endif
      xtra_words++;
    }
  if(xtra_words>1)
    printf("%s: Extra words in data buffer = %d\n",
	   __FUNCTION__,xtra_words);

}

/* tidEnableFiber/tidDisableFiber
 *  - Enable/Disable Fiber transceiver
 *    mask = Bits 0-7 indicative of the transceiver to enable/disable
 *  Returns contents of enableBits
 * ... needs testing
 */

int
tidEnableFiber(unsigned int mask)
{
  unsigned int rval;

  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }

  TIDLOCK;
  tidWrite(&TIDp->enableBits,
	   tidRead(&TIDp->enableBits) | (mask & TID_ENABLEBITS_HFBR_MASK) );
  rval = tidRead(&TIDp->enableBits);
  TIDUNLOCK;

  return rval;
  
}

int
tidDisableFiber(unsigned int mask)
{
  unsigned int rval;

  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }

  TIDLOCK;
  tidWrite(&TIDp->enableBits,
	   tidRead(&TIDp->enableBits) & ~(mask & TID_ENABLEBITS_HFBR_MASK) );
  rval = tidRead(&TIDp->enableBits);
  TIDUNLOCK;

  return rval;
  
}

int
tidSetBusyInputMask(unsigned int mask)
{
  unsigned int rval;

  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }

  TIDLOCK;
  rval = tidRead(&TIDp->enableBits) & ~(TID_ENABLEBITS_BUSY_INPUT_MASK);
  rval |= (mask);
  tidWrite(&TIDp->enableBits, rval);
  TIDUNLOCK;

  return rval;
}

/* tidEnableI2CDevHack / tidDisableI2CDevHack
 *   - Enable/Disable I2C Device address hack
 *     Enable if I2C Device address is non-standard (as it is, as of Dec 2010)
 */

void
tidEnableI2CDevHack()
{
  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return;
    }

  TIDLOCK;
  tidWrite(&TIDp->enableBits,
	   tidRead(&TIDp->enableBits) | (TID_ENABLEBITS_I2C_DEV_HACK) );
  TIDUNLOCK;

}

void
tidDisableI2CDevHack()
{
  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return;
    }

  TIDLOCK;
  tidWrite(&TIDp->enableBits,
	   tidRead(&TIDp->enableBits) & ~(TID_ENABLEBITS_I2C_DEV_HACK) );
  TIDUNLOCK;

}

/* tidEnableBusError / tidDisableBusError
 *  - Enable/Disable Bus Errors to terminate Block Reads
 */

void
tidEnableBusError()
{

  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return;
    }

  TIDLOCK;
  tidWrite(&TIDp->adr32,
	   tidRead(&TIDp->adr32) | (TID_ADR32_ENABLE_BERR) );
  TIDUNLOCK;

}

void
tidDisableBusError()
{

  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return;
    }

  TIDLOCK;
  tidWrite(&TIDp->adr32,
	   tidRead(&TIDp->adr32) & ~(TID_ADR32_ENABLE_BERR) );
  TIDUNLOCK;

}


int
tidBuildSlotMask(int slot, int *outmask)
{
/*   static int outmask=0; */
  
/*   if(clear) */
/*     outmask = 0; */

  (*outmask) |= (1<<(slot-1));

  return *outmask;
}

/*
  tidPayloadPort2VMESlot
  - Routine to return the VME slot, provided the VXS payload port
  - This does not access the bus, just a map in the library.
  - FIXME: Make sure this works.
*/
int
tidPayloadPort2VMESlot(int payloadport)
{
  int rval=0;
  int islot;
  if(payloadport<1 || payloadport>18)
    {
      printf("%s: ERROR: Invalid payloadport %d\n",
	     __FUNCTION__,payloadport);
      return ERROR;
    }

  for(islot=1;islot<MAX_VME_SLOTS;islot++)
    {
      if(payloadport == PayloadPort[islot])
	{
	  rval = islot;
	  break;
	}
    }

  if(rval==0)
    {
      printf("%s: ERROR: Unable to find VME Slot from Payload Port %d\n",
	     __FUNCTION__,payloadport);
      rval=ERROR;
    }

  return rval;
}

/*
  tidPayloadPortMask2VMESlotMask
  - Routine to return the VME slot MASK, provided the VXS payload port MASK
  - Convention: 
  bit 0: Slot/Port 1
  bit 1: Slot/Port 2
  ...
  bit 5: Slot/Port 6
  .etc.
  FIXME: Make sure this works.
*/
int
tidPayloadPortMask2VMESlotMask(int payloadport_mask)
{
  int iport=0;
  int rval=0;
  int vmeslot_mask=0;

  if(payloadport_mask >= 0x20000)
    {
      printf("%s: ERROR: Invalid payloadport_mask (0x%x).  Includes port > 17.",
	     __FUNCTION__,payloadport_mask);
      return ERROR;
    }

  for(iport=1;iport<=17;iport++)
    {
      if(payloadport_mask & (1<<iport))
	{
	  rval = tidPayloadPort2VMESlot(iport);
	  if(rval!=ERROR)
	    {
	      printf("iport = %d   rval = %d\n",iport,rval);
	      if(rval!=0)
		vmeslot_mask |= (1<<(rval-1));
	    }
	  else
	    {
	      printf("%s: tidPayloadPort2VMEslot(%d) returned ERROR\n",
		     __FUNCTION__,iport);
	      return ERROR;
	    }
	}
    }

  return vmeslot_mask;
}

/*
  tidVMESlot2PayloadPort
  - Routine to return the VXS Payload Port provided the VME slot
  - This does not access the bus, just a map in the library.
  - FIXME: Make sure this works.
*/
int
tidVMESlot2PayloadPort(int vmeslot)
{
  int rval=0;
  if(vmeslot<1 || vmeslot>MAX_VME_SLOTS) 
    {
      printf("%s: ERROR: Invalid VME slot %d\n",
	     __FUNCTION__,vmeslot);
      return ERROR;
    }

  rval = (int)PayloadPort[vmeslot];

  if(rval==0)
    {
      printf("%s: ERROR: Unable to find Payload Port from VME Slot %d\n",
	     __FUNCTION__,vmeslot);
      rval=ERROR;
    }

  return rval;

}

/*
  tidVMESlotMask2PayloadPortMask
  - Routine to return the VXS payload port MASK, provided the VME slot MASK
  - Convention: 
  bit 0: Slot/Port 1
  bit 1: Slot/Port 2
  ...
  bit 5: Slot/Port 6
  .etc.
  FIXME: Make sure this works.
*/
int
tidVMESlotMask2PayloadPortMask(int vmeslot_mask)
{
  int islot=0;
  int rval=0;
  int payloadport_mask=0;

  if(vmeslot_mask >= (1<<(MAX_VME_SLOTS-1)) )
    {
      printf("%s: ERROR: Invalid vmeslot_mask (0x%x).  Includes slot > %d.\n",
	     __FUNCTION__,vmeslot_mask,MAX_VME_SLOTS);
      return ERROR;
    }

  for(islot=1;islot<=MAX_VME_SLOTS;islot++)
    {
      rval = tidVMESlot2PayloadPort(islot);
      if(rval!=ERROR)
	{
	  if(rval!=0)
	    payloadport_mask |= (1<<(rval-1));
	}
      else
	{
	  printf("%s: ERROR: tidVMESlot2PayloadPort(%d) returned ERROR\n",
		 __FUNCTION__,islot);
	  return ERROR;
	}
    }

  return payloadport_mask;
}

/*
  tidSetPrescale - Set the prescale factor for the external trigger
  prescale - factor for prescale.  Actual prescale will be set
  to 2^{prescale}
  Max {prescale} available is 15 or a factor of 32768
		  
  RETURNS: OK if successful, otherwise ERROR.

*/

int
tidSetPrescale(int prescale)
{
  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }

  if(prescale<0 || prescale>15)
    {
      printf("%s: ERROR: Invalid prescale (%d).  Must be between 0 and 7.",
	     __FUNCTION__,prescale);
      return ERROR;
    }

  TIDLOCK;
  tidWrite(&TIDp->trigDelay,
	   ((tidRead(&TIDp->trigDelay) & ~(TID_TRIGDELAY_PRESCALE_MASK))
	    | (prescale<<28)) );
  TIDUNLOCK;

  return OK;
}

int
tidGetPrescale()
{
  int rval;
  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }

  TIDLOCK;
  rval = (tidRead(&TIDp->trigDelay) & (TID_TRIGDELAY_PRESCALE_MASK))>>28;
  TIDUNLOCK;

  return rval;
}

/*
  tidSetTriggerPulse - Set the characteristics of a specified trigger
  trigger:  1: set chars for trigger 1
  2: set chars for trigger 2 (playback trigger)
  delay:    delay in units of 4ns
  width:    pulse width in units of 4ns

  RETURNS: OK if successful, otherwise ERROR

*/

int
tidSetTriggerPulse(int trigger, int delay, int width)
{
  unsigned int rval=0;
  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }

  if(trigger<1 || trigger>2)
    {
      printf("%s: ERROR: Invalid trigger (%d).  Must be 1 or 2.\n",
	     __FUNCTION__,trigger);
      return ERROR;
    }
  if(delay<0 || delay>0xffff)
    {
      printf("%s: ERROR: Invalid delay (%d).  Must be less than 65536\n",
	     __FUNCTION__,delay);
      return ERROR;
    }
  if(width<0 || width>0xf)
    {
      printf("%s: ERROR: Invalid width (%d).  Must be less than 16\n",
	     __FUNCTION__,width);
    }

  TIDLOCK;
  if(trigger==1)
    {
      rval = tidRead(&TIDp->trigDelay) & 
	~(TID_TRIGDELAY_TRIG1_DELAY_MASK | TID_TRIGDELAY_TRIG1_WIDTH_MASK) ;
      rval |= ( (delay) | (width<<8) );
      tidWrite(&TIDp->trigDelay, rval);
    }
  if(trigger==2)
    {
      rval = tidRead(&TIDp->trigDelay) & 
	~(TID_TRIGDELAY_TRIG2_DELAY_MASK | TID_TRIGDELAY_TRIG2_WIDTH_MASK) ;
      rval |= ( (delay<<16) | (width<<24) );
      tidWrite(&TIDp->trigDelay, rval);
    }
  TIDUNLOCK;
  
  return OK;
}

void
tidSetSyncDelayWidth(unsigned int delay, unsigned int width)
{
  if(TIDp == NULL) 
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return;
    }

  if(width>0x3f)
    {
      printf("%s: ERROR: Invalid width (%d)\n",__FUNCTION__,width);
      return;
    }

  if(delay>0x7f)
    {
      printf("%s: ERROR: Invalid delay (%d)\n",__FUNCTION__,delay);
      return;
    }

  printf("%s: Setting Sync delay = 0x%x   width = 0x%x\n",
	 __FUNCTION__,delay,width);

  /* FIXME: Be a bit more elegant, next time */
  TIDLOCK;
  tidWrite(&TIDp->vmeSyncDelay,delay | (width<<8));
  TIDUNLOCK;

}
void 
tidTrigLinkReset()
{
  if(TIDp == NULL) 
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return;
    }
  
  TIDLOCK;
  /*   WriteStart: reset the buffer write counter, and disable the trigger data */
  tidWrite(&TIDp->vmeSyncLoad,0x77); 

  taskDelay(1);
  /*   just in case that the sync is later than TrgData */
  tidWrite(&TIDp->vmeSyncLoad,0x77);
  taskDelay(1);
  /*   taskDelay(60); */

  /* Sync width (0x1f) and Sync delay (0x54) before being serialized */
/*   tidWrite(&TIDp->vmeSyncDelay,0x54 | (0x3f<<8)); */
  /*   taskDelay(60); */

  /* ReadStart: Start the readbuffer and enable the TrgData */
  tidWrite(&TIDp->vmeSyncLoad,0x55); 
  /*   taskDelay(60); */

  TIDUNLOCK;

  printf ("%s: Trigger Data Link was reset.\n",__FUNCTION__);
}

void
tidSyncReset()
{
  if(TIDp == NULL) 
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return;
    }
  
  TIDLOCK;
  /*   tidWrite(&TIDp->vmeSyncLoad,0x77); */
  /* Sync width (0x1f) and Sync delay (0x54) before being serialized */
/*   tidWrite(&TIDp->vmeSyncDelay,0x54 | (0x3f<<8)); */
/*   tidWrite(&TIDp->vmeSyncDelay,0x54 | (0x3f<<8)); */
  tidWrite(&TIDp->vmeSyncLoad,0xDD); 
  /*   tidWrite(&TIDp->vmeSyncLoad,0x55); */

  TIDUNLOCK;

  printf ("%s: RESET sent over Sync Link.\n",__FUNCTION__);

}

void
tidClockReset()
{
  if(TIDp == NULL) 
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return;
    }
  
  TIDLOCK;
  /*   tidWrite(&TIDp->vmeSyncLoad,0x77); */
  tidWrite(&TIDp->vmeSyncLoad,0x22); 
  /*   tidWrite(&TIDp->vmeSyncLoad,0x55); */

  TIDUNLOCK;

  printf ("%s: RESET sent over Sync Link.\n",__FUNCTION__);

}

/*
  tidSetIntCount - Temp routine to set the tidIntCount to the current
  value in the daqStatus register 
*/
int
tidSetIntCount()
{
  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }

  printf("%s: Set tidIntCount from %d to ",__FUNCTION__,tidIntCount);

  TIDLOCK;
  tidIntCount = tidRead(&TIDp->daqStatus) & (TID_DAQSTATUS_BLOCKS_IN_DAQ_MASK);
  TIDUNLOCK;

  printf("%d\n",tidIntCount);

  return tidIntCount;

}

/*************************************************************

 TID Interrupt/Polling routines

*************************************************************/

/*
  tidIntPoll
  - Routine that returns 1 if Block is available in buffer, otherwise 0.

*/

int 
tidIntPoll()
{
  unsigned int sval=0;

  TIDLOCK;
  sval = (tidRead(&TIDp->daqStatus) & TID_DAQSTATUS_BLOCKS_AVAILABLE_MASK)>>24; 
  /*   if( (tidIntCount==0) && (sval==0) ) */
  /*     { */
  /*       TIDUNLOCK; */
  /*       return 0; */
  /*     } */
  if( sval )
    {
      tidDaqCount = sval;
      tidIntCount++;
      TIDUNLOCK;
      return (1);
    } 
  else 
    {
      TIDUNLOCK;
      return (0);
    }

}

unsigned int
tidGetIntCount()
{
  return(tidIntCount);
}

/*************************************************************
 Interrupt/Polling routines
*************************************************************/

/*
  tidInt
  - Default interrupt handler
  Handles the TID interrupt.  Calls a user defined routine,
  if it was connected with tidIntConnect()
    
  FIXME: Write this.

*/

static void
tidInt(void)
{
  tidIntCount++;

  INTLOCK;

  if (tidIntRoutine != NULL)	/* call user routine */
    (*tidIntRoutine) (tidIntArg);

  /* Acknowledge trigger */
  if(tidDoAck==1) 
    {
      tidIntAck();
    }
  INTUNLOCK;

}

/*
  tidPoll
  - Default Polling Server Thread
  Handles the polling of latched triggers.  Calls a user
  defined routine if was connected with tidIntConnect.

  FIXME: Check tidIntPoll call when the reg/bits are done.

*/

static void
tidPoll(void)
{
  int tiddata;
#ifndef VXWORKS
  int policy=0;
  struct sched_param sp;

  /* Set scheduler and priority for this thread 
     FIXME: Does this only work in Linux? */
  policy=SCHED_FIFO;
  sp.sched_priority=40;
  printf("%s: Entering polling loop...\n",__FUNCTION__);
  pthread_setschedparam(pthread_self(),policy,&sp);
  pthread_getschedparam(pthread_self(),&policy,&sp);
  printf ("%s: INFO: Running at %s/%d\n",__FUNCTION__,
	  (policy == SCHED_FIFO ? "FIFO"
	   : (policy == SCHED_RR ? "RR"
	      : (policy == SCHED_OTHER ? "OTHER"
		 : "unknown"))), sp.sched_priority);  
#endif 

  while(1) 
    {

      // If still need Ack, don't test the Trigger Status
      if(tidNeedAck) continue;

      tiddata = 0;
	  
      /* FIXME: So far, tidIntPoll never returns ERROR */
      tiddata = tidIntPoll();
      if(tiddata == ERROR) 
	{
	  printf("%s: ERROR: tidIntPoll returned ERROR.\n",__FUNCTION__);
	  break;
	}


      if(tiddata && tidIntRunning) // FIXME: Not sure if tidIntRunning here is correct
	{
	  INTLOCK; // FIXME: Causes race condition with tidIntDisconnect if tidIntRunning not checked
	
	  if (tidIntRoutine != NULL)	/* call user routine */
	    (*tidIntRoutine) (tidIntArg);
	
	  /* Write to TID to Acknowledge Interrupt */	  
	  if(tidDoAck==1) 
	    {
	      tidIntAck();
	    }
	  INTUNLOCK;
	}
    
      pthread_testcancel();

    }
  printf("%s: Read ERROR: Exiting Thread\n",__FUNCTION__);
  pthread_exit(0);

}

/*
  tidStartPollingThread
  - Routine that launches tidPoll in its own thread 

*/

/* static void */
void
tidStartPollingThread(void)
{
  int ptid_status;

  ptid_status = 
    pthread_create(&tidpollthread,
		   NULL,
		   (void*(*)(void *)) tidPoll,
		   (void *)NULL);
  if(ptid_status!=0) 
    {						
      printf("%s: ERROR: TID Polling Thread could not be started.\n",
	     __FUNCTION__);	
      printf("\t pthread_create returned: %d\n",ptid_status);
    }

}


/*
  tidIntConnect 
  - Connect a user routine to the TIR Interrupt or
  latched trigger, if polling.

  FIXME: Waiting for Register and Bit definitions

*/

int
tidIntConnect(unsigned int vector, VOIDFUNCPTR routine, unsigned int arg)
{
  
#ifndef VXWORKS
  int status;
#endif

  if(TIDp == NULL) 
    {
      printf("%s: ERROR: TIR not initialized\n",__FUNCTION__);
      return(ERROR);
    }


#ifdef VXWORKS
  /* Disconnect any current interrupts */
  if((intDisconnect(tidIntVec) !=0))
    printf("%s: Error disconnecting Interrupt\n",__FUNCTION__);
#endif

  tidIntCount = 0;
  tidDoAck = 1;

  /* Set Vector and Level */
  if((vector < 0xFF)&&(vector > 0x40)) 
    {
      tidIntVec = vector;
    }
  else
    {
      tidIntVec = TID_INT_VEC;
    }

  TIDLOCK;
  tidWrite(&TIDp->intSetup, 
	   (tidRead(&TIDp->intSetup) & ~(TID_INTSETUP_LEVEL_MASK | TID_INTSETUP_VEC_MASK) ) 
	   | (tidIntLevel<<8) | tidIntVec );
  printf("intsetup = 0x%08x\n",tidRead(&TIDp->intSetup));
  TIDUNLOCK;

  if(tidReadoutMode==TID_READOUT_INT)
    {
#ifdef VXWORKS
      intConnect(INUM_TO_IVEC(tidIntVec),tidInt,arg);
#else
      status = vmeIntConnect (tidIntVec, tidIntLevel,
			      tidInt,arg);
      if (status != OK) {
	printf("vmeIntConnect failed\n");
	TIDUNLOCK;
	return(ERROR);
      }
#endif  
    }

  printf("%s: INFO: Interrupt Vector = 0x%x  Level = %d\n",
	 __FUNCTION__,tidIntVec,tidIntLevel);

  if(routine) 
    {
      tidIntRoutine = routine;
      tidIntArg = arg;
    }
  else
    {
      tidIntRoutine = NULL;
      tidIntArg = 0;
    }

  return(OK);

}

/*
  tidIntDisconnect
  - Disable interrupts or kill the polling service thread

  FIXME: Waiting for regs and bits 

*/
int
tidIntDisconnect()
{
#ifndef VXWORKS
  int status;
#endif
  void *res;


  if(TIDp == NULL) 
    {
      printf("%s: ERROR: TIR not initialized\n",__FUNCTION__);
      return ERROR;
    }

  if(tidIntRunning) 
    {
      /* FIXME: Write this routine? */
      printf("%s: ERROR: TID is Enabled - Call tidIntDisable() first\n",
	     __FUNCTION__);
      return ERROR;
    }

  INTLOCK;

  switch (tidReadoutMode) 
    {
    case TID_READOUT_INT:

#ifdef VXWORKS
      /* Disconnect any current interrupts */
      sysIntDisable(tidIntLevel);
      if((intDisconnect(tidIntVec) !=0))
	printf("%s: Error disconnecting Interrupt\n",__FUNCTION__);
#else
      status = vmeIntDisconnect(tidIntLevel);
      if (status != OK) 
	{
	  printf("vmeIntDisconnect failed\n");
	}
#endif
      break;
    case TID_READOUT_POLL:
      if(tidpollthread) 
	{
	  if(pthread_cancel(tidpollthread)<0) 
	    perror("pthread_cancel");
	  if(pthread_join(tidpollthread,&res)<0)
	    perror("pthread_join");
	  if (res == PTHREAD_CANCELED)
	    printf("%s: Polling thread canceled\n",__FUNCTION__);
	  else
	    printf("%s: ERROR: Polling thread NOT canceled\n",__FUNCTION__);

	}
      break;
    default:
      break;
    }

  INTUNLOCK;

  printf("%s: Disconnected\n",__FUNCTION__);

  return OK;
  
}

/*
  tidAckConnect
  - Connect a user routine to be executed instead of the default 
  TID interrupt/trigger latching acknowledge prescription
*/
int
tidAckConnect(VOIDFUNCPTR routine, unsigned int arg)
{
  if(routine)
    {
      tidAckRoutine = routine;
      tidAckArg = arg;
    }
  else
    {
      printf("%s: WARN: routine undefined.\n",__FUNCTION__);
      tidAckRoutine = NULL;
      tidAckArg = 0;
      return ERROR;
    }
  return OK;
}

/*
  tidIntAck
  - Acknowledge an interrupt or latched trigger.  This "should" effectively 
  release the "Busy" state of the TID.
  Execute a user defined routine, if it is defined.  Otherwise, use
  a default prescription.

  FIXME: Still need the register and bit
*/
void
tidIntAck()
{
  if(TIDp == NULL) {
    logMsg("%s: ERROR: TIR not initialized\n",__FUNCTION__,0,0,0,0,0);
    return;
  }

  if (tidAckRoutine != NULL)
    {
      /* Execute user defined Acknowlege, if it was defined */
      TIDLOCK;
      (*tidAckRoutine) (tidAckArg);
      TIDUNLOCK;
    }
  else
    {
      TIDLOCK;
      tidNeedAck = 0;
      tidDoAck = 1;
      tidWrite(&TIDp->reset,TID_RESET_BLOCK_ACK);
      TIDUNLOCK;
    }

}

/*
  tidIntEnable
  - Enable interrupts or latching triggers (depending on set TID mode)
  
  if iflag==1, trigger counter will be reset

*/
int
tidIntEnable(int iflag)
{
#ifdef VXWORKS
  int lock_key=0;
#endif

  if(TIDp == NULL) 
    {
      printf("%s: ERROR: TIR not initialized\n",__FUNCTION__);
      return(-1);
    }

  TIDLOCK;
  if(iflag == 1)
    {
      tidWrite(&TIDp->reset,(1<<4));
      printf("%s: Set me free\n",__FUNCTION__);
      tidIntCount = 0;
    }

  tidIntRunning = 1;
  tidDoAck      = 1;
  tidNeedAck    = 0;

  switch (tidReadoutMode)
    {
    case TID_READOUT_POLL:
      tidStartPollingThread();
      break;

    case TID_READOUT_INT:
#ifdef VXWORKS
      lock_key = intLock();
      sysIntEnable(tidIntLevel);
#endif
      tidWrite(&TIDp->enableBits,
	       tidRead(&TIDp->enableBits) | (1<<13) | (1<<21));
      break;

    default:
      tidIntRunning = 0;
#ifdef VXWORKS
      if(lock_key)
	intUnlock(lock_key);
#endif
      printf("%s: ERROR: TID Readout Mode not defined %d\n",
	     __FUNCTION__,tidReadoutMode);
      TIDUNLOCK;
      return(ERROR);
      
    }

  TIDUNLOCK; /* Locks performed in tidSetTriggerSource() */
  switch(tidTriggerSource)
    {
    case TID_SOURCE_EXT:
      tidSetTriggerSource(3,0);
      break;

    case TID_SOURCE_TS:
      tidSetTriggerSource(1,0);
      break;

    case TID_SOURCE_TS2:
      tidSetTriggerSource(6,0);
      break;

    case TID_SOURCE_SOFT:
      tidSetTriggerSource(4,0);
      break;

    case TID_SOURCE_RANDOM:
      tidSetTriggerSource(7,0);
      break;

    default:
      tidIntRunning = 0;
      printf("%s: ERROR: TID Trigger Source not defined %d\n",
	     __FUNCTION__,tidTriggerSource);
    }
#ifdef VXWORKS
  if(lock_key)
    intUnlock(lock_key);
#endif

  return(OK);

}

/*
  tidIntDisable
  - Disable interrupts or latching triggers

*/
void 
tidIntDisable()
{

  if(TIDp == NULL) 
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return;
    }

  tidDisableTriggerSource(0);
  if(tidTriggerSource == TID_SOURCE_RANDOM)
    tidDisableRandomTrigger();

  TIDLOCK;
  tidWrite(&TIDp->enableBits,
	   tidRead(&TIDp->enableBits) & ~(1<<13));
  tidIntRunning = 0;
  TIDUNLOCK;
}

unsigned int
tidBReady()
{
  unsigned int rval;
  if(TIDp == NULL) 
    {
      logMsg("%s: ERROR: TID not initialized\n",__FUNCTION__,2,3,4,5,6);
      return 0;
    }

  TIDLOCK;
  rval  = (tidRead(&TIDp->blocksReady)&0xff)>>24;
  TIDUNLOCK;

  return rval;
}

int
tidSetBlockWaitingThreshold(unsigned int thres)
{
  if(TIDp == NULL) 
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }

  if(thres>0xff || thres==0)
    {
      printf("%s: ERROR: Invalid value for thres (%d)\n",
	     __FUNCTION__,thres);
      return ERROR;
    }

  TIDLOCK;
  tidWrite(&TIDp->intSetup, (tidRead(&TIDp->intSetup) & ~TID_INTSETUP_BLOCK_WAIT_THRES)
	   | (thres<<24));
  TIDUNLOCK;

  return OK;
}

/* 
   tidSetupFiberLoopback
   - A test setup using Fiber 5 output to Fiber 1 to provide a trigger path
*/
int
tidSetupFiberLoopback()
{
  if(TIDp == NULL) 
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }
  
  TIDLOCK;
  tidWrite(&TIDp->reset,0x4000);

  tidWrite(&TIDp->vmeSyncLoad,0x77);

/*   tidWrite(&TIDp->vmeSyncDelay,0x54 | (0x3f<<8)); */

  tidWrite(&TIDp->vmeSyncLoad,0x55);

  tidWrite(&TIDp->triggerCmdCode,0x18);

  tidWrite(&TIDp->trigDelay,0x300);
  TIDUNLOCK;

  return OK;
}

int
tidLoadTriggerTable()
{
  int ipat;
  /* This pattern associates (according to Gu)
     pins 21/22 | 23/24 | 25/26 : trigger1
     pins 29/30 | 31/32 | 33/34 : trigger2
  */
  unsigned int trigPattern[16] =
    {
      0x0C0C0C00, 0x0C0C0C0C, 0xCCCCCCC0, 0xCCCCCCCC,
      0xCCCCCCC0, 0xCCCCCCCC, 0xCCCCCCC0, 0xCCCCCCCC,
      0xCCCCCCC0, 0xCCCCCCCC, 0xCCCCCCC0, 0xCCCCCCCC, 
      0xCCCCCCC0, 0xCCCCCCCC, 0xCCCCCCC0, 0xCCCCCCCC
    };

  if(TIDp == NULL) 
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }
  
  TIDLOCK;
  for(ipat=0; ipat<16; ipat++)
    tidWrite(&TIDp->trigTable[ipat], trigPattern[ipat]);

  TIDUNLOCK;

  return OK;
}

unsigned int
tidGetDaqStatus()
{
  unsigned int rval;
  if(TIDp == NULL) 
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }
  
  TIDLOCK;
  rval = tidRead(&TIDp->daqStatus);
  TIDUNLOCK;

  return rval;
}

unsigned int
tidGetTSTriggerInfo()
{
  unsigned int rval;
  if(TIDp == NULL) 
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }
  
  TIDLOCK;
  rval = tidRead(&TIDp->TSTriggerInfo);
  TIDUNLOCK;

  return rval;
}

int
tidSetGTPInput(unsigned int inpMask)
{
  if(TIDp == NULL) 
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }

  TIDLOCK;
  tidWrite(&TIDp->intSetup,
	   ((inpMask<<16) & (TID_INTSETUP_GTPINPUT_MASK)) |
	   (tidRead(&TIDp->intSetup) & ~(TID_INTSETUP_GTPINPUT_MASK))
	   );
  TIDUNLOCK;

  return OK;
}

int
tidSetClockSource(unsigned int source)
{
  unsigned int clkset=0;

  if(TIDp == NULL) 
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }

  if(source>4)
    {
      printf("%s: ERROR: Invalid Clock Souce (%d)\n",__FUNCTION__,source);
      return ERROR;      
    }

  printf("%s: Setting clock source %d\n",__FUNCTION__,source);

  switch(source)
    {
    case 0: // ONBOARD
      clkset = TID_INTSETUP_CLK_ONBOARD;
      break;
    case 1:
      clkset = TID_INTSETUP_CLK_FIBER1;
      break;
    case 2:
      clkset = TID_INTSETUP_CLK_FIBER5;      
      break;
    case 3:
      clkset = TID_INTSETUP_CLK_FP;
      break;
    }

  TIDLOCK;
  tidWrite(&TIDp->intSetup,
	   (tidRead(&TIDp->intSetup) & ~(TID_INTSETUP_CLK_SOURCE_MASK))
	   | clkset);
  /* Reset DCM (Digital Clock Manager) - 250/200MHz */
  tidWrite(&TIDp->reset,TID_RESET_DCM_250);
  taskDelay(1);
  /* Reset DCM (Digital Clock Manager) - 125MHz */
  tidWrite(&TIDp->reset,TID_RESET_DCM_125);
  taskDelay(1);
  TIDUNLOCK;

  return OK;
}

/*
 * tidTrigger2
 *    - fire a trigger 2 via VME
 *
 */

int
tidTrigger2()
{
  if(TIDp == NULL) 
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }
  
  TIDLOCK;
  tidWrite(&TIDp->triggerCmdCode, 0xA180);
  tidWrite(&TIDp->triggerCmdCode, 0xA018);
  TIDUNLOCK;
  return OK;

}

void 
FiberMeas(void)
{
  unsigned int fiberLatency, syncDelay;
  if(TIDp == NULL) {
    printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
    return;
  }
  
  TIDLOCK;
  tidWrite(&TIDp->reset,0x4000);  // reset the IODELAY
  taskDelay(10);
  tidWrite(&TIDp->reset,0x2000);  // auto adjust the return signal phase
  taskDelay(10);
  tidWrite(&TIDp->reset,0x8000);  // measure the fiber latency
  taskDelay(10);

  printf("%s: Fiber Latency is 0x%x\n",
	 __FUNCTION__,tidRead(&TIDp->fiberLatencyMeasurement));
  
  tidWrite(&TIDp->reset,0x800);   // auto adjust the sync phase for HFBR#1
  taskDelay(10);
  tidWrite(&TIDp->reset,0x1000);  // auto adjust the sync phase for HFBR#5
  taskDelay(10);

  fiberLatency = tidRead(&TIDp->fiberLatencyMeasurement);
  syncDelay = (0x7f-(((fiberLatency>>23)&0x1ff)/2));
  syncDelay=(syncDelay<<8)&0xff00;  //set the sync delay according to the fiber latency

  printf("%s\n\n fiberLatency = 0x%08x   syncDelay = 0x%08x\n\n",__FUNCTION__,fiberLatency,syncDelay);

  tidWrite(&TIDp->fiberSyncAlignment,syncDelay);
  syncDelay = tidRead(&TIDp->fiberSyncAlignment);

#ifdef GUSTUFF
  // A24 buffer write
  sysBusToLocalAdrs(0x39,TID_REG_ADDR+0x100,&laddr);		/* trigger interface */
  *laddr = 0x4000;  // reset the IODELAY
  cpuDelay(10000);
  *laddr = 0x2000;  // auto adjust the return signal phase
  cpuDelay(10000000);
  *laddr = 0x8000;  // measure the fiber latency
  cpuDelay(10000000);

  sysBusToLocalAdrs(0x39,TID_REG_ADDR+0x18,&laddr);		/* trigger interface */
  printf("Fiber Latency is %x\n",*laddr);

  sysBusToLocalAdrs(0x39,TID_REG_ADDR+0x100,&laddr);		/* trigger interface */
  *laddr = 0x800;   // auto adjust the sync phase for HFBR#1
  cpuDelay(10000000);
  *laddr = 0x1000;  // auto adjust the sync phase for HFBR#5
  cpuDelay(10000000);

  sysBusToLocalAdrs(0x39,TID_REG_ADDR+0x18,&laddr);		/* trigger interface */
  fiberLatency = *laddr;  //fiber latency measurement result
  syncDelay = (0x7f-(((fiberLatency>>23)&0x1ff)/2));
  syncDelay=(syncDelay<<8)&0xff00;  //set the sync delay according to the fiber latency
  cpuDelay(1000);

  sysBusToLocalAdrs(0x39,TID_REG_ADDR+0x1c,&laddr);		/* trigger interface */
  *laddr = syncDelay;
  syncDelay= *laddr;
#endif

  TIDUNLOCK;

  printf (" \n The fiber latency of 0x18 is: %x \n", fiberLatency );
  printf (" \n The sync latency of 0x1C is: %x \n",syncDelay);
}

void
FiberMeasWrite(unsigned int number)
{
  unsigned int fiberLatency, syncDelay;
  if(TIDp == NULL) {
    printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
    return;
  }

  fiberLatency=0;
  TIDLOCK;
  tidWrite(&TIDp->reset,0x4000);  // reset the IODELAY
  taskDelay(10);
  /*   tidWrite(&TIDp->reset,0x2000);  // auto adjust the return signal phase */
  /*   taskDelay(10); */
  /*   tidWrite(&TIDp->reset,0x8000);  // measure the fiber latency */
  /*   taskDelay(10); */

  /*   printf("%s: Fiber Latency is 0x%x\n", */
  /* 	 __FUNCTION__,tidRead(&TIDp->fiberLatencyMeasurement)); */
  
  tidWrite(&TIDp->reset,0x800);   // auto adjust the sync phase for HFBR#1
  taskDelay(10);
  tidWrite(&TIDp->reset,0x1000);  // auto adjust the sync phase for HFBR#5
  taskDelay(10);

  /*   fiberLatency = tidRead(&TIDp->fiberLatencyMeasurement); */
  /*   syncDelay = (0x7f-(((fiberLatency>>23)&0x1ff)/2)); */
  syncDelay = (0xcf-(number));
  syncDelay=(syncDelay<<16)&0xff0000;  //set the sync delay according to the fiber latency

  printf("%s\n\n syncDelay = 0x%08x\n\n",__FUNCTION__,syncDelay);

  tidWrite(&TIDp->fiberSyncAlignment,syncDelay);
  syncDelay = tidRead(&TIDp->fiberSyncAlignment);

  TIDUNLOCK;

}

void
nickConfig()
{
  tidInit((21<<19),0,0,0);

  /* Set Clock Source - Onboard*/
  tidSetClockSource(0);

  /* Enable fiber handling the loopback */
  tidEnableFiber( (1<<0) | (1<<4) );

  tidSetSyncDelayWidth(0x54, 0x3f);

  /* Set Trigger 1 pulse delay (0*4ns = 0ns) and width [(7+1)*4ns = 32ns] */
  tidSetTriggerPulse(1,0,7);

  /* Set Trigger 2 pulse delay (0*4ns = 0ns) and width [(7+1)*4ns = 32ns] */
  tidSetTriggerPulse(2,0,7);

  /* Set sync source as TS Loopback */
  tidSetSyncSource(4);

  tidStatus();

}

/* Register Read/Write routines */
#ifndef DEVEL
static 
#endif
unsigned int tidRead(volatile unsigned int *addr)
{
  unsigned int rval;

  rval = *addr;

#ifndef VXWORKS
  rval = LSWAP(rval);
#endif


  return rval;
}

#ifndef DEVEL
static 
#endif
void tidWrite(volatile unsigned int *addr, unsigned int val)
{
  /* #define I2CHACK */
#ifdef I2CHACK
  unsigned int addr_diff=0;

  addr_diff = (unsigned int)addr - (unsigned int)TIDp;
  if(addr_diff >= 0x30000 && addr_diff <= 0x4FFFF)
    {
      tidRead(&TIDp->enableBits);
    }
#endif

#ifndef VXWORKS
  val = LSWAP(val);
#endif
  /*   printf("%s: addr = 0x%08x   val = 0x%08x\n", */
  /* 	 __FUNCTION__,(unsigned int)addr-(unsigned int)TIDp,val); */
  


  *addr = val;

  return;
}

