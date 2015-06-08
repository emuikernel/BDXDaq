
#if defined(VXWORKS) || defined(Linux_vme)

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

#define DEVEL

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

/* Global Variables */
volatile struct TID_A24RegStruct *TIDp=NULL;  /* pointer to TID memory map */
volatile        unsigned int     *TIDpd=NULL; /* pointer to TID data FIFO */
int tidA24Offset=0;                           /* Difference in CPU A24 Base and VME A24 Base */
int tidA32Base  =0x08000000;                  /* Minimum VME A32 Address for use by TID */
int tidA32Offset=0;                           /* Difference in CPU A32 Base and VME A32 Base */
int tidMaster=1;                              /* TID is the Trigger Master (Supervisor) */
int tidCrateID=0x59;                          /* Crate ID */
int tidBlockLevel=0;                          /* Block level for TID */
unsigned int TID_DATA_BLOCK_HEADER=0;         /* Block header info, constucted at tidInit */
unsigned int TID_DATA_BLOCK_TRAILER=0;        /* Block trailer info, constucted at tidInit */
unsigned int        tidIntCount    = 0;
unsigned int        tidDaqCount    = 0;       /* Block count from previous update (in daqStatus) */
unsigned int        tidReadoutMode = 0;
unsigned int        tidTriggerSource = 0;     /* Set with tidSetTriggerSource(...) */
unsigned int        tidSlaveMask   = 0;       /* TI Slaves (mask) to be used with TI Master */
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
static void tidStartPollingThread(void);

/* polling thread pthread and pthread_attr */
pthread_attr_t tidpollthread_attr;
pthread_t      tidpollthread;


/*
 Libraries of routines for the SD and CTP; tidLib.c can be compiled without it,
 but sdLib.c and ctpLib.c cannot be compiled without tidLib.c
 NOTE: in our Makefile tidLib.c must be touched if sdLib.c or ctpLib.c
 has been changed, otherwise it will not be recompiled
*/
#include "cinclude/sdLib.c"
#include "cinclude/ctpLib.c"


/*******************************************************************************
 *
 *  tidInit - Initialize the TIDp register space into local memory,
 *  and setup registers given user input
 *
 *  ARGs: 
 *    tSlot  - Slot number of the TID
 *    mode   - Readout/Triggering Mode
 *          0: External Trigger - Interrupt Mode
 *          1: TS/TImaster Trigger - Interrupt Mode
 *          2: External Trigger - Polling Mode
 *          3: TS/TImaster Trigger - Polling Mode
 *
 *    iFlag  - Initialization type
 *          0: Initialize the TID (default behavior)
 *          1: Do not initialize the board, just setup the pointers
 *             to the registers
 *
 *  RETURNS: OK if successful, otherwise ERROR.
 *
 */

int
tidInit(unsigned int tAddr, unsigned int mode, int iFlag)
{
  unsigned int laddr;
  unsigned int rval, boardID;
  unsigned int fpgaUserCode;
  int stat, res;
  int noBoardInit=0;
  int a32Enabled=0;

  /* Check VME address */
  if(tAddr<0 || tAddr>0xffffff)
  {
    printf("%s: ERROR: Invalid VME Address (%d)\n",__FUNCTION__,tAddr);
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
  }
  
  /* Get the FPGA User Code and print out some details */
  fpgaUserCode = tidGetFirmwareVersion();
  if(fpgaUserCode>0)
  {
    printf("  User ID: 0x%x \tFirmware (version - revision): 0x%X - 0x%03X\n",
	     (fpgaUserCode&0xFFFF0000)>>16, (fpgaUserCode&0xF000)>>12, fpgaUserCode&0xFFF);
  }
  else
    return ERROR;


  /* Set some defaults, dependent on Master/Slave status */
  tidReadoutMode = mode;
  switch(mode)
  {
    case TID_READOUT_EXT_INT:
    case TID_READOUT_EXT_POLL:
      printf("... Configure as TI Master...\n");
      /* Master (Supervisor) Configuration: takes in external triggers */
      tidMaster = 1;
      /* BUSY from Loopback and Switch Slot B */
      tidSetBusySource(TID_BUSY_LOOPBACK | TID_BUSY_SWB,1);
      /* Onboard Clock Source */
      tidSetClockSource(TID_CLOCK_ONBOARD);
      /* Loopback Sync Source */
      tidSetSyncSource(TID_SYNC_LOOPBACK);
      break;

    case TID_READOUT_TS_INT:
    case TID_READOUT_TS_POLL:
      printf("... Configure as TI Slave...\n");
      /* Slave Configuration: takes in triggers from the Master (supervisor) */
      tidMaster = 0;
      /* BUSY from Switch Slot B */
      tidSetBusySource(TID_BUSY_SWB,1);
      /* Enable HFBR#1 */
      tidEnableFiber(1);
      /* HFBR#1 Clock Source */
      tidSetClockSource(TID_CLOCK_HFBR1);
      /* HFBR#1 Sync Source */
      tidSetSyncSource(TID_SYNC_HFBR1);
      /* HFBR#1 Trigger Source */
      tidSetTriggerSource(TID_TRIGGER_HFBR1);
      break;

    default:
      printf("%s: Invalid TID Mode %d\n", __FUNCTION__,mode);
      return ERROR;
  }

  /* Check if we should exit here, or initialize some board defaults */
  if(noBoardInit)
  {
    return OK;
  }

  /* Setup some Other Library Defaults */

  /* Reset I2C engine */
  vmeWrite32(&TIDp->reset,(1<<1));

  /* Set Default Block Level to 1, and default crateID */
  tidSetBlockLevel(1);
  tidSetCrateID(0x1);

  /* Set Event format 2 */
  tidSetEventFormat(1);

  /* Set tidReadoutMode */
  if(mode>3)
  {
    printf("%s: Invalid TID Mode %d\n", __FUNCTION__,mode);
    return ERROR;
  }
  else
    tidReadoutMode = mode;

  /* Setup A32 data buffer with library default */
  vmeWrite32(&TIDp->adr32, (tidA32Base) | (TID_ADR32_ENABLE) );

  a32Enabled = vmeRead32(&TIDp->adr32)&(TID_ADR32_ENABLE);
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
    printf("%s: ERROR in sysBusToLocalAdrs(0x09,0x%x,&laddr) \n", __FUNCTION__,tidA32Base);
    TIDUNLOCK;
    return(ERROR);
  }
