/* ncr5390Lib1.c - NCR 53C90 Advanced SCSI Controller (ASC) library (SCSI-1) */

/* Copyright 1989-1996 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01j,03may96,dds  fixed for SCSI1 & SCSI2 comapatibilty.
01i,06feb96,dat  double interrupt protection, clr semaphore with each
       		 ascCommand. Chg'd ascIntr debug messages.
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
system.  The only exception in this portion of the driver is the 
ncr5390CtrlCreate() which creates a controller structure.

INCLUDE FILES
ncr5390.h

SEE ALSO: scsiLib,
.I "NCR 53C90A, 53C90B Advanced SCSI Controller,"
.pG "I/O System"
*/

#include "vxWorks.h"
#define NCR5390_1_LOCAL_FUNCS
#include "drv/scsi/ncr5390.h"
#undef NCR5390_1_LOCAL_FUNCS
#include "memLib.h"
#include "errnoLib.h"
#include "logLib.h"
#include "scsiLib.h"
#include "stdlib.h"
#include "stdio.h"
#include "sysLib.h"
#include "taskLib.h"

/* defines */

#define ASC_MAX_XFER_LENGTH	((UINT) (0xffff)) /* max data xfer length */

/* globals */

IMPORT BOOL scsiDebug;
IMPORT BOOL scsiIntsDebug;

/* forward declarations */

LOCAL void ascCommand (ASC *, FAST int);
LOCAL void ascIntr (ASC *);
LOCAL void ascXferCountGet (FAST ASC *, FAST int *);
LOCAL STATUS ascXferCountSet (FAST ASC *, FAST int);
LOCAL STATUS ncr5390CtrlInit (FAST ASC *, FAST int , FAST UINT, int);
LOCAL STATUS ncr5390Show ( FAST SCSI_CTRL *);

LOCAL STATUS ascDevSelect (SCSI_PHYS_DEV *, SCSI_TRANSACTION *);
LOCAL STATUS ascMsgInAck (SCSI_CTRL *, BOOL);
LOCAL STATUS ascBytesOut (SCSI_PHYS_DEV *, char *, int, int);
LOCAL STATUS ascBytesIn (SCSI_PHYS_DEV *, char *, int, int);
LOCAL STATUS ascBusPhaseGet (SCSI_CTRL *, int, int *);
LOCAL void ascSelTimeOutCvt (SCSI_CTRL *, UINT , UINT *);
LOCAL STATUS ascBusIdGet (ASC *, int *);
LOCAL void ascScsiBusReset (ASC *);
LOCAL void ascHwInit (ASC *);




/*******************************************************************************
*
* ncr5390Scsi1IfInit - initialize the SCSI-2 interface to ncr5390
*
* NOMANUAL
*/
void ncr5390Scsi1IfInit ()
    {
    /* create table */

    ncr5390IfTblInit ();

    /* initialize table */

    pNcr5390IfTbl->ascCommand       = (FUNCPTR) ascCommand;
    pNcr5390IfTbl->ascIntr          = (FUNCPTR) ascIntr;
    pNcr5390IfTbl->ascXferCountGet  = (FUNCPTR) ascXferCountGet;
    pNcr5390IfTbl->ascXferCountSet  = (FUNCPTR) ascXferCountSet;
    pNcr5390IfTbl->ncr5390CtrlInit  = (FUNCPTR) ncr5390CtrlInit;
    pNcr5390IfTbl->ncr5390Show      = (FUNCPTR) ncr5390Show;
    }

/*******************************************************************************
*
* ncr5390CtrlCreate - create a control structure for an NCR 53C90 ASC
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
* (This must be a power of 2, for example, 1, 2, 4, etc.)
* .iP <clkPeriod>
* the period, in nanoseconds, of the signal to the ASC clock input (used only
* for select command timeouts).
* .iP "<ascDmaBytesIn> and <ascDmaBytesOut>"
* board-specific parameters to handle DMA input and output.
* If these are NULL (0), ASC program transfer mode is used.
* DMA is possible only during SCSI data in/out phases.
* The interface to these DMA routines must be of the form:
* .CS
*     STATUS xxDmaBytes{In, Out}
*         (
*         SCSI_PHYS_DEV  *pScsiPhysDev,  /@ ptr to phys dev info    @/
*         UINT8          *pBuffer,       /@ ptr to the data buffer  @/
*         int            bufLength       /@ number of bytes to xfer @/
*         )
* .CE
* .LP
*
* RETURNS: A pointer to an NCR_5390_SCSI_CTRL structure,
* or NULL if memory is insufficient or the parameters are invalid.
*/

