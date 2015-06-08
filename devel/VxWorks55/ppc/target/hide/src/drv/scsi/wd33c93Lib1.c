/* wd33c93Lib1.c - WD33C93 SCSI-Bus Interface Controller library (SCSI-1) */

/* Copyright 1989-1995 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01w,06may96,jds  yet more doc tweaks
01v,01may96,jds  more doc cleanup
01u,03feb95,rhp  documentation cleanup
01t,10oct93,jds  fixed for SCSI1 and SCSI2 compatability
01s,24feb93,jdi  documentation cleanup.
01r,19feb93,ccc  removed bypass of DMA routine if DMA is included.
01q,26sep92,ccc  changed sbicShow() to wd33c93Show() and added example.
01p,24sep92,ccc  fixed timeout in sbicBusPhaseGet().
01o,20jul92,gae  fixed parameters to logMsg().
01n,18jul92,smb  Changed errno.h to errnoLib.h.
01m,12jul92,ccc  added reset of SCSI bus if timed out (bus hung).
01l,04jun92,ccc  added timeout to semTake()s.
01k,26may92,rrr  the tree shuffle
		  -changed includes to have absolute path from h/
01j,20apr92,ccc  fixed warnings.
01i,07oct91,rrr  some forward decls
01h,04oct91,rrr  passed through the ansification filter
                  -changed functions to ansi style
		  -changed includes to have absolute path from h/
		  -changed VOID to void
		  -changed copyright notice
01g,05apr91,jdi	 documentation -- removed header parens and x-ref numbers;
		 doc review by jcc.
01f,01feb91,jaa	 documentation.
01e,02oct90,jcc  UTINY became UINT8; changes in sem{Give, Take}() calls
		 since SEM_ID's became SEMAPHORE's in various structures;
		 malloc() became calloc() in wd33c93CtrlCreate(); miscellaneous.
01d,21sep90,jcc  misc. cleanup for 5.0 release.
01c,10aug90,dnw  added forward declarations for void functions.
01b,18jul90,jcc  made semTake() calls 5.0 compatible; clean-up.
01a,28feb90,jcc  written
*/

/*
DESCRIPTION
This library contains part of the I/O driver for the
Western Digital WD33C93 and WD33C93A SCSI-Bus Interface Controllers
(SBIC).  The driver routines in this library depend on the SCSI-1 
version of the SCSI standard; for driver routines that do not depend
on SCSI-1 or SCSI-2, and for overall SBIC driver documentation, see
wd33c93Lib.

USER-CALLABLE ROUTINES
Most of the routines in this driver are accessible only through the
I/O system.  The only exception in this portion of the driver is
wd33c93CtrlCreate(), which creates a controller structure.

INCLUDE FILES
wd33c93.h, wd33c93_1.h

SEE ALSO: scsiLib, scsi1Lib, wd33c93Lib
*/

#include "vxWorks.h"
#define WD33C93_1_LOCAL_FUNCS
#include "drv/scsi/wd33c93_1.h"
#undef  WD33C93_1_LOCAL_FUNCS
#include "memLib.h"
#include "logLib.h"
#include "scsiLib.h"
#include "stdlib.h"
#include "errnoLib.h"
#include "stdio.h"
#include "sysLib.h"

#define WD_33C93_MAX_BYTES_PER_XFER  ((UINT) 0xffffff)

typedef WD_33C93_SCSI_CTRL SBIC;

/* imported functions */

IMPORT STATUS scsiTransact ();
IMPORT STATUS scsiCtrlInit ();

/* forward static functions */

LOCAL STATUS sbicDevSelect (SCSI_PHYS_DEV *, SCSI_TRANSACTION *);
LOCAL STATUS sbicMsgInAck (SCSI_CTRL *, BOOL);
LOCAL STATUS sbicBytesOut (SCSI_PHYS_DEV *, char *, int, int);
LOCAL STATUS sbicBytesIn (SCSI_PHYS_DEV *, char *, int, int);
LOCAL STATUS sbicBusPhaseGet (SCSI_CTRL *, int, int *);
LOCAL void sbicSelTimeOutCvt (SCSI_CTRL *, UINT, UINT *);
LOCAL void sbicScsiBusReset (SBIC *);
LOCAL void sbicHwInit (SBIC *);

WD_33C93_SCSI_CTRL *wd33c93CtrlCreate (FAST UINT8 *, int, UINT, int,
					     FUNCPTR, FUNCPTR, FUNCPTR);
LOCAL STATUS wd33c93CtrlInit (FAST SBIC *, FAST int, FAST UINT, int);
LOCAL STATUS sbicXferCountSet (FAST SBIC *, FAST UINT);
LOCAL void sbicXferCountGet (FAST SBIC *, FAST int *);
LOCAL void sbicCommand (SBIC *, UINT8);
LOCAL void sbicIntr (SBIC *);
LOCAL void sbicRegRead (SBIC *, UINT8, int *);
LOCAL void sbicRegWrite (SBIC *, UINT8, UINT8);
LOCAL STATUS wd33c93Show (FAST SCSI_CTRL *);

