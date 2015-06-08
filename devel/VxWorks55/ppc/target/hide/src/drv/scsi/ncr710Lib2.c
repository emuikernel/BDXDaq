/* ncr710Lib2.c - NCR 53C710 SCSI I/O Processor (SIOP) library (SCSI-2) */

/* Copyright 1989-2001 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
04p,19oct01,dat  Documentation formatting
04o,26feb99,dat  added init of wideXfer to FALSE. SPR 24089
04n,03dec98,ihw  Modified handling of ident message for new thread:
		 add normal message out (if any) - e.g. sync/wide xfer.
04m,26aug98,sut  renamed ncr710StepEnable into ncr710StepEnable2 ;
                 in ncr710CtrlCreate added support to use
                 ncr710StepEnable2 as the ncr710StepEnable
                 see also ncr710CommLib.c
04l,24aug98,sut  renamed ncr710SingleStep into ncr710SingleStep2 ;
                 In ncr710CtrlCreateScsi2 added support to use
                 ncr710SingleStep2 as the ncr710SingleStep
                 see also ncr710CommLib.c
04k,18May98,pfl  SPR 21260: added ncr710ResetATN() 
04j,09jul97,dgp  doc: correct fonts per SPR 7853
04i,29oct96,dgp  doc: editing for newly published SCSI libraries
04h,06may96,jds  and more doc tweaks...
04g,01may96,jds  yet more doc tweaks...
04f,08nov95,jds  more doc tweaks
04e,20sep95,jdi  doc cleanup.
04d,28apr95,jds  worked with ian on selection problem (hang) with WD33c93; 
		 integrated into WRS tree; backward compatability stuff
04c,16mar94,ihw  fixed problems highlighted by selection by WD 33C93
04b,27may94,ihw  documented prior to release
04a,02may94,ihw  major modifications to work with new SCSI architecture
    	    	    supports tagged commands
03b,23feb94,ihw  improved single-step support
03a,18feb94,ihw  modified for enhanced SCSI library: multiple initiators,
                    disconnect/reconnect and synchronous transfer supported
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
It is designed to work with scsi2Lib.  This driver runs in
conjunction with a script program for the NCR 53C710 chip.
The script uses the NCR 53C710 DMA function for data transfers.
This driver supports cache functions through cacheLib.

USER-CALLABLE ROUTINES
Most of the routines in this driver are accessible only through the I/O
system.  Three routines, however, must be called directly.  
ncr710CtrlCreateScsi2() creates a controller structure and
ncr710CtrlInitScsi2() initializes it.
The NCR 53C710 hardware registers need to be configured according to
the hardware implementation.  If the default configuration is not correct,
the routine ncr710SetHwRegisterScsi2() must be used to properly configure
the registers.

INTERNAL
This driver supports multiple initiators, disconnect/reconnect, tagged
command queueing and synchronous data transfer protocol.  In general, the
SCSI system and this driver will automatically choose the best combination
of these features to suit the target devices used.  However, the default
choices may be over-ridden by using the function "scsiTargetOptionsSet()"
(see scsi2Lib).

There are debug variables to trace events in the driver.
<scsiDebug> scsi2Lib debug variable, trace event in scsi2Lib, ncr710ScsiPhase(),
and ncr710Transact().
<scsiIntsDebug> prints interrupt informations.

INCLUDE FILES
ncr710.h, ncr710_2.h, ncr710Script.h, ncr710Script2.h

SEE ALSO: scsiLib, scsi2Lib, cacheLib,
.pG "I/O System"
*/

#define  INCLUDE_SCSI2
#include "vxWorks.h"
#include "memLib.h"
#include "ctype.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "logLib.h"
#include "semLib.h"
#include "intLib.h"
#include "errnoLib.h"
#include "cacheLib.h"
#include "taskLib.h"
#include "drv/scsi/ncr710.h"
#include "drv/scsi/ncr710Script.h"

/* defines */

typedef NCR_710_SCSI_CTRL SIOP;

#define SIOP_MAX_XFER_LENGTH  	((UINT) (0x00ffffff)) /* max data xfer length */


/* External */
IMPORT BOOL scsiDebug;
IMPORT BOOL scsiErrors;
IMPORT BOOL scsiIntsDebug;

IMPORT VOIDFUNCPTR ncr710SingleStepRtn; /* Single step routine */
IMPORT VOIDFUNCPTR ncr710StepEnableRtn; /* Step enable step routine */

/*
 *  Configurable options
 */
int ncr710SingleStepSemOptions = SEM_Q_PRIORITY;

char *ncr710ScsiTaskName      = SCSI_DEF_TASK_NAME;
int   ncr710ScsiTaskOptions   = SCSI_DEF_TASK_OPTIONS;
int   ncr710ScsiTaskPriority  = SCSI_DEF_TASK_PRIORITY;
int   ncr710ScsiTaskStackSize = SCSI_DEF_TASK_STACK_SIZE;


/* forward declarations */

LOCAL void   ncr710HwInit        (SIOP *pSiop);
LOCAL int    ncr710BusIdGet      (SIOP *pSiop, UINT busIdBits);
LOCAL UINT   ncr710RemainderGet  (SIOP *pSiop, UINT phase);
LOCAL int    ncr710EventTypeGet  (SIOP *pSiop);
LOCAL STATUS ncr710PhaseMismatch (NCR710_THREAD * pThread,
				  int             phase,
				  UINT            remCount);
LOCAL STATUS ncr710ScsiBusControl (SIOP *pSiop, int operation);
LOCAL void   ncr710ScsiBusReset   (SIOP *pSiop);
LOCAL STATUS ncr710ThreadActivate (SIOP *pSiop, NCR710_THREAD *pThread);
LOCAL BOOL   ncr710ThreadAbort    (SIOP *pSiop, NCR710_THREAD *pThread);
LOCAL void   ncr710Event          (SIOP *pSiop, NCR710_EVENT  *pEvent);
LOCAL void   ncr710ThreadEvent   (NCR710_THREAD *pThread, NCR710_EVENT *pEvent);
LOCAL void   ncr710InitEvent     (NCR710_THREAD *pThread, NCR710_EVENT *pEvent);
LOCAL void   ncr710InitIdentEvent(NCR710_THREAD *pThread, NCR710_EVENT *pEvent);
LOCAL void   ncr710TargIdentEvent(NCR710_THREAD *pThread, NCR710_EVENT *pEvent);
LOCAL void   ncr710IdentInContinue (NCR710_THREAD *pThread);
LOCAL void   ncr710ThreadReconnect (NCR710_THREAD *pThread);
LOCAL void   ncr710SharedMemInit   (SIOP *pSiop, NCR710_SHARED *pShMem);
LOCAL STATUS ncr710ThreadInit      (SIOP *pSiop, NCR710_THREAD *pThread);
LOCAL STATUS ncr710IdentThreadInit (NCR710_THREAD *pThread);
LOCAL void   ncr710ThreadUpdate    (NCR710_THREAD *pThread);
LOCAL void   ncr710ThreadComplete  (NCR710_THREAD *pThread);
LOCAL void   ncr710ThreadDefer     (NCR710_THREAD *pThread);
LOCAL void   ncr710ThreadFail      (NCR710_THREAD *pThread, int errNum);
LOCAL void   ncr710ThreadStateSet  (NCR710_THREAD *pThread,
				    SCSI_THREAD_STATE state);
LOCAL STATUS ncr710Activate      (SIOP               *pSiop,
				  NCR710_THREAD      *pThread);
LOCAL STATUS ncr710Resume        (SIOP               *pSiop,
				  NCR710_THREAD      *pThread,
				  NCR710_SCRIPT_ENTRY entryId);
LOCAL void   ncr710Abort         (SIOP               *pSiop);
LOCAL void   ncr710ScriptStart   (SIOP               *pSiop,
				  NCR710_THREAD      *pThread,
				  NCR710_SCRIPT_ENTRY entryId);
LOCAL STATUS ncr710ThreadParamsSet (NCR710_THREAD * pThread,
				    UINT8      offset,
				    UINT8      period);
LOCAL STATUS ncr710XferParamsQuery (SCSI_CTRL *pScsiCtrl,
				    UINT8     *pOffset,
				    UINT8     *pPeriod);
LOCAL STATUS ncr710XferParamsSet   (SCSI_CTRL *pScsiCtrl,
				    UINT8      offset,
				    UINT8      period);
LOCAL BOOL   ncr710XferParamsCvt   (SIOP  *pSiop,
				    UINT8 *pOffset,
				    UINT8 *pPeriod,
				    UINT8 *pXferParams,
				    UINT8 *pClockDiv);
LOCAL void   ncr710ResetATN ( FAST SCSI_CTRL *pScsiCtrl );

LOCAL void ncr710SingleStep2 (SIOP* pSiop, BOOL verbose);
LOCAL void ncr710StepEnable2 (SIOP* pSiop, BOOL boolValue);
LOCAL void   ncr710ResetATN ( FAST SCSI_CTRL *pScsiCtrl );