NCR_5390_SCSI_CTRL *ncr5390CtrlCreate
    (
    FAST UINT8 *baseAdrs,       /* base address of ASC */
    int regOffset,              /* addr offset between consecutive regs. */
    UINT clkPeriod,             /* period of controller clock (nsec) */
    FUNCPTR ascDmaBytesIn,      /* SCSI DMA input function */
    FUNCPTR ascDmaBytesOut      /* SCSI DMA output function */
    )
    {
    FAST ASC *pAsc;		/* ptr to ASC info */

    /* Initialize the driver function table */

    ncr5390Scsi1IfInit();
    
    /* verify parameters */

    if (regOffset == 0)
	return ((ASC *) NULL);

    /* calloc the controller info structure; return ERROR if unable */

    pAsc = (ASC *) calloc (1, sizeof (ASC));

    if (pAsc == (ASC *) NULL)
        return ((ASC *) NULL);

    /* fill in generic SCSI info for this controller */

    scsiCtrlInit (&pAsc->scsiCtrl);

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

    pAsc->scsiCtrl.clkPeriod = clkPeriod;
    pAsc->scsiCtrl.maxBytesPerXfer = ASC_MAX_XFER_LENGTH;

    pAsc->scsiCtrl.scsiBusReset      = (VOIDFUNCPTR) ascScsiBusReset;

    pAsc->scsiCtrl.scsiTransact      = (FUNCPTR) scsiTransact;
    pAsc->scsiCtrl.scsiDevSelect     = (FUNCPTR) ascDevSelect;
    pAsc->scsiCtrl.scsiBytesIn 	     = (FUNCPTR) ascBytesIn;
    pAsc->scsiCtrl.scsiBytesOut	     = (FUNCPTR) ascBytesOut;
    pAsc->scsiCtrl.scsiDmaBytesIn    = (FUNCPTR) ascDmaBytesIn;
    pAsc->scsiCtrl.scsiDmaBytesOut   = (FUNCPTR) ascDmaBytesOut;
    pAsc->scsiCtrl.scsiBusPhaseGet   = (FUNCPTR) ascBusPhaseGet;
    pAsc->scsiCtrl.scsiMsgInAck      = (FUNCPTR) ascMsgInAck;
    pAsc->scsiCtrl.scsiSelTimeOutCvt = (VOIDFUNCPTR) ascSelTimeOutCvt;

    pAsc->pDevToSelect = (SCSI_PHYS_DEV *) NULL;

    return (pAsc);
    }

/*******************************************************************************
*
* ncr5390CtrlInit - initialize a control structure for an NCR 53C90 ASC
*
* This routine initializes an ASC structure created by ncr5390CtrlCreate().
* It must be called before the ASC is used.  This routine can be called more
* than once; however, it should be called only while there is no activity
* on the SCSI interface, since the specified configuration 
* is written to the ASC.
*
* Before returning, this routine pulses RST (reset) on the SCSI bus, thus
* resetting all attached devices.
*
* The input parameters are as follows:
* .iP <pAsc> 4
* a pointer to an NCR_5390_SCSI_CTRL structure created with
* ncr5390CtrlCreate().
* .iP <scsiCtrlBusId>
* the SCSI bus ID of the ASC, in the range 0 - 7.  The ID is somewhat arbitrary;
* the value 7, or highest priority, is conventional.
* .iP <defaultSelTimeOut>
* the timeout, in microseconds, for selecting a SCSI device attached to this
* controller.  This value is used as a default if no timeout is specified in 
* scsiPhysDevCreate()).  The recommended value zero (0)
* specifies SCSI_DEF_SELECT_TIMEOUT (250 millisec).  The maximum timeout
* possible is approximately 3 seconds.  Values exceeding this revert to the
* maximum.  For more information about chip timeouts, see the manual
* .I "NCR 53C90A, 53C90B Advanced SCSI Controller."
* .iP <scsiPriority>
* the priority to which a task is set when performing a SCSI transaction.
* Valid priorities are 0 to 255.  Alternatively, the value -1 specifies 
* that the priority should not be altered during SCSI transactions.
*
* RETURNS: OK, or ERROR if a parameter is out of range.
*
* SEE ALSO: scsiPhysDevCreate(),
* .I "NCR 53C90A, 53C90B Advanced SCSI Controller"
*/

LOCAL STATUS ncr5390CtrlInit
    (
    FAST NCR_5390_SCSI_CTRL *pAsc, /* ptr to ASC struct */
    FAST int scsiCtrlBusId,        /* SCSI bus ID of this ASC */
    FAST UINT defaultSelTimeOut,   /* default dev sel timeout (microsec) */
    int scsiPriority               /* priority of task doing SCSI I/O */
    )
    {
    UINT tempSelTimeOut;	/* temp. value of select time-out (no units) */
    FAST UINT tempClkPeriod;	/* local copy of clock period */
    int  tempClkConvFactor;	/* local copy of clock conversion factor */

    /* verify scsiCtrlBusId and enter legal value in ASC structure */

    if (scsiCtrlBusId < SCSI_MIN_BUS_ID || scsiCtrlBusId > SCSI_MAX_BUS_ID)
	return (ERROR);

    pAsc->scsiCtrl.scsiCtrlBusId = (UINT8) scsiCtrlBusId;

    /* verify scsiPriority and enter legal value in ASC structure */

    if (scsiPriority < NONE || scsiPriority > 0xff)
	return (ERROR);

    pAsc->scsiCtrl.scsiPriority = scsiPriority;

    /* issue NO-OP (required after hardware reset) */

    ascCommand (pAsc, NCR5390_CHIP_RESET);
    taskDelay (2);
    ascCommand (pAsc, NCR5390_DMA_OP | NCR5390_NOP);

    /* set clock conversion factor */

    tempClkPeriod = pAsc->scsiCtrl.clkPeriod;

    if (tempClkPeriod >= 100)		/* clock freq <= 10 Mhz */
	tempClkConvFactor = 2;
    else if (tempClkPeriod >= 67)	/* clock freq <= 15 Mhz */
	tempClkConvFactor = 3;
    else if (tempClkPeriod >= 50)	/* clock freq <= 20 Mhz */
	tempClkConvFactor = 4;
    else 				/* clock freq > 20 Mhz */
	tempClkConvFactor = 5;

    *pAsc->pClkReg = pAsc->clkCvtFactor = (UINT8) tempClkConvFactor;

    /* verify defaultSelTimeOut, convert it from usec to ASC register values,
     * and enter value in ASC structure
     */

    ascSelTimeOutCvt (&pAsc->scsiCtrl, defaultSelTimeOut, &tempSelTimeOut);
    pAsc->defaultSelTimeOut = (UINT8) tempSelTimeOut;

    /* disconnect not supported for now */

    pAsc->scsiCtrl.disconnect = (TBOOL) FALSE;

    ascHwInit (pAsc); 	/* initialize the ASC hardware */

    return (OK);
    }

