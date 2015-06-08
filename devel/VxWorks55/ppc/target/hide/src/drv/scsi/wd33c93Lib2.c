/* wd33c93Lib2.c - WD33C93 SCSI-Bus Interface Controller library (SCSI-2) */

/* Copyright 1984-1999 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
02h,03mar99,dat  added init of wideXfer to FALSE. SPR 24089
02h,11jan99,aeg  added semTerminate () in wd33c93CtrlCreateScsi2 ().
02g,29oct96,dgp  doc: editing for newly published SCSI libraries
02f,06may96,jds  and more doc tweaks...
02e,01may96,jds  yet more doc tweaks...
02d,13nov95,jds  more doc tweaks
02c,20sep95,jdi  doc tweaks.
02b,20jul95,jds  changed wd33c93Intr to sbicIntr to keep it consistent with
		 previous incarnations of this driver. Integrated into 
		 vxWorks5.2
03a,16mar95,ihw  major modifications to work with generic SCSI thread manager
02g,14jan94,ihw  ANSI-fied and generally smartened up to v5.1 standard
02f,19jul92,ihw  improved support for disconnect/reconnect
02e,17jul92,ihw  added sync. points in sbicBytes{In,Out}()
02d,01jul92,ihw  modifications to support synchronous transfer
02c,04oct91,ihw  fixed problem with reselection (interrupt lock-out)
02b,19jun91,ihw  extensive modifications for disconnect/reconnect support
02a,06jun91,ihw  modifications for multiple-initiator support
01e,02oct90,jcc  UTINY became UINT8; changes in sem{Give, Take}() calls 
		 since SEM_ID's became SEMAPHORE's in various structures;
		 malloc() became calloc() in wd33c93CtrlCreate(); miscellaneous.
01d,21sep90,jcc  misc. cleanup for 5.0 release.
01c,10aug90,dnw  added forward declarations for VOID functions.
01b,18jul90,jcc  made semTake() calls 5.0 compatible; clean-up.
01a,28feb90,jcc  written
*/

/*
DESCRIPTION
This library contains part of the I/O driver for the Western Digital WD33C93 
family of SCSI-2 Bus Interface Controllers (SBIC).  It is designed to work 
with scsi2Lib. The driver routines in this library depend on the
SCSI-2 ANSI specification; for general driver routines and for overall SBIC
documentation, see wd33c93Lib.

USER-CALLABLE ROUTINES
Most of the routines in this driver are accessible only through the I/O
system.  The only exception in this portion of the driver is
wd33c93CtrlCreateScsi2(), which creates a controller structure.

INCLUDE FILES
wd33c93.h, wd33c93_2.h

SEE ALSO: scsiLib, scsi2Lib, wd33c93Lib, 
.pG "I/O System"
*/

#include "vxWorks.h"
#define WD33C93_2_LOCAL_FUNCS
#include "drv/scsi/wd33c93_2.h"
#undef WD33C93_2_LOCAL_FUNCS
#include "errnoLib.h"
#include "intLib.h"
#include "logLib.h"
#include "msgQLib.h"
#include "semLib.h"
#include "stdio.h"
#include "stdlib.h"
#include "tickLib.h"

#define WD_33C93_MAX_BYTES_PER_XFER  ((UINT) 0xffffff)

typedef WD_33C93_SCSI_CTRL SBIC;


/* globals */

int wd33c93XferDoneSemOptions  = SEM_Q_PRIORITY;

char *wd33c93ScsiTaskName      = SCSI_DEF_TASK_NAME;
int   wd33c93ScsiTaskOptions   = SCSI_DEF_TASK_OPTIONS;
int   wd33c93ScsiTaskPriority  = SCSI_DEF_TASK_PRIORITY;
int   wd33c93ScsiTaskStackSize = SCSI_DEF_TASK_STACK_SIZE;

IMPORT SCSI_CTRL *pSysScsiCtrl;

/* forward declarations */

LOCAL STATUS sbicDevSelect (SCSI_CTRL *pScsiCtrl, int devBusId,
			    UINT selTimeOut, UINT8 *msgBuf, UINT msgLen);
LOCAL STATUS sbicBusControl (SCSI_CTRL *pScsiCtrl, int operation);
LOCAL STATUS sbicXferParamsQuery (SCSI_CTRL *pScsiCtrl, UINT8 *pOffset,
    	    	    	    	                        UINT8 *pPeriod);
LOCAL STATUS sbicXferParamsSet (SCSI_CTRL *pScsiCtrl, UINT8 offset,
				                      UINT8 period);
LOCAL int sbicInfoXfer (SCSI_CTRL *pScsiCtrl, int phase, UINT8 *pBuf, UINT bufLength);
LOCAL void sbicSelTimeOutCvt (SBIC *pSbic, UINT timeOutInUsec,
    	    	    	      UINT *pTimeOutSetting);
LOCAL BOOL   sbicXferParamsCvt (SBIC  *pSbic, UINT8 *pOffset, UINT8 *pPeriod,
				UINT8 *pXferParams);
LOCAL STATUS sbicXferCountSet (SBIC *pSbic, UINT count);
LOCAL void sbicXferCountGet (SBIC *pSbic, UINT *pCount);
LOCAL void sbicCommand (SBIC *pSbic, UINT8 cmdCode);
LOCAL void sbicHwInit (SBIC *pSbic);
LOCAL void sbicPostResetInit (SBIC *pSbic);
LOCAL void sbicRegRead (SBIC *pSbic, UINT8 regAdrs, int  *pDatum);
LOCAL void sbicRegWrite (SBIC *pSbic, UINT8 regAdrs, UINT8 datum);
LOCAL void sbicIntr (SBIC *);
WD_33C93_SCSI_CTRL *wd33c93CtrlCreateScsi2 ( FAST UINT8 *sbicBaseAdrs, 
		       int regOffset, UINT clkPeriod, FUNCPTR sysScsiBusReset,
		       int sysScsiResetArg, UINT sysScsiDmaMaxBytes,
		       FUNCPTR sysScsiDmaStart, FUNCPTR sysScsiDmaAbort,
		       int sysScsiDmaArg);