/*******************************************************************************
*
* wd33c93Scsi1IfInit - initialize the SCSI-2 interface to wd33c93
*
* NOMANUAL
*/
void wd33c93Scsi1IfInit ()
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
* wd33c93CtrlCreate - create and partially initialize a WD33C93 SBIC structure
*
* This routine creates an SBIC data structure and must be called before using
* an SBIC chip.  It should be called once and only once for a specified SBIC.  
* Since it allocates memory for a structure needed by all routines in
* wd33c93Lib, it must be called before any other routines in the library.
* After calling this routine, at least one call to wd33c93CtrlInit() should
* be made before any SCSI transaction is initiated using the SBIC.
*
* Note that only the non-multiplexed processor interface is supported.
*
* The input parameters are as follows:
* .iP <sbicBaseAdrs> 4
* the address where the CPU accesses the lowest register of
* the SBIC.
* .iP <regOffset>
* the address offset (in bytes) to access consecutive registers.  (This must
* be a power of 2; for example, 1, 2, 4, etc.)
* .iP <clkPeriod>
* the period, in nanoseconds, of the signal-to-SBIC clock input used only for
* select command timeouts.
* .iP <devType>
* a constant corresponding to the type (part number) of this controller; 
* possible options are enumerated in wd33c93.h under the heading "SBIC 
* device type."
* .iP <sbicScsiReset>
* a board-specific routine to assert the RST line on the SCSI bus, which causes
* all connected devices to return to a known quiescent state.
* .iP "<spcDmaBytesIn> and <spcDmaBytesOut>"
* board-specific routines to handle DMA input and output.  
* If these are NULL (0), SBIC program transfer mode is used.
* DMA is implemented only during SCSI data in/out phases.
* The interface to these DMA routines must be of the form:
* .CS
*     STATUS xxDmaBytes{In, Out}
*         (
*         SCSI_PHYS_DEV  *pScsiPhysDev,  /@ ptr to phys dev info    @/
*         UINT8          *pBuffer,       /@ ptr to the data buffer @/
*         int            bufLength       /@ number of bytes to xfer @/
*         )
* .CE
*
* RETURNS: A pointer to the SBIC control structure,
* or NULL if memory is insufficient or parameters are invalid.
*
* SEE ALSO: wd33c93.h
*/

WD_33C93_SCSI_CTRL *wd33c93CtrlCreate 
    (
    FAST UINT8 *sbicBaseAdrs,   /* base address of SBIC */
    int         regOffset,      /* addr offset between consecutive regs. */
    UINT        clkPeriod,      /* period of controller clock (nsec) */
    int         devType,        /* SBIC device type */
    FUNCPTR     sbicScsiReset,  /* SCSI bus reset function */
    FUNCPTR     sbicDmaBytesIn, /* SCSI DMA input function */
    FUNCPTR     sbicDmaBytesOut /* SCSI DMA output function */
    )
    {
    FAST SBIC *pSbic;		/* ptr to SBIC info */

    /* initialize the driver function table */

    wd33c93Scsi1IfInit ();

    /* verify input parameters */

    if (regOffset == 0)
        return ((SBIC *) NULL);

    /* calloc the controller info structure; return NULL if unable */

    if ((pSbic = (SBIC *) calloc (1, sizeof (SBIC))) == NULL)
        return ((SBIC *) NULL);

    /* fill in generic SCSI info for this controller */

    scsiCtrlInit (&pSbic->scsiCtrl);

    /* fill in SBIC specific data for this controller */

    pSbic->pAdrsReg    = sbicBaseAdrs;
    pSbic->pAuxStatReg = sbicBaseAdrs;
    pSbic->pRegFile    = sbicBaseAdrs + regOffset;

    pSbic->devType     = devType;

    pSbic->scsiCtrl.clkPeriod = clkPeriod;
    pSbic->scsiCtrl.maxBytesPerXfer = WD_33C93_MAX_BYTES_PER_XFER;

    /* fill in board-specific SCSI bus reset routine */

    pSbic->scsiCtrl.scsiBusReset      = sbicScsiBusReset;

    /* fill in driver-specific routines for scsiLib interface */

    pSbic->scsiCtrl.scsiTransact      = scsiTransact;
    pSbic->scsiCtrl.scsiDevSelect     = sbicDevSelect;
    pSbic->scsiCtrl.scsiBytesIn	      = sbicBytesIn;
    pSbic->scsiCtrl.scsiBytesOut      = sbicBytesOut;
    pSbic->scsiCtrl.scsiDmaBytesIn    = sbicDmaBytesIn;
    pSbic->scsiCtrl.scsiDmaBytesOut   = sbicDmaBytesOut;
    pSbic->scsiCtrl.scsiBusPhaseGet   = sbicBusPhaseGet;
    pSbic->scsiCtrl.scsiMsgInAck      = sbicMsgInAck;
    pSbic->scsiCtrl.scsiSelTimeOutCvt = sbicSelTimeOutCvt;

    pSbic->sbicScsiReset = sbicScsiReset;

    return (pSbic);
    }