/*******************************************************************************
*
* ascDevSelect - attempt to select a SCSI physical device
*
* This routine is intended to be called from scsiLib, not directly.
*
* RETURNS: OK if device was successfully selected, otherwise ERROR.
*/

LOCAL STATUS ascDevSelect
    (
    FAST SCSI_PHYS_DEV *pScsiPhysDev,   /* ptr to SCSI physical device info */
    SCSI_TRANSACTION *pScsiXaction      /* ptr to SCSI transaction info     */
    )
    {
    FAST ASC *pAsc;		/* ptr to ASC info */
    int ascBusId;               /* SCSI bus ID of the ASC */
    UINT8 identMsg;		/* for construction of the IDENTIFY message */
    int ix;			/* loop index */
    UINT8 *pCmdByte;		/* ptr to a command byte */
    UINT8 ascOpcode;		/* ASC opcode byte */

    pAsc = (ASC *) pScsiPhysDev->pScsiCtrl;

    if ((ascBusIdGet (pAsc, &ascBusId) == ERROR) ||
        (ascBusId == pScsiPhysDev->scsiDevBusId))
	{
        return (ERROR);
	}

    pAsc->pDevToSelect  = pScsiPhysDev;
    pScsiPhysDev->devStatus = SELECT_REQUESTED;

    *pAsc->pBidReg = (UINT8) pScsiPhysDev->scsiDevBusId;
    *pAsc->pTmoReg = (UINT8) pScsiPhysDev->selTimeOut;

    if (pScsiPhysDev->useIdentify)		/* send an identify message */
	{
	identMsg = SCSI_MSG_IDENTIFY |
	(pAsc->scsiCtrl.disconnect ?
	SCSI_MSG_IDENT_DISCONNECT : 0) |
	(UINT8) pScsiPhysDev->scsiDevLUN;
	*pAsc->pFifoReg = identMsg;		/* load ident msg into FIFO */
	}

    pCmdByte = pScsiXaction->cmdAddress;

    for (ix = 0; ix < pScsiXaction->cmdLength; ix++)
	*pAsc->pFifoReg = *pCmdByte++;

    if (pScsiPhysDev->useIdentify)
	{
	if (pScsiPhysDev->msgLength == 0)
	    ascOpcode = (UINT8) NCR5390_ATN_SELECT;
	else
	    ascOpcode = (UINT8) NCR5390_STOP_SELECT;
	}
    else
	ascOpcode = (UINT8) NCR5390_SELECT;

    ascCommand (pAsc, ascOpcode);

    if (semTake (&pAsc->scsiCtrl.ctrlSyncSem,
		 ((pScsiXaction->cmdTimeout / SCSI_TIMEOUT_1SEC) + 1) *
		 sysClkRateGet())
		 == ERROR)
	{
	/* timeout on semaphore, didn't get an interrupt */
	printErr ("ascDevSelect: No interrupt received.\n");

errorRecovery:
	/* flush fifo, reset bus, delay to allow devices to reset */
	ascCommand (pAsc, NCR5390_FIFO_FLUSH);
	ascScsiBusReset (pAsc);
	taskDelay (3 * sysClkRateGet ()); /* 3 seconds */

	return (ERROR);
	}

    if (!(pAsc->savedIntrReg & NCR5390_DISCONNECTED))
	{
	if (!(pAsc->savedIntrReg == (NCR5390_BUS_SERVICE |
				     NCR5390_FUNC_COMPLETE)))
	    {
	    printErr ("ascDevSelect: Unknown chip state.\n");
	    printErr
		("statReg = 0x%02x, stepReg = 0x%02x, intrReg = 0x%02x\n",
	 	 pAsc->savedStatReg, pAsc->savedStepReg, pAsc->savedIntrReg);
	    goto errorRecovery;
	    }
	else
	    {
	    switch ((int) ascOpcode)
		{
		case NCR5390_SELECT:
		    if (pAsc->savedStepReg == 4)
			return (OK);

		case NCR5390_ATN_SELECT:
		    if (pAsc->savedStepReg == 4)
			return (OK);

		case NCR5390_STOP_SELECT:
		    if (pAsc->savedStepReg == 1)
			return (OK);
		}
	    ascCommand (pAsc, NCR5390_FIFO_FLUSH);
	    return (OK);
	    }
	}
    else
	{
	/* simple device select timeout */
	ascCommand (pAsc, NCR5390_FIFO_FLUSH);
	errnoSet (S_scsiLib_SELECT_TIMEOUT);
        return (ERROR);
	}
    }

#if	FALSE	/* for sync/disconnect in future */
/*******************************************************************************
*
* ascAtnSet - assert the ATN line to request a MESSAGE OUT phase on SCSI
*/

LOCAL void ascAtnSet
    (
    FAST SCSI_CTRL *pScsiCtrl   /* ptr to ASC struct */
    )
    {
    ascCommand ((ASC *) pScsiCtrl, NCR5390_SET_ATTENTION);
    }
#endif	/* FALSE */

/*******************************************************************************
*
* ascMsgInAck - de-assert the ACK line to accept message
*
* This routine should be called when an incoming message has been read and
* accepted.  If the incoming message did not imply an impending disconnect,
* give the synchronization semaphore, so that subsequent phase can be
* detected.
*
* RETURNS: OK.
*/