LOCAL STATUS wd33c93CtrlInit (FAST SBIC *pSbic, FAST int scsiCtrlBusId,
		              FAST UINT defaultSelTimeOut);
LOCAL STATUS wd33c93Show ( FAST SCSI_CTRL *pScsiCtrl);

/*******************************************************************************
*
* wd33c93Scsi2IfInit - initialize the SCSI-2 interface to wd33c93
*
* NOMANUAL
*/
void wd33c93Scsi2IfInit ()
    {

    /* create table */

    wd33c93IfTblInit ();

    /* initialize table */

    pWd33c93IfTbl->sbicCommand       = (FUNCPTR) sbicCommand;
    pWd33c93IfTbl->sbicIntr          = (FUNCPTR) sbicIntr;
    pWd33c93IfTbl->sbicRegRead       = (FUNCPTR) sbicRegRead;
    pWd33c93IfTbl->sbicRegWrite      = (FUNCPTR) sbicRegWrite;
    pWd33c93IfTbl->sbicXferCountGet  = (FUNCPTR) sbicXferCountGet;
    pWd33c93IfTbl->sbicXferCountSet  = (FUNCPTR) sbicXferCountSet;
    pWd33c93IfTbl->wd33c93CtrlInit   = (FUNCPTR) wd33c93CtrlInit;
    pWd33c93IfTbl->wd33c93Show       = (FUNCPTR) wd33c93Show;

    }

/*******************************************************************************
*
* wd33c93CtrlCreateScsi2 - create and partially initialize an SBIC structure
*
* This routine creates an SBIC data structure and must be called before using
* an SBIC chip.  It must be called exactly once for a specified SBIC.
* Since it allocates memory for a structure needed by all routines in
* wd33c93Lib2, it must be called before any other routines in the library.
* After calling this routine, at least one call to wd33c93CtrlInit() must
* be made before any SCSI transaction is initiated using the SBIC.
*
* NOTE: Only the non-multiplexed processor interface is supported.
*
* A detailed description of the input parameters follows:
* .iP `sbicBaseAdrs'
* the address at which the CPU would access the lowest 
* (AUX STATUS) register of the SBIC.
* .iP `regOffset'
* the address offset (bytes) to access consecutive registers.
* (This must be a power of 2, for example, 1, 2, 4, etc.)
* .iP `clkPeriod'
* the period in nanoseconds of the signal to SBIC CLK input.
* .iP "`sysScsiBusReset' and `sysScsiResetArg'"
* the board-specific routine to pulse the SCSI bus RST signal.
* The specified argument is passed to this routine when it is called.
* It may be used to identify the SCSI bus to be reset, if there is a
* choice.  The interface to this routine is of the form:
* .CS
*     void xxBusReset 
*         (
*         int arg;  	    	      /@ call-back argument @/ 
*         )
* .CE
* .iP "`sysScsiDmaMaxBytes', `sysScsiDmaStart', `sysScsiDmaAbort', and `sysScsiDmaArg'"
* board-specific routines to handle DMA transfers to and from the SBIC;
* if the maximum DMA byte count is zero, programmed I/O is used.
* Otherwise, non-NULL function pointers to DMA start and abort routines
* must be provided.
* The specified argument is passed to these routines when they are
* called; it may be used to identify the DMA channel to use, for example.
* Note that DMA is implemented only during SCSI data in/out phases.
* The interface to these DMA routines must be of the form:
* .CS
*     STATUS xxDmaStart 
*         (
*         int arg;  	    	      /@ call-back argument           @/
*         UINT8 *pBuffer;             /@ ptr to the data buffer       @/
*         UINT bufLength;             /@ number of bytes to xfer      @/
*   	  int direction;    	      /@ 0 = SCSI->mem, 1 = mem->SCSI @/
*         )
*
*     STATUS xxDmaAbort
*         (
*         int arg;  	    	      /@ call-back argument @/
*         )
* .CE
*
* RETURNS: A pointer to the SBIC structure, or NULL if memory is 
* insufficient or the parameters are invalid.
*/