/*******************************************************************************
*
* ncr710CtrlCreateScsi2 - create a control structure for the NCR 53C710 SIOP
*
* This routine creates an SIOP data structure and must be called before
* using an SIOP chip. It must be called exactly once for a 
* specified SIOP controller.
* Since it allocates memory for a structure needed by all routines in 
* ncr710Lib, it must be called before any other routines in the library.
* After calling this routine, ncr710CtrlInitScsi2() must be called at least
* once before any SCSI transactions are initiated using the SIOP.
*
* A detailed description of the input parameters follows:
* .iP <baseAdrs>
* the address at which the CPU accesses the lowest (SCNTL0/SIEN)
* register of the SIOP.
* .iP <clkPeriod>
* the period of the SIOP SCSI clock input, in nanoseconds, multiplied
* by 100.  This is used to determine the clock period for the
* SCSI core of the chip and affects the timing of both asynchronous
* and synchronous transfers.
* Several commonly used values are defined in ncr710.h as follows:
*.CS
*	NCR710_1667MHZ  6000    /@ 16.67Mhz chip @/
*	NCR710_20MHZ    5000    /@ 20Mhz chip    @/
*	NCR710_25MHZ    4000    /@ 25Mhz chip    @/
*	NCR710_3750MHZ  2667    /@ 37.50Mhz chip @/
*	NCR710_40MHZ    2500    /@ 40Mhz chip    @/
*	NCR710_50MHZ    2000    /@ 50Mhz chip    @/
*	NCR710_66MHZ    1515    /@ 66Mhz chip    @/
*	NCR710_6666MHZ  1500    /@ 66.66Mhz chip @/
*.CE
*
* RETURNS: A pointer to the NCR_710_SCSI_CTRL structure, or NULL if memory 
* is unavailable or there are invalid parameters.
*/
NCR_710_SCSI_CTRL *ncr710CtrlCreateScsi2 
    (
    UINT8 *baseAdrs,		/* base address of the SIOP */
    UINT   clkPeriod		/* clock controller period (nsec*100) */
    )

    {
    FAST SIOP *pSiop;		    /* ptr to SIOP info */
    int        nBytes;	    	    /* total amount of cache-coherent memory */
    SCSI_CTRL *pScsiCtrl;
    static NCR710_HW_REGS ncr710HwRegs = DEFAULT_710_HW_REGS;


    /* SingleStep routine selection */
    
    ncr710SingleStepRtn = ncr710SingleStep2;
    ncr710StepEnableRtn = ncr710StepEnable2;


    /* verify parameters */
    if (baseAdrs == ((UINT8 *) NULL))
	return ((SIOP *) NULL);
    
    if ((clkPeriod > NCR710_1667MHZ) ||  
        (clkPeriod < NCR710_6666MHZ))
        return ((SIOP *) NULL);
    
    /* check that dma buffers are cache-coherent */

    if (!CACHE_DMA_IS_WRITE_COHERENT () ||
	!CACHE_DMA_IS_READ_COHERENT ())
	{
	SCSI_MSG ("ncr710CtrlCreateScsi2: shared memory not cache coherent.\n",
		  0, 0, 0, 0, 0, 0);
        return ((SIOP *) NULL);
	}

    /* calloc the controller info structure and two shared data areas */

    nBytes = sizeof (SIOP) + 2 * sizeof (NCR710_SHARED);

    if ((pSiop = (SIOP *) cacheDmaMalloc (nBytes)) == (SIOP *) NULL)
        return ((SIOP *) NULL);

    bzero ((char *) pSiop, nBytes);

    pSiop->pIdentShMem  = (NCR710_SHARED *) (pSiop + 1);
    pSiop->pClientShMem = pSiop->pIdentShMem + 1;

    /* set up configuration variables */

    pScsiCtrl = (SCSI_CTRL *) pSiop;
    
    pScsiCtrl->eventSize  = sizeof (NCR710_EVENT);
    pScsiCtrl->threadSize = sizeof (NCR710_THREAD);

    /* fill in virtual functions used by SCSI library */
    
    pScsiCtrl->maxBytesPerXfer = SIOP_MAX_XFER_LENGTH;

    pScsiCtrl->scsiTransact = (FUNCPTR) scsiTransact;

    pScsiCtrl->scsiEventProc       = (VOIDFUNCPTR) ncr710Event;
    pScsiCtrl->scsiThreadInit      = (FUNCPTR)     ncr710ThreadInit;
    pScsiCtrl->scsiThreadActivate  = (FUNCPTR)     ncr710ThreadActivate;
    pScsiCtrl->scsiThreadAbort     = (FUNCPTR)     ncr710ThreadAbort;
    pScsiCtrl->scsiBusControl      = (FUNCPTR)     ncr710ScsiBusControl;
    pScsiCtrl->scsiXferParamsQuery = (FUNCPTR)     ncr710XferParamsQuery;
    pScsiCtrl->scsiXferParamsSet   = (FUNCPTR)     ncr710XferParamsSet;

    /* the following virtual functions are not used with the NCR 53C710 */
   
    pScsiCtrl->wideXfer		       = FALSE;
    pScsiCtrl->scsiWideXferParamsQuery = NULL;
    pScsiCtrl->scsiWideXferParamsSet   = NULL;
    pScsiCtrl->scsiDevSelect = NULL;
    pScsiCtrl->scsiInfoXfer  = NULL;

    /* fill in generic SCSI info for this controller */

    scsiCtrlInit (&pSiop->scsiCtrl);

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
    pSiop->pTemp     = (UINT *) (baseAdrs + OFF_TEMP);
    pSiop->pLcrc     = baseAdrs + OFF_LCRC;
    pSiop->pCtest8   = baseAdrs + OFF_CTEST8;
    pSiop->pIstat    = baseAdrs + OFF_ISTAT;
    pSiop->pDfifo    = baseAdrs + OFF_DFIFO;
    pSiop->pDcmd     = baseAdrs + OFF_DCMD;
    pSiop->pDbc      = (UINT *) (baseAdrs + OFF_DBC);
    pSiop->pDnad     = (UINT *) (baseAdrs + OFF_DNAD);
    pSiop->pDsp      = (UINT *) (baseAdrs + OFF_DSP);
    pSiop->pDsps     = (UINT *) (baseAdrs + OFF_DSPS);
    pSiop->pScratch0 = baseAdrs + OFF_SCRATCH0;
    pSiop->pScratch1 = baseAdrs + OFF_SCRATCH1;
    pSiop->pScratch2 = baseAdrs + OFF_SCRATCH2;
    pSiop->pScratch3 = baseAdrs + OFF_SCRATCH3;
    pSiop->pDcntl    = baseAdrs + OFF_DCNTL;
    pSiop->pDwt      = baseAdrs + OFF_DWT;
    pSiop->pDien     = baseAdrs + OFF_DIEN;
    pSiop->pDmode    = baseAdrs + OFF_DMODE;
    pSiop->pAdder    = (UINT *) (baseAdrs + OFF_ADDER);

    pSiop->clkPeriod = clkPeriod;

    /*
     *	Initialise hardware-dependent registers to default values.
     */
    bcopy ((char *)&ncr710HwRegs, (char *)&pSiop->hwRegs,
	                          sizeof (NCR710_HW_REGS));

    /*
     *	Create synchronisation semaphore for single-step support
     */
    if ((pSiop->singleStepSem = semBCreate(ncr710SingleStepSemOptions,
					   SEM_EMPTY)) == NULL)
	{
	SCSI_MSG ("ncr710CtrlCreateScsi2: semBCreate of singleStepSem failed.\n"
		  , 0, 0, 0, 0, 0, 0);
	goto failed;
        }

    /*
     *	Initialise controller state variables
     */
    pSiop->state      = NCR710_STATE_IDLE;
    pSiop->cmdPending = FALSE;

    /*
     *	Initialize fixed fields in client shared data area
     */
    ncr710SharedMemInit (pSiop, pSiop->pClientShMem);

    /*
     *	Identification thread has been created by the generic initialisation.
     *	Initialise it for use with the NCR 53C710.
     */
    ncr710IdentThreadInit ((NCR710_THREAD *) pScsiCtrl->pIdentThread);

    /* spawn SCSI manager - use generic code from "scsiLib.c" */

    pScsiCtrl->scsiMgrId = taskSpawn (ncr710ScsiTaskName,
		       	    	      ncr710ScsiTaskPriority,
		       	    	      ncr710ScsiTaskOptions,
		       	    	      ncr710ScsiTaskStackSize,
		       	    	      (FUNCPTR) scsiMgr,
		       	    	      (int) pSiop, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    if (pScsiCtrl->scsiMgrId == ERROR)
	{
	SCSI_MSG ("ncr710CtrlCreateScsi2: can't spawn SCSI manager task\n",
		  0, 0, 0, 0, 0, 0);
	goto failed;
	}

    return (pSiop);

failed:
    if (pSiop->singleStepSem != NULL)
	(void) semDelete (pSiop->singleStepSem);

    (void) cacheDmaFree ((char *) pSiop);
    
    return (NULL);
    }

/*******************************************************************************
*
* ncr710CtrlInitScsi2 - initialize a control structure for the NCR 53C710 SIOP
*
* This routine initializes an SIOP structure after the structure is created
* with ncr710CtrlCreateScsi2().  This structure must be initialized before the
* SIOP can be used.  It may be called more than once if needed;  however,
* it must only be called while there is no activity on the SCSI interface.
*
* A detailed description of the input parameters follows:
* .iP <pSiop>
* a pointer to the NCR_710_SCSI_CTRL structure created with
* ncr710CtrlCreateScsi2().
* .iP <scsiCtrlBusId>
* the SCSI bus ID of the SIOP.  Its value is somewhat arbitrary:  seven (7),
* or highest priority, is conventional.  The value must be in the range 0 - 7.
* .iP <scsiPriority>
* this parameter is ignored.  All SCSI I/O is now done in the context of
* the SCSI manager task; if necessary, the priority of the manager task may be
* changed using taskPrioritySet() or by setting the value of the global
* variable `ncr710ScsiTaskPriority' before calling ncr710CtrlCreateScsi2().
* .LP
*
* RETURNS: OK, or ERROR if the parameters are out of range.
*
* SEE ALSO: ncr710CtrlCreateScsi2()
*/

STATUS ncr710CtrlInitScsi2
    (
    FAST NCR_710_SCSI_CTRL *pSiop,  /* ptr to SIOP struct */
    int scsiCtrlBusId,		    /* SCSI bus ID of this SIOP */
    int scsiPriority		    /* task priority when doing SCSI I/O */
    )

    {
    SCSI_CTRL * pScsiCtrl = (SCSI_CTRL *) pSiop;

    /*
     *	Validate parameters
     */
    if (pSiop == (SIOP *)NULL)
	return (ERROR);

    /*
     *	Validate and set bus ID
     */
    if (scsiCtrlBusId < SCSI_MIN_BUS_ID || scsiCtrlBusId > SCSI_MAX_BUS_ID)
	return (ERROR);
    
    pScsiCtrl->scsiCtrlBusId = scsiCtrlBusId;

    /*
     *	Initialise the SIOP
     */
    ncr710HwInit (pSiop);

    ncr710ScriptStart (pSiop,
		       (NCR710_THREAD *) pScsiCtrl->pIdentThread,
		       NCR710_SCRIPT_WAIT);

    pSiop->state = NCR710_STATE_PASSIVE;

    return (OK);
    }

/*******************************************************************************
*
* ncr710HwInit - initialize the SIOP chip to a known state
*
* This routine puts the SIOP into a known quiescent state.  Because the value
* of some registers depend upon the hardware implementation of the chip you 
* may have to use ncr710SetHwRegister() to adjust them.
*
* This routine does not touch the bits in registers that are set by 
* the ncr710SetHwRegister() routine.
*
* See the NCR 53C710 Data Manual for fuller details of the registers
* programmed by this routine.
*
* SEE ALSO 
*	ncr710SetHwRegister()
*
* NOMANUAL
*/

LOCAL void ncr710HwInit
    (
    FAST SIOP *pSiop			/* ptr to an SIOP info structure */
    )

    {
    UINT clkPeriod;

    /*
     *	Software reset the SIOP
     */
    *pSiop->pIstat = B_SOFTRST;
    *pSiop->pIstat = 0;

    /*
     *	Initialise hardware-dependent registers
     */
    ncr710SetHwRegisterScsi2 (pSiop, &pSiop->hwRegs);

    /*
     *	DMA mode, control and watchdog timer registers
     *
     *	Use manual start and NCR 53C700 compatibility mode.
     *	Set clock conversion factor (the clkPeriod has been checked in
     *	"ncr710CtrlCreate()") and save async clock period for use by
     *	"ncr710XferParamsCvt()".  Disable the watchdog timer.
     */
    *pSiop->pDwt    = 0;
    *pSiop->pDmode |= B_MAN;
    *pSiop->pDcntl |= B_COM;

    clkPeriod = pSiop->clkPeriod;
    
    if (clkPeriod >= NCR710_25MHZ)   	    	    /* 16.67 - 25.00 MHz */
    	{
        *pSiop->pDcntl |= NCR710_16MHZ_DIV;
	pSiop->asyncClkPeriod = clkPeriod;
    	}
    else if (clkPeriod >= NCR710_3750MHZ)    	    /* 25.01 - 37.50 MHz */
    	{
        *pSiop->pDcntl |= NCR710_25MHZ_DIV;
	pSiop->asyncClkPeriod = clkPeriod * 3 / 2;
    	}
    else if (clkPeriod >= NCR710_50MHZ) 	    /* 37.51 - 50.00 MHz */
    	{
        *pSiop->pDcntl |= NCR710_50MHZ_DIV;
	pSiop->asyncClkPeriod = clkPeriod * 2;
    	}
    else /* (clkPeriod >= NCR710_6666MHZ) */ 	    /* 50.01 - 66.67 MHz */
    	{
        *pSiop->pDcntl |= NCR710_66MHZ_DIV;
	pSiop->asyncClkPeriod = clkPeriod * 3;
    	}

    /*
     *	SCSI control registers
     *
     *	If disconnect/reconnect is enabled, use full arbitration.
     *	Always generate parity; check for parity if required to do so.
     *	Set slow cable mode if specified.
     */
    *pSiop->pScntl0 = B_EPG;
    *pSiop->pScntl1 = 0;

    if (pSiop->scsiCtrl.disconnect) 
	{
        *pSiop->pScntl0 |= (B_ARB1 | B_ARB0);
	*pSiop->pScntl1 |= B_ESR;
	}

    if (pSiop->parityCheckEnbl)
	*pSiop->pScntl0 |= B_EPC;
     
    if (pSiop->slowCableMode)
	*pSiop->pScntl1 |= B_EXC;

    /*
     *	Chip test registers
     *
     *	Enable active negation (needed for fast SCSI)
     */
    *pSiop->pCtest0 = B_EAN;

    /*
     *	Set own SCSI bus ID (checked in "ncr710CtrlInit()")
     */
    *pSiop->pScid = (1 << pSiop->scsiCtrl.scsiCtrlBusId);

    /*
     *	Enable relevant SCSI and DMA interrupts (see "ncr710EventTypeGet()")
     *
     *	Note: in target mode the phase mismatch interrupt (B_MA) is used to
     *	signal assertion of ATN.  Currently we cannot handle this either in
     *	this library or in the script.  Therefore, the script automatically
     *	disables this interrupt in target mode and enables it (for phase
     *	mismatch) in initiator mode.  Also see "ncr710EventTypeGet()".
     */
    *pSiop->pSien = (B_STO | B_SGE | B_UDC | B_RSTE | B_PAR); 

    *pSiop->pDien = (B_BF | B_ABT | B_SIR | B_WTD | B_IID);
    }

/*******************************************************************************
*
* ncr710SetHwRegisterScsi2 - set hardware-dependent registers for the NCR 53C710
*
* This routine sets up the registers used in the hardware
* implementation of the chip.  Typically, this routine is called by 
* the sysScsiInit() routine from the BSP.  
*
* The input parameters are as follows:
* .iP <pSiop> 4
* a pointer to the NCR_710_SCSI_CTRL structure created with
* ncr710CtrlCreateScsi2().
* .iP <pHwRegs>
* a pointer to a NCR710_HW_REGS structure that is filled with the logical
* values 0 or 1 for each bit of each register described below.
*
* This routine includes only the bit registers that can be used to modify 
* the behavior of the chip. The default configuration used during 
* ncr710CtlrCreateScsi2() and ncr710CrtlInitScsi2() is 
* {0,0,0,0,1,0,0,0,0,0,0,0,0,1,0}.
*
*.CS
*    typedef struct
*        {
*        int ctest4Bit7;   /@ Host bus multiplex mode @/
*        int ctest7Bit7;   /@ Disable/enable burst cache capability @/
*        int ctest7Bit6;   /@ Snoop control bit1 @/
*        int ctest7Bit5;   /@ Snoop control bit0 @/
*        int ctest7Bit1;   /@ invert tt1 pin (sync bus host mode only)@/
*        int ctest7Bit0;   /@ enable differential scsi bus capability@/
*        int ctest8Bit0;   /@ Set snoop pins mode @/
*        int dmodeBit7;    /@ Burst Length transfer bit 1 @/
*        int dmodeBit6;    /@ Burst Length transfer bit 0 @/
*        int dmodeBit5;    /@ Function code bit FC2 @/
*        int dmodeBit4;    /@ Function code bit FC1 @/
*        int dmodeBit3;    /@ Program data bit (FC0) @/
*        int dmodeBit1;    /@ user  programmable transfer type @/
*        int dcntlBit5;    /@ Enable Ack pin @/
*        int dcntlBit1;    /@ Enable fast arbitration on host port @/
*        } NCR710_HW_REGS;
*
*.CE
* For a more detailed explanation of the register bits, refer to the
* .I "NCR 53C710 SCSI I/O Processor Programming Guide."
*
* NOTE
* Because this routine writes to the chip registers you cannot use it
* if there is any SCSI bus activity.
*
* RETURNS: OK, or ERROR if any input parameter is NULL.
*
* SEE ALSO: ncr710CtrlCreateScsi2(), 
* .I "NCR 53C710 SCSI I/O Processor Programming Guide"
*/

STATUS ncr710SetHwRegisterScsi2
    (
    FAST SIOP	   *pSiop,	/* pointer to SIOP info */
    NCR710_HW_REGS *pHwRegs     /* pointer to a NCR710_HW_REGS info */
    )

    {
    /* check input parameters */
    if ((pSiop == NULL) || (pHwRegs == NULL))
	return (ERROR);

    /* fill the SIOP structure with new parameters */
    bcopy ((char *)pHwRegs, (char *) &pSiop->hwRegs, sizeof(NCR710_HW_REGS));

    /* Set each bit register to the new value */
    
    /* This bit must be set first else the chip will not anwser to a slave 
     * access in some hardware implementations.
     */
    ((pHwRegs->dcntlBit5)  ? (*pSiop->pDcntl   =  B_EA)
			   : (*pSiop->pDcntl   =  0));
    ((pHwRegs->ctest4Bit7) ? (*pSiop->pCtest4 |=  B_MUX)
			   : (*pSiop->pCtest4 &= ~B_MUX));
    ((pHwRegs->ctest7Bit7) ? (*pSiop->pCtest7 |=  B_CDIS)
			   : (*pSiop->pCtest7 &= ~B_CDIS));
    ((pHwRegs->ctest7Bit6) ? (*pSiop->pCtest7 |=  B_SC1)
			   : (*pSiop->pCtest7 &= ~B_SC1));
    ((pHwRegs->ctest7Bit5) ? (*pSiop->pCtest7 |=  B_SC0)
			   : (*pSiop->pCtest7 &= ~B_SC0));
    ((pHwRegs->ctest7Bit1) ? (*pSiop->pCtest7 |=  B_TT1)
			   : (*pSiop->pCtest7 &= ~B_TT1));
    ((pHwRegs->ctest7Bit0) ? (*pSiop->pCtest7 |=  B_DIFF) 
			   : (*pSiop->pCtest7 &= ~B_DIFF));
    ((pHwRegs->ctest8Bit0) ? (*pSiop->pCtest8 |=  B_SM)
			   : (*pSiop->pCtest8 &= ~B_SM));
    ((pHwRegs->dmodeBit7)  ? (*pSiop->pDmode  |=  B_BL1)
			   : (*pSiop->pDmode  &= ~B_BL1));
    ((pHwRegs->dmodeBit6)  ? (*pSiop->pDmode  |=  B_BL0)
			   : (*pSiop->pDmode  &= ~B_BL0));
    ((pHwRegs->dmodeBit5)  ? (*pSiop->pDmode  |=  B_FC2)
			   : (*pSiop->pDmode  &= ~B_FC2));
    ((pHwRegs->dmodeBit4)  ? (*pSiop->pDmode  |=  B_FC1)
			   : (*pSiop->pDmode  &= ~B_FC1));
    ((pHwRegs->dmodeBit3)  ? (*pSiop->pDmode  |=  B_PD)
			   : (*pSiop->pDmode  &= ~B_PD));
    ((pHwRegs->dmodeBit1)  ? (*pSiop->pDmode  |=  B_U0TT0)
			   : (*pSiop->pDmode  &= ~B_U0TT0));
    ((pHwRegs->dcntlBit1)  ? (*pSiop->pDcntl  |=  B_FA)
			   : (*pSiop->pDcntl  &= ~B_FA));
    return (OK);
    }


/*******************************************************************************
*
* ncr710ScsiBusControl - miscellaneous low-level SCSI bus control operations
*
* Currently supports only the SCSI_BUS_RESET operation; other operations are
* not used explicitly by the driver because they are carried out automatically
* by the script program.
*
* NOTE: after the SCSI bus has been reset, the SIOP generates an interrupt
* which causes an NCR710_BUS_RESET event to be sent to the SCSI manager.
* See "ncr710Intr()".
*
* RETURNS: OK, or ERROR if an invalid operation is requested.
*
* NOMANUAL
*/

LOCAL STATUS ncr710ScsiBusControl
    (
    SIOP * pSiop,   	    /* ptr to controller info                   */
    int    operation	    /* bitmask for operation(s) to be performed */
    )
    {
    if ((operation & ~SCSI_BUS_RESET) != 0)
	return (ERROR);
	
    if (operation & SCSI_BUS_RESET)
	ncr710ScsiBusReset (pSiop);

    return (OK);
    }

/*******************************************************************************
*
* ncr710ScsiBusReset - assert the RST line on the SCSI bus 
*
* Issue a SCSI Bus Reset command to the NCR 710. This should put all devices
* on the SCSI bus in an initial quiescent state.
*
* The bus reset will generate an interrupt which is handled by the normal
* ISR (see "ncr710Intr()").
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
    *pSiop->pScntl1 |=  B_RST;

    taskDelay (2);			/* pause for at least 250 us */

    *pSiop->pScntl1 &= ~B_RST;
    }

