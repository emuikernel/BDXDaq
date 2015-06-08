/* ncr5390Lib2.c - NCR 53C90 Advanced SCSI Controller (ASC) library (SCSI-2) */
    
/* Copyright 1989-1996 Wind River Systems, Inc. */

#include "copyright_wrs.h"
   
/*
modification history
--------------------
03k,03mar99,dat  added init of wideXfer to FALSE. SPR 24089
03j,11jul97,dgp  doc: correct INCLUDE FILES (delete ncr)
03i,09jul97,dgp  doc: corrects fonts per SPR 7853
03h,28mar97,dds  SPR 8220: added check for parity errors.
03g,29oct96,dgp  doc: editing for newly published SCSI libraries
03f,22aug96,dds  changed ncr5390minXferDmaThreshold to 
		 ncr5390MinXferDmaThreshold
03e,22aug96,dds  SPR#7067: added fix for single byte dma transfers.
03d,06may96,dds  fixed for SCSI1 & SCSI2 comapatibilty.
02c,25jan96,jds  fixed dma semaphore bug for frc5ce
02b,07jun95,ihw  major re-hash for enhanced SCSI library
    	         added partial support for advanced features of NCR53C9X
02a,22mar95,jds  rewrite for enhanced SCSI-2 library (event based)
01j,06mar95,js   fixed to support the latest WRS SCSI drivers.
01i,12may94,jkw  added support for SCSI-2
01h,23feb93,jdi  documentation cleanup.
01g,26sep92,ccc  doc changes, renamed ascShow to ncr5390Show and added example.
01f,13aug92,ccc  cleaned up warnings, added timeout.
01e,18jul92,smb  Changed errno.h to errnoLib.h.
01d,26may92,rrr  the tree shuffle
		  -changed includes to have absolute path from h/
01c,07oct91,rrr  some forward decls
01b,04oct91,rrr  passed through the ansification filter
		  -changed functions to ansi style
		  -changed includes to have absolute path from h/
		  -fixed #else and #endif
		  -changed VOID to void
		  -changed copyright notice
01a,19jan90,jcc  Written.
*/

/*
DESCRIPTION
This is the I/O driver for the NCR 53C90 Advanced SCSI Controller (ASC).
It is designed to work in conjunction with scsiLib.

USER-CALLABLE ROUTINES
Most of the routines in this driver are accessible only through the I/O
system. The only exception in this portion of the driver is the
ncr5390CtrlCreateScsi2() which creates a controller structure.

INCLUDE FILES
ncr5390.h

SEE ALSO: scsiLib,
.I "NCR 53C90A, 53C90B Advanced SCSI Controller,"
.pG "I/O System"
*/

#define INCLUDE_SCSI2
#include "vxWorks.h"
#define NCR5390_2_LOCAL_FUNCS
#include "drv/scsi/ncr5390.h"
#undef NCR5390_2_LOCAL_FUNCS
#include "cacheLib.h"
#include "memLib.h"
#include "errnoLib.h"
#include "logLib.h"
#include "scsiLib.h"
#include "semLib.h"
#include "stdlib.h"
#include "stdio.h"
#include "sysLib.h"
#include "intLib.h"
#include "tickLib.h"
#include "taskLib.h"

/* globals */

char * ncr5390ScsiTaskName       = SCSI_DEF_TASK_NAME;
int    ncr5390ScsiTaskOptions    = SCSI_DEF_TASK_OPTIONS;
int    ncr5390ScsiTaskPriority   = SCSI_DEF_TASK_PRIORITY;
int    ncr5390ScsiTaskStackSize  = SCSI_DEF_TASK_STACK_SIZE;

int    ncr5390XferDoneSemOptions = SEM_Q_PRIORITY;

BOOL   ncr5390XferDmaDataOnly    = FALSE;/* TRUE => DMA only for data xfers */
int    ncr5390MinXferDmaThreshold   = 1;	/* use DMA if xfer len > this value */

/* IMPORT  */

IMPORT BOOL scsiDebug;
IMPORT BOOL scsiIntsDebug;

/* forward declarations */

LOCAL void ascCommand (ASC *pAsc, int cmdCode);
LOCAL void ncr5390Intr (ASC *);
LOCAL void ascXferCountGet (FAST ASC *, FAST UINT *);
LOCAL STATUS ascXferCountSet (FAST ASC *, FAST UINT);
LOCAL STATUS ncr5390CtrlInitScsi2 (FAST ASC *, FAST int , FAST UINT, int);
LOCAL STATUS ncr5390ShowScsi2 ( FAST NCR_5390_SCSI_CTRL *);


LOCAL STATUS ascDevSelect (SCSI_CTRL * pScsiCtrl,
                           int         devBusId,
                           UINT        selTimeOut,
                           UINT8 *     msgBuf,
                           UINT        msgLen);
LOCAL void   ascSelTimeOutCvt (FAST ASC* pAsc, UINT timeOutInUsec,
			     UINT *pTimeOutSetting);
LOCAL void   ascHwInit (ASC *pAsc);
LOCAL int    ascChipTypeGet (ASC *pAsc);
LOCAL UINT   ascMaxBytesPerXferGet (int chipType);
LOCAL int    ascInfoXfer (SCSI_CTRL *pAsc, int phase,
			  UINT8 *pBuffer,  int bufLength);
LOCAL int    ascProgXfer (NCR_5390_SCSI_CTRL *pAsc, UINT8 *pBuffer, 
			  int bufLength, int direction);
LOCAL int    ascDmaXfer  (NCR_5390_SCSI_CTRL *pAsc, UINT8 *pBuffer,
			  int bufLength, int direction);
LOCAL BOOL   ascProgXferContinue (NCR_5390_SCSI_CTRL * pAsc, BOOL ackPending);
LOCAL UINT   ascBytesLeftGet (NCR_5390_SCSI_CTRL *pAsc);
LOCAL STATUS ascXferParamsSet (SCSI_CTRL *pScsiCtrl,
                               UINT8      offset,
                               UINT8      period);
LOCAL STATUS ascXferParamsQuery ();
LOCAL STATUS ascBusControl ();
LOCAL BOOL   ascXferParamsCvt ();



/*******************************************************************************
*
* ncr5390Scsi1IfInit - initialize the SCSI-2 interface to ncr5390
*
* NOMANUAL
*/
void ncr5390Scsi2IfInit ()
    {
    /* create table */

    ncr5390IfTblInit ();

    /* initialize table */

    pNcr5390IfTbl->ascCommand       = (FUNCPTR) ascCommand;
    pNcr5390IfTbl->ascIntr          = (FUNCPTR) ncr5390Intr;
    pNcr5390IfTbl->ascXferCountGet  = (FUNCPTR) ascXferCountGet;
    pNcr5390IfTbl->ascXferCountSet  = (FUNCPTR) ascXferCountSet;
    pNcr5390IfTbl->ncr5390CtrlInit   = (FUNCPTR) ncr5390CtrlInitScsi2;
    pNcr5390IfTbl->ncr5390Show       = (FUNCPTR) ncr5390ShowScsi2;
    }