WD_33C93_SCSI_CTRL *wd33c93CtrlCreateScsi2
    (
    FAST UINT8 *sbicBaseAdrs,	     /* base address of the SBIC              */
    int         regOffset,	     /* address offset between SBIC registers */
    UINT        clkPeriod,	     /* period of the SBIC clock (nsec)       */
    FUNCPTR     sysScsiBusReset,     /* function to reset SCSI bus            */
    int         sysScsiResetArg,     /* argument to pass to above function    */
    UINT        sysScsiDmaMaxBytes,  /* maximum byte count using DMA          */
    FUNCPTR     sysScsiDmaStart,     /* function to start SCSI DMA transfer   */
    FUNCPTR     sysScsiDmaAbort,     /* function to abort SCSI DMA transfer   */
    int         sysScsiDmaArg	     /* argument to pass to above functions   */
    )
    {
    FAST SBIC *pSbic;		/* ptr to SBIC info */

    /* verify input parameters */

    if ((regOffset == 0) || (clkPeriod == 0))
	return ((SBIC *) NULL);
    
    if ((sysScsiDmaMaxBytes != 0) &&
	((sysScsiDmaStart == NULL) || (sysScsiDmaAbort == NULL)))
	return ((SBIC *) NULL);
    
    /* calloc the controller info structure; return NULL if unable */

    if ((pSbic = (SBIC *) calloc (1, sizeof (SBIC))) == NULL)
        return ((SBIC *) NULL);

    /*
     *  Set up sizes of event and thread structures.  Must be done before
     *	calling "scsiCtrlInit()".
     *
     *	Since this controller uses standard event and thread structures,
     *	it could simply leave these values set to 0 (i.e., use default).
     */

    pSbic->scsiCtrl.eventSize  = sizeof (SCSI_EVENT);
    pSbic->scsiCtrl.threadSize = sizeof (SCSI_THREAD);
    
    /* leave transact, event and thread management routines as default */

    /* fill in driver-specific routines for scsiLib interface */

    pSbic->scsiCtrl.scsiDevSelect       = sbicDevSelect;
    pSbic->scsiCtrl.scsiInfoXfer        = sbicInfoXfer;
    pSbic->scsiCtrl.scsiBusControl      = sbicBusControl;
    pSbic->scsiCtrl.scsiXferParamsQuery = sbicXferParamsQuery;
    pSbic->scsiCtrl.scsiXferParamsSet   = (FUNCPTR)sbicXferParamsSet;

    pSbic->scsiCtrl.wideXfer		= FALSE;
    pSbic->scsiCtrl.scsiWideXferParamsQuery = NULL;
    pSbic->scsiCtrl.scsiWideXferParamsSet   = NULL;

    /* fill in driver-specific variables for scsiLib interface */

    if ((sysScsiDmaMaxBytes != 0) &&
	(sysScsiDmaMaxBytes <= WD_33C93_MAX_BYTES_PER_XFER))
	pSbic->scsiCtrl.maxBytesPerXfer = sysScsiDmaMaxBytes;
    else
	pSbic->scsiCtrl.maxBytesPerXfer = WD_33C93_MAX_BYTES_PER_XFER;

    /* fill in generic SCSI info for this controller */

    scsiCtrlInit (&pSbic->scsiCtrl);

    /* initialize SBIC info transfer synchronisation semaphore */

    if (semBInit (&pSbic->xferDoneSem, wd33c93XferDoneSemOptions, SEM_EMPTY)
	         == ERROR)
	{
	SCSI_MSG ("wd33c93CtrlCreate: semBInit of xferDoneSem failed\n",
		  0, 0, 0, 0, 0, 0);
	(void) free ((char *) pSbic);
	return ((WD_33C93_SCSI_CTRL *) NULL);
	}
					  
    /* initialise state variables */

    pSbic->state       = SBIC_STATE_IDLE;
    pSbic->initPending = FALSE;
    pSbic->xferPending = FALSE;
    
    /* fill in SBIC specific data for this controller */

    pSbic->clkPeriod   = clkPeriod;
    pSbic->pAdrsReg    = sbicBaseAdrs;
    pSbic->pAuxStatReg = sbicBaseAdrs;
    pSbic->pRegFile    = sbicBaseAdrs + regOffset;

    /* fill in board-specific SCSI bus reset and DMA xfer routines */

    pSbic->sysScsiDmaStart = sysScsiDmaStart;
    pSbic->sysScsiDmaAbort = sysScsiDmaAbort;
    pSbic->sysScsiDmaArg   = sysScsiDmaArg;

    pSbic->sysScsiBusReset = sysScsiBusReset;
    pSbic->sysScsiResetArg = sysScsiResetArg;
    
    /* spawn SCSI manager - use generic code from "scsiLib.c" */

    pSbic->scsiCtrl.scsiMgrId = taskSpawn (wd33c93ScsiTaskName,
		       	    	           wd33c93ScsiTaskPriority,
		       	    	           wd33c93ScsiTaskOptions,
		       	    	           wd33c93ScsiTaskStackSize,
		       	    	           (FUNCPTR) scsiMgr,
		       	    	           (int) pSbic,
					   0, 0, 0, 0, 0, 0, 0, 0, 0);

    if (pSbic->scsiCtrl.scsiMgrId == ERROR)
	{
	SCSI_MSG ("wd33c93CtrlCreate: can't spawn SCSI manager task\n",
		  0, 0, 0, 0, 0, 0);
	semTerminate (&pSbic->xferDoneSem);
	free ((char *) pSbic);
	return ((SBIC *) NULL);
	}

    return (pSbic);
    }

/*******************************************************************************
*
* wd33c93CtrlInit - initialize user specifiable fields in an SBIC structure
*
* After an SBIC structure is created with wd33c93CtrlCreate(2), but
* before using the SBIC, it must be initialized by calling this routine.
* It may be called more than once if desired.  However, it should only be
* called while there is no activity on the SCSI interface.
*
* A detailed description of the input parameters follows:
* 
* .iP `pSbic'
* pointer to the WD_33C93_SCSI_CTRL structure created with wd33c93CtrlCreate(2).
* 
* .iP `scsiCtrlBusId'
* the SCSI bus ID of the SBIC; somewhat arbitrary, seven (7),
* or highest priority, is conventional.  Must be in range 0 - 7.
* 
* .iP `defaultSelTimeOut'
* the timeout (in microsec) for selecting a SCSI device
* attached to this controller; called default since the
* timeout may be specified per device.
* The recommended value zero (0)
* specifies SCSI_DEF_SELECT_TIMEOUT (250 millisec).
* Values over the maximum timeout period specify the maximum,
* which is approximately 2 seconds (depending on SBIC clock
* period).  See also Western Digital documentation.
*
* RETURNS: OK, or ERROR if out-of-range parameter(s).
*/

LOCAL STATUS wd33c93CtrlInit
    (
    FAST SBIC *pSbic,		/* ptr to SBIC info */
    FAST int  scsiCtrlBusId,	/* SCSI bus ID of this SBIC */
    FAST UINT defaultSelTimeOut /* default dev. select timeout (microsec) */
    )
    {
    /* verify scsiCtrlBusId and enter legal value in SBIC structure */

    if (scsiCtrlBusId < SCSI_MIN_BUS_ID || scsiCtrlBusId > SCSI_MAX_BUS_ID)
	return (ERROR);
    else
	pSbic->scsiCtrl.scsiCtrlBusId = scsiCtrlBusId;

    if (defaultSelTimeOut == 0)
	pSbic->defSelTimeOut = SCSI_DEF_SELECT_TIMEOUT;
    else
	pSbic->defSelTimeOut = defaultSelTimeOut;

    sbicHwInit (pSbic); 	/* initialize the SBIC hardware */

    return (OK);
    }

