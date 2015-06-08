/* mb87030Lib.c - Fujitsu MB87030 SCSI Protocol Controller (SPC) library */

/* Copyright 1989-1996 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01u,14dec94,jds  timeout too small in spcBusPhaseGet(); spr 3877 fixed
01t,26mar94,ccc  bus free race condition fix for spr 3197
01s,23feb93,jdi  documentation cleanup.
01r,11feb93,ccc  fixed error in comment for select timeout (spr 1901).
01q,26sep92,ccc  renamed spcShow() to mb87030Show() and added example.
01p,24sep92,ccc  made change in spcManBytesIn() to handle extended message.
		 spcShow() now returns OK or ERROR if SCSI not configured.
01o,07aug92,ccc  added DMA support.
01n,18jul92,smb  Changed errno.h to errnoLib.h.
01m,26may92,rrr  the tree shuffle
		  -changed includes to have absolute path from h/
01l,07oct91,rrr  some forward decls
01k,04oct91,rrr  passed through the ansification filter
                  -changed functions to ansi style
		  -changed includes to have absolute path from h/
		  -changed VOID to void
		  -changed copyright notice
01j,05apr91,jdi	 documentation -- removed header parens and x-ref numbers;
		 doc review by jcc.
01i,29feb91,jaa	 documentation.
01h,09oct90,jcc  lint.
01g,03oct90,jcc  changed leastSigAdrsBit to regOffset in mb87030CtrlCreate();
		 eliminated busFreeDelay parameter to mb87030CtrlInit().
01f,02oct90,jcc  UTINY became UINT8; changes in sem{Give, Take}() calls
		 since SEM_ID's became SEMAPHORE's in various structures;
		 malloc() became calloc() in wd33c93CtrlCreate(); miscellaneous.
01e,10aug90,dnw  added forward declarations for void functions.
01d,18jul90,jcc  made semTake() calls 5.0 compatible; clean-up.
01c,08jun90,jcc  fixed bug in handling of incoming messages. documentation.
01b,23mar90,jcc  removed DMA and disconnect options from mb87030CtrlInit().
		 lint.
01a,19jan90,jcc  formerly the device specific part of spcScsiDrv.c.
*/

/*
DESCRIPTION
This is the I/O driver for the Fujitsu MB87030 SCSI Protocol Controller
(SPC) chip.  It is designed to work in conjunction with scsiLib.

USER-CALLABLE ROUTINES
Most of the routines in this driver are accessible only through the I/O
system.  Two routines, however, must be called directly:  mb87030CtrlCreate()
to create a controller structure, and mb87030CtrlInit() to initialize
the controller structure.

INCLUDE FILES
mb87030.h

SEE ALSO: scsiLib,
.I "Fujitsu Small Computer Systems Interface MB87030 Synchronous/Asynchronous Protocol Controller Users Manual,"
.pG "I/O System"
*/

#include "vxWorks.h"
#include "drv/scsi/mb87030.h"
#include "memLib.h"
#include "errnoLib.h"
#include "logLib.h"
#include "scsiLib.h"
#include "stdlib.h"
#include "stdio.h"
#include "sysLib.h"

/* defines */

#define SPC_MAX_XFER_LENGTH  	(UINT) (0x00800000) /* max data xfer length */
#define SPC_DEF_BUS_FREE_DELAY  3	/* nominal value for bus free delay */

/* macros */
#define SPC_BUS_PHASE_SET(pSpc, busPhase) spcPctlSet (pSpc, -1, (UINT8)busPhase)

typedef MB_87030_SCSI_CTRL SPC;

IMPORT BOOL scsiDebug;
IMPORT BOOL scsiIntsDebug;

void spcCommand (SPC *pSpc, UINT8 cmdCode);
STATUS spcProgBytesOut (SCSI_PHYS_DEV *pScsiPhysDev, UINT8 *pBuffer,
    			int bufLength, int scsiPhase);
STATUS spcProgBytesIn (SCSI_PHYS_DEV *pScsiPhysDev, UINT8 *pBuffer,
    			int bufLength, int scsiPhase);
STATUS spcXferCountSet (SPC *pSpc, int count);

/* forward static functions */

static STATUS spcDevSelect (SCSI_PHYS_DEV *pScsiPhysDev, SCSI_TRANSACTION
		*pScsiXaction);
static STATUS spcMsgInAck (SCSI_CTRL *pScsiCtrl, BOOL expectDisconn);
static STATUS spcBytesOut (SCSI_PHYS_DEV *pScsiPhysDev, char *pBuffer, int
		bufLength, int scsiPhase);
static STATUS spcBytesIn (SCSI_PHYS_DEV *pScsiPhysDev, char *pBuffer, int
		bufLength, int scsiPhase);
static STATUS spcBusPhaseGet (SCSI_CTRL *pScsiCtrl, int timeOutInUsec, int
		*pBusPhase);
static int byteParityGet (UINT8 byte);
static void spcSelTimeOutCvt (SCSI_CTRL *pScsiCtrl, UINT timeOutInUsec, UINT
		*pTimeOutSetting);
static STATUS spcBusIdGet (SPC *pSpc, int *pBusId);
static void spcIntsEnable (SPC *pSpc);
static void spcIntsDisable (SPC *pSpc);
static void spcScmdSet (SPC *pSpc, UINT8 cmdCode, int resetOutBit, int
		interceptXferBit, int prgXferBit, int termModeBit);
static void spcTmodSet (SPC *pSpc, BOOL syncXfer, int maxOffset, int
		minPerMinusOne);
static void spcPctlSet (SPC *pSpc, int busFreeIntEnblBit, UINT8 xferPhase);
static void spcScsiBusReset (SPC *pSpc);
static void spcHwInit (SPC *pSpc);


/*******************************************************************************
*
* mb87030CtrlCreate - create a control structure for an MB87030 SPC
*
* This routine creates a data structure that must exist before the SPC chip can
* be used.  This routine should be called once and only once for a specified 
* SPC.  It should be the first routine called, since it allocates memory for
* a structure needed by all other routines in the library.
*
* After calling this routine, at least one call to mb87030CtrlInit() should
* be made before any SCSI transaction is initiated using the SPC chip.
*
* A detailed description of the input parameters follows:
* .iP <spcBaseAdrs> 4
* the address at which the CPU would access the lowest 
* register of the SPC.
* .iP <regOffset>
* the address offset (bytes) to access consecutive registers.
* (This must be a power of 2, for example, 1, 2, 4, etc.)
* .iP <clkPeriod>
* the period in nanoseconds of the signal to the SPC clock input (only
* used for select command timeouts).
* .iP <spcDataParity>
* the parity bit must be defined by one of the following constants,
* according to whether the input to SPC DP is GND, +5V, or a valid
* parity signal, respectively:
* .nf
*     SPC_DATA_PARITY_LOW
*     SPC_DATA_PARITY_HIGH
*     SPC_DATA_PARITY_VALID
* .fi
* .iP "<spcDmaBytesIn> and <spcDmaBytesOut>"
* pointers to board-specific routines to handle DMA input and output.  
* If these are NULL (0), SPC program transfer mode is used.
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
*
* RETURNS: A pointer to the SPC control structure, or NULL if memory 
* is insufficient or parameters are invalid.
*/