/*******************************************************************************
*
* ncr5390CtrlCreateScsi2 - create a control structure for an NCR 53C90 ASC
*
* This routine creates a data structure that must exist before the ASC chip
* can be used.  This routine must be called exactly once for a specified
* ASC, and must be the first routine called, since it calloc's a structure
* needed by all other routines in the library.
*
* The input parameters are as follows:
* .iP <baseAdrs> 4
* the address at which the CPU would access the lowest 
* register of the ASC.
* .iP <regOffset>
* the address offset (bytes) to access consecutive registers.
* .iP <clkPeriod>
* the period, in nanoseconds, of the signal to the ASC clock input.
* .iP "<sysScsiDmaMaxBytes>, <sysScsiDmaStart>, <sysScsiDmaAbort>, and <sysScsiDmaArg>"
* board-specific routines to handle DMA transfers to and from the ASC;
* if the maximum DMA byte count is zero, programmed I/O is used.
* Otherwise, non-NULL function pointers to DMA start and abort routines
* must be provided.
* The specified argument is passed to these routines when they are
* called; it may be used to identify the DMA channel to use, for example.
* The interface to these DMA routines must be of the form:
* .CS
*     STATUS xxDmaStart (arg, pBuffer, bufLength, direction)
*
*         int arg;  	    	      /@ call-back argument           @/
*         UINT8 *pBuffer;             /@ ptr to the data buffer       @/
*         UINT bufLength;             /@ number of bytes to xfer      @/
*   	  int direction;    	      /@ 0 = SCSI->mem, 1 = mem->SCSI @/
*
*     STATUS xxDmaAbort (arg)
*
*         int arg;  	    	      /@ call-back argument @/
* .CE
* Implementation details for the DMA routines can be found in the
* specific DMA driver for that board.
*
* NOTE:
* If there is no DMA interface, synchronous transfers are not supported.
* This is a limitation of the NCR5390 hardware.
*
* RETURNS: A pointer to an NCR_5390_SCSI_CTRL structure,
* or NULL if memory is insufficient or the parameters are invalid.
*/

NCR_5390_SCSI_CTRL *ncr5390CtrlCreateScsi2
    (
    FAST UINT8* baseAdrs,           	/* base address of ASC               */
    int         regOffset,          	/* offset between consecutive regs.  */
    UINT        clkPeriod,          	/* period of controller clock (nsec) */
    UINT        sysScsiDmaMaxBytes, 	/* maximum byte count using DMA      */
    FUNCPTR     sysScsiDmaStart,	/* function to start SCSI DMA xfer   */
    FUNCPTR     sysScsiDmaAbort,	/* function to abort SCSI DMA xfer   */
    int         sysScsiDmaArg		/* argument to pass to above funcs.  */
    )
    {
    FAST ASC *pAsc;		/* ptr to ASC info */

    /* initialize the driver function table */

    ncr5390Scsi2IfInit ();

    /* verify parameters */

    if (regOffset == 0)
	return ((ASC *) NULL);

    if ((sysScsiDmaMaxBytes != 0) &&
	((sysScsiDmaStart == NULL) || (sysScsiDmaAbort == NULL)))
	return ((ASC *) NULL);
    
    /* calloc the controller info structure; return NULL if unable */

    pAsc = (ASC *) calloc (1, sizeof (ASC));

    if (pAsc == (ASC *) NULL)
        return ((ASC *) NULL);

    /* fill in ASC specific data for this controller */

    pAsc->pTclReg  = baseAdrs;
    pAsc->pTchReg  = baseAdrs + (0x1 * regOffset);
    pAsc->pFifoReg = baseAdrs + (0x2 * regOffset);
    pAsc->pCmdReg  = baseAdrs + (0x3 * regOffset);
    pAsc->pStatReg = baseAdrs + (0x4 * regOffset);
    pAsc->pIntrReg = baseAdrs + (0x5 * regOffset);
    pAsc->pStepReg = baseAdrs + (0x6 * regOffset);
    pAsc->pFlgsReg = baseAdrs + (0x7 * regOffset);
    pAsc->pCfg1Reg = baseAdrs + (0x8 * regOffset);
    pAsc->pClkReg  = baseAdrs + (0x9 * regOffset);
    pAsc->pTestReg = baseAdrs + (0xa * regOffset);
    pAsc->pCfg2Reg = baseAdrs + (0xb * regOffset);
    pAsc->pCfg3Reg = baseAdrs + (0xc * regOffset);  /* if present !! */
    pAsc->pTcxReg  = baseAdrs + (0xe * regOffset);  /* if present !! */

    pAsc->clkPeriod = clkPeriod;

    /* determine chip type, etc. */

    pAsc->chipType =  ascChipTypeGet (pAsc);

    pAsc->maxBytesPerXfer = ascMaxBytesPerXferGet (pAsc->chipType); 

    /*
     *  Set up sizes of event and thread structures.  Must be done before
     *  calling "scsiCtrlInit()".
     *
     *  Since this controller uses standard event and thread structures,
     *  it could simply leave these values set to 0 (i.e., use default).
     */

    pAsc->scsiCtrl.eventSize  = sizeof (SCSI_EVENT);
    pAsc->scsiCtrl.threadSize = sizeof (SCSI_THREAD);

    /* leave transact, event and thread management routines as default */

    /* fill in driver-specific routines for scsiLib interface */

    pAsc->scsiCtrl.scsiDevSelect       = (FUNCPTR) ascDevSelect;
    pAsc->scsiCtrl.scsiInfoXfer        = (FUNCPTR) ascInfoXfer;
    pAsc->scsiCtrl.scsiBusControl      = (FUNCPTR) ascBusControl;
    pAsc->scsiCtrl.scsiXferParamsQuery = (FUNCPTR) ascXferParamsQuery;
    pAsc->scsiCtrl.scsiXferParamsSet   = (FUNCPTR) ascXferParamsSet;

    pAsc->scsiCtrl.wideXfer	       = FALSE;
    pAsc->scsiCtrl.scsiWideXferParamsQuery = NULL;
    pAsc->scsiCtrl.scsiWideXferParamsSet   = NULL;

    /* fill in driver-specific variables for scsiLib interface */

    if ((sysScsiDmaMaxBytes != 0) &&
	(sysScsiDmaMaxBytes <= pAsc->maxBytesPerXfer))
	pAsc->scsiCtrl.maxBytesPerXfer = sysScsiDmaMaxBytes;
    else
	pAsc->scsiCtrl.maxBytesPerXfer = pAsc->maxBytesPerXfer;

    /* fill in generic SCSI info for this controller */

    scsiCtrlInit (&pAsc->scsiCtrl);

    /* create ASC synchronisation semaphore */

    if ((pAsc->xferDoneSem = semBCreate (ncr5390XferDoneSemOptions, SEM_EMPTY))
        == NULL)
        {
        SCSI_MSG ("ncr5390CtrlCreate: semBCreate of xferDoneSem failed\n",
                  0, 0, 0, 0, 0, 0);
        (void) free ((char *) pAsc);
        return ((NCR_5390_SCSI_CTRL *) NULL);
        }

    /* initialise state variables */

    pAsc->state          = ASC_STATE_IDLE;
    pAsc->reqPhase       = NONE;
    pAsc->xferPending    = FALSE;
    pAsc->xferDirection  = NULL;
    pAsc->xferUsingDma   = FALSE;
    pAsc->syncDataXfer   = FALSE;

    /* fill in board-specific DMA xfer routines */

    pAsc->sysScsiDmaStart = sysScsiDmaStart;
    pAsc->sysScsiDmaAbort = sysScsiDmaAbort;
    pAsc->sysScsiDmaArg	  = sysScsiDmaArg;

    /* disable synchronous transfer if no DMA routines provided */

    if (pAsc->sysScsiDmaStart == 0)
	pAsc->scsiCtrl.syncXfer = FALSE;

    /* spawn SCSI manager - use generic code from "scsiLib.c" */

    pAsc->scsiCtrl.scsiMgrId = taskSpawn (ncr5390ScsiTaskName,
                                          ncr5390ScsiTaskPriority,
                                          ncr5390ScsiTaskOptions,
                                          ncr5390ScsiTaskStackSize,
                                          (FUNCPTR) scsiMgr,
                                          (int) pAsc,
                                          0, 0, 0, 0, 0, 0, 0, 0, 0);

    if (pAsc->scsiCtrl.scsiMgrId == ERROR)
        {
        SCSI_MSG ("ncr5390CtrlCreate: can't spawn SCSI manager task\n",
                  0, 0, 0, 0, 0, 0);
        free ((char *) pAsc);
        return ((ASC *) NULL);
        }
    
    return (pAsc);
    }