#else
  res = vmeBusToLocalAdrs(0x09,(char *)tidA32Base,(char **)&laddr);
  if (res != 0) 
  {
    printf("%s: ERROR in vmeBusToLocalAdrs(0x09,0x%x,&laddr) \n", __FUNCTION__,tidA32Base);
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

  /* Set this to 1 (ROC Lock mode), by default. */
  tidSetBlockAckThreshold(1);

  /* Setup a default Sync Delay and Pulse width */
  tidSetSyncDelayWidth(0x54, 0x3f);

  /* Set Trigger 1 pulse delay (0*4ns = 0ns) and width [(7+1)*4ns = 32ns] */
  tidSetTriggerPulse(1,0,7);

  /* Set Trigger 2 pulse delay (0*4ns = 0ns) and width [(7+1)*4ns = 32ns] */
  tidSetTriggerPulse(2,0,7);

  /* Disable all TS Inputs */
  tidDisableTSInput(0x3f);

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
  unsigned int dataSetup, enableBits, intSetup, adr32, daqStatus, liveTimer, busyTimer, monitor;
  unsigned int oport, tsCounter;
  unsigned int blockStatus[5], iblock, nblocksReady, nblocksNeedAck;
  unsigned int TIbase;
  unsigned int ifiber, fibermask;
  unsigned int iinp, inpmask;

  if(TIDp==NULL)
  {
    printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
    return;
  }

  TIDLOCK;
  dataSetup    = vmeRead32(&TIDp->dataSetup);
  enableBits   = vmeRead32(&TIDp->enableBits);
  intSetup     = vmeRead32(&TIDp->intSetup);

  adr32        = vmeRead32(&TIDp->adr32);
  daqStatus    = vmeRead32(&TIDp->daqStatus);
  liveTimer    = vmeRead32(&TIDp->liveTimer);
  busyTimer    = vmeRead32(&TIDp->busyTimer);
  monitor      = vmeRead32(&TIDp->monitor);

  oport        = vmeRead32(&TIDp->oport);
  tsCounter    = vmeRead32(&TIDp->TSInputCounter);

  for(iblock=0;iblock<5;iblock++)
  {
    blockStatus[iblock] = vmeRead32(&TIDp->blockStatus[iblock]);
  }
  TIDUNLOCK;

  TIbase = (unsigned int)TIDp;

  printf("\n");
#ifdef VXWORKS
  printf("STATUS for TID at base address 0x%08x \n",
	 (unsigned int) TIDp);
#else
  printf("STATUS for TID at VME (USER) base address 0x%08x (0x%08x) \n",
	 (unsigned int) TIDp - tidA24Offset, (unsigned int) TIDp);
#endif
  printf("--------------------------------------------------------------------------------\n");
  printf(" A32 Data buffer ");
  if(adr32&TID_ADR32_ENABLE)
  {
    printf("ENABLED at ");
#ifdef VXWORKS
    printf("base address 0x%08x\n",TIDpd);
#else
    printf("VME (USER) base address 0x%08x (0x%08x)\n",
	     (unsigned int)TIDpd - tidA32Offset, (unsigned int)TIDpd);
#endif
  }
  else
    printf("DISABLED\n");


  if(tidMaster)
    printf(" Configured as a TI Master\n");
  else
    printf(" Configured as a TI Slave\n");

  printf(" Readout Count: %d\n",tidIntCount);
  printf(" Registers (offset):\n");
  printf("  dataSetup  (0x%04x) = 0x%08x\t", (unsigned int)(&TIDp->dataSetup) - TIbase, dataSetup);
  printf("  enableBits (0x%04x) = 0x%08x\n", (unsigned int)(&TIDp->enableBits) - TIbase, enableBits);
  printf("  intSetup   (0x%04x) = 0x%08x\t", (unsigned int)(&TIDp->intSetup) - TIbase, intSetup);
  printf("  adr32      (0x%04x) = 0x%08x\n", (unsigned int)(&TIDp->adr32) - TIbase, adr32);
  printf("  daqStatus  (0x%04x) = 0x%08x\t", (unsigned int)(&TIDp->daqStatus) - TIbase, daqStatus);
  printf("  liveTimer  (0x%04x) = 0x%08x\n", (unsigned int)(&TIDp->liveTimer) - TIbase, liveTimer);
  printf("  busyTimer  (0x%04x) = 0x%08x\t", (unsigned int)(&TIDp->busyTimer) - TIbase, busyTimer);
  printf("  monitor    (0x%04x) = 0x%08x\n", (unsigned int)(&TIDp->monitor) - TIbase, monitor);
  printf("\n");

  printf(" Crate ID = 0x%02x\n",(dataSetup&TID_DATASETUP_CRATEID_MASK));
  printf(" Block size = %d\n",(dataSetup&TID_DATASETUP_BLOCKSIZE_MASK)>>8);

  fibermask = enableBits & TID_ENABLEBITS_HFBR_MASK;
  if(fibermask)
  {
    printf(" HFBR enabled (0x%x)= ",fibermask);
    for(ifiber=0; ifiber<8; ifiber++)
	{
	  if( fibermask & (1<<ifiber) ) printf(" %d",ifiber+1);
	}
    printf("\n");
  }
  else
    printf(" All HFBR Disabled\n");

  if(tidMaster)
  {
    if(tidSlaveMask)
	{
	  printf(" TI Slaves Configured on HFBR (0x%x) = ",tidSlaveMask);
	  fibermask = tidSlaveMask;
	  for(ifiber=0; ifiber<8; ifiber++)
	  {
	    if( fibermask & (1<<ifiber)) printf(" %d",ifiber+1);
	  }
	  printf("\n");	
	}
    else
      printf(" No TI Slaves Configured on HFBR\n");
  }

  if(tidTriggerSource)
  {
    printf(" Trigger input source =\n");
    if(tidTriggerSource & TID_DATASETUP_TRIGSRC_P0) printf("   P0 Input\n");
    if(tidTriggerSource & TID_DATASETUP_TRIGSRC_HFBR1) printf("   HFBR #1 Input\n");
    if(tidTriggerSource & TID_DATASETUP_TRIGSRC_LOOPBACK) printf("   TI Master Loopback\n");
    if(tidTriggerSource & TID_DATASETUP_TRIGSRC_FPTRG) printf("   Front Panel TRG\n");
    if(tidTriggerSource & TID_DATASETUP_TRIGSRC_VME) printf("   VME Command\n");
    if(tidTriggerSource & TID_DATASETUP_TRIGSRC_TSINPUTS) printf("   Front Panel TS Inputs\n");
    if(tidTriggerSource & TID_DATASETUP_TRIGSRC_TSrev2) printf("   Trigger Supervisor (rev2)\n");
    if(tidTriggerSource & TID_DATASETUP_TRIGSRC_RND) printf("   Internal Random\n");
  }
  else 
    printf(" No Trigger input source configured\n");

  inpmask = (intSetup & TID_INTSETUP_GTPINPUT_MASK)>>16;
  if(inpmask)
    {
      printf(" Front Panel TS Inputs Enabled: ");
      for(iinp=0; iinp<6; iinp++)
	{
	  if( inpmask & (1<<iinp)) 
	    printf(" %d",iinp+1);
	}
      printf("\n");	
    }
  else
    {
      printf(" All Front Panel TS Inputs Disabled\n");
    }
  printf(" Sync source = \n");
  if(dataSetup & TID_DATASETUP_SYNCSRC_P0)
    printf("   P0 Input\n");
  if(dataSetup & TID_DATASETUP_SYNCSRC_HFBR1)
    printf("   HFBR #1 Input\n");
  if(dataSetup & TID_DATASETUP_SYNCSRC_HFBR5)
    printf("   HFBR #5 Input\n");
  if(dataSetup & TID_DATASETUP_SYNCSRC_FP)
    printf("   Front Panel Input\n");
  if(dataSetup & TID_DATASETUP_SYNCSRC_LOOPBACK)
    printf("   TI Master Loopback\n");

  printf(" BUSY source = \n");
  if(enableBits & TID_ENABLEBITS_BUSY_ENABLE_SWA)
    printf("   Switch Slot A\n");
  if(enableBits & TID_ENABLEBITS_BUSY_ENABLE_SWB)
    printf("   Switch Slot B\n");
  if(enableBits & TID_ENABLEBITS_BUSY_ENABLE_VMEP2)
    printf("   P2 Input\n");
  if(enableBits & TID_ENABLEBITS_BUSY_ENABLE_TDC)
    printf("   Front Panel F1TDC Connection\n");
  if(enableBits & TID_ENABLEBITS_BUSY_ENABLE_ADC)
    printf("   Front Panel FADC250 Connection\n");
  if(enableBits & TID_ENABLEBITS_BUSY_ENABLE_FP)
    printf("   Front Panel\n");
  if(enableBits & TID_ENABLEBITS_BUSY_ENABLE_LOOPBACK)
    printf("   Loopback\n");
  if(enableBits & TID_ENABLEBITS_BUSY_ENABLE_HFBR1)
    printf("   HFBR #1\n");
  if(enableBits & TID_ENABLEBITS_BUSY_ENABLE_HFBR2)
    printf("   HFBR #2\n");
  if(enableBits & TID_ENABLEBITS_BUSY_ENABLE_HFBR3)
    printf("   HFBR #3\n");
  if(enableBits & TID_ENABLEBITS_BUSY_ENABLE_HFBR4)
    printf("   HFBR #4\n");
  if(enableBits & TID_ENABLEBITS_BUSY_ENABLE_HFBR5)
    printf("   HFBR #5\n");
  if(enableBits & TID_ENABLEBITS_BUSY_ENABLE_HFBR6)
    printf("   HFBR #6\n");
  if(enableBits & TID_ENABLEBITS_BUSY_ENABLE_HFBR7)
    printf("   HFBR #7\n");
  if(enableBits & TID_ENABLEBITS_BUSY_ENABLE_HFBR8)
    printf("   HFBR #8\n");

  printf(" Event Format = %d\n",(dataSetup&TID_DATASETUP_EVENTFORMAT_MASK)>>30);

  if(enableBits&TID_ENABLEBITS_IRQ_ENABLE)
    printf(" Interrupts ENABLED\n");
  else
    printf(" Interrupts DISABLED\n");
  printf("   Level = %d   Vector = 0x%02x\n",
	 (intSetup&TID_INTSETUP_LEVEL_MASK)>>8, (intSetup&TID_INTSETUP_VEC_MASK));
  
  if(adr32&TID_ADR32_ENABLE_BERR)
    printf(" Bus Errors Enabled\n");

  printf(" Blocks ready for readout: %d\n",(daqStatus&TID_DAQSTATUS_BLOCKS_AVAILABLE_MASK)>>24);
  if(tidMaster)
    {
      /* TI slave block status */
      fibermask = tidSlaveMask;
      for(ifiber=0; ifiber<8; ifiber++)
	{
	  if( fibermask & (1<<ifiber) )
	    {
	      if( (ifiber % 2) == 0)
		{
		  nblocksReady   = blockStatus[ifiber/2] & TID_BLOCKSTATUS_NBLOCKS_READY0;
		  nblocksNeedAck = (blockStatus[ifiber/2] & TID_BLOCKSTATUS_NBLOCKS_NEEDACK0)>>8;
		}
	      else
		{
		  nblocksReady   = (blockStatus[(ifiber-1)/2] & TID_BLOCKSTATUS_NBLOCKS_READY1)>>16;
		  nblocksNeedAck = (blockStatus[(ifiber-1)/2] & TID_BLOCKSTATUS_NBLOCKS_NEEDACK1)>>24;
		}
	      printf("  Fiber %d  :  Blocks ready / need acknowledge: %d / %d\n",
		     ifiber+1,nblocksReady, nblocksNeedAck);
	    }
	}

      /* TI master block status */
      nblocksReady   = (blockStatus[4] & TID_BLOCKSTATUS_NBLOCKS_READY1)>>16;
      nblocksNeedAck = (blockStatus[4] & TID_BLOCKSTATUS_NBLOCKS_NEEDACK1)>>24;
      printf("  Loopback :  Blocks ready / need acknowledge: %d / %d\n",
	     nblocksReady, nblocksNeedAck);

      printf(" TI Master input counter %d\n",tsCounter);
    }

  printf("--------------------------------------------------------------------------------\n");
  printf("\n\n");

}