/*******************************************************************************
*
* ncr710IntrScsi2 - interrupt service routine for the SIOP
*
* Find the event type corresponding to this interrupt, and carry out any
* actions which must be done before the SIOP is re-started.  Determine
* whether or not the SIOP is connected to the bus (depending on the event
* type - see note below).  If not, start a client script if possible or
* else just make the SIOP wait for something else to happen.
*
* Notify the SCSI manager of a controller event.
*
* NOTE:
* The "connected" Boolean tells whether there is a SCSI thread in progress
* which must be continued before any other SCSI activity can occur.  In
* principle, it might seem that reading the appropriate bit in the SIOP's
* SCNTL1 register would be better than inferring the value from the interrupt
* code.
*
* However, the SCNTL1 register may change between the time the script
* completes (generates the interrupt) and the time it is read - for example,
* if the SIOP is reselected immediately after generating a DISCONNECTED
* interrupt.
*
* Because the action taken by the ISR depends critically on what will happen
* in software as a result of the current interrupt, and _not_ on the current
* state of the SIOP hardware, if the hardware "connected" bit were used, the
* above scenario would cause the ISR to fail to re-start a script when in
* fact it should do.  The result would be that the SIOP would forever remain
* IDLE, and the SCSI system would become deadlocked.  (I've seen it happen !)
*
* One disadvantage is that the ISR has to know a little too much about the
* semantics of the interrupt codes - an improvement might be to have a
* distinguishing feature (e.g. bit set) in the interrupt code to indicate
* whether or not to start a new script.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void ncr710IntrScsi2 
    (
    SIOP *pSiop
    )
    {
    NCR710_EVENT    event;
    SCSI_EVENT *    pScsiEvent = (SCSI_EVENT *) &event;
    SCSI_CTRL *     pScsiCtrl  = (SCSI_CTRL *)  pSiop;
    NCR710_THREAD * pThread    = (NCR710_THREAD *) pSiop->pHwThread;

    BOOL connected = FALSE;
    BOOL notify    = TRUE;
    int  oldState  = (int) pSiop->state;

    /*
     *	Save (partial) SIOP register context in current thread
     */
    pThread->scratch0 = *pSiop->pScratch0;
    pThread->scratch1 = *pSiop->pScratch1;
    pThread->scratch2 = *pSiop->pScratch2;
    pThread->scratch3 = *pSiop->pScratch3;

    pScsiEvent->type = ncr710EventTypeGet (pSiop);
    
    SCSI_INT_DEBUG_MSG ("ncr710Intr: DSA = 0x%08x, DSP = 0x%08x, type = %d.\n",
			*pSiop->pDsa, *pSiop->pDsp, pScsiEvent->type, 0, 0, 0);

    /*
     *  Synchronise with single-step routine, if enabled.
     */
    if (pSiop->singleStep)
    	semGive (pSiop->singleStepSem);

    if (pScsiEvent->type == NCR710_SINGLE_STEP)
	return;
    
    /*
     *	Handle interrupt according to type.
     */
    switch (pScsiEvent->type)
	{
	/*
	 *  Following cases result from completion of a script run.
	 */
	case NCR710_CMD_COMPLETE:
	case NCR710_DISCONNECTED:
	    connected = FALSE;
	    break;

    	case NCR710_MESSAGE_OUT_SENT:
    	case NCR710_MESSAGE_IN_RECVD:
	case NCR710_EXT_MESSAGE_SIZE:
	case NCR710_NO_MSG_OUT:
	case NCR710_SCRIPT_ABORTED:
	    connected = TRUE;
	    break;

	case NCR710_PHASE_MISMATCH:
	    event.remCount = ncr710RemainderGet (pSiop, pThread->nBusPhase);
	    
	    connected = TRUE;
	    break;

	/*
	 *  Following cases are asynchronous conditions, i.e. not resulting
	 *  directly from running a script.
	 */
    	case NCR710_READY:
	    connected = FALSE;
	    notify    = FALSE;
	    break;
	    
    	case NCR710_SELECTED:
    	case NCR710_RESELECTED:
	    pScsiEvent->busId = ncr710BusIdGet (pSiop, pThread->nBusIdBits);

	    pScsiEvent->nBytesIdent =
		(pThread->nHostFlags & FLAGS_IDENTIFY) ? 1 : 0;

	    connected = TRUE;
	    break;

	case NCR710_SCSI_BUS_RESET:
	    connected = FALSE;
	    break;

	/*
	 *  Following cases are error conditions (mixture of synchronous
	 *  and asynchronous).
	 */
	case NCR710_SCSI_TIMEOUT:
	    connected = FALSE;
	    break;

	case NCR710_ILLEGAL_PHASE:
	    connected = TRUE;
	    break;

	case NCR710_UNEXPECTED_DISCON:
	    connected = FALSE;
	    break;

    	case NCR710_NO_IDENTIFY:
	    logMsg ("ncr710Intr: no valid IDENTIFY message at (re)select.\n",
		    0, 0, 0, 0, 0, 0);
	    connected = TRUE;
	    break;
	    
    	case NCR710_SPURIOUS_CMD:
	    logMsg ("ncr710Intr: spurious command interrupt.\n",
		    0, 0, 0, 0, 0, 0);
	    connected = FALSE;
	    break;

	case NCR710_FATAL_ERROR:
	    logMsg ("ncr710Intr: unrecoverable error - re-starting SIOP.\n",
		    0, 0, 0, 0, 0, 0);
	    ncr710HwInit (pSiop);
	    connected = FALSE;
	    break;

	default:
	    logMsg ("ncr710Intr: unexpected interrupt status (%d).\n",
		    pScsiEvent->type, 0, 0, 0, 0, 0);
	    break;
	}

    /*
     *  Controller is now idle: if possible, make it run a script.
     *
     *	If a SCSI thread is suspended and must be processed at task-level,
     *	leave the SIOP idle.  It will be re-started by the SCSI manager
     *	calling "ncr710Resume()".
     *
     *	Otherwise, if there's a new SCSI thread to start (i.e., the SCSI
     *	manager has called "ncr710Activate()"), start the appropriate script.
     *
     *	Otherwise, start a script which puts the SIOP into passive mode
     *	waiting for re-selection, selection or a host command.
     *
     *	In all cases, clear any request to start a new thread.  The only
     *	tricky case is when there was a request pending and the SIOP is
     *	left IDLE.  This should only ever occur when the current event is
     *	selection or reselection, in which case the SCSI manager will retry
     *	the activation request.  (Also see "ncr710Activate ()".)
     */
    if (connected)
	{
    	pSiop->state = NCR710_STATE_IDLE;
	}
    else if (pSiop->cmdPending)
	{
	ncr710ScriptStart (pSiop,
			   (NCR710_THREAD *) pSiop->pNewThread,
			   NCR710_SCRIPT_INIT_START);

	pSiop->state = NCR710_STATE_ACTIVE;
	}
    else
	{
	ncr710ScriptStart (pSiop,
			   (NCR710_THREAD *) pScsiCtrl->pIdentThread,
			   NCR710_SCRIPT_WAIT);
	
	pSiop->state = NCR710_STATE_PASSIVE;
	}

    pSiop->cmdPending = FALSE;

    SCSI_INT_DEBUG_MSG ("ncr710Intr: state %d -> %d\n",
			oldState, pSiop->state, 0, 0, 0, 0);

    /*
     *	Send the event to the SCSI manager to be processed.
     */
    if (notify)
    	scsiMgrEventNotify ((SCSI_CTRL *) pSiop, pScsiEvent, sizeof (event));
    }

/*******************************************************************************
*
* ncr710BusIdGet - find bus ID of device which selected/reselected the SIOP.
*
* Calculate bus ID of SCSI peer which (re)selected the SIOP; "busIdBits"
* contains a copy of the LCRC register, i.e. an image of the data bits
* asserted on the SCSI bus during (re)selection.  Extract the peer's ID
* by masking out the bit corresponding to our bus ID then shifting until
* we find a bit set.
* 
* RETURNS: bus ID of connected peer device
*
* NOTE: This routine is called at interrupt level.
*
* NOMANUAL
*/
LOCAL int ncr710BusIdGet
    (
    SIOP * pSiop,			/* ptr to controller info       */
    UINT   busIdBits			/* bits corresponding to bus ID */
    )
    {
    UINT8 devBusId;

    busIdBits &= ~(*pSiop->pScid);
    
    for (devBusId = 0; devBusId <= SCSI_MAX_BUS_ID; ++devBusId)
	{
	if (busIdBits & (1 << devBusId))
	    break;
	}

    return (devBusId);
    }

/*******************************************************************************
*
* ncr710RemainderGet - get remaining xfer count and clean up after mismatch
*
* Clean up the SIOP's data path and calculate the number of bytes which
* were expected to be, but have not been transferred.
*
* The information transfer phase which halted because of the mismatch has
* been saved as part of the SIOP's register context.
*
* RETURNS: number of bytes not transferred
*
* NOTE: This routine is called at interrupt level.
*
* NOMANUAL
*/

LOCAL UINT ncr710RemainderGet
    (
    SIOP *pSiop,			/* pointer to controller info     */
    UINT  phase				/* phase terminated by mismatch   */
    )

    {
    UINT  remCount;
    UINT8 tmpCount;
    UINT8 countFifo;

    /*
     *	Find remaining byte count (may be corrected later).  For a fuller
     *	explanation, see Chapter 2 of the NCR 53C710 Data Manual.
     */
    remCount = *pSiop->pDbc & NCR710_COUNT_MASK;

    switch (phase)
        {
        case PHASE_DATA_IN:
        case PHASE_MSG_IN:
	case PHASE_STATUS:
	    /*
	     *	Input phases: check for pending byte to store, update
	     *	buffer if there is one.
	     */
            if (*pSiop->pSstat1 & B_ILF)
                {
                *((UINT8 *)pSiop->pDnad) = *pSiop->pSidl;
                remCount--;
                }
    	    break;

        case PHASE_DATA_OUT:
        case PHASE_MSG_OUT:
	case PHASE_COMMAND:
            /*
	     *  Output phases: check for data in fifo and output registers
	     *	(SODR & SODL), count from DBC minus DFIFO count and 0x7f
	     *	(see NCR 53C710 Data Manual).
	     */
            tmpCount  = (UINT8)(remCount & 0x7f);
            countFifo = *pSiop->pDfifo & 0x7f;
            tmpCount  = (countFifo - tmpCount) & 0x7f;
    
            if (*pSiop->pSstat1 & B_OLF)
                tmpCount++;

            if (*pSiop->pSstat1 & B_ORF)
                tmpCount++;
    
            remCount += tmpCount;
	    break;
    
        default:
            logMsg ("ncr710RemainderGet: invalid phase.\n", 0, 0, 0, 0, 0, 0);
	    break;
        }

    /*
     *	Clear data FIFOs
     */
    *pSiop->pCtest8 |= B_CLF;

    return (remCount);
    }

