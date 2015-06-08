/* ncr710Lib.c - NCR 53C710 SCSI I/O Processor (SIOP) library (SCSI-1) */

/* Copyright 1989-1999 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
02v,11jan99,aeg  added semTerminate () in ncr710StepEnable1 () when disabling.
02u,26aug98,sut  renamed ncr710StepEnable into ncr710StepEnable1 ;
                 in ncr710CtrlCreate added support to use
                 ncr710StepEnable1 as the ncr710StepEnable
                 see also ncr710CommLib.c
02t,24aug98,sut  renamed ncr710SingleStep into ncr710SingleStep1 ;
                 in ncr710CtrlCreate added support to use
                 ncr710SingleStep1 as the ncr710SingleStep
                 see also ncr710CommLib.c
02s,12sep97,dds  SPR # 4831: changing clock frequency value has no effect on
                 the DCNTL register.
02r,03may96,jds  and more doc tweaks...
02q,01may96,jds  yet more doc tweaks...
02p,08nov95,jds  more doc tweaks.
02o,13feb95,jdi  doc tweak.
02n,07nov94,jds  incorporated Carl's patch for cache coherency (spr 2210)
02m,23feb93,jdi  documentation cleanup.
02l,10feb93,ccc  fixed cache problem and null pointers (spr 1995).
02k,26sep92,ccc  doc changes.
02j,18aug92,ccc  fixed warnings.
02i,31jul92,dnw  Changed to new cacheLib.
		 Added requirement that dma cache be write coherent and
		 removed corresponding flushes.
02h,20jul92,eve  Move debug macros from ncr710.h.
02g,20jul92,eve  Added cache support for 5.1. 
02f,18jul92,smb  Changed errno.h to errnoLib.h.
02e,13jul92,eve	 Make a single exit process both for OK and ERROR in 
		 ncr710ScsiPhase() and create ncr710CheckStatRegs().
02d,11jul92,eve	 Suppress ncr710CmdBuild() since scsiCmdBuild() is public
		 in scsiLib.c.Suppress ncr710Relocation() routine.
02c,02jul92,eve	 start ANSI modifications.
02b,21apr92,eve	 Add disconnect capability.
02a,03mar92,eve	 Add the process for new bit in the ncr710HwRegister() 
		 and suppress access to the EA bit in the other routines. 
01n,20feb92,eve  Change the clock frequency parameter by a value
                 like xxns * 100 in ncr710CtrlCreate().
01m,12jan91,eve  Add support for sync transfer capability.
01l,10dec91,eve  Correct bug in ncr710ScsiPhase() regarding the Bus fault
                 Wtd timeout and illegal instruction.
01k,29nov91,eve  Change semTake timeout in ncr710StartPhase().
01j,29nov91,eve  Correct use of pDevToSelect,suppress initialisation
                 devSync semaphore in ncr710HwInit().
01i,07nov91,eve  Perform a select with attention in ncr710ScsiPhase()
		 if useIdentify is set.
01h,01nov91,ccc  Documentation changes, changed modification numbers.
01g,01nov91,eve  Suppress pccchip2 include
01f,27oct91,eve  Suppress regOffset argument in ncr710CtlrCreate(),
		 the chip need to be connected to all of the address 
		 bus (31-0).
01e,27oct91,eve  Add ncr710SetHwRegister() routine.
01d,25oct91,eve  Allocate dynamicly ncrCtlShare data structure
		 keep only a pointer in SIOP info.
01c,24oct91,eve	 Change clockPeriod by a frequency value
		 in ncr710CtrlCreate().
01b,24oct91,eve  Try to cleaning up documentation.
01a,23oct91,eve  Created. 
*/

/*
DESCRIPTION
This is the I/O driver for the NCR 53C710 SCSI I/O Processor (SIOP).
It is designed to work with scsi1Lib.  It also runs in
conjunction with a script program for the NCR 53C710 chip.
This script uses the NCR 53C710 DMA function for data transfers.
This driver supports cache functions through cacheLib.

USER-CALLABLE ROUTINES
Most of the routines in this driver are accessible only through the I/O
system.  Three routines, however, must be called directly:  ncr710CtrlCreate()
to create a controller structure, and ncr710CtrlInit() to initialize it.
The NCR 53C710 hardware registers need to be configured according to
the hardware implementation.  If the default configuration is not proper,
the routine ncr710SetHwRegister() should be used to properly configure
the registers.

INTERNAL
This driver supports the connect/disconnect capability.  But this feature is 
invalidated in the driver code in ncr710CtrlInit() :
.CS
    /@ disconnect is supported for now,but this feature is not included
     @ in this revision.
     @/
    pSiop->scsiCtrl.disconnect = FALSE; /@ TRUE to support disconnect @/; 
.CE   
To validate the disconnect, this field must be set to TRUE (and recompile)
and the field <pScsiPhysDev->useIdentify> must be also set to TRUE.
It also support the synchronous message.  The new scsiSyncTarget() routine 
in the scsiLib could be use to set up a synchronous protocol between the target
and the initiator.
There are debug variables to trace events in the driver.
<scsiDebug> scsiLib debug variable, trace event in scsiLib, ncr710ScsiPhase(),
and ncr710ScsiTransact().
<scsiIntsDebug> prints interrupt informations.
<ncr710LogErr> print out error case and  dump the ncr710 register contents.
<ncr710Debug> Print out informations during the SCSI transaction. 

INCLUDE FILES
ncr710.h, ncr710_1.h, ncr710Script.h, ncr710Script1.h

SEE ALSO: scsiLib, scsi1Lib, cacheLib,
.I "NCR 53C710 SCSI I/O Processor Programming Guide,"
.pG "I/O System"
*/

#include "vxWorks.h"
#include "memLib.h"
#include "ctype.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "logLib.h"
#include "semLib.h"
#include "ioLib.h"
#include "errnoLib.h"
#include "cacheLib.h"
#include "taskLib.h"
#include "drv/scsi/ncr710.h"
#include "drv/timer/timerDev.h"

/* defines */

typedef NCR_710_SCSI_CTRL SIOP;

#define SIOP_MAX_XFER_LENGTH  	((UINT) (0x00ffffff)) /* max data xfer length */

/* Time out used in the semtake in the driver.  Because this is a global value  
 * that not a good way to handle a time out for a scsi command.  This should 
 * be done in the scsiLib for each command running and processed by the
 * driver during the devSync semTake(). 
 */ 
#define DEF_TIMEOUT 0x1000000


/* ncr710 debug level */ 
#define NCR710_MSG	\
	printErr

#define NCR710_DEBUG_MSG	\
    if (ncr710Debug)		\
        NCR710_MSG 

/* screen log error at task level */
#define LOG_ERR\
        if (ncr710LogErr == 1)\
                printErr
        
#define LOG_ERR_CTXT    \
        if (ncr710LogErr == 1) ncr710Show((SCSI_CTRL *)pSiop)
 
/* ncr710 debug phase mismatch level */
#define NCR710_MM_MSG\
        logMsg
 
#define NCR710_MISMATCH_MSG\
    if (ncr710MmatchLog == 1)\
        NCR710_MM_MSG

/* External */
IMPORT BOOL scsiDebug;
IMPORT BOOL scsiIntsDebug;
IMPORT UINT relocation;	      /* relocation table information */
IMPORT UINT selWithAtn_idx;   /* index for the first entry point */
IMPORT UINT waitSelect;	      /* Idle script entry point */
IMPORT UINT selWithAtn;	      /* First scsi Script routine atn set */
IMPORT UINT selWithoutAtn;    /* First scsi Script routine ,no atn */
IMPORT UINT ackMsg1;	      /* retry entry Script routine for extended msg */
			      /* before a data phase */
IMPORT UINT ackMsg3;	      /* retry entry Script routine msg after data */
			      /* phase */
IMPORT UINT checkPhData;      /* Entry Script routine to ack a message */
			      /* after a data phase and look for the next */
			      /* phase. */
IMPORT UINT ackAtnMsg;	      /* Entry Script routine to send a msg out */ 
IMPORT UINT endAbort;	      /* Entry Script when a msg out as reset */
			      /* device is send */ 
IMPORT UINT asortPh;	      /* Entry script to sort phase before data */
			      /* phase */
IMPORT UINT checkNewCmd;      /* Entry point to start a new cmd at intr lvl */ 
IMPORT UINT *script_ptr[];    /* table of all entry in the script */


IMPORT VOIDFUNCPTR ncr710SingleStepRtn;   /* Single step routine */
IMPORT VOIDFUNCPTR ncr710StepEnableRtn; /* Single step routine */

/* This part allow to log some information at interrupt level. That a 
 * conditionnal compile feature.  This is useful to keep track of 
 * disconnect/reconnect operation at interrupt level.
 * A show routine, ncr710LogShow() print and increment the <pLogMemShow>
 * pointer.
 */
#if FALSE    /* FALSE to disable interrupt log memory trace 
              * at the compile time.
              */

/* To enable interrupt log memory capability at the compile time. Allocated 
* memory with a malloc (size set by LOG_SIZE_MEM) and assign this return 
* value to <pNcr710Deb>, and pLogMemCount, set ncr710LogMem to 1,  and 
* ncr710LogCount to 0 .
*/
#define NCR710_INT_LOG_MEM

#endif /* TRUE/FALSE */ 


#ifdef NCR710_INT_LOG_MEM	/* Interrupt trace enabled */  

#define LOG_SIZE_MEM   0x10000 /* Memory log size */

int ncr710LogMem = 0;		/* enable/disable memory log at the run time */
UINT *pNcr710Deb;		/* pointer to the beginning of log memory */
				/* array */
UINT *pLogMemShow;		/* internal pointer for incremental show of */
				/* log memory */
int ncr710LogCount = 0;		/* memory log fifo counter */

/* macro to write memory log element */
#define LOG_MEM(s)				\
	*(pNcr710Deb + ncr710LogCount)= s;	\
	ncr710LogCount++;			\
        if (ncr710LogCount >= (LOG_SIZE_MEM /4))\
		ncr710LogCount = 0;

#define LOG_MEM_INTR\
    if (ncr710LogMem == 1)				\
        {						\
        LOG_MEM((UINT)ncr710CountIntr)			\
        LOG_MEM((UINT)ncr710LogCount)			\
        LOG_MEM(*pSiop->pDsps)				\
        LOG_MEM((UINT)(*pSiop->pScratch3))		\
        LOG_MEM((UINT)pSiop->pNcrCtl)			\
        LOG_MEM((UINT)pSiop->pNcrCtlCmd)		\
        LOG_MEM((UINT)pScsiPhysDev)			\
        LOG_MEM((UINT)pSiop->pNcrCtl->pScsiPhysDev)	\
        LOG_MEM((UINT)pSiop->pNcrCtl->pData)		\
        LOG_MEM(pSiop->pNcrCtl->dataCount)		\
        LOG_MEM((UINT)pSiop->commandRequest)		\
        LOG_MEM((UINT)(*pSiop->pScratch0))		\
        LOG_MEM((UINT)(*pSiop->pSbcl))			\
        LOG_MEM((UINT)(*pSiop->pSdid))			\
        LOG_MEM((UINT)(*pSiop->pIstat))			\
        LOG_MEM((UINT)(0x99999999))			\
        }
#endif /* NCR710_INT_LOG_MEM */


/* variables */ 

TBOOL siopSStep;		/* Global state of single step script */
				/* execution */
int ncr710CountIntr = 0;	/* Global interrupt counter */

/* Debug variables use by the debug macros */
int ncr710Debug = 0;            /* <1> enable debug msg */
int ncr710MmatchLog = 0;        /* <1> enable mismatch screen log (int lvl) */
int ncr710LogErr = 0;           /* <1> enable print task level screen error 
                                 * log (task lvl).
                                 */
/* default timeout value use in semTake (devSyncSem.. */
int ncr710TimeOut = DEF_TIMEOUT; 

/* forward declarations */

LOCAL void ncr710SelTimeOutCvt (SCSI_CTRL *pScsiCtrl,UINT timeOutInUsec,
                                   UINT *pTimeOutSetting);
LOCAL void ncr710ScsiBusReset (SIOP *pSiop);
LOCAL STATUS ncr710ScsiTransact(SCSI_PHYS_DEV *pScsiPhysDev, 
                                SCSI_TRANSACTION *pScsiXaction);
LOCAL STATUS ncr710ScsiPhase(SCSI_PHYS_DEV *pScsiPhysDev, 
                             SCSI_TRANSACTION *pScsiXaction);
STATUS ncr710SetHwRegister (SIOP *pSiop, NCR710_HW_REGS *pHwRegs);
LOCAL void ncr710HwInit (SIOP *pSiop);
LOCAL STATUS ncr710SyncMsgConvert (SCSI_PHYS_DEV *pScsiPhysDev, UINT8 *pMsgIn,
                                   int *pSyncVal, int *pOffsetVal);	
LOCAL UINT *ncr710RejectRecv (SIOP *pSiop, SCSI_PHYS_DEV *pScsiPhysDev, 
                              NCR_CTL *pNcrCtl);
LOCAL UINT *ncr710ExtMsgRecv (SIOP *pSiop, SCSI_PHYS_DEV *pScsiPhysDev,
                              NCR_CTL *pNcrCtl, UINT8 *msgIn);
LOCAL UINT *ncr710PhaseMismatch (SIOP *pSiop, SCSI_PHYS_DEV *pScsiPhysDev, 
                                 NCR_CTL *pNcrCtl, UINT phaseRequested);
LOCAL STATUS ncr710BuildMsgOut (SCSI_PHYS_DEV *pScsiPhysDev);
LOCAL STATUS ncr710FirstStartPhase (SIOP *pSiop, NCR_CTL *pNcrCtl, 
                                    SCSI_PHYS_DEV *pScsiPhysDev, int timeOut); 
LOCAL STATUS ncr710StartPhase (SIOP *pSiop, NCR_CTL *pNcrCtl, 
			       UINT *scriptAddress, int timeOut); 
LOCAL void ncr710StartScript (SIOP *pSiop, NCR_CTL *pNcrCtl, 
                              SCSI_PHYS_DEV *pScsiPhysDev, UINT *scriptEntry);
LOCAL STATUS ncr710CheckStatRegs (SIOP *pSiop, SCSI_PHYS_DEV *pScsiPhysDev,
				  UINT *localStart);
LOCAL void ncr710FlushCache (SIOP *pSiop, NCR_CTL *pNcrCtl);

LOCAL void ncr710SingleStep1 (SIOP* pSiop);
LOCAL void ncr710StepEnable1 (SIOP* pSiop, BOOL boolValue);

/*******************************************************************************
*
* ncr710CtrlCreate - create a control structure for an NCR 53C710 SIOP
*
* This routine creates an SIOP data structure and must be called before
* using an SIOP chip.  It should be called once and only once for a 
* specified SIOP.  Since it allocates memory for a structure needed by all
* routines in ncr710Lib, it must be called before any other routines in the
* library.  After calling this routine, ncr710CtrlInit() should be called
* at least once before any SCSI transactions are initiated using the SIOP.
*
* A detailed description of the input parameters follows:
* .iP <baseAdrs> 4
* the address at which the CPU accesses the lowest 
* register of the SIOP.
* .iP <freqValue>
* the value at the SIOP SCSI clock input.  This is used to determine 
* the clock period for the SCSI core of the chip and the synchronous
* divider value for synchronous transfer.
* It is important to have the right timing on the SCSI bus.
* The <freqValue> parameter is defined as the SCSI clock input value, in 
* nanoseconds, multiplied by 100.  Several <freqValue> constants are 
* defined in ncr710.h as follows:
* .CS
*     NCR710_1667MHZ  5998    /@ 16.67Mhz chip @/
*     NCR710_20MHZ    5000    /@ 20Mhz chip    @/
*     NCR710_25MHZ    4000    /@ 25Mhz chip    @/
*     NCR710_3750MHZ  2666    /@ 37.50Mhz chip @/
*     NCR710_40MHZ    2500    /@ 40Mhz chip    @/
*     NCR710_50MHZ    2000    /@ 50Mhz chip    @/
*     NCR710_66MHZ    1515    /@ 66Mhz chip    @/
*     NCR710_6666MHZ  1500    /@ 66.66Mhz chip @/
* .CE
*
* RETURNS: A pointer to the NCR_710_SCSI_CTRL structure, or NULL if memory 
* is insufficient or parameters are invalid. 
*/