/*******************************************************************************
*
* sbicDevSelect - attempt to select a SCSI device
*
* Initiate selection of the specified SCSI target, asserting ATN if there is
* an identification message to be sent.  Return immediately; do not wait for
* the selection to complete.  (This will be notified by an asynchronous
* message posted in the controller's event queue).
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
* Therefore, a SELECT command is issued only if the controller is currently
* IDLE (waiting for (re)selection or a host command).  If the controller is
* CONNECTED, (re)selection is assumed to have already occurred.  Note that
* this is not an error condition; there is no way for the caller to reliably
* distinguish the two cases, because even if the thread appears to have been
* activated it may yet be "pre-empted" by (re)selection.
*
* The controller should never be in SELECT_PENDING state.  If it is, the
* SCSI manager has tried to activate multiple concurrent threads on the
* controller, which indicates a major (software) disaster.
*
* NOTE: Interrupt locking is required to ensure that the correct action
* is taken once the controller state has been checked.
*
* RETURNS: OK, or ERROR if the controller is in an invalid state (this
* indicates a major software failure).
*/
LOCAL STATUS sbicDevSelect
    (
    SCSI_CTRL *pScsiCtrl,	/* ptr to SCSI controller info     */
    int        devBusId,	/* SCSI bus ID of device to select */
    UINT       selTimeOut,	/* select t-o period (usec)        */
    UINT8     *msgBuf,		/* ptr to identification message   */
    UINT       msgLen	    	/* maximum number of message bytes */
    )
    {
    FAST SBIC *pSbic;		/* ptr to SBIC info             */
    int        lockKey;		/* saved interrupt lock key     */
    UINT8      selectCmd;   	/* actual SELECT command to use */
    STATUS     status;

    pSbic = (SBIC *) pScsiCtrl;

    /* reject attempts to select self */
    
    if (devBusId == pScsiCtrl->scsiCtrlBusId)
	{
	errnoSet (S_scsiLib_ILLEGAL_BUS_ID);
        return (ERROR);
	}

    /* convert timeout period to SBIC units, in place */
    
    sbicSelTimeOutCvt (pSbic, selTimeOut, &selTimeOut);

    selectCmd = (msgLen != 0) ? SBIC_CMD_SEL_ATN : SBIC_CMD_SELECT;
    
    /*
     *	Check SBIC is IDLE, then issue SELECT command.  Must be done with
     *	interrupts locked to avoid races with the reselection ISR.
     *
     *	If the controller is not IDLE, do not issue a select command.  Just
     *	return - there must be a (re)selection event pending which will be
     *	handled exactly as if it occurred just after the select command was
     *	issued.
     */
    lockKey = intLock ();

    switch (pSbic->state)
	{
	case SBIC_STATE_IDLE:
	    pSbic->state = SBIC_STATE_SELECT_PENDING;

	    sbicRegWrite (pSbic, SBIC_REG_DEST_ID,   (UINT8) devBusId);
	    sbicRegWrite (pSbic, SBIC_REG_TO_PERIOD, (UINT8) selTimeOut);
	    sbicCommand  (pSbic, selectCmd);

	    status = OK;
	    break;

	case SBIC_STATE_CONNECTED:
	    status = OK;
	    break;

	case SBIC_STATE_SELECT_PENDING:
	default:
	    status = ERROR;
	    break;
	}

    intUnlock (lockKey);

    if (status != OK)
	{
	SCSI_DEBUG_MSG ("sbicDevSelect: invalid controller state (%d)\n",
		    	pSbic->state, 0, 0, 0, 0, 0);
	}

    return (status);
    }


/*******************************************************************************
*
* sbicBusControl - miscellaneous low-level SCSI bus control operations
*
* Allows the caller to:-
*
*   - assert the SCSI ATN signal (indicating a message out is available)
*   - negate the SCSI ACK signal (indicating a message in has been read)
*
* The "operation" parameter is a bitmask which allows any combination of these
* functions to be carried out with a single call.  Note that the order in
* which things are done is important: ATN is asserted (if specified) before
* ACK is negated.  (This allows an incoming SCSI message to be rejected.)
*
* NOTE: since the SBIC is not connected to the SCSI bus RST signal, a board-
* specific function is used to implement the BUS_RESET command.  As a result
* of calling this function, something must notify the SCSI manager task that
* a bus reset has occurred ...
*
* RETURNS: OK (no error conditions)
*/

LOCAL STATUS sbicBusControl
    (
    SCSI_CTRL *pScsiCtrl,	/* ptr to SBIC info                         */
    int        operation	/* bitmask for operation(s) to be performed */
    )
    {
    FAST SBIC *pSbic = (SBIC *) pScsiCtrl;

    if (operation & SCSI_BUS_RESET)
	(*pSbic->sysScsiBusReset) (pSbic->sysScsiResetArg);

    if (operation & SCSI_BUS_ASSERT_ATN)
	sbicCommand (pSbic, SBIC_CMD_SET_ATN);

    if (operation & SCSI_BUS_NEGATE_ACK)
    	sbicCommand (pSbic, SBIC_CMD_NEG_ACK);

    return (OK);
    }

/*******************************************************************************
*
* sbicXferParamsQuery - get (synchronous) transfer parameters
*
* Updates the synchronous transfer parameters suggested in the call to match
* the SBIC's capabilities.  Transfer period is in SCSI units (multiples of
* 4 ns).
*
* Note: the transfer period is made longer and the offset is made smaller if
* the SBIC cannot handle the specified values.
*
* RETURNS: OK
*/

LOCAL STATUS sbicXferParamsQuery
    (
    SCSI_CTRL *pScsiCtrl,		/* ptr to SBIC info             */
    UINT8     *pOffset,			/* max REQ/ACK offset  [in/out] */
    UINT8     *pPeriod			/* min transfer period [in/out] */
    )
    {
    UINT8 unused;

    (void) sbicXferParamsCvt ((SBIC *) pScsiCtrl, pOffset, pPeriod, &unused);
    
    return (OK);
    }
    
