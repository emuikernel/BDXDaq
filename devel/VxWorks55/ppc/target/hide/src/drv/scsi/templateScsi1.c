/* templateScsi1.c - template SCSI-1 driver */

/* Copyright 1989-1997 Wind River Systems, Inc. */

/*
modification history
--------------------
TODO -	Remove the template modification history and begin a new history
	starting with version 01a and growing the history upward with
	each revision.

01d,24sep97,dat  changed args to TEMPLATE_READ macro
01c,20aug97,dat  code review comments from Dinesh
01b,11aug97,dat  fixed compilation bugs
01a,07apr97,dat  written from ncr5390Lib1.c, ver 01j
*/

/*
DESCRIPTION

TODO - Describe the entire chip completely.  If this device is a part of
a larger ASIC, describe the complete device in a single paragraph.

TODO - Describe the device completely.  Describe all operating modes and
capabilties, whether used or not.

TODO - Describe the modes and limitations of this driver.  Describe how
this driver interacts with the chip and any limitations imposed by the
software.  If this driver interacts with other drivers, describe that
interaction.

USER-CALLABLE ROUTINES
The primary user routines for this driver library are templateCtrlCreate()
and templateCtrlInit().
TemplateCtrlCreate() is used to create a device driver structure that will
represent and control the physical controller device.  
TemplateCtrlInit() is used to setup the device resetting the chip and the
SCSI bus.  See the synopsis for arguments.

DRIVER CUSTOMIZATION

TODO - Describe all macros and constants that the user can change that
will effect the behaviour of this driver.

This template driver presumes that the device is memory mapped, and that the
<regOffset> argument to templateCtrlCreate() indicates the interval between
registers, usually 1, 2 or 4 bytes.  If the base address for the device is
0x1000 and the regOffset is 4, then the CPU address of register number 3
is (0x1000 + (4 * 3)) = 0x100C.

The macro TEMPLATE_REG_READ(pDev, reg,result) is used to read a
byte from a specified register.  The three arguments are the device structure
pointer, the register id (element of the dev structure), 
and the variable (not a pointer) to receive the register contents.
(If the third argument were a pointer, then it would affect compiler
optimization and could lead to inefficient code generation).

The macro TEMPLATE_REG_WRITE(pDev, reg,data) is used to write data to the
specified register address.  These macros presume memory mapped i/o by
default.  Both macros can be redefined to tailor the driver to some other
i/o model.

The macro SCSI_DEBUG_MSG(format,...) can be used to insert debugging
messages anywhere in the code.  The debug messages will appear only when
the global variable scsiDebug is set to a non-zero value.

INCLUDE FILES
templateScsi1.h

SEE ALSO: scsiLib,
.pG "I/O System"
*/

#include "vxWorks.h"

#include "memLib.h"
#include "errnoLib.h"
#include "logLib.h"
#include "scsiLib.h"
#include "stdlib.h"
#include "stdio.h"
#include "taskLib.h"
#include "sysLib.h"

/* defines */

#define TEMPLATE_MAX_XFER_LENGTH ((UINT) (0xffff)) /* max data xfer length */
#define TEMPLATE_MAX_BUS_ID	(0x7)

/* Dummy command codes */

#define TEMPLATE_ATN_SELECT	(0x1)
#define TEMPLATE_STOP_SELECT	(0x2)
#define TEMPLATE_SELECT		(0x3)

#ifndef TEMPLATE_REG_READ
#   define TEMPLATE_REG_READ(pDev, reg,result) \
	((result) = *pDev->reg)
#endif  /*TEMPLATE_REG_READ*/

#ifndef TEMPLATE_REG_WRITE
#   define TEMPLATE_REG_WRITE(pDev, reg, data) \
	(*pDev->reg = data)
#endif  /*TEMPLATE_REG_WRITE*/