NCR_710_SCSI_CTRL *ncr710CtrlCreate 
    (
    UINT8 *baseAdrs,		/* base address of the SIOP */
    UINT freqValue		/* clock controller period (nsec*100) */
    )

    {
    FAST SIOP *pSiop;		    /* ptr to SIOP info */
    FAST NCR_CTL_CTXT *pNcrCtxt;    /* ptr to SIOP context */	 
	/* local dependant hardware registers values */
    static NCR710_HW_REGS hwRegs = DEFAULT_710_HW_REGS;

    /* SingleStep routine selection */
    
    ncr710SingleStepRtn = ncr710SingleStep1;
    ncr710StepEnableRtn = ncr710StepEnable1;
    

    /* verify parameters */
    if (baseAdrs == ((UINT8 *) NULL))
	return ((SIOP *) NULL);
    
    if ((freqValue > NCR710_1667MHZ) ||  
        (freqValue < NCR710_6666MHZ))
        return ((SIOP *) NULL);
    
    /* check that dma buffers are write coherent */

    if (!CACHE_DMA_IS_WRITE_COHERENT () ||
	!CACHE_DMA_IS_READ_COHERENT ())
	{
	printf ("\nshared memory not cache coherent\n");
        return ((SIOP *) NULL);
	}

    /* calloc the controller info structure; return ERROR if unable */

    pSiop = (SIOP *) cacheDmaMalloc (sizeof (SIOP));

    if (pSiop == (SIOP *) NULL)
        return ((SIOP *) NULL);

    bzero ((char *) pSiop, sizeof (SIOP));

    /* cacheDmaMalloc the data share info structure; return ERROR if unable */

    pNcrCtxt = (NCR_CTL_CTXT *) cacheDmaMalloc (sizeof (NCR_CTL_CTXT));

    if (pNcrCtxt == (NCR_CTL_CTXT *) NULL)
	{
	printErr("ncr710CtrlCreate :malloc NCR_CTL_CTXT context failed\n");
	(void) cacheDmaFree ((char *) pSiop);
        return((SIOP *) NULL);
	}

    bzero ((char *) pNcrCtxt, sizeof (NCR_CTL_CTXT));

    /* fill in generic SCSI info for this controller */

    scsiCtrlInit (&pSiop->scsiCtrl);

    /* Init NcrCtxt pointer in siop structure */

    pSiop->pNcrCtxt = pNcrCtxt;

    /* fill in SIOP specific data for this controller */

    pSiop->pSien     = baseAdrs + OFF_SIEN;
    pSiop->pSdid     = baseAdrs + OFF_SDID;
    pSiop->pScntl1   = baseAdrs + OFF_SCNTL1;
    pSiop->pScntl0   = baseAdrs + OFF_SCNTL0;
    pSiop->pSocl     = baseAdrs + OFF_SOCL;
    pSiop->pSodl     = baseAdrs + OFF_SODL;
    pSiop->pSxfer    = baseAdrs + OFF_SXFER;
    pSiop->pScid     = baseAdrs + OFF_SCID;
    pSiop->pSbcl     = baseAdrs + OFF_SBCL;
    pSiop->pSbdl     = baseAdrs + OFF_SBDL;
    pSiop->pSidl     = baseAdrs + OFF_SIDL;
    pSiop->pSfbr     = baseAdrs + OFF_SFBR;
    pSiop->pSstat2   = baseAdrs + OFF_SSTAT2;
    pSiop->pSstat1   = baseAdrs + OFF_SSTAT1;
    pSiop->pSstat0   = baseAdrs + OFF_SSTAT0;
    pSiop->pDstat    = baseAdrs + OFF_DSTAT;
    pSiop->pDsa      = (UINT *) (baseAdrs + OFF_DSA);
    pSiop->pCtest3   = baseAdrs + OFF_CTEST3;
    pSiop->pCtest2   = baseAdrs + OFF_CTEST2;
    pSiop->pCtest1   = baseAdrs + OFF_CTEST1;
    pSiop->pCtest0   = baseAdrs + OFF_CTEST0;
    pSiop->pCtest7   = baseAdrs + OFF_CTEST7;
    pSiop->pCtest6   = baseAdrs + OFF_CTEST6;
    pSiop->pCtest5   = baseAdrs + OFF_CTEST5;
    pSiop->pCtest4   = baseAdrs + OFF_CTEST4;
    pSiop->pTemp     = (UINT *) (baseAdrs + (OFF_TEMP));
    pSiop->pLcrc     = baseAdrs + OFF_LCRC;
    pSiop->pCtest8   = baseAdrs + OFF_CTEST8;
    pSiop->pIstat    = baseAdrs + OFF_ISTAT;
    pSiop->pDfifo    = baseAdrs + OFF_DFIFO;
    pSiop->pDcmd     =  baseAdrs + OFF_DCMD;
    pSiop->pDbc      = (UINT *) (baseAdrs + (OFF_DBC));
    pSiop->pDnad     = (UINT *) (baseAdrs + (OFF_DNAD));
    pSiop->pDsp      = (UINT *) (baseAdrs + (OFF_DSP));
    pSiop->pDsps     = (UINT *) (baseAdrs + (OFF_DSPS));
    pSiop->pScratch0 = baseAdrs + OFF_SCRATCH0;
    pSiop->pScratch1 = baseAdrs + OFF_SCRATCH1;
    pSiop->pScratch2 = baseAdrs + OFF_SCRATCH2;
    pSiop->pScratch3 = baseAdrs + OFF_SCRATCH3;
    pSiop->pDcntl    = baseAdrs + OFF_DCNTL;
    pSiop->pDwt      = baseAdrs + OFF_DWT;
    pSiop->pDien     = baseAdrs + OFF_DIEN;
    pSiop->pDmode    = baseAdrs + OFF_DMODE;
    pSiop->pAdder    = (UINT *) (baseAdrs + (OFF_ADDER));

    pSiop->scsiCtrl.clkPeriod = freqValue;
    pSiop->scsiCtrl.maxBytesPerXfer = SIOP_MAX_XFER_LENGTH;

    pSiop->scsiCtrl.scsiBusReset      = (VOIDFUNCPTR) ncr710ScsiBusReset; 

    pSiop->scsiCtrl.scsiTransact       = (FUNCPTR) ncr710ScsiTransact;
    pSiop->scsiCtrl.scsiSyncMsgConvert = (FUNCPTR) ncr710SyncMsgConvert;

    /* no longer needed with this kind of chip */
    pSiop->scsiCtrl.scsiDevSelect      = NULL;
    pSiop->scsiCtrl.scsiBytesIn        = NULL;
    pSiop->scsiCtrl.scsiBytesOut       = NULL;
    pSiop->scsiCtrl.scsiDmaBytesIn     = NULL;
    pSiop->scsiCtrl.scsiDmaBytesOut    = NULL;
    pSiop->scsiCtrl.scsiBusPhaseGet    = NULL;
    pSiop->scsiCtrl.scsiMsgInAck       = NULL;
    pSiop->scsiCtrl.scsiSetAtn         = NULL;

    /* Fill the siop structure with the default value for 
       some hardware dependant registers */
     bcopy ((char *)&hwRegs,(char *)&pSiop->hwRegs,sizeof (NCR710_HW_REGS));

    /* Just here to avoid a bus error in scsiAutoConfig 
       the chip can't be programming for an another value (0 or 250ms) */
    pSiop->scsiCtrl.scsiSelTimeOutCvt = (VOIDFUNCPTR)ncr710SelTimeOutCvt;	

    pSiop->pDevToSelect = (SCSI_PHYS_DEV *) NULL;

    /* create the mutex siop data semaphore */
    pSiop->pMutexData = semBCreate(SEM_Q_FIFO,SEM_FULL);

    if (pSiop->pMutexData == (SEM_ID)NULL)
	{
	(void) cacheDmaFree ((char *) pSiop);
	(void) cacheDmaFree ((char *) pNcrCtxt);
	return((SIOP *)NULL);
        }

 
#ifdef NCR710_INT_LOG_MEM
    pNcr710Deb = (UINT *)malloc(LOG_SIZE_MEM);
    pLogMemShow = pNcr710Deb;
#endif

    return (pSiop);
    }

/*******************************************************************************
*
* ncr710CtrlInit - initialize a control structure for an NCR 53C710 SIOP
*
* This routine initializes an SIOP structure, after the structure is created
* with ncr710CtrlCreate().  This structure must be initialized before the
* SIOP can be used.  It may be called more than once;  however,
* it should be called only while there is no activity on the SCSI interface.
*
* Before returning, this routine pulses RST (reset) on the SCSI bus, thus
* resetting all attached devices.
*
* The input parameters are as follows:
* .iP <pSiop> 4
* a pointer to the NCR_710_SCSI_CTRL structure created with
* ncr710CtrlCreate().
* .iP <scsiCtrlBusId>
* the SCSI bus ID of the SIOP, in the range 0 - 7.  The ID is somewhat 
* arbitrary; the value 7, or highest priority, is conventional.
* .iP <scsiPriority>
* the priority to which a task is set when performing a SCSI transaction.
* Valid priorities are 0 to 255.  Alternatively, the value -1 specifies that 
* the priority should not be altered during SCSI transactions. 
*
* RETURNS: OK, or ERROR if parameters are out of range.
*/

STATUS ncr710CtrlInit
    (
    FAST NCR_710_SCSI_CTRL *pSiop, /* ptr to SIOP struct */
    FAST int scsiCtrlBusId,	   /* SCSI bus ID of this SIOP */
    int scsiPriority		   /* priority of task when doing SCSI I/O */
    )

    {
    int ix;
    int jx;
    NCR_CTL *pNcrCtl;

    /* Check if pSiop not NULL */
    if (pSiop == (SIOP *)NULL)
	return (ERROR);

    pNcrCtl = &pSiop->pNcrCtxt->ncrCtl[0][0];

    pSiop->pNcrCtl = (NCR_CTL *) NULL;

    /* verify scsiCtrlBusId and enter legal value in SIOP structure */
    if (scsiCtrlBusId < SCSI_MIN_BUS_ID || scsiCtrlBusId > SCSI_MAX_BUS_ID)
	return (ERROR);
    
    pSiop->scsiCtrl.scsiCtrlBusId = (UINT8) scsiCtrlBusId;

    /* translate id 0-7 in power of 2 (1000000 = 7).It's for saving time
     * at interrupt level in a reconnection scheme. 
     */
    pSiop->ctrlIdPow2 = (UINT8) (0x1 << pSiop->scsiCtrl.scsiCtrlBusId);

    /* verify scsiPriority and enter legal value in SIOP structure */
    if (scsiPriority < NONE || scsiPriority > 0xff)
	return (ERROR);
    
    pSiop->scsiCtrl.scsiPriority = scsiPriority;

    /* disconnect is supported for now,but this feature is not included
     * in this revision.
     */
    pSiop->scsiCtrl.disconnect = FALSE; /* TRUE to enable disconnect */


    ncr710ScsiBusReset (pSiop);

    /* initialize the SIOP hardware */
    ncr710HwInit (pSiop);
  
    /* Fill fixed values in the SIOP nexus */
    for (ix = 0; ix<= SCSI_MAX_BUS_ID; ix++)
	{
	for (jx = 0; jx <= SCSI_MAX_LUN; jx++)
	    {
	    /* Init fixed count */
 	    pNcrCtl->msgInCount = 1;
	    pNcrCtl->extMsgInCount = 4;
	    pNcrCtl->statusCount = 1;
            pNcrCtl->pIdentMsg = CACHE_DMA_VIRT_TO_PHYS (
						 &pSiop->saveIdentMsg);
            pNcrCtl->identCount = 1;
            pNcrCtl++;
 	    }
	}

    return (OK);
    }

/*******************************************************************************
*
* ncr710ScsiBusReset - assert the RST line on the SCSI bus 
*
* Issue a SCSI Bus Reset command to the NCR 710.  This should put all devices
* on the SCSI bus in an initial quiescent state.
* This routine also clears the interrupt status by reading all status
* registers.  Because the chip has an interrupt stack, it is necessary to
* loop on the status registers to clear any pending interrupts.
*
* RETURNS: N/A
*
* NOMANUAL
*/

LOCAL void ncr710ScsiBusReset
    (
    FAST SIOP *pSiop	/* ptr to SIOP info */
   )

    {
    UINT8 tempReg;

    *pSiop->pScntl1 |= (UINT8) (B_RST);
    taskDelay (2);
    *pSiop->pScntl1 &= ~B_RST;
   
    /* Check for interrupt pending after scsi reset */
    tempReg = ((*pSiop->pIstat) & (B_DIP | B_SIP));

    while  (tempReg != 0)
        {
        /* Read scsi Status interrupt */
        tempReg = *pSiop->pSstat0; 
	taskDelay (2);
        tempReg = *pSiop->pDstat; 
	taskDelay (2);
	tempReg = ((*pSiop->pIstat) & (B_DIP | B_SIP));
        }
    }