/*******************************************************************************
*
* wd33c93CtrlInit - initialize the user-specified fields in a WD33C93 SBIC structure
*
* This routine initializes an SBIC structure, after the structure is created
* with wd33c93CtrlCreate().  This structure must be initialized before the SBIC
* can be used.  It may be called more than once; however, it should
* be called only while there is no activity on the SCSI interface.
*
* Before returning, this routine pulses RST (reset) on the SCSI bus, thus
* resetting all attached devices.
*
* The input parameters are as follows:
* .iP <pSbic> 4
* a pointer to the WD_33C93_SCSI_CTRL structure created with
* wd33c93CtrlCreate().
* .iP <scsiCtrlBusId>
* the SCSI bus ID of the SBIC, in the range 0 - 7.  The ID is somewhat 
* arbitrary; the value 7, or highest priority, is conventional.
* .iP <defaultSelTimeOut>
* the timeout, in microseconds, for selecting a SCSI device attached to this
* controller.  This value is used as a default if no timeout is specified in
* scsiPhysDevCreate().  The recommended value zero (0) specifies
* SCSI_DEF_SELECT_TIMEOUT (250 millisec).  The maximum timeout possible is
* approximately 2 seconds.  Values exceeding this revert to the
* maximum.  For more information about chip timeouts, see the manuals
* .I "Western Digital WD33C92/93 SCSI-Bus Interface Controller,"
* .I "Western Digital WD33C92A/93A SCSI-Bus Interface Controller."
* .iP <scsiPriority>
* the priority to which a task is set when performing a SCSI
* transaction.  Valid priorities are 0 to 255.  Alternatively, the value -1 
* specifies that the priority should not be altered during SCSI transactions.
*
* RETURNS: OK, or ERROR if a parameter is out of range.
*
* SEE ALSO: scsiPhysDevCreate(),
* .I "Western Digital WD33C92/93 SCSI-Bus Interface Controller,"
* .I "Western Digital WD33C92A/93A SCSI-Bus Interface Controller"
*/

LOCAL STATUS wd33c93CtrlInit
    (
    FAST SBIC *pSbic,             /* ptr to SBIC info                       */
    FAST int   scsiCtrlBusId,     /* SCSI bus ID of this SBIC               */
    FAST UINT  defaultSelTimeOut, /* default dev. select timeout (microsec) */
    int        scsiPriority       /* priority of task when doing SCSI I/O   */
    )
    {
    UINT tempSelTimeOut;	/* temp. value of select timeout (no units) */

    /* verify scsiCtrlBusId and enter legal value in SBIC structure */

    SCSI_DEBUG_MSG ("scsiCtrlBusId = %d\n", scsiCtrlBusId, 0, 0, 0, 0, 0);

    if (scsiCtrlBusId < SCSI_MIN_BUS_ID || scsiCtrlBusId > SCSI_MAX_BUS_ID)
	return (ERROR);

    pSbic->scsiCtrl.scsiCtrlBusId = scsiCtrlBusId;

    if (scsiPriority < NONE || scsiPriority > 0xff)
        return (ERROR);

    pSbic->scsiCtrl.scsiPriority = scsiPriority;

    /* verify defaultSelTimeOut, convert it from usec to SBIC register values,
     * and enter value in SBIC structure
     */

    sbicSelTimeOutCvt (&pSbic->scsiCtrl, defaultSelTimeOut, &tempSelTimeOut);
    pSbic->defaultSelTimeOut = (UINT8) tempSelTimeOut;

    /* disconnect not supported for now */

    pSbic->scsiCtrl.disconnect = (TBOOL) FALSE;

    sbicHwInit (pSbic); 	/* initialize the SBIC hardware */

    return (OK);
    }

/*******************************************************************************
*
* sbicDevSelect - attempt to select a SCSI device
*
* This routine is intended to be called from scsiLib, not directly.
*
* RETURNS: OK if the device was successfully selected, otherwise ERROR.
*/