MB_87030_SCSI_CTRL *mb87030CtrlCreate
    (
    FAST UINT8 *spcBaseAdrs,    /* base address of SPC */
    int regOffset,              /* addr offset between consecutive regs. */
    UINT clkPeriod,             /* period of controller clock (nsec) */
    int spcDataParity,          /* type of input to SPC DP (data parity) */
    FUNCPTR spcDMABytesIn,      /* SCSI DMA input function */
    FUNCPTR spcDMABytesOut      /* SCSI DMA output function */
    )
    {
    FAST SPC *pSpc;		/* ptr to SPC info */

    /* verify parameters */

    if (regOffset == 0)
	return ((SPC *) NULL);

    if ((spcDataParity < SPC_DATA_PARITY_LOW) ||
	(spcDataParity > SPC_DATA_PARITY_VALID))
	return ((SPC *) NULL);

    /* malloc the controller info structure; return ERROR if unable */

    pSpc = (SPC *) malloc (sizeof (SPC));

    if (pSpc == (SPC *) NULL)
        return ((SPC *) NULL);

    /* fill in generic SCSI info for this controller */

    scsiCtrlInit (&pSpc->scsiCtrl);

    /* fill in SPC specific data for this controller */

    pSpc->pBdidReg = spcBaseAdrs;
    pSpc->pSctlReg = spcBaseAdrs + (0x1 * regOffset);
    pSpc->pScmdReg = spcBaseAdrs + (0x2 * regOffset);
    pSpc->pTmodReg = spcBaseAdrs + (0x3 * regOffset);
    pSpc->pIntsReg = spcBaseAdrs + (0x4 * regOffset);
    pSpc->pPsnsReg = spcBaseAdrs + (0x5 * regOffset);
    pSpc->pSdgcReg = spcBaseAdrs + (0x5 * regOffset);
    pSpc->pSstsReg = spcBaseAdrs + (0x6 * regOffset);
    pSpc->pSerrReg = spcBaseAdrs + (0x7 * regOffset);
    pSpc->pPctlReg = spcBaseAdrs + (0x8 * regOffset);
    pSpc->pMbcReg  = spcBaseAdrs + (0x9 * regOffset);
    pSpc->pDregReg = spcBaseAdrs + (0xa * regOffset);
    pSpc->pTempReg = spcBaseAdrs + (0xb * regOffset);
    pSpc->pTchReg  = spcBaseAdrs + (0xc * regOffset);
    pSpc->pTcmReg  = spcBaseAdrs + (0xd * regOffset);
    pSpc->pTclReg  = spcBaseAdrs + (0xe * regOffset);
    pSpc->pExbfReg = spcBaseAdrs + (0xf * regOffset);

    pSpc->spcDataParity = spcDataParity;

    pSpc->scsiCtrl.clkPeriod = clkPeriod;
    pSpc->scsiCtrl.maxBytesPerXfer = SPC_MAX_XFER_LENGTH;

    pSpc->scsiCtrl.scsiBusReset      = spcScsiBusReset;

    pSpc->scsiCtrl.scsiTransact      = scsiTransact;
    pSpc->scsiCtrl.scsiDevSelect     = spcDevSelect;
    pSpc->scsiCtrl.scsiBytesIn 	     = spcBytesIn;
    pSpc->scsiCtrl.scsiBytesOut	     = spcBytesOut;
    pSpc->scsiCtrl.scsiDmaBytesIn    = spcDMABytesIn;
    pSpc->scsiCtrl.scsiDmaBytesOut   = spcDMABytesOut;
    pSpc->scsiCtrl.scsiBusPhaseGet   = spcBusPhaseGet;
    pSpc->scsiCtrl.scsiMsgInAck      = spcMsgInAck;
    pSpc->scsiCtrl.scsiSelTimeOutCvt = spcSelTimeOutCvt;

    pSpc->pDevToSelect = (SCSI_PHYS_DEV *) NULL;

    return (pSpc);
    }

/*******************************************************************************
*
* mb87030CtrlInit - initialize a control structure for an MB87030 SPC
*
* This routine initializes an SPC control structure created by
* mb87030CtrlCreate().  It must be called before the SPC is used.  This
* routine can be called more than once; however, it should be
* called only while there is no activity on the SCSI interface.
*
* Before returning, this routine pulses RST (reset) on the SCSI bus, thus
* resetting all attached devices.
*
* The input parameters are as follows:
* .iP <pSpc> 4
* a pointer to the MB_87030_SCSI_CTRL structure created with
* mb87030CtrlCreate().
* .iP <scsiCtrlBusId>
* the SCSI bus ID of the SIOP, in the range 0 - 7.  The ID is somewhat 
* arbitrary; the value 7, or highest priority, is conventional.
* .iP <defaultSelTimeOut>
* the timeout, in microseconds, for selecting a SCSI device
* attached to this controller.  The recommended value 0
* specifies SCSI_DEF_SELECT_TIMEOUT (250 milliseconds).
* The maximum timeout possible is approximately 3 seconds.
* Values exceeding this revert to the maximum.
* .iP <scsiPriority>
* the priority to which a task is set when performing a SCSI
* transaction.  Valid priorities range from 0 to 255.  Alternatively, 
* the value -1 specifies that the priority should not be
* altered during SCSI transactions.
*
* RETURNS: OK, or ERROR if parameters are out of range.
*/

STATUS mb87030CtrlInit
    (
    FAST MB_87030_SCSI_CTRL *pSpc, /* ptr to SPC struct */
    FAST int scsiCtrlBusId,        /* SCSI bus ID of this SPC */
    FAST UINT defaultSelTimeOut,   /* default dev sel timeout (microsec) */
    int scsiPriority               /* priority of task doing SCSI I/O */
    )
    {
    UINT tempSelTimeOut;	/* temp. value of select time-out (no units) */
    int busFreeDelay = SPC_DEF_BUS_FREE_DELAY;
    int busFreeDelayParity;	/* parity of busFreeDelay parameter */

    /* verify scsiCtrlBusId and enter legal value in SPC structure */

    if (scsiCtrlBusId < SCSI_MIN_BUS_ID || scsiCtrlBusId > SCSI_MAX_BUS_ID)
	return (ERROR);

    pSpc->scsiCtrl.scsiCtrlBusId = (UINT8) scsiCtrlBusId;

    /* verify scsiPriority and enter legal value in SPC structure */

    if (scsiPriority < NONE || scsiPriority > 0xff)
	return (ERROR);

   pSpc->scsiCtrl.scsiPriority = scsiPriority;

    /* adjust busFreeDelay parity, and enter value in SPC structure */

    if (pSpc->spcDataParity != SPC_DATA_PARITY_VALID)
        {
        busFreeDelayParity = byteParityGet ((UINT8) busFreeDelay);

        if (busFreeDelayParity == pSpc->spcDataParity)
            busFreeDelay ^= 0x1;
        }

    pSpc->busFreeDelay = busFreeDelay;

    /* verify defaultSelTimeOut, convert it from usec to SPC register values,
     * and enter value in SPC structure
     */

    spcSelTimeOutCvt (&pSpc->scsiCtrl, defaultSelTimeOut, &tempSelTimeOut);
    pSpc->defaultSelTimeOut = (UINT16) tempSelTimeOut;

    /* disconnect not supported for now */

    pSpc->scsiCtrl.disconnect = (TBOOL) FALSE;

    spcHwInit (pSpc); 	/* initialize the SPC hardware */

    return (OK);
    }

/*******************************************************************************
*
* spcDevSelect - attempt to select a SCSI physical device
*
* This routine is intended to be called from scsiLib, not directly.
*
* RETURNS: OK if device was successfully selected, otherwise ERROR.
*/