/*******************************************************************************
*
* ncr710Intr - interrupt service routine for the SIOP
*
* This interrupt routine just performs a clear of interrupt 
* conditions and copies all of the interrupt status of the SIOP
* (status registers and script instruction interrupt values). 
* A check of the interrupt issued is done at the task level in
* ncr710ScsiPhase function except for a phase mismatch, a new command to 
* start, and a reconnect process.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void ncr710Intr 
    (
    SIOP *pSiop		/* ptr to SIOP info */
    )

    {
    UINT8 tmpReg;
    UINT8 phaseReqReg;
    TBOOL scriptIntr;        /* used to detect true end in single step Mode */
    UINT8 tmpLun;            /* used to extract lun from ident msgin */
    UINT8 tmp;
    SCSI_PHYS_DEV  *pScsiPhysDev;   /* Used when reconnected occur */
    UINT *localStart;               /* Used to restart Script */
    UINT intrRegs;	     /* local 32bit read of interrupt status */

    ncr710CountIntr++;

    /* Clear previous values */
    pSiop->saveSstat0 = 0;
    pSiop->saveDstat = 0;
    pSiop->saveScriptIntrStat=0;

    /* Update phase requested for this nexus */
    phaseReqReg = *pSiop->pScratch0;

    /* Read interrupt status registers with a long acces to avoid any 
     * potential timing problem. The ncr710 Data manuel specify that a 
     * 12BCLK period must be insert beetwen two 8bit read of SSTAT0
     * and DSTAT.  This limitation does not exist for a 32bit access.
     */
    while ((tmpReg = (*pSiop->pIstat & (B_SIP | B_DIP))) != 0)
        {
#if _BYTE_ORDER==_BIG_ENDIAN
        intrRegs = *((UINT *)pSiop->pSstat2);
        pSiop->saveSstat0 |= (UINT8)((intrRegs >> 8) & 0x0FF);
        pSiop->saveDstat  |= (UINT8)(intrRegs & 0x0FF);
#else
        intrRegs = *((UINT *)pSiop->pDstat);
        pSiop->saveSstat0 |= (UINT8)((intrRegs >> 16) & 0x0FF);
        pSiop->saveDstat  |= (UINT8)((intrRegs >> 24) & 0x0FF);
#endif
        }

    /* check for Single Step interrupt */
    if (pSiop->saveDstat & B_SSI)
        SCSI_INT_DEBUG_MSG("Single Step interrupt\n", 0, 0, 0, 0, 0, 0);
 
    if (pSiop->saveDstat & B_SIR)
        {
        scriptIntr = TRUE;
        }
    /* read the result value of int script instruction */
    pSiop->saveScriptIntrStat = *pSiop->pDsps;

    /* Process new command / disconnect / reconnected for exclusive 
     * access data part
     */
    switch (pSiop->saveScriptIntrStat)
	{
        
	case NEW_COMMAND_PROCESS:
            SCSI_INT_DEBUG_MSG("ncr710Intr:start new cmd \n", 0, 0, 0, 0, 0, 0);

            /* Initialize global nexus pointer */
            pSiop->pNcrCtl = pSiop->pNcrCtlCmd; 

#ifdef NCR710_INT_LOG_MEM
	    LOG_MEM_INTR;
#endif /* NCR710_INT_LOG_MEM */
            /* Flush all data involve in a transaction. */
            ncr710FlushCache(pSiop, pSiop->pNcrCtl);

	    ncr710StartScript (pSiop,pSiop->pNcrCtl,
			      (SCSI_PHYS_DEV *)pSiop->pNcrCtl->pScsiPhysDev,
                              (UINT *)pSiop->pNcrCtl->scriptAddr);
            return;

        case RECONNECT_PROCESS:
            SCSI_INT_DEBUG_MSG("ncr710Intr:reconnect\n", 0, 0, 0, 0, 0, 0);

	    if ((pSiop->saveIdentMsg & SCSI_MSG_IDENTIFY) == 0)
		{
                /* It's a bogus reconnect continue */
                logMsg ("ncr710Intr:bogus reselect msgIn = 0x%02x\n",
		        pSiop->saveIdentMsg, 0, 0, 0, 0, 0);

#ifdef NCR710_INT_LOG_MEM
	    LOG_MEM_INTR;
#endif /* NCR710_INT_LOG_MEM */

                ncr710StartScript(pSiop,pSiop->pNcrCtl,
				(SCSI_PHYS_DEV *)pSiop->pNcrCtl->pScsiPhysDev,
				&ackMsg1);
                return;
                }

            tmpLun = pSiop->saveIdentMsg & SCSI_MAX_LUN;

            /* Get scsi Id : we 've got the power of 2 both for
             *    own controller id and target id,xor first to get 
             *   the target id  10000100 (ctr = 7 ,target = 2). 
	     */
             
            tmp = (pSiop->ctrlIdPow2) ^ (*pSiop->pScratch3); 

            /* extract number from power of 2 picture for indexing */
            for (tmpReg=0;!(tmp & 0x1);tmpReg++)
		tmp = (tmp >> 1);

            /* Give devSyncSem for reconnected target by indexing ncrCtl 
	     * device array.
	     */
            pScsiPhysDev =(SCSI_PHYS_DEV *)pSiop->pNcrCtxt->
			  ncrCtl[tmpReg][tmpLun].pScsiPhysDev;

#ifdef NCR710_INT_LOG_MEM
            LOG_MEM_INTR;
#endif /* NCR710_INT_LOG_MEM */

	    /* restart scsi phase routine for this device */
            semGive (&pScsiPhysDev->devSyncSem);
            return;

        case RECONNECT_IN_SELECT:
#ifdef NCR710_INT_LOG_MEM
            LOG_MEM_INTR;
#endif /* NCR710_INT_LOG_MEM */

            SCSI_INT_DEBUG_MSG("ncr710Intr:reconnect in select\n",
			       0, 0, 0, 0, 0, 0);

	    ncr710StartScript(pSiop, pSiop->pNcrCtl,
			(SCSI_PHYS_DEV *)pSiop->pNcrCtl->pScsiPhysDev,
			&checkNewCmd); 
            return;

        case BAD_NEW_CMD:
            logMsg ("BAD_NEW_CMD\n", 0, 0, 0, 0, 0, 0);

#ifdef NCR710_INT_LOG_MEM
            LOG_MEM_INTR;
#endif /* NCR710_INT_LOG_MEM */

            ncr710StartScript(pSiop,pSiop->pNcrCtl, 
			     (SCSI_PHYS_DEV *)pSiop->pNcrCtl->pScsiPhysDev,
			     &waitSelect);
            return;

        default:
	    break;
	}

#ifdef NCR710_INT_LOG_MEM
        LOG_MEM_INTR;
#endif /* NCR710_INT_LOG_MEM */
    
    pScsiPhysDev = (SCSI_PHYS_DEV *)pSiop->pNcrCtl->pScsiPhysDev;

    /* handle the mismatch case at interrupt level because some count and
     * pointer have to be adjust with few chip register informations. 
     */
    if ((tmpReg = (pSiop->saveSstat0 & B_MA)) == B_MA)
        {

        localStart = ncr710PhaseMismatch(pSiop, 
				(SCSI_PHYS_DEV *)pSiop->pNcrCtl->pScsiPhysDev,
			        pSiop->pNcrCtl,(UINT)phaseReqReg);

        ncr710StartScript(pSiop,pSiop->pNcrCtl, 
			 (SCSI_PHYS_DEV *)pSiop->pNcrCtl->pScsiPhysDev,
			 localStart);

        SCSI_INT_DEBUG_MSG("mismatch intr\n", 0, 0, 0, 0, 0, 0);
        return;
        }

    /* give the synchronize semaphore to any startscript caller 
     * and check for single step feature.  If single step is enable
     * check to give semaphore only if it an INT script opcode 
     * occurred.
     */
    if (siopSStep != TRUE)
        {
        semGive (&pScsiPhysDev->devSyncSem);
	}
    else 
	{
	/* Give single step Semaphore */
	semGive (&pSiop->singleStepSem);

	if (scriptIntr == TRUE) 
	    {
	    /* Give only if it's an INT Script opcode executed */
	    logMsg ("ncr710Intr : INTR OPCODE DETECTED  \n", 0, 0, 0, 0, 0, 0);

            semGive (&pScsiPhysDev->devSyncSem);
	    }
        }
    }
/*******************************************************************************
*
* ncr710HwInit - initialize the SIOP chip to a known state
*
* This routine puts the SIOP into a known quiescent state.  Because the value
* of some registers are coupled with the hardware implemetation of the chip you 
* have to use ncr710SetHwRegister() to adjust the software/hardware couple.
* This routine does not touch the bits in register that are set by 
* the ncr710SetHwRegister() routine.
*
* SEE ALSO 
*	ncr710SetHwRegister()
*/

LOCAL void ncr710HwInit
    (
    SIOP *pSiop          /* ptr to an SIOP info structure */
    )

    {
    int scratch;

    /* Disable single step feature */
    siopSStep = FALSE;

    /* Perform a soft reset on Chip to get a known state */
    *pSiop->pIstat = B_SOFTRST;
    *pSiop->pIstat = 0;

    /* Init hardware dependant Registers */
    ncr710SetHwRegister (pSiop, &pSiop->hwRegs);

    /* set clock conversion factor, already checked in xxCtrlCreate */
    scratch = pSiop->scsiCtrl.clkPeriod;

    if (scratch >= NCR710_25MHZ) 
        *pSiop->pDcntl |= NCR710_16MHZ_DIV; /* 16.67 - 25Mhz range */

    if ((scratch >= NCR710_3750MHZ) && (scratch < NCR710_25MHZ)) 
        *pSiop->pDcntl |= NCR710_25MHZ_DIV; /* 25.01 - 37.50Mhz range */

    if ((scratch >= NCR710_50MHZ) && (scratch < NCR710_3750MHZ)) 
        *pSiop->pDcntl |= NCR710_50MHZ_DIV; /* 37.51 - 50Mhz range */

    if (scratch < NCR710_50MHZ)
        *pSiop->pDcntl |= NCR710_66MHZ_DIV; /* 50 - 66.66Mhz range */
    

    /* Manual start mode Program data bit hardware implementation dependant */
     *pSiop->pDmode |= B_MAN;


    /* Enable Manual script start and 710 mode */
    *pSiop->pDcntl |=  B_COM;


    /* check for slow cable mode  & Select /reselect */
    if (pSiop->slowCableMode == TRUE) 
	    *pSiop->pScntl1 |= B_EXC;

    if (pSiop->scsiCtrl.disconnect == TRUE) 
	{
	*pSiop->pScntl1 |= B_ESR;
        /* full arbitration */
        *pSiop->pScntl0 = (B_ARB1 | B_ARB0);
	}
    else 
	{
	/* Simple arbitration */
	*pSiop->pScntl0 &= ~(B_ARB1 | B_ARB0);
	}

    /* Parity from chip */
    *pSiop->pScntl0 |= B_EPG;

    /* Check for checking parity */
    if (pSiop->parityCheckEnbl == TRUE)
	*pSiop->pScntl0 |= B_EPC;
     
    /* Set own scsi ID Value check in CtrlInit */
    *pSiop->pScid = (1 << pSiop->scsiCtrl.scsiCtrlBusId);
    

    /* enable Scsi Interrupt:the select timeout, unexpected disconnect,
     * , phase mismatch interrupt, parity error.
     */
    *pSiop->pSien = (B_STO | B_UDC | B_MA | B_PAR); 

    /* enable Dma interrrupt: bus fault ,abort ,watchdog timeout, 
     *  illegal instruction.
     */
    *pSiop->pDien |= (B_BF | B_ABT | B_WTD | B_IID);

    /* StartScript at the Idle point */
    ncr710StartScript (pSiop, (NCR_CTL *) NULL,
		       (SCSI_PHYS_DEV *) NULL, &waitSelect);
    }

/*******************************************************************************
*
* ncr710SetHwRegister - set hardware-dependent registers for the NCR 53C710 SIOP
*
* This routine sets up the registers used in the hardware
* implementation of the chip.  Typically, this routine is called by 
* the sysScsiInit() routine from the board support package. 
* 
* The input parameters are as follows:
* .iP <pSiop> 4
* a pointer to the NCR_710_SCSI_CTRL structure created with
* ncr710CtrlCreate().
* .iP <pHwRegs>
* a pointer to a NCR710_HW_REGS structure that is filled with the logical 
* values 0 or 1 for each bit of each register described below.
*
* This routine includes only the bit registers that can be used to modify 
* the behavior of the chip. The default configuration used during 
* ncr710CtlrCreate() and ncr710CrtlInit() is {0,0,0,0,1,0,0,0,0,0,0,0,0,1,0}.
*
* .CS
*     typedef struct
*         {
*         int ctest4Bit7;    /@ host bus multiplex mode @/
*         int ctest7Bit7;    /@ disable/enable burst cache capability @/
*         int ctest7Bit6;    /@ snoop control bit1 @/
*         int ctest7Bit5;    /@ snoop control bit0 @/
*         int ctest7Bit1;    /@ invert tt1 pin (sync bus host mode only) @/
*         int ctest7Bit0;    /@ enable differential SCSI bus capability @/
*         int ctest8Bit0;    /@ set snoop pins mode @/
*         int dmodeBit7;     /@ burst length transfer bit 1 @/
*         int dmodeBit6;     /@ burst length transfer bit 0 @/
*         int dmodeBit5;     /@ function code bit FC2 @/
*         int dmodeBit4;     /@ function code bit FC1 @/
*         int dmodeBit3;     /@ program data bit (FC0) @/
*         int dmodeBit1;     /@ user-programmable transfer type @/
*         int dcntlBit5;     /@ enable ACK pin @/
*         int dcntlBit1;     /@ enable fast arbitration on host port @/
*         } NCR710_HW_REGS;
* .CE
* For a more detailed description of the register bits,
* see the 
* .I "NCR 53C710 SCSI I/O Processor Programming Guide."
*
* NOTE
* Because this routine writes to the NCR 53C710 chip registers, it cannot
* be used when there is any SCSI bus activity.
*
* RETURNS: OK, or ERROR if an input parameter is NULL.
*
* SEE ALSO: ncr710CtlrCreate(),
* .I "NCR 53C710 SCSI I/O Processor Programming Guide"
*/

STATUS ncr710SetHwRegister
    (
    SIOP	   *pSiop,	/* pointer to SIOP info */
    NCR710_HW_REGS *pHwRegs     /* pointer to NCR710_HW_REGS info */
   )

    {

    /* check input parameters */
    if ((pSiop == NULL) || (pHwRegs == NULL))
	return (ERROR);

    /* fill the SIOP structure with new parameters */
    bcopy ((char *)pHwRegs, (char *) &pSiop->hwRegs, sizeof(NCR710_HW_REGS));

    /* Set Each bit register to the new value */
    
    /* This bit must be set first else the chip will no anwser to a slave 
     * acces in some hardware implementation. 
     */
    ((pHwRegs->dcntlBit5) ? *pSiop->pDcntl = B_EA : 
				(*pSiop->pDcntl &= ~B_EA));
    ((pHwRegs->ctest4Bit7) ? *pSiop->pCtest4 |= B_MUX : 
				(*pSiop->pCtest4 &= ~B_MUX));
    ((pHwRegs->ctest7Bit7) ? *pSiop->pCtest7 |= B_CDIS : 
				(*pSiop->pCtest7 &= ~B_CDIS));
    ((pHwRegs->ctest7Bit6) ? *pSiop->pCtest7 |= B_SC1 : 
				(*pSiop->pCtest7 &= ~B_SC1));
    ((pHwRegs->ctest7Bit5) ? *pSiop->pCtest7 |= B_SC0: 
				(*pSiop->pCtest7 &= ~B_SC0));
    ((pHwRegs->ctest7Bit1) ? *pSiop->pCtest7 |= B_TT1 : 
				(*pSiop->pCtest7 &= ~B_TT1));
    ((pHwRegs->ctest7Bit0) ? *pSiop->pCtest7 |= B_DIFF : 
				(*pSiop->pCtest7 &= ~B_DIFF));
    ((pHwRegs->ctest8Bit0) ? *pSiop->pCtest8 |= B_SM : 
				(*pSiop->pCtest8 &= ~B_SM));
    ((pHwRegs->dmodeBit7) ? *pSiop->pDmode |= B_BL1 : 
				(*pSiop->pDmode &= ~B_BL1));
    ((pHwRegs->dmodeBit6) ? *pSiop->pDmode |= B_BL0 : 
				(*pSiop->pDmode &= ~B_BL0));
    ((pHwRegs->dmodeBit5) ? *pSiop->pDmode |= B_FC2 : 
				(*pSiop->pDmode &= ~B_FC2));
    ((pHwRegs->dmodeBit4) ? *pSiop->pDmode |= B_FC1 : 
				(*pSiop->pDmode &= ~B_FC1));
    ((pHwRegs->dmodeBit3) ? *pSiop->pDmode |= B_PD : 
				(*pSiop->pDmode &= ~B_PD));
    ((pHwRegs->dmodeBit1) ? *pSiop->pDmode |= B_U0TT0 : 
				(*pSiop->pDmode &= ~B_U0TT0));
    ((pHwRegs->dcntlBit1) ? *pSiop->pDcntl |= B_FA : 
				(*pSiop->pDcntl &= ~B_FA));
    return (OK);
    }