LOCAL STATUS sbicDevSelect
    (
    FAST SCSI_PHYS_DEV *pScsiPhysDev,   /* ptr to SCSI physical device info */
    SCSI_TRANSACTION   *pScsiXaction    /* ptr to SCSI transaction info	    */
    )
    {
    FAST SBIC *pSbic;		/* ptr to SBIC info */
    int sbicBusId;		/* SCSI bus ID of the SBIC */
    UINT8 identMsg;		/* for construction of the IDENTIFY message */
    STATUS status;		/* placeholder for status */
    int scsiPhase;		/* SCSI bus phase following the select */

    pSbic = (SBIC *) pScsiPhysDev->pScsiCtrl;

    sbicRegRead (pSbic, SBIC_REG_OWN_ID, &sbicBusId);

    if (sbicBusId == pScsiPhysDev->scsiDevBusId)
	{
	errnoSet (S_scsiLib_ILLEGAL_BUS_ID);
        return (ERROR);
	}

    sbicRegWrite (pSbic, SBIC_REG_DEST_ID, (UINT8) pScsiPhysDev->scsiDevBusId);
    sbicRegWrite (pSbic, SBIC_REG_TO_PERIOD, (UINT8) pScsiPhysDev->selTimeOut);
    pSbic->pDevToSelect = pScsiPhysDev;
    sbicCommand (pSbic, pScsiPhysDev->useIdentify ?
		  	SBIC_CMD_SEL_ATN :
		  	SBIC_CMD_SELECT);

    semTake (&pScsiPhysDev->devSyncSem, WAIT_FOREVER);

    if (pScsiPhysDev->devStatus == SELECT_SUCCESSFUL)
	{
	if (pScsiPhysDev->useIdentify) 	/* send an identify message */
    	    {
	    if ((sbicBusPhaseGet (&pSbic->scsiCtrl, 0, &scsiPhase) == ERROR) ||
		(scsiPhase != SCSI_MSG_OUT_PHASE))
		return (ERROR);

            identMsg = SCSI_MSG_IDENTIFY |
		       (pSbic->scsiCtrl.disconnect ?
			SCSI_MSG_IDENT_DISCONNECT : 0) |
		       (UINT8) pScsiPhysDev->scsiDevLUN;

	    status = sbicProgBytesOut (pScsiPhysDev, &identMsg,
				       sizeof (identMsg), SCSI_MSG_OUT_PHASE);

	    if (status == ERROR)
		return (ERROR);
            }

        return (OK);
	}

    if (pScsiPhysDev->devStatus == SELECT_TIMEOUT)
	errnoSet (S_scsiLib_SELECT_TIMEOUT);

    return (ERROR);
    }

/*******************************************************************************
*
* sbicMsgInAck - issue a NEG_ACK command thereby accepting an incoming message
*
* This routine should be called when an incoming message has been read and
* accepted.  If the incoming message did not imply an impending disconnect,
* give the synchronization semaphore, so that subsequent phase can be detected.
*
* RETURNS: OK.
*/

LOCAL STATUS sbicMsgInAck
    (
    FAST SCSI_CTRL *pScsiCtrl,  /* ptr to SBIC info                 */
    BOOL expectDisconn          /* whether a disconnect is expected */
    )
    {
    sbicCommand ((SBIC *) pScsiCtrl, SBIC_CMD_NEG_ACK);

    if (!expectDisconn)
        semGive (&pScsiCtrl->ctrlSyncSem);

    return (OK);
    }

/*******************************************************************************
*
* sbicProgBytesOut - output data bytes to SCSI using 'program' transfer
*
* This routine transfers the data from the data buffer into the
* chip's data register.
*
* RETURNS: OK, or ERROR if <bufLength> is out of range.
*
* NOMANUAL
*/

STATUS sbicProgBytesOut
    (
    SCSI_PHYS_DEV *pScsiPhysDev, /* ptr to drive info in SBIC	      */
    FAST UINT8 *pBuffer,         /* pointer to the data buffer	      */
    FAST int bufLength,          /* number of bytes to be transferred */
    int scsiPhase                /* phase of the transfer	      */
    )
    {
    FAST SBIC *pSbic;		/* ptr to SBIC info */

    SCSI_DEBUG_MSG ("sbicProgBytesOut: bufLength = %d, scsiPhase = %d\n",
		    bufLength, scsiPhase, 0, 0, 0, 0);

    pSbic = (SBIC *) pScsiPhysDev->pScsiCtrl;

    if (bufLength != 1)
	{
        if (sbicXferCountSet (pSbic, (UINT) bufLength) == ERROR)
	    return (ERROR);
        sbicCommand (pSbic, SBIC_CMD_XFER_INFO);
	}
    else
        sbicCommand (pSbic, SBIC_CMD_XFER_INFO | SBIC_CMD_SING_BYTE_XFER);

    *pSbic->pAdrsReg = SBIC_REG_DATA;

    while (bufLength-- > 0)
	{
	while (!(*pSbic->pAuxStatReg & SBIC_AUX_STAT_DBUF_READY))
	    ;

	*pSbic->pRegFile = *pBuffer++;
	}

    return (OK);
    }

/*******************************************************************************
*
* sbicProgBytesIn - input data bytes from SCSI using 'program' transfer
*
* This routine transfers data into the data buffer from the chip's
* data register.
*
* RETURNS: OK, or ERROR if <bufLength> is out of range.
*
* NOMANUAL
*/