LOCAL STATUS ascMsgInAck
    (
    FAST SCSI_CTRL *pScsiCtrl,  /* ptr to ASC info                  */
    BOOL expectDisconn          /* whether a disconnect is expected */
    )
    {
    ascCommand ((ASC *) pScsiCtrl, NCR5390_MSG_ACCEPTED);

    semTake (&pScsiCtrl->ctrlSyncSem, WAIT_FOREVER);

    return (OK);
    }

/*******************************************************************************
*
* ascProgBytesOut - output bytes to SCSI using 'program' transfer
*
* RETURNS: OK if successful, otherwise ERROR.
*
* NOMANUAL
*/

STATUS ascProgBytesOut
    (
    SCSI_PHYS_DEV *pScsiPhysDev, /* ptr to physical device info       */
    FAST UINT8 *pBuffer,         /* ptr to the data buffer            */
    FAST int bufLength,          /* number of bytes to be transferred */
    int scsiPhase                /* phase of the transfer             */
    )
    {
    FAST ASC *pAsc;		 /* ptr to ASC info */
    FAST int ix;		 /* loop index */

    pAsc = (ASC *) pScsiPhysDev->pScsiCtrl;

    if (*pAsc->pFlgsReg & NCR5390_MORE_DATA)
        SCSI_DEBUG_MSG ("ascProgBytesOut: FIFO Not Empty.\n",
			0, 0, 0, 0, 0, 0);

    if (bufLength <= 0)
	return OK;

    ascCommand (pAsc, NCR5390_DMA_OP | NCR5390_NOP);

    while (bufLength >= NCR5390_FIFO_DEPTH)
	{
	for (ix = 0; ix < NCR5390_FIFO_DEPTH; ix++)
            *pAsc->pFifoReg = *pBuffer++;

	ascCommand (pAsc, NCR5390_INFO_TRANSFER);

	if (semTake (&pAsc->scsiCtrl.ctrlSyncSem, sysClkRateGet ()) == ERROR)
	    {
	    errnoSet (S_scsiLib_DATA_TRANSFER_TIMEOUT);
	    return (ERROR);
	    }

	bufLength -= NCR5390_FIFO_DEPTH;
        }

    if (bufLength > 0)
	{
	while (bufLength > 0)
            {
            *pAsc->pFifoReg = *pBuffer++;
	    bufLength--;
            }

	ascCommand (pAsc, NCR5390_INFO_TRANSFER);

	if (semTake (&pAsc->scsiCtrl.ctrlSyncSem, sysClkRateGet ()) == ERROR)
	    {
	    errnoSet (S_scsiLib_DATA_TRANSFER_TIMEOUT);
	    return (ERROR);
	    }

	}

    return (OK);
    }

/*******************************************************************************
*
* ascProgBytesIn - input bytes from SCSI using 'program' transfer
*
* RETURNS: OK if successful, otherwise ERROR.
*
* NOMANUAL
*/

STATUS ascProgBytesIn
    (
    SCSI_PHYS_DEV *pScsiPhysDev,        /* ptr to physical device info       */
    FAST UINT8 *pBuffer,                /* ptr to the data buffer            */
    FAST int bufLength,                 /* number of bytes to be transferred */
    int scsiPhase                       /* phase of the transfer             */
    )
    {
    FAST ASC *pAsc;		/* ptr to ASC info */

    SCSI_DEBUG_MSG ("ascProgBytesIn: scsiPhase = %s, bufLength = %d\n",
		    (int) scsiPhaseNameGet (scsiPhase), bufLength, 0, 0, 0, 0);

    pAsc = (ASC *) pScsiPhysDev->pScsiCtrl;

    if (*pAsc->pFlgsReg & NCR5390_MORE_DATA)
        SCSI_DEBUG_MSG ("ascProgBytesIn: FIFO Not Empty.\n",
			0, 0, 0, 0, 0, 0);

    ascCommand (pAsc, NCR5390_DMA_OP | NCR5390_NOP);

    while (bufLength > 0)
        {
	ascCommand (pAsc, NCR5390_INFO_TRANSFER);

	if (semTake (&pAsc->scsiCtrl.ctrlSyncSem, sysClkRateGet ()) == ERROR)
	    {
	    errnoSet (S_scsiLib_DATA_TRANSFER_TIMEOUT);
	    return (ERROR);
	    }

        *pBuffer++ = *pAsc->pFifoReg;
	bufLength--;
        }

    if (scsiPhase == SCSI_MSG_IN_PHASE)
        {
        ascMsgInAck ((SCSI_CTRL *) pScsiPhysDev->pScsiCtrl, TRUE);
        }

    return (OK);
    }

/*******************************************************************************
*
* ascBytesOut - branch to the appropriate output routine dependent on SCSI phase
*
* RETURNS: OK if specified bytes were output successfully, otherwise ERROR.
*/