/******************************************************************************
*
* ncr710EventTypeGet - parse SCSI and DMA status registers at interrupt time
*
* NOTE
*   Only status bits which have corresponding interrupts enabled are checked !
*
* RETURNS: an interrupt (event) type code
*
* NOMANUAL
*/
LOCAL int ncr710EventTypeGet
    (
    SIOP * pSiop
    )
    {
    UINT  intrRegs;
    UINT8 scsiStatus;
    UINT8 dmaStatus;

    /*
     *	Read interrupt status registers
     *
     *	Note: read as a long word to avoid requirement for a delay when
     *	SCSI status and DMA status registers are read individually.
     */
    scsiStatus = dmaStatus = 0;

    *pSiop->pIstat &= ~B_ABORT;
    
    while ((*pSiop->pIstat & (B_SIP | B_DIP)) != 0)
        {
#if _BYTE_ORDER==_BIG_ENDIAN
        intrRegs = *((UINT *)pSiop->pSstat2);
        scsiStatus |= (UINT8)((intrRegs >> 8) & 0x0FF);
        dmaStatus  |= (UINT8)( intrRegs       & 0x0FF);
#else
        intrRegs = *((UINT *)pSiop->pDstat);
        scsiStatus |= (UINT8)((intrRegs >> 16) & 0x0FF);
        dmaStatus  |= (UINT8)((intrRegs >> 24) & 0x0FF);
#endif
        }
	SCSI_INT_DEBUG_MSG ("ncr710EventTypeGet: SCSI status = 0x%02x, "
			    "DMA status = 0x%02x\n",
		    	    scsiStatus, dmaStatus, 0, 0, 0, 0);

    /*
     *	Check for fatal errors (first !)
     */
    if  (scsiStatus & B_SGE)
        {
	logMsg ("ncr710: SCSI bus gross error.\n", 0, 0, 0, 0, 0, 0);
	return (NCR710_FATAL_ERROR);
        }

    if  (scsiStatus & B_PAR)
        {
	logMsg ("ncr710: parity error.\n", 0, 0, 0, 0, 0, 0);
	return (NCR710_FATAL_ERROR);
        }

    if (dmaStatus & B_IID)
        {
	logMsg ("ncr710: illegal instruction (DSP = 0x%08x).\n",
		*pSiop->pDsp, 0, 0, 0, 0, 0);
	return (NCR710_FATAL_ERROR);
        }

    if (dmaStatus & B_BF)
        {
	logMsg ("ncr710: bus fault (DSP = 0x%08x).\n",
		*pSiop->pDsp, 0, 0, 0, 0, 0);
	return (NCR710_FATAL_ERROR);
        }

    if (dmaStatus & B_WTD)
        {
	logMsg ("ncr710: watchdog timeout (DSP = 0x%08x).\n",
		*pSiop->pDsp, 0, 0, 0, 0, 0);
	return (NCR710_FATAL_ERROR);
        }

    /*
     *	No fatal errors; try the rest (NB order of tests is important !)
     */
    if  (scsiStatus & B_RSTE)
        {
	SCSI_INT_DEBUG_MSG ("ncr710EventTypeGet: SCSI bus reset.\n",
			    0, 0, 0, 0, 0, 0);

	*pSiop->pCtest8 |= B_CLF;	/* clear FIFOs */
	
	return (NCR710_SCSI_BUS_RESET);
        }
     
    if  (scsiStatus & B_UDC)
        {
	SCSI_INT_DEBUG_MSG ("ncr710EventTypeGet: unexpected disconnection.\n",
			    0, 0, 0, 0, 0, 0);

	*pSiop->pCtest8 |= B_CLF;	/* clear FIFOs */
	
	return (NCR710_UNEXPECTED_DISCON);
        }

    if (scsiStatus & B_STO)
	{
    	SCSI_INT_DEBUG_MSG ("ncr710EventTypeGet: SCSI bus timeout.\n",
			    0, 0, 0, 0, 0, 0);

	*pSiop->pCtest8 |= B_CLF;	/* clear FIFOs */
	
	return (NCR710_SCSI_TIMEOUT);
	}

    if (scsiStatus & B_MA)
        {
	/*
	 * workaround for problem with ATN when selected
	 * by ncr710 (negates and then briefly re-asserts
	 * ATN during REQ/ACK cycle, causing an ATN
	 * interrupt even though ATN ends up negated).
	 *
	 * The "phase mismatch" interrupt is used to indicate assertion of
	 * ATN in target mode.  This interrupt should have been disabled
	 * by the script when in target mode, because the current code does
	 * not know how to handle it.
	 */
	if (*pSiop->pScntl0 & B_TRG)
	    {
	    if (*pSiop->pSien & B_MA)
		{
		logMsg ("ncr710EventTypeGet: enabled ATN interrupt!\n",
			0, 0, 0, 0, 0, 0);
		return (NCR710_FATAL_ERROR);
		}
	    }
	else
	    {
	    SCSI_INT_DEBUG_MSG ("ncr710EventTypeGet: phase mismatch.\n",
			    0, 0, 0, 0, 0, 0);
	    return (NCR710_PHASE_MISMATCH);
	    }
        }
    
    if (dmaStatus & B_ABT)
        {
	SCSI_INT_DEBUG_MSG ("ncr710EventTypeGet: script aborted.\n",
		    	    0, 0, 0, 0, 0, 0);
	return (NCR710_SCRIPT_ABORTED);
        }

    if (dmaStatus & B_SIR)
	{
	SCSI_INT_DEBUG_MSG ("ncr710EventTypeGet: script completed.\n",
			    0, 0, 0, 0, 0, 0);
	return (*pSiop->pDsps);
        }

    if (dmaStatus & B_SSI)
	{
	SCSI_INT_DEBUG_MSG ("ncr710EventTypeGet: single-step.\n",
			    0, 0, 0, 0, 0, 0);

	return (NCR710_SINGLE_STEP);
	}

    /*
     *	No (expected) reason for the interrupt !
     */
    logMsg ("ncr710EventTypeGet: spurious interrupt !\n", 0, 0, 0, 0, 0, 0);
    
    return (NCR710_FATAL_ERROR);
    }


/*******************************************************************************
* ncr710ThreadActivate - activate a SCSI connection for an initiator thread
*
* Set whatever thread/controller state variables need to be set.  Ensure that
* all buffers used by the thread are coherent with the contents of the
* system caches (if any).
*
* Set transfer parameters for the thread based on what its target device
* last negotiated.
*
* Update the thread context (including shared memory area) and note that
* there is a new client script to be activated (see "ncr710Activate()").
*
* Set the thread's state to ESTABLISHED.  This is strictly a bit premature,
* but there is no distinction as far as this driver is concerned between a
* thread which is connecting and one which has connected (i.e., the script
* just runs to completion and we have to examine its exit status to determine
* how far through the process the thread got).
*
* Do not wait for the script to be activated.  Completion of the script will
* be signalled by an event which is handled by "ncr710Event()".
*
* RETURNS: OK or ERROR
*
* NOMANUAL
*/
LOCAL STATUS ncr710ThreadActivate
    (
    SIOP *          pSiop,		/* ptr to controller info */
    NCR710_THREAD * pThread		/* ptr to thread info     */
    )
    {
    SCSI_CTRL *   pScsiCtrl   = (SCSI_CTRL *)   pSiop;
    SCSI_THREAD * pScsiThread = (SCSI_THREAD *) pThread;
    SCSI_TARGET * pScsiTarget = pScsiThread->pScsiTarget;
    
    SCSI_DEBUG_MSG ("ncr710ThreadActivate: thread 0x%08x: activating\n",
		    (int) pThread, 0, 0, 0, 0, 0);

    scsiCacheSynchronize (pScsiThread, SCSI_CACHE_PRE_COMMAND);

    /*
     *	Reset controller state variables: set sync xfer parameters
     */
    pScsiCtrl->msgOutState = SCSI_MSG_OUT_NONE;
    pScsiCtrl->msgInState  = SCSI_MSG_IN_NONE;
    
    scsiSyncXferNegotiate (pScsiCtrl, pScsiTarget, SYNC_XFER_NEW_THREAD);

    if (ncr710ThreadParamsSet (pThread, pScsiTarget->xferOffset,
			     	        pScsiTarget->xferPeriod) != OK)
	{
	SCSI_ERROR_MSG ("ncr710ThreadActivate: failed to set thread params.\n",
			0, 0, 0, 0, 0, 0);
	return (ERROR);
	}

    /*
     *  Concatenate the ident message with a pending 'normal' message out,
     *  if possible.  This allows the script to send the first message out
     *  within the same MSG OUT phase as the IDENTIFY message - needed on
     *  some target systems (e.g. DG Clariion RAID) to avoid the message
     *  being rejected(!).
     */
 
    pSiop->identMsgLength = 0;
 
    bcopy (pScsiThread->identMsg,
           pSiop->identMsg,
           pScsiThread->identMsgLength);
 
    pSiop->identMsgLength += pScsiThread->identMsgLength;
 
    if (pScsiCtrl->msgOutState == SCSI_MSG_OUT_PENDING)
        {
        bcopy (pScsiCtrl->msgOutBuf,
               pSiop->identMsg + pSiop->identMsgLength,
               pScsiCtrl->msgOutLength);
 
        pSiop->identMsgLength += pScsiCtrl->msgOutLength;
        }

    /*
     *	Update thread context; activate the thread
     */
    ncr710ThreadUpdate (pThread);
    
    if (ncr710Activate (pSiop, pThread) != OK)
	{
	SCSI_ERROR_MSG ("ncr710ThreadActivate: failed to activate thread.\n",
		        0, 0, 0, 0, 0, 0);
	return (ERROR);
	}

    pScsiCtrl->pThread = pScsiThread;

    ncr710ThreadStateSet (pThread, SCSI_THREAD_ESTABLISHED);
    
    return (OK);
    }


/*******************************************************************************
* ncr710ThreadAbort - abort a thread
*
* If the thread is not currently connected, do nothing and return FALSE to
* indicate that the SCSI manager should abort the thread.
*
* Otherwise (the thread is active onthe controller), build an ABORT or
* ABORT TAG message which will (eventually) be sent, causing the taget to
* disconnect.  Abort the current script run so that this message can be
* sent.  Set the state of the thread accordingly, and return TRUE to
* indicate that the controller driver will handle the abort process.
*
* RETURNS: TRUE if the thread is being aborted by this driver (i.e. it is
* currently active on the controller, else FALSE.
*
* NOMANUAL
*/
LOCAL BOOL ncr710ThreadAbort
    (
    SIOP *          pSiop,		/* ptr to controller info */
    NCR710_THREAD * pThread		/* ptr to thread info     */
    )
    {
    BOOL          tagged;
    SCSI_CTRL *   pScsiCtrl   = (SCSI_CTRL *)   pSiop;
    SCSI_THREAD * pScsiThread = (SCSI_THREAD *) pThread;

    SCSI_DEBUG_MSG ("ncr710ThreadAbort: thread 0x%08x (state = %d) aborting\n",
		    (int) pThread, pScsiThread->state, 0, 0, 0, 0);

    if (pScsiThread != pScsiCtrl->pThread)
	return (FALSE);
    
    switch (pScsiThread->state)
	{
	case SCSI_THREAD_INACTIVE:
	case SCSI_THREAD_WAITING:
	case SCSI_THREAD_DISCONNECTED:
	    return (FALSE);
	    break;

	default:
	    /*
	     *	Build an ABORT (or ABORT TAG) message.  When this has been
	     *	sent, the target should disconnect.  Mark the thread aborted
	     *	and continue until disconnection.
	     */
	    tagged = (pScsiThread->tagNumber != NONE);

	    pScsiCtrl->msgOutBuf[0] = tagged ? SCSI_MSG_ABORT_TAG
		    	    	    	     : SCSI_MSG_ABORT;
	    pScsiCtrl->msgOutLength = 1;
	    pScsiCtrl->msgOutState  = SCSI_MSG_OUT_PENDING;

	    ncr710Abort (pSiop);
	    
    	    ncr710ThreadStateSet (pThread, SCSI_THREAD_ABORTING);
	    break;
	}

    return (TRUE);
    }


/*******************************************************************************
* ncr710Event - NCR 53C710 SCSI controller event processing routine
*
* Parse the event type and act accordingly.  Controller-level events are
* handled within this function, and the event is then passed to the current
* thread (if any) for thread-level processing.
*
* Note the special case when (re)selection occurs: if there is a current
* thread when the event occurs, it receives the event (and is assumed to
* defer itself) before the identification thread is made current.  The
* event is then forwarded to the identification thread.
*
* RETURNS: N/A
*/
LOCAL void ncr710Event
    (
    SIOP *         pSiop,
    NCR710_EVENT * pEvent
    )
    {
    SCSI_CTRL  *    pScsiCtrl  = (SCSI_CTRL *)  pSiop;
    SCSI_EVENT *    pScsiEvent = (SCSI_EVENT *) pEvent;
    NCR710_THREAD * pThread    = (NCR710_THREAD *) pScsiCtrl->pThread;
    
    SCSI_DEBUG_MSG ("ncr710Event: received event %d (thread = 0x%08x)\n",
		    pScsiEvent->type, (int) pThread, 0, 0, 0, 0);

    /*
     *	Do controller-level event processing
     */
    switch (pScsiEvent->type)
	{
	case NCR710_SELECTED:
	case NCR710_RESELECTED:
	    /*
	     *	Forward event to current thread, if any (it should defer)
	     *	then install a reserved thread for identification purposes.
	     */
    	    if (pThread != 0)
	    	ncr710ThreadEvent (pThread, pEvent);

    	    pScsiCtrl->peerBusId = pScsiEvent->busId;

    	    pScsiCtrl->pThread = pScsiCtrl->pIdentThread;
	    
	    pScsiCtrl->pThread->role = (pScsiEvent->type == NCR710_SELECTED)
		    	    	     ? SCSI_ROLE_IDENT_TARG
				     : SCSI_ROLE_IDENT_INIT;

	    pThread = (NCR710_THREAD *) pScsiCtrl->pThread;

	    scsiMgrCtrlEvent (pScsiCtrl, SCSI_EVENT_CONNECTED);
	    break;

	case NCR710_DISCONNECTED:
	case NCR710_CMD_COMPLETE:
	case NCR710_UNEXPECTED_DISCON:
	case NCR710_SCSI_TIMEOUT:
    	    pScsiCtrl->peerBusId = NONE;
    	    pScsiCtrl->pThread   = 0;

	    scsiMgrCtrlEvent (pScsiCtrl, SCSI_EVENT_DISCONNECTED);
	    
    	    /* assert (pThread != 0); */
	    break;

	case NCR710_SCSI_BUS_RESET:
    	    pScsiCtrl->peerBusId = NONE;
    	    pScsiCtrl->pThread   = 0;

    	    scsiMgrBusReset (pScsiCtrl);
	    break;

	default:
	    /*
	     *	Any other event type is assumed to be thread-specific.
	     *	The thread event handler will report an error if it's
	     *	not a valid type.
	     */
	    if (pThread == 0)
		{
	    	logMsg ("ncr710Event: invalid event type (%d)\n",
		    	pScsiEvent->type, 0, 0, 0, 0, 0);
		}
	    break;
	}
    /*
     *	If there's a thread on the controller, forward the event to it
     */
    if (pThread != 0)
	ncr710ThreadEvent (pThread, pEvent);
    }

    