typedef struct { /* TEMPLATE_DEV */
    SCSI_CTRL scsiCtrl;         /* generic SCSI controller info */
    SEM_ID pMutexData;          /* use to protect global siop data */
    SCSI_PHYS_DEV *pDevToSelect;/* device to select at intr. level or NULL */
    } TEMPLATE_DEV;

/* globals */

IMPORT BOOL	scsiDebug;
IMPORT BOOL	scsiIntsDebug;

/* forward declarations */

      STATUS	templateCtrlInit (TEMPLATE_DEV *, int , UINT, int);
      void	templateIntr (TEMPLATE_DEV *);

LOCAL void	templateCommand (TEMPLATE_DEV *, int);
LOCAL STATUS	templateDevSelect (SCSI_PHYS_DEV *, SCSI_TRANSACTION *);
LOCAL STATUS	templateMsgInAck (SCSI_CTRL *, BOOL);
LOCAL STATUS	templateBytesOut (SCSI_PHYS_DEV *, char *, int, int);
LOCAL STATUS	templateBytesIn (SCSI_PHYS_DEV *, char *, int, int);
LOCAL STATUS	templateBusPhaseGet (SCSI_CTRL *, int, int *);
LOCAL void	templateSelTimeOutCvt (SCSI_CTRL *, UINT , UINT *);
LOCAL STATUS	templateBusIdGet (TEMPLATE_DEV *, int *);
LOCAL void	templateScsiBusReset (TEMPLATE_DEV *);
LOCAL void	templateHwInit (TEMPLATE_DEV *);

/*
 * TODO - remove or replace these macros, they are just pseudo code to allow
 * this template driver to compile without warnings.
 */

#define DEVICE_IS_DISCONNECTED	0
#define NOT_EXPECTED_STATE	0
#define DEVICE_IS_NOT_INTERRUPTING 0
#define DMA_TERMINAL_COUNT_REACHED 0
#define DEVICE_FUNC_COMPLETE 0

/*******************************************************************************
*
* templateCtrlCreate - create a TEMPLATE_DEV device
*
* This routine creates a data structure that must exist before the
* TEMPLATE_DEV chip can be used.  This routine must be called exactly once
* for a specified TEMPLATE_DEV, and must be the first routine called, since
* it calloc's a structure needed by all other routines in the library.
*
* TODO - The input arguments are device specific.  The argument list must
* be updated.
*
* The device specific input parameters are as follows:
* .iP <baseAdrs> 4
* the address at which the CPU would access the lowest 
* register of the TEMPLATE_DEV.
* .iP <regOffset>
* the address offset (bytes) to access consecutive registers.
* (This must be a power of 2, for example, 1, 2, 4, etc.)
* .iP <clkPeriod>
* the period, in nanoseconds, of the signal to the TEMPLATE_DEV clock input (used only
* for select command timeouts).
* .iP "<templateDmaBytesIn> and <templateDmaBytesOut>"
* board-specific parameters to handle DMA input and output.
* If these are NULL (0), TEMPLATE_DEV program mode is used.
* DMA is used only during SCSI data in/out phases.
* The interface to these DMA routines must be of the form:
* .CS
* STATUS templateDmaBytes{In, Out}
*     (
*     TEMPLATE_DEV*  pScsiPhysDev,  /@ ptr to phys dev info    @/
*     UINT8*         pBuffer,       /@ ptr to the data buffer  @/
*     int            bufLength      /@ number of bytes to xfer @/
*     )
* .CE
* .LP
*
* After creating a device, the user will usually then initialize the
* device by calling templateCtrlInit().
*
* RETURNS: A pointer to an TEMPLATE_DEV structure,
* or NULL if memory is insufficient or the parameters are invalid.
*/