/*******************************************************************************
*
* ncr5390CtrlInitScsi2 - initialize a control structure for an NCR 53C90 ASC
*
* This routine initializes an ASC structure created by ncr5390CtrlCreate().
* It must be called before the ASC is used.  This routine can be called more
* than once; however, it should be called only while there is no activity
* on the SCSI interface, since the specified configuration 
* is written to the ASC.
*
* The input parameters are as follows:
* .iP `pAsc' 4
* a pointer to an NCR_5390_SCSI_CTRL structure created with
* ncr5390CtrlCreate().
* .iP `scsiCtrlBusId'
* the SCSI bus ID of the ASC, in the range 0 - 7.  The ID is somewhat arbitrary;
* the value 7, or highest priority, is conventional.
* .iP `defaultSelTimeOut'
* the timeout, in microseconds, for selecting a SCSI device attached to this
* controller.  This value is used as a default if no timeout is specified in 
* scsiPhysDevCreate()).  The recommended value zero (0)
* specifies SCSI_DEF_SELECT_TIMEOUT (250 millisec).  The maximum timeout
* possible is approximately 3 seconds.  Values exceeding this revert to the
* maximum.  For more information about chip timeouts, see the manual
* .I "NCR 53C90A, 53C90B Advanced SCSI Controller."
*
* RETURNS: OK, or ERROR if a parameter is out of range.
*
* SEE ALSO: scsiPhysDevCreate(),
* .I "NCR 53C90A, 53C90B Advanced SCSI Controller"
*/

LOCAL STATUS ncr5390CtrlInitScsi2
    (
    FAST NCR_5390_SCSI_CTRL *pAsc, /* ptr to ASC struct */
    FAST int  scsiCtrlBusId,       /* SCSI bus ID of this ASC */
    FAST UINT defaultSelTimeOut,    /* default dev sel timeout (microsec) */
    int scsiPriority
    )
    {

    /* verify scsiCtrlBusId and enter legal value in ASC structure */

    if (scsiCtrlBusId < SCSI_MIN_BUS_ID || scsiCtrlBusId > SCSI_MAX_BUS_ID)
	return (ERROR);

    pAsc->scsiCtrl.scsiCtrlBusId = (UINT8) scsiCtrlBusId;

    if (defaultSelTimeOut == 0)
        pAsc->defSelTimeOut = SCSI_DEF_SELECT_TIMEOUT;
    else
        pAsc->defSelTimeOut = defaultSelTimeOut;

    ascHwInit (pAsc); 	/* initialize the ASC hardware */

    return (OK);
    }

/******************************************************************************
*
* ascDevSelect - attempt to select a SCSI physical device
*
* This routine is intended to be called from scsiLib, not directly.
*
* RETURNS: OK if device was successfully selected, otherwise ERROR.
*/

LOCAL STATUS ascDevSelect
    (
    SCSI_CTRL *pScsiCtrl,   	    	/* ptr to SCSI controller info     */
    int        devBusId,                /* SCSI bus ID of device to select */
    UINT       selTimeOut,              /* select t-o period (usec)        */
    UINT8 *    msgBuf,                  /* ptr to identification message   */
    UINT       msgLen                   /* maximum number of message bytes */
    )
    {
    FAST ASC *pAsc = (ASC *)pScsiCtrl;	/* ptr to ASC info */
    STATUS    status;
    UINT      cvtSelTimeOut;    /* converted value of select time-out */
    int       lockKey;
    UINT      selectCmd;

    if (devBusId == pScsiCtrl->scsiCtrlBusId)
	{
        errnoSet (S_scsiLib_ILLEGAL_BUS_ID);
        return (ERROR);
	}

    /* convert the time to ASC units */

    ascSelTimeOutCvt (pAsc, selTimeOut, &cvtSelTimeOut);

    /*
     *  Assume select with attention always. However, we can have a one
     *  byte identify msg or a three byte msg out for tagged command 
     *  queueing. The NCR5390 controller transfers either one byte or
     *  three bytes right after selection is over depending on the type
     *  of select issued.
     *
     *	Currently, only the first case (automatic transfer of one byte of
     *	identification) is supported.  The generic scsiLib code will ensure
     *	that any remaining identification bytes are sent "manually".
     */
    switch (msgLen)
	{
	case 1:
	    selectCmd = NCR5390_SEL_ATN_STOP;
	    break;

	case 3:
	    selectCmd = NCR5390_SEL_ATN_STOP;	/* sic */
	    break;

    	case 0:
	    logMsg ("ascDevSelect: select without ATN not supported.\n",
		    0, 0, 0, 0, 0, 0);
	    return (ERROR);
	    
	default:
            logMsg ("ascDevSelect: invalid identify message length (%d)\n",
		    msgLen, 0, 0, 0, 0, 0);
	    return (ERROR);
	}

    /*
     *  Check ASC is IDLE, then issue SELECT command.  Must be done with
     *  interrupts locked to avoid races with the reselection ISR.
     *
     *  If the controller is not IDLE, do not issue a select command.  Just
     *  return - there must be a (re)selection event pending which will be
     *  handled exactly as if it occurred just after the select command was
     *  issued.
     */
    lockKey = intLock ();

    switch (pAsc->state)
        {
        case ASC_STATE_IDLE:
            pAsc->state     = ASC_STATE_SELECT_PENDING;
            *pAsc->pBidReg  = (UINT8) devBusId;
            pAsc->selBusId  = devBusId;
            *pAsc->pTmoReg  = (UINT8) cvtSelTimeOut;
            *pAsc->pFifoReg = *msgBuf;
            ascCommand (pAsc, selectCmd);  /* issue "select" command */
            status = OK;
            break;

        case ASC_STATE_CONNECTED:
            status = OK;
            break;

        case ASC_STATE_SELECT_PENDING:
        default:
            status = ERROR;
            break;
        }

    intUnlock (lockKey);

    if (status != OK)
        {
        SCSI_DEBUG_MSG ("ascDevSelect: invalid controller state (%d)\n",
                        pAsc->state, 0, 0, 0, 0, 0);
        }

    return (status);
    }

/******************************************************************************
*
* ascBusControl - miscellaneous low-level SCSI bus control operations
*
* Allows the caller to:-
*
*   - assert the SCSI ATN signal (indicating a message out is available)
*   - negate the SCSI ACK signal (indicating a message in has been read)
*   - reset the SCSI bus
*
* The "operation" parameter is a bitmask which allows any combination of these
* functions to be carried out with a single call.  Note that the order in
* which things are done is important: ATN is asserted (if specified) before
* ACK is negated.  (This allows an incoming SCSI message to be rejected.)
*
* RETURNS: OK, or ERROR if connection ID is stale.
*/