/*******************************************************************************
 *
 * tidGetFirmwareVersion - Get the Firmware Version
 *
 * RETURNS: User Code if successful, ERROR otherwise
 *
 *  FIXME: Problem with the firmware with this implementation (1Apr2011)
 *
 */

int
tidGetFirmwareVersion()
{
  unsigned int rval=0;
  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }

  TIDLOCK;
  /* reset the VME_to_JTAG engine logic */
  vmeWrite32(&TIDp->reset,(1<<2));

  /* Reset FPGA JTAG to "reset_idle" state */
  vmeWrite32(&TIDp->JTAGFPGABase[(0x003C)>>2],0);

  /* enable the user_code readback */
  vmeWrite32(&TIDp->JTAGFPGABase[(0x092C)>>2],0x3c8);

  /* shift in 32-bit to FPGA JTAG */
  vmeWrite32(&TIDp->JTAGFPGABase[(0x1F1C)>>2],0);
  
  rval = vmeRead32(&TIDp->JTAGFPGABase[(0x1F1C)>>2]);
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
  vmeWrite32(&TIDp->reset,0x04);
  vmeWrite32(&TIDp->JTAGPROMBase[(0x3c)>>2],0);
  vmeWrite32(&TIDp->JTAGPROMBase[(0xf2c)>>2],0xEE);
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
  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }

  
  TIDLOCK;
  vmeWrite32(&TIDp->vmeSyncLoad,0x33); 
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
  vmeWrite32(&TIDp->reset,TID_RESET_VMEREGS);
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
  vmeWrite32(&TIDp->dataSetup,
	   (vmeRead32(&TIDp->dataSetup) & ~TID_DATASETUP_CRATEID_MASK)  | crateID);
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
  vmeWrite32(&TIDp->dataSetup,
	   (vmeRead32(&TIDp->dataSetup) & ~TID_DATASETUP_BLOCKSIZE_MASK) | (blockLevel<<8));
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
 *         2: Front Panel (TRG)
 *         3: Front Panel TS Inputs
 *         4: TS (rev2) 
 *         5: Random
 *
 * RETURNS: OK if successful, ERROR otherwise
 *
 */