STATUS sbicProgBytesIn
    (
    SCSI_PHYS_DEV *pScsiPhysDev,   /* ptr to drive info in SBIC         */
    FAST UINT8    *pBuffer,        /* pointer to the data buffer        */
    FAST int       bufLength,      /* number of bytes to be transferred */
    int            scsiPhase       /* phase of the transfer             */
    )
    {
    FAST SBIC *pSbic;		/* ptr to SBIC info */

    SCSI_DEBUG_MSG ("sbicProgBytesIn: bufLength = %d, scsiPhase = %d\n",
		    bufLength, scsiPhase, 0, 0, 0, 0);

    pSbic = (SBIC *) pScsiPhysDev->pScsiCtrl;

    if (bufLength != 1)
	{
        if (sbicXferCountSet (pSbic, (UINT) bufLength) == ERROR)
	    return (ERROR);

        sbicCommand (pSbic, SBIC_CMD_XFER_INFO);
	}
    else
        sbicCommand (pSbic, SBIC_CMD_XFER_INFO | SBIC_CMD_SING_BYTE_XFER);

    *pSbic->pAdrsReg = SBIC_REG_DATA;

    while (bufLength-- > 0)
	{
	while (!(*pSbic->pAuxStatReg & SBIC_AUX_STAT_DBUF_READY))
	    ;

	*pBuffer++ = *pSbic->pRegFile;
	}

    if (scsiPhase == SCSI_MSG_IN_PHASE)
	{
	semTake (&pSbic->scsiCtrl.ctrlSyncSem, WAIT_FOREVER);
	sbicMsgInAck ((SCSI_CTRL *) pScsiPhysDev->pScsiCtrl, TRUE);
	}

    return (OK);
    }

/*******************************************************************************
*
* sbicBytesOut - switch to appropriate output routine
*
* This routine executes the scsiDmaBytesOut() routine if not NULL, otherwise
* sbicProgBytesOut() is run.
*
* RETURNS: The return value from the called routine.
*/

LOCAL STATUS sbicBytesOut 
    (
    FAST SCSI_PHYS_DEV *pScsiPhysDev,   /* ptr to SCSI physical dev info     */
    FAST char          *pBuffer,        /* ptr to byte buffer for output     */
    int                 bufLength,      /* number of bytes to be transferred */
    int                 scsiPhase       /* SCSI bus phase of the transfer    */
    )
    {
    FAST SBIC *pSbic;			/* ptr to SBIC info */

    pSbic = (SBIC *) pScsiPhysDev->pScsiCtrl;

    if ((scsiPhase != SCSI_DATA_OUT_PHASE) ||
        (pSbic->scsiCtrl.scsiDmaBytesOut == NULL))
	{
        return (sbicProgBytesOut (pScsiPhysDev, (UINT8 *) pBuffer,
                                  bufLength, scsiPhase));
	}
    else
	{
        return ((pSbic->scsiCtrl.scsiDmaBytesOut)
                (pScsiPhysDev, (UINT8 *) pBuffer, bufLength));
	}
    }

/*******************************************************************************
*
* sbicBytesIn - switch to appropriate input routine
*
* This routine executes the scsiDmaBytesIn() routine if not NULL, otherwise
* sbicPorgBytesIn() is run.
*
* RETURNS: The return value from the called routine.
*/

LOCAL STATUS sbicBytesIn 
    (
    FAST SCSI_PHYS_DEV *pScsiPhysDev,   /* ptr to SCSI physical dev info     */
    FAST char          *pBuffer,        /* ptr to byte buffer for output     */
    int                 bufLength,      /* number of bytes to be transferred */
    int                 scsiPhase       /* SCSI bus phase of the transfer    */
    )
    {
    FAST SBIC *pSbic;                   /* ptr to SBIC info */

    pSbic = (SBIC *) pScsiPhysDev->pScsiCtrl;

    if ((scsiPhase != SCSI_DATA_IN_PHASE) ||
        (pSbic->scsiCtrl.scsiDmaBytesIn == NULL))
	{
        return (sbicProgBytesIn (pScsiPhysDev, (UINT8 *) pBuffer,
                                  bufLength, scsiPhase));
	}
    else
	{
        return ((pSbic->scsiCtrl.scsiDmaBytesIn)
                (pScsiPhysDev, (UINT8 *) pBuffer, bufLength));
	}
    }

/*******************************************************************************
*
* sbicBusPhaseGet - return the current SCSI bus phase in *pBusPhase
*
* This routine gets the value of the current SCSI bus phase.
*
* RETURNS: OK, or ERROR if no phase is detected.
*/