LOCAL STATUS ascBytesOut
    (
    FAST SCSI_PHYS_DEV *pScsiPhysDev,   /* ptr to SCSI physical dev info     */
    FAST char *pBuffer,                 /* ptr to byte buffer for output     */
    int bufLength,                      /* number of bytes to be transferred */
    int scsiPhase                       /* SCSI bus phase of the transfer    */
    )
    {
    FAST ASC *pAsc;			/* ptr to ASC info */
    STATUS status;			/* local status for iterative xfers */
    int xferLength;			/* local length for iterative xfers */

    pAsc = (ASC *) pScsiPhysDev->pScsiCtrl;

    do
	{
	xferLength = min (bufLength, ASC_MAX_XFER_LENGTH);

        if ((scsiPhase != SCSI_DATA_OUT_PHASE) ||
	    (pAsc->scsiCtrl.scsiDmaBytesOut == NULL))
	    {
	    status = ascProgBytesOut (pScsiPhysDev, (UINT8 *) pBuffer,
				      xferLength, scsiPhase);
	    }
        else
	    {
	    status = (pAsc->scsiCtrl.scsiDmaBytesOut)
		     (pScsiPhysDev, (UINT8 *) pBuffer, xferLength);
	    }

	/* if ERROR was returned, exit now */

	if (status != OK)
	    break;
	else
    	    pBuffer += ASC_MAX_XFER_LENGTH;
	} while ((bufLength -= ASC_MAX_XFER_LENGTH) > 0);

    return (status);
    }

/*******************************************************************************
*
* ascBytesIn - branch to the appropriate input routine dependent on SCSI phase
*
* RETURNS: OK if requested bytes were input successfully, otherwise ERROR.
*/

LOCAL STATUS ascBytesIn
    (
    FAST SCSI_PHYS_DEV *pScsiPhysDev,   /* ptr to SCSI physical dev info     */
    FAST char *pBuffer,                 /* ptr to byte buffer for output     */
    int bufLength,                      /* number of bytes to be transferred */
    int scsiPhase                       /* SCSI bus phase of the transfer    */
    )
    {
    FAST ASC *pAsc;			/* ptr to ASC info */
    STATUS status;                      /* local status for iterative xfers */
    int xferLength;                     /* local length for iterative xfers */

    pAsc = (ASC *) pScsiPhysDev->pScsiCtrl;

    do
        {
        xferLength = min (bufLength, ASC_MAX_XFER_LENGTH);

        if ((scsiPhase != SCSI_DATA_IN_PHASE) ||
            (pAsc->scsiCtrl.scsiDmaBytesIn == NULL))
            {
            status = ascProgBytesIn (pScsiPhysDev, (UINT8 *) pBuffer,
                                     xferLength, scsiPhase);
            }
        else
            {
            status = (pAsc->scsiCtrl.scsiDmaBytesIn)
                     (pScsiPhysDev, (UINT8 *) pBuffer, xferLength);
            }

        /* if ERROR was returned, exit now */

        if (status != OK)
            break;
	else
    	    pBuffer += ASC_MAX_XFER_LENGTH;
        } while ((bufLength -= ASC_MAX_XFER_LENGTH) > 0);

    return (status);
    }

/*******************************************************************************
*
* ascBusPhaseGet - return the current SCSI bus phase in *pBusPhase
*/

LOCAL STATUS ascBusPhaseGet
    (
    FAST SCSI_CTRL *pScsiCtrl,  /* ptr to SCSI controller info     */
    int timeOutInUsec,          /* timeout in usec (0 == infinity) */
    int *pBusPhase              /* ptr to returned bus phase       */
    )
    {
    FAST ASC *pAsc;		/* ptr to ASC info */

    pAsc = (ASC *) pScsiCtrl;

    if (pAsc->savedIntrReg & NCR5390_BUS_SERVICE)
        {
        *pBusPhase = pAsc->savedStatReg & NCR5390_PHASE_MASK;
        return (OK);
        }

    if (pAsc->savedIntrReg & NCR5390_DISCONNECTED)
        {
        *pBusPhase = SCSI_BUS_FREE_PHASE;
        return (OK);
        }

    /* otherwise return current bus state */
    *pBusPhase = *pAsc->pStatReg & NCR5390_PHASE_MASK;

    return (OK);
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
    FAST SCSI_CTRL *pScsiCtrl,          /* ptr to SCSI controller info */
    FAST UINT timeOutInUsec,            /* time-out in microsecs       */
    FAST UINT *pTimeOutSetting          /* time-out equivalent setting */
    )
    {
    FAST ASC *pAsc = (ASC *) pScsiCtrl;	/* ptr to ASC info */
    FAST UINT tempSelTimeOut;		/* temp. select time-out setting */

    if (timeOutInUsec == 0)
	timeOutInUsec = SCSI_DEF_SELECT_TIMEOUT;

    tempSelTimeOut = ((timeOutInUsec * 1000) /
		      (8192 * (pAsc->clkCvtFactor) * pScsiCtrl->clkPeriod)) + 1;

    if (tempSelTimeOut > 0xff)
	tempSelTimeOut = 0xff;

    *pTimeOutSetting = tempSelTimeOut;
    }

/*******************************************************************************
*
* ascBusIdGet - get the current SCSI bus ID of the ASC.
*
* Copies the bus ID to <pBusId>.
*
* RETURNS:
* OK if Bus ID register holds a legal value, otherwise ERROR.
*/

LOCAL STATUS ascBusIdGet
    (
    FAST ASC *pAsc,             /* ptr to ASC info        */
    FAST int *pBusId            /* ptr to returned bus ID */
    )
    {
    *pBusId = (int) *pAsc->pCfg1Reg & NCR5390_OWN_ID_MASK;
    return (OK);
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
    FAST ASC *pAsc,     /* ptr to ASC info     */
    FAST int count      /* count value to load */
    )
    {
    if (count < 0 || count > ASC_MAX_XFER_LENGTH)
        return (ERROR);

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
    FAST ASC *pAsc,             /* ptr to ASC info       */
    FAST int *pCount            /* ptr to returned value */
    )
    {
    FAST UINT tempCount = 0;

    tempCount |=  (UINT) *pAsc->pTclReg;
    tempCount |= ((UINT) *pAsc->pTchReg) << 8;
    *pCount = tempCount;
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
    /* clear the sync semaphore, just in case */
    semTake (&pAsc->scsiCtrl.ctrlSyncSem, NO_WAIT);

    *pAsc->pCmdReg = (UINT8) cmdCode;
    }