/*******************************************************************************
*
* sbicXferParamsSet - set transfer parameters
*
* Programs the SBIC to use the specified transfer parameters.  An offset
* of zero specifies asynchronous transfer (period is then irrelevant).
* Transfer period is in SCSI units (multiples of 4 ns).
*
* RETURNS: OK if transfer parameters are OK, else ERROR.
*/

LOCAL STATUS sbicXferParamsSet
    (
    SCSI_CTRL *pScsiCtrl,		/* ptr to SBIC info       */
    UINT8      offset,			/* max REQ/ACK offset     */
    UINT8      period			/* min transfer period    */
    )
    {
    FAST SBIC *pSbic = (SBIC *) pScsiCtrl;
    UINT8      xferParams;
    
    if (!sbicXferParamsCvt (pSbic, &offset, &period, &xferParams))
	{				/* should never happen */
	errnoSet (S_scsiLib_ILLEGAL_PARAMETER);
	return (ERROR);
	}

    sbicRegWrite (pSbic, SBIC_REG_SYNC_XFER, xferParams);

    return (OK);
    }

/*******************************************************************************
*
* sbicInfoXfer - transfer information bytes to/from target via SCSI bus
*
* Executes a "Transfer Info" command to read (write) bytes from (to) the
* SCSI bus.  If the transfer phase is DATA IN or DATA OUT and there is a
* DMA routine available, DMA is used - otherwise it's a tight programmed
* i/o loop.
*
* Returns when the transfer has completed; i.e., the last byte has been
* received (sent) and ACK is still asserted in the case of MESSAGE IN
* transfers, or a request for a new information transfer has occurred (for
* all other types of transfer).
*
* The returned value is the number of bytes actually transferred across the
* SCSI bus.  In the case of DATA phases, this may be less than the requested
* transfer length because the target may change the information phase "early"
* (to send a message in, for example).  For non-DATA transfers, if the byte
* count transferred is not what was asked for, it is probably an error
* condition (left to the caller's discretion).
*
* RETURNS: Number of bytes transferred across SCSI bus, or ERROR.
*/

LOCAL int sbicInfoXfer
    (
    FAST SCSI_CTRL *pScsiCtrl,      	/* ptr to SCSI controller info       */
    int             phase,  	    	/* SCSI phase being transferred      */
    FAST UINT8     *pBuf,		/* ptr to byte buffer for i/o        */
    UINT            bufLength		/* number of bytes to be transferred */
    )
    {
    FAST SBIC  *pSbic;			/* ptr to SBIC info                  */
    FAST UINT8  auxStatus;		/* SBIC auxiliary status             */
    UINT        bytesLeft;		/* bytes not transferred across bus  */
    int         direction;		/* input (READ) or output (WRITE)    */
    BOOL        usingDMA;		/* using DMA for this transfer ?     */

    pSbic = (SBIC *) pScsiCtrl;

    SCSI_DEBUG_MSG ("sbicInfoXfer: phase = %d, buf = 0x%x, length = %d\n",
		    phase, (int) pBuf, bufLength, 0, 0, 0);

    /*
     *	Set transfer count, issue Transfer Info command
     */
    if (sbicXferCountSet (pSbic, (UINT) bufLength) != OK)
	{
    	errnoSet (S_scsiLib_ILLEGAL_PARAMETER);
	return (ERROR);
	}

    sbicCommand (pSbic, SBIC_CMD_XFER_INFO);

    /*
     *	Identify phase, set transfer mode accordingly
     */
    switch (phase)
	{
    	case SCSI_DATA_OUT_PHASE:
	    direction = WRITE;
	    usingDMA  = (pSbic->sysScsiDmaStart != NULL);
	    break;
	    
    	case SCSI_DATA_IN_PHASE:
	    direction = READ;
	    usingDMA  = (pSbic->sysScsiDmaStart != NULL);
	    break;

	case SCSI_COMMAND_PHASE:
    	case SCSI_MSG_OUT_PHASE:
	    direction = WRITE;
	    usingDMA  = FALSE;
	    break;
	    
	case SCSI_STATUS_PHASE:
    	case SCSI_MSG_IN_PHASE:
	    direction = READ;
	    usingDMA  = FALSE;
	    break;

        default:
	    SCSI_MSG ("sbicInfoXfer: invalid bus phase (%d)\n",
		      phase, 0, 0, 0, 0, 0);
	    return (ERROR);
	}
    
    /*
     *	Start DMA, if used, or programmed i/o loop to transfer data
     */
    pSbic->xferPending = TRUE;
    
    if (usingDMA)
	{
        if ((*pSbic->sysScsiDmaStart) (pSbic->sysScsiDmaArg,
				       pBuf,
				       bufLength,
				       direction) != OK)
	    {
	    SCSI_MSG ("sbicInfoXfer: unable to start DMA transfer\n",
		      0, 0, 0, 0, 0, 0);
	    return (ERROR);
	    }
	}
    else if (direction == READ)
	{
    	*pSbic->pAdrsReg = SBIC_REG_DATA;

    	while ((auxStatus = *pSbic->pAuxStatReg) & SBIC_AUX_STAT_BUSY)
	    {
	    if (auxStatus & SBIC_AUX_STAT_DBUF_READY)
	    	*pBuf++ = *pSbic->pRegFile;
	    }
	}
    else /* (direction == WRITE) */
	{
    	*pSbic->pAdrsReg = SBIC_REG_DATA;

    	while ((auxStatus = *pSbic->pAuxStatReg) & SBIC_AUX_STAT_BUSY)
	    {
	    if (auxStatus & SBIC_AUX_STAT_DBUF_READY)
	    	*pSbic->pRegFile = *pBuf++;
	    }
	}

    /*
     *	Wait for transfer to complete: find out how many bytes transferred,
     *  abort DMA transfer if necessary.
     */
    semTake (&pSbic->xferDoneSem, WAIT_FOREVER);
    
    sbicXferCountGet (pSbic, &bytesLeft);
    
    if (bytesLeft > bufLength)
	{
	SCSI_MSG ("sbicInfoXfer: invalid count (len = 0x%0x, left = 0x%0x)\n",
		bufLength, bytesLeft, 0, 0, 0, 0);
	return (ERROR);
	}

    if (usingDMA && (bytesLeft > 0))
	(*pSbic->sysScsiDmaAbort) (pSbic->sysScsiDmaArg);

    SCSI_DEBUG_MSG ("sbicInfoXfer: transferred %d of %d byte(s)\n",
		    bufLength - bytesLeft, bufLength, 0, 0, 0, 0);
    
    return (bufLength - bytesLeft);
    }