/*******************************************************************************
* ncr710ThreadEvent - NCR 53C710 thread event processing routine
*
* Forward the event to the proper handler for the thread's current role.
*
* If the thread is still active, update the thread context (including
* shared memory area) and resume the thread.
*
* RETURNS: N/A
*
* NOMANUAL
*/
LOCAL void ncr710ThreadEvent
    (
    NCR710_THREAD * pThread,
    NCR710_EVENT *  pEvent
    )
    {
    SCSI_EVENT *  pScsiEvent  = (SCSI_EVENT *)  pEvent;
    SCSI_THREAD * pScsiThread = (SCSI_THREAD *) pThread;
    SIOP *        pSiop       = (SIOP *)        pScsiThread->pScsiCtrl;
    NCR710_SCRIPT_ENTRY entryPt;
    
    SCSI_DEBUG_MSG ("ncr710ThreadEvent: thread 0x%08x: received event %d\n",
		    (int) pThread, pScsiEvent->type, 0, 0, 0, 0);

    switch (pScsiThread->role)
	{
	case SCSI_ROLE_INITIATOR:
	    ncr710InitEvent (pThread, pEvent);

	    entryPt = NCR710_SCRIPT_INIT_CONTINUE;
	    break;
	    
	case SCSI_ROLE_IDENT_INIT:
	    ncr710InitIdentEvent (pThread, pEvent);

	    entryPt = NCR710_SCRIPT_INIT_CONTINUE;
	    break;

	case SCSI_ROLE_IDENT_TARG:
	    ncr710TargIdentEvent (pThread, pEvent);

	    entryPt = NCR710_SCRIPT_TGT_DISCONNECT;
	    break;
	    
	case SCSI_ROLE_TARGET:
	default:
	    logMsg ("ncr710ThreadEvent: thread 0x%08x: invalid role (%d)\n",
		    (int) pThread, pScsiThread->role, 0, 0, 0, 0);
	    entryPt = NCR710_SCRIPT_TGT_DISCONNECT;
	    break;
	}

    /*
     *	Resume thread iff it is still connected
     */
    switch (pScsiThread->state)
	{
	case SCSI_THREAD_INACTIVE:
	case SCSI_THREAD_WAITING:
	case SCSI_THREAD_DISCONNECTED:
	    break;

	default:
    	    ncr710ThreadUpdate (pThread);

	    if (ncr710Resume (pSiop, pThread, entryPt) != OK)
	    	{
	    	SCSI_ERROR_MSG ("ncr710ThreadEvent: failed to resume thread\n",
			    	0, 0, 0, 0, 0, 0);
	
		ncr710ThreadFail (pThread, S_scsiLib_DISCONNECTED);
	    	}
	    break;
	}
    }

/*******************************************************************************
* ncr710InitEvent - NCR 53C710 initiator thread event processing routine
*
* Parse the event type and handle it accordingly.  This may result in state
* changes for the thread, state variables being updated, etc.
*
* RETURNS: N/A
*
* NOMANUAL
*/
LOCAL void ncr710InitEvent
    (
    NCR710_THREAD * pThread,
    NCR710_EVENT *  pEvent
    )
    {
    SCSI_EVENT *  pScsiEvent  = (SCSI_EVENT *)  pEvent;
    SCSI_THREAD * pScsiThread = (SCSI_THREAD *) pThread;
    SCSI_CTRL *   pScsiCtrl   = pScsiThread->pScsiCtrl;

    /*
     *  Update controller msg in/out state after script completes
     */
    pScsiCtrl->msgOutState = pThread->nMsgOutState;
    pScsiCtrl->msgInState  = pThread->nMsgInState;
    
    /*
     *	Parse script exit status; handle as necessary
     */
    switch (pScsiEvent->type)
	{
	case NCR710_DISCONNECTED:
    	    SCSI_DEBUG_MSG ("DISCONNECT message in\n", 0, 0, 0, 0, 0, 0);

	    scsiMgrThreadEvent (pScsiThread, SCSI_THREAD_EVENT_DISCONNECTED);

	    ncr710ThreadStateSet (pThread, SCSI_THREAD_DISCONNECTED);
	    break;

	case NCR710_CMD_COMPLETE:
	    SCSI_DEBUG_MSG ("COMMAND COMPLETE message in\n", 0, 0, 0, 0, 0, 0);

	    ncr710ThreadComplete (pThread);
	    break;

	case NCR710_SELECTED:
	case NCR710_RESELECTED:
    	    SCSI_DEBUG_MSG ("ncr710InitEvent: thread 0x%08x: (re)selection.\n",
		    	    (int) pThread, 0, 0, 0, 0, 0);

	    ncr710ThreadDefer (pThread);
	    break;

    	case NCR710_MESSAGE_OUT_SENT:
	    (void) scsiMsgOutComplete (pScsiCtrl, pScsiThread);
	    break;
	    
	case NCR710_MESSAGE_IN_RECVD:
	    (void) scsiMsgInComplete (pScsiCtrl, pScsiThread);
	    break;
	    
	case NCR710_NO_MSG_OUT:
	    /*
	     *	The target has requested a message out when there is none
	     *	pending.  Set up a NO-OP message to be sent when thread is
	     *  resumed.
	     *
	     *	The script could handle this on its own, but arguably the
	     *	host should be involved as it may represent an error.
	     */
	    pScsiCtrl->msgOutBuf[0] = SCSI_MSG_NO_OP;
	    pScsiCtrl->msgOutLength = 1;
	    pScsiCtrl->msgOutState  = SCSI_MSG_OUT_NONE;    /* sic */
	    break;
	    
	case NCR710_EXT_MESSAGE_SIZE:
	    /*
	     *  The SIOP has just read the length byte for an extended
	     *  message in.  The shared memory area is updated with the
	     *	appropriate length just before the thread is resumed (see
	     *	"ncr710ThreadUpdate()".
	     */
	    /* assert (pScsiCtrl->msgInState == SCSI_MSG_IN_EXT_MSG_DATA); */
	    break;
	    
	case NCR710_PHASE_MISMATCH:
	    if (ncr710PhaseMismatch (pThread,
				     pThread->nBusPhase,
				     pEvent->remCount) != OK)
		{
		ncr710ThreadFail (pThread, errno);
		}
	    break;

	case NCR710_SCSI_TIMEOUT:
	    SCSI_ERROR_MSG ("ncr710InitEvent: thread 0x%08x: select timeout.\n",
			    (int) pThread, 0, 0, 0, 0, 0);

	    ncr710ThreadFail (pThread, S_scsiLib_SELECT_TIMEOUT);
	    break;

	case NCR710_SCRIPT_ABORTED:
	    SCSI_DEBUG_MSG ("ncr710InitEvent: thread 0x%08x: aborted\n",
			    (int) pThread, 0, 0, 0, 0, 0);
	    break;
	    
	case NCR710_SCSI_BUS_RESET:
    	    SCSI_DEBUG_MSG ("ncr710InitEvent: thread 0x%08x: bus reset\n",
		    	    (int) pThread, 0, 0, 0, 0, 0);
	    /*
	     *	Do not try to resume this thread.  SCSI mgr will tidy up.
	     */
	    ncr710ThreadStateSet (pThread, SCSI_THREAD_INACTIVE);
	    break;

	case NCR710_UNEXPECTED_DISCON:
	    /* not really unexpected after an abort message ... */
    	    SCSI_ERROR_MSG ("ncr710InitEvent: thread 0x%08x: "
		    	    "unexpected disconnection\n",
		    	    (int) pThread, 0, 0, 0, 0, 0);

	    ncr710ThreadFail (pThread, S_scsiLib_DISCONNECTED);
	    break;
	    
	case NCR710_ILLEGAL_PHASE:
	    SCSI_ERROR_MSG ("ncr710InitEvent: thread 0x%08x: "
			    "illegal phase requested.\n",
			    (int) pThread, 0, 0, 0, 0, 0);

	    ncr710ThreadFail (pThread, S_scsiLib_INVALID_PHASE);
	    break;

	default:
	    logMsg ("ncr710InitEvent: invalid event type (%d)\n",
		    pScsiEvent->type, 0, 0, 0, 0, 0);
	    break;
	}
    }

    
/*******************************************************************************
* ncr710InitIdentEvent - NCR 53C710 identification thread event processing routine
*
* Parse the event type and handle it accordingly.  This may result in state
* changes for the thread, state variables being updated, etc.
*
* RETURNS: N/A
*
* NOMANUAL
*/
LOCAL void ncr710InitIdentEvent
    (
    NCR710_THREAD * pThread,
    NCR710_EVENT *  pEvent
    )
    {
    SCSI_EVENT *  pScsiEvent  = (SCSI_EVENT *)  pEvent;
    SCSI_THREAD * pScsiThread = (SCSI_THREAD *) pThread;
    SCSI_CTRL *   pScsiCtrl   = pScsiThread->pScsiCtrl;

    /*
     *  Update controller msg in/out state after script completes
     */
    pScsiCtrl->msgOutState = pThread->nMsgOutState;
    pScsiCtrl->msgInState  = pThread->nMsgInState;
    
    /*
     *	Parse script exit status; handle as necessary
     */
    switch (pScsiEvent->type)
	{
	case NCR710_RESELECTED:
    	    pScsiThread->nBytesIdent = pScsiEvent->nBytesIdent;

	    bcopy ((char *) pScsiCtrl->identBuf,
		   (char *) pScsiThread->identMsg,
		   pScsiThread->nBytesIdent);

	    ncr710ThreadStateSet (pThread, SCSI_THREAD_IDENT_IN);

    	    ncr710IdentInContinue (pThread);
	    break;

	case NCR710_MESSAGE_OUT_SENT:
	    /*
	     *	This will be after we have sent an "ABORT (TAG)" msg.
	     *	The target will disconnect any time; it may have already
	     *	done so, in which case we won't be able to resume the
	     *	thread, but no matter.
	     */
	    break;

	case NCR710_MESSAGE_IN_RECVD:
	    /*
	     *  Continue parsing the identification message.  It
	     *  should by now be complete.
	     */
	    /*
	     *	First byte of ident msg is already in ident buffer.
	     *	Remaining bytes are in the normal message input buffer.
	     *	This should always be a two-byte message (viz. QUEUE TAG);
	     *	it would be nicer if there were a way to avoid hard-coding
	     *	this.
	     */
	    bcopy ((char *) pScsiCtrl->msgInBuf,
		   (char *) pScsiThread->identMsg + pScsiThread->nBytesIdent,
		   2);
	    
    	    pScsiThread->nBytesIdent += 2;

    	    ncr710IdentInContinue (pThread);
	    break;

	case NCR710_SCRIPT_ABORTED:
	    SCSI_DEBUG_MSG ("ncr710InitIdentEvent: thread 0x%08x: aborted\n",
			    (int) pThread, 0, 0, 0, 0, 0);
	    break;

        case NCR710_DISCONNECTED:
	    SCSI_DEBUG_MSG ("ncr710InitIdentEvent: thread 0x%08x: disconnected\n",
			    (int) pThread, 0, 0, 0, 0, 0);

	    ncr710ThreadFail (pThread, S_scsiLib_DISCONNECTED);
	    break;
	    
	case NCR710_SCSI_BUS_RESET:
    	    SCSI_DEBUG_MSG ("ncr710InitIdentEvent: thread 0x%08x: bus reset\n",
		    	    (int) pThread, 0, 0, 0, 0, 0);
	    
	    /*
	     *	Do not try to resume this thread.  SCSI mgr will tidy up.
	     */
	    ncr710ThreadStateSet (pThread, SCSI_THREAD_INACTIVE);
	    break;

	case NCR710_UNEXPECTED_DISCON:
	    /* not really unexpected after an abort message ... */
    	    SCSI_ERROR_MSG ("ncr710InitIdentEvent: thread 0x%08x: "
		    	    "unexpected disconnection\n",
		    	    (int) pThread, 0, 0, 0, 0, 0);

	    ncr710ThreadFail (pThread, S_scsiLib_DISCONNECTED);
	    break;
	    
	case NCR710_ILLEGAL_PHASE:
	    SCSI_ERROR_MSG ("ncr710InitIdentEvent: thread 0x%08x: "
			    "illegal phase requested.\n",
			    (int) pThread, 0, 0, 0, 0, 0);
	    
	    ncr710ThreadFail (pThread, S_scsiLib_INVALID_PHASE);
	    break;

	default:
	    logMsg ("ncr710InitIdentEvent: invalid event type (%d)\n",
		    pScsiEvent->type, 0, 0, 0, 0, 0);
	    break;
	}
    }

    
/*******************************************************************************
* ncr710TargIdentEvent - NCR 53C710 identification thread event processing routine
*
* Parse the event type and handle it accordingly.  This may result in state
* changes for the thread, state variables being updated, etc.
*
* RETURNS: N/A
*
* NOMANUAL
*/
LOCAL void ncr710TargIdentEvent
    (
    NCR710_THREAD * pThread,
    NCR710_EVENT *  pEvent
    )
    {
    SCSI_EVENT *  pScsiEvent  = (SCSI_EVENT *)  pEvent;
    SCSI_THREAD * pScsiThread = (SCSI_THREAD *) pThread;
    SCSI_CTRL *   pScsiCtrl   = pScsiThread->pScsiCtrl;

    /*
     *  Update controller msg in/out state after script completes
     */
    pScsiCtrl->msgOutState = pThread->nMsgOutState;
    pScsiCtrl->msgInState  = pThread->nMsgInState;
    
    /*
     *	Parse script exit status; handle as necessary
     */
    switch (pScsiEvent->type)
	{
	case NCR710_SELECTED:
    	    pScsiThread->nBytesIdent = pScsiEvent->nBytesIdent;

	    bcopy ((char *) pScsiCtrl->identBuf,
		   (char *) pScsiThread->identMsg,
		   pScsiThread->nBytesIdent);

	    ncr710ThreadStateSet (pThread, SCSI_THREAD_IDENT_IN);
	    break;
	    
        case NCR710_DISCONNECTED:
	    SCSI_DEBUG_MSG ("ncr710TargIdentEvent: thread 0x%08x: disconnected\n",
			    (int) pThread, 0, 0, 0, 0, 0);

	    ncr710ThreadStateSet (pThread, SCSI_THREAD_INACTIVE);
	    break;
	    
	case NCR710_SCSI_BUS_RESET:
    	    SCSI_DEBUG_MSG ("ncr710TargIdentEvent: thread 0x%08x: bus reset\n",
		    	    (int) pThread, 0, 0, 0, 0, 0);
	    
	    /*
	     *	Do not try to resume this thread.  SCSI mgr will tidy up.
	     */
	    ncr710ThreadStateSet (pThread, SCSI_THREAD_INACTIVE);
	    break;

	case NCR710_UNEXPECTED_DISCON:
    	    SCSI_ERROR_MSG ("ncr710TargIdentEvent: thread 0x%08x: "
		    	    "unexpected disconnection\n",
		    	    (int) pThread, 0, 0, 0, 0, 0);

	    ncr710ThreadStateSet (pThread, SCSI_THREAD_INACTIVE);
	    break;
	    
	default:
	    logMsg ("ncr710TargIdentEvent: invalid event type (%d)\n",
		    pScsiEvent->type, 0, 0, 0, 0, 0);
	    break;
	}

    }

    