/*******************************************************************************
*
* ncr710ScsiPhase - process a scsi transaction with the script code
*
* This routine is called by ncr710ScsiTransact() to start the script 
* that performs the scsi phase transaction.  The ncrCtl shared data
* structure is filled with the apropriate parameters, pointer to the 
* command block,message out area,local message in area,scsi status 
* byte and data in/out buffer.
* The interrupt status is also processing in this routine.  This can 
* be done here because the mutex semaphore is taken in ncr710ScsiTransact.
*
* RETURNS: OK, or ERROR if not successful for any reason.
*
* NOMANUAL
*/

LOCAL STATUS ncr710ScsiPhase
    (
    SCSI_PHYS_DEV *pScsiPhysDev,	/* ptr to the target device */
    SCSI_TRANSACTION *pScsiXaction 	/* ptr to the transaction info */
    )

    {
    SIOP *pSiop;		/* Driver controller info for device */
    SCSI_CTRL *pScsiCtrl;       /* SCSI controller info for device */
    STATUS status;		/* routine return status */
    LOCAL UINT8 msgIn [MAX_MSG_IN_BYTES];  /* local msg in array */
    NCR_CTL *pNcrCtl;		/* local pNcrCtl nexus pointer */
    TBOOL transactEnd;		/* global flag for end of transaction */
    UINT localSwitch; 		/* local intr return switch status */
    UINT8 identMsg;		/* local to build identify byte message */
    UINT8* pMsgOut;		/* local msgout pointer */ 
    UINT *localStart = 0;	/* local current script restart address */	

    /* Debug info print */
    SCSI_DEBUG_MSG("ncr710Phase scsiAction=0x%x -------\n", (int) pScsiXaction,
		   0, 0, 0, 0, 0);
    SCSI_DEBUG_MSG("pScsiPhysDev = 0x%08x", (int) pScsiPhysDev, 0 ,0 ,0 ,0 ,0);

    /* controller info */
    pScsiCtrl = pScsiPhysDev->pScsiCtrl;

    pSiop = (SIOP *) (pScsiPhysDev->pScsiCtrl);

    /* save current scsi command context requested */
    pScsiPhysDev->pScsiXaction = pScsiXaction;

    /* init current pointer to scsi info structure */
    pSiop->pDevToSelect = pScsiPhysDev;

    /* init current siop context structure */
    pNcrCtl = &pSiop->pNcrCtxt->ncrCtl
	      [pScsiPhysDev->scsiDevBusId][pScsiPhysDev->scsiDevLUN];

    /* If a disconnect will occur, retain physDev info pointer */
    pNcrCtl->pScsiPhysDev = (UINT *)pScsiPhysDev;

    /* initialise pointer in a shared data area pointer by pNcrCtl */

    /* check and init target ID */
    if (pScsiCtrl->scsiCtrlBusId == (UINT8)pScsiPhysDev->scsiDevBusId)
       return(ERROR);

    /* Set target id in a data memory structure 00IDSY00 expects by the chip */
    pNcrCtl->device =  (INIT_BUS_ID) << (pScsiPhysDev->scsiDevBusId);
    
    /* Init Cmd count and pointer */
    pNcrCtl->cmdCount = pScsiXaction->cmdLength;
    pNcrCtl->pCmd = pScsiXaction->cmdAddress;

    /* Init data count and pointer */
    pNcrCtl->dataCount = pScsiXaction->dataLength;
    pNcrCtl->pData = pScsiXaction->dataAddress;
    pScsiPhysDev->savedTransLength  = pScsiXaction->dataLength;
    pScsiPhysDev->savedTransAddress = pScsiXaction->dataAddress;

    /* init message IN pointer */
    pNcrCtl->pMsgIn = msgIn;
    pNcrCtl->pExtMsgIn = (UINT8 *) &msgIn[1];

    /* init status address */
    pNcrCtl->pScsiStatus = &pScsiXaction->statusByte; 

    /* Check if some message out have to be built.The useMsgout tag will 
     * decide what 's kind of message has to be built. 
     */ 

    status = ncr710BuildMsgOut(pScsiPhysDev);
    
    if (status == ERROR)
	return (ERROR);

    /* Init message out count and pointer */
    pNcrCtl->msgOutCount = pScsiPhysDev->msgLength;
    pNcrCtl->pMsgOut = (UINT8 *) SCSI_GET_PTR_MSGOUT(pScsiPhysDev);

    /* choose the selWithAtn if useIdentify or/and useMsgout and build 
     * Identify msgout.
     */
    
    if ((pScsiPhysDev->useIdentify == TRUE) ||
	(pScsiPhysDev->useMsgout   != SCSI_NO_MSGOUT))

	pNcrCtl->scriptAddr = (UINT) &selWithAtn;
    else

	pNcrCtl->scriptAddr = (UINT) &selWithoutAtn;

    if (pScsiPhysDev->useIdentify == TRUE)
	{	
	/* build identify message */

        pNcrCtl->pMsgOut = (UINT8 *) SCSI_GET_PTR_IDENTIFY(pScsiPhysDev);

        identMsg = SCSI_MSG_IDENTIFY |
                   (pSiop->scsiCtrl.disconnect ?
                   SCSI_MSG_IDENT_DISCONNECT : 0) |
                   (UINT8) pScsiPhysDev->scsiDevLUN;

        *pNcrCtl->pMsgOut = identMsg;

        if (pScsiPhysDev->useMsgout == SCSI_NO_MSGOUT)
            pNcrCtl->msgOutCount = 1;
        else
            pNcrCtl->msgOutCount++;
        }
           
    /* Call the siopScriptPhase to perform a run on ncr chip */

    SCSI_DEBUG_MSG("ncr710ScsiPhase:FirstStartPhase \n", 0, 0, 0, 0, 0, 0);
    transactEnd = FALSE;
    
    /* Set device status in SELECT_REQUESTED state */
    pScsiPhysDev->devStatus = SELECT_REQUESTED;

    status = ncr710FirstStartPhase (pSiop, pNcrCtl, pScsiPhysDev,
				    ncr710TimeOut); 

    if (status == ERROR)
       return(status);

    while (transactEnd != TRUE) 
        {

        NCR710_DEBUG_MSG("siopIntrStatus=0x%08x", pSiop->saveScriptIntrStat);
        NCR710_DEBUG_MSG(" saveSstat0=0x%02x", pSiop->saveSstat0);
        NCR710_DEBUG_MSG(" saveDstat=0x%02x\n", pSiop->saveDstat);

        /* local pointer to msg out array */
        pMsgOut = SCSI_GET_PTR_MSGOUT(pScsiPhysDev);

        if  ((pSiop->saveDstat & B_SIR) != B_SIR) 
       	    {
            /* Check for status registers interrupt */  
	    status = ncr710CheckStatRegs(pSiop, pScsiPhysDev, 
					 (UINT *)&localStart);

	    if (status == OK )
		goto contTransaction;

            /* all of these conditions are fatal */
            SCSI_DEBUG_MSG("out from phase with error\n", 0, 0, 0, 0, 0, 0);
	    SCSI_DEBUG_MSG("Scsi Phase End = 0x%02x\n",*pSiop->pSbcl,
			   0, 0, 0, 0, 0);
            goto exitPhase;
	    }

        localSwitch = (pSiop->saveScriptIntrStat);

        /* No error what's kind of intr script */
        switch (localSwitch)
            {
            case BAD_MSGIN_BEFORE_CMD:
	    case BAD_MSG_AFTER_CMD:
            case BAD_MSG_AFTER_DATA:
            case BAD_MSG_INSTEAD_CMDCOMP:

		/* invalidate msgin array */
		CACHE_USER_INVALIDATE (pNcrCtl->pMsgIn, MAX_MSG_IN_BYTES);

                LOG_ERR("Not an expected msg received!! msgin = 0x%02\n", 
			msgIn[0]);
                LOG_ERR_CTXT;

		/* Check if it's an ident message */
                if (msgIn[0] == identMsg)
                    {
                    /* Accept and restart to the next phase requested */
                    if ((localSwitch == BAD_MSG_AFTER_DATA) ||
                        (localSwitch == BAD_MSG_INSTEAD_CMDCOMP))
			localStart = &ackMsg3;
                    else
			localStart = &ackMsg1;
                    } 
                 else
		    {
                    /* reject Msg */
                    LOG_ERR("Not an expected msg ..send reject msg!!\n");

                    pNcrCtl->pMsgOut = SCSI_GET_PTR_MSGOUT(pScsiPhysDev);
                    *pNcrCtl->pMsgOut = (UINT8)SCSI_MSG_MESSAGE_REJECT;
                    pNcrCtl->msgOutCount = 1;
                    localStart = (UINT *)&ackAtnMsg; 

                    /* flush msgout array */
		    CACHE_USER_FLUSH (pNcrCtl->pMsgOut,1);
                    }
                 break;

	    case MSGIN_EXPECT_AFTER_ST:
                /* not a msgIn phase after a status */
                LOG_ERR("msgIn expect after status!!\n");
                LOG_ERR_CTXT;
                localStart = &checkPhData;
                break;

            case MSGOUT_EXPECT:
                /* result of a select with atn and no msgout phase income,  
		 * goto the requested target phase.
		 */
	        LOG_ERR("Bad phase after select with atn!!\n");
                LOG_ERR_CTXT;
                localStart = &asortPh;
                break;

            case BAD_PH_AFTER_CMD:
	        LOG_ERR("Bad phase after a command phase!!\n");
                LOG_ERR_CTXT;
		/* it`s not data/status/msgin ?, restart to the requested 
		 * phase.
		 */
                localStart = &checkPhData;
                break;
                    
	    case EXTMSG_BEFORE_CMD:
            case EXTMSG_AFTER_CMD:
            case EXTMSG_AFTER_DATA:
                LOG_ERR("Extended msg received!!\n");
                  
                /* invalidate Msgin array */
		CACHE_USER_INVALIDATE (msgIn, sizeof(msgIn));

                localStart = ncr710ExtMsgRecv(pSiop, pScsiPhysDev, pNcrCtl, 
					      &msgIn[0]);

                /* Flush msgout array in case of reject msg */
		CACHE_USER_FLUSH (pNcrCtl->pMsgOut, MAX_MSG_OUT_BYTES);

                break;            

	    case REJECT_MSG1:
	        LOG_ERR("reject message received!!\n");
                LOG_ERR_CTXT;
		localStart = ncr710RejectRecv(pSiop, pScsiPhysDev, pNcrCtl);

		/* flush msgout array */
		CACHE_USER_FLUSH (pNcrCtl->pMsgOut, MAX_MSG_OUT_BYTES);
                break;

            case BAD_NEW_CMD:
                LOG_ERR("BAD NEW CMD!!\n");
                LOG_ERR_CTXT;
		status = ERROR;
 		goto exitPhase;

	    case GOOD_END:
                NCR710_DEBUG_MSG("ncr710ScsiPhase:good end\n");
		transactEnd = (TBOOL) TRUE;
                break;

            case ABORT_CLEAR_END:
                /* in this case return error ,because that means that you 
		 * sent a message reset device and the target goes directly 
		 * in bus free phase.
		 */

                LOG_ERR("ncr710ScsiPhase:abort end!!\n");
                LOG_ERR_CTXT;
		status = ERROR;
                goto exitPhase;

            case BAD_PH_BEFORE_CMD:
                LOG_ERR("bad phase before cmd!!\n");
                LOG_ERR_CTXT;
		status = ERROR;
                goto exitPhase;

            case PH_UNKNOWN:
                LOG_ERR("scsi phase unknown!!\n");
                LOG_ERR_CTXT;
		status = ERROR;
                goto exitPhase;

            case RES_IN_DETECTED:
            case RES_OUT_DETECTED:
		LOG_ERR("RES4 RES5 phase detected!!\n");
                LOG_ERR_CTXT;
		status = ERROR;
                goto exitPhase;

            case SAVDATP_BEFORE_CMD:
            case SAVDATP_AFTER_CMD:
		NCR710_DEBUG_MSG("save pointer before the data phase\n");
                localStart = &ackMsg1;
                break;
 
            case SAVDATP_AFTER_DATA:
		NCR710_DEBUG_MSG("save pointer after the data phase\n");

                /* Restart to accept message at a data phase point */
                localStart = &ackMsg3;
                break;
 
	    case DISC_BEFORE_CMD:
	    case DISC_AFTER_CMD:
	    case DISC_AFTER_DATA:
		NCR710_DEBUG_MSG("disconnect message\n");

                /* If it's a first disconnection ,clear commandRequest.
		 * The normal sequence is
                 * IDLE->SELECT_REQUESTED->DISCONNECTED->RECONNECTED..->IDLE.
		 */

                semTake (pSiop->pMutexData,WAIT_FOREVER);

                if (pScsiPhysDev->devStatus == SELECT_REQUESTED)
                    {
                    pSiop->commandRequest = FALSE;
                    }

                if (pSiop->commandRequest == TRUE)
			*pSiop->pIstat |= B_SIGP;

                semGive(pSiop->pMutexData);

		/* Save reconnect re-start script entry point */
                if (pSiop->saveScriptIntrStat == DISC_AFTER_DATA)
                    localStart = &ackMsg3;
                else
                    localStart = &ackMsg1;
 
                if (pScsiPhysDev->devStatus == SELECT_REQUESTED)
		    {
		    semGive (&pScsiCtrl->ctrlMutexSem);
		    }
                    
                /* Update devStatus */
                pScsiPhysDev->devStatus = DISCONNECTED;

                /* Restart script to the idle point */
                ncr710StartScript(pSiop, pSiop->pNcrCtl, pScsiPhysDev,
				  &waitSelect);
                /* Suspend disconnected device */
		semTake(&pScsiPhysDev->devSyncSem, WAIT_FOREVER);

                /* The reconnect will occur and restart the current SCSI 
		 * transaction at this point for this device 
		 */

		/* Restore nexus context */
                pSiop->pNcrCtl = pNcrCtl; 

		/* Update dev status */
                pScsiPhysDev->devStatus = RECONNECTED;

                break;

            case RESTORE_POINTER:
		NCR710_DEBUG_MSG("restore pointer received\n");

                pNcrCtl->pData = pScsiPhysDev->savedTransAddress;
                pNcrCtl->dataCount = pScsiPhysDev->savedTransLength;

                localStart = &ackMsg1;
                break;

            case SELECT_AS_TARGET:
                LOG_ERR("selected as a target!!\n");
                LOG_ERR_CTXT;
		status = ERROR;
		goto exitPhase;

	    default: 
                LOG_ERR("unknown intr script value!!\n");
                LOG_ERR("INT STATUS UNKNOWN:0x%08x pScsiPhysDev=0x%08x\n",
			pSiop->saveScriptIntrStat, pScsiPhysDev);
                LOG_ERR_CTXT;
		status = ERROR;
		transactEnd = (TBOOL) TRUE;
                break;
            }
contTransaction:

	/* transaction not yet finish restart script to next phase */

	if (transactEnd == FALSE)
	    status = ncr710StartPhase(pSiop, pNcrCtl, localStart,ncr710TimeOut);
        }
exitPhase:

    if (status == ERROR)
        NCR710_DEBUG_MSG("exit ERROR from Phase\n");
    else
	NCR710_DEBUG_MSG("exit OK from Phase\n");

    /* restore pending command context in case of reselect during
       a select of a current starting */
    semTake (pSiop->pMutexData,WAIT_FOREVER);

    if ((pScsiPhysDev->devStatus == SELECT_REQUESTED) ||
        (pScsiPhysDev->devStatus == SELECT_TIMEOUT))
	pSiop->commandRequest = FALSE;

    if (pSiop->commandRequest == TRUE)
        *pSiop->pIstat |= B_SIGP;

    semGive(pSiop->pMutexData);

    /* Give semaphore only if it's not after a disconnect/reconnect */
    if ((pScsiPhysDev->devStatus == SELECT_REQUESTED) ||
        (pScsiPhysDev->devStatus == SELECT_TIMEOUT))
        semGive (&pScsiCtrl->ctrlMutexSem);

    if (pNcrCtl->dataCount < 0x800)
        /* flush data */
        CACHE_USER_FLUSH (pNcrCtl->pData, pNcrCtl->dataCount);
    else
	CACHE_USER_FLUSH (0, ENTIRE_CACHE);

    /* invalidate scsi status */
    CACHE_USER_FLUSH (pNcrCtl->pScsiStatus, pNcrCtl->statusCount);

    /* Restart Script at the Idle point */
    ncr710StartScript (pSiop, pSiop->pNcrCtl, pScsiPhysDev, &waitSelect);

    return (status);
    }