TEMPLATE_DEV * templateCtrlCreate
    (
    /* TODO - this argument list is device specific, change as needed */
    UINT8 *baseAdrs,		/* base address of TEMPLATE_DEV */
    int regOffset,		/* addr offset between consecutive regs. */
    UINT clkPeriod,		/* period of controller clock (nsec) */
    FUNCPTR templateDmaBytesIn, /* SCSI DMA input function */
    FUNCPTR templateDmaBytesOut /* SCSI DMA output function */
    )
    {
    TEMPLATE_DEV *pDev;		/* ptr to TEMPLATE_DEV info */

    /* TODO - verify parameters, return NULL if necessary */

    /* calloc the controller info structure; return NULL if unable */

    pDev = (TEMPLATE_DEV *) calloc (1, sizeof (TEMPLATE_DEV));

    if (pDev == (TEMPLATE_DEV *) NULL)
        return ((TEMPLATE_DEV *) NULL);

    /* fill in generic SCSI info for this controller */

    scsiCtrlInit (&pDev->scsiCtrl);

    /* fill in remainder of scsiCtrl structure */

    pDev->scsiCtrl.clkPeriod		= clkPeriod;
    pDev->scsiCtrl.maxBytesPerXfer	= TEMPLATE_MAX_XFER_LENGTH;
    pDev->scsiCtrl.scsiBusReset		= (VOIDFUNCPTR) templateScsiBusReset;
    pDev->scsiCtrl.scsiTransact		= (FUNCPTR) scsiTransact;
    pDev->scsiCtrl.scsiDevSelect	= (FUNCPTR) templateDevSelect;
    pDev->scsiCtrl.scsiBytesIn		= (FUNCPTR) templateBytesIn;
    pDev->scsiCtrl.scsiBytesOut		= (FUNCPTR) templateBytesOut;
    pDev->scsiCtrl.scsiDmaBytesIn	= (FUNCPTR) templateDmaBytesIn;
    pDev->scsiCtrl.scsiDmaBytesOut	= (FUNCPTR) templateDmaBytesOut;
    pDev->scsiCtrl.scsiBusPhaseGet	= (FUNCPTR) templateBusPhaseGet;
    pDev->scsiCtrl.scsiMsgInAck		= (FUNCPTR) templateMsgInAck;
    pDev->scsiCtrl.scsiSelTimeOutCvt	= (VOIDFUNCPTR) templateSelTimeOutCvt;

    /* initialize other template_dev fields */

    pDev->pDevToSelect = (SCSI_PHYS_DEV *) NULL;

    /* TODO - fill in device specific data for this controller */

    return (pDev);
    }

/*******************************************************************************
*
* templateCtrlInit - initialize a TEMPLATE_DEV
*
* This routine initializes an TEMPLATE_DEV structure created by 
* templateCtrlCreate(). It must be called before the TEMPLATE_DEV is used.
* This routine can be called more than once; however, it should be called
* only while there is no activity on the SCSI interface, since the specified
* configuration is written to the TEMPLATE_DEV.
*
* Before returning, this routine pulses RST (reset) on the SCSI bus, thus
* resetting all attached devices.
*
* The input parameters are as follows:
* .iP <pDev> 4
* a pointer to an TEMPLATE_DEV structure created with templateCtrlCreate().
* .iP <scsiCtrlBusId>
* the SCSI bus ID of the TEMPLATE_DEV, in the range 0 - 7.  The ID is
* somewhat arbitrary; the value 7, or highest priority, is conventional.
* .iP <defaultSelTimeOut>
* the timeout, in microseconds, for selecting a SCSI device attached to this
* controller.  This value is used as a default if no timeout is specified in 
* scsiPhysDevCreate()).  The recommended value zero (0)
* specifies SCSI_DEF_SELECT_TIMEOUT (250 millisec).  The maximum timeout
* possible is approximately 3 seconds.  Values exceeding this revert to the
* maximum.  For more information about chip timeouts, see the device manual.
* .iP <scsiPriority>
* the priority to which a task is set when performing a SCSI transaction.
* Valid priorities are 0 to 255.  Alternatively, the value -1 specifies 
* that the priority should not be altered during SCSI transactions.
*
* RETURNS: OK, or ERROR if a parameter is out of range.
*
* SEE ALSO: scsiPhysDevCreate(),
*/