/*******************************************************************************
*
* ascScsiBusReset - assert the RST line on the SCSI bus
*
* Issue a SCSI Bus Reset command to the NCR 5390.  This should put all devices
* on the SCSI bus in an initial quiescent state.
*/

LOCAL void ascScsiBusReset
    (
    FAST ASC *pAsc      /* ptr to ASC info */
    )
    {
    ascCommand (pAsc, NCR5390_BUS_RESET);

    semTake (&pAsc->scsiCtrl.ctrlSyncSem, WAIT_FOREVER);

    /* so BusPhaseGet will report BUS_FREE after this */
    pAsc->savedIntrReg = NCR5390_DISCONNECTED;

    /* allow time for reset signal to become inactive */
    taskDelay (2);
    }

/*******************************************************************************
*
* ascIntr - interrupt service routine for the ASC
*
* NOMANUAL
*/

LOCAL void ascIntr
    (
    ASC *pAsc          /* ptr to ASC info */
    )
    {
    FAST SCSI_PHYS_DEV *pDevToSelect;
    FAST UINT8 savedStatReg;
    FAST UINT8 savedIntrReg;
    FAST UINT8 savedStepReg;
    char temp;

    temp = *pAsc->pStatReg;

    /* Quick exit on false interrupt */
    if ((temp & NCR5390_INTERRUPT) == 0)
	return;

    savedStatReg = pAsc->savedStatReg = temp;
    savedStepReg = pAsc->savedStepReg = *pAsc->pStepReg & 0x7;
    savedIntrReg = pAsc->savedIntrReg = *pAsc->pIntrReg;

    SCSI_INT_DEBUG_MSG 
	("*** ASC INTERRUPT: stat = 0x%02x, step = 0x%02x, intr = 0x%02x\n",
	 savedStatReg, savedStepReg, savedIntrReg, 0, 0, 0);

    if (savedStatReg & NCR5390_TERMINAL_CNT)
	{
	/* to clear the TC condition */
/*TEST	SCSI_INT_DEBUG_MSG ("ascIntr: Terminal Count detected\n",0,0,0,0,0,0); TEST*/
	ascCommand (pAsc, NCR5390_DMA_OP | NCR5390_NOP);
	}

    if (savedIntrReg & NCR5390_FUNC_COMPLETE)
        {
    	/* if a selection attempt has been made, report success */

	if (((pDevToSelect = pAsc->pDevToSelect) != (SCSI_PHYS_DEV *) NULL) &&
	    (pDevToSelect->devStatus == SELECT_IN_PROGRESS))
	    {
	    pDevToSelect->devStatus = SELECT_SUCCESSFUL;
	    pAsc->pDevToSelect = (SCSI_PHYS_DEV *) NULL;
	    }
        }

    if (savedIntrReg & NCR5390_DISCONNECTED)
        {
    	/* if a device has been selected, then a timeout has occurred */

	if (((pDevToSelect = pAsc->pDevToSelect) != (SCSI_PHYS_DEV *) NULL) &&
	    (pDevToSelect->devStatus == SELECT_IN_PROGRESS))
	    {
	    pDevToSelect->devStatus = SELECT_TIMEOUT;
	    pAsc->pDevToSelect = (SCSI_PHYS_DEV *) NULL;
	    }
	else
	    {
	    pAsc->scsiCtrl.scsiBusPhase = SCSI_BUS_FREE_PHASE;
	    }
        }

    semGive (&pAsc->scsiCtrl.ctrlSyncSem);

    /* Check that interrupt line has had time to go low */
    if ((*pAsc->pStatReg) & NCR5390_INTERRUPT)
	{
	int ascDelayLoops=5;
	static int ascDelayCount;
	int i;

	for (i=0;i < ascDelayLoops; i++)
		ascDelayCount += 1;
	}

    }

/*******************************************************************************
*
* ascHwInit - initialize the ASC chip to a known state
*
* This routine puts the ASC into a known quiescent state and issues a reset
* to the SCSI Bus if any signals are active, thus putting target devices in
* some presumably known state.  Currently the initial state is not configurable
* and does not enable reselection.
*
* INTERNAL
* Needs to handle parity enable
*/

LOCAL void ascHwInit
    (
    ASC *pAsc          /* ptr to an ASC info structure */
    )
    {
    UINT8 tempCfg1Reg = (UINT8) 0;

    /* build local copy of desired Configuration-1 Register */

    tempCfg1Reg = ((UINT8) pAsc->scsiCtrl.scsiCtrlBusId) |
		  (pAsc->parityCheckEnbl ? NCR5390_PAR_CHECK_ENBL : 0) |
		  (pAsc->parityTestMode  ? NCR5390_PAR_TEST_ENBL  : 0) |
		  (pAsc->resetReportDsbl ? NCR5390_RESET_REP_DSBL : 0) |
		  (pAsc->slowCableMode   ? NCR5390_SLOW_CABLE     : 0);

    /* write local copy of desired Configuration-1 Register to chip */

    *pAsc->pCfg1Reg = tempCfg1Reg;

    ascScsiBusReset (pAsc);
    }