/*******************************************************************************
*
* ncr710ScsiTransact - obtain exclusive use of SCSI controller for a transaction
*
* This routine calls ncr710ScsiPhase() to execute the specified
* command.  If there are physical path management errors, then
* this routine returns ERROR.  If not, then the status returned
* from the command is checked.  If it is "Check Condition", then
* a "Request Sense" CCS command is executed and the sense key is
* examined.  An indication of the success of the command is
* returned (OK or ERROR).  This routine is almost the same as the
* scsiLib.  If some modification occurs in the scsiLib.c, the same
* change should be done here. 
*
* RETURNS: OK if successful, or ERROR if a path management error occurs
* or the status or sense information indicates an error.
*
* NOMANUAL
*/

LOCAL STATUS ncr710ScsiTransact
    (
    SCSI_PHYS_DEV *pScsiPhysDev,        /* ptr to the target device */
    SCSI_TRANSACTION *pScsiXaction      /* ptr to the transaction info */
    )

    {
    SCSI_CTRL *pScsiCtrl;	/* SCSI controller info for device */
    SCSI_BLK_DEV_NODE *pBlkDevNode; /* ptr for looping through BLK_DEV list */
    STATUS status;		/* routine return status */
    int senseKey;		/* extended sense key from target */
    int addSenseCode;		/* additional sense code from target */
    int taskId = taskIdSelf ();	/* calling task's ID */
    int taskPriority;		/* calling task's current priority */

    pScsiCtrl = pScsiPhysDev->pScsiCtrl;

    /* set task priority to SCSI controller's (if != NONE) */

    if (pScsiCtrl->scsiPriority != NONE)
	{
	taskPriorityGet (taskId, &taskPriority);
	taskPrioritySet (taskId, pScsiCtrl->scsiPriority);
	}

    /* take the device and controller semaphores */

    semTake (&pScsiPhysDev->devMutexSem, WAIT_FOREVER);

    if ((status = ncr710ScsiPhase(pScsiPhysDev, pScsiXaction)) == ERROR)
	{
	SCSI_DEBUG_MSG("ncr710ScsiTransact: scsiPhaseSequence ERROR.\n",
			0, 0, 0, 0, 0, 0);
	goto cleanExit;
	}

    /* check device status and take appropriate action */

    SCSI_DEBUG_MSG("ncr710ScsiTransact: Check Status value = 0x%x.\n",
                   pScsiXaction->statusByte, 0, 0, 0, 0, 0);

    switch(pScsiXaction->statusByte & SCSI_STATUS_MASK)
	{
	case SCSI_STATUS_GOOD:
	    status = OK;
	    pScsiPhysDev->lastSenseKey = SCSI_SENSE_KEY_NO_SENSE;
	    pScsiPhysDev->lastAddSenseCode = (UINT8) 0;
	    goto cleanExit;

	case SCSI_STATUS_CHECK_CONDITION:
	    {
	    SCSI_COMMAND reqSenseCmd;		/* REQUEST SENSE command */
	    SCSI_TRANSACTION reqSenseXaction;	/* REQUEST SENSE xaction */

                                                /* REQUEST SENSE buffer  */
            UINT8 reqSenseData [REQ_SENSE_ADD_LENGTH_BYTE + 1];

            /* build a REQUEST SENSE command and transact it */

            (void) scsiCmdBuild (reqSenseCmd, &reqSenseXaction.cmdLength,
                                 SCSI_OPCODE_REQUEST_SENSE,
                                 pScsiPhysDev->scsiDevLUN, FALSE, 0,
                                 pScsiPhysDev->reqSenseDataLength, (UINT8) 0);
 
            reqSenseXaction.cmdAddress    = (UINT8 *) reqSenseCmd;
 
            /* if there is no user request sense buffer ,supply it */
 
            if (pScsiPhysDev->pReqSenseData == (UINT8 *)NULL)
                {
                reqSenseXaction.dataAddress  = reqSenseData;
 
                if (!pScsiPhysDev->extendedSense)
                    reqSenseXaction.dataLength    = 4;
                else
                    reqSenseXaction.dataLength = REQ_SENSE_ADD_LENGTH_BYTE + 1;
                } 
            else 
                {
                reqSenseXaction.dataAddress   = pScsiPhysDev->pReqSenseData;
                reqSenseXaction.dataLength    =
                                           pScsiPhysDev->reqSenseDataLength;
                }
 
            reqSenseXaction.dataDirection = O_RDONLY;
            reqSenseXaction.addLengthByte = REQ_SENSE_ADD_LENGTH_BYTE;

	    SCSI_DEBUG_MSG("ncr710ScsiTransact: issuing a REQUEST SENSE command.\n",
			    0, 0, 0, 0, 0, 0);

	    if ((status = ncr710ScsiPhase (pScsiPhysDev, &reqSenseXaction))
		== ERROR)
		{
		SCSI_DEBUG_MSG("ncr710ScsiTransact: scsiPhaseSequence ERROR.\n",
				0, 0, 0, 0, 0, 0);
		goto cleanExit;
		}

	    /* REQUEST SENSE command status != GOOD indicates fatal error */

	    if (reqSenseXaction.statusByte != SCSI_STATUS_GOOD)
		{
		SCSI_DEBUG_MSG("scsiTransact: non-zero REQ SENSE status.\n",
				0, 0, 0, 0, 0, 0);
		errnoSet (S_scsiLib_REQ_SENSE_ERROR);
		status = ERROR;
		goto cleanExit;
		}

	    /* if device uses Nonextended Sense Data Format, return now */

	    if (!pScsiPhysDev->extendedSense)
		{
		status = ERROR;
		goto cleanExit;
		}

	    /* check sense key and take appropriate action */

	    pScsiPhysDev->lastSenseKey =
		(pScsiPhysDev->pReqSenseData)[2] & SCSI_SENSE_KEY_MASK;

	    pScsiPhysDev->lastAddSenseCode = (pScsiPhysDev->pReqSenseData)[12];
	    addSenseCode = (int) pScsiPhysDev->lastAddSenseCode;

	    switch (senseKey = (int) pScsiPhysDev->lastSenseKey)
	        {
	        case SCSI_SENSE_KEY_NO_SENSE:
	            {
	            SCSI_DEBUG_MSG("ncr710ScsiTransact: No Sense\n",
				    0, 0, 0, 0, 0, 0);
	            status = OK;
	            goto cleanExit;
	            }
	        case SCSI_SENSE_KEY_RECOVERED_ERROR:
	            {
		    SCSI_DEBUG_MSG("ncr710ScsiTransact: Recovered Error Sense,",
				    0, 0, 0, 0, 0, 0);
		    SCSI_DEBUG_MSG("Additional Sense Code = 0x%02x\n",
				    addSenseCode, 0, 0, 0, 0, 0);
	            status = OK;
	            goto cleanExit;
	            }
	        case SCSI_SENSE_KEY_NOT_READY:
	            {
		    SCSI_DEBUG_MSG("ncr710ScsiTransact: Not Ready Sense,",
				    0, 0, 0, 0, 0, 0);
		    SCSI_DEBUG_MSG("Additional Sense Code = 0x%02x\n",
				    addSenseCode, 0, 0, 0, 0, 0);
	            errnoSet (S_scsiLib_DEV_NOT_READY);
	            status = ERROR;
	            goto cleanExit;
	            }
	        case SCSI_SENSE_KEY_UNIT_ATTENTION:
	            {
		    SCSI_DEBUG_MSG("ncr710ScsiTransact: Unit Attention Sense,",
				    0, 0, 0, 0, 0, 0);
		    SCSI_DEBUG_MSG("Additional Sense Code = 0x%02x\n",
				    addSenseCode, 0, 0, 0, 0, 0);
		    if (addSenseCode == 0x28)
			{
    			semTake (&pScsiPhysDev->blkDevList.listMutexSem, 
				 WAIT_FOREVER);

    			for (pBlkDevNode = (SCSI_BLK_DEV_NODE *)
			         lstFirst
				     (&pScsiPhysDev->blkDevList.blkDevNodes);
         		     pBlkDevNode != NULL;
         		     pBlkDevNode = (SCSI_BLK_DEV_NODE *)
				 lstNext (&pBlkDevNode->blkDevNode))
		 	    {
			    pBlkDevNode->scsiBlkDev.blkDev.bd_readyChanged =
			        TRUE;
       		  	    }

    			semGive (&pScsiPhysDev->blkDevList.listMutexSem);
			}
		    else if (addSenseCode == 0x29)
			{
			pScsiPhysDev->resetFlag = TRUE;
			}

		    /* issue a MODE SENSE command */

    		    {
    		    UINT8 modeSenseHeader [4];    /* mode sense data header */
    		    SCSI_COMMAND modeSenseCommand;/* SCSI command byte array */
    		    SCSI_TRANSACTION scsiXaction; /* info on a SCSI xaction */

    		    if (scsiCmdBuild (modeSenseCommand, &scsiXaction.cmdLength,
				      SCSI_OPCODE_MODE_SENSE,
				      pScsiPhysDev->scsiDevLUN, FALSE,
				      0, sizeof (modeSenseHeader), (UINT8) 0)
			== ERROR)
			goto cleanExit;

    		    scsiXaction.cmdAddress    = modeSenseCommand;
    		    scsiXaction.dataAddress   = modeSenseHeader;
    		    scsiXaction.dataDirection = O_RDONLY;
    		    scsiXaction.dataLength    = sizeof (modeSenseHeader);
    		    scsiXaction.addLengthByte = MODE_SENSE_ADD_LENGTH_BYTE;

	    	    SCSI_DEBUG_MSG("ncr710ScsiTransact: issuing a MODE SENSE cmd.\n",
				    0, 0, 0, 0, 0, 0);

	    	    if ((status = ncr710ScsiPhase (pScsiPhysDev,
					          &scsiXaction)) == ERROR)
			{
			SCSI_DEBUG_MSG("ncr710ScsiTransact returned ERROR\n",
					0, 0, 0, 0, 0, 0);
			goto cleanExit;
			}


	    	    /* MODE SENSE command status != GOOD indicates
		     * fatal error
		     */

	    	    if (scsiXaction.statusByte != SCSI_STATUS_GOOD)
			{
			SCSI_DEBUG_MSG("ncr710ScsiTransact: bad MODE SELECT stat.\n",
					0, 0, 0, 0, 0, 0);
			status = ERROR;
			goto cleanExit;
			}
	    	    else
			{
    			semTake (&pScsiPhysDev->blkDevList.listMutexSem, 
				 WAIT_FOREVER);

    			for (pBlkDevNode = (SCSI_BLK_DEV_NODE *)
			         lstFirst
				     (&pScsiPhysDev->blkDevList.blkDevNodes);
         		     pBlkDevNode != NULL;
         		     pBlkDevNode = (SCSI_BLK_DEV_NODE *)
				 lstNext (&pBlkDevNode->blkDevNode))
		 	    {
			    pBlkDevNode->scsiBlkDev.blkDev.bd_mode =
		    	        (modeSenseHeader [2] & (UINT8) 0x80) ? O_RDONLY
								     : O_RDWR;
       		  	    }

    			semGive (&pScsiPhysDev->blkDevList.listMutexSem);

			SCSI_DEBUG_MSG("Write-protect bit = %x.\n",
			    	        (modeSenseHeader [2] & (UINT8) 0x80),
					0, 0, 0, 0, 0);
			}
    		    }
		    errnoSet (S_scsiLib_UNIT_ATTENTION);
	            status = ERROR;
	            goto cleanExit;
	            }
	        case SCSI_SENSE_KEY_DATA_PROTECT:
	            {
		    SCSI_DEBUG_MSG("ncr710ScsiTransact: Data Protect Sense,",
				    0, 0, 0, 0, 0, 0);
		    SCSI_DEBUG_MSG("Additional Sense Code = 0x%02x\n",
				    addSenseCode, 0, 0, 0, 0, 0);
		    if (addSenseCode == 0x27)
			errnoSet (S_scsiLib_WRITE_PROTECTED);
	            status = ERROR;
	            goto cleanExit;
	            }
	        default:
		    {
		    SCSI_DEBUG_MSG("ncr710ScsiTransact: Sense = %x,",
				    senseKey, 0, 0, 0, 0, 0);
		    SCSI_DEBUG_MSG("Additional Sense Code = 0x%02x\n",
				    addSenseCode, 0, 0, 0, 0, 0);
	            status = ERROR;
	            goto cleanExit;
	            }
	        }
	    }

	case SCSI_STATUS_BUSY:
	    errnoSet (S_scsiLib_DEV_NOT_READY);
	    SCSI_DEBUG_MSG("device returned busy status.\n", 0, 0, 0, 0, 0, 0);
	    status = ERROR;
	    break;

	default:
	    status = ERROR;
	}

cleanExit:

    /* give back controller and device mutual exclusion semaphores */

    SCSI_DEBUG_MSG("ncr710ScsiTransact Give devMutexSem\n", 0, 0, 0, 0, 0 ,0);
    semGive (&pScsiPhysDev->devMutexSem);

    /* restore task's normal priority, if previously altered */

    if (pScsiCtrl->scsiPriority != NONE)
        taskPrioritySet (taskId, taskPriority);

    return (status);
    }