/*******************************************************************************
*
* ncr710PhaseMismatch - recover from a SCSI bus phase mismatch
*
* This routine does whatever is required to keep the pointers, counts, etc.
* used by task-level software in step when a SCSI phase mismatch occurs.
*
* The interrupt-level mismatch processing has stored the phase of the
* information transfer before the mismatch, and the number of bytes
* remaining to be transferred.  See "ncr710RemainderGet()".
*
* Note that the only phase mismatches supported at this level are:
*
* 1)	during data in/out phases - presumably because the target has
*   	transferred as much data as it intends to before sending a message
*   	in (typically DISCONNECT or COMMAND COMPLETE).  Recovery consists
*   	of updating the active data pointer/count according to the number
*   	of data bytes actually transferred before the mismatch.
*
* 2)	during a message out phase - presumably because the target does not
*   	understand our outgoing message and is sending a MESSAGE REJECT
*   	message, or similar.  No recovery is needed here - it's all done
*   	when the MESSAGE REJECT message has been received (see routine
*   	"scsiMsgOutReject()").
*
* 3)	during a message in phase - presumably because we have asserted ATN
*   	to abort or reject an incoming message.  No recovery is needed here -
*   	it's done by the thread management code, which should have enough
*   	state information to know what to do.
*
* RETURNS: OK, or ERROR for an unsupported or invalid phase
*
* NOMANUAL
*/
LOCAL STATUS ncr710PhaseMismatch
    (
    NCR710_THREAD  * pThread,		/* ptr to thread info           */
    int              phase,		/* bus phase before mismatch    */
    UINT             remCount		/* # bytes not yet transferred  */
    )
    {
    SCSI_THREAD *pScsiThread = (SCSI_THREAD *) pThread;
    UINT xferCount;
    
    switch (phase)
	{
	case SCSI_DATA_IN_PHASE:
	case SCSI_DATA_OUT_PHASE:
	    xferCount = pThread->pShMem->data.size - remCount;
	    
	    pScsiThread->activeDataAddress += xferCount;
	    pScsiThread->activeDataLength  -= xferCount;
	    
	    SCSI_DEBUG_MSG ("ncr710PhaseMismatch: data transfer aborted "
			    "(%d of %d bytes transferred).\n",
			    xferCount, pThread->pShMem->data.size, 0, 0, 0, 0);
	    break;
	    
	case SCSI_MSG_OUT_PHASE:
	    SCSI_DEBUG_MSG("ncr710PhaseMismatch: message out aborted "
			   "(%d of %d bytes sent).\n",
			   pScsiThread->pScsiCtrl->msgOutLength,
			   pScsiThread->pScsiCtrl->msgOutLength - remCount,
			   0, 0, 0, 0);
            /* Abort the Out message */
            pScsiThread->pScsiCtrl->msgOutLength = 0;
            pScsiThread->pScsiCtrl->msgOutState = SCSI_MSG_OUT_NONE;
            ncr710ResetATN(pScsiThread->pScsiCtrl);

	    break;
	    
	case SCSI_MSG_IN_PHASE:
	    SCSI_DEBUG_MSG("ncr710PhaseMismatch: message in aborted "
			   "(%d bytes received).\n",
			   pScsiThread->pScsiCtrl->msgInLength,
			   0, 0, 0, 0, 0);
	    break;

	case SCSI_COMMAND_PHASE:
	case SCSI_STATUS_PHASE:
	    SCSI_ERROR_MSG ("ncr710PhaseMismatch: unsupported phase (%d).\n",
			    phase, 0, 0, 0, 0, 0);
	    return (ERROR);
	    
	default:
	    logMsg ("ncr710PhaseMismatch: invalid phase (%d).\n",
		    phase, 0, 0, 0, 0, 0);
	    return (ERROR);
        }

    return (OK);
    }


/*******************************************************************************
*
* ncr710IdentInContinue - continue incoming identification
*
* Parse the message built up so far.  If it is not yet complete, do nothing.
* If the message is complete, attempt to reconnect the thread it identifies,
* and deactivate this thread (the identification thread is no longer active).
* Otherwise (identification has failed), abort the identification sequence.
*
* NOTE:
* This is almost entirely generic code.  It would be nice if the non-hardware
* specific parts could be provided as a standard routine by the SCSI library.
*
* RETURNS: N/A
*
* NOMANUAL
*/
LOCAL void ncr710IdentInContinue
    (
    NCR710_THREAD * pThread
    )
    {
    SCSI_THREAD *     pNewThread;
    SCSI_THREAD *     pScsiThread = (SCSI_THREAD *) pThread;
    SCSI_CTRL *       pScsiCtrl   = pScsiThread->pScsiCtrl;
    SCSI_IDENT_STATUS status;
    SCSI_THREAD_STATE state;

    status = scsiIdentMsgParse (pScsiCtrl, pScsiThread->identMsg,
			                   pScsiThread->nBytesIdent,
			       	          &pScsiThread->pScsiPhysDev,
				    	  &pScsiThread->tagNumber);

    switch (status)
	{
	case SCSI_IDENT_INCOMPLETE:
	    state = SCSI_THREAD_IDENT_IN;
	    break;

	case SCSI_IDENT_COMPLETE:
	    scsiMgrThreadEvent (pScsiThread, SCSI_THREAD_EVENT_RECONNECTED);

	    if ((pNewThread = scsiMgrPhysDevActiveThreadFind (
				    	    	pScsiThread->pScsiPhysDev,
				    	    	pScsiThread->tagNumber)) == 0)
		{
		state = SCSI_THREAD_IDENT_ABORTING;
		}
	    else
		{
	    	ncr710ThreadReconnect ((NCR710_THREAD *) pNewThread);

	    	state = SCSI_THREAD_INACTIVE;
		}
	    break;

	case SCSI_IDENT_FAILED:
	    state = SCSI_THREAD_IDENT_ABORTING;
	    break;

	default:
	    logMsg ("ncr710IdentInContinue: invalid ident status (%d)\n",
    	    	    status, 0, 0, 0, 0, 0);
	    state = SCSI_THREAD_INACTIVE;
	    break;
	}

    if (state == SCSI_THREAD_IDENT_ABORTING)
	ncr710ThreadAbort ((SIOP *) pScsiCtrl, pThread);

    ncr710ThreadStateSet (pThread, state);
    }


/*******************************************************************************
*
* ncr710ThreadReconnect - reconnect a thread
*
* Restore the SCSI pointers for the thread (this really should be in a more
* generic section of code - perhaps part of the SCSI manager's thread event
* procesing ?).  Update the newly-connected thread's context (including
* shared memory area) and resume it.  Set the thread's state to ESTABLISHED.
*
* RETURNS: N/A
*
* NOMANUAL
*/
LOCAL void ncr710ThreadReconnect
    (
    NCR710_THREAD * pThread
    )
    {
    SCSI_THREAD * pScsiThread = (SCSI_THREAD *) pThread;
    SCSI_CTRL   * pScsiCtrl   = pScsiThread->pScsiCtrl;
    SIOP        * pSiop       = (SIOP *) pScsiCtrl;
    
    SCSI_DEBUG_MSG ("ncr710ThreadReconnect: reconnecting thread 0x%08x\n",
		    (int) pThread, 0, 0, 0, 0, 0);

    pScsiCtrl->pThread = pScsiThread;

    /* Implied RESTORE POINTERS action: see "scsiMsgInComplete ()" */

    pScsiThread->activeDataAddress = pScsiThread->savedDataAddress;
    pScsiThread->activeDataLength  = pScsiThread->savedDataLength;

    ncr710ThreadUpdate (pThread);

    if (ncr710Resume (pSiop, pThread, NCR710_SCRIPT_INIT_CONTINUE) != OK)
	{
	SCSI_ERROR_MSG ("ncr710ThreadReconnect: failed to resume thread.\n",
			0, 0, 0, 0, 0, 0);

	ncr710ThreadFail (pThread, S_scsiLib_DISCONNECTED);
	return;
	}

    ncr710ThreadStateSet (pThread, SCSI_THREAD_ESTABLISHED);
    }


/*******************************************************************************
* ncr710SharedMemInit - initialize the fields in a shared memory area
*
* Initialise pointers and counts for all message transfers.  These are
* always directed to buffers provided by the SCSI_CTRL structure.
*
* RETURNS: N/A
*
* NOMANUAL
*/
LOCAL void ncr710SharedMemInit
    (
    SIOP *          pSiop,
    NCR710_SHARED * pShMem
    )
    {
    pShMem->identIn.size = 1;
    pShMem->identIn.addr = CACHE_DMA_VIRT_TO_PHYS (pSiop->scsiCtrl.identBuf);

    pShMem->msgOut.size = 0;		/* set dynamically */
    pShMem->msgOut.addr = CACHE_DMA_VIRT_TO_PHYS (pSiop->scsiCtrl.msgOutBuf);
    
    pShMem->msgIn.size  = 1;
    pShMem->msgIn.addr  = CACHE_DMA_VIRT_TO_PHYS (pSiop->scsiCtrl.msgInBuf);

    pShMem->msgInSecond.size = 1;
    pShMem->msgInSecond.addr = pShMem->msgIn.addr + 1;

    pShMem->msgInRest.size   = 0;  	/* set dynamically */
    pShMem->msgInRest.addr   = pShMem->msgIn.addr + 2;
    }


/*******************************************************************************
*
* ncr710ThreadInit - initialize a client thread structure
*
* Initialize the fixed data for a thread (i.e., independent of the command).
* Called once when a thread structure is first created.
*
* RETURNS: OK, or ERROR if an error occurs
*
* NOMANUAL
*/

LOCAL STATUS ncr710ThreadInit
    (
    SIOP *          pSiop,
    NCR710_THREAD * pThread
    )
    {
    if (scsiThreadInit (&pThread->scsiThread) != OK)
	return (ERROR);
    
    pThread->pShMem = pSiop->pClientShMem;

    return (OK);
    }

    
/*******************************************************************************
*
* ncr710IdentThreadInit - initialize an identification thread structure
*
* Set up pointers and counts for all buffers other than messages.  Also set
* transfer parameters for asynchronous mode, and update the shared memory
* area to match this thread.
*
* NOTE:
* The pointers/counts set here are normally never used by the identification
* thread; however, when an identification thread is aborted, it's possible
* that the target will attempt to transfer one or more non-message bytes
* before requesting the abort message out.  To handle this, the thread's
* pointers and counts all specify a dummy buffer.  There is no re-entrancy
* problem here because we do not care what data is transferred.  This might
* be handled more elegantly if there were a special script entry to abort
* an identification thread.
*
* RETURNS: OK, or ERROR if an error occurs
*
* NOMANUAL
*/

LOCAL STATUS ncr710IdentThreadInit
    (
    NCR710_THREAD * pThread
    )
    {
    SCSI_THREAD * pScsiThread = (SCSI_THREAD *) pThread;
    SIOP *        pSiop       = (SIOP *) pScsiThread->pScsiCtrl;
    static UINT8  dummy;
    
    /*
     *	Initialise Pointers and counts in thread
     */
    pScsiThread->cmdLength         = 1;
    pScsiThread->cmdAddress        = &dummy;
    pScsiThread->dataLength        = 1;
    pScsiThread->dataAddress       = &dummy;
    pScsiThread->activeDataLength  = 1;
    pScsiThread->activeDataAddress = &dummy;
    pScsiThread->savedDataLength   = 1;
    pScsiThread->savedDataAddress  = &dummy;
    pScsiThread->statusLength      = 1;
    pScsiThread->statusAddress     = &dummy;
    pScsiThread->identMsgLength    = 1;
    
    /*
     *	Initialise SIOP register context in thread
     */
    pThread->nHostFlags   = 0;
    pThread->nMsgOutState = SCSI_MSG_OUT_NONE;
    pThread->nMsgInState  = SCSI_MSG_IN_NONE;

    pThread->sxfer = NCR710_SYNC_XFER_PARAMS_ASYNC;
    pThread->sbcl  = NCR710_CLOCK_DIVIDE_ASYNC;

    /*
     *	Initialise shared memory area
     */
    pThread->pShMem = pSiop->pIdentShMem;
    
    ncr710SharedMemInit (pSiop, pThread->pShMem);

    ncr710ThreadUpdate (pThread);
    
    return (OK);
    }


/*******************************************************************************
*
* ncr710ThreadUpdate - update the thread structure for a current SCSI command
*
* Update the dynamic data (e.g. data pointers, transfer parameters) in
* the thread to reflect the latest state of the corresponding physical device.
*
* RETURNS: N/A
*
* NOMANUAL
*/