STATUS templateCtrlInit
    (
    TEMPLATE_DEV* pDev,		/* ptr to TEMPLATE_DEV struct */
    int scsiCtrlBusId,		/* SCSI bus ID of this TEMPLATE_DEV */
    UINT defaultSelTimeOut,	/* default dev sel timeout (microsec) */
    int scsiPriority		/* priority of task doing SCSI I/O */
    )
    {

    /* verify scsiCtrlBusId and enter legal value in TEMPLATE_DEV structure */

    if (scsiCtrlBusId < SCSI_MIN_BUS_ID || scsiCtrlBusId > TEMPLATE_MAX_BUS_ID)
	return (ERROR);
    pDev->scsiCtrl.scsiCtrlBusId = (UINT8) scsiCtrlBusId;

    /* verify scsiPriority and enter legal value in TEMPLATE_DEV structure */

    if (scsiPriority < NONE || scsiPriority > 0xff)
	return (ERROR);
    pDev->scsiCtrl.scsiPriority = scsiPriority;

    /* TODO - if needed, issue NO-OP (required by some devices) */

    /* TODO - if needed, compute and set device clocking information */

    /*
     * TODO - verify defaultSelTimeOut, convert it from usec to TEMPLATE_DEV
     * register values, and enter value in TEMPLATE_DEV structure or registers.
     */

    /* disconnect is not supported with SCSI-1, for now */

    pDev->scsiCtrl.disconnect = (TBOOL) FALSE;

    templateHwInit (pDev); 	/* initialize the TEMPLATE_DEV hardware */

    return (OK);
    }

/*******************************************************************************
*
* templateDevSelect - attempt to select a SCSI physical device
*
* This routine is called from scsiLib to select a physical device.
* The driver selects the device and sends the initial MSG_OUT and CMD
* bytes.
*
* RETURNS: OK if device was successfully selected, otherwise ERROR.
*/

LOCAL STATUS templateDevSelect
    (
    SCSI_PHYS_DEV *pScsiPhysDev,   /* ptr to SCSI physical device info */
    SCSI_TRANSACTION *pScsiXaction /* ptr to SCSI transaction info     */
    )
    {
    TEMPLATE_DEV *pDev;		/* ptr to TEMPLATE_DEV info */
    int templateBusId;          /* SCSI bus ID of the TEMPLATE_DEV */
    UINT8 identMsg;		/* for construction of the IDENTIFY message */
    int ix;			/* loop index */
    UINT8 *pCmdByte;		/* ptr to a command byte */
    UINT8 templateOpcode;	/* TEMPLATE_DEV opcode byte */

    pDev = (TEMPLATE_DEV *) pScsiPhysDev->pScsiCtrl;

    /* Return ERROR, if trying to select device's own id */

    if ((templateBusIdGet (pDev, &templateBusId) == ERROR) ||
        (templateBusId == pScsiPhysDev->scsiDevBusId))
	{
        return (ERROR);
	}

    pDev->pDevToSelect  = pScsiPhysDev;
    pScsiPhysDev->devStatus = SELECT_REQUESTED;

    /* TODO - issue device select command sequence */

    if (pScsiPhysDev->useIdentify)		/* send an identify message */
	{
	identMsg = SCSI_MSG_IDENTIFY |
	(pDev->scsiCtrl.disconnect ?
	SCSI_MSG_IDENT_DISCONNECT : 0) |
	(UINT8) pScsiPhysDev->scsiDevLUN;

	/* TODO - send identMsg in MSG_OUT phase */
	}

    pCmdByte = pScsiXaction->cmdAddress;

    for (ix = 0; ix < pScsiXaction->cmdLength; ix++)
	{
	/* TODO - send other command bytes */
	;
	}

    if (pScsiPhysDev->useIdentify)
	{
	if (pScsiPhysDev->msgLength == 0)
	    templateOpcode = (UINT8) TEMPLATE_ATN_SELECT;
	else
	    templateOpcode = (UINT8) TEMPLATE_STOP_SELECT;
	}
    else
	templateOpcode = (UINT8) TEMPLATE_SELECT;

    /* Send proper SELECT command code */

    templateCommand (pDev, templateOpcode);

    if (semTake (&pDev->scsiCtrl.ctrlSyncSem,
		 ((pScsiXaction->cmdTimeout / SCSI_TIMEOUT_1SEC) + 1) *
		 sysClkRateGet())
		 == ERROR)
	{
	/* timeout on semaphore, didn't get an interrupt */

	printErr ("templateDevSelect: No interrupt received.\n");

errorRecovery:

	/* TODO - flush device FIFOs, if needed */

	/* reset bus, delay 3 seconds to allow devices to reset */

	templateScsiBusReset (pDev);
	taskDelay (3 * sysClkRateGet ()); /* 3 seconds */
	return (ERROR);
	}

    if (!(DEVICE_IS_DISCONNECTED))
	{
	if (NOT_EXPECTED_STATE)
	    {
	    printErr ("templateDevSelect: Unknown chip state.\n");

	    /* TODO - print error statistics */

	    goto errorRecovery;
	    }
	else
	    {
	    /* TODO - flush FIFOs, if needed */

	    return (OK);
	    }
	}
    else
	{
	/* simple device select timeout */

	/* TODO - Flush FIFOs, if needed */

	errnoSet (S_scsiLib_SELECT_TIMEOUT);
        return (ERROR);
	}
    }