int
tidSetTriggerSource(int trig)
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

  if(!tidMaster)
  {
    /* Only valid trigger for TI Slave is HFBR1 */
    datasetup = TID_DATASETUP_TRIGSRC_HFBR1;
    if( (trig & ~TID_TRIGGER_HFBR1) != 0)
	{
	  printf("%s: WARN:  Only valid trigger source for TI Slave is HFBR1 (%d).",
		 __FUNCTION__,TID_TRIGGER_HFBR1);
	  printf("  Ignoring specified trig (0x%x)\n",trig);
	}
  }
  else 
  {
    /* Setup for TI Master */
    /* Set VME and Loopback by default */
    datasetup  = TID_DATASETUP_TRIGSRC_VME;
    datasetup |= TID_DATASETUP_TRIGSRC_LOOPBACK;

    switch(trig)
	{
	case TID_TRIGGER_P0:
	  datasetup |= TID_DATASETUP_TRIGSRC_P0;
	  break;

	case TID_TRIGGER_HFBR1:
	  datasetup |= TID_DATASETUP_TRIGSRC_HFBR1;
	  break;

	case TID_TRIGGER_FPTRG:
	  datasetup |= TID_DATASETUP_TRIGSRC_FPTRG;
	  break;

	case TID_TRIGGER_TSINPUTS:
	  datasetup |= TID_DATASETUP_TRIGSRC_TSINPUTS;
	  break;

	case TID_TRIGGER_TSREV2:
	  datasetup |= TID_DATASETUP_TRIGSRC_TSrev2;
	  break;

	case TID_TRIGGER_RANDOM:
	  datasetup |= TID_DATASETUP_TRIGSRC_RND;
	  datasetup |= TID_DATASETUP_TRIGSRC_HFBR1;
	  break;

	}
  }

  tidTriggerSource = datasetup;
  printf("%s: INFO: tidTriggerSource = 0x%08x\n",__FUNCTION__,tidTriggerSource);

  return OK;
}

/*******************************************************************************
 *
 * tidSetTriggerSourceMask - Set trigger sources with specified trigmask
 *
 *    This routine is for special use when tidSetTriggerSource(...) does
 *    not set all of the trigger sources that is required by the user.
 *
 *  trigmask bits:  
 *                 0:  P0
 *                 1:  HFBR #1 
 *                 2:  TI Master Loopback
 *                 3:  Front Panel (TRG) Input
 *                 4:  VME Trigger
 *                 5:  Front Panel TS Inputs
 *                 6:  TS (rev 2) Input
 *                 7:  Random Trigger
 *
 * RETURNS: OK if successful, ERROR otherwise
 *
 */

int
tidSetTriggerSourceMask(int trigmask)
{
  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }

  /* Check input mask */
  if(trigmask>0xff)
    {
      printf("%s: ERROR: Invalid trigger source mask (0x%x).\n",
	     __FUNCTION__,trigmask);
      return ERROR;
    }

  tidTriggerSource = (trigmask)<<16;

  return OK;
}

/*******************************************************************************
 *
 * tidEnableTriggerSource - Enable trigger sources set by 
 *                          tidSetTriggerSource(...) or
 *                          tidSetTriggerSourceMask(...)
 *
 * RETURNS: OK if successful, ERROR otherwise
 *
 */

int
tidEnableTriggerSource()
{
  unsigned int datasetup=0;

  if(TIDp==NULL)
  {
    printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
    return ERROR;
  }

  if(tidTriggerSource==0)
  {
    printf("%s: WARN: No Trigger Sources Enabled\n",__FUNCTION__);
    return OK;
  }
  else
  {
    printf("%s: INFO: Enable Trigger Sources 0x%x\n",__FUNCTION__,tidTriggerSource);
  }

  TIDLOCK;
  datasetup = vmeRead32(&TIDp->dataSetup);
  vmeWrite32(&TIDp->dataSetup,
	   (datasetup & ~(TID_DATASETUP_TRIGSRC_MASK)) | tidTriggerSource);
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
      vmeWrite32(&TIDp->dataSetup,
	       vmeRead32(&TIDp->dataSetup) & ~TID_DATASETUP_TRIGSRC_FPTRG);
    }
  else
    {
      vmeWrite32(&TIDp->dataSetup,
	       vmeRead32(&TIDp->dataSetup) & ~TID_DATASETUP_TRIGSRC_MASK);
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
  datasetup = vmeRead32(&TIDp->dataSetup) & ~TID_DATASETUP_SYNCSRC_MASK;

  switch(sync)
    {
    case TID_SYNC_P0:
      datasetup |= TID_DATASETUP_SYNCSRC_P0;
      break;

    case TID_SYNC_HFBR1:
      datasetup |= TID_DATASETUP_SYNCSRC_HFBR1;
      break;

    case TID_SYNC_HFBR5:
      datasetup |= TID_DATASETUP_SYNCSRC_HFBR5;
      break;

    case TID_SYNC_FP:
      datasetup |= TID_DATASETUP_SYNCSRC_FP;
      break;

    case TID_SYNC_LOOPBACK:
      datasetup |= TID_DATASETUP_SYNCSRC_LOOPBACK;
      break;

    }

  vmeWrite32(&TIDp->dataSetup,datasetup);
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
  datasetup  = vmeRead32(&TIDp->dataSetup) & ~TID_DATASETUP_EVENTFORMAT_MASK;
  datasetup |= TID_DATASETUP_TWOBLOCK_DUMMY;

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

  vmeWrite32(&TIDp->dataSetup,datasetup);
  datasetup = vmeRead32(&TIDp->dataSetup);

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
  vmeWrite32(&TIDp->softTrig,
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
  unsigned int word = (1<<3) | (7); /* ~4kHz, enable */

  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }

  TIDLOCK;
  vmeWrite32(&TIDp->randomTrig,word | (word<<4));
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
  vmeWrite32(&TIDp->randomTrig,0);
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
  int dCnt, retVal, xferCount; /* blknum; */
  volatile unsigned int *laddr;
  unsigned int vmeAdr, val; /* bhead; */

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

      retVal = usrVme2MemDmaStart(vmeAdr, (UINT32)laddr, (nwrds<<2));
/*sergey
#ifdef VXWORKS
      retVal = sysVmeDmaSend((UINT32)laddr, vmeAdr, (nwrds<<2), 0);
#else
      retVal = vmeDmaSend((UINT32)laddr, vmeAdr, (nwrds<<2));
#endif
*/

    if(retVal |= 0) 
	{
	  logMsg("\ntidReadBlock: ERROR in DMA transfer Initialization 0x%x\n",retVal,0,0,0,0,0);
	  TIDUNLOCK;
	  return(retVal);
	}

      /* Wait until Done or Error */
	  retVal = usrVme2MemDmaDone();
/*sergey
#ifdef VXWORKS
      retVal = sysVmeDmaDone(10000,1);
#else
      retVal = vmeDmaDone();
#endif
*/
    if(retVal > 0)
	{
#ifdef VXWORKS
	  xferCount = (/*nwrds - */(retVal>>2) + dummy); /* Number of longwords transfered */
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
      ii=0;

      while(ii<nwrds) 
	{
	  val = (unsigned int) *TIDpd;
#ifndef VXWORKS
	  val = LSWAP(val);
#endif
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

  return OK;
}

/*******************************************************************************
 *
 * tidEnableFiber / tidDisableFiber
 *  - Enable/Disable Fiber transceiver
 *    fiber: integer indicative of the transceiver to enable / disable
 *
 *  Note:  All Fiber are enabled by default 
 *         (no harm, except for 1-2W power usage)
 *
 * RETURNS: OK if successful, ERROR otherwise.
 *
 */

int
tidEnableFiber(unsigned int fiber)
{
  unsigned int rval;
  unsigned int fiberbit;

  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }

  if((fiber<1) | (fiber>8))
    {
      printf("%s: ERROR: Invalid value for fiber (%d)\n",
	     __FUNCTION__,fiber);
      return ERROR;
    }

  fiberbit = (1<<(fiber-1));

  TIDLOCK;
  rval = vmeRead32(&TIDp->enableBits);
  vmeWrite32(&TIDp->enableBits,
	   rval | fiberbit );
  TIDUNLOCK;

  return OK;
  
}

int
tidDisableFiber(unsigned int fiber)
{
  unsigned int rval;
  unsigned int fiberbit;

  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }

  if((fiber<1) | (fiber>8))
    {
      printf("%s: ERROR: Invalid value for fiber (%d)\n",
	     __FUNCTION__,fiber);
      return ERROR;
    }

  fiberbit = (1<<(fiber-1));

  TIDLOCK;
  rval = vmeRead32(&TIDp->enableBits);
  vmeWrite32(&TIDp->enableBits,
	   rval & ~fiberbit );
  TIDUNLOCK;

  return rval;
  
}