/*******************************************************************************
*
* ncr5390Show - display the values of all readable NCR 53C90 ASC registers
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

LOCAL STATUS ncr5390Show
    (
    FAST SCSI_CTRL *pScsiCtrl      /* ptr to ASC info */
    )
    {
#if FALSE
    int busId;
    FAST UINT8 tempReg;
    int xferCount;
#endif /* FALSE */

    ASC *pAsc;

    if (pScsiCtrl == NULL)
	{
	if (pSysScsiCtrl == NULL)
	    {
	    printErr ("No SCSI controller specified.\n");
	    return (ERROR);
	    }
	pScsiCtrl = pSysScsiCtrl;
	}

    pAsc = (ASC *) pScsiCtrl;

    printf ("pTclReg  = 0x%02x\n", *pAsc->pTclReg);
    printf ("pTchReg  = 0x%02x\n", *pAsc->pTchReg);
    printf ("pFifoReg = 0x%02x\n", *pAsc->pFifoReg);
    printf ("pCmdReg  = 0x%02x\n", *pAsc->pCmdReg);
    printf ("pStatReg = 0x%02x\n", *pAsc->pStatReg);
    printf ("pIntrReg = 0x%02x\n", *pAsc->pIntrReg);
    printf ("pStepReg = 0x%02x\n", *pAsc->pStepReg);
    printf ("pFlgsReg = 0x%02x\n", *pAsc->pFlgsReg);
    printf ("pCfg1Reg = 0x%02x\n", *pAsc->pCfg1Reg);
    printf ("pCfg2Reg = 0x%02x\n", *pAsc->pCfg2Reg);

#if FALSE
    /* Display current SCSI Bus ID
    */

    if (ascBusIdGet (pAsc, &busId) == OK)
        printf ("SCSI Bus ID: %-5d\n", busId);
    else
        printf ("SCSI Bus ID: ERROR\n");

    /* Display ASC Control Register
    */
    tempReg = *pAsc->pSctlReg;
    printf ("SCTL (0x%02x): ", (int) tempReg);
    printf ("%s", (tempReg & ASC_SCTL_RESET_AND_DSBL) ? "ascDisabl " : "");
    printf ("%s", (tempReg & ASC_SCTL_CTRL_RESET)     ? "ctrlReset " : "");
    printf ("%s", (tempReg & ASC_SCTL_DIAG_MODE)      ? "diagMode  " : "");
    printf ("%s", (tempReg & ASC_SCTL_ARBIT_ENBL)     ? "arbitEnbl " : "");
    printf ("%s", (tempReg & ASC_SCTL_PARITY_ENBL)    ? "paritEnbl " : "");
    printf ("%s", (tempReg & ASC_SCTL_SELECT_ENBL)    ? "selecEnbl " : "");
    printf ("%s", (tempReg & ASC_SCTL_RESELECT_ENBL)  ? "reselEnbl " : "");
    printf ("%s", (tempReg & ASC_SCTL_INT_ENBL)       ? "intsEnbl  " : "");
    printf ("\n");

    /* Display Command Register
    */
    tempReg = *pAsc->pScmdReg;
    printf ("SCMD (0x%02x): ", (int) tempReg);
    switch (tempReg & ASC_SCMD_CMD_MASK)
        {
        case ASC_SCMD_BUS_RELEASE:   printf ("busRlease "); break;
        case ASC_SCMD_SELECT:        printf ("select    "); break;
        case ASC_SCMD_RESET_ATN:     printf ("resetAtn  "); break;
        case ASC_SCMD_SET_ATN:       printf ("setAtn    "); break;
        case ASC_SCMD_XFER:          printf ("transfer  "); break;
        case ASC_SCMD_XFER_PAUSE:    printf ("xferPause "); break;
        case ASC_SCMD_RESET_ACK_REQ: printf ("resAckReq "); break;
        case ASC_SCMD_SET_ACK_REQ:   printf ("setAckReq "); break;
        }
    printf ("%s", (tempReg & ASC_SCMD_RESET_OUT)       ? "resetOut  " : "");
    printf ("%s", (tempReg & ASC_SCMD_INTERCEPT_XFER)  ? "intcptXfr " : "");
    printf ("%s", (tempReg & ASC_SCMD_PRG_XFER)        ? "progXfer  " : "");
    printf ("%s", (tempReg & ASC_SCMD_TERM_MODE)       ? "termMode  " : "");
    printf ("\n");

    /* Display Transfer Mode Register
    */
    tempReg = *pAsc->pTmodReg;
    printf ("TMOD (0x%02x): ", (int) tempReg);
    printf ("%s", (tempReg & ASC_TMOD_SYNC_XFER) ? "syncMode  " :
                                                   "asyncMode ");
    /* JCC Should display offset and period params also
    */
    printf ("\n");

    /* Display Interrupt Sense Register
    */
    tempReg = *pAsc->pIntsReg;
    printf ("INTS (0x%02x): ", (int) tempReg);
    printf ("%s", (tempReg & ASC_INTS_SELECTED)     ? "selected  " : "");
    printf ("%s", (tempReg & ASC_INTS_RESELECTED)   ? "reselect  " : "");
    printf ("%s", (tempReg & ASC_INTS_DISCONNECT)   ? "disconnct " : "");
    printf ("%s", (tempReg & ASC_INTS_COM_COMPLETE) ? "comComplt " : "");
    printf ("%s", (tempReg & ASC_INTS_SERVICE_REQ)  ? "servReq   " : "");
    printf ("%s", (tempReg & ASC_INTS_TIMEOUT)      ? "timeout   " : "");
    printf ("%s", (tempReg & ASC_INTS_HARD_ERROR)   ? "hardError " : "");
    printf ("%s", (tempReg & ASC_INTS_RESET_COND)   ? "resetCond " : "");
    printf ("\n");

    /* Display Phase Sense Register
    */
    tempReg = *pAsc->pPsnsReg;
    printf ("PSNS (0x%02x): ", (int) tempReg);
    printf ("%s", (tempReg & ASC_PSNS_REQ) ? "REQ1 " : "req0 ");
    printf ("%s", (tempReg & ASC_PSNS_ACK) ? "ACK1 " : "ack0 ");
    printf ("%s", (tempReg & ASC_PSNS_ATN) ? "ATN1 " : "atn0 ");
    printf ("%s", (tempReg & ASC_PSNS_SEL) ? "SEL1 " : "sel0 ");
    printf ("%s", (tempReg & ASC_PSNS_BSY) ? "BSY1 " : "bsy0 ");
    printf ("%s", (tempReg & ASC_PSNS_MSG) ? "MSG1 " : "msg0 ");
    printf ("%s", (tempReg & ASC_PSNS_C_D) ? "C_D1 " : "c_d0 ");
    printf ("%s", (tempReg & ASC_PSNS_I_O) ? "I_O1 " : "i_o0 ");
    printf ("\n");

    /* Display ASC Status Register
    */
    tempReg = *pAsc->pSstsReg;
    printf ("SSTS (0x%02x): ", (int) tempReg);
    switch (tempReg & ASC_SSTS_OPER_STAT_MASK)
        {
        case ASC_SSTS_NO_CONNECT_IDLE:  printf ("noConIdle "); break;
        case ASC_SSTS_SELECT_WAIT:      printf ("waitSelec "); break;
        case ASC_SSTS_TARGET_MANUAL:    printf ("trgManual "); break;
        case ASC_SSTS_RESELECT_EXEC:    printf ("reselExec "); break;
        case ASC_SSTS_TARGET_XFER:      printf ("targXfer  "); break;
        case ASC_SSTS_INITIATOR_MANUAL: printf ("iniManual "); break;
        case ASC_SSTS_INITIATOR_WAIT:   printf ("iniWait   "); break;
        case ASC_SSTS_SELECT_EXEC:      printf ("selecExec "); break;
        case ASC_SSTS_INITIATOR_XFER:   printf ("iniXfer   "); break;
        default:                        printf ("operUndef "); break;
        }
    printf ("%s", (tempReg & ASC_SSTS_SCSI_RESET) ? "scsiReset " : "");
    printf ("%s", (tempReg & ASC_SSTS_TC_0)       ? "xferCnt=0 " : "");
    switch (tempReg & ASC_SSTS_DREG_STAT_MASK)
        {
        case ASC_SSTS_DREG_EMPTY:   printf ("dregEmpty "); break;
        case ASC_SSTS_DREG_PARTIAL: printf ("dregPartl "); break;
        case ASC_SSTS_DREG_FULL:    printf ("dregFull  "); break;
        default:                    printf ("dregUndef "); break;
        }
    printf ("\n");

    /* Display ASC Error Status Register
    */
    tempReg = *pAsc->pSerrReg;
    printf ("SERR (0x%02x): ", (int) tempReg);
    switch (tempReg & ASC_SERR_PAR_ERROR_MASK)
        {
        case ASC_SERR_NO_PAR_ERROR:  printf ("noParErr  "); break;
        case ASC_SERR_PAR_ERROR_OUT: printf ("outParErr "); break;
        case ASC_SERR_PAR_ERROR_IN:  printf ("inpParErr "); break;
        default:                      printf ("pErrUndef "); break;
        }
    printf ("%s", (tempReg & ASC_SERR_TC_PARITY)    ? "tcParErr  " : "");
    printf ("%s", (tempReg & ASC_SERR_PHASE_ERROR)  ? "phaseErr  " : "");
    printf ("%s", (tempReg & ASC_SERR_SHORT_PERIOD) ? "shortPeri " : "");
    printf ("%s", (tempReg & ASC_SERR_OFFSET_ERROR) ? "offsetErr " : "");
    printf ("\n");

    /* Display Phase Control Register
    */
    tempReg = *pAsc->pPctlReg;
    printf ("PCTL (0x%02x): ", (int) tempReg);
    printf ("%s", (tempReg & ASC_PCTL_BF_INT_ENBL) ? "bfIntEnbl " :
                                                     "bfIntDsbl ");
    switch (tempReg & ASC_PCTL_PHASE_MASK)
        {
        case ASC_PCTL_DATA_OUT:  printf ("phDataOut "); break;
        case ASC_PCTL_DATA_IN:   printf ("phDataIn  "); break;
        case ASC_PCTL_COMMAND:   printf ("phCommand "); break;
        case ASC_PCTL_STATUS:    printf ("phStatus  "); break;
        case ASC_PCTL_UNUSED_0:  printf ("phUnused0 "); break;
        case ASC_PCTL_UNUSED_1:  printf ("phUnused1 "); break;
        case ASC_PCTL_MESS_OUT:  printf ("phMessOut "); break;
        case ASC_PCTL_MESS_IN:   printf ("phMessIn  "); break;
        }
    printf ("\n");

    /* Display Modified Byte Counter
    */
    tempReg = *pAsc->pMbcReg;
    printf ("MBC  (0x%02x): ", (int) tempReg);
    printf ("%-10d", (int) tempReg);
    printf ("\n");

    /* Display Transfer Counter
    */
    ascXferCountGet (pAsc, &xferCount);
    printf ("XFER COUNT : ");
    printf ("0x%06x =%10d", xferCount, xferCount);
    printf ("\n");
#endif	/* FALSE */

    return (OK);
    }