LOCAL STATUS ascBusControl
    (
    SCSI_CTRL *pScsiCtrl,       /* ptr to ASC info*/
    int        operation        /* bitmask for operation(s) to be performed */
    )
    {
    FAST ASC *pAsc = (ASC *) pScsiCtrl;

    if (operation & SCSI_BUS_RESET)
	{
	ascCommand ((ASC *) pAsc, NCR5390_BUS_RESET);
	SCSI_DEBUG_MSG ("ascBusControl: BUS RESET\n", 0, 0, 0, 0, 0, 0);
	}
    
    if (operation & SCSI_BUS_ASSERT_ATN) 
        {
        ascCommand ((ASC *) pAsc, NCR5390_SET_ATTENTION);  
        SCSI_DEBUG_MSG  ("ascBusControl: ASSERT ATN \n ", 0, 0, 0, 0, 0, 0);
        }
    
    if (operation & SCSI_BUS_NEGATE_ACK)
        {
        ascCommand ((ASC *) pAsc, NCR5390_MSG_ACCEPTED);
        SCSI_DEBUG_MSG  ("ascBusControl: NEGATE ACK \n", 0, 0, 0, 0, 0, 0); 
        }
    return (OK);
    }

/******************************************************************************
*
* ascXferParamsQuery - get (synchronous) transfer parameters
*
* Updates the synchronous transfer parameters suggested in the call to match
* the ASC's capabilities.  Transfer period is in SCSI units (multiples of
* 4 ns).
*
* Note: the transfer period is made longer and the offset is made smaller if
* the ASC cannot handle the specified values.
*
* RETURNS: OK
*/

LOCAL STATUS ascXferParamsQuery
    (
    SCSI_CTRL *pScsiCtrl,               /* ptr to ASC info              */
    UINT8     *pOffset,                 /* max REQ/ACK offset  [in/out] */
    UINT8     *pPeriod                  /* min transfer period [in/out] */
    )
    {
    UINT8 unused;

    (void) ascXferParamsCvt ((ASC *) pScsiCtrl, pOffset, pPeriod, 
                             &unused, &unused);
    return (OK);
    }

/*******************************************************************************
*
* ascXferParamsSet - set transfer parameters
*
* Programmes the ASC to use the specified transfer parameters.  An offset
* of zero specifies asynchronous transfer (period is then irrelevant).
* Transfer period is in SCSI units (multiples of 4 ns).
*
* RETURNS: OK if transfer parameters are OK, else ERROR.
*/

LOCAL STATUS ascXferParamsSet
    (
    SCSI_CTRL *pScsiCtrl,               /* ptr to ASC info        */
    UINT8      offset,                  /* max REQ/ACK offset     */
    UINT8      period                   /* min transfer period    */
    )
    {
    ASC * pAsc = (ASC *) pScsiCtrl;
    UINT8 periodRegVal;
    UINT8 offsetRegVal;

    if (!ascXferParamsCvt (pAsc, &offset, &period, &periodRegVal,
			                           &offsetRegVal))
        {                               /* should never happen */
        errnoSet (S_scsiLib_ILLEGAL_PARAMETER);
        return (ERROR);
        }

    pAsc->syncDataXfer = (offsetRegVal != 0);

    *pAsc->pStepReg = periodRegVal;
    *pAsc->pFlgsReg = offsetRegVal;
    
    return (OK);
    }

/*******************************************************************************
*
* ascInfoXfer - transfer information bytes to/from target via SCSI bus
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

LOCAL int ascInfoXfer
    (
    FAST SCSI_CTRL *pScsiCtrl,          /* ptr to SCSI controller info       */
    int             phase,              /* current phase for info transfer   */
    FAST UINT8     *pBuf,               /* ptr to byte buffer for i/o        */
    int             bufLength           /* number of bytes to be transferred */
    )
    {
    FAST ASC  *pAsc;
    UINT       bytesDone;
    int        direction;
    BOOL       useDma;

    pAsc = (ASC *) pScsiCtrl;

    SCSI_DEBUG_MSG ("ascInfoXfer: phase = %s, buf = 0x%x, length = %d\n",
		    (int) scsiPhaseNameGet (phase), (int) pBuf, bufLength,
		    0, 0, 0);

    /* Silently filter out null transfers */
    
    if (bufLength == 0)
	return (0);
    
    /* Identify phase, set transfer mode accordingly */

    useDma = ((pAsc->sysScsiDmaStart != 0) &&
	      (bufLength >= ncr5390MinXferDmaThreshold));

    switch (phase)
        {
        case SCSI_DATA_OUT_PHASE:
	    direction = WRITE;
	    break;

        case SCSI_DATA_IN_PHASE:
	    direction = READ;
	    break;

        case SCSI_COMMAND_PHASE:
        case SCSI_MSG_OUT_PHASE:
	    direction = WRITE;
	    
	    if (ncr5390XferDmaDataOnly)
		useDma = FALSE;
    	    break;
	    
        case SCSI_STATUS_PHASE:
        case SCSI_MSG_IN_PHASE:
	    direction = READ;
	    
	    if (ncr5390XferDmaDataOnly)
		useDma = FALSE;
	    break;

        default:
            SCSI_MSG ("ascInfoXfer: invalid bus phase (%d)\n", phase,
                      0, 0, 0, 0, 0);
            return (ERROR);
        }

    pAsc->xferPhase = phase;

    bytesDone = (useDma) ? ascDmaXfer  (pAsc, pBuf, bufLength, direction)
	    	    	 : ascProgXfer (pAsc, pBuf, bufLength, direction);

    SCSI_DEBUG_MSG ("ascInfoXfer: transferred %d of %d bytes.\n",
	            bytesDone, bufLength, 0, 0, 0, 0);

    return (bytesDone);
    }

/*******************************************************************************
*
* ascProgXfer - transfer bytes between SCSI and memory using programmed i/o
*
* Execute a programmed i/o transfer between memory and the ASC's FIFO.
* Return when the transfer is complete, or terminated (e.g., by a change
* in phase requested by the target).
*
* If this is a write operation, fill the FIFO with as much data as possible.
* Then, for either a read or write, issue a Transfer Info command; this will
* transfer one byte (read) or the number of bytes in the FIFO (write) before
* generating an interrupt.  Block on the "transfer done" sempahore.
*
* The interrupt service routine continues the transfer until it is complete
* or terminates, at which time the sempahore is given.  The state variables
* in the ASC structure now reflect how many bytes were _not_ transferred
* across the SCSI bus (0 if the transfer completes normally).
*
* RETURNS: Number of bytes transferred on the SCSI bus, or ERROR.
*
* NOMANUAL
*/

LOCAL int ascProgXfer
    (
    ASC   * pAsc,			/* ptr to ASC controller info        */
    UINT8 * pBuffer,			/* ptr to the data buffer            */
    int     bufLength,			/* number of bytes to be transferred */
    int     direction	    	    	/* READ: SCSI->mem, WRITE: mem->SCSI */
    )
    {
    int bytesLeft;
    
    /* Set state variables for ISR */

    pAsc->xferUsingDma  = FALSE;
    pAsc->xferDirection = direction;
    pAsc->xferNBytes    = bufLength;
    pAsc->xferAddr      = pBuffer;
    pAsc->xferPending   = TRUE;

    /* If a write operation, pre-fill the FIFO */

    if (direction == WRITE)
	{
    	FAST int     nBytes = min (bufLength, NCR5390_FIFO_DEPTH);
	FAST UINT8 * p      = pBuffer;

	pAsc->xferAddr   += nBytes;
	pAsc->xferNBytes -= nBytes;

	while (nBytes-- > 0)
            *pAsc->pFifoReg = *p++;
	}

    /* Set off the transfer (continued in ISR); wait until finished */
    
    ascCommand (pAsc, NCR5390_INFO_TRANSFER);

    if (semTake (pAsc->xferDoneSem, WAIT_FOREVER) == ERROR)
        {
	logMsg ("ascProgXfer: semTake failed.\n", 0, 0, 0, 0, 0, 0);

	return (ERROR);
	}

    bytesLeft = ascBytesLeftGet (pAsc);
    
    return (bufLength - bytesLeft);
    }