/*******************************************************************************
 *
 * tidSetBusySource
 *  - Set the busy source with a given sourcemask
 *    sourcemask bits: 
 *             0: Switch Slot B (SD)
 *             1: P2 (Distribution Module for CAEN TDCs)
 *             2: Front Panel
 *             3: Loopback (For TI Master)
 *          4-11: HFBR #1-8
 *
 *    rFlag - decision to reset the global source flags
 *             0: Keep prior busy source settings and set new "sourcemask"
 *             1: Reset, using only that specified with "sourcemask"
*/
int
tidSetBusySource(unsigned int sourcemask, int rFlag)
{
  unsigned int enablebits=0;

  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }
  
  if(sourcemask>0xfff)
    {
      printf("%s: ERROR: Invalid value for sourcemask (0x%x)\n",
	     __FUNCTION__, sourcemask);
      return ERROR;
    }

  if(rFlag)
    {
      /* Read in the previous value , resetting previous BUSYs*/
      enablebits = vmeRead32(&TIDp->enableBits) & ~(TID_ENABLEBITS_BUSY_INPUT_MASK);
    }
  else
    {
      /* Read in the previous value , keeping previous BUSYs*/
      enablebits = vmeRead32(&TIDp->enableBits);
    }

  if(sourcemask & TID_BUSY_SWB)
    enablebits |= TID_ENABLEBITS_BUSY_ENABLE_SWB;

  if(sourcemask & TID_BUSY_P2)
    enablebits |= TID_ENABLEBITS_BUSY_ENABLE_VMEP2;

  if(sourcemask & TID_BUSY_FP)
    enablebits |= TID_ENABLEBITS_BUSY_ENABLE_FP;

  if(sourcemask & TID_BUSY_LOOPBACK)
    enablebits |= TID_ENABLEBITS_BUSY_ENABLE_LOOPBACK;
  
  if(sourcemask & TID_BUSY_HFBR1)
    enablebits |= TID_ENABLEBITS_BUSY_ENABLE_HFBR1;

  if(sourcemask & TID_BUSY_HFBR2)
    enablebits |= TID_ENABLEBITS_BUSY_ENABLE_HFBR2;

  if(sourcemask & TID_BUSY_HFBR3)
    enablebits |= TID_ENABLEBITS_BUSY_ENABLE_HFBR3;

  if(sourcemask & TID_BUSY_HFBR4)
    enablebits |= TID_ENABLEBITS_BUSY_ENABLE_HFBR4;

  if(sourcemask & TID_BUSY_HFBR5)
    enablebits |= TID_ENABLEBITS_BUSY_ENABLE_HFBR5;

  if(sourcemask & TID_BUSY_HFBR6)
    enablebits |= TID_ENABLEBITS_BUSY_ENABLE_HFBR6;

  if(sourcemask & TID_BUSY_HFBR7)
    enablebits |= TID_ENABLEBITS_BUSY_ENABLE_HFBR7;

  if(sourcemask & TID_BUSY_HFBR8)
    enablebits |= TID_ENABLEBITS_BUSY_ENABLE_HFBR8;
  
  vmeWrite32(&TIDp->enableBits, enablebits);

  return OK;

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
  rval = vmeRead32(&TIDp->enableBits) & ~(TID_ENABLEBITS_BUSY_INPUT_MASK);
  rval |= (mask);
  vmeWrite32(&TIDp->enableBits, rval);
  TIDUNLOCK;

  return rval;
}

/*******************************************************************************
 *
 * tidEnableI2CDevHack / tidDisableI2CDevHack
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
  vmeWrite32(&TIDp->enableBits,
	   vmeRead32(&TIDp->enableBits) | (TID_ENABLEBITS_I2C_DEV_HACK) );
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
  vmeWrite32(&TIDp->enableBits,
	   vmeRead32(&TIDp->enableBits) & ~(TID_ENABLEBITS_I2C_DEV_HACK) );
  TIDUNLOCK;

}

/*******************************************************************************
 *
 * tidEnableBusError / tidDisableBusError
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
  vmeWrite32(&TIDp->adr32,
	   vmeRead32(&TIDp->adr32) | (TID_ADR32_ENABLE_BERR) );
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
  vmeWrite32(&TIDp->adr32,
	   vmeRead32(&TIDp->adr32) & ~(TID_ADR32_ENABLE_BERR) );
  TIDUNLOCK;

}


/*******************************************************************************
 *
 * tidPayloadPort2VMESlot
 *  - Routine to return the VME slot, provided the VXS payload port
 *  - This does not access the bus, just a map in the library.
 *  - FIXME: Make sure this works.
 *
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

/*******************************************************************************
 *
 *  tidVMESlot2PayloadPort
 *  - Routine to return the VXS Payload Port provided the VME slot
 *  - This does not access the bus, just a map in the library.
 *  - FIXME: Make sure this works.
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


/*******************************************************************************
 *
 *  tidSetPrescale - Set the prescale factor for the external trigger
 *
 *     prescale: Factor for prescale.  
 *               Actual prescale will be set to 2^{prescale}
 *               Max {prescale} available is 15 or a factor of 32768
 *
 *  RETURNS: OK if successful, otherwise ERROR.
 *
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
  vmeWrite32(&TIDp->trigDelay,
	   ((vmeRead32(&TIDp->trigDelay) & ~(TID_TRIGDELAY_PRESCALE_MASK))
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
  rval = (vmeRead32(&TIDp->trigDelay) & (TID_TRIGDELAY_PRESCALE_MASK))>>28;
  TIDUNLOCK;

  return rval;
}

/*******************************************************************************
 *
 *  tidSetTriggerPulse - Set the characteristics of a specified trigger
 *
 *   trigger:  
 *           1: set for trigger 1
 *           2: set for trigger 2 (playback trigger)
 *   delay:    delay in units of 4ns
 *   width:    pulse width in units of 4ns
 *
 * RETURNS: OK if successful, otherwise ERROR
 *
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
      rval = vmeRead32(&TIDp->trigDelay) & 
	~(TID_TRIGDELAY_TRIG1_DELAY_MASK | TID_TRIGDELAY_TRIG1_WIDTH_MASK) ;
      rval |= ( (delay) | (width<<8) );
      vmeWrite32(&TIDp->trigDelay, rval);
    }
  if(trigger==2)
    {
      rval = vmeRead32(&TIDp->trigDelay) & 
	~(TID_TRIGDELAY_TRIG2_DELAY_MASK | TID_TRIGDELAY_TRIG2_WIDTH_MASK) ;
      rval |= ( (delay<<16) | (width<<24) );
      vmeWrite32(&TIDp->trigDelay, rval);
    }
  TIDUNLOCK;
  
  return OK;
}

/*******************************************************************************
 *
 *  tidSetSyncDelayWidth
 *  - Set the delay time and width of the Sync signal
 *
 *  ARGS:
 *       delay:  the delay (latency) set in units of 4ns.
 *       width:  the width set in units of 4ns.
 *
 */

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

  TIDLOCK;
  vmeWrite32(&TIDp->vmeSyncDelay,delay | (width<<8));
  TIDUNLOCK;

}