/*******************************************************************************
*
* templateMsgInAck - de-assert the ACK line to accept message
*
* This routine should be called when an incoming message has been read and
* accepted.  If the incoming message did not imply an impending disconnect,
* give the synchronization semaphore, so that subsequent phase can be
* detected.
*
* RETURNS: OK.
*/

LOCAL STATUS templateMsgInAck
    (
    SCSI_CTRL* pScsiCtrl,	/* ptr to TEMPLATE_DEV info		*/
    BOOL expectDisconn		/* whether a disconnect is expected	*/
    )
    {

    /* TODO - Send MSG_ACCEPTED to terminate MSG_IN phase, wait for new phase */

    return (OK);
    }

/*******************************************************************************
*
* templateProgBytesOut - output bytes using programmed i/o
*
* This routine is called from templateBytesOut to output data in programmed
* i/o mode.  This presumes that no DMA output routine exists, or we are
* not in data out phase.
*
* RETURNS: OK if successful, otherwise ERROR.
*
* NOMANUAL
*/

STATUS templateProgBytesOut
    (
    SCSI_PHYS_DEV* pScsiPhysDev,/* ptr to physical device info       */
    UINT8* pBuffer,		/* ptr to the data buffer            */
    int bufLength,		/* number of bytes to be transferred */
    int scsiPhase		/* phase of the transfer             */
    )
    {
    TEMPLATE_DEV *pDev;		/* ptr to TEMPLATE_DEV info */

    pDev = (TEMPLATE_DEV *) pScsiPhysDev->pScsiCtrl;

    if (bufLength <= 0)
	return OK;

    /* TODO - give any setup commands */

    while (bufLength > 0)
	{

	/* TODO - output byte at pBuffer, exit on error */

	pBuffer++;
	bufLength--;
        }

    return (OK);
    }

/*******************************************************************************
*
* templateProgBytesIn - input bytes from SCSI using programmed i/o
*
* Read bytes during any input mode.
*
* RETURNS: OK if successful, otherwise ERROR.
*
* NOMANUAL
*/