/*******************************************************************************
*
* ascDmaXfer - transfer bytes between SCSI and memory using DMA
*
* Execute a DMA transfer between memory and the ASC's FIFO.
* Return when the transfer is complete, or terminated (e.g., by a change
* in phase requested by the target).
*
* Ensure that any buffers are made cache coherent for the DMA controller.
* The only buffers that need any action here are those used for message
* phase transfers, as all others are handled by scsiCacheSynchronize().
*
* Set the ASC's transfer counter, then issue a Transfer Info command and
* start the DMA.  The ASC's Transfer Info command will stop when the whole
* buffer has been transferred, or if the target changes phase or disconnects.
* Block on the "transfer done" sempahore, which is given by the ISR when the
* transfer stops.
*
* After the transfer, work out how many bytes were not transferred across
* the SCSI bus; if this is not zero (i.e., the transfer terminated early),
* abort the DMA operation.
*
* RETURNS: Number of bytes transferred on the SCSI bus, or ERROR.
*
* NOMANUAL
*/

LOCAL int ascDmaXfer
    (
    ASC   * pAsc,			/* ptr to ASC controller info        */
    UINT8 * pBuffer,			/* ptr to the data buffer            */
    int     bufLength,			/* number of bytes to be transferred */
    int     direction	    	    	/* READ: SCSI->mem, WRITE: mem->SCSI */
    )
    {
    int bytesLeft;

    /* Do whatever is required to ensure cache coherency */

    {
    void * addr = (void *) pBuffer;
    int    len  = bufLength;
    
#ifdef	SPARC
    /*
     *	cacheClear() occasionally crashes on microSPARC, unless the entire
     *	cache is cleared.  This is a reliable (if sledgehammer) work-around.
     */
    addr = 0;
    len  = ENTIRE_CACHE;
#endif

    switch (pAsc->xferPhase)
	{
	case SCSI_MSG_IN_PHASE:		/* flush & invalidate cache */
	    	CACHE_USER_FLUSH      (addr, len);
		CACHE_USER_INVALIDATE (addr, len);
	    break;

	case SCSI_MSG_OUT_PHASE:	/* flush cache */
	    CACHE_USER_FLUSH (addr, len);
	    break;

	default:
	    break;
	}
    }
    
    /* Set DMA transfer counter */

    if (ascXferCountSet (pAsc, bufLength) != OK)
	{
    	errnoSet (S_scsiLib_ILLEGAL_PARAMETER);
	return (ERROR);
	}
    
    ascCommand (pAsc, NCR5390_DMA_OP | NCR5390_NOP);

    /* Set state variables for ISR */

    pAsc->xferUsingDma  = TRUE;
    pAsc->xferDirection = direction;
    pAsc->xferNBytes    = bufLength;
    pAsc->xferAddr      = pBuffer;
    pAsc->xferPending   = TRUE;

    /* Set off the transfer, start the DMA; wait until finished */
    
    ascCommand (pAsc, NCR5390_DMA_OP | NCR5390_INFO_TRANSFER);

    (*pAsc->sysScsiDmaStart) (pAsc->sysScsiDmaArg,  pBuffer, bufLength,
			      direction);

    if (semTake (pAsc->xferDoneSem, WAIT_FOREVER) == ERROR)
        {
	logMsg ("ascDmaXfer: semTake failed.\n", 0, 0, 0, 0, 0, 0);

	return (ERROR);
	}

    /* If the transfer terminated early, kill the DMA */

    bytesLeft = ascBytesLeftGet (pAsc);
    
    if (bytesLeft != 0)
	(*pAsc->sysScsiDmaAbort) (pAsc->sysScsiDmaArg);

    return (bufLength - bytesLeft);
    }

/*******************************************************************************
*
* ascXferParamsCvt - convert transfer period from SCSI to ASC units
*
* Given a "suggested" REQ/ACK offset and transfer period (in SCSI units of
* 4 ns), return the nearest offset and transfer period the ASC is capable of
* using.  Also return the corresponding value of the ASC's Synchronous
* Transfer register.
*
* An offset of zero specifies asynchronous transfer, in which case the period
* is irrelevant.  Otherwise, the offset may be clipped to be within the
* maximum limit the ASC is capable of.
*
* The transfer period is normally rounded towards longer periods if the ASC
* is not capable of using the exact specified value.  The period may also be
* clipped to be within the ASC's maximum and minimum limits according to its
* clock period.
*
* If either the offset or period need to be clipped, the value FALSE is
* retuned as this may reflect an error condition.
*
* NOTE:
* If there is no DMA interface, synchronous transfers are not supported.
* This is a limitation of the NCR5390 hardware.
*
* RETURNS: TRUE if input period is within ASC's range, else FALSE
*/

LOCAL BOOL ascXferParamsCvt
    (
    FAST ASC   * pAsc,			/* ptr to ASC info                   */
    FAST UINT8 * pOffset,		/* REQ/ACK offset                    */
    FAST UINT8 * pPeriod,		/* xfer period, SCSI units (x 4 ns)  */
    UINT8      * pPeriodRegVal,		/* ptr to sync period register value */
    UINT8      * pOffsetRegVal		/* ptr to sync offset register value */
    )
    {  
    UINT  clkCycles;
    UINT8 offset  = *pOffset;
    UINT8 period  = *pPeriod;
    BOOL  inRange = TRUE;

    SCSI_DEBUG_MSG ("ascXferParamsCvt: requested offset %d, period %d.\n", 
                     offset, period, 0, 0, 0, 0);

    if (offset == SCSI_SYNC_XFER_ASYNC_OFFSET)
    	{
    	offset    = 0;
    	clkCycles = 0;
    	}

    else if (pAsc->sysScsiDmaStart == 0)
    	{
	/* Should never get here: see ncr5390CtrlCreate() */

	logMsg ("ascXferParamsCvt: sync xfer unsupported (no DMA).\n",
		0, 0, 0, 0, 0, 0);

	offset    = 0;
	clkCycles = 0;

	inRange = FALSE;
	}

    else
        {
        /*
         *  Round (and perhaps clip) transfer period to suit ASC
         *
         *  Note: split into two stages to avoid associativity problems,
         *      and careful to round up rather than truncate result.
         */

        clkCycles  = (period * 4) + (pAsc->clkPeriod - 1);
        clkCycles /= pAsc->clkPeriod;

        if (clkCycles < ASC_MIN_XFER_PERIOD)
            {
            clkCycles = ASC_MIN_XFER_PERIOD;
            inRange   = FALSE;
            }

        else if (clkCycles > ASC_MAX_XFER_PERIOD)  /* very unlikely ! */
            {
            clkCycles = ASC_MAX_XFER_PERIOD;
            inRange   = FALSE;
            }

        period = (clkCycles * pAsc->clkPeriod) / 4;

        /* Clip offset (if necessary) to suit ASC */

        if (offset > ASC_MAX_REQ_ACK_OFFSET)
            {
            offset  = ASC_MAX_REQ_ACK_OFFSET;
            inRange = FALSE;
            }
        }

    SCSI_DEBUG_MSG ("ascXferParamsCvt: granted   offset %d, period %d.\n",
		    offset, period, 0, 0, 0, 0);

    *pOffset = offset;
    *pPeriod = period;
    
    *pPeriodRegVal = clkCycles & 0x1f;
    *pOffsetRegVal = offset    & 0x0f;

    return (inRange);
    }