/*******************************************************************************
 *
 *  tidTrigLinkReset
 *  - Reset the trigger link.
 *
 */

void 
tidTrigLinkReset()
{
  if(TIDp == NULL) 
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return;
    }
  
  TIDLOCK;
  vmeWrite32(&TIDp->vmeSyncLoad,TID_TRIGLINK_DISABLE); 
  taskDelay(1);

  vmeWrite32(&TIDp->vmeSyncLoad,TID_TRIGLINK_DISABLE); 
  taskDelay(1);

  vmeWrite32(&TIDp->vmeSyncLoad,TID_TRIGLINK_ENABLE); 
  TIDUNLOCK;

  printf ("%s: Trigger Data Link was reset.\n",__FUNCTION__);
}

/*******************************************************************************
 *
 *  tidSyncReset
 *  - Generate a Sync Reset signal.  This signal is sent to the loopback and
 *    all configured TI Slaves.
 *
 */

void
tidSyncReset()
{
  if(TIDp == NULL) 
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return;
    }
  
  TIDLOCK;
  vmeWrite32(&TIDp->vmeSyncLoad,TID_SYNC_RESET); 
  TIDUNLOCK;

}

/*******************************************************************************
 *
 *  tidClockReset
 *  - Generate a Clock Reset signal.  This signal is sent to the loopback and
 *    all configured TI Slaves.
 *
 */

void
tidClockReset()
{
  if(TIDp == NULL) 
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return;
    }
  
  TIDLOCK;
  vmeWrite32(&TIDp->vmeSyncLoad,TID_250_CLOCK_RESET); 
  TIDUNLOCK;

}

/*************************************************************

 TID Interrupt/Polling routines

*************************************************************/

/*******************************************************************************
 *
 *  tidIntPoll
 *  - Routine that returns 1 if Block is available in buffer, otherwise 0.
 *
 */

int 
tidIntPoll()
{
  unsigned int sval=0;

  TIDLOCK;
  sval = (vmeRead32(&TIDp->daqStatus) & TID_DAQSTATUS_BLOCKS_AVAILABLE_MASK)>>24; 

  if(sval)
  {
    tidDaqCount = sval;
    tidIntCount++;
    TIDUNLOCK;
    return(1);
  } 
  else 
  {
    TIDUNLOCK;
    return(0);
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

/*******************************************************************************
 *
 *  tidInt
 *  - Default interrupt handler
 *    Handles the TID interrupt.  Calls a user defined routine,
 *    if it was connected with tidIntConnect()
 *    
 *  FIXME: Write this.
 *
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

/*******************************************************************************
 *
 *  tidPoll
 *  - Default Polling Server Thread
 *    Handles the polling of latched triggers.  Calls a user
 *    defined routine if was connected with tidIntConnect.
 *
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

      /* If still need Ack, don't test the Trigger Status */
      if(tidNeedAck) continue;

      tiddata = 0;
	  
      /* FIXME: So far, tidIntPoll never returns ERROR */
      tiddata = tidIntPoll();
      if(tiddata == ERROR) 
	{
	  printf("%s: ERROR: tidIntPoll returned ERROR.\n",__FUNCTION__);
	  break;
	}


      if(tiddata && tidIntRunning) /* FIXME: Not sure if tidIntRunning here is correct */
	{
	  INTLOCK; /* FIXME: Causes race condition with tidIntDisconnect if tidIntRunning not checked */
	
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

/*******************************************************************************
 *
 *  tidStartPollingThread
 *  - Routine that launches tidPoll in its own thread 
 *
 */

static void
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


/*******************************************************************************
 *
 *
 *  tidIntConnect 
 *  - Connect a user routine to the TIR Interrupt or
 *    latched trigger, if polling.
 *
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
  vmeWrite32(&TIDp->intSetup, 
	   (vmeRead32(&TIDp->intSetup) & ~(TID_INTSETUP_LEVEL_MASK | TID_INTSETUP_VEC_MASK) ) 
	   | (tidIntLevel<<8) | tidIntVec );
  TIDUNLOCK;

  switch (tidReadoutMode)
    {
    case TID_READOUT_TS_POLL:
    case TID_READOUT_EXT_POLL:
      break;

    case TID_READOUT_TS_INT:
    case TID_READOUT_EXT_INT:
#ifdef VXWORKS
      intConnect(INUM_TO_IVEC(tidIntVec),tidInt,arg);
#else
      status = vmeIntConnect (tidIntVec, tidIntLevel,
			      tidInt,arg);
      if (status != OK) 
	{
	  printf("vmeIntConnect failed\n");
	  return(ERROR);
	}
#endif  
      break;

    default:
      printf("%s: ERROR: TID Mode not defined (%d)\n",
	     __FUNCTION__,tidReadoutMode);
      return ERROR;
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

/*******************************************************************************
 *
 *
 *  tidIntDisconnect
 *  - Disable interrupts or kill the polling service thread
 *
 *  FIXME: Waiting for regs and bits 
 *
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
      logMsg("%s: ERROR: TID is Enabled - Call tidIntDisable() first\n",
	     __FUNCTION__,2,3,4,5,6);
      return ERROR;
    }

  INTLOCK;

  switch (tidReadoutMode) 
    {
    case TID_READOUT_TS_INT:
    case TID_READOUT_EXT_INT:

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

    case TID_READOUT_TS_POLL:
    case TID_READOUT_EXT_POLL:
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

/*******************************************************************************
 *
 *  tidAckConnect
 *  - Connect a user routine to be executed instead of the default 
 *  TID interrupt/trigger latching acknowledge prescription
 *
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

/*******************************************************************************
 *
 *  tidIntAck
 *  - Acknowledge an interrupt or latched trigger.  This "should" effectively 
 *  release the "Busy" state of the TID.
 *  Execute a user defined routine, if it is defined.  Otherwise, use
 *  a default prescription.
 *
 *  FIXME: Still need the register and bit
 */
void
tidIntAck()
{
  if(TIDp == NULL)
  {
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
    vmeWrite32(&TIDp->reset,TID_RESET_BLOCK_ACK);
    TIDUNLOCK;
  }

}

/*******************************************************************************
 *
 *  tidIntEnable
 *  - Enable interrupts or latching triggers (depending on set TID mode)
 *  
 *  if iflag==1, trigger counter will be reset
 *
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
    vmeWrite32(&TIDp->reset,(1<<4));
    printf("%s: Set me free\n",__FUNCTION__);
    tidIntCount = 0;
  }

  tidIntRunning = 1;
  tidDoAck      = 1;
  tidNeedAck    = 0;

  switch (tidReadoutMode)
  {
    case TID_READOUT_TS_POLL:
    case TID_READOUT_EXT_POLL:
      tidStartPollingThread();
      break;

    case TID_READOUT_TS_INT:
    case TID_READOUT_EXT_INT:
#ifdef VXWORKS
      lock_key = intLock();
      sysIntEnable(tidIntLevel);
#endif
      vmeWrite32(&TIDp->enableBits,
	  vmeRead32(&TIDp->enableBits) | (1<<13) | (1<<21));
      break;

    default:
      tidIntRunning = 0;
#ifdef VXWORKS
      if(lock_key) intUnlock(lock_key);
#endif
      printf("%s: ERROR: TID Readout Mode not defined %d\n",
	     __FUNCTION__,tidReadoutMode);
      TIDUNLOCK;
      return(ERROR);    
  }

  TIDUNLOCK; /* Locks performed in tidEnableTriggerSource() */

  tidEnableTriggerSource();

#ifdef VXWORKS
  if(lock_key)
    intUnlock(lock_key);
#endif

  return(OK);
}

/*******************************************************************************
 *
 *  tidIntDisable
 *  - Disable interrupts or latching triggers
 *
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

  TIDLOCK;
  vmeWrite32(&TIDp->enableBits,
	     vmeRead32(&TIDp->enableBits) & ~(TID_ENABLEBITS_IRQ_ENABLE));
  tidIntRunning = 0;
  TIDUNLOCK;
}

/*******************************************************************************
 *
 *  tidBready
 *   - Returns the number of Blocks available for readout
 *
 * RETURNS: Number of blocks available for readout if successful, otherwise ERROR
 *
 */

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
  rval = (vmeRead32(&TIDp->daqStatus)&TID_DAQSTATUS_BLOCKS_AVAILABLE_MASK)>>24;
  TIDUNLOCK;

  return rval;
}

/*******************************************************************************
 *
 *  tidSetBlockAckThreshold
 *   - Set the threshold for the difference between blocks in the system
 *     (all masters/slaves) and the blocks requiring an acknowledge.
 *
 *     If this threshold is met, the TI Master will go BUSY.
 *     The BUSY is released as soon as the difference is one less of the threshold
 *
 *  ARG:   thres: 
 *               0:  No threshold  -  Pipeline mode
 *               1:  One Acknowdge for each Block - "ROC LOCK" mode
 *           2-255:  "Buffered" mode.
 *
 * RETURNS: OK if successful, otherwise ERROR
 *
 */

int
tidSetBlockAckThreshold(unsigned int thres)
{
  if(TIDp == NULL) 
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }

  if(thres>0xff)
    {
      printf("%s: ERROR: Invalid value for thres (%d)\n",
	     __FUNCTION__,thres);
      return ERROR;
    }

  TIDLOCK;
  vmeWrite32(&TIDp->intSetup, (vmeRead32(&TIDp->intSetup) & ~TID_INTSETUP_BLOCK_WAIT_THRES)
	   | (thres<<24));
  TIDUNLOCK;

  return OK;
}