STATUS templateProgBytesIn
    (
    SCSI_PHYS_DEV *pScsiPhysDev,/* ptr to physical device info       */
    UINT8 *pBuffer,		/* ptr to the data buffer            */
    int bufLength,		/* number of bytes to be transferred */
    int scsiPhase		/* phase of the transfer             */
    )
    {
    TEMPLATE_DEV *pDev;		/* ptr to TEMPLATE_DEV info */

    if (bufLength <= 0)
	return OK;

    pDev = (TEMPLATE_DEV *) pScsiPhysDev->pScsiCtrl;

    /* TODO - give any appropriate setup commands */

    while (bufLength > 0)
        {
        *pBuffer++ = 0; /* TODO - transfer one byte to *pBuffer, exit on error */
	bufLength--;
        }

    /* send ack to terminate msgin phase */

    if (scsiPhase == SCSI_MSG_IN_PHASE)
        {
        templateMsgInAck ((SCSI_CTRL *) pScsiPhysDev->pScsiCtrl, TRUE);
        }

    return (OK);
    }

/*******************************************************************************
*
* templateBytesOut - write bytes to SCSI data-out phase
*
* This routine is called from scsi1Lib to output data.
* Data is written using either the programmed out routine, or the dma
* out routine.  DMA is only used during the data out
* phase.  Programmed output is used for all other phases.
*
* RETURNS: OK if specified bytes were output successfully, otherwise ERROR.
*/

LOCAL STATUS templateBytesOut
    (
    SCSI_PHYS_DEV* pScsiPhysDev,	/* ptr to SCSI physical dev info     */
    char* pBuffer,			/* ptr to byte buffer for output     */
    int bufLength,			/* number of bytes to be transferred */
    int scsiPhase			/* SCSI bus phase of the transfer    */
    )
    {
    TEMPLATE_DEV *pDev;			/* ptr to TEMPLATE_DEV info */
    STATUS status;			/* local status for iterative xfers */
    int xferLength;			/* local length for iterative xfers */

    pDev = (TEMPLATE_DEV *) pScsiPhysDev->pScsiCtrl;

    while (bufLength > 0)
	{
	xferLength = min (bufLength, TEMPLATE_MAX_XFER_LENGTH);

        if ((scsiPhase != SCSI_DATA_OUT_PHASE) ||
	    (pDev->scsiCtrl.scsiDmaBytesOut == NULL))
	    {
	    status = templateProgBytesOut (pScsiPhysDev, (UINT8 *) pBuffer,
				      xferLength, scsiPhase);
	    }
        else
	    {
	    status = (pDev->scsiCtrl.scsiDmaBytesOut)
		     (pScsiPhysDev, (UINT8 *) pBuffer, xferLength);
	    }

	/* if ERROR was returned, exit now */

	if (status != OK)
	    break;

	pBuffer += TEMPLATE_MAX_XFER_LENGTH;
	bufLength -= TEMPLATE_MAX_XFER_LENGTH;
	}

    return (status);
    }

/*******************************************************************************
*
* templateBytesIn -  read bytes from SCSI data in phase
*
* Read data from SCSI.  Programmed i/o is used for all phases other than
* data in.  For data in phase, DMA input will be used if available, otherwise
* programmed input is used for data in phase too.
*
* RETURNS: OK if requested bytes were input successfully, otherwise ERROR.
*/

LOCAL STATUS templateBytesIn
    (
    SCSI_PHYS_DEV* pScsiPhysDev,/* ptr to SCSI physical dev info     */
    char* pBuffer,              /* ptr to byte buffer for output     */
    int bufLength,              /* number of bytes to be transferred */
    int scsiPhase               /* SCSI bus phase of the transfer    */
    )
    {
    TEMPLATE_DEV* pDev;		/* ptr to TEMPLATE_DEV info */
    STATUS status = OK;		/* local status for iterative xfers */
    int xferLength;		/* local length for iterative xfers */

    pDev = (TEMPLATE_DEV *) pScsiPhysDev->pScsiCtrl;

    while (bufLength > 0)
        {
        xferLength = min (bufLength, TEMPLATE_MAX_XFER_LENGTH);

        if ((scsiPhase != SCSI_DATA_IN_PHASE) ||
            (pDev->scsiCtrl.scsiDmaBytesIn == NULL))
            {
            status = templateProgBytesIn (pScsiPhysDev, (UINT8 *) pBuffer,
                                     xferLength, scsiPhase);
            }
        else
            {
            status = (pDev->scsiCtrl.scsiDmaBytesIn)
                     (pScsiPhysDev, (UINT8 *) pBuffer, xferLength);
            }

        if (status != OK)
            break;

	pBuffer += TEMPLATE_MAX_XFER_LENGTH;
        bufLength -= TEMPLATE_MAX_XFER_LENGTH;
	}

    return (status);
    }