/*******************************************************************************
*
* sbicSelTimeOutCvt - convert a select timeout from usec to SBIC units 
*
* The conversion formula is given on p. 11 of the Western Digital WD33C93
* manual. Note that 0 specifies the default setting of 250 usec. Also,
* the SBIC accepts up to a 8-bit timeout, so a maximum value of 0xff is
* returned in *pTimeOutSetting.
*/

LOCAL void sbicSelTimeOutCvt
    (
    SBIC      *pSbic,		    	/* ptr to SBIC info */
    FAST UINT  timeOutInUsec,		/* timeout in microsecs */
    FAST UINT *pTimeOutSetting		/* ptr to result */
    )
    {
    FAST UINT tempSelTimeOut;	/* temp. select timeout setting */

    if (timeOutInUsec == (UINT) 0)
	timeOutInUsec = pSbic->defSelTimeOut;

    tempSelTimeOut = (timeOutInUsec / (80 * pSbic->clkPeriod)) + 1; 

    if (tempSelTimeOut > 0xff)
	tempSelTimeOut = 0xff;

    *pTimeOutSetting = tempSelTimeOut;
    }

/*******************************************************************************
* sbicXferParamsCvt - convert transfer period from SCSI to SBIC units
*
* Given a "suggested" REQ/ACK offset and transfer period (in SCSI units of
* 4 ns), return the nearest offset and transfer period the SBIC is capable of
* using.  Also return the corresponding value of the SBIC's Synchronous
* Transfer register.
*
* An offset of zero specifies asynchronous transfer, in which case the period
* is irrelevant.  Otherwise, the offset may be clipped to be within the
* maximum limit the SBIC is capable of.
*
* The transfer period is normally rounded towards longer periods if the SBIC
* is not capable of using the exact specified value.  The period may also be
* clipped to be within the SBIC's maximum and minimum limits according to its
* clock period.
*
* If either the offset or period need to be clipped, the value FALSE is
* retuned as this may reflect an error condition.
*
* Currently assumes that the SBIC is a 33C93, not a 33C93A (i.e. fixed clock
* divider of 2) - see 33C93A data sheet pp 9, 13 and 14 for further details.
*
* RETURNS: TRUE if input period is within SBIC's range, else FALSE
*/

LOCAL BOOL sbicXferParamsCvt
    (
    FAST SBIC  *pSbic,			/* ptr to SBIC info                  */
    FAST UINT8 *pOffset,		/* REQ/ACK offset                    */
    FAST UINT8 *pPeriod,		/* xfer period, SCSI units (x 4 ns)  */
    FAST UINT8 *pXferParams		/* corresponding Sync Xfer Reg value */
    )
    {
    UINT8 offset  = *pOffset;
    UINT8 period  = *pPeriod;
    BOOL  inRange = TRUE;
    UINT8 xferParams;
    
    if (offset == SCSI_SYNC_XFER_ASYNC_OFFSET)
	xferParams = SBIC_SYNC_XFER_PARAMS_ASYNC;

    else
	{
	UINT clkCycles;
	
	/*
	 *  Round (and perhaps clip) transfer period to suit SBIC
	 *
	 *  Note: split into two stages to avoid associativity problems,
	 *  	and careful to round up rather than truncate result.
	 */
	clkCycles  = (period * 4) + (pSbic->clkPeriod - 1);
	clkCycles /= pSbic->clkPeriod;
	
	if (clkCycles < SBIC_MIN_XFER_PERIOD)
	    {
	    clkCycles = SBIC_MIN_XFER_PERIOD;
	    inRange   = FALSE;
	    }
	
	else if (clkCycles > SBIC_MAX_XFER_PERIOD)  /* very unlikely ! */
	    {
	    clkCycles = SBIC_MAX_XFER_PERIOD;
	    inRange   = FALSE;
	    }
	
	period = (clkCycles * pSbic->clkPeriod) / 4;

	/*
	 *  Clip offset (if necessary) to suit SBIC
	 */
    	if (offset > SBIC_MAX_REQ_ACK_OFFSET)
	    {
	    offset  = SBIC_MAX_REQ_ACK_OFFSET;
	    inRange = FALSE;
	    }

	/*
	 *  Merge offset and cycles to form Sync. Transfer Reg. contents
	 */
	if (clkCycles == SBIC_MAX_XFER_PERIOD)
	    clkCycles = 0;
	
	xferParams = (clkCycles << SBIC_SYNC_XFER_PERIOD_SHIFT) | offset;
	}

    *pOffset     = offset;
    *pPeriod     = period;
    *pXferParams = xferParams;

    return (inRange);
    }

/*******************************************************************************
*
* sbicXferCountSet - load the SBIC transfer counter with the specified count 
*
* RETURNS: OK if count is in range 0 - 0xffffff, otherwise ERROR.
*
* NOMANUAL
*/

LOCAL STATUS sbicXferCountSet
    (
    FAST SBIC *pSbic,			/* ptr to SBIC info */
    FAST UINT count			/* count value to load */
    )
    {
    if (count > WD_33C93_MAX_BYTES_PER_XFER)
        return (ERROR);

    *pSbic->pAdrsReg = SBIC_REG_XFER_COUNT_MSB;
    
    *pSbic->pRegFile = (UINT8) ((count >> 16) & 0xff);
    *pSbic->pRegFile = (UINT8) ((count >>  8) & 0xff);
    *pSbic->pRegFile = (UINT8) ( count        & 0xff);
    
    return (OK);
    }