/*******************************************************************************
 *
 *   tidSetupFiberLoopback
 *   - A test setup using Fiber 5 output to Fiber 1 to provide a trigger path
 *
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
  vmeWrite32(&TIDp->reset,0x4000);

  vmeWrite32(&TIDp->vmeSyncLoad,0x77);

  vmeWrite32(&TIDp->vmeSyncLoad,0x55);

  vmeWrite32(&TIDp->triggerCmdCode,0x18);

  vmeWrite32(&TIDp->trigDelay,0x300);
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
    vmeWrite32(&TIDp->trigTable[ipat], trigPattern[ipat]);

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
  rval = vmeRead32(&TIDp->daqStatus);
  TIDUNLOCK;

  return rval;
}

/*******************************************************************************
 *
 *   tidEnableTSInput / tidDisableTSInput
 *   - Enable/Disable trigger inputs labelled TS#1-6 on the Front Panel
 *     These inputs MUST be disabled if not connected.
 *
 *   ARGs:   inpMask:
 *       bits 0:  TS#1
 *       bits 1:  TS#2
 *       bits 2:  TS#3
 *       bits 3:  TS#4
 *       bits 4:  TS#5
 *       bits 5:  TS#6
 *
 */

int
tidEnableTSInput(unsigned int inpMask)
{
  unsigned int rval;
  if(TIDp == NULL) 
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }

  if(inpMask>0x3f)
    {
      printf("%s: ERROR: Invalid inpMask (0x%x)\n",__FUNCTION__,inpMask);
      return ERROR;
    }

  TIDLOCK;
  rval = vmeRead32(&TIDp->intSetup);
  vmeWrite32(&TIDp->intSetup,
	     rval |
	   ((inpMask<<16) & (TID_INTSETUP_GTPINPUT_MASK)) );
  TIDUNLOCK;

  return OK;
}

int
tidDisableTSInput(unsigned int inpMask)
{
  unsigned int rval;
  if(TIDp == NULL) 
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }

  if(inpMask>0x3f)
    {
      printf("%s: ERROR: Invalid inpMask (0x%x)\n",__FUNCTION__,inpMask);
      return ERROR;
    }

  TIDLOCK;
  rval = vmeRead32(&TIDp->intSetup);
  vmeWrite32(&TIDp->intSetup,
	     rval & ~((inpMask<<16) & (TID_INTSETUP_GTPINPUT_MASK)) );
  TIDUNLOCK;

  return OK;
}

/*******************************************************************************
 *
 *   tidSetOutputPort
 *   - Set (or unset) high level for the output ports on the front panel
 *     labelled as O#1-4
 *
 *   ARGs:   
 *       set1:  O#1
 *       set2:  O#2
 *       set3:  O#3
 *       set4:  O#4
 *
 */

int
tidSetOutputPort(unsigned int set1, unsigned int set2, unsigned int set3, unsigned int set4)
{
  unsigned int bits=0;
  if(TIDp == NULL) 
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }
  
  if(set1)
    bits |= (1<<0);
  if(set2)
    bits |= (1<<1);
  if(set3)
    bits |= (1<<2);
  if(set4)
    bits |= (1<<3);

  TIDLOCK;
  vmeWrite32(&TIDp->oport, bits);
  TIDUNLOCK;

  return OK;
}



/*******************************************************************************
 *
 *   tidSetClockSource
 *   - Set the clock to the specified source.
 *
 *   ARGs:   source:
 *            0:  Onboard clock (for TI Master)
 *            1:  HFBR #1 (for TI Slaves)
 *            2:  HFBR #5
 *            3:  Front Panel Input
 *
 */

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
    case 0: /* ONBOARD */
      clkset = TID_INTSETUP_CLK_ONBOARD;
      break;
    case 1:
      clkset = TID_INTSETUP_CLK_HFBR1;
      break;
    case 2:
      clkset = TID_INTSETUP_CLK_HFBR5;      
      break;
    case 3:
      clkset = TID_INTSETUP_CLK_FP;
      break;
    }

  TIDLOCK;
  vmeWrite32(&TIDp->intSetup,
	   (vmeRead32(&TIDp->intSetup) & ~(TID_INTSETUP_CLK_SOURCE_MASK))
	   | clkset);
  /* Reset DCM (Digital Clock Manager) - 250/200MHz */
  vmeWrite32(&TIDp->reset,TID_RESET_DCM_250);
  taskDelay(1);
  /* Reset DCM (Digital Clock Manager) - 125MHz */
  vmeWrite32(&TIDp->reset,TID_RESET_DCM_125);
  taskDelay(1);
  TIDUNLOCK;

  return OK;
}