/*******************************************************************************
*
* ncr710Show - display the values of all readable NCR 53C710 SIOP registers
*
* This routine displays the state of the NCR 53C710 SIOP registers in a 
* user-friendly manner.  It is useful primarily for debugging.  
* The input parameter is the pointer to the SIOP information structure 
* returned by the ncr710CtrlCreate() call.
*
* NOTE
* The only readable register during a script execution is the Istat register. 
* If this routine is used during the execution of a SCSI command, the result 
* could be unpredictable.
*
* EXAMPLE:
* .CS
*     -> ncr710Show
*     NCR710 Registers 
*     ---------------- 
*     0xfff47000: Sien    = 0xa5 Sdid    = 0x00 Scntl1  = 0x00 Scntl0  = 0x04
*     0xfff47004: Socl    = 0x00 Sodl    = 0x00 Sxfer   = 0x80 Scid    = 0x80
*     0xfff47008: Sbcl    = 0x00 Sbdl    = 0x00 Sidl    = 0x00 Sfbr    = 0x00
*     0xfff4700c: Sstat2  = 0x00 Sstat1  = 0x00 Sstat0  = 0x00 Dstat   = 0x80
*     0xfff47010: Dsa     = 0x00000000
*     0xfff47014: Ctest3  = ???? Ctest2  = 0x21 Ctest1  = 0xf0 Ctest0  = 0x00
*     0xfff47018: Ctest7  = 0x32 Ctest6  = ???? Ctest5  = 0x00 Ctest4  = 0x00
*     0xfff4701c: Temp    = 0x00000000
*     0xfff47020: Lcrc    = 0x00 Ctest8  = 0x00 Istat   = 0x00 Dfifo   = 0x00
*     0xfff47024: Dcmd/Ddc= 0x50000000
*     0xfff47028: Dnad    = 0x00066144
*     0xfff4702c: Dsp     = 0x00066144
*     0xfff47030: Dsps    = 0x00066174
*     0xfff47037: Scratch3= 0x00 Scratch2= 0x00 Scratch1= 0x00 Scratch0= 0x0a
*     0xfff47038: Dcntl   = 0x21 Dwt     = 0x00 Dien    = 0x37 Dmode   = 0x01
*     0xfff4703c: Adder   = 0x000cc2b8
* .CE
*
* RETURNS: OK, or ERROR if <pScsiCtrl> and <pSysScsiCtrl> are both NULL.
*
* SEE ALSO: ncr710CtrlCreate()
*/

STATUS ncr710Show 
    (
    FAST SCSI_CTRL *pScsiCtrl 	/* ptr to SCSI controller info */
    )
    {
    FAST SIOP *pSiop;		/* prt to SIOP info */

    if (pScsiCtrl == NULL)
	{
	if (pSysScsiCtrl == NULL)
	    {
	    printErr ("No SCSI controller specified.\n");
	    return (ERROR);
	    }
	pScsiCtrl = pSysScsiCtrl;
	}

    pSiop = (SIOP *) pScsiCtrl; 

    printf ("NCR710 Registers \n");
    printf ("---------------- \n");
    printf ("0x%08x: ", (int) pSiop->pSien);
    printf ("Sien    = 0x%02x ", *pSiop->pSien); 
    printf ("Sdid    = 0x%02x ", *pSiop->pSdid); 
    printf ("Scntl1  = 0x%02x ", *pSiop->pScntl1); 
    printf ("Scntl0  = 0x%02x\n", *pSiop->pScntl0); 
    printf ("0x%08x: ", (int) pSiop->pSocl);
    printf ("Socl    = 0x%02x ", *pSiop->pSocl); 
    printf ("Sodl    = 0x%02x ", *pSiop->pSodl); 
    printf ("Sxfer   = 0x%02x ", *pSiop->pSxfer); 
    printf ("Scid    = 0x%02x\n", *pSiop->pScid); 
    printf ("0x%08x: ", (int) pSiop->pSbcl);
    printf ("Sbcl    = 0x%02x ", *pSiop->pSbcl); 
    printf ("Sbdl    = 0x%02x ", *pSiop->pSbdl); 
    printf ("Sidl    = 0x%02x ", *pSiop->pSidl); 
    printf ("Sfbr    = 0x%02x\n", *pSiop->pSfbr); 
    printf ("0x%08x: ", (int) pSiop->pSstat2);
    printf ("Sstat2  = 0x%02x ", *pSiop->pSstat2); 
    printf ("Sstat1  = 0x%02x ", *pSiop->pSstat1); 
    printf ("Sstat0  = 0x%02x ", *pSiop->pSstat0); 
    printf ("Dstat   = 0x%02x\n", *pSiop->pDstat); 
    printf ("0x%08x: ", (int) pSiop->pDsa);
    printf ("Dsa     = 0x%08x\n", *pSiop->pDsa); 
    /* read fifo is not a good idea */
    printf ("0x%08x: ", (int) pSiop->pCtest3);
    printf ("Ctest3  = ???? "); 
    printf ("Ctest2  = 0x%02x ", *pSiop->pCtest2); 
    printf ("Ctest1  = 0x%02x ", *pSiop->pCtest1); 
    printf ("Ctest0  = 0x%02x\n", *pSiop->pCtest0); 
    printf ("0x%08x: ", (int) pSiop->pCtest7);
    printf ("Ctest7  = 0x%02x ", *pSiop->pCtest7); 
    /* read fifo is not a good idea */
    printf ("Ctest6  = ???? "); 
    printf ("Ctest5  = 0x%02x ", *pSiop->pCtest5); 
    printf ("Ctest4  = 0x%02x\n", *pSiop->pCtest4); 
    printf ("0x%08x: ", (int) pSiop->pTemp);
    printf ("Temp    = 0x%08x\n", *pSiop->pTemp); 
    printf ("0x%08x: ", (int) pSiop->pLcrc);
    printf ("Lcrc    = 0x%02x ", *pSiop->pLcrc); 
    printf ("Ctest8  = 0x%02x ", *pSiop->pCtest8); 
    printf ("Istat   = 0x%02x ", *pSiop->pIstat); 
    printf ("Dfifo   = 0x%02x\n", *pSiop->pDfifo); 
    printf ("0x%08x: ", (int) pSiop->pDbc);
    printf ("Dcmd/Ddc= 0x%08x\n", *pSiop->pDbc); 
    printf ("0x%08x: ", (int) pSiop->pDnad);
    printf ("Dnad    = 0x%08x\n", *pSiop->pDnad); 
    printf ("0x%08x: ", (int) pSiop->pDsp);
    printf ("Dsp     = 0x%08x\n", *pSiop->pDsp); 
    printf ("0x%08x: ", (int) pSiop->pDsps);
    printf ("Dsps    = 0x%08x\n", *pSiop->pDsps); 
    printf ("0x%08x: ", (int) pSiop->pScratch0);
    printf ("Scratch3= 0x%02x ", *pSiop->pScratch3); 
    printf ("Scratch2= 0x%02x ", *pSiop->pScratch2); 
    printf ("Scratch1= 0x%02x ", *pSiop->pScratch1); 
    printf ("Scratch0= 0x%02x\n", *pSiop->pScratch0); 
    printf ("0x%08x: ", (int) pSiop->pDcntl);
    printf ("Dcntl   = 0x%02x ", *pSiop->pDcntl); 
    printf ("Dwt     = 0x%02x ", *pSiop->pDwt); 
    printf ("Dien    = 0x%02x ", *pSiop->pDien); 
    printf ("Dmode   = 0x%02x\n", *pSiop->pDmode); 
    printf ("0x%08x: ", (int) pSiop->pAdder);
    printf ("Adder   = 0x%08x\n", *pSiop->pAdder); 

    return (OK);
    }

/*******************************************************************************
*
* ncr710StartScript - start a script code 
*
* Routine to start a script with the NCR chip.  This routine takes
* 4 parameters, a SIOP pointer, the nexus pointer, the pScsiPhysdev pointer
* and the script code entry point.
* <pScsiPhysDev> is used to set up the synchronous parameters if the target
* supports this mode and the new ID descriptor is flushed.
* The Dsa register is load with the address of NCRTL (<pNcrCtl>) nexus data,
* and the register Dsp with the address of the script routine to start.
* Then the start bit is set in the Dcntl register.  No mutex semaphores
* are handled in this code.  This routine is call from ncr710Intr.
*
* RETURNS: N/A
* 
* NOMANUAL
*/

LOCAL void ncr710StartScript
    (
    SIOP *pSiop,			/* pointer to  SIOP info */
    NCR_CTL *pNcrCtl,			/* ncr nexus info */
    SCSI_PHYS_DEV *pScsiPhysDev,	/* scsi phys dev info */
    UINT *scriptEntry 			/* routine address entry point */
    )

    {
    /* Check for Sync transfer init 00/ID/Xferp & offset/00 */
    if (pNcrCtl != (NCR_CTL *) NULL && pScsiPhysDev->syncXfer == TRUE)
        {
        /* init sbcl clock division */
        *pSiop->pSbcl = (UINT8)pScsiPhysDev->syncReserved2;
 
        /* init device descriptor with sxfer reg value */
        pNcrCtl->device |= pScsiPhysDev->syncReserved1;

        /* For a reconnect set directly register */
        *pSiop->pSxfer = (UINT8)(pScsiPhysDev->syncReserved1 / 256); 
        *pSiop->pSxfer |= B_DHP;
        }
     else
	{
        /* Set assync mode */
        *pSiop->pSbcl = 0;
        *pSiop->pSxfer = 0;
        *pSiop->pSxfer |= B_DHP;
        }

    /* Clear manual start bit */
    *pSiop->pDcntl &= ~B_STD; 

    /* load data address */
    if (pNcrCtl == (NCR_CTL *) NULL)
	*pSiop->pDsa = (UINT) NULL;
    else
	*pSiop->pDsa = (UINT) CACHE_DMA_VIRT_TO_PHYS (pNcrCtl);


    /* Enable script interrupt */
    *pSiop->pDien |= B_SIR;

    /* load start address */
    *pSiop->pDsp = (UINT) scriptEntry;

    /* startScript */
    *pSiop->pDcntl |= B_STD; 
    }

/*******************************************************************************
*
* ncr710StartPhase - start the script code phase 
*
* This routine is use to restart a script to during a SCSI transaction after
* any script interrupt condition.
* The devSyncSem semaphore is taken and will be given by the interrupt routine.
* The return value is give by the script INT <value> instruction.  If a 
* semaphore timeout occurs during the transaction error status is returned.
* This timeout must be expressed in ticks.  
*  
* RETURNS: Interrupt Status or ERROR
* 
* NOMANUAL
*/

LOCAL STATUS ncr710StartPhase
    (
    SIOP *pSiop,			/* pointer to SIOP info */
    NCR_CTL *pNcrCtl,
    UINT *scriptAddress,		/* script entry  point */
    int timeOut 			/* Timeout on semTake */
    )

    {
    FAST SCSI_PHYS_DEV *pScsiPhysDev = (SCSI_PHYS_DEV *)(pNcrCtl->pScsiPhysDev);

    NCR710_DEBUG_MSG("StartPhase pNcrCtl=0x%08x\n",pNcrCtl);
    NCR710_DEBUG_MSG("----- pScsiPhysDev=0x%08x\n",pScsiPhysDev);

    ncr710StartScript (pSiop, pNcrCtl, pScsiPhysDev, scriptAddress);

    if ((semTake (&pScsiPhysDev->devSyncSem,timeOut) == ERROR))
	{
	LOG_ERR("ncr710StartPhase : SemTake timeout\n");
	return (ERROR);
	}
    return (pSiop->saveScriptIntrStat);
    }

/*******************************************************************************
*
* ncr710FirstStartPhase - start the script code phase 
*
* This routine starts the main script to handle a SCSI transaction.
* The controller semaphore <ctrlMutexSem> is taken and will be given 
* by a disconnect or any command termination.  The devSyncSem semaphore is 
* given at interrupt level.
* The pMutexData semaphore is used to set up the global information needed 
* to start the new command at interrupt level.
* It is used to start the first select for a SCSI transaction.  The SIGP is set
* to start the current command by aborting the <wait SELECT..> script 
* instruction at interrupt level. 
* The return value is given by the script INT <value> instruction.
* If a semaphore timeout occurs during the transaction an ERROR status is
* returned.This timeout must be expressed in tick. 
* 
* RETURNS: Script Interrupt Status or ERROR
*
* NOMANUAL
*/
 
LOCAL STATUS ncr710FirstStartPhase 
    (
    SIOP *pSiop,                        /* pointer to SIOP info */
    NCR_CTL *pNcrCtl,			/* pointer to siop nexus */
    SCSI_PHYS_DEV *pScsiPhysDev,	/* pointer to device info */
    int timeOut 			/* Timeout on semTake */
    )

    {
    SCSI_CTRL *pScsiCtrl = (SCSI_CTRL *)pSiop;
    
    /* Get exclusive Acces */
    semTake (&pScsiCtrl->ctrlMutexSem, WAIT_FOREVER);
    semTake (pSiop->pMutexData,WAIT_FOREVER);

    pSiop->pNcrCtlCmd = pNcrCtl;

    /* Set sigp bit to start the command at interrupt level */

    pSiop->commandRequest = TRUE;

    *pSiop->pIstat |= B_SIGP;

    semGive(pSiop->pMutexData);

    if ((semTake (&pScsiPhysDev->devSyncSem,timeOut) == ERROR))
        {
        LOG_ERR("ncr710StartFirstPhase : SemTake timeout\n");
        return (ERROR);
        }
    return (pSiop->saveScriptIntrStat);
    }
 
/*******************************************************************************
*
* ncr710SyncMsgConvert - compute the ncr710 values for synchronous tranfert  
* 
* This routine should be called when an incoming extended synchronous message
* has been read.  This routine will try to find out the closest value possible
* for REQ/ACK period to fit the contents of the target message value.
* If it's impossible to fit exactly,  it returns the closest value it finds.
* If the input value is too slow (<0xfa assync throughput) or too high (>0x19 
* around 10Mbit/s) a null offset and period will be return.
* If the offset is greater than the maximun chip value,  it's set to the
* maximum offset allowable for the chip.
* If some of input parameters are not consistant an ERROR will be returned.
* The hardware register (SBCL and SXFER) values are computed and returned 
* in the syncReserved1 and synvReserved2 field in the SCSI_PHYS_DEV structure
* to be used each time by the driver when the SCSI_PHYS_DEV have a <syncXfer>
* field set to TRUE. 
*
* RETURNS: OK or ERROR if some bad parameters.
*
* NOTE: As this routine uses input clock frequency and clock division
* ,it should be called after a ncr710CtrlCreate and ncr710CtrlInit.
*
* SEE ALSO
* scsiSyncTarget(), scsiLib, scsiLib.h 
*
* NOMANUAL
*/
 