LOCAL STATUS sbicBusPhaseGet
    (
    FAST SCSI_CTRL *pScsiCtrl,     /* ptr to SCSI controller info     */
    int             timeOutInUsec, /* timeout in usec (0 == infinity) */
    FAST int       *pBusPhase      /* ptr to returned bus phase       */
    )
    {
    FAST SBIC * pSbic;		/* ptr to SBIC info */
    int timeOut;

    pSbic = (SBIC *) pScsiCtrl;

    if (timeOutInUsec > SCSI_TIMEOUT_5SEC)
	timeOut = ((timeOutInUsec / SCSI_TIMEOUT_1SEC) * sysClkRateGet ());
    else
	timeOut = ((timeOutInUsec * sysClkRateGet()) / SCSI_TIMEOUT_1SEC);

    if (timeOut == 0)
	timeOut = WAIT_FOREVER;

    if (semTake (&pScsiCtrl->ctrlSyncSem, timeOut) == ERROR)
	{
	sbicScsiBusReset (pSbic);	/* the bus is hung(?), reset it */
	return (ERROR);
	}

    if (pSbic->scsiCtrl.scsiBusPhase == NONE)
	return (ERROR);

    *pBusPhase = pSbic->scsiCtrl.scsiBusPhase;
    return (OK);
    }

/*******************************************************************************
*
* sbicSelTimeOutCvt - convert a select timeout from usec to SBIC units
*
* The conversion formula is given on p. 11 of the Western Digital WD33C93
* manual.  Note that 0 specifies the default setting of 250 usec.  Also,
* the SBIC accepts up to a 8-bit timeout, so a maximum value of 0xff is
* returned in *pTimeOutSetting.
*
* RETURNS: N/A.
*/

LOCAL void sbicSelTimeOutCvt
    (
    FAST SCSI_CTRL *pScsiCtrl,          /* ptr to SCSI controller info */
    FAST UINT       timeOutInUsec,      /* timeout in microsecs        */
    FAST UINT      *pTimeOutSetting     /* ptr to result               */
    )
    {
    FAST UINT tempSelTimeOut;	/* temp. select timeout setting */

    if (timeOutInUsec == (UINT) 0)
	timeOutInUsec = SCSI_DEF_SELECT_TIMEOUT;

    tempSelTimeOut = (timeOutInUsec / (80 * pScsiCtrl->clkPeriod)) + 1;

    if (tempSelTimeOut > 0xff)
	tempSelTimeOut = 0xff;

    *pTimeOutSetting = tempSelTimeOut;
    }

/*******************************************************************************
*
* sbicXferCountSet - load the SBIC transfer counter with the specified count
*
* RETURNS: OK, or ERROR if <count> is not in the
* range 0 - WD_33C93_MAX_BYTES_PER_XFER.
*
* NOMANUAL
*/

STATUS sbicXferCountSet
    (
    FAST SBIC *pSbic,   /* ptr to SBIC info    */
    FAST UINT  count    /* count value to load */
    )
    {
    int nbits = 24;

    if (count > WD_33C93_MAX_BYTES_PER_XFER)
        return (ERROR);

    *pSbic->pAdrsReg = (SBIC_REG_XFER_COUNT_MSB);

    while (nbits)
        {
        *pSbic->pRegFile = (UINT8) ((count >> (nbits - 8)) & 0xff);
        nbits -= 8;
        }

    return (OK);
    }

/*******************************************************************************
*
* sbicXferCountGet - fetch the SBIC transfer count
*
* The value of the transfer counter is copied to *pCount.
*
* RETURNS: N/A.
*
* NOMANUAL
*/

void sbicXferCountGet
    (
    FAST SBIC *pSbic,   /* ptr to SBIC info      */
    FAST int  *pCount   /* ptr to returned value */
    )
    {
    FAST UINT tempCount = (UINT) 0;

    *pSbic->pAdrsReg = SBIC_REG_XFER_COUNT_MSB;

    tempCount |= ((UINT) *pSbic->pRegFile) << 16;
    tempCount |= ((UINT) *pSbic->pRegFile) << 8;
    tempCount |=  (UINT) *pSbic->pRegFile;
    *pCount = (int) tempCount;
    }

/*******************************************************************************
*
* sbicCommand - write a command code to the SBIC Command Register
*
* RETURNS: N/A.
*
* NOMANUAL
*/

void sbicCommand
    (
    SBIC *pSbic,        /* ptr to SBIC info */
    UINT8 cmdCode       /* new command code */
    )
    {
    /* delay until previous command has been interpretted */

    while (*pSbic->pAuxStatReg & SBIC_AUX_STAT_CMD_IN_PROG)
	;

    /* load new command */

    *pSbic->pAdrsReg = SBIC_REG_COMMAND;
    *pSbic->pRegFile = cmdCode;
    }

/*******************************************************************************
*
* sbicScsiBusReset - assert the RST line on the SCSI bus
*
* This routine asserts the RST line on the SCSI bus, which should cause
* all connected devices to return to a known quiescent state.
*
* RETURNS: N/A.
*/