/*******************************************************************************
*
* ascSelTimeOutCvt - convert a select time-out in microseconds to its
*		     equivalent ASC setting
*
* The conversion formula is given on p. 15 of the 1989 NCR 53C90A / 53C90B
* manual.  Note that 0 translates to the standard setting of 250 microsec.
* Also, the ASC accepts up to a 8 bit time-out, so a maximum value of 0xff is
* returned in *pTimeOutSetting.
*/

LOCAL void ascSelTimeOutCvt
    (
    FAST ASC  * pAsc,			/* ptr to SCSI controller info */
    FAST UINT   timeOutInUsec,		/* time-out in microsecs       */
    FAST UINT * pTimeOutSetting		/* time-out equivalent setting */
    )
    {
    FAST UINT selTimeOut;		/* temp. select time-out setting */
    FAST UINT clkCvtFactor;		/* clock conversion factor       */

    if (timeOutInUsec == 0)
	timeOutInUsec = SCSI_DEF_SELECT_TIMEOUT;

    if ((clkCvtFactor = pAsc->clkCvtFactor) == 0)
	clkCvtFactor = 8;

    selTimeOut = ((timeOutInUsec * 1000) /
		  (8192 * clkCvtFactor * pAsc->clkPeriod)) + 1;

    if (selTimeOut > 0xff)
	selTimeOut = 0xff;

    *pTimeOutSetting = selTimeOut;
    }

/*******************************************************************************
*
* ascXferCountSet - load the ASC transfer counter with the specified count
*
* RETURNS: OK if count is in range 0 - 0xffff, otherwise ERROR.
*
* NOMANUAL
*/

LOCAL STATUS ascXferCountSet
    (
    FAST ASC * pAsc,			/* ptr to ASC info     */
    FAST UINT  count			/* count value to load */
    )
    {
    if (count == 0 || count > pAsc->maxBytesPerXfer)
        return (ERROR);

    if (pAsc->chipType == ASC_NCR53C9X)
	*pAsc->pTcxReg = (UINT8) ((count >> 16) & 0xff);
    
    *pAsc->pTchReg = (UINT8) ((count >> 8) & 0xff);
    *pAsc->pTclReg = (UINT8)  (count       & 0xff);

    return (OK);
    }

/*******************************************************************************
*
* ascXferCountGet - fetch the ASC transfer count
*
* RETURNS: The value of the transfer counter is returned in *pCount.
*
* NOMANUAL
*/

LOCAL void ascXferCountGet
    (
    FAST ASC  * pAsc,			/* ptr to ASC info       */
    FAST UINT * pCount			/* ptr to returned value */
    )
    {
    FAST UINT count;

    count = ((*pAsc->pTchReg & 0xff) << 8) | (*pAsc->pTclReg & 0xff);

    if (pAsc->chipType == ASC_NCR53C9X)
	count |= ((*pAsc->pTcxReg & 0xff) << 16);

    *pCount = count;
    }

/*******************************************************************************
*
* ascCommand - write a command code to the ASC Command Register
*
* NOMANUAL
*/

LOCAL void ascCommand
    (
    FAST ASC *pAsc,     /* ptr to ASC info  */
    FAST int cmdCode    /* new command code */
    )
    {
    *pAsc->pCmdReg = (UINT8) cmdCode;
    }

/*******************************************************************************
*
* ncr5390Intr - interrupt service routine for the ASC
*
* NOMANUAL
*/

LOCAL void ncr5390Intr
    (
    ASC *pAsc          /* ptr to ASC info */
    )
    {
    SCSI_EVENT event;
    BOOL       xferComplete = FALSE;
    UINT8      savedStatReg = *pAsc->pStatReg;
    UINT8      savedIntrReg = *pAsc->pIntrReg;
    int        phase        = (savedStatReg & ASC_STAT_BITS);
    int        state        = pAsc->state;

    SCSI_INT_DEBUG_MSG ("ncr5390Intr: statReg = 0x%02x, intrReg = 0x%02x\n",
                        savedStatReg, savedIntrReg, 0, 0, 0, 0);

    event.type = NONE;

    /* ILLEGAL COMMAND */

    if (savedIntrReg & NCR5390_ILLEGAL_CMD)
        {
        logMsg ("ncr5390Intr: ILLEGAL COMMAND\n", 0, 0, 0, 0, 0, 0);
        }

    /* RESET */

    else if (savedIntrReg & NCR5390_SCSI_RESET)
        {
        SCSI_INT_DEBUG_MSG ("ncr5390Intr: RESET\n", 0, 0, 0, 0, 0, 0);

    	event.type = SCSI_EVENT_BUS_RESET;

    	state = ASC_STATE_IDLE;
        }

    /* RESELECTED */

    else if (savedIntrReg & NCR5390_RESELECTED)
    	{
    	int   busId;
    	UINT8 reselDataBits;
    	UINT8 identMsg;

	SCSI_INT_DEBUG_MSG ("ncr5390Intr: RESELECTED\n", 0, 0, 0, 0, 0, 0);

    	reselDataBits = *pAsc->pFifoReg;
    	identMsg      = *pAsc->pFifoReg;

        reselDataBits &= ~(1 << pAsc->scsiCtrl.scsiCtrlBusId);

    	for (busId = SCSI_MIN_BUS_ID; busId <= SCSI_MAX_BUS_ID; ++busId)
    	    {
	    if ((reselDataBits & (1 << busId)) != 0)
		break;
	    }

    	if (busId > SCSI_MAX_BUS_ID)
    	    {
	    logMsg ("ncr5390Intr: invalid bus ID on reselection\n",
		    0, 0, 0, 0, 0, 0);
	    }

	event.type        = SCSI_EVENT_RESELECTED;
	event.busId       = busId;
	event.identMsg[0] = identMsg;
	event.nBytesIdent = 1;

	state = ASC_STATE_CONNECTED;
    	}

    /* SELECT TIMEOUT */

    else if ((state == ASC_STATE_SELECT_PENDING) &&
    	    (savedIntrReg & NCR5390_DISCONNECTED))
    	{
	SCSI_INT_DEBUG_MSG ("ncr5390Intr: SELECT TIMEOUT\n", 0, 0, 0, 0, 0, 0);

	ascCommand (pAsc, NCR5390_FIFO_FLUSH);	/* flush unused identify msg */
	
    	ascCommand (pAsc, NCR5390_SELECTION_ENBL);
    
	event.type = SCSI_EVENT_TIMEOUT;
	
	state = ASC_STATE_IDLE;
	}

    /* SELECT (FUNCTION) COMPLETE */
		
    else if ((state == ASC_STATE_SELECT_PENDING) &&
    	    (savedIntrReg & NCR5390_FUNC_COMPLETE))
	{
        SCSI_INT_DEBUG_MSG ("ncr5390Intr: SELECT COMPLETE\n", 0, 0, 0, 0, 0, 0);

        /* SELECT with ATN and STOP transfers 1 byte of identify MSG_OUT */

	/*
	 *  Need to post a selection event here, so we can later
	 *  post a transfer request event (see end of ascIntr()).
	 */
        event.type        = SCSI_EVENT_CONNECTED;
        event.busId       = pAsc->selBusId;
        event.nBytesIdent = 1; 

        scsiMgrEventNotify ((SCSI_CTRL *)pAsc, &event, sizeof (event));

	if (savedIntrReg & NCR5390_BUS_SERVICE)
	    {
	    event.type  = SCSI_EVENT_XFER_REQUEST;
   	    event.phase = phase;
	    }

    	state = ASC_STATE_CONNECTED;
	}

    /* DISCONNECTED */

    else if ((state == ASC_STATE_CONNECTED) &&
    	     (savedIntrReg & NCR5390_DISCONNECTED))
    	{
    	SCSI_INT_DEBUG_MSG ("ncr5390Intr: DISCONNECTED\n", 0, 0, 0, 0, 0, 0);
    
    	ascCommand (pAsc, NCR5390_SELECTION_ENBL);
    
    	event.type = SCSI_EVENT_DISCONNECTED;
    
    	if (pAsc->xferPending)
	    xferComplete = TRUE;
    
    	state = ASC_STATE_IDLE;
    	}

    /* BUS SERVICE */

    else if ((state == ASC_STATE_CONNECTED) &&
    	    (savedIntrReg & NCR5390_BUS_SERVICE))
    	{
        SCSI_INT_DEBUG_MSG ("ncr5390Intr: BUS SERVICE (phase %d)\n",
			    phase, 0, 0, 0, 0, 0);

	event.type  = SCSI_EVENT_XFER_REQUEST;	/* but see later ... */
   	event.phase = phase;
	
	pAsc->reqPhase = phase;

	if (!pAsc->xferPending)
	    {
	    /* this is a new transfer request */
	    }
        
        else if (pAsc->xferUsingDma)
	    {
	    xferComplete = TRUE;
	    }

	else				/* programmed i/o transfer */
	    {
	    xferComplete = ascProgXferContinue (pAsc, FALSE);

	    if (!xferComplete)
		{
		event.type = NONE;	/* suppress xfer request event */
		}
	    }
    	}

    /* FUNCTION COMPLETE */

    else if ((state == ASC_STATE_CONNECTED) &&
    	     (savedIntrReg & NCR5390_FUNC_COMPLETE))
        {
        SCSI_INT_DEBUG_MSG ("ncr5390Intr: FUNCTION COMPLETE\n",
			    0, 0, 0, 0, 0, 0);

        if (!(pAsc->xferPending && (pAsc->xferPhase == NCR5390_MSGIN_PHASE)))
	    {
            logMsg ("ncr5390Intr: Unexpected Function Complete interrupt.\n",
		    0, 0, 0, 0, 0, 0);
	    }

        else if (pAsc->xferUsingDma)
	    {
            xferComplete = TRUE;
	    }

        else
            {
	    xferComplete = ascProgXferContinue (pAsc, TRUE);
	    }
        }

    /*  Handle state transition, if any */

    if (state != pAsc->state)
        {
        SCSI_INT_DEBUG_MSG ("ncr5390Intr: ASC state transition: %d -> %d\n",
                            pAsc->state, state, 0, 0, 0, 0);
        pAsc->state = state;
        }

    if (xferComplete)
	{
	pAsc->xferPending = FALSE;
	semGive (pAsc->xferDoneSem);
	}

    /* PARITY ERROR */

    if (savedStatReg & NCR5390_PARITY_ERR)
        {
        logMsg ("ncr5390Intr: PARITY ERROR\n", 0, 0, 0, 0, 0, 0);
	ascHwInit (pAsc);
	ascBusControl ((SCSI_CTRL *) pAsc, SCSI_BUS_RESET);
	event.type = SCSI_EVENT_PARITY_ERR; 
        }

    /*
     *  Post event to SCSI manager for further processing
     */
    if (event.type != NONE)
        scsiMgrEventNotify ((SCSI_CTRL *)pAsc, &event, sizeof (event));
    }