/*******************************************************************************
 *
 * tidVmeTrigger1
 *    - fire a single trigger 1 via VME
 *
 */

int
tidVmeTrigger1()
{
  if(TIDp == NULL) 
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }
  
  TIDLOCK;
  vmeWrite32(&TIDp->triggerCmdCode, 0x0018);
  TIDUNLOCK;
  return OK;

}

/*******************************************************************************
 *
 * tidVmeTrigger2
 *    - fire a single trigger 2 via VME
 *
 */

int
tidVmeTrigger2()
{
  if(TIDp == NULL) 
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }
  
  TIDLOCK;
  vmeWrite32(&TIDp->triggerCmdCode, 0x0180);
  TIDUNLOCK;
  return OK;

}

/*******************************************************************************
 *
 * tidSetFiberDelay
 *    - Set the delay (for this crate) and offset (common to all crates)
 *      required to align the sync and triggers for all crates.
 *
 *  For the EEL TwoCrate tests... offset = 0xcf
 *
 */


void
tidSetFiberDelay(unsigned int delay, unsigned int offset)
{
  unsigned int fiberLatency, syncDelay;
  if(TIDp == NULL) 
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return;
    }

  fiberLatency=0;
  TIDLOCK;
  vmeWrite32(&TIDp->reset,0x4000);  /* reset the IODELAY */
  taskDelay(10);
  
  vmeWrite32(&TIDp->reset,0x800);   /* auto adjust the sync phase for HFBR#1 */
  taskDelay(10);
  vmeWrite32(&TIDp->reset,0x1000);  /* auto adjust the sync phase for HFBR#5 */
  taskDelay(10);

  syncDelay = (offset-(delay));
  syncDelay=(syncDelay<<16)&0xff0000;  /* set the sync delay according to the fiber latency */

  printf("%s\n\n syncDelay = 0x%08x\n\n",__FUNCTION__,syncDelay);

  vmeWrite32(&TIDp->fiberSyncAlignment,syncDelay);
  syncDelay = vmeRead32(&TIDp->fiberSyncAlignment);

  TIDUNLOCK;

}

/*******************************************************************************
 *
 * tidAddSlave
 *    - Add and configurate a TI Slave for the TI Master.
 *      This routine should be used by the TI Master to configure
 *      HFBR ports and BUSY sources.
 *  ARGs:
 *     fiber:  The fiber port of the TI Master that is connected to the slave
 *
 */

int
tidAddSlave(unsigned int fiber)
{
  if(TIDp == NULL) 
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }

  if(!tidMaster)
    {
      printf("%s: ERROR: TID is not the TI Master.\n",__FUNCTION__);
      return ERROR;
    }

  if((fiber<1) || (fiber>8) )
    {
      printf("%s: ERROR: Invalid value for fiber (%d)\n",
	     __FUNCTION__,fiber);
      return ERROR;
    }

  if(fiber<5)
    {
      printf("%s: ERROR: TI Slaves connected to TI Master HFBR %d not supported.\n",
	     __FUNCTION__,fiber);
      return ERROR;
    }

  /* Add this slave to the global slave mask */
  tidSlaveMask |= (1<<(fiber-1));
  
  /* Add this fiber as a busy source (use first fiber macro as the base) */
  if(tidSetBusySource(TID_BUSY_HFBR1<<(fiber-1),0)!=OK)
    return ERROR;

  /* Enable the fiber */
  if(tidEnableFiber(fiber)!=OK)
    return ERROR;

  return OK;

}

/* FIXME: Replace this one with a one that's a bit more descriptive of the process */
void 
FiberMeas(void)
{
  unsigned int fiberLatency, syncDelay;
  if(TIDp == NULL) {
    printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
    return;
  }
  
  TIDLOCK;
  vmeWrite32(&TIDp->reset,0x4000);  /* reset the IODELAY */
  taskDelay(10);
  vmeWrite32(&TIDp->reset,0x2000);  /* auto adjust the return signal phase */
  taskDelay(10);
  vmeWrite32(&TIDp->reset,0x8000);  /* measure the fiber latency */
  taskDelay(10);

  printf("%s: Fiber Latency is 0x%x\n",
	 __FUNCTION__,vmeRead32(&TIDp->fiberLatencyMeasurement));
  
  vmeWrite32(&TIDp->reset,0x800);   /* auto adjust the sync phase for HFBR#1 */
  taskDelay(10);
  vmeWrite32(&TIDp->reset,0x1000);  /* auto adjust the sync phase for HFBR#5 */
  taskDelay(10);

  fiberLatency = vmeRead32(&TIDp->fiberLatencyMeasurement);
  syncDelay = (0x7f-(((fiberLatency>>23)&0x1ff)/2));
  syncDelay=(syncDelay<<8)&0xff00;  /* set the sync delay according to the fiber latency */

  printf("%s\n\n fiberLatency = 0x%08x   syncDelay = 0x%08x\n\n",__FUNCTION__,fiberLatency,syncDelay);

  vmeWrite32(&TIDp->fiberSyncAlignment,syncDelay);
  syncDelay = vmeRead32(&TIDp->fiberSyncAlignment);

#ifdef GUSTUFF
  /* A24 buffer write */
  sysBusToLocalAdrs(0x39,TID_REG_ADDR+0x100,&laddr);		/* trigger interface */
  *laddr = 0x4000;  /* reset the IODELAY */
  cpuDelay(10000);
  *laddr = 0x2000;  /* auto adjust the return signal phase */
  cpuDelay(10000000);
  *laddr = 0x8000;  /* measure the fiber latency */
  cpuDelay(10000000);

  sysBusToLocalAdrs(0x39,TID_REG_ADDR+0x18,&laddr);		/* trigger interface */
  printf("Fiber Latency is %x\n",*laddr);

  sysBusToLocalAdrs(0x39,TID_REG_ADDR+0x100,&laddr);		/* trigger interface */
  *laddr = 0x800;   /* auto adjust the sync phase for HFBR#1 */
  cpuDelay(10000000);
  *laddr = 0x1000;  /* auto adjust the sync phase for HFBR#5 */
  cpuDelay(10000000);

  sysBusToLocalAdrs(0x39,TID_REG_ADDR+0x18,&laddr);		/* trigger interface */
  fiberLatency = *laddr;  /* fiber latency measurement result */
  syncDelay = (0x7f-(((fiberLatency>>23)&0x1ff)/2));
  syncDelay=(syncDelay<<8)&0xff00;  /* set the sync delay according to the fiber latency */
  cpuDelay(1000);

  sysBusToLocalAdrs(0x39,TID_REG_ADDR+0x1c,&laddr);		/* trigger interface */
  *laddr = syncDelay;
  syncDelay= *laddr;
#endif

  TIDUNLOCK;

  printf (" \n The fiber latency of 0x18 is: %x \n", fiberLatency );
  printf (" \n The sync latency of 0x1C is: %x \n",syncDelay);
}



#else /* dummy version*/

void
tidLib_dummy()
{
  return;
}

#endif