LOCAL STATUS spcDevSelect
    (
    FAST SCSI_PHYS_DEV *pScsiPhysDev,   /* ptr to SCSI physical device info */
    SCSI_TRANSACTION *pScsiXaction      /* ptr to SCSI transaction info */
    )
    {
    FAST SPC *pSpc;		/* ptr to SPC info */
    int spcBusId;               /* SCSI bus ID of the SPC */
    UINT8 identMsg;		/* for construction of the IDENTIFY message */
    STATUS status;		/* placeholder for status */
    int scsiPhase;		/* SCSI bus phase following the select */

    pSpc = (SPC *) pScsiPhysDev->pScsiCtrl;

    if (spcBusIdGet (pSpc, &spcBusId) == ERROR)
        return (ERROR);

    if (spcBusId == pScsiPhysDev->scsiDevBusId)
        return (ERROR);

    pSpc->selectTempReg = (UINT8) ((1 << spcBusId) |
				   (1 << pScsiPhysDev->scsiDevBusId));

    pSpc->pDevToSelect  = pScsiPhysDev;
    pScsiPhysDev->devStatus = SELECT_REQUESTED;
    *pSpc->pPctlReg |= SPC_PCTL_BF_INT_ENBL;

    semTake (&pScsiPhysDev->devSyncSem, WAIT_FOREVER);

    if (pScsiPhysDev->devStatus == SELECT_SUCCESSFUL)
	{
	if (pScsiPhysDev->useIdentify) 	/* send an identify message */
    	    {
	    if ((spcBusPhaseGet (&pSpc->scsiCtrl, 0,
				 &scsiPhase) == ERROR) ||
		(scsiPhase != SCSI_MSG_OUT_PHASE))
		{
		SCSI_DEBUG_MSG ("scsiPhase = %x\n", scsiPhase,
				0, 0, 0, 0, 0);
		return (ERROR);
		}

            identMsg = SCSI_MSG_IDENTIFY |
		       (pSpc->scsiCtrl.disconnect ?
			SCSI_MSG_IDENT_DISCONNECT : 0) |
		       (UINT8) pScsiPhysDev->scsiDevLUN;

	    status = spcProgBytesOut (pScsiPhysDev, &identMsg,
				      sizeof (identMsg), scsiPhase);

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
* spcManBytesOut - output byte(s) to SCSI using manual transfer
*
* RETURNS: OK.
*
* NOMANUAL
*/

STATUS spcManBytesOut
    (
    FAST SPC *pSpc,             /* ptr to SPC info */
    FAST UINT8 *pBuffer,        /* ptr to the output buffer */
    FAST int bufLength,         /* length of the output buffer */
    BOOL atnReset               /* whether ATN should be reset on last byte */
    )
    {
    FAST int ix;

    for (ix = 0; ix < bufLength; ix++)
	{
        while (!(*pSpc->pPsnsReg & SPC_PSNS_REQ))
            ;

        if (atnReset && (ix == bufLength - 1))
            spcCommand (pSpc, SPC_SCMD_RESET_ATN);

        *pSpc->pTempReg = *pBuffer++;
        spcCommand (pSpc, SPC_SCMD_SET_ACK_REQ);

        while (*pSpc->pPsnsReg & SPC_PSNS_REQ)
            ;

        spcCommand (pSpc, SPC_SCMD_RESET_ACK_REQ);
	}

    return (OK);
    }

/*******************************************************************************
*
* spcManBytesIn - input byte(s) from SCSI using manual transfer
*
* RETURNS: OK.
*
* NOMANUAL
*/

STATUS spcManBytesIn
    (
    FAST SPC *pSpc,             /* ptr to SPC info */
    FAST UINT8 *pBuffer,        /* ptr to the input buffer */
    FAST int bufLength          /* length of the input buffer */
    )
    {
    FAST int ix;

    for (ix = 0; ix < bufLength; ix++)
	{
        while (!(*pSpc->pPsnsReg & SPC_PSNS_REQ))
            ;

        spcCommand (pSpc, SPC_SCMD_SET_ACK_REQ);

        while (*pSpc->pPsnsReg & SPC_PSNS_REQ)
            ;

        *pBuffer++ = *pSpc->pTempReg;

	spcCommand (pSpc, SPC_SCMD_RESET_ACK_REQ);
	}

    semGive (&pSpc->scsiCtrl.ctrlSyncSem);
    return (OK);
    }

/*******************************************************************************
*
* spcMsgInAck - de-assert the ACK line to accept message
*
* This routine should be called when an incoming message has been read and
* accepted.  If the incoming message did not imply an impending disconnect,
* give the synchronization semaphore, so that subsequent phase can be
* detected.
*
* RETURNS: OK.
*/

LOCAL STATUS spcMsgInAck
    (
    FAST SCSI_CTRL *pScsiCtrl,  /* ptr to SPC info */
    BOOL expectDisconn          /* whether a disconnect is expected */
    )
    {
    spcCommand ((SPC *) pScsiCtrl, SPC_SCMD_RESET_ACK_REQ);

    if (!expectDisconn)
	semGive (&pScsiCtrl->ctrlSyncSem);

    return (OK);
    }

/*******************************************************************************
*
* spcProgBytesOut - output bytes to SCSI using 'program' transfer
*
* RETURNS: OK if successful, otherwise ERROR.
*
* INTERNAL
* Error handling is inadequate.
*
* NOMANUAL
*/

STATUS spcProgBytesOut
    (
    SCSI_PHYS_DEV *pScsiPhysDev, /* ptr to physical device info */
    FAST UINT8 *pBuffer,         /* ptr to the data buffer */
    FAST int bufLength,          /* number of bytes to be transferred */
    int scsiPhase                /* phase of the transfer */
    )
    {
    FAST SPC *pSpc;		/* ptr to SPC info */

    SCSI_DEBUG_MSG ("spcProgBytesOut: bufLength = %d, scsiPhase = %d\n",
		    bufLength, scsiPhase, 0, 0, 0, 0);

    pSpc = (SPC *) pScsiPhysDev->pScsiCtrl;

    spcIntsDisable (pSpc);

    if (spcXferCountSet (pSpc, bufLength) == ERROR)
	return (ERROR);

    SPC_BUS_PHASE_SET(pSpc, scsiPhase);

    if (scsiPhase == SCSI_COMMAND_PHASE)
        spcTmodSet (pSpc, pScsiPhysDev->syncXfer, 0, 0);

    spcScmdSet (pSpc, SPC_SCMD_XFER, 0, 0, 1, 0);

    while (!(*pSpc->pSstsReg & SPC_SSTS_XFER))
        ;

    while (bufLength >= 8)
	{
        while (!(*pSpc->pSstsReg & SPC_SSTS_DREG_EMPTY))
            ;
        *pSpc->pDregReg = *pBuffer++;
        *pSpc->pDregReg = *pBuffer++;
        *pSpc->pDregReg = *pBuffer++;
        *pSpc->pDregReg = *pBuffer++;
        *pSpc->pDregReg = *pBuffer++;
        *pSpc->pDregReg = *pBuffer++;
        *pSpc->pDregReg = *pBuffer++;
        *pSpc->pDregReg = *pBuffer++;
	bufLength -= 8;
        }

    while (!(*pSpc->pSstsReg & SPC_SSTS_DREG_EMPTY))
        ;

    while (bufLength > 0)
        {
        *pSpc->pDregReg = *pBuffer++;
	bufLength--;
        }

    spcIntsEnable (pSpc);

    return (OK);
    }

/*******************************************************************************
*
* spcProgBytesIn - input bytes from SCSI using 'program' transfer
*
* RETURNS: OK if successful, otherwise ERROR.
*
* INTERNAL
* Error handling is inadequate.
*
* NOMANUAL
*/

STATUS spcProgBytesIn
    (
    SCSI_PHYS_DEV *pScsiPhysDev,        /* ptr to physical device info */
    FAST UINT8 *pBuffer,                /* ptr to the data buffer */
    FAST int bufLength,                 /* number of bytes to be transferred */
    int scsiPhase                       /* phase of the transfer */
    )
    {
    FAST SPC *pSpc;		/* ptr to SPC info */

    SCSI_DEBUG_MSG ("spcProgBytesIn: bufLength = %d, scsiPhase = %d\n",
		    bufLength, scsiPhase, 0, 0, 0, 0);

    pSpc = (SPC *) pScsiPhysDev->pScsiCtrl;

    spcIntsDisable (pSpc);

    if (spcXferCountSet (pSpc, bufLength) == ERROR)
	return (ERROR);

    SPC_BUS_PHASE_SET(pSpc, scsiPhase);
    spcScmdSet (pSpc, SPC_SCMD_XFER, 0, 0, 1, 0);

    while (!(*pSpc->pSstsReg & SPC_SSTS_XFER))
        ;

    while (bufLength >= 8)
	{
        while (!(*pSpc->pSstsReg & SPC_SSTS_DREG_FULL))
            ;
        *pBuffer++ = *pSpc->pDregReg;
        *pBuffer++ = *pSpc->pDregReg;
        *pBuffer++ = *pSpc->pDregReg;
        *pBuffer++ = *pSpc->pDregReg;
        *pBuffer++ = *pSpc->pDregReg;
        *pBuffer++ = *pSpc->pDregReg;
        *pBuffer++ = *pSpc->pDregReg;
        *pBuffer++ = *pSpc->pDregReg;
	bufLength -= 8;
        }

    while (bufLength > 0)
        {
        while (*pSpc->pSstsReg & SPC_SSTS_DREG_EMPTY)
            ;
        *pBuffer++ = *pSpc->pDregReg;
	bufLength--;
        }

    spcIntsEnable (pSpc);

    return (OK);
    }

/******************************************************************************
*
* spcDmaBytesOut - Setup DMA transfer on the device
*
* RETURNS: OK, or ERROR if the count is not set properly.
*/

LOCAL STATUS spcDmaBytesOut
    (
    SCSI_PHYS_DEV *pScsiPhysDev,        /* ptr to phys dev info    */
    UINT8 *pBuffer,                     /* prt to the data buffer  */
    int    bufLength,                   /* nubmer of bytes to xfer */
    int    scsiPhase
    )
    {
    FAST SPC *pSpc;             /* ptr to SPC info */

    SCSI_DEBUG_MSG ("spcDmaBytesOut: bufLength = %d, Address = %08x\n",
                    bufLength, (int) &pBuffer, 0, 0, 0, 0);

    pSpc = (SPC *) pScsiPhysDev->pScsiCtrl;

    spcIntsDisable (pSpc);

    if (spcXferCountSet (pSpc, bufLength) == ERROR)
        return (ERROR);

    SPC_BUS_PHASE_SET(pSpc, scsiPhase);
    if (scsiPhase == SCSI_COMMAND_PHASE)
        spcTmodSet (pSpc, pScsiPhysDev->syncXfer, 0, 0);

    spcScmdSet (pSpc, SPC_SCMD_XFER, 0, 0, 0, 0);

    return ((pSpc->scsiCtrl.scsiDmaBytesOut)
            (pScsiPhysDev, (UINT8 *) pBuffer, bufLength));
    }
 
/*******************************************************************************
*
* spcBytesOut - branch to the appropriate output routine dependent on SCSI phase
*
* RETURNS: OK if specified bytes were output successfully, otherwise ERROR.
*/

LOCAL STATUS spcBytesOut
    (
    FAST SCSI_PHYS_DEV *pScsiPhysDev,   /* ptr to SCSI physical dev info */
    FAST char *pBuffer,                 /* ptr to byte buffer for output */
    int bufLength,                      /* number of bytes to be transferred */
    int scsiPhase                       /* SCSI bus phase of the transfer */
    )
    {
    FAST SPC *pSpc;			/* ptr to SPC info */

    pSpc = (SPC *) pScsiPhysDev->pScsiCtrl;

    if ((scsiPhase != SCSI_DATA_OUT_PHASE) ||
	(pSpc->scsiCtrl.scsiDmaBytesOut == NULL) ||
	(bufLength < 10))
	return (spcProgBytesOut (pScsiPhysDev, (UINT8 *) pBuffer,
				 bufLength, scsiPhase));
    else
	return ((spcDmaBytesOut)
		(pScsiPhysDev, (UINT8 *) pBuffer, bufLength, scsiPhase));
    }

/******************************************************************************
*
* spcDmaBytesIn - Setup DMA transfer on the device
*
* RETURNS: OK, or ERROR if the count is not set properly.
*/

LOCAL STATUS spcDmaBytesIn
    (
    SCSI_PHYS_DEV *pScsiPhysDev,        /* ptr to phys dev info    */
    UINT8 *pBuffer,                     /* prt to the data buffer  */
    int    bufLength,                   /* nubmer of bytes to xfer */
    int    scsiPhase
    )
    {
    FAST SPC *pSpc;             /* ptr to SPC info */

    SCSI_DEBUG_MSG ("spcDmaBytesIn: bufLength = %d, Addr = %08x\n",
                    bufLength, (int) &pBuffer, 0, 0, 0, 0);
 
    pSpc = (SPC *) pScsiPhysDev->pScsiCtrl;

    spcIntsDisable (pSpc);

    if (spcXferCountSet (pSpc, bufLength) == ERROR)
        return (ERROR);

    SPC_BUS_PHASE_SET(pSpc, scsiPhase);
    spcScmdSet (pSpc, SPC_SCMD_XFER, 0, 0, 0, 0);

    return ((pSpc->scsiCtrl.scsiDmaBytesIn)
            (pScsiPhysDev, (UINT8 *) pBuffer, bufLength));
    }

/*******************************************************************************
*
* spcBytesIn - branch to the appropriate input routine dependent on SCSI phase
*
* RETURNS: OK if requested bytes were input successfully, otherwise ERROR.
*/

LOCAL STATUS spcBytesIn
    (
    FAST SCSI_PHYS_DEV *pScsiPhysDev,   /* ptr to SCSI physical dev info */
    FAST char *pBuffer,                 /* ptr to byte buffer for output */
    int bufLength,                      /* number of bytes to be transferred */
    int scsiPhase                       /* SCSI bus phase of the transfer */
    )
    {
    FAST SPC *pSpc;			/* ptr to SPC info */

    pSpc = (SPC *) pScsiPhysDev->pScsiCtrl;

    switch (scsiPhase)
	{
	case SCSI_DATA_IN_PHASE:
	    if ((pSpc->scsiCtrl.scsiDmaBytesIn == NULL) ||
		(bufLength < 10))
		return (spcProgBytesIn (pScsiPhysDev, (UINT8 *) pBuffer,
				        bufLength, scsiPhase));
	    else
		return (spcDmaBytesIn (pScsiPhysDev, (UINT8 *) pBuffer,
				     bufLength, scsiPhase));

	case SCSI_STATUS_PHASE:
	    return (spcProgBytesIn (pScsiPhysDev, (UINT8 *) pBuffer,
				    bufLength, scsiPhase));
	case SCSI_MSG_IN_PHASE:
	    return (spcManBytesIn (pSpc, (UINT8 *) pBuffer, bufLength));
	}

    return (ERROR);
    }

#define CYCLES_PER_USEC  1

/*******************************************************************************
*
* spcBusPhaseGet - return the current SCSI bus phase in *pBusPhase
*
* INTERNAL
* Should do an errnoSet.
*/

LOCAL STATUS spcBusPhaseGet
    (
    FAST SCSI_CTRL *pScsiCtrl,  /* ptr to SCSI controller info */
    int timeOutInUsec,          /* timeout in usec (0 == infinity) */
    int *pBusPhase              /* ptr to returned bus phase */
    )
    {
    FAST int ix = 0;		/* loop index */
    FAST SPC *pSpc;		/* ptr to SPC info */
    FAST int maxCycles;		/* maximum cycles to wait for REQ signal */

    pSpc = (SPC *) pScsiCtrl;

    maxCycles = timeOutInUsec * CYCLES_PER_USEC;

    if (semTake (&pScsiCtrl->ctrlSyncSem, sysClkRateGet() * 5) == ERROR)
	{
	spcScsiBusReset (pSpc);	/* the bus is hung(?), reset it */
	return (ERROR);
	}

    while (!(*pSpc->pPsnsReg & SPC_PSNS_REQ) &&
	   (pScsiCtrl->scsiBusPhase != SCSI_BUS_FREE_PHASE))
	{
	if ((maxCycles > 0) && (ix++ == maxCycles))
	    {
	    if (scsiDebug)
	        logMsg ("spcBusPhaseGet: timed-out waiting for REQ\n",
			0, 0, 0, 0, 0, 0);
	    spcScsiBusReset (pSpc); /* the bus is hung(?), reset it */
	    return (ERROR);
	    }
	}

    if (pScsiCtrl->scsiBusPhase == SCSI_BUS_FREE_PHASE)
        {
        *pBusPhase = SCSI_BUS_FREE_PHASE;
        pScsiCtrl->scsiBusPhase = NONE;
        return (OK);
        }

    *pBusPhase = pScsiCtrl->scsiBusPhase =
	(int) (*pSpc->pPsnsReg & SPC_PSNS_PHASE_MASK);

    SCSI_DEBUG_MSG ("scsiPhase = %x\n", *pBusPhase, 0, 0, 0, 0, 0);

    return (OK);
    }

/*******************************************************************************
*
* byteParityGet - return the parity of a given byte
*
* RETURNS: 1 for odd parity, 0 for even parity.
*/

LOCAL int byteParityGet
    (
    FAST UINT8 byte             /* byte for which to report parity */
    )
    {
    FAST int result = 0;	/* cumulative parity of byte */
    FAST int ix;		/* loop variable */

    for (ix = 0; ix < 8; ix++)
	{
	if (byte & ((UINT8) (1 << ix)))
	    result ^= 0x1;
	}

    return (result);
    }

/*******************************************************************************
*
* spcSelTimeOutCvt - convert a select time-out in microseconds to its
*		     equivalent SPC setting
*
* The conversion formula is given on p. 4-8 of the 1988 Fujitsu SCSI Product
* Profiles manual.  The additive constant 15 has been omitted here since it is
* trivial.  Note that 0 translates to the standard setting of 250 usec.  Also,
* the SPC accepts up to a 16 bit time-out, so a maximum value of 0xffff is
* returned in *pTimeOutSetting.
*/

LOCAL void spcSelTimeOutCvt
    (
    FAST SCSI_CTRL *pScsiCtrl,          /* ptr to SCSI controller info */
    FAST UINT timeOutInUsec,            /* time-out in microsecs */
    FAST UINT *pTimeOutSetting          /* time-out equivalent setting */
    )
    {
    FAST SPC *pSpc = (SPC *) pScsiCtrl;	/* ptr to SPC info */
    FAST UINT tempSelTimeOut;		/* temp. select time-out setting */
    int msbParity;			/* parity of MSB of tempSelTimeOut */
    int lsbParity;			/* parity of LSB of tempSelTimeOut */

    if (timeOutInUsec == 0)
	timeOutInUsec = SCSI_DEF_SELECT_TIMEOUT;

    tempSelTimeOut = (((timeOutInUsec * 1000) >> 9) / pScsiCtrl->clkPeriod) + 1;
    if (tempSelTimeOut > 0xffff)
	tempSelTimeOut = 0xffff;

    if (pSpc->spcDataParity != SPC_DATA_PARITY_VALID)
	{
	msbParity = byteParityGet ((UINT8) MSB(tempSelTimeOut));
	lsbParity = byteParityGet ((UINT8) LSB(tempSelTimeOut));

	if (msbParity == pSpc->spcDataParity)
	    tempSelTimeOut ^= 0x100;

	if (lsbParity == pSpc->spcDataParity)
	    tempSelTimeOut ^= 0x1;
	}

    *pTimeOutSetting = tempSelTimeOut;
    }

/*******************************************************************************
*
* spcBusIdGet - get the current SCSI bus ID of the SPC.
*
* Copies the bus ID to <pBusId>.
*
* RETURNS:
* OK if Bus ID register holds a legal value, otherwise ERROR.
*/

LOCAL STATUS spcBusIdGet
    (
    FAST SPC *pSpc,             /* ptr to SPC info */
    FAST int *pBusId            /* ptr to returned bus ID */
    )
    {
    FAST UINT8 tempReg; 	/* local copy of bus device ID register */
    FAST int ix;		/* count index */

    tempReg = *pSpc->pBdidReg;
    for (ix = SCSI_MIN_BUS_ID; ix <= SCSI_MAX_BUS_ID; ix++)
	{
	if (tempReg == (UINT8) (1 << ix))
	    {
	    *pBusId = ix;
	    return (OK);
	    }
	}
    *pBusId = NONE;
    return (ERROR);
    }

/*******************************************************************************
*
* spcXferCountSet - load the SPC transfer counter with the specified count
*
* RETURNS:
* OK if count is in range 0 - 0xffffff, otherwise ERROR.
*
* NOMANUAL
*/

STATUS spcXferCountSet
    (
    FAST SPC *pSpc,     /* ptr to SPC info */
    FAST int count      /* count value to load */
    )
    {
    if (count < 0 || count > SPC_MAX_XFER_LENGTH)
        return (ERROR);
    else
        {
        *pSpc->pTchReg = (UINT8) ((count >> 16) & 0xff);
        *pSpc->pTcmReg = (UINT8) ((count >>  8) & 0xff);
        *pSpc->pTclReg = (UINT8)  (count  	& 0xff);
        return (OK);
        }
    }

/*******************************************************************************
*
* spcXferCountGet - fetch the SPC transfer count
*
* RETURNS: The value of the transfer counter is returned in *pCount.
*
* NOMANUAL
*/

void spcXferCountGet
    (
    FAST SPC *pSpc,             /* ptr to SPC info */
    FAST int *pCount            /* ptr to returned value */
    )
    {
    FAST UINT tempCount = 0;

    tempCount |=  (UINT) *pSpc->pTclReg;
    tempCount |= ((UINT) *pSpc->pTcmReg) << 8;
    tempCount |= ((UINT) *pSpc->pTchReg) << 16;
    *pCount = tempCount;
    }

/*******************************************************************************
*
* spcIntsEnable - enable the SPC to assert interrupts
*
* This routine sets the INT Enable bit in the SPC Control register.
*/

LOCAL void spcIntsEnable
    (
    FAST SPC *pSpc      /* ptr to SPC info */
    )
    {
    *pSpc->pSctlReg |= SPC_SCTL_INT_ENBL;
    }

/*******************************************************************************
*
* spcIntsDisable - disable the SPC to assert interrupts
*
* This routine resets the INT Enable bit in the SPC Control register.
*/

LOCAL void spcIntsDisable
    (
    FAST SPC *pSpc      /* ptr to SPC info */
    )
    {
    *pSpc->pSctlReg &= ~SPC_SCTL_INT_ENBL;
    }

/*******************************************************************************
*
* spcScmdSet - write to the SPC Command Register
*
* Provides maximum flexibility in controlling the options of the Command Reg.
* The passed parameter for each bit should be set to SPC_RESET_BIT (0),
* SPC_SET_BIT (1), or SPC_PRESERVE_BIT (-1).  In the case of the command code,
* either the desired command code or SPC_PRESERVE_FIELD (0xff) should be passed.
*/

LOCAL void spcScmdSet
    (
    FAST SPC *pSpc,         /* ptr to SPC info */
    FAST UINT8 cmdCode,     /* new command code or SPC_PRESERVE_FIELD */
    int resetOutBit,        /* set, reset, or preserve "reset out" bit */
    int interceptXferBit,   /* set, reset, or preserve "intercept xfer" bit */
    int prgXferBit,         /* set, reset, or preserve "program transfer" bit */
    int termModeBit         /* set, reset, or preserve "termination mode" bit */
    )
    {
    FAST UINT8 tempScmd;    /* temporary copy of SPC Command register */

    tempScmd = *pSpc->pScmdReg;
    if (cmdCode != SPC_PRESERVE_FIELD)
        {
        tempScmd &= ~SPC_SCMD_CMD_MASK;     /* clear existing command code */
                                            /* fill in the desired command */
        tempScmd |= (cmdCode & SPC_SCMD_CMD_MASK);
        }
    switch (resetOutBit)
        {
        case SPC_PRESERVE_BIT:  break;
        case SPC_SET_BIT:       tempScmd |= SPC_SCMD_RESET_OUT; break;
        case SPC_RESET_BIT:     tempScmd &= ~SPC_SCMD_RESET_OUT; break;
        }
    switch (interceptXferBit)
        {
        case SPC_PRESERVE_BIT:  break;
        case SPC_SET_BIT:       tempScmd |= SPC_SCMD_INTERCEPT_XFER; break;
        case SPC_RESET_BIT:     tempScmd &= ~SPC_SCMD_INTERCEPT_XFER; break;
        }
    switch (prgXferBit)
        {
        case SPC_PRESERVE_BIT:  break;
        case SPC_SET_BIT:       tempScmd |= SPC_SCMD_PRG_XFER; break;
        case SPC_RESET_BIT:     tempScmd &= ~SPC_SCMD_PRG_XFER; break;
        }
    switch (termModeBit)
        {
        case SPC_PRESERVE_BIT:  break;
        case SPC_SET_BIT:       tempScmd |= SPC_SCMD_TERM_MODE; break;
        case SPC_RESET_BIT:     tempScmd &= ~SPC_SCMD_TERM_MODE; break;
        }
    *pSpc->pScmdReg = tempScmd;
    }

/*******************************************************************************
*
* spcCommand - write a command code to the SPC Command Register
*
* NOMANUAL
*/

void spcCommand
    (
    FAST SPC *pSpc,     /* ptr to SPC info */
    FAST UINT8 cmdCode      /* new command code */
    )
    {
    FAST UINT8 tempScmd;    /* temporary copy of SPC Command register */

    tempScmd = *pSpc->pScmdReg;
    tempScmd &= ~SPC_SCMD_CMD_MASK;	       /* clear existing command code */
    tempScmd |= (cmdCode & SPC_SCMD_CMD_MASK); /* fill in the desired command */
    *pSpc->pScmdReg = tempScmd;
    }

/*******************************************************************************
*
* spcTmodSet - write to the SPC Transfer Mode Register
*
* Used to set up SPC for synchronous or asynchronous data transfers. maxOffset
* and minPeriod parameters may be specified as legal values:
*           maxOffset:          1 - 8       (see Fujitsu documentation)
*           minPerMinusOne:     0 - 3       (see Fujitsu documentation)
* or as SPC_PRESERVE_FIELD (0xff).  They will be ignored if syncXfer is FALSE,
* since they are only relevant to synchronous transfer mode.  Illegal values
* will merely be masked into legality.
*/

LOCAL void spcTmodSet
    (
    FAST SPC *pSpc,     /* ptr to SPC info */
    BOOL syncXfer,              /* TRUE for synch. xfer, FALSE for asynch. */
    FAST int maxOffset,         /* maximum transfer offset */
    FAST int minPerMinusOne     /* minimum transfer period minus one */
    )
    {
    FAST UINT8 tempTmod;    /* temporary copy of SPC Transfer Mode register */

    tempTmod = *pSpc->pTmodReg;
    if (syncXfer == FALSE)
        tempTmod &= ~SPC_TMOD_SYNC_XFER;
    else
        {
        tempTmod |= SPC_TMOD_SYNC_XFER;
        if (maxOffset != SPC_PRESERVE_FIELD)
            {
            tempTmod &= ~SPC_TMOD_MAX_OFFSET_MASK;  /* clear existing offset */
                                                    /* set the desired offset */
            tempTmod |= ((maxOffset << 4) & SPC_TMOD_MAX_OFFSET_MASK);
            }
        if (minPerMinusOne != SPC_PRESERVE_FIELD)
            {
            tempTmod &= ~SPC_TMOD_MIN_PERIOD_MASK;  /* clear existing period */
                                                    /* set the desired period */
            tempTmod |= ((minPerMinusOne << 2) & SPC_TMOD_MIN_PERIOD_MASK);
            }
        }
    *pSpc->pTmodReg = tempTmod;
    }

/*******************************************************************************
*
* spcPctlSet - write to the SPC Phase Control Register
*
* Provides flexibility in controlling the options of the Phase Control Reg.
* The passed parameter for the busFreeIntEnbl bit should be set to
* SPC_RESET_BIT (0), SPC_SET_BIT (1), or SPC_PRESERVE_BIT (-1).  In the case
* of the transfer phase, either the desired phase or SPC_PRESERVE_FIELD
* (0xff) should be passed.
*/

LOCAL void spcPctlSet
    (
    FAST SPC *pSpc,         /* ptr to SPC info */
    int busFreeIntEnblBit,  /* set, reset, or preserve "bus free int." bit */
    FAST UINT8 xferPhase    /* new transfer phase or SPC_PRESERVE_FIELD */
    )
    {
    FAST UINT8 tempPctl;    /* temporary copy of SPC Pctl register */

    tempPctl = *pSpc->pPctlReg;
    switch (busFreeIntEnblBit)
        {
        case SPC_PRESERVE_BIT:  break;
        case SPC_SET_BIT:       tempPctl |= SPC_PCTL_BF_INT_ENBL; break;
        case SPC_RESET_BIT:     tempPctl &= ~SPC_PCTL_BF_INT_ENBL; break;
        }
    if (xferPhase != SPC_PRESERVE_FIELD)
        {
        tempPctl &= ~SPC_PCTL_PHASE_MASK;   /* clear existing xfer phase */
                                            /* fill in the new phase */
        tempPctl |= (xferPhase & SPC_PCTL_PHASE_MASK);
        }
    *pSpc->pPctlReg = tempPctl;
    }

/*******************************************************************************
*
* spcScsiBusReset - assert the RST line on the SCSI bus
*
* Asserts the RST line on SCSI which should cause all connected devices to
* return to a quiescent state.
* JCC Timing delays should be made prettier and more precise.
*/

LOCAL void spcScsiBusReset
    (
    FAST SPC *pSpc      /* ptr to SPC info */
    )
    {
    int delayVar;	/* delay loop counter */

    spcScmdSet (pSpc,
                SPC_SCMD_BUS_RELEASE,
                SPC_SET_BIT,
                SPC_PRESERVE_BIT,
                SPC_PRESERVE_BIT,
                SPC_PRESERVE_BIT);

    for (delayVar = 0; delayVar < 0x1000; delayVar++)
	;

    spcScmdSet (pSpc,
                SPC_PRESERVE_FIELD,
                SPC_RESET_BIT,
                SPC_PRESERVE_BIT,
                SPC_PRESERVE_BIT,
                SPC_PRESERVE_BIT);

    for (delayVar = 0; delayVar < 0x2000; delayVar++)
	;
    }

LOCAL BOOL spcHardErrPrint = FALSE;
LOCAL int hardErrors = 0;

/*******************************************************************************
*
* spcIntr - interrupt service routine for the SPC
*
* NOMANUAL
*/

void spcIntr
    (
    SPC *pSpc          /* ptr to SPC info */
    )
    {
    FAST SCSI_PHYS_DEV *pDevToSelect;
    FAST UINT8 tempInts;
    FAST UINT8 tempSerr;

    tempInts = *pSpc->pIntsReg;
    tempSerr = *pSpc->pSerrReg;

    if (scsiIntsDebug)
        logMsg ("*** SPC INTERRUPT: Ints %08x, Serr %08x", tempInts, tempSerr,
		0, 0, 0, 0);

    if (tempInts & SPC_INTS_SELECTED)
        {
	/* this is currently not supported */
        *pSpc->pIntsReg = SPC_INTS_SELECTED;
	if (scsiIntsDebug)
            logMsg ("Selected ", 0, 0, 0, 0, 0, 0);
        }

    if (tempInts & SPC_INTS_RESELECTED)
        {
        *pSpc->pIntsReg = SPC_INTS_RESELECTED;
	if (scsiIntsDebug)
            logMsg ("Reselected ", 0, 0, 0, 0, 0, 0);
    	/* if a selection attempt has been made, report lost arbitration */

	if (((pDevToSelect = pSpc->pDevToSelect) != (SCSI_PHYS_DEV *) NULL) &&
	    (pDevToSelect->devStatus == SELECT_IN_PROGRESS))
	    {
	    pDevToSelect->devStatus = SELECT_LOST_ARBIT;
	    pSpc->pDevToSelect = (SCSI_PHYS_DEV *) NULL;
	    semGive (&pDevToSelect->devSyncSem);
	    }
        }

    if (tempInts & SPC_INTS_COM_COMPLETE)
        {
        *pSpc->pIntsReg = SPC_INTS_COM_COMPLETE;
	if (scsiIntsDebug)
            logMsg ("commComplete ", 0, 0, 0, 0, 0, 0);

    	/* if a selection attempt has been made, report success */

	if (((pDevToSelect = pSpc->pDevToSelect) != (SCSI_PHYS_DEV *) NULL) &&
	    (pDevToSelect->devStatus == SELECT_IN_PROGRESS))
	    {
	    pDevToSelect->devStatus = SELECT_SUCCESSFUL;
	    pSpc->pDevToSelect = (SCSI_PHYS_DEV *) NULL;
	    semGive (&pDevToSelect->devSyncSem);
	    }

	semGive (&pSpc->scsiCtrl.ctrlSyncSem);
        }

    if (tempInts & SPC_INTS_TIMEOUT)
        {
        *pSpc->pIntsReg = SPC_INTS_TIMEOUT;
	if (scsiIntsDebug)
            logMsg ("Timeout ", 0, 0, 0, 0, 0, 0);
    	/* if a selection attempt has been made, report the time-out */

	if (((pDevToSelect = pSpc->pDevToSelect) != (SCSI_PHYS_DEV *) NULL) &&
	    (pDevToSelect->devStatus == SELECT_IN_PROGRESS))
	    {
	    pDevToSelect->devStatus = SELECT_TIMEOUT;
	    pSpc->pDevToSelect = (SCSI_PHYS_DEV *) NULL;
	    semGive (&pDevToSelect->devSyncSem);
	    }

        }

    if (tempInts & SPC_INTS_DISCONNECT)
        {
	spcPctlSet (pSpc, SPC_RESET_BIT, SPC_PCTL_SELECT);
        *pSpc->pIntsReg = SPC_INTS_DISCONNECT;
	if (scsiIntsDebug)
            logMsg ("Disconnect ", 0, 0, 0, 0, 0, 0);
    	/* if a device is to be selected, do so now */

	if (((pDevToSelect = pSpc->pDevToSelect) != (SCSI_PHYS_DEV *) NULL) &&
    	    (pDevToSelect->devStatus == SELECT_REQUESTED))
	    {
	    if (pDevToSelect->useIdentify)
		{
	        spcCommand (pSpc, SPC_SCMD_SET_ATN);
		}
	    *pSpc->pTempReg = pSpc->selectTempReg;
	    (void) spcXferCountSet (pSpc,
				    (int) (pDevToSelect->selTimeOut << 8) |
				   	  (pSpc->busFreeDelay));
	    spcCommand (pSpc, SPC_SCMD_SELECT);
	    pDevToSelect->devStatus = SELECT_IN_PROGRESS;
	    }
	else
	    {
	    pSpc->scsiCtrl.scsiBusPhase = SCSI_BUS_FREE_PHASE;
	    semGive (&pSpc->scsiCtrl.ctrlSyncSem);
	    }
        }

    if (tempInts & SPC_INTS_SERVICE_REQ)
        {
        *pSpc->pIntsReg = SPC_INTS_SERVICE_REQ;
	if (scsiIntsDebug)
	    {
            logMsg ("serviceReq (sstsReg = 0x%02x) ", *pSpc->pSstsReg,
		0, 0, 0, 0, 0);
	    }
        }

    if (tempInts & SPC_INTS_HARD_ERROR)
        {
        *pSpc->pIntsReg = SPC_INTS_HARD_ERROR;
	if (scsiIntsDebug)
	    {
            hardErrors++;
            if (spcHardErrPrint)
                {
                logMsg ("hardError (serrReg = 0x%02x) ", tempSerr,
			0, 0, 0, 0, 0);
                }
	    }
        }

    /* the following condition should not attain under allowable SCSI
     * configurations (i.e., vxWorks target is the sole initiator)
     */

    if (tempInts & SPC_INTS_RESET_COND)
        {
        *pSpc->pIntsReg = SPC_INTS_RESET_COND;
	if (scsiIntsDebug)
            logMsg ("Reset (unexpected, multiple intiators not supported)",
		0, 0, 0, 0, 0, 0);
        }

    if (scsiIntsDebug)
	logMsg ("\n", 0, 0, 0, 0, 0, 0);
    }

/*******************************************************************************
*
* spcHwInit - initialize the SPC chip to a known state
*
* This routine puts the SPC into a known quiescent state and issues a reset
* to the SCSI Bus if any signals are active, thus putting target devices in
* some presumably known state.  Currently the initial state is not configurable
* and does not enable reselection.
*
* INTERNAL
* Needs to handle parity enable
*/

LOCAL void spcHwInit
    (
    SPC *pSpc          /* ptr to an SPC info structure */
    )
    {
    UINT8 tempSctl;

    tempSctl = SPC_SCTL_RESET_AND_DSBL | SPC_SCTL_INT_ENBL;
    if (pSpc->scsiCtrl.disconnect)
	tempSctl |= (SPC_SCTL_ARBIT_ENBL | SPC_SCTL_RESELECT_ENBL);
    *pSpc->pSctlReg = tempSctl;

    /* set SCSI bus ID of controller */

    *pSpc->pBdidReg = pSpc->scsiCtrl.scsiCtrlBusId;

    *pSpc->pScmdReg = (UINT8) 0;	/* set cmd to 'bus release' */
    *pSpc->pTmodReg = (UINT8) 0;	/* asynch data transfer initially */
    *pSpc->pPctlReg = (UINT8) 0;	/* bus free int disabled, no phase */

    (void) spcXferCountSet (pSpc, 0x00);	/* clear xfer counter */

    *pSpc->pSctlReg &= ~SPC_SCTL_RESET_AND_DSBL;

    spcScsiBusReset (pSpc);
    }

/*******************************************************************************
*
* mb87030Show - display the values of all readable MB87030 SPC registers
*
* This routine displays the state of the SPC registers in a user-friendly 
* manner.  It is useful primarily for debugging.
*
* EXAMPLE:
* .CS
*     -> mb87030Show
*     SCSI Bus ID: 7    
*     SCTL (0x01): intsEnbl  
*     SCMD (0x00): busRlease 
*     TMOD (0x00): asyncMode 
*     INTS (0x00):            
*     PSNS (0x00): req0 ack0 atn0 sel0 bsy0 msg0 c_d0 i_o0 
*     SSTS (0x05): noConIdle xferCnt=0 dregEmpty 
*     SERR (0x00): noParErr  
*     PCTL (0x00): bfIntDsbl phDataOut 
*     MBC  (0x00): 0         
*     XFER COUNT : 0x000000 =         0
* .CE
*
* RETURNS: OK, or ERROR if <pScsiCtrl> and <pSysScsiCtrl> are both NULL.
*/

STATUS mb87030Show
    (
    FAST SCSI_CTRL *pScsiCtrl		/* ptr to SCSI controller info */
    )
    {
    int busId;
    FAST UINT8 tempReg;
    FAST SPC *pSpc;	/* ptr to SPC info */
    int xferCount;

    if (pScsiCtrl == NULL)
	{
	if (pSysScsiCtrl == NULL)
	    {
	    printErr ("No SCSI controller specified.\n");
	    return (ERROR);
	    }

	pScsiCtrl = pSysScsiCtrl;
	}

    pSpc = (MB_87030_SCSI_CTRL *) pScsiCtrl;

    /* Display current SCSI Bus ID */

    if (spcBusIdGet (pSpc, &busId) == OK)
        printf ("SCSI Bus ID: %-5d\n", busId);
    else
        printf ("SCSI Bus ID: ERROR\n");

    /* Display SPC Control Register */
    tempReg = *pSpc->pSctlReg;
    printf ("SCTL (0x%02x): ", (int) tempReg);
    printf ("%s", (tempReg & SPC_SCTL_RESET_AND_DSBL) ? "spcDisabl " : "");
    printf ("%s", (tempReg & SPC_SCTL_CTRL_RESET)     ? "ctrlReset " : "");
    printf ("%s", (tempReg & SPC_SCTL_DIAG_MODE)      ? "diagMode  " : "");
    printf ("%s", (tempReg & SPC_SCTL_ARBIT_ENBL)     ? "arbitEnbl " : "");
    printf ("%s", (tempReg & SPC_SCTL_PARITY_ENBL)    ? "paritEnbl " : "");
    printf ("%s", (tempReg & SPC_SCTL_SELECT_ENBL)    ? "selecEnbl " : "");
    printf ("%s", (tempReg & SPC_SCTL_RESELECT_ENBL)  ? "reselEnbl " : "");
    printf ("%s", (tempReg & SPC_SCTL_INT_ENBL)       ? "intsEnbl  " : "");
    printf ("\n");

    /* Display Command Register */
    tempReg = *pSpc->pScmdReg;
    printf ("SCMD (0x%02x): ", (int) tempReg);
    switch (tempReg & SPC_SCMD_CMD_MASK)
        {
        case SPC_SCMD_BUS_RELEASE:   printf ("busRlease "); break;
        case SPC_SCMD_SELECT:        printf ("select    "); break;
        case SPC_SCMD_RESET_ATN:     printf ("resetAtn  "); break;
        case SPC_SCMD_SET_ATN:       printf ("setAtn    "); break;
        case SPC_SCMD_XFER:          printf ("transfer  "); break;
        case SPC_SCMD_XFER_PAUSE:    printf ("xferPause "); break;
        case SPC_SCMD_RESET_ACK_REQ: printf ("resAckReq "); break;
        case SPC_SCMD_SET_ACK_REQ:   printf ("setAckReq "); break;
        }
    printf ("%s", (tempReg & SPC_SCMD_RESET_OUT)       ? "resetOut  " : "");
    printf ("%s", (tempReg & SPC_SCMD_INTERCEPT_XFER)  ? "intcptXfr " : "");
    printf ("%s", (tempReg & SPC_SCMD_PRG_XFER)        ? "progXfer  " : "");
    printf ("%s", (tempReg & SPC_SCMD_TERM_MODE)       ? "termMode  " : "");
    printf ("\n");

    /* Display Transfer Mode Register */
    tempReg = *pSpc->pTmodReg;
    printf ("TMOD (0x%02x): ", (int) tempReg);
    printf ("%s", (tempReg & SPC_TMOD_SYNC_XFER) ? "syncMode  " :
                                                   "asyncMode ");
    /* JCC Should display offset and period params also
    */
    printf ("\n");

    /* Display Interrupt Sense Register */
    tempReg = *pSpc->pIntsReg;
    printf ("INTS (0x%02x): ", (int) tempReg);
    printf ("%s", (tempReg & SPC_INTS_SELECTED)     ? "selected  " : "");
    printf ("%s", (tempReg & SPC_INTS_RESELECTED)   ? "reselect  " : "");
    printf ("%s", (tempReg & SPC_INTS_DISCONNECT)   ? "disconnct " : "");
    printf ("%s", (tempReg & SPC_INTS_COM_COMPLETE) ? "comComplt " : "");
    printf ("%s", (tempReg & SPC_INTS_SERVICE_REQ)  ? "servReq   " : "");
    printf ("%s", (tempReg & SPC_INTS_TIMEOUT)      ? "timeout   " : "");
    printf ("%s", (tempReg & SPC_INTS_HARD_ERROR)   ? "hardError " : "");
    printf ("%s", (tempReg & SPC_INTS_RESET_COND)   ? "resetCond " : "");
    printf ("\n");

    /* Display Phase Sense Register */
    tempReg = *pSpc->pPsnsReg;
    printf ("PSNS (0x%02x): ", (int) tempReg);
    printf ("%s", (tempReg & SPC_PSNS_REQ) ? "REQ1 " : "req0 ");
    printf ("%s", (tempReg & SPC_PSNS_ACK) ? "ACK1 " : "ack0 ");
    printf ("%s", (tempReg & SPC_PSNS_ATN) ? "ATN1 " : "atn0 ");
    printf ("%s", (tempReg & SPC_PSNS_SEL) ? "SEL1 " : "sel0 ");
    printf ("%s", (tempReg & SPC_PSNS_BSY) ? "BSY1 " : "bsy0 ");
    printf ("%s", (tempReg & SPC_PSNS_MSG) ? "MSG1 " : "msg0 ");
    printf ("%s", (tempReg & SPC_PSNS_C_D) ? "C_D1 " : "c_d0 ");
    printf ("%s", (tempReg & SPC_PSNS_I_O) ? "I_O1 " : "i_o0 ");
    printf ("\n");

    /* Display SPC Status Register */
    tempReg = *pSpc->pSstsReg;
    printf ("SSTS (0x%02x): ", (int) tempReg);
    switch (tempReg & SPC_SSTS_OPER_STAT_MASK)
        {
        case SPC_SSTS_NO_CONNECT_IDLE:  printf ("noConIdle "); break;
        case SPC_SSTS_SELECT_WAIT:      printf ("waitSelec "); break;
        case SPC_SSTS_TARGET_MANUAL:    printf ("trgManual "); break;
        case SPC_SSTS_RESELECT_EXEC:    printf ("reselExec "); break;
        case SPC_SSTS_TARGET_XFER:      printf ("targXfer  "); break;
        case SPC_SSTS_INITIATOR_MANUAL: printf ("iniManual "); break;
        case SPC_SSTS_INITIATOR_WAIT:   printf ("iniWait   "); break;
        case SPC_SSTS_SELECT_EXEC:      printf ("selecExec "); break;
        case SPC_SSTS_INITIATOR_XFER:   printf ("iniXfer   "); break;
        default:                        printf ("operUndef "); break;
        }
    printf ("%s", (tempReg & SPC_SSTS_SCSI_RESET) ? "scsiReset " : "");
    printf ("%s", (tempReg & SPC_SSTS_TC_0)       ? "xferCnt=0 " : "");
    switch (tempReg & SPC_SSTS_DREG_STAT_MASK)
        {
        case SPC_SSTS_DREG_EMPTY:   printf ("dregEmpty "); break;
        case SPC_SSTS_DREG_PARTIAL: printf ("dregPartl "); break;
        case SPC_SSTS_DREG_FULL:    printf ("dregFull  "); break;
        default:                    printf ("dregUndef "); break;
        }
    printf ("\n");

    /* Display SPC Error Status Register */
    tempReg = *pSpc->pSerrReg;
    printf ("SERR (0x%02x): ", (int) tempReg);
    switch (tempReg & SPC_SERR_PAR_ERROR_MASK)
        {
        case SPC_SERR_NO_PAR_ERROR:  printf ("noParErr  "); break;
        case SPC_SERR_PAR_ERROR_OUT: printf ("outParErr "); break;
        case SPC_SERR_PAR_ERROR_IN:  printf ("inpParErr "); break;
        default:                      printf ("pErrUndef "); break;
        }
    printf ("%s", (tempReg & SPC_SERR_TC_PARITY)    ? "tcParErr  " : "");
    printf ("%s", (tempReg & SPC_SERR_PHASE_ERROR)  ? "phaseErr  " : "");
    printf ("%s", (tempReg & SPC_SERR_SHORT_PERIOD) ? "shortPeri " : "");
    printf ("%s", (tempReg & SPC_SERR_OFFSET_ERROR) ? "offsetErr " : "");
    printf ("\n");

    /* Display Phase Control Register */
    tempReg = *pSpc->pPctlReg;
    printf ("PCTL (0x%02x): ", (int) tempReg);
    printf ("%s", (tempReg & SPC_PCTL_BF_INT_ENBL) ? "bfIntEnbl " :
                                                     "bfIntDsbl ");
    switch (tempReg & SPC_PCTL_PHASE_MASK)
        {
        case SPC_PCTL_DATA_OUT:  printf ("phDataOut "); break;
        case SPC_PCTL_DATA_IN:   printf ("phDataIn  "); break;
        case SPC_PCTL_COMMAND:   printf ("phCommand "); break;
        case SPC_PCTL_STATUS:    printf ("phStatus  "); break;
        case SPC_PCTL_UNUSED_0:  printf ("phUnused0 "); break;
        case SPC_PCTL_UNUSED_1:  printf ("phUnused1 "); break;
        case SPC_PCTL_MESS_OUT:  printf ("phMessOut "); break;
        case SPC_PCTL_MESS_IN:   printf ("phMessIn  "); break;
        }
    printf ("\n");

    /* Display Modified Byte Counter */
    tempReg = *pSpc->pMbcReg;
    printf ("MBC  (0x%02x): ", (int) tempReg);
    printf ("%-10d", (int) tempReg);
    printf ("\n");

    /* Display Transfer Counter */
    spcXferCountGet (pSpc, &xferCount);
    printf ("XFER COUNT : ");
    printf ("0x%06x =%10d", xferCount, xferCount);
    printf ("\n");

    return (OK);
    }