LOCAL void ncr710ThreadUpdate
    (
    NCR710_THREAD * pThread		/* thread info */
    )
    {
    SCSI_THREAD   * pScsiThread = (SCSI_THREAD *) pThread;
    SCSI_CTRL     * pScsiCtrl   = pScsiThread->pScsiCtrl;
    SIOP          * pSiop       = (SIOP *) pScsiCtrl;
    NCR710_SHARED * pShMem      = pThread->pShMem;
    UINT            flags       = 0;
    UINT            msgOutSize;
    UINT            msgInSize;

    /*
     *  If there is an identification message, ensure ATN is asserted
     *	during (re)selection.
     */
    if (pScsiThread->identMsgLength != 0)
	flags |= FLAGS_IDENTIFY;

    /*
     *	Update SIOP register context
     */
    pThread->nHostFlags   = flags;
    pThread->nMsgOutState = pScsiCtrl->msgOutState;
    pThread->nMsgInState  = pScsiCtrl->msgInState;

    /*
     *	Update dynamic message in/out sizes
     */
    if (pScsiCtrl->msgOutState == SCSI_MSG_OUT_NONE)
	msgOutSize = 0;
    else
	msgOutSize = pScsiCtrl->msgOutLength;

    if (pScsiCtrl->msgInState != SCSI_MSG_IN_EXT_MSG_DATA)
	msgInSize = 0;
    
    else if ((msgInSize = pScsiCtrl->msgInBuf[SCSI_EXT_MSG_LENGTH_BYTE]) == 0)
	msgInSize = SCSI_EXT_MSG_MAX_LENGTH;

    /*
     *	Update shared memory area
     */
    pShMem->command.size  = pScsiThread->cmdLength;
    pShMem->command.addr  = pScsiThread->cmdAddress;
    pShMem->data.size     = pScsiThread->activeDataLength;
    pShMem->data.addr     = pScsiThread->activeDataAddress;
    pShMem->status.size   = pScsiThread->statusLength;
    pShMem->status.addr   = pScsiThread->statusAddress;

    /* These used to be pScsiThread->identMsg/identMsgLength */
    pShMem->identOut.size = pSiop->identMsgLength;
    pShMem->identOut.addr = pSiop->identMsg;

    pShMem->msgOut.size    = msgOutSize;
    pShMem->msgInRest.size = msgInSize;
    }

/*******************************************************************************
*
* ncr710ThreadComplete - successfully complete execution of a client thread
*
* Set the thread status and errno appropriately, depending on whether or
* not the thread has been aborted.  Set the thread inactive, and notify
* the SCSI manager of the completion.
*
* RETURNS: N/A
*
* NOMANUAL
*/
LOCAL void ncr710ThreadComplete
    (
    NCR710_THREAD * pThread
    )
    {
    SCSI_THREAD * pScsiThread = (SCSI_THREAD *) pThread;
    
    SCSI_DEBUG_MSG ("ncr710ThreadComplete: thread 0x%08x completed\n",
		    (int) pThread, 0, 0, 0, 0, 0);

    if (pScsiThread->state == SCSI_THREAD_ABORTING)
	{
	pScsiThread->status = ERROR;
	pScsiThread->errNum = S_scsiLib_ABORTED;
	}
    else
	{
    	pScsiThread->status = OK;
    	pScsiThread->errNum = 0;
    	}
    
    ncr710ThreadStateSet (pThread, SCSI_THREAD_INACTIVE);

    scsiCacheSynchronize (pScsiThread, SCSI_CACHE_POST_COMMAND);

    scsiMgrThreadEvent (pScsiThread, SCSI_THREAD_EVENT_COMPLETED);
    }


/*******************************************************************************
*
* ncr710ThreadDefer - defer execution of a thread
*
* Set the thread's state to INACTIVE and notify the SCSI manager of the
* deferral event.
*
* RETURNS: N/A
*
* NOMANUAL
*/
LOCAL void ncr710ThreadDefer
    (
    NCR710_THREAD * pThread
    )
    {
    SCSI_THREAD * pScsiThread = (SCSI_THREAD *) pThread;
    
    SCSI_DEBUG_MSG ("ncr710ThreadDefer: thread 0x%08x deferred\n",
		    (int) pThread, 0, 0, 0, 0, 0);

    ncr710ThreadStateSet (pThread, SCSI_THREAD_INACTIVE);

    scsiMgrThreadEvent (pScsiThread, SCSI_THREAD_EVENT_DEFERRED);
    }
    
	
/*******************************************************************************
*
* ncr710ThreadFail - complete execution of a thread, with error status
*
* Set the thread's status and errno according to the type of error.  Set
* the thread's state to INACTIVE, and notify the SCSI manager of the
* completion event.
*
* RETURNS: N/A
*
* NOMANUAL
*/
LOCAL void ncr710ThreadFail
    (
    NCR710_THREAD * pThread,
    int             errNum
    )
    {
    SCSI_THREAD * pScsiThread = (SCSI_THREAD *) pThread;
    
    SCSI_DEBUG_MSG ("ncr710ThreadFail: thread 0x%08x failed (errno = %d)\n",
		    (int) pThread, errNum, 0, 0, 0, 0);

    pScsiThread->status = ERROR;

    if (pScsiThread->state == SCSI_THREAD_ABORTING)
	pScsiThread->errNum = S_scsiLib_ABORTED;

    else
    	pScsiThread->errNum = errNum;
    
    ncr710ThreadStateSet (pThread, SCSI_THREAD_INACTIVE);

    scsiMgrThreadEvent (pScsiThread, SCSI_THREAD_EVENT_COMPLETED);
    }
    
	
/******************************************************************************
*
* ncr710ThreadStateSet - set the state of a thread
*
* This is really just a place-holder for debugging and possible future
* enhancements such as state-change logging.
*
* RETURNS: N/A
*
* NOMANUAL
*/
LOCAL void ncr710ThreadStateSet
    (
    NCR710_THREAD *   pThread,		/* ptr to thread info */
    SCSI_THREAD_STATE state
    )
    {
    SCSI_THREAD * pScsiThread = (SCSI_THREAD *) pThread;
    
    SCSI_DEBUG_MSG ("ncr710ThreadStateSet: thread 0x%08x: %d -> %d\n",
		    (int) pThread, pScsiThread->state, state, 0, 0, 0);

    pScsiThread->state = state;
    }


/******************************************************************************
*
* ncr710Activate - activate a script corresponding to a new thread
*
* Request activation of (the script for) a new thread, if possible; do not
* wait for the script to complete (or even start) executing.  Activation
* is requested by signalling the controller, which causes an interrupt.
* The script is started by the ISR in response to this event.
*
* There is a race condition inherent in SCSI between the synchronous
* activation of a thread (i.e., execution of this routine) and asynchronous
* activation of a thread as a result of selection or reselection.  The SCSI
* manager is designed to take account of this, and assumes that either the
* new thread has been successfully activated or a (re)selection has taken
* place, as determined by the next event it receives.  In the case when
* (re)selection occurs, the current activation request is deferred and
* retried later.
*
* Therefore, an activation request is lodged only if the controller is
* currently PASSIVE (waiting for (re)selection or a host command).  If the
* controller is IDLE, (re)selection is assumed to have already occurred.
* Note that this is not an error condition; there is no way for the
* caller to distinguish the two cases, because even if the thread appears
* to have been activated it may yet be "pre-empted" by (re)selection.
*
* The controller should never be ACTIVE.  If it is, the SCSI manager has
* tried to activate multiple concurrent threads on the controller, which
* indicates a major (software) disaster.
*
* NOTE: Interrupt locking is required to ensure that the correct action
* is taken once the controller state has been checked.
*
* RETURNS: OK, or ERROR if the controller is in an invalid state (this
* indicates a major software failure).
*
* NOMANUAL
*/
LOCAL STATUS ncr710Activate
    (
    SIOP *          pSiop,
    NCR710_THREAD * pThread
    )
    {
    STATUS status;
    int    key;

    if (pSiop->cmdPending)
	{
	logMsg ("ncr710Activate: activation request already pending !\n",
		0, 0, 0, 0, 0, 0);
	return (ERROR);
	}
    
    /*
     *	Check controller state, set a new command pending and signal it
     *  if necessary.
     */
    key = intLock ();

    switch (pSiop->state)
	{
	case NCR710_STATE_IDLE:
	    status = OK;
	    break;

	case NCR710_STATE_PASSIVE:
	    pSiop->pNewThread = pThread;
	    pSiop->cmdPending = TRUE;

	    *pSiop->pIstat |= B_SIGP;

	    status = OK;
	    break;
	    
	case NCR710_STATE_ACTIVE:
	default:
	    status = ERROR;
	    break;
	}

    intUnlock (key);

    if (status != OK)
	{
	logMsg ("ncr710Activate: invalid controller state.\n",
		pSiop->state, 0, 0, 0, 0, 0);

	errnoSet (S_scsiLib_SOFTWARE_ERROR);
	}

    return (status);
    }

/*******************************************************************************
* ncr710Resume - resume a script corresponding to a suspended thread
*
* NOTE: the script can only be resumed if the controller is currently idle.
* To avoid races, interrupts must be locked while this is checked and the
* script re-started.
*
* Reasons why the controller might not be idle include SCSI bus reset and
* unexpected disconnection, both of which might occur in practice.  Hence
* this is not considered to be a major software error.
*
* RETURNS: OK, or ERROR if the controller is in an invalid state (this
* should not be treated as a major software failure).
*
* NOMANUAL
*/

LOCAL STATUS ncr710Resume
    (
    SIOP *              pSiop,		/* ptr to controller info          */
    NCR710_THREAD *     pThread,	/* ptr to thread info              */
    NCR710_SCRIPT_ENTRY entryId		/* entry point of script to resume */
    )
    {
    STATUS status;
    int    key;

    /*
     *	Check validity of connection and start script if OK
     */
    key = intLock ();

    switch (pSiop->state)
	{
	case NCR710_STATE_IDLE:
	    SCSI_INT_DEBUG_MSG ("ncr710Resume: thread: 0x%08x: state %d -> %d\n",
			    	(int) pThread,
			    	NCR710_STATE_IDLE, NCR710_STATE_ACTIVE,
			    	0, 0, 0);

	    ncr710ScriptStart (pSiop, pThread, entryId);

	    pSiop->state = NCR710_STATE_ACTIVE;
	    status = OK;
	    break;

	case NCR710_STATE_PASSIVE:
	case NCR710_STATE_ACTIVE:
	default:
	    status = ERROR;
	    break;
	}

    intUnlock (key);

    return (status);
    }

/*******************************************************************************
* ncr710Abort - abort the active script corresponding to the current thread
*
* Check that there is currently an active script running.  If so, set the
* SIOP's Abort flag which will halt the script and cause an interrupt.
*
* RETURNS: N/A
*
* NOMANUAL
*/

LOCAL void ncr710Abort
    (
    SIOP * pSiop			/* ptr to controller info */
    )
    {
    STATUS status;
    int    key;

    key = intLock ();

    switch (pSiop->state)
	{
	case NCR710_STATE_IDLE:
	case NCR710_STATE_PASSIVE:
	    status = OK;
	    break;

	case NCR710_STATE_ACTIVE:
	    *pSiop->pIstat |= B_ABORT;
	    status = OK;
	    break;
	    
	default:
	    status = ERROR;
	    break;
	}

    intUnlock (key);

    if (status != OK)
	{
	logMsg ("ncr710Abort: thread = 0x%08x: invalid state (%d)\n",
		pSiop->state, 0, 0, 0, 0, 0);
	}
    }

/*******************************************************************************
*
* ncr710ScriptStart - start the SIOP executing a script
*
* Restore the SIOP register context, including the shared memory area, from
* the thread context.  Put the address of the script entry point into the
* DSP register.  If not in single-step mode, start the script.
*
* NOTE: should always be called with SIOP interrupts locked.
*
* RETURNS: N/A
* 
* NOMANUAL
*/

LOCAL void ncr710ScriptStart
    (
    SIOP               *pSiop,		/* pointer to  SIOP info */
    NCR710_THREAD      *pThread,	/* ncr thread info */
    NCR710_SCRIPT_ENTRY entryId		/* routine address entry point */
    )

    {
    /*
     *	Script entry point addresses.  These are resolved by PROCs
     *	defined in the script itself (see "ncr710init.n").
     *
     *	NOTE: The number and order of members of this array _must_ match
     *	the enum NCR710_SCRIPT_ENTRY.
     */
    static ULONG * ncr710ScriptEntry [] =
	{
        ncr710Wait,			/* wait for re-select or host cmd   */
    	ncr710InitStart,		/* start an initiator thread        */
    	ncr710InitContinue,		/* continue an initiator thread     */
	ncr710TgtDisconnect, 	    	/* disconnect a target thread       */
	};    

    /*
     *	Set current hardware thread pointer for ISR
     */
    pSiop->pHwThread = pThread;

    /*
     *	Restore the SIOP register context for this thread.
     */
    *pSiop->pScratch0 = pThread->scratch0;
    *pSiop->pScratch1 = pThread->scratch1;
    *pSiop->pScratch2 = pThread->scratch2;
    *pSiop->pScratch3 = pThread->scratch3;
    
    *pSiop->pSxfer = pThread->sxfer | B_DHP;
    *pSiop->pSbcl  = pThread->sbcl;

    /*
     *	Set the shared data address, load the script start address,
     *	then start the SIOP unless it's in single-step mode.
     */
    *pSiop->pDsa = (UINT) CACHE_DMA_VIRT_TO_PHYS (pThread->pShMem);

    *pSiop->pDsp = (UINT) ncr710ScriptEntry [entryId];

    if (pSiop->singleStep)
	{
	logMsg ("ncr710ScriptStart: single-step required to start script\n",
		0, 0, 0, 0, 0, 0);
	}
    else
	{
    	*pSiop->pDcntl |= B_STD;
	}
    }


/*******************************************************************************
*
* ncr710ThreadParamsSet - set various parameters for a thread
*
* Parameters include transfer offset and period, as well as the ID of the
* target device.  All of these end up as encoded values stored either in
* the thread's register context or its associated shared memory area.
*
* Transfer period is specified in SCSI units (multiples of 4 ns).  An offset
* of zero specifies asynchronous transfer.
*
* RETURNS: OK if parameters are OK, else ERROR.
*
* NOMANUAL
*/

LOCAL STATUS ncr710ThreadParamsSet
    (
    NCR710_THREAD * pThread,	    	/* thread to be affected  */
    UINT8           offset,		/* max REQ/ACK offset     */
    UINT8           period		/* min transfer period    */
    )
    {
    SCSI_THREAD *   pScsiThread = (SCSI_THREAD *) pThread;
    SIOP *          pSiop       = (SIOP *) pScsiThread->pScsiCtrl;
    UINT            busId       = pScsiThread->pScsiTarget->scsiDevBusId;
    NCR710_SHARED * pShMem      = pThread->pShMem;

    if (!ncr710XferParamsCvt (pSiop, &offset, &period, &pThread->sxfer,
			                               &pThread->sbcl))
	{				/* should never happen */
	errnoSet (S_scsiLib_ILLEGAL_PARAMETER);
	return (ERROR);
	}

    pShMem->device = (pThread->sxfer <<  NCR710_XFER_PARAMS_SHIFT) |
                                  (1 << (NCR710_TARGET_BUS_ID_SHIFT + busId));

    return (OK);
    }