LOCAL STATUS ncr710SyncMsgConvert
    (
    SCSI_PHYS_DEV *pScsiPhysDev,        /* ptr to device info */
    UINT8 *pMsgIn,                      /* ptr to extended message */
    int *pSyncVal,                      /* to return sync info */
    int *pOffsetVal                     /* to return offset info */
    )

    {
    SIOP *pSiop;	/* local pointer to the controller info */
    int coreClock;	/* core clock value */
    int div;		/* core clock divider */
    int bestFound;
    int bestXferp;
    int bestDiv;
    int xferLoop;
    int periodValue;
    int offsetValue;
    int tmp = 0 ;

    if ((pScsiPhysDev == (SCSI_PHYS_DEV *) NULL) ||
        (pMsgIn       == NULL) ||
        (pSyncVal     == NULL) ||
        (pOffsetVal   == NULL))
        {
        return (ERROR);
        }

    /* Get period and offset from the incoming message */
    periodValue = (int)pMsgIn[3];
    offsetValue = (int)pMsgIn[4];


    if ((periodValue > MAX_SCSI_SYNC_PERIOD) || 
	(periodValue < MIN_SCSI_SYNC_PERIOD))
        {
        errnoSet (S_scsiLib_SYNC_VAL_UNSUPPORTED);  
        return (ERROR);
        }
 
    /* check for a chip supported offset value */
    if ((UINT)offsetValue > (UINT)NCR710_MAXSYNC_OFF)
         offsetValue = NCR710_MAXSYNC_OFF;

    /* express period in ns * 4 (SCSI ANSI) and mutiply by 100 to 
     * have a fix decimal part.
     */
    periodValue = periodValue * SCSI_SYNC_STEP_PERIOD * 100;
    pSiop = (SIOP *) (pScsiPhysDev->pScsiCtrl);

    bestFound = MAX_SCSI_SYNC_PERIOD * SCSI_SYNC_STEP_PERIOD * 100;
    bestXferp = NCR710_MAX_XFERP + 1 ;
    bestDiv = 0;

    coreClock = pSiop->scsiCtrl.clkPeriod; 

    /* Find out the closest division value to match the scsi period requested.
     * The formulas used are : 
     * 		PERIOD = TCP * (4 + XFERP)
     *          TCP    = 1/coreClock
     *          XFERP is the sync transfer period code used by the 
     *          chip (range 0-7).
     *          coreClock is the scsi core clock input divided by 1, 1.5, 2.
     *          or coreClock * (2/2), (2/3), (2/4).
     */


    for (div = 1; div < 4; div++)
	{
        for (xferLoop = 0; xferLoop <= 7; xferLoop++)
            {
	    tmp = ((coreClock * (div+1)) * (4 + xferLoop)) / 2 ;
            if (tmp >= periodValue)
	        {
                if ((bestFound) > tmp)
		    {
                    bestFound = tmp; 
                    bestXferp = xferLoop;
		    bestDiv   = div;
		    }
		}
            }
        }
    if (bestXferp > NCR710_MAX_XFERP)
        {

        /* nothing fits properly use assync tranfert */
 
        *pSyncVal = periodValue / (SCSI_SYNC_STEP_PERIOD * 100);
        *pOffsetVal = 0;
        return (OK);
        }

    /* Xfer register values */
    pScsiPhysDev->syncReserved1 =  (offsetValue * 256);
    pScsiPhysDev->syncReserved1 |=  (bestXferp * 4096);

    /* sbcl register clock divider value */
    pScsiPhysDev->syncReserved2 = (UINT8) bestDiv;

    /* return computed values */

    *pSyncVal = bestFound / (SCSI_SYNC_STEP_PERIOD * 100);
    *pOffsetVal = offsetValue;

    return(OK);
    }

/*******************************************************************************
*
* ncr710BuildMsgOut - build a message out
*
* Build a message out in the scsiPhysDev structure according to the 
* <useMsgOut> code in the scsiPhysDev structure.
* Only two message type are supported, synchronous and device reset message.
* The value code <SCSI_OTHER_MSGOUT> allow the user to build his own message
* to send to the target.
* For the synchronous message the specific driver routine is called.  If the 
* Driver do not support currently this feature a <S_scsiLib_SYNC_UNSUPPORTED>
* is returned.
* Because the message count is not incremental only one message should be
* build at time.
*
* RETURNS:ERROR or OK
*
* NOMANUAL
*
*/

LOCAL STATUS ncr710BuildMsgOut
    (
    SCSI_PHYS_DEV *pScsiPhysDev		/* pointer to phys dev info */
    )

    {
    UINT8 *pMsgOut;
    STATUS status;
    int syncVal;
    int offsetVal;
    
    switch (pScsiPhysDev->useMsgout)
        {

        case SCSI_OTHER_MSGOUT:
	    /* this is a user message ,don't modify anything */
            break;

        case SCSI_NO_MSGOUT:
            /* no message out to build clear msgLength */
	    pScsiPhysDev->msgLength = 0;
            break;

        case SCSI_WIDE_MSGOUT:
            /* Not supported */
	    pScsiPhysDev->msgLength = 0;
            break;

        case SCSI_DEV_RST_MSGOUT:
            pMsgOut = SCSI_GET_PTR_MSGOUT(pScsiPhysDev);
            pMsgOut[0]= SCSI_DEV_RST_MSGOUT;
            pScsiPhysDev->msgLength = 1; 
            break;

        case SCSI_SYNC_MSGOUT:
            /* Check if the driver is supporting sync capability */
            NCR710_DEBUG_MSG("SCSI_SYNC_MSGOUT\n");
            if (pScsiPhysDev->pScsiCtrl->scsiSyncMsgConvert == (FUNCPTR) NULL)
		{
		errnoSet(S_scsiLib_SYNC_UNSUPPORTED);	
                return(ERROR);
		}

            /* Build Message out */

            syncVal = pScsiPhysDev->syncXferPeriod / SCSI_SYNC_STEP_PERIOD;
            offsetVal = pScsiPhysDev->syncXferOffset;

            pMsgOut = SCSI_GET_PTR_MSGOUT(pScsiPhysDev);

            status = scsiBuildExtMsgOut(pMsgOut,SCSI_EXT_MSG_SYNC_XFER_REQ);

            /* set sync Value in extended sync message */

            if ((syncVal < MIN_SCSI_SYNC_PERIOD) ||
                (syncVal > MAX_SCSI_SYNC_PERIOD))
                    syncVal = MIN_SCSI_SYNC_PERIOD;

            /* round up value for the controller */

            pMsgOut[3] = (UINT8) syncVal;
            pMsgOut[4] = (UINT8) offsetVal;

            status = (*pScsiPhysDev->pScsiCtrl->scsiSyncMsgConvert) 
		      (pScsiPhysDev,pMsgOut, &syncVal, &offsetVal);

            NCR710_DEBUG_MSG("msgout[0]=0x%02x\n",pMsgOut[0]);
            NCR710_DEBUG_MSG("msgout[1]=0x%02x\n",pMsgOut[1]);
            NCR710_DEBUG_MSG("msgout[2]=0x%02x\n",pMsgOut[2]);
            NCR710_DEBUG_MSG("msgout[3]=0x%02x\n",pMsgOut[3]);
            NCR710_DEBUG_MSG("msgout[4]=0x%02x\n",pMsgOut[4]);
            NCR710_DEBUG_MSG("syncVal=0x%02x\n",syncVal);
            NCR710_DEBUG_MSG("offetVal=0x%02x\n",offsetVal);
 

            if (status == ERROR)
     	        return(ERROR);

            /* set the round up values in the message out */
            pMsgOut[3] = (UINT8) syncVal;
            pMsgOut[4] = (UINT8) offsetVal;

            /* Set Message Count */
            pScsiPhysDev->msgLength = 0x5;
            break;
 
        default:
	    return(ERROR);
        }

    return(OK);
    }

/*******************************************************************************
*
* ncr710PhaseMismatch - handle a phase mismacth case.
*
* It handles the phase mismatch case on the scsi.  This is occurred when 
* a target plans to disconnect during any data and message transfer phase or
* when the device is not able to read more data (Tape device).
* This routine updates the data or message count according to the state of 
* the ncr fifo and internal registers.  The save data pointers are also 
* updated to avoid any copy if a disconnect occur.
* The current phase requested is determined from the <phaseRequested> 
* parameter.
* 
* RETURNS:Script address to execute.
*
* NOTE:This routine is called at interrupt level.
*
* NOMANUAL
*
*/

LOCAL UINT *ncr710PhaseMismatch
    (
    SIOP *pSiop,		       /* pointer to controller info */
    SCSI_PHYS_DEV *pScsiPhysDev,       /* pointer to phys dev info */
    NCR_CTL *pNcrCtl,                  /* pointer to the current siop context */
    UINT phaseRequested		       /* phase requested */
    )

    {
    UINT  ncrRegCount;
    UINT8 tmpCount;
    UINT8 countFifo;
    UINT *localStart;

    NCR710_MISMATCH_MSG("scsi phase mismatch error\n", 0, 0, 0, 0, 0, 0);


    /* Clear any msg out request */
    pScsiPhysDev->useMsgout = SCSI_NO_MSGOUT;

    /* update pointer for data in if there is some data in sidl register */
    ncrRegCount = (*pSiop->pDbc) & NCR710_COUNT_MASK;

    NCR710_MISMATCH_MSG("ncr DBC = 0x%08x ", *pSiop->pDbc, 0, 0, 0, 0, 0);
    NCR710_MISMATCH_MSG("pData=0x%08x count=0x%08x\n", (int) pNcrCtl->pData,
                         pNcrCtl->dataCount, 0, 0, 0, 0);

    /* Check phase requested to update the good couple pointer/count */
    switch (phaseRequested)
        {

        case PHASE_DATAIN:
        case PHASE_MSGIN:

            if (*pSiop->pSstat1 & B_ILF)
                {
                /* update user buffer */
                *((UINT8 *)pSiop->pDnad) = *pSiop->pSidl;
                ncrRegCount--;
                }
            *pSiop->pCtest8 |= B_CLF;
            /* Update data pointer and count */

            if (phaseRequested == PHASE_MSGIN)
                {
                pNcrCtl->pMsgIn += (pNcrCtl->msgInCount - ncrRegCount);
                pNcrCtl->msgInCount -= (pNcrCtl->msgInCount - ncrRegCount);
                NCR710_MISMATCH_MSG("mismatch msgin\n", 0, 0, 0, 0, 0, 0);
                localStart = &ackMsg1;
                }
            else
                {
                pNcrCtl->pData += (pNcrCtl->dataCount - ncrRegCount);
                pNcrCtl->dataCount -= (pNcrCtl->dataCount - ncrRegCount);

                pScsiPhysDev->savedTransLength =  pNcrCtl->dataCount;
                pScsiPhysDev->savedTransAddress = pNcrCtl->pData;

		NCR710_MISMATCH_MSG("mismatch datain ", 0, 0, 0, 0, 0, 0);
		NCR710_MISMATCH_MSG("pData=0x%08x count=0x%08x\n",
				    (int) pNcrCtl->pData, pNcrCtl->dataCount,
				    0, 0, 0, 0);
                localStart = &checkPhData;
                }
 
            /* Clear Fifo and restart where phase is checked */
            *pSiop->pCtest8 |= B_CLF;
            return (localStart);

        case PHASE_DATAOUT:
        case PHASE_MSGOUT:

            /* Check for data in fifo and registers (SODR & SODL),
	     * count from DCMD minus DFIFO count and 0x7f (see ncr710 
	     * data manuel).
	     */

            NCR710_MISMATCH_MSG("mismatch dataout\n", 0, 0, 0, 0, 0, 0);

            tmpCount = (UINT8)(ncrRegCount & 0x7f);
            countFifo = *pSiop->pDfifo & 0x7f;
            tmpCount = (countFifo - tmpCount) & 0x7f;
    
            /* Check for SODL and SODR if residual data */
            if (*pSiop->pSstat1 & B_OLF)
                tmpCount++;
            if (*pSiop->pSstat1 & B_ORF)
                tmpCount++;
    
            ncrRegCount += tmpCount;
    
            /* Update dataCount and pointer */
            if (phaseRequested == PHASE_MSGOUT)
                {
                pNcrCtl->pMsgOut += (pNcrCtl->msgOutCount - ncrRegCount);
                pNcrCtl->msgOutCount -= (pNcrCtl->msgOutCount - ncrRegCount);
    
                localStart = &asortPh;
                NCR710_MISMATCH_MSG("mismatch msgout\n", 0, 0, 0, 0, 0, 0);
                }
            else
                {
                pNcrCtl->pData += (pNcrCtl->dataCount - ncrRegCount);
                pNcrCtl->dataCount -= (pNcrCtl->dataCount - ncrRegCount);

                pScsiPhysDev->savedTransLength =  pNcrCtl->dataCount;
                pScsiPhysDev->savedTransAddress = pNcrCtl->pData;

                localStart = &checkPhData;
                NCR710_MISMATCH_MSG("mismatch dataout ", 0, 0, 0, 0, 0, 0);
		NCR710_MISMATCH_MSG("pData=0x%08x count=0x%08x\n",
                                   (int) pNcrCtl->pData, pNcrCtl->dataCount, 
				   0, 0, 0, 0);
                }

            /* Clear Fifo and restart where phase is checked */
            *pSiop->pCtest8 |= B_CLF; 
            return(localStart);
    
        default:
            NCR710_MISMATCH_MSG("mismatch default unknown phase\n",
				0, 0, 0, 0, 0, 0);
            /* clear ncr fifo */
            *pSiop->pCtest8 |= B_CLF;
            return (&asortPh);
        }
    }

/*******************************************************************************
*
* ncr710ExtMsgRecv - handle a an extended message received
*
* It check the type of extended message received.  If it's not supported a
* message reject will be built.  Only the sync extended message is supported.
* If the synchronous target message values can't fit with the controller,
* a reset device message is sent to the target to be sure to get a 
* assynchronous protocol.
* 
* RETURNS: Script address to execute.
*
* NOMANUAL
*
*/

LOCAL UINT *ncr710ExtMsgRecv
    (
    SIOP *pSiop,                    /* pointer to controller info */
    SCSI_PHYS_DEV *pScsiPhysDev,    /* pointer to phys dev info */
    NCR_CTL *pNcrCtl,               /* pointer to the current siop context */
    UINT8 *msgIn		    /* pointer to msgin array */
    )

    {
    int syncVal;
    int offsetVal;
    UINT *localStart;
    STATUS status;

    LOG_ERR("msgin[0]=0x%02x\n", msgIn[0], 0, 0, 0, 0, 0);
    LOG_ERR("msgin[1]=0x%02x\n", msgIn[1], 0, 0, 0, 0, 0);
    LOG_ERR("msgin[2]=0x%02x\n", msgIn[2], 0, 0, 0, 0, 0);
    LOG_ERR("msgin[3]=0x%02x\n", msgIn[3], 0, 0, 0, 0, 0);
    LOG_ERR("msgin[4]=0x%02x\n", msgIn[4], 0, 0, 0, 0, 0);

    switch (msgIn[2])
        {

        case SCSI_EXT_MSG_SYNC_XFER_REQ:
       
            /* Find out hardware value for this chip to try to fit the 
	     * target value.
	     */
            status = ncr710SyncMsgConvert((SCSI_PHYS_DEV *)pNcrCtl->pScsiPhysDev
					  , msgIn, &syncVal, &offsetVal);

            if ((syncVal   != msgIn[3]) || (syncVal   == 0) ||
                (offsetVal == 0) || (status    == ERROR))
                {

                /* use msg out array build a reset msg */
                NCR710_DEBUG_MSG("rst dev syncCalc=%d  syncTarget=%d\n",
				 syncVal, msgIn[3]);

                /* Use SCSI_PHYS_DEV structure for the value to be returned
		 * by the target to the caller, it's possible because the 
		 * <pScsiPhysDev->syncXfer> is used to check if the device
		 * supports the sync transfer.
		 */

                pScsiPhysDev->syncXferPeriod = msgIn[3] * 4;
                pScsiPhysDev->syncXferOffset = msgIn[4];
                pScsiPhysDev->syncXfer = (TBOOL) FALSE;

                pNcrCtl->pMsgOut = SCSI_GET_PTR_MSGOUT(pScsiPhysDev);
                *pNcrCtl->pMsgOut = (UINT8) SCSI_MSG_BUS_DEVICE_RESET;
                pNcrCtl->msgOutCount = 1;
                return(&endAbort);
                }

            /* set sync value in register */
            *pSiop->pSbcl = (UINT8)(pScsiPhysDev->syncReserved2);
            *pSiop->pSxfer &= B_DHP;
            *pSiop->pSxfer |= (UINT8)((pScsiPhysDev->syncReserved1/256) & 0x7f);

            /* Set up sync values in physDev info for future use */
            pScsiPhysDev->syncXferPeriod = msgIn[3] * 4;
            pScsiPhysDev->syncXferOffset = msgIn[4];
 
	    /* set sync tranfert to true if the offset is not null 
	     * (assync agreement).
	     */ 
            if (msgIn[4] > 0)
                {
                pScsiPhysDev->syncXfer = (TBOOL) TRUE;
                }
            localStart = (UINT *) &ackMsg1;
            break;


       default:
            /* not a supported message send a reject Message */
            LOG_ERR("unsupported extended message\n");

            pNcrCtl->pMsgOut = SCSI_GET_PTR_MSGOUT(pScsiPhysDev);
            *pNcrCtl->pMsgOut = (UINT8)SCSI_MSG_MESSAGE_REJECT;
            pNcrCtl->msgOutCount = 1;

            localStart = (UINT *)&ackAtnMsg;
            break;
        }

    return(localStart);
    }
 