/*******************************************************************************
*
* templateBusPhaseGet - return the current SCSI bus phase in *pBusPhase
*
* This template driver tries to remember the current bus state with
* each interrupt.  It is usually more efficient to return data from
* memory than to do an i/o cycle.
*
* RETURNS: OK, always.
*/

LOCAL STATUS templateBusPhaseGet
    (
    SCSI_CTRL* pScsiCtrl,  /* ptr to SCSI controller info     */
    int timeOutInUsec,          /* timeout in usec (0 == infinity) */
    int* pBusPhase              /* ptr to returned bus phase       */
    )
    {
    TEMPLATE_DEV *pDev;		/* ptr to TEMPLATE_DEV info */

    pDev = (TEMPLATE_DEV *) pScsiCtrl;

    /*
     * TODO - if current bus phase was stored in memory from
     * last interrupt cycle, then return that information.
     * Otherwise, read bus phase from device.
     */

    return (OK);
    }

/*******************************************************************************
*
* templateSelTimeOutCvt - convert a select time-out in microseconds to its
* equivalent TEMPLATE_DEV setting
*
* Also, the TEMPLATE_DEV accepts up to a 8 bit time-out, so a maximum value
* of 0xff is returned in *pTimeOutSetting.
*/

LOCAL void templateSelTimeOutCvt
    (
    SCSI_CTRL* pScsiCtrl,          /* ptr to SCSI controller info */
    UINT timeOutInUsec,            /* time-out in microsecs       */
    UINT* pTimeOutSetting          /* time-out equivalent setting */
    )
    {
    /*
     * TODO - convert microseconds to device data.
     * Store at *pTimeOutSetting
     */
    }

/*******************************************************************************
*
* templateBusIdGet - get the current SCSI bus ID of the TEMPLATE_DEV.
*
* Copies the bus ID to <pBusId>.
*
* RETURNS:
* OK if Bus ID register holds a legal value, otherwise ERROR.
*/

LOCAL STATUS templateBusIdGet
    (
    TEMPLATE_DEV* pDev,             /* ptr to TEMPLATE_DEV info        */
    int* pBusId            /* ptr to returned bus ID */
    )
    {
    /* TODO - put device id into *pBusId location */

    return (OK);
    }

/*******************************************************************************
*
* templateCommand - write a command code to the TEMPLATE_DEV Command Register
*
* NOMANUAL
*/

LOCAL void templateCommand
    (
    TEMPLATE_DEV* pDev,		/* ptr to TEMPLATE_DEV info  */
    int cmdCode			/* new command code */
    )
    {
    /*
     * Clear the sync semaphore, just in case this command generates an
     * interrupt.
     */
    semTake (&pDev->scsiCtrl.ctrlSyncSem, NO_WAIT);

    /* TODO - issue command to device */
    }

/*******************************************************************************
*
* templateScsiBusReset - assert the RST line on the SCSI bus
*
* Issue a SCSI Bus Reset command to the NCR 5390.  This should put all devices
* on the SCSI bus in an initial quiescent state.
*/

LOCAL void templateScsiBusReset
    (
    TEMPLATE_DEV *pDev		/* ptr to TEMPLATE_DEV info */
    )
    {
    /* TODO - issue bus reset command */

    semTake (&pDev->scsiCtrl.ctrlSyncSem, WAIT_FOREVER);

    /* TODO - set remembered bus phase to be BUS_FREE */

    /* allow time for reset signal to become inactive */

    taskDelay (2);
    }