/*******************************************************************************
* ascProgXferContinue - continue a programmed i/o transfer
*
* This routine is expected to be called by the ASC ISR to continue a
* programmed i/o transfer started by ascProgXfer().
*
* If the transfer is a read from SCSI, and there is (at least) one byte in
* the FIFO, read it.  (If there are any more bytes, they may be from a
* synchronous data-in transfer - leave them alone.)
*
* If there has been a change in the requested phase, or the byte count has
* reached zero, the transfer has terminated, in which case there's no more
* to do here.
*
* If we are in a message-in transfer with an ACK pending, negate ACK and
* return.  The ASC will generate a bus service interrupt when the target
* is ready to transfer the next byte.  (Note that this is done _after_ the
* test for completion of a transfer; this means that at the end of a
* message-in transfer, ACK is always left asserted so that the higher-level
* code can parse the message before negating ACK).
*
* Otherwise, continue the transfer.  If it is a write operation, fill the
* FIFO with the next chunk of data.  For either a write or a read, issue a
* Transfer Info command.
*
* RETURNS: TRUE if transfer has completed, else FALSE.
*/

LOCAL BOOL ascProgXferContinue
    (
    ASC * pAsc,				/* ptr to NCR 5390 info       */
    BOOL  msgInAckPending		/* TRUE => need to negate ACK */
    )
    {
    /* Get a byte from the FIFO if reading */

    if (pAsc->xferDirection == READ)
	{
	if ((pAsc->xferNBytes                      != 0) &&
	    ((*pAsc->pFlgsReg & NCR5390_MORE_DATA) != 0))
	    {
	    *pAsc->xferAddr = *pAsc->pFifoReg;

	    ++pAsc->xferAddr;
	    --pAsc->xferNBytes;
	    }
    	}

    /* Check for termination of transfer */

    if ((pAsc->xferNBytes == 0) || (pAsc->reqPhase != pAsc->xferPhase))
	return (TRUE);

    /* Negate ACK if it has been left pending (only for MSG IN transfers) */

    if (msgInAckPending)
	{
	ascCommand (pAsc, NCR5390_MSG_ACCEPTED);

	return (FALSE);
	}

    /* Fill the FIFO if writing */

    if (pAsc->xferDirection == WRITE)
	{
	FAST UINT    nBytes = min (pAsc->xferNBytes, NCR5390_FIFO_DEPTH);
	FAST UINT8 * pBuf   =      pAsc->xferAddr;

	pAsc->xferAddr   += nBytes;
	pAsc->xferNBytes -= nBytes;
	
	while (nBytes-- > 0)
	    *pAsc->pFifoReg = *pBuf++;
	}

    /* Issue another Transfer Info command */

    ascCommand (pAsc, NCR5390_INFO_TRANSFER);

    return (FALSE);
    }


/*******************************************************************************
*
* ascBytesLeftGet - get remaining byte count after info transfer
*
* Calculate the number of bytes that were _not_ transferred across the SCSI
* bus during an info transfer started by either ascProgXfer() or ascDmaXfer().
*
* For DMA, the remaining byte count is the contents of the transfer counter,
* whereas for programmed i/o it is the "xferNBytes" variable in the ASC data.
*
* If the operation is a READ, all the valid bytes for this transfer will
* have been read out of the FIFO, either by the ISR (programmed i/o) or by
* the DMA.  Thus there is no need to adjust the remaining byte count.
*
* If the operation is a WRITE, some of the bytes written to the ASC (either
* by the ISR for programmed i/o or by the DMA) may still be in the FIFO.
* Read the FIFO flags to find out how many, and add this number to the
* remaining byte count since these bytes have not gone across the SCSI bus.
* Normally, flush the FIFO to get rid of these bytes.  However, if the next
* transfer is a data-in phase and the ASC is in synchronous mode, do _not_
* flush the FIFO as it may already contain some data-in phase bytes.  (The
* ASC hardware handles this as a special case and locks the FIFO flags to
* indicate the number of outgoing bytes discarded, before starting to buffer
* incoming data.)
*
* RETURNS: remaining byte count.
*/