/*******************************************************************************
*
* ncr710RejectRecv - handle a reject message received
*
* Check what the last message sent to determine the appropriate action.
* If it's a sync extended message all the sync capability both for the hardware
* and the physDev structure will be cleared.
* If it's was a reject message or anything else,send a NO OP message to the 
* target.
*
* RETURNS: Script address to execute.
*
* NOMANUAL
*
*/

LOCAL UINT *ncr710RejectRecv
    (
    SIOP *pSiop,                        /* pointer to controller info */
    SCSI_PHYS_DEV *pScsiPhysDev,        /* pointer to phys dev info */
    NCR_CTL *pNcrCtl                    /* pointer to the nexus context */
    )

    {

    /* if the previous msg was a sync msg ,means that the target does not 
     * support the synchronous protocol. */
    pNcrCtl->pMsgOut = SCSI_GET_PTR_MSGOUT(pScsiPhysDev);

    if ((pNcrCtl->pMsgOut[0] == SCSI_MSG_EXTENDED_MESSAGE) &&
        (pNcrCtl->pMsgOut[2] == SCSI_EXT_MSG_SYNC_XFER_REQ))
        {

        /* Clear Hardware sync register */
        *pSiop->pSbcl = 0;
        *pSiop->pSxfer &= B_DHP;

        /* Clear sync capability in physDev structure */
        pScsiPhysDev->syncXfer = (TBOOL) FALSE;
        pScsiPhysDev->syncReserved2  = 0;
        pScsiPhysDev->syncReserved1  = 0;
        pScsiPhysDev->syncXferPeriod = 0;
        pScsiPhysDev->syncXferOffset = 0;
        /* Clear msg out request */
        pScsiPhysDev->useMsgout = SCSI_NO_MSGOUT;
        return(&ackMsg1);
        }

    /* if the previous msg was a unknown send a no op message */
    *pNcrCtl->pMsgOut = (UINT8)SCSI_MSG_NO_OP;
    pNcrCtl->msgOutCount = 1;
    return(&ackAtnMsg);

    }
 
/********************************************************************************
*
* ncr710CheckStatRegs - handle the ncr710 status registers
*
* Check and process the copy of the SSTAT0 and DSTAT ncr710 registers.
* Look through all register bits to find out the errors.
* If an error implies to restart the script, it is returned to <*localSatart>
* and the return status is set to OK.  Else returns ERROR.
*
* RETURNS: OK, ERROR.
*
* NOMANUAL
*
*/

LOCAL STATUS ncr710CheckStatRegs
    (
    SIOP *pSiop,			/* pointer to scsiCtrl info */
    SCSI_PHYS_DEV *pScsiPhysDev,	/* pointer to physDev info */
    UINT *localStart			/* return script address */
    )

    {
    UINT8 scratch;
    UINT8 *pMsgOut;

    /* local pointer to msg out array */
    pMsgOut = SCSI_GET_PTR_MSGOUT(pScsiPhysDev);

    /* check scsi Error */

    scratch = pSiop->saveSstat0;

    /* this is a strange a error I got a function complete 
     * and a time out ???? ,that hapen just after the 
     * select.(function complete = device selected).
     */
    if  ((scratch & (B_FCMP | B_STO)) == (B_FCMP | B_STO))
        {
        LOG_ERR("scsi function complete & timeout\n");
        LOG_ERR_CTXT;
        *localStart = (UINT)&asortPh;
	return(OK);
        }
    
    /* check for mismatch phase error.This case is handle at interrupt 
    * level.
    */
    if ((scratch & B_MA) == B_MA)
        {
        LOG_ERR("mismatch at task level\n");
        LOG_ERR_CTXT;
        }
    
    /* check for Function complete */
    if  ((scratch & B_FCMP) == B_FCMP)
        {
        LOG_ERR("scsi function complete intr\n");
        LOG_ERR_CTXT;
        }
    
        /* Check for timeout error */
        if ((scratch & B_STO) == (UINT8) B_STO) 
            {
            if (pScsiPhysDev->devStatus == SELECT_REQUESTED)
                {
                pScsiPhysDev->devStatus = SELECT_TIMEOUT;
                errnoSet (S_scsiLib_SELECT_TIMEOUT);
                }
            LOG_ERR("ncr710Phase : scsi time out detected\n");
            LOG_ERR_CTXT;

            /* Clear internal chip fifos */
            *pSiop->pCtest8 |= B_CLF; 
            }
    
    /* check for selected interrupt */
    if  ((scratch & B_SEL) == B_SEL)
        {
        LOG_ERR("scsi selected intr\n");
        LOG_ERR_CTXT;
        }
    
    /* check for scsi gross error */
    if  ((scratch & B_SGE) == B_SGE)
        {
        LOG_ERR("scsi gross error\n");
        LOG_ERR_CTXT;
        }
    
    /* check for unexpected disconnect */
    if  ((scratch & B_UDC) == B_UDC)
        {
        /* Check if some rst device msg have been send */
        if (pMsgOut[0] == SCSI_MSG_BUS_DEVICE_RESET) 
            {
            LOG_ERR("unexpected disconnect because target reseted\n"); 
            LOG_ERR_CTXT;
            /* Clear sync capability in physDev structure */
            pScsiPhysDev->syncXfer = (TBOOL) FALSE;
            /* Clear Hardware sync register */
            *pSiop->pSbcl = 0;
            *pSiop->pSxfer &= B_DHP; 
            }
        else
            {
            LOG_ERR("scsi unexpected disconnect\n");
            LOG_ERR_CTXT;
            /* Clear msg out request */
            pScsiPhysDev->useMsgout = SCSI_NO_MSGOUT;
            }
        }

    /* check for reset received */
    if  ((scratch & B_RSTE) == B_RSTE)
        {
        LOG_ERR("scsi reset received\n");
        LOG_ERR_CTXT;
        }
     
    /* check for parity error */
    if  ((scratch & B_PAR) == B_PAR)
        {
        LOG_ERR("scsi or bus parity error\n");
        LOG_ERR_CTXT;
        }
    
    /* check Dma error */

    scratch = pSiop->saveDstat;

    /* check for illegal instruction */
    if (scratch & B_IID)   
        {
        LOG_ERR("ncr710 illegal instruction\n");
        LOG_ERR_CTXT;
        }
    
    /* check for bus fault */
    if (scratch & B_BF)   
        {
        LOG_ERR("ncr710 access memory error\n");
        LOG_ERR_CTXT;
        }
    
    /* check for a timeout bus fault */
    if (scratch & B_WTD)  
        {
        LOG_ERR("ncr710 watchdog timeout error\n");
        LOG_ERR_CTXT;
        }
    
    /* check for abort condition  */
    if (scratch & B_ABT) 
        {
        LOG_ERR("ncr710 abort bit set\n");
        LOG_ERR_CTXT;
        }
    
    /* all of these conditions are fatal */
    return(ERROR);
    }

/*******************************************************************************
*
* ncr710FlushCache - flush cache entry
*
* It flushes all memory space used by the chip to transfer data from memory to 
* scsi.
* All of the data space to perform a transaction must be set before calling it.
* It uses at interrupt level to start a new scsi command with a safe cache 
* context.
* 
* RETURN: nothing.
*
* NOMANUAL
*/

LOCAL void ncr710FlushCache
    (
    SIOP *pSiop,			/* SIOP info pointer */
    NCR_CTL *pNcrCtl			/* nexus info pointer */
    )

    {
    SCSI_PHYS_DEV *pScsiPhysDev;
    SCSI_TRANSACTION *pScsiXaction;

    pScsiPhysDev = (SCSI_PHYS_DEV *)pNcrCtl->pScsiPhysDev;
    pScsiXaction = pScsiPhysDev->pScsiXaction;

    /* Flush user data buffer if it's a scsi write */

    if (pScsiPhysDev->pScsiXaction->dataDirection == O_WRONLY)
	CACHE_USER_FLUSH (pNcrCtl->pData, pNcrCtl->dataCount);

    /* Flush scsi command buffer */

    CACHE_USER_FLUSH (pNcrCtl->pCmd, pNcrCtl->cmdCount);

    /* Flush msgout array with the maximum msgout length possible */

    CACHE_USER_FLUSH (pNcrCtl->pMsgOut, MAX_MSG_OUT_BYTES + 1);
    }

/*******************************************************************************
*
* ncr710StepEnable1 - enable/disable script single step 
*
* Enable/disable single step facility on ncr chip.Also, unmask/mask single 
* step interrupt in Dien register.  To use the single step feature you must
* disable all the script code that turns on the SCSI timeout.  
* The script instruction is: 
*    move CTEST7 & ~B_TIMEO to CTEST7
* This instruction is used in the script to enable the time out only before 
* a select phase and awaits DISCONNECT.
* The timeout is disabled by:
*    move CTEST7 | B_TIMEO to CTEST7
* during the remainder of the script code.
* To disable it just put a ";" to the beginning of the line and recompile 
* the driver.  Before any SCSI execution you just have to use 
* ncr710StepEnable (pSiop, TRUE) to run in single step mode.  
* To run a script step use ncr710SingleStep(pSiop).
* To disable use ncr710StepEnable (pSiop, FALSE).
* 
* RETURNS: N/A
*
* NOTE: In the future it might be good to use a
* scratch register on the chip for use by the script
* enable/disable the Timeout feature at start of 
* the run time of the script.
* 
* NOMANUAL
*/

void ncr710StepEnable1 
    (
    SIOP *pSiop,		/* pointer to SIOP info */ 
    BOOL boolValue 		/* TRUE/FALSE to enable/disable single step */
    )

    {
    if  (boolValue == TRUE)
	{
	/* Disable intr timeout */
	*pSiop->pSien &= ~B_STO;

	/* Set global state */
        siopSStep = TRUE;

	/* enable single step interrupt */
        *pSiop->pDien |= B_SSI;

	/* set single step mode */
	*pSiop->pDcntl |= B_SSM;

	/* disable manual Start */
	*pSiop->pDmode &= ~B_MAN;

	/* Disable time out on the chip */
        *pSiop->pCtest7 |= B_TOUT;

	/* Init Single Step semaphore */
	semBInit (&pSiop->singleStepSem, SEM_Q_FIFO, SEM_EMPTY);
	}
    else 
	{
	/* Enable intr timeout */
	*pSiop->pSien |= B_STO;

	/* Set global state */
        siopSStep = FALSE;

	/* unset single step mode */
	*pSiop->pDcntl &= ~B_SSM;

	/* disable script interrupt */
        *pSiop->pDien &= ~B_SSI;

	/* enable manual Start */
	*pSiop->pDmode |= B_MAN;

	/* Enable time out on the chip */
        *pSiop->pCtest7 &= ~B_TOUT;

	/* terminate the single step semaphore */
	semTerminate (&pSiop->singleStepSem);
        }
    }

/*******************************************************************************
*
* ncr710SingleStep1 - perform a single step 
*
* Perform a single step by writing the STD bit in the DCNTRL register.
* The parameter is a pointer to the SIOP information. 
* 
* RETURNS: N/A
* 
* NOMANUAL
*/

void ncr710SingleStep1
    (
    SIOP * pSiop 		/* pointer to SIOP info */ 
    )

    {
    UINT *pAdd;


    *pSiop->pDcntl |= B_STD;

    semTake (&pSiop->singleStepSem, WAIT_FOREVER);

    pAdd = (UINT *) (*pSiop->pDsp);
    pAdd--;
    pAdd--;

    printf ("ncr710 Cur  Opcode @0x%08x: 0x%08x ", (int) pAdd, *pAdd); pAdd++;
    printf ("0x%08x\n",*pAdd); pAdd++;
    printf ("ncr710 Next Opcode @0x%08x: 0x%08x ", (int) pAdd, *pAdd); pAdd++;
    printf ("0x%08x\n", *pAdd); 
    ncr710Show ((SCSI_CTRL *) pSiop);
    }

/*******************************************************************************
*
* ncr710SetIntrTimeout - enable / disable SCSI interrupt timeout 
*
* Enable or disable SCSI timeout interrupt.
* The parameter is a pointer to the SIOP information. 
* 
* RETURNS: N/A
* 
* NOMANUAL
*/

void ncr710SetIntrTimeout 
    (
    SIOP * pSiop,		/* pointer to SIOP info */ 
    TBOOL boolVal 		/* TRUE enable time out */
    )

    {
    if (boolVal == TRUE)
        *pSiop->pSien |= B_STO;
    else
	*pSiop->pSien &= ~B_STO;
    }

/*******************************************************************************
*
* ncr710SelDevTimeout - do nothing 
*
* This entry point is here to satisfy scsiLib.  This option is not supported 
* with ncr710 chip.  The only SCSI time out value is 250ms.
*
* RETURNS: N/A
*
* NOMANUAL
*/

LOCAL void ncr710SelTimeOutCvt
    (
    FAST SCSI_CTRL *pScsiCtrl,          /* ptr to SCSI controller info */
    FAST UINT timeOutInUsec,            /* timeout in microsecs */
    FAST UINT *pTimeOutSetting          /* ptr to result */
    )
    {
    }

#ifdef NCR710_INT_LOG_MEM
/*******************************************************************************
*
* ncr710LogShow - show a log memory interrupt
*
* Show a log memory result in a friendly way.  To use only in trace debug way.
* This routine uses a global pointer pLogMemShow to display a log mem node.
*
* NOMANUAL
*
*/

void ncr710LogShow
    (
    )
    {
    printf("counIntr:    0x%08x",*pLogMemShow);pLogMemShow++;
    printf("logCount:   0x%08x",*pLogMemShow);pLogMemShow++;
    printf("dsps:      0x%08x\n",*pLogMemShow);pLogMemShow++;
    printf("scratch3:    0x%08x",*pLogMemShow);pLogMemShow++;
    printf("pNcrCtl:    0x%08x",*pLogMemShow);pLogMemShow++;
    printf("pNcrCtlCmd:0x%08x\n",*pLogMemShow);pLogMemShow++;
    printf("pScsiPhysDev:0x%08x",*pLogMemShow);pLogMemShow++;
    printf("pPhysDevNcr:0x%08x",*pLogMemShow);pLogMemShow++;
    printf("pData:     0x%08x\n",*pLogMemShow);pLogMemShow++;
    printf("dataCount:   0x%08x",*pLogMemShow);pLogMemShow++;
    printf("cmdRequest: 0x%08x",*pLogMemShow);pLogMemShow++;
    printf("scratch0:  0x%08x\n",*pLogMemShow);pLogMemShow++;
    printf("sbcl:        0x%08x",*pLogMemShow);pLogMemShow++;
    printf("sdid:       0x%08x",*pLogMemShow);pLogMemShow++;
    printf("istat:     0x%08x\n",*pLogMemShow);pLogMemShow++;
    pLogMemShow++;
    }
#endif /* NCR710_INT_LOG_MEM */