/*******************************************************************************
*
* ncr710XferParamsQuery - get (synchronous) transfer parameters
*
* Updates the synchronous transfer parameters suggested in the call to match
* the NCR 53C710's capabilities.  Transfer period is in SCSI units (multiples
* of 4 ns).
*
* Note: the transfer period is made longer and the offset is made smaller if
* the NCR 53C710 cannot handle the specified values.
*
* RETURNS: OK
*
* NOMANUAL
*/

LOCAL STATUS ncr710XferParamsQuery
    (
    SCSI_CTRL *pScsiCtrl,		/* ptr to controller info       */
    UINT8     *pOffset,			/* max REQ/ACK offset  [in/out] */
    UINT8     *pPeriod			/* min transfer period [in/out] */
    )
    {
    UINT8 unused;

    (void) ncr710XferParamsCvt ((SIOP *) pScsiCtrl, pOffset, pPeriod,
				&unused, &unused);
    
    return (OK);
    }
    
/*******************************************************************************
*
* ncr710XferParamsSet - set transfer parameters
*
* Validate the requested parameters, convert to the NCR 53C710's native
* format and save in the current thread for later use (the chip's registers
* are not actually set until the next script activation for this thread).
*
* Transfer period is specified in SCSI units (multiples of 4 ns).  An offset
* of zero specifies asynchronous transfer.
*
* RETURNS: OK if transfer parameters are OK, else ERROR.
*
* NOMANUAL
*/

LOCAL STATUS ncr710XferParamsSet
    (
    SCSI_CTRL *pScsiCtrl,		/* ptr to controller info */
    UINT8      offset,			/* max REQ/ACK offset     */
    UINT8      period			/* min transfer period    */
    )
    {
    NCR710_THREAD * pThread = (NCR710_THREAD *) pScsiCtrl->pThread;

    return (ncr710ThreadParamsSet (pThread, offset, period));
    }

/*******************************************************************************
* ncr710XferParamsCvt - convert transfer period from SCSI to NCR 53C710 units
*
* Given a "suggested" REQ/ACK offset and transfer period (in SCSI units of
* 4 ns), return the nearest offset and transfer period the NCR 53C710 is
* capable of using.  Also return the corresponding values of the NCR 53C710's
* Synchronous Transfer and Synchronous Bus Control registers.
*
* An offset of zero specifies asynchronous transfer, in which case the period
* is irrelevant.  Otherwise, the offset may be clipped to be within the
* maximum limit the NCR 53C710 is capable of.
*
* The transfer period is normally rounded towards longer periods if the NCR
* 53C710 is not capable of using the exact specified value.  The period may
* also be clipped to be within the NCR 53C710's maximum and minimum limits
* according to its clock period.
*
* If either the offset or period need to be clipped, the value FALSE is
* retuned as this may reflect an error condition.
*
* RETURNS: TRUE if input period is within NCR 53C710's range, else FALSE
*
* NOMANUAL
*/

LOCAL BOOL ncr710XferParamsCvt
    (
    FAST SIOP  *pSiop,			/* ptr to controller info            */
    FAST UINT8 *pOffset,		/* REQ/ACK offset                    */
    FAST UINT8 *pPeriod,		/* xfer period, SCSI units (x 4 ns)  */
    FAST UINT8 *pXferParams,		/* corresponding Sync Xfer Reg value */
    FAST UINT8 *pClockDiv		/* corresponding clock division bits */
    )
    {
    UINT  offset  = (UINT) *pOffset;
    UINT  period  = (UINT) *pPeriod;
    BOOL  inRange = TRUE;
    UINT8 xferParams  = 0;		/* initialised to avoid warnings */
    UINT8 clockDivide = 0;		/* initialised to avoid warnings */

    if (offset == SCSI_SYNC_XFER_ASYNC_OFFSET)
	{
	xferParams  = NCR710_SYNC_XFER_PARAMS_ASYNC;
	clockDivide = NCR710_CLOCK_DIVIDE_ASYNC;

	offset = SCSI_SYNC_XFER_ASYNC_OFFSET;
	period = 0;
    	}
    else
	{
	UINT div;
	UINT clkCycles;
    	UINT bestPeriod = (UINT) NONE;	/* i.e., not a valid SCSI period */
	
	SCSI_DEBUG_MSG ("ncr710XferParamsCvt: requested: "
			"offset = %d, period = %d\n",
		    	offset, period, 0, 0, 0, 0);

	/*
	 *  Clip period (if necessary) to be within legal SCSI limits
	 */
	if (period < SCSI_SYNC_XFER_MIN_PERIOD)
	    {
	    period  = SCSI_SYNC_XFER_MIN_PERIOD;
	    inRange = FALSE;
	    }
	
    	/*
     	 *  Clip offset (if necessary) to suit NCR 53C710
     	 */
    	if (offset > NCR710_MAX_REQ_ACK_OFFSET)
	    {
	    offset  = NCR710_MAX_REQ_ACK_OFFSET;
	    inRange = FALSE;
	    }

	/*
	 *  Find the best attainable period which is equal to or greater than
	 *  the requested period.  Basic formula for period is:
	 *
	 *  	period = clkPeriod * (4 + xferParams)
	 *
	 *  where:
	 *
	 *  	period	    = sync transfer period (ns x 100)
	 *  	clkPeriod   = sync clock    period (ns x 100)
	 *  	xferParams  = value in bits 6-4 of SCSI Xfer register (0-7)
	 *
	 *  The sync clock can have four possible values depending on the
	 *  value of the clock divisor bits in the SCSI Bus Control register
	 *  bits 1,0:
	 *
	 *  	00  - sync clock period = async clock period (set in HwInit)
	 *  	01  - sync clock period = input clock period * 1.0 (2/2)
	 *  	10  - sync clock period = input clock period * 1.5 (3/2)
	 *  	11  - sync clock period = input clock period * 2.0 (4/2)
	 *
	 *  For each sync clock period, calculate the required number of
	 *  clock cycles to achieve the requested period (or greater).
	 *  If less than the minimum possible (4), round up to the minimum.
	 *  If the result is not greater than the maximum number of clock
	 *  cycles, then the corresponding period is attainable and is
	 *  calculated.  The lowest attainable period is selected.
	 */
    	for (div = NCR710_MIN_CLOCK_DIV; div <= NCR710_MAX_CLOCK_DIV; ++div)
	    {
	    UINT clkPeriod;

	    if (div == NCR710_CLOCK_DIVIDE_ASYNC)
	    	clkPeriod = pSiop->asyncClkPeriod;
	    else
	    	clkPeriod = pSiop->clkPeriod * (div + 1) / 2;
	
	    /*
	     *  Round (and perhaps clip) transfer period to suit NCR 53C710
	     *
	     *  Note: split into two stages to avoid associativity problems,
	     *  	and careful to round up rather than truncate result.
	     */
	    clkCycles  = (period * 4 * 100) + (clkPeriod - 1);
	    clkCycles /= clkPeriod;

	    if (clkCycles < 4 + NCR710_MIN_XFER_PERIOD)
		clkCycles = 4 + NCR710_MIN_XFER_PERIOD;

	    if (clkCycles <= 4 + NCR710_MAX_XFER_PERIOD)
	    	{
	    	UINT actualPeriod = clkPeriod * clkCycles / (4 * 100);

	    	if ((actualPeriod >= period) &&
		    (actualPeriod <  bestPeriod))
	    	    {
                    bestPeriod  = actualPeriod;
    	    	    xferParams  = ((clkCycles - 4) << NCR710_SYNC_XFER_PERIOD_SHIFT)
	    	    	    	| offset;
		    clockDivide = div;
		    }
	    	}
	    }

	/*
	 *  If "bestPeriod" has been set, a valid period has been found.
	 *  Otherwise, reset to use asynchronous transfer.
	 */
    	if (bestPeriod != (UINT) NONE)
	    period = bestPeriod;

	else
            {
	    xferParams  = NCR710_SYNC_XFER_PARAMS_ASYNC;
	    clockDivide = NCR710_CLOCK_DIVIDE_ASYNC;

	    offset = SCSI_SYNC_XFER_ASYNC_OFFSET;
	    period = 0;

	    inRange = FALSE;
	    }

	SCSI_DEBUG_MSG ("ncr710XferParamsCvt: granted:   "
			"offset = %d, period = %d\n",
		    	offset, period, 0, 0, 0, 0);
    	}

    *pOffset     = offset;
    *pPeriod     = period;
    *pXferParams = xferParams;
    *pClockDiv   = clockDivide;

    return (inRange);
    }


/*******************************************************************************
*
* ncr710StepEnable2 - Enable/disable script single step 
*
* Enable/disable single step facility on ncr chip.  Also, unmask/mask single 
* step interrupt in Dien register.
* Before any SCSI execution you just have to use 
* ncr710StepEnable (pSiop, TRUE) to run in single step mode.  
* To run a script step use ncr710SingleStep(pSiop).
* To disable use ncr710StepEnable (pSiop, FALSE).
* 
* RETURNS: N/A
*
* NOMANUAL
*/

void ncr710StepEnable2
    (
    SIOP *pSiop,		/* pointer to SIOP info       */
    BOOL enable	    	    	/* TRUE => enable single-step */
    )

    {
    if  (enable)
	{
	/* enable single-step interrupt */
        *pSiop->pDien |= B_SSI;

	/* set single-step mode */
	*pSiop->pDcntl |= B_SSM;

	/* disable manual Start */
	*pSiop->pDmode &= ~B_MAN;
	}
    else 
	{
	/* unset single-step mode */
	*pSiop->pDcntl &= ~B_SSM;

	/* disable single-step interrupt */
        *pSiop->pDien &= ~B_SSI;

	/* enable manual Start */
	*pSiop->pDmode |= B_MAN;
        }

    pSiop->singleStep = enable;
    }

/*******************************************************************************
*
* ncr710SingleStep2 - Perform a single step 
*
* Perform a single step by writing the STD bit in the DCNTL register.
* The parameter is a pointer to the SIOP information. 
* 
* RETURNS: N/A
* 
* NOMANUAL
*/

void ncr710SingleStep2
    (
    SIOP * pSiop, 		/* pointer to SIOP info */ 
    BOOL   verbose		/* show all registers   */
    )

    {
    UINT *siopPc;

    /*
     *	Start the SIOP: if not in single-step mode it will then continue
     *	the normal instruction sequence.  If in single-step mode, wait
     *	until one instruction has been executed.
     */
    *pSiop->pDcntl |= B_STD;

    if (!pSiop->singleStep)
	return;

    semTake (pSiop->singleStepSem, WAIT_FOREVER);

    /*
     *	Allow any tasks using SCSI to get a look-in - they may change the
     *	controller state, in which case we'd like to see that change now
     *	rather than next time.
     */
    taskDelay (5);

    /*
     *	Show what the SIOP is about to do
     */
    if (pSiop->state == NCR710_STATE_IDLE)
	printf ("NCR710 is idle.\n\n");
    else
	{
	siopPc = (UINT *) *pSiop->pDsp;
	
	printf ("NCR710 [%d] Next Opcode (0x%08x) => 0x%08x 0x%08x\n\n",
		pSiop->state, (int) siopPc, siopPc[0], siopPc[1]);
	}
    
    /*
     *	Dump the SIOP registers, if in verbose mode
     */
    if (verbose)
    	ncr710ShowScsi2 ((SCSI_CTRL *) pSiop);
    }

/*******************************************************************************
*
* ncr710ShowScsi2 - display the values of all readable NCR 53C710 SIOP registers
*
* This routine displays the state of the NCR 53C710 SIOP registers in a 
* user-friendly way.  It is primarily used for debugging. 
* The input parameter is the pointer to the SIOP information
* structure returned by the ncr710CtrlCreateScsi2() call.
*
* NOTE
* The only readable register during a script execution is the Istat register. 
* If you use this routine during the execution of a SCSI command, the result 
* could be unpredictable.
*
* EXAMPLE:
* .CS
* -> ncr710Show
* NCR710 Registers 
* ---------------- 
* 0xfff47000: Sien    = 0xa5 Sdid    = 0x00 Scntl1  = 0x00 Scntl0  = 0x04
* 0xfff47004: Socl    = 0x00 Sodl    = 0x00 Sxfer   = 0x80 Scid    = 0x80
* 0xfff47008: Sbcl    = 0x00 Sbdl    = 0x00 Sidl    = 0x00 Sfbr    = 0x00
* 0xfff4700c: Sstat2  = 0x00 Sstat1  = 0x00 Sstat0  = 0x00 Dstat   = 0x80
* 0xfff47010: Dsa     = 0x00000000
* 0xfff47014: Ctest3  = ???? Ctest2  = 0x21 Ctest1  = 0xf0 Ctest0  = 0x00
* 0xfff47018: Ctest7  = 0x32 Ctest6  = ???? Ctest5  = 0x00 Ctest4  = 0x00
* 0xfff4701c: Temp    = 0x00000000
* 0xfff47020: Lcrc    = 0x00 Ctest8  = 0x00 Istat   = 0x00 Dfifo   = 0x00
* 0xfff47024: Dcmd/Ddc= 0x50000000
* 0xfff47028: Dnad    = 0x00066144
* 0xfff4702c: Dsp     = 0x00066144
* 0xfff47030: Dsps    = 0x00066174
* 0xfff47037: Scratch3= 0x00 Scratch2= 0x00 Scratch1= 0x00 Scratch0= 0x0a
* 0xfff47038: Dcntl   = 0x21 Dwt     = 0x00 Dien    = 0x37 Dmode   = 0x01
* 0xfff4703c: Adder   = 0x000cc2b8
* value = 0 = 0x0
* .CE
*
* RETURNS: OK, or ERROR if <pScsiCtrl> and <pSysScsiCtrl> are both NULL.
*
* SEE ALSO: ncr710CtrlCreateScsi2()
*/

STATUS ncr710ShowScsi2 
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
* ncr710ResetATN - Resets the ATN signal
*
* This routine resets the ATN line for the give controller
*
* RETURNS : N/A
*/

LOCAL void ncr710ResetATN ( FAST SCSI_CTRL *pScsiCtrl )
    {
    FAST SIOP       *pSiop;         /* prt to SIOP info */

    pSiop = (SIOP *) pScsiCtrl;
        
    *pSiop->pSocl &= (UINT8)0xf7;
    }