/*******************************************************************************
*
* sbicXferCountGet - fetch the SBIC transfer count 
*
* The value of the transfer counter is copied to *pCount.
*
* NOMANUAL
*/

LOCAL void sbicXferCountGet
    (
    FAST SBIC *pSbic,			/* ptr to SBIC info */
    FAST UINT *pCount			/* ptr to returned value */
    )
    {
    FAST UINT tempCount;

    *pSbic->pAdrsReg = SBIC_REG_XFER_COUNT_MSB;
    
    tempCount  = ((UINT) *pSbic->pRegFile) << 16;
    tempCount |= ((UINT) *pSbic->pRegFile) << 8;
    tempCount |=  (UINT) *pSbic->pRegFile;
    
    *pCount = (int) tempCount;
    }

/*******************************************************************************
*
* sbicCommand - write a command code to the SBIC Command Register
*
* NOMANUAL
*/

LOCAL void sbicCommand
    (
    SBIC *pSbic,			/* ptr to SBIC info */
    UINT8 cmdCode			/* new command code */
    )
    {
    while (*pSbic->pAuxStatReg & SBIC_AUX_STAT_CMD_IN_PROG)
	;

    *pSbic->pAdrsReg = SBIC_REG_COMMAND;
    *pSbic->pRegFile = cmdCode;
    }

/*******************************************************************************
*
* sbicIntr - interrupt service routine for the SBIC
*
* NOMANUAL
*/
LOCAL void sbicIntr
    (
    SBIC *pSbic				/* ptr to SBIC info */
    )
    {
    UINT8 scsiStatus;
    int   targetId;
    SBIC_STATE state;
    SCSI_EVENT event;

    int  busPhase     = NONE;   	/* init. to prevent a warning */
    BOOL initComplete = FALSE;
    BOOL xferComplete = FALSE;

    event.type = NONE;

    /*
     *	Read SCSI status register and "decode" into state transitions etc.
     */
    *pSbic->pAdrsReg = SBIC_REG_SCSI_STATUS;
    scsiStatus       = *pSbic->pRegFile;

    state = pSbic->state;

    SCSI_INT_DEBUG_MSG ("SBIC Interrupt: code = 0x%02x (state = %d)\n",
			scsiStatus, state, 0, 0, 0, 0);

    /*
     *	Parse SBIC SCSI status; generate state info (don't change state yet !)
     */
    if (scsiStatus & STAT_PHASE_REQ)
	{
	busPhase    = (int) (scsiStatus & STAT_MCI_BITS);
	scsiStatus |= STAT_MCI_BITS;
	}

    /*
     *	If the target disconnects after an info transfer but before a new
     *	phase has been requested, the 33C93 appears to generate a normal
     *	disconnect interrupt rather than an unexpected disconnect.  To keep
     *	the ISR logic clean, we convert this to an unexpected disconnect.
     */
    if ((scsiStatus == STAT_SERV_REQ_DISCON) && pSbic->xferPending)
	{
	scsiStatus = STAT_TERM_UNEXP_DISCON;
	}
    
    switch (scsiStatus)
	{
	/*  Group: Reset */
	
	case STAT_SUCC_RESET:
	    initComplete = TRUE;
	    state        = SBIC_STATE_IDLE;
	    break;

	/*  Group: Successful Completion */

	case STAT_SUCC_SELECT:
	    /* assert (state == SBIC_STATE_SELECT_PENDING); */
	    /* Select command completed successfully */
	    
    	    sbicRegRead (pSbic, SBIC_REG_DEST_ID, &targetId);

	    event.type        = SCSI_EVENT_CONNECTED;
	    event.busId       = targetId & 0x07;
	    event.nBytesIdent = 0;

	    state = SBIC_STATE_CONNECTED;
	    break;

	case STAT_SUCC_XFER_MCI:
	    /* assert (state == SBIC_STATE_CONNECTED); */
	    /* current transfer completed: req for new phase */
	    
	    event.type  = SCSI_EVENT_XFER_REQUEST;
	    event.phase = busPhase;
	    
	    xferComplete = TRUE;
	    break;

	/*  Group: Paused/Aborted */

	case STAT_PAUSE_MSG_IN:
	    /* assert (state == SBIC_STATE_CONNECTED); */
	    /* current msg in transfer completed: ack asserted */
	    
	    xferComplete = TRUE;
	    break;

	/*  Group: Terminated */

	case STAT_TERM_UNEXP_DISCON:
	    /* assert (state == SBIC_STATE_CONNECTED); */
	    /* current transfer terminated due to disconnection */
	    
	    event.type = SCSI_EVENT_DISCONNECTED;
	    
	    xferComplete = TRUE;

	    state = SBIC_STATE_IDLE;
	    break;
	    
	case STAT_TERM_TIMEOUT:
	    /* assert (pSbic->state == SBIC_STATE_SELECT_PENDING); */
	    /* Select command timed out */

	    event.type = SCSI_EVENT_TIMEOUT;
	    
	    state = SBIC_STATE_IDLE;
	    break;

	case STAT_TERM_UNX_PHASE:
	    /* assert (state == SBIC_STATE_CONNECTED); */
	    /* current transfer terminated by req for new phase */
	    
	    event.type  = SCSI_EVENT_XFER_REQUEST;
	    event.phase = busPhase;

	    xferComplete = TRUE;
	    break;

	/*  Group: Service Required */
	    
	case STAT_SERV_REQ_RESELECT:
    	    sbicRegRead (pSbic, SBIC_REG_SOURCE_ID, &targetId);

	    event.type        = SCSI_EVENT_RESELECTED;
	    event.busId       = targetId & 0x07;
	    event.nBytesIdent = 0;

	    state = SBIC_STATE_CONNECTED;
	    break;
	    
	case STAT_SERV_REQ_DISCON:
	    /* assert (state == SBIC_STATE_CONNECTED); */

	    event.type = SCSI_EVENT_DISCONNECTED;

	    state = SBIC_STATE_IDLE;
	    break;

	case STAT_SERV_REQ_ASSERTED:
	    /* assert (state == SBIC_STATE_CONNECTED); */

	    event.type  = SCSI_EVENT_XFER_REQUEST;
	    event.phase = busPhase;
	    break;
	    
	default:
	    logMsg ("Unexpected SBIC Interrupt, Code = 0x%02x (state = %d)\n",
		    scsiStatus, state, 0, 0, 0, 0);
	    break;
	}

    /*
     *	Handle state transition, if any
     */
    if (state != pSbic->state)
	{
	SCSI_INT_DEBUG_MSG ("SBIC state transition: %d -> %d\n",
			    pSbic->state, state, 0, 0, 0, 0);
	
	pSbic->state = state;
    	}

    /*
     *	Synchronize with task-level code
     */
    if (initComplete && pSbic->initPending)
	{
        pSbic->initPending = FALSE;

	sbicPostResetInit (pSbic);
	}

    if (xferComplete && pSbic->xferPending)
	{
	pSbic->xferPending = FALSE;

	semGive (&pSbic->xferDoneSem);
	}

    /*
     *	Post event to SCSI manager for further processing
     */
    if (event.type != NONE)
	scsiMgrEventNotify ((SCSI_CTRL *)pSbic, &event, sizeof (event));
    }