LOCAL UINT ascBytesLeftGet
    (
    ASC * pAsc				/* ptr to NCR 5390 info */
    )
    {
    UINT bytesInFIFO = (*pAsc->pFlgsReg & NCR5390_MORE_DATA);
    UINT bytesLeft;

    if (pAsc->xferUsingDma)
	ascXferCountGet (pAsc, &bytesLeft);
    else
	bytesLeft = pAsc->xferNBytes;

    if ((bytesInFIFO != 0) && (pAsc->xferDirection == WRITE))
	{
	bytesLeft += bytesInFIFO;

	if (pAsc->syncDataXfer && (pAsc->reqPhase == SCSI_DATA_IN_PHASE))
	    {
	    /* special case: do not flush incoming data from FIFO! */
	    }
	else
	    {
	    ascCommand (pAsc, NCR5390_FIFO_FLUSH);
	    }
	}

    return (bytesLeft);
    }


/*******************************************************************************
*
* ascHwInit - initialize the ASC chip to a known state
*
* This routine puts the ASC into a known quiescent state.
*
* Currently the initial state is not configurable and does not enable
* reselection.
*
* NOTE:
* The chip type must have already been determined (see ncr5390CtrlCreate()).
*
* INTERNAL
* Needs to handle parity enable
*/

LOCAL void ascHwInit
    (
    ASC *pAsc          /* ptr to an ASC info structure */
    )
    {
    UINT8 cfg1RegVal;
    UINT  clkPeriod = pAsc->clkPeriod;
    int   clkConvFactor;

    /* issue NO-OP (required after hardware reset) */

    ascCommand (pAsc, NCR5390_CHIP_RESET);
    taskDelay (1);
    ascCommand (pAsc, NCR5390_DMA_OP | NCR5390_NOP);

    /* enable parity checking */

    pAsc->parityCheckEnbl = TRUE;

    /* Set value of configuration-1 register */

    cfg1RegVal = ((UINT8) pAsc->scsiCtrl.scsiCtrlBusId) |
		  (pAsc->parityCheckEnbl ? NCR5390_PAR_CHECK_ENBL : 0) |
		  (pAsc->parityTestMode  ? NCR5390_PAR_TEST_ENBL  : 0) |
		  (pAsc->resetReportDsbl ? NCR5390_RESET_REP_DSBL : 0) |
		  (pAsc->slowCableMode   ? NCR5390_SLOW_CABLE     : 0);

    *pAsc->pCfg1Reg = cfg1RegVal;

    /* Set value of configuration-2 register, if present */

    if (pAsc->chipType != ASC_NCR5390)
	*pAsc->pCfg2Reg = (NCR5390_FEATURE_ENABLE | NCR5390_SCSI_2);
    
    /* Set value of configuration-3 register, if present */
    
    if (pAsc->chipType == ASC_NCR53C9X)
	{
	UINT8 cfg3RegVal = 0;
	
    	if (clkPeriod < 40)		/* clock freq > 25 MHz */
	    cfg3RegVal |= NCR5390_FAST_CLOCK;

	if (clkPeriod <= 25)	    	/* clock freq >= 40 MHz */
	    cfg3RegVal |= NCR5390_FAST_SCSI;

	*pAsc->pCfg3Reg = cfg3RegVal;
	}
	
    /* set clock conversion factor */

    if (clkPeriod >= 100)		/* clock freq <= 10 Mhz */
        clkConvFactor = 2;
    else if (clkPeriod >= 67)		/* clock freq <= 15 Mhz */
        clkConvFactor = 3;
    else if (clkPeriod >= 50)		/* clock freq <= 20 Mhz */
        clkConvFactor = 4;
    else if (clkPeriod >= 40)           /* clock freq <= 25 Mhz */
        clkConvFactor = 5;
    else if (clkPeriod >= 33)	    	/* clock freq <= 30 MHz */
	clkConvFactor = 6;
    else if (clkPeriod >= 28)	    	/* clock freq <= 35 MHz */
	clkConvFactor = 7;
    else    	    	    	    	/* clock freq >  35 MHz */
	clkConvFactor = 0;

    *pAsc->pClkReg = pAsc->clkCvtFactor = (UINT8) clkConvFactor;
    }

/*******************************************************************************
*
* ascChipTypeGet - determine type of ASC chip present
*
* NOTE:
* This code is not very general and has not been fully tested.  It has side
* effects on the ASC chip so it should only be called during initialisation.
*
* RETURNS: ASC chip type as defined in ncr5390.h
*/

LOCAL int ascChipTypeGet
    (
    ASC * pAsc
    )
    {
    /* Set feature enable bit in cfg 2 reg: if it sticks, assume a 53C9X */

    *pAsc->pCfg2Reg = NCR5390_FEATURE_ENABLE;

    return ((*pAsc->pCfg2Reg & NCR5390_FEATURE_ENABLE) != 0) ? ASC_NCR53C9X
		                                             : ASC_NCR5390;
    }

/*******************************************************************************
*
* ascMaxBytesPerXferGet - get limit of ASC's transfer counter
*
* RETURNS: maximum length info transfer (using DMA)
*/

LOCAL UINT ascMaxBytesPerXferGet
    (
    int chipType
    )
    {
    switch (chipType)
	{
    	case ASC_NCR53C9X: return (0x00ffffff);	    /* 24-bit counter */
	default:           return (0x0000ffff);	    /* 16-bit counter */
	}
    }

/*******************************************************************************
*
* ncr5390ShowScsi2 - display the values of all readable NCR 53C90 ASC registers
*
* This routine displays the state of the ASC registers in a user-friendly 
* manner.  It is useful primarily for debugging.
*
* EXAMPLE:
* .CS
*     -> ncr5390Show
*     pTclReg  = 0x00
*     pTchReg  = 0x02
*     pFifoReg = 0x00 
*     pCmdReg  = 0x12
*     pStatReg = 0x00
*     pIntrReg = 0x00 
*     pStepReg = 0xc4
*     pFlgsReg = 0x00 
*     pCfg1Reg = 0x07
* .CE
*
* RETURNS: OK, or ERROR if <pScsiCtrl> and <pSysScsiCtrl> are both NULL.
*/

LOCAL STATUS ncr5390ShowScsi2
    (
    FAST NCR_5390_SCSI_CTRL *pScsiCtrl      /* ptr to ASC info */
    )
    {
    ASC *pAsc;

    if (pScsiCtrl == NULL)
	{
	if (pSysScsiCtrl == NULL)
	    {
	    printErr ("No SCSI controller specified.\n");
	    return (ERROR);
	    }
	pScsiCtrl = (ASC *) pSysScsiCtrl;
	}

    pAsc = pScsiCtrl;

    printf ("pTclReg  = 0x%02x\n", *pAsc->pTclReg);
    printf ("pTchReg  = 0x%02x\n", *pAsc->pTchReg);
    printf ("pFifoReg = 0x%02x\n", *pAsc->pFifoReg);
    printf ("pCmdReg  = 0x%02x\n", *pAsc->pCmdReg);
    printf ("pStatReg = 0x%02x\n", *pAsc->pStatReg);
    printf ("pIntrReg = 0x%02x\n", *pAsc->pIntrReg);
    printf ("pStepReg = 0x%02x\n", *pAsc->pStepReg);
    printf ("pFlgsReg = 0x%02x\n", *pAsc->pFlgsReg);
    printf ("pCfg1Reg = 0x%02x\n", *pAsc->pCfg1Reg);

    return (OK);
    }