LOCAL void sbicScsiBusReset
    (
    FAST SBIC *pSbic    /* ptr to SBIC info */
    )
    {
    (*pSbic->sbicScsiReset) (pSbic);
    }

/*******************************************************************************
*
* sbicIntr - interrupt service routine for the SBIC
*
* RETURNS: N/A.
*
* NOMANUAL
*/

LOCAL void sbicIntr
    (
    SBIC *pSbic          /* ptr to SBIC info */
    )
    {
    FAST UINT8 scsiStatus;
    FAST UINT8 statusGroup;

    /* read 'SCSI Status' Reg. and determine status group and code */

    *pSbic->pAdrsReg = SBIC_REG_SCSI_STATUS;
    scsiStatus = pSbic->scsiStatusCode = *pSbic->pRegFile;

    statusGroup = STAT_GROUP(scsiStatus);

    SCSI_INT_DEBUG_MSG ("*** SBIC INTERRUPT *** ", 0, 0, 0, 0, 0, 0);

    switch (statusGroup)
	{
	case STAT_GROUP_RESET:
	    SCSI_INT_DEBUG_MSG ("SBIC Reset interrupt, Code = 0x%x\n",
				scsiStatus, 0, 0, 0, 0, 0);

	    break;

	case STAT_GROUP_SUCCESSFUL:
	    SCSI_INT_DEBUG_MSG ("SBIC Successful Completion interrupt, \
				Code = 0x%x\n", scsiStatus, 0, 0, 0, 0, 0);
	    switch (scsiStatus)
		{
		case STAT_SUCC_SELECT:
		    pSbic->pDevToSelect->devStatus = SELECT_SUCCESSFUL;
		    semGive (&pSbic->pDevToSelect->devSyncSem);
		    break;
		}
	    break;

	case STAT_GROUP_PAUSE_ABORTED:
	    SCSI_INT_DEBUG_MSG ("SBIC Aborted interrupt, Code = 0x%x\n",
				scsiStatus, 0, 0, 0, 0, 0);
	    switch (scsiStatus)
		{
		case STAT_PAUSE_MSG_IN:
		    semGive (&pSbic->scsiCtrl.ctrlSyncSem);
		    break;
		}
	    break;

	case STAT_GROUP_TERMINATED:
	    SCSI_INT_DEBUG_MSG ("SBIC Terminated interrupt, Code = 0x%x\n",
				scsiStatus, 0, 0, 0, 0, 0);
	    switch (scsiStatus)
		{
		case STAT_TERM_TIMEOUT:
		    pSbic->pDevToSelect->devStatus = SELECT_TIMEOUT;
		    semGive (&pSbic->pDevToSelect->devSyncSem);
		    break;
		}
	    break;

	case STAT_GROUP_SERVICE_REQ:
	    SCSI_INT_DEBUG_MSG ("SBIC Serv. Req. interrupt, Code = 0x%x\n",
				scsiStatus, 0, 0, 0, 0, 0);
	    switch (scsiStatus)
		{
		case STAT_SERV_REQ_DISCON:
		    pSbic->scsiCtrl.scsiBusPhase = SCSI_BUS_FREE_PHASE;
		    semGive (&pSbic->scsiCtrl.ctrlSyncSem);
		    break;
		}
	    break;

	default:
	    SCSI_INT_DEBUG_MSG ("SBIC << Unrecognized >> interrupt, \
				Code = 0x%x\n", scsiStatus, 0, 0, 0, 0, 0);
	}

    if (scsiStatus & STAT_PHASE_REQ)
	{
	pSbic->scsiCtrl.scsiBusPhase = (int) (scsiStatus & STAT_MCI_BITS);
	semGive (&pSbic->scsiCtrl.ctrlSyncSem);
	}
    }

/*******************************************************************************
*
* sbicHwInit - initialize the SBIC to a known state
*
* This routine puts the SBIC into a known quiescent state and issues a reset
* to the SCSI bus if any signals are active, thus putting target devices in
* some presumably known state.  Currently the initial state is not configurable
* and does not enable reselection.
*
* RETURNS: N/A.
*/

LOCAL void sbicHwInit
    (
    SBIC *pSbic                 /* ptr to an SBIC structure */
    )
    {
    int tempOwnIdReg = 0;	/* where to form value for "own ID" reg */

    if (pSbic->devType == SBIC_WD33C93A)
	{
	if (pSbic->scsiCtrl.clkPeriod >= 100)
	    tempOwnIdReg = SBIC_FREQ_SELECT_LOW;
	else if (pSbic->scsiCtrl.clkPeriod >= 66)
	    tempOwnIdReg = SBIC_FREQ_SELECT_MID;
	else
	    tempOwnIdReg = SBIC_FREQ_SELECT_HIGH;
	}

    tempOwnIdReg |= pSbic->scsiCtrl.scsiCtrlBusId;

    sbicScsiBusReset (pSbic);

    sbicRegWrite(pSbic, SBIC_REG_OWN_ID, (UINT8) tempOwnIdReg);
    sbicCommand (pSbic, SBIC_CMD_RESET);
    }