/*******************************************************************************
*
* templateIntr - interrupt service routine for the TEMPLATE_DEV
*
* NOMANUAL
*/

void templateIntr
    (
    TEMPLATE_DEV *pDev          /* ptr to TEMPLATE_DEV info */
    )
    {
    SCSI_PHYS_DEV *pDevToSelect;

    /* TODO - read current device state, including bus phase */

    /* Quick exit if no interrupt present, this is not an error. */

    if (DEVICE_IS_NOT_INTERRUPTING)
	return;

    /* TODO - save status registers as needed */

    if (DMA_TERMINAL_COUNT_REACHED)
	{
	/* TODO - if necessary, clear the DMA TC condition */
	}

    if (DEVICE_FUNC_COMPLETE)
        {
    	/* if a selection attempt has been made, report success */

	if (((pDevToSelect = pDev->pDevToSelect) != (SCSI_PHYS_DEV *) NULL) &&
	    (pDevToSelect->devStatus == SELECT_IN_PROGRESS))
	    {
	    pDevToSelect->devStatus = SELECT_SUCCESSFUL;
	    pDev->pDevToSelect = (SCSI_PHYS_DEV *) NULL;
	    }
        }
    else if (DEVICE_IS_DISCONNECTED)
        {
    	/* if a device has not been selected, then a timeout has occurred */

	if (((pDevToSelect = pDev->pDevToSelect) != (SCSI_PHYS_DEV *) NULL) &&
	    (pDevToSelect->devStatus == SELECT_IN_PROGRESS))
	    {
	    pDevToSelect->devStatus = SELECT_TIMEOUT;
	    pDev->pDevToSelect = (SCSI_PHYS_DEV *) NULL;
	    }
	else
	    {
	    pDev->scsiCtrl.scsiBusPhase = SCSI_BUS_FREE_PHASE;
	    }
        }

    /* send synch signal to controlling task */

    semGive (&pDev->scsiCtrl.ctrlSyncSem);

    /*
     * TODO - If device is slow, check that the interrupt line has
     * actually gone low, but don't loop forever.
     */

    }

/*******************************************************************************
*
* templateHwInit - initialize the TEMPLATE_DEV chip to a known state
*
* This routine puts the TEMPLATE_DEV into a known quiescent state and issues a reset
* to the SCSI Bus if any signals are active, thus putting target devices in
* some presumably known state.  Currently the initial state is not configurable
* and does not enable reselection.
*
* INTERNAL
* Needs to handle parity enable
*/

LOCAL void templateHwInit
    (
    TEMPLATE_DEV* pDev          /* ptr to an TEMPLATE_DEV info structure */
    )
    {
    /* TODO - reset the device to its initial state */

    templateScsiBusReset (pDev);	 /* Reset the bus */
    }

#ifdef INCLUDE_SHOW_ROUTINES
/*******************************************************************************
*
* templateShow - display the status of the TEMPLATE_DEV device
*
* This routine displays the state of the TEMPLATE_DEV registers in a
* user-friendly manner.  It is useful primarily for debugging.
*
* EXAMPLE:
* .CS
*     -> templateShow
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

LOCAL STATUS templateShow
    (
    SCSI_CTRL *pScsiCtrl      /* ptr to TEMPLATE_DEV info */
    )
    {
    TEMPLATE_DEV *pDev;

    if (pScsiCtrl == NULL)
	{
	if (pSysScsiCtrl == NULL)
	    {
	    printErr ("No SCSI controller specified.\n");
	    return (ERROR);
	    }
	pScsiCtrl = pSysScsiCtrl;
	}

    pDev = (TEMPLATE_DEV *) pScsiCtrl;

    /* TODO - access and display all the device registers */

    return (OK);
    }
#endif /* INCLUDE_SHOW_ROUTINES */