/*******************************************************************************
*
* sbicHwInit - initialize the SBIC to a known state
*
* This routine puts the SBIC into a known quiescent state.  It does *not*
* reset the SCSI bus (and any other devices thereon).
*
* The reselection interrupt is enabled iff the "disconnect" flag is set in
* the SCSI controller data structure.  DMA transfers are enabled iff there is
* a DMA transfer function in the SCSI controller data structure.
*
* NOTE: the SBIC does not preserve its entire register context over a software
* reset so the initialisation is completed in the reset interrupt handler.
* If the routine is called at task level, it waits until this processing has
* been completed (a busy wait, as it's only a few microseconds).  If it is
* called from interrupt level, it does *not* wait for the reset to complete.
* Be warned !
*/

LOCAL void sbicHwInit
    (
    SBIC *pSbic				/* ptr to an SBIC structure */
    )
    {
    pSbic->initPending = TRUE;
    
    sbicRegWrite (pSbic, SBIC_REG_OWN_ID, (UINT8) pSbic->scsiCtrl.scsiCtrlBusId);
    sbicCommand  (pSbic, SBIC_CMD_RESET);

    if (!intContext ())
	{
        /*
    	 *  Wait for "reset" command to finish.  Busy wait on flag set by
         *  reset interrupt handler - shouldn't be more than a few microsecs,
         *  but don't wait more than SBIC_MAX_RESET_WAIT clock ticks in any
	 *  case.
    	 */
	ULONG timeOut = tickGet () + SBIC_MAX_RESET_WAIT;
	
    	while (pSbic->initPending && (tickGet () < timeOut))
	    ;

        if (pSbic->initPending)
	    logMsg ("sbicHwInit: initialisation command not completed !\n",
		    0, 0, 0, 0, 0, 0);
        }
    }


/******************************************************************************
*
* sbicPostResetInit - SBIC post-reset initialisation
*
* This routine completes the SBIC initialisation started in "sbicHwInit ()".
* It is called at interrupt level after the software reset command has
* completed.
*/

LOCAL void sbicPostResetInit
    (
    FAST SBIC *pSbic			/* ptr to SBIC info */
    )
    {
    
    UINT8 srcIdVal = pSbic->scsiCtrl.disconnect ?
	    	          SBIC_SRC_ID_RESEL_ENABLE : 0x00;
    
    UINT8 ctrlVal  = (pSbic->sysScsiDmaStart != NULL) ?
	    	          SBIC_CONTROL_DMA_ENABLE : 0x00;
    
    sbicRegWrite (pSbic, SBIC_REG_SOURCE_ID, srcIdVal);
    sbicRegWrite (pSbic, SBIC_REG_CONTROL,   ctrlVal);
    }


/*******************************************************************************
*
* sbicRegRead - Get the contents of a specified SBIC register
*/

LOCAL void sbicRegRead
    (
    SBIC *pSbic,			/* ptr to an SBIC structure    */
    UINT8 regAdrs,			/* address of register to read */
    int  *pDatum			/* buffer for return value     */
    )
    {
    *pSbic->pAdrsReg = regAdrs;
    *pDatum = (int) *pSbic->pRegFile;
    }

/*******************************************************************************
*
* sbicRegWrite - write a value to a specified SBIC register
*
* NOMANUAL
*/

LOCAL void sbicRegWrite
    (
    SBIC *pSbic,			/* ptr to an SBIC structure     */
    UINT8 regAdrs,			/* address of register to write */
    UINT8 datum				/* value to be written          */
    )
    {
    *pSbic->pAdrsReg = regAdrs;
    *pSbic->pRegFile = datum;
    }

/*******************************************************************************
*
* wd33c93Show - Display values of all readable wd33c93 chip registers
*
* Only for use during debugging.
*
* RETURNS: OK | ERROR.
*/

LOCAL STATUS wd33c93Show
    (
    FAST SCSI_CTRL *pScsiCtrl		/* ptr to SCSI controller info */
    )
    {
    FAST int ix;
    FAST SBIC *pSbic;		/* ptr to SBIC info */

    if (pScsiCtrl == NULL)
        {
        if (pSysScsiCtrl != NULL)
            pScsiCtrl = pSysScsiCtrl;
        else
            {
            printErr ("No SCSI controller specified.\n");
            return (ERROR);
            }
        }    

    pSbic = (SBIC *) pScsiCtrl;

    for (ix = SBIC_REG_OWN_ID; ix <= SBIC_REG_COMMAND; ix++)
	{
        *pSbic->pAdrsReg = (UINT8) ix;
	printf ("REG #%02x = 0x%02x\n", ix, *pSbic->pRegFile);
	}

    return (OK);
    }