/*******************************************************************************
*
* sbicRegRead - Get the contents of a specified SBIC register
*
* RETURNS: N/A.
*
* NOMANUAL
*/

LOCAL void sbicRegRead
    (
    SBIC *pSbic,         /* ptr to an SBIC structure */
    UINT8 regAdrs,       /* register to read         */
    int  *pDatum         /* put data here            */
    )
    {
    *pSbic->pAdrsReg = regAdrs;
    *pDatum = (int) *pSbic->pRegFile;
    }

/*******************************************************************************
*
* sbicRegWrite - write a value to a specified SBIC register
*
* RETURNS: N/A.
*
* NOMANUAL
*/

LOCAL void sbicRegWrite
    (
    SBIC *pSbic,         /* ptr to an SBIC structure */
    UINT8 regAdrs,       /* register to write        */
    UINT8 datum          /* data to write            */
    )
    {
    *pSbic->pAdrsReg = regAdrs;
    *pSbic->pRegFile = datum;
    }

/*******************************************************************************
*
* wd33c93Show - display the values of all readable WD33C93 chip registers
*
* This routine displays the state of the SBIC registers in a user-friendly
* manner.  It is useful primarily for debugging.  It should not be invoked 
* while another running process is accessing the SCSI controller.
*
* EXAMPLE:
* .CS
*     -> sbicShow
*     REG #00 (Own ID         ) = 0x07
*     REG #01 (Control        ) = 0x00
*     REG #02 (Timeout Period ) = 0x20
*     REG #03 (Sectors        ) = 0x00
*     REG #04 (Heads          ) = 0x00
*     REG #05 (Cylinders MSB  ) = 0x00 
*     REG #06 (Cylinders LSB  ) = 0x00
*     REG #07 (Log. Addr. MSB ) = 0x00
*     REG #08 (Log. Addr. 2SB ) = 0x00
*     REG #09 (Log. Addr. 3SB ) = 0x00
*     REG #0a (Log. Addr. LSB ) = 0x00
*     REG #0b (Sector Number  ) = 0x00
*     REG #0c (Head Number    ) = 0x00 
*     REG #0d (Cyl. Number MSB) = 0x00
*     REG #0e (Cyl. Number LSB) = 0x00
*     REG #0f (Target LUN     ) = 0x00
*     REG #10 (Command Phase  ) = 0x00
*     REG #11 (Synch. Transfer) = 0x00
*     REG #12 (Xfer Count MSB ) = 0x00
*     REG #13 (Xfer Count 2SB ) = 0x00
*     REG #14 (Xfer Count LSB ) = 0x00
*     REG #15 (Destination ID ) = 0x03
*     REG #16 (Source ID      ) = 0x00
*     REG #17 (SCSI Status    ) = 0x42
*     REG #18 (Command        ) = 0x07
* .CE
*
* RETURNS: OK, or ERROR if <pScsiCtrl> and <pSysScsiCtrl> are both NULL.
*/

LOCAL STATUS wd33c93Show
    (
    FAST SCSI_CTRL *pScsiCtrl   /* ptr to SCSI controller info */
    )
    {
    FAST int ix;
    FAST SBIC *pSbic;		/* ptr to SBIC info */
    LOCAL char * regName [] =
	{
	"Own ID	        ",
	"Control        ",
	"Timeout Period ",
	"Sectors        ",
	"Heads          ",
	"Cylinders MSB  ",
	"Cylinders LSB  ",
	"Log. Addr. MSB ",
	"Log. Addr. 2SB ",
	"Log. Addr. 3SB ",
	"Log. Addr. LSB ",
	"Sector Number  ",
	"Head Number    ",
	"Cyl. Number MSB",
	"Cyl. Number LSB",
	"Target LUN     ",
	"Command Phase  ",
	"Synch. Transfer",
	"Xfer Count MSB ",
	"Xfer Count 2SB ",
	"Xfer Count LSB ",
	"Destination ID ",
	"Source ID      ",
	"SCSI Status    ",
	"Command        "
	};

    if (pScsiCtrl == NULL)
        {
	if (pSysScsiCtrl == NULL)
	    {
	    printErr ("No SCSI controller specified.\n");
	    return (ERROR);
	    }

        pScsiCtrl = pSysScsiCtrl;
        }

    pSbic = (SBIC *) pScsiCtrl;

    for (ix = SBIC_REG_OWN_ID; ix <= SBIC_REG_COMMAND; ix++)
	{
        *pSbic->pAdrsReg = (UINT8) ix;
	printf ("REG #%02x (%s) = 0x%02x\n", ix, regName [ix],
		*pSbic->pRegFile);
	}

    return (OK);
    }

