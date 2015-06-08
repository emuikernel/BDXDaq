/* templateScsi2.c - Template for a SCSI-2 driver */

/* Copyright 1989-1997 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
TODO -	Remove the template modification history and begin a new history
	starting with version 01a and growing the history upward with
	each revision.

01d,24sep97,dat  changed arguments to TEMPLATE_REG_READ.
01c,20aug97,dat  code review comments from Dinesh
01b,11aug97,dat  fixed compilation bugs.
01a,01aug97,dat  written (from ncr810Lib.c, ver 01p)
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

TODO - Update the remainder of this documentation to reflect your new
driver code.

For each controller device we create a manager task to manage the threads
on each bus.

This driver supports multiple initiators, disconnect/reconnect, tagged
command queueing and synchronous data transfer protocol.  In general, the
SCSI system and this driver will automatically choose the best combination
of these features to suit the target devices used.  However, the default
choices may be over-ridden by using the function "scsiTargetOptionsSet()"
(see scsiLib).

USER-CALLABLE ROUTINES
Most of the routines in this driver are accessible only through the I/O
system.  Three routines, however, must be called directly:
templateCtrlCreate() to create a controller structure, and templateCtrlInit()
to initialize it.

There are debug variables to trace events in the driver.
<scsiDebug> scsiLib debug variable, trace event in scsiLib, scsiScsiPhase(),
and scsiTransact().
<scsiIntsDebug> prints interrupt information.

The macro SCSI_DEBUG_MSG(format,...) can be used to insert debugging
messages anywhere in the code.  The debug messages will appear only when
the global variable scsiDebug is set to a non-zero value.

INTERFACE
The BSP must connect the interrupt service routine for the controller device
to the appropriate interrupt system.  The routine to be called is
templateIntr(), and the argument is the pointer to the controller device pSiop.
i.e.

.CS
    pSiop = templateCtrlCreate (...);
    intConnect (XXXX, templateIntr, pSiop);
    templateCtrlInit (pSiop, ...);
.CE

HARDWARE ACCESS
All hardware access is to be done through macros.  The default definition
of the TEMPLATE_REG_READ() and TEMPLATE_REG_WRITE() macros assumes a
memory mapped i/o model.  The macros can be redefined as necessary to 
accomodate other models, and situations where timing and write pipe
considerations need to be addressed.

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

INCLUDE FILES
scsiLib.h
*/

#define INCLUDE_SCSI2	/* This is a SCSI2 driver */
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
#include "scsiLib.h"

/* defines */

/* state enumeration, different hardware states for the controller */

typedef enum templateState	/* TEMPLATE_STATE */
    {
    TEMPLATE_STATE_IDLE = 0,		/* not running any script            */
    TEMPLATE_STATE_PASSIVE,		/* waiting for reselect or host cmd  */
    TEMPLATE_STATE_ACTIVE		/* running a client script           */
    } TEMPLATE_STATE;

/* Hardware specific events, supplements the SCSI_EVENT types */

typedef enum templateEvent	/* TEMPLATE_EVENT_TYPE, hardware events */
    {
    TEMPLATE_SINGLE_STEP=30,		/* don't conflict with SCSI_EVENTS */
    TEMPLATE_ABORT,
    TEMPLATE_UNEXPECTED_DISCON,
    TEMPLATE_SCSI_TIMEOUT,
    TEMPLATE_HANDSHAKE_TIMEOUT,
    TEMPLATE_PHASE_MISMATCH,
    TEMPLATE_SCRIPT_ABORTED,
    TEMPLATE_SCSI_COMPLETE,
    TEMPLATE_FATAL_ERROR,
    TEMPLATE_MESSAGE_OUT_SENT,
    TEMPLATE_MESSAGE_IN_RECVD,
    TEMPLATE_NO_MSG_OUT,
    TEMPLATE_EXT_MESSAGE_SIZE,
    TEMPLATE_ILLEGAL_PHASE,
    } TEMPLATE_EVENT_TYPE;

/* identifiers for the different hardware scripts the device can execute */

typedef enum templateScriptEntry /* TEMPLATE_SCRIPT_ENTRY */
    {
    TEMPLATE_SCRIPT_WAIT           = 0,	/* wait for re-select or host cmd */
    TEMPLATE_SCRIPT_INIT_START     = 1,	/* start an initiator thread      */
    TEMPLATE_SCRIPT_INIT_CONTINUE  = 2,	/* continue an initiator thread   */
    TEMPLATE_SCRIPT_TGT_DISCONNECT = 3	/* disconnect a target thread     */
    } TEMPLATE_SCRIPT_ENTRY;

/* The expanded EVENT structure, with add'l device specific info */

typedef struct { 		/* TEMPLATE_EVENT, extended EVENT type */
    SCSI_EVENT scsiEvent;
    int remCount;
    } TEMPLATE_EVENT;

/* The expanded THREAD structure, with add'l device specific info */

typedef struct templateThread	/* TEMPLATE_THREAD, extended thread type */
    {
    SCSI_THREAD	scsiThread;	/* generic SCSI thread structure */

    /* TODO - add device specific thread info */

    UINT8 nHostFlags;		/* DUMMY information */
    UINT8 nMsgOutState;		/* DUMMY information */
    UINT8 nMsgInState;		/* DUMMY information */
    UINT8 nBusIdBits;		/* DUMMY information */
    UINT8 nBusPhase;		/* DUMMY information */
    } TEMPLATE_THREAD;

/* The expanded SCSI_CTRL structure with add'l device specific info */

typedef struct {		/* TEMPLATE_SCSI_CTRL */
    SCSI_CTRL scsiCtrl;         /* generic SCSI controller info */

    /* TODO - add device specific information */

    SEM_ID    singleStepSem;	/* use to debug script in single step mode */
    
    /* Hardware implementation dependencies */

    TEMPLATE_THREAD* pIdentThread;
    TEMPLATE_THREAD* pNewThread;/* DUMMY */
    TEMPLATE_THREAD* pHwThread; /* DUMMY */
    TEMPLATE_STATE   state;	/* DUMMY */
    UCHAR*           pCmd;	/* DUMMY Device command/status register */
    int   devType;		/* DUMMY type of device (see define's below) */
    BOOL  resetReportDsbl;	/* DUMMY disable SCSI bus reset reporting */
    BOOL  parityTestMode;	/* DUMMY enable parity test mode */
    BOOL  parityCheckEnbl;	/* DUMMY to enable parity checking */
    UINT  clkPeriod;	    	/* DUMMY period of ctrl clock (nsec x 100) */
    UINT8 clkDiv;		/* DUMMY async and sync clock divider */
    BOOL  cmdPending;		/* DUMMY task wants to start new command */
    BOOL  singleStep;		/* DUMMY single step mode */
    } TEMPLATE_SCSI_CTRL;

typedef TEMPLATE_SCSI_CTRL SIOP;	/* shorthand */

/* Hardware Abstraction macros hide all actual access to the chip */

#define TEMPLATE_REG_READ(pSiop, reg, result) \
	((result) = *(pSiop->reg)

#define TEMPLATE_REG_WRITE(pSiop, reg, data) \
	(*pSiop->reg = data)

/* Other miscellaneous defines */

#define TEMPLATE_MAX_XFER_WIDTH 1		/* 16 bit wide transfer */
#define TEMPLATE_MAX_XFER_LENGTH 0xffffff	/* 16MB max transfer */

/* Configurable options, scsi manager task */

int   templateSingleStepSemOptions = SEM_Q_PRIORITY;
char* templateScsiTaskName         = SCSI_DEF_TASK_NAME;
int   templateScsiTaskOptions      = SCSI_DEF_TASK_OPTIONS;
int   templateScsiTaskPriority     = SCSI_DEF_TASK_PRIORITY;
int   templateScsiTaskStackSize    = SCSI_DEF_TASK_STACK_SIZE;

/* forward declarations */

LOCAL STATUS templateThreadActivate (SIOP* pSiop, TEMPLATE_THREAD* pThread);
LOCAL void templateEvent (SIOP* pSiop, TEMPLATE_EVENT* pEvent);
LOCAL STATUS templateThreadInit (SIOP* pSiop, TEMPLATE_THREAD* pThread);
LOCAL STATUS templateThreadActivate (SIOP* pSiop, TEMPLATE_THREAD* pThread);
LOCAL BOOL templateThreadAbort (SIOP* pSiop, TEMPLATE_THREAD* pThread);
LOCAL STATUS templateScsiBusControl (SIOP* pSiop, int operation);
LOCAL STATUS templateXferParamsQuery (SCSI_CTRL* pScsiCtrl, UINT8* pOffset,
					UINT8* pPeriod);
LOCAL STATUS templateXferParamsSet (SCSI_CTRL* pScsiCtrl, UINT8 offset,
					UINT8 period);
LOCAL STATUS templateWideXferParamsQuery (SCSI_CTRL* pScsiCtrl,
					UINT8* xferWidth);
LOCAL STATUS templateWideXferParamsSet (SCSI_CTRL* pScsiCtrl, UINT8 xferWidth);
LOCAL void templateScriptStart (SIOP* pSiop, TEMPLATE_THREAD* pThread,
				TEMPLATE_SCRIPT_ENTRY entryId);
LOCAL int templateEventTypeGet (SIOP* pSiop);
LOCAL STATUS templateThreadParamsSet (TEMPLATE_THREAD*, UINT8, UINT8);
LOCAL STATUS templateActivate (SIOP* pSiop, TEMPLATE_THREAD* pThread);
LOCAL void templateThreadStateSet (TEMPLATE_THREAD* pThread,
				SCSI_THREAD_STATE state);
LOCAL void templateAbort (SIOP* pSiop);
LOCAL void templateThreadEvent (TEMPLATE_THREAD* pThread, 
				TEMPLATE_EVENT* pEvent);
LOCAL void templateInitEvent (TEMPLATE_THREAD* pThread,
				TEMPLATE_EVENT*  pEvent);
LOCAL void templateThreadUpdate (TEMPLATE_THREAD* pThread);
LOCAL void templateInitIdentEvent(TEMPLATE_THREAD* pThread,
				TEMPLATE_EVENT* pEvent);
LOCAL void templateIdentInContinue (TEMPLATE_THREAD *pThread);
LOCAL STATUS templateResume (SIOP* pSiop, TEMPLATE_THREAD* pThread,
			    TEMPLATE_SCRIPT_ENTRY entryId);
LOCAL void templateThreadFail (TEMPLATE_THREAD* pThread, int errNum);
LOCAL void templateThreadComplete (TEMPLATE_THREAD* pThread);
LOCAL void templateThreadDefer (TEMPLATE_THREAD* pThread);
LOCAL STATUS templatePhaseMismatch (TEMPLATE_THREAD* pThread, int phase,
				UINT remCount);
LOCAL void templateThreadReconnect (TEMPLATE_THREAD* pThread);

/*******************************************************************************
*
* templateCtrlCreate - create a structure for a TEMPLATE device
*
* This routine creates a SCSI Controller data structure and must be called 
* before using an SCSI Controller chip.  It should be called once and only 
* once for a specified SCSI Controller controller. Since it allocates memory
* for a structure needed by all routines in templateLib, it must be called
* before any other routines in the library. After calling this routine,
* templateCtrlInit() should be called at least once before any SCSI
* transactions are initiated using the SCSI Controller.
*
* RETURNS: A pointer to TEMPLATE_SCSI_CTRL structure, or NULL if memory 
* is unavailable or there are invalid parameters.
*/

TEMPLATE_SCSI_CTRL *templateCtrlCreate 
    (

    /* TODO - This argument list is device specific information */

    UINT8* baseAdrs,		/* DUMMY, base address of the SCSI Controller */
    UINT   clkPeriod,		/* DUMMY, clock controller period (nsec*100) */
    UINT16 devType		/* DUMMY, SCSI device type */
    )
    {
    FAST SIOP *pSiop;		    /* ptr to SCSI Controller info */
    SCSI_CTRL *pScsiCtrl;
    
    /* TODO - cacheDmaMalloc the controller struct and any shared data areas */

    pScsiCtrl = &(pSiop->scsiCtrl);

    /* fill in generic SCSI info for this controller */

    pScsiCtrl->eventSize 	   = sizeof (TEMPLATE_EVENT);
    pScsiCtrl->threadSize 	   = sizeof (TEMPLATE_THREAD);
    pScsiCtrl->maxBytesPerXfer 	   = TEMPLATE_MAX_XFER_LENGTH;
    pScsiCtrl->wideXfer 	   = FALSE;
    pScsiCtrl->scsiTransact 	   = (FUNCPTR)	   scsiTransact;
    pScsiCtrl->scsiEventProc       = (VOIDFUNCPTR) templateEvent;
    pScsiCtrl->scsiThreadInit      = (FUNCPTR)     templateThreadInit;
    pScsiCtrl->scsiThreadActivate  = (FUNCPTR)     templateThreadActivate;
    pScsiCtrl->scsiThreadAbort     = (FUNCPTR)     templateThreadAbort;
    pScsiCtrl->scsiBusControl      = (FUNCPTR)     templateScsiBusControl;
    pScsiCtrl->scsiXferParamsQuery = (FUNCPTR)     templateXferParamsQuery;
    pScsiCtrl->scsiXferParamsSet   = (FUNCPTR)     templateXferParamsSet;
    pScsiCtrl->scsiWideXferParamsQuery = (FUNCPTR) templateWideXferParamsQuery;
    pScsiCtrl->scsiWideXferParamsSet   = (FUNCPTR) templateWideXferParamsSet;

    /* TODO - fill in device specific data for this controller */

    scsiCtrlInit (pScsiCtrl);

    /* Create synchronisation semaphore for single-step support  */

    pSiop->singleStepSem = semBCreate (templateSingleStepSemOptions, SEM_EMPTY);

    /* TODO - Initialize fields in any client shared data area */

    /* spawn SCSI manager - use generic code from "scsiLib.c" */

    pScsiCtrl->scsiMgrId = taskSpawn (templateScsiTaskName,
		       	    	      templateScsiTaskPriority,
		       	    	      templateScsiTaskOptions,
		       	    	      templateScsiTaskStackSize,
		       	    	      (FUNCPTR) scsiMgr,
		       	    	      (int) pSiop, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    
    return (pSiop);
    }

/*******************************************************************************
*
* templateCtrlInit - initialize a SCSI Controller Structure.
*
* This routine initializes an  SCSI Controller structure, after the structure
* is created with templateCtrlCreate().  This structure must be initialized
* before the SCSI Controller can be used.  It may be called more than once
* if needed; however,it should only be called while there is no activity on
* the SCSI interface.
*
* RETURNS: OK, or ERROR if parameters are out of range.
*/

STATUS templateCtrlInit
    (
    SIOP* pSiop,	/* ptr to SCSI Controller struct */
    int scsiCtrlBusId	/* SCSI bus ID of this SCSI Controller */
    )
    {
    SCSI_CTRL* pScsiCtrl = &pSiop->scsiCtrl;

    pScsiCtrl->scsiCtrlBusId = scsiCtrlBusId;

    /* TODO - Initialise and reset the SCSI Controller */

    templateScriptStart (pSiop, (TEMPLATE_THREAD *)pScsiCtrl->pIdentThread,
			TEMPLATE_SCRIPT_WAIT);

    pSiop->state = TEMPLATE_STATE_PASSIVE;

    return (OK);
    }

/*******************************************************************************
*
* templateIntr - interrupt service routine for the SCSI Controller
*
* The first thing to determine is if the device is generating an interrupt.
* If not, then this routine must exit as quickly as possible.
*
* Find the event type corresponding to this interrupt, and carry out any
* actions which must be done before the SCSI Controller is re-started.  
* Determine  whether or not the SCSI Controller is connected to the bus 
* (depending on the event type - see note below).  If not, start a client 
* script if possible or else just make the SCSI Controller wait for something 
* else to happen.
*
* Notify the SCSI manager of a controller event.
*
* RETURNS: N/A
*/

void templateIntr
    (
    SIOP *pSiop
    )
    {
    TEMPLATE_EVENT	event;
    SCSI_EVENT*		pScsiEvent = (SCSI_EVENT *) &event.scsiEvent;
    BOOL 		connected = TRUE;
    BOOL 		notify    = TRUE;
    TEMPLATE_STATE  	oldState  = (int) pSiop->state;

    /* TODO - If device is not interrupting, then return immediately */

    /* TODO - Save (partial) Controller register context in current thread */

    pScsiEvent->type = templateEventTypeGet (pSiop);

    /* Synchronise with single-step routine, if enabled. */

    if (pSiop->singleStep)
    	semGive (pSiop->singleStepSem);

    if (pScsiEvent->type == TEMPLATE_SINGLE_STEP)
	return;
    
    switch (pScsiEvent->type)
	{
	/*
	 * TODO - 
	 * For events where the bus is not connected to a target,
	 * set connected=FALSE.
	 *
	 * For events not requiring scsi manager, set notify = FALSE.
	 */

	/* Generic event types */

	case SCSI_EVENT_SELECTED:
	case SCSI_EVENT_RESELECTED:
	case SCSI_EVENT_DISCONNECTED:
	case SCSI_EVENT_BUS_RESET:

	/* device specific events */

	case TEMPLATE_ABORT:
	case TEMPLATE_UNEXPECTED_DISCON:
	case TEMPLATE_SCSI_TIMEOUT:
	case TEMPLATE_HANDSHAKE_TIMEOUT:
	case TEMPLATE_PHASE_MISMATCH:
	case TEMPLATE_SCRIPT_ABORTED:
	case TEMPLATE_SCSI_COMPLETE:
	case TEMPLATE_SINGLE_STEP:
	case TEMPLATE_FATAL_ERROR:

	default:
	    connected = TRUE;
	    notify = TRUE;
	}

    /*
     *  Controller is now idle: if possible, make it run a script.
     *
     *	If a SCSI thread is suspended and must be processed at task-level,
     *	leave the device idle.  It will be re-started by the SCSI manager
     *	calling "templateResume()".
     *
     *	Otherwise, if there's a new SCSI thread to start (i.e., the SCSI
     *	manager has called "threadActivate()"), start the appropriate script.
     *
     *	Otherwise, start a script which puts the device into passive mode
     *	waiting for re-selection, selection or a host command.
     *
     *	In all cases, clear any request to start a new thread.  The only
     *	tricky case is when there was a request pending and the device is
     *	left IDLE.  This should only ever occur when the current event is
     *	selection or reselection, in which case the SCSI manager will retry
     *	the activation request.  (Also see "templateActivate ()".)
     */

    if (connected)
	{
    	pSiop->state = TEMPLATE_STATE_IDLE;
	}
    else if (pSiop->cmdPending)
	{
	templateScriptStart (pSiop, pSiop->pNewThread,
			   TEMPLATE_SCRIPT_INIT_START);

	pSiop->state = TEMPLATE_STATE_ACTIVE;
	}
    else
	{
	templateScriptStart (pSiop, pSiop->pIdentThread,
			   TEMPLATE_SCRIPT_WAIT);
	
	pSiop->state = TEMPLATE_STATE_PASSIVE;
	}

    pSiop->cmdPending = FALSE;

    SCSI_INT_DEBUG_MSG ("templateIntr: state %d -> %d\n",
			oldState, pSiop->state, 0, 0, 0, 0);

    /* Send the event to the SCSI manager to be processed. */

    if (notify)
    	scsiMgrEventNotify ((SCSI_CTRL *) pSiop, pScsiEvent, sizeof (event));
    }

/*******************************************************************************
*
* templateEvent - SCSI Controller event processing routine
*
* Parse the event type and act accordingly.  Controller-level events are
* handled within this function, and the event is then passed to the current
* thread (if any) for thread-level processing.
*
* RETURNS: N/A
*/
LOCAL void templateEvent
    (
    SIOP *         pSiop,
    TEMPLATE_EVENT * pEvent
    )
    {
    SCSI_CTRL  *    pScsiCtrl  = (SCSI_CTRL *)  pSiop;
    SCSI_EVENT *    pScsiEvent = &pEvent->scsiEvent;
    TEMPLATE_THREAD * pThread    = (TEMPLATE_THREAD *) pScsiCtrl->pThread;
 
    SCSI_DEBUG_MSG ("templateEvent: received event %d (thread = 0x%08x)\n",
		    pScsiEvent->type, (int) pThread, 0, 0, 0, 0);

    /*	Do controller-level event processing */

    switch (pScsiEvent->type)
	{
	case SCSI_EVENT_SELECTED:
	case SCSI_EVENT_RESELECTED:

	    /*
	     *	Forward event to current thread, if any (it should defer)
	     *	then install a reserved thread for identification purposes.
	     */

    	    if (pThread != 0)
	    	templateThreadEvent (pThread, pEvent);

    	    pScsiCtrl->peerBusId = pScsiEvent->busId;

    	    pScsiCtrl->pThread = pScsiCtrl->pIdentThread;
	    
	    pScsiCtrl->pThread->role = (pScsiEvent->type == SCSI_EVENT_SELECTED)
		    	    	     ? SCSI_ROLE_IDENT_TARG
				     : SCSI_ROLE_IDENT_INIT;

	    pThread = (TEMPLATE_THREAD *) pScsiCtrl->pThread;

	    scsiMgrCtrlEvent (pScsiCtrl, SCSI_EVENT_CONNECTED);
	    break;

	case SCSI_EVENT_DISCONNECTED:
	case TEMPLATE_SCSI_COMPLETE:
	case TEMPLATE_UNEXPECTED_DISCON:
	case TEMPLATE_SCSI_TIMEOUT:
    	    pScsiCtrl->peerBusId = NONE;
    	    pScsiCtrl->pThread   = 0;

	    scsiMgrCtrlEvent (pScsiCtrl, SCSI_EVENT_DISCONNECTED);
	    
	    break;

	case SCSI_EVENT_BUS_RESET:
    	    pScsiCtrl->peerBusId = NONE;
    	    pScsiCtrl->pThread   = 0;

    	    scsiMgrBusReset (pScsiCtrl);
	    break;

	default:

	    /*
	     * Any other event type is assumed to be thread-specific.
	     * The thread event handler will report an error if it's
	     * not a valid type.
	     */

	    if (pThread == 0)
		{
	    	logMsg ("templateEvent: invalid event type (%d)\n",
		    	pScsiEvent->type, 0, 0, 0, 0, 0);
		}
	    break;
	}

    /* If there's a thread on the controller, forward the event to it */

    if (pThread != 0)
	templateThreadEvent (pThread, pEvent);
    }
    
/*******************************************************************************
*
* templateThreadInit - initialize a client thread structure
*
* Initialize the fixed data for a thread (i.e., independent of the command).
* Called once when a thread structure is first created.
*
* RETURNS: OK, or ERROR if an error occurs
*/

LOCAL STATUS templateThreadInit
    (
    SIOP *          pSiop,
    TEMPLATE_THREAD * pThread
    )
    {
    scsiThreadInit (&pThread->scsiThread);

    /* TODO - device specific thread initialization */

    return (OK);
    }

/*******************************************************************************
*
* templateThreadActivate - activate a SCSI connection for an initiator thread
*
* Set whatever thread/controller state variables need to be set.  Ensure that
* all buffers used by the thread are coherent with the contents of the
* system caches (if any).
*
* Set transfer parameters for the thread based on what its target device
* last negotiated.
*
* Update the thread context (including shared memory area) and note that
* there is a new client script to be activated (see "templateActivate()").
*
* Set the thread's state to ESTABLISHED.
* Do not wait for the script to be activated.  Completion of the script will
* be signalled by an event which is handled by "templateEvent()".
*
* RETURNS: OK or ERROR
*/

LOCAL STATUS templateThreadActivate
    (
    SIOP *          pSiop,		/* ptr to controller info */
    TEMPLATE_THREAD * pThread		/* ptr to thread info     */
    )
    {
    SCSI_CTRL *   pScsiCtrl   = (SCSI_CTRL *)   pSiop;
    SCSI_THREAD * pScsiThread = &pThread->scsiThread;
    SCSI_TARGET * pScsiTarget = pScsiThread->pScsiTarget;
    
    SCSI_DEBUG_MSG ("templateThreadActivate: thread 0x%08x: activating\n",
		    (int) pThread, 0, 0, 0, 0, 0);

    scsiCacheSynchronize (pScsiThread, SCSI_CACHE_PRE_COMMAND);

    /* Reset controller state variables: set sync xfer parameters */

    pScsiCtrl->msgOutState = SCSI_MSG_OUT_NONE;
    pScsiCtrl->msgInState  = SCSI_MSG_IN_NONE;
    
    /*
     * A wide transfer is initiated before a synchronous transfer. However,
     * when a wide transfer has been done the next activation causes the
     * synchronous transfer to be activated. Another possible but more
     * complicated way to implement this would be to have both wide and
     * synchronous negotiations in the same activation or thread i.e.
     * the same SCSI transaction.
     */

    scsiWideXferNegotiate (pScsiCtrl, pScsiTarget, WIDE_XFER_NEW_THREAD);
    scsiSyncXferNegotiate (pScsiCtrl, pScsiTarget, SYNC_XFER_NEW_THREAD);

    if (templateThreadParamsSet (pThread, pScsiTarget->xferOffset,
			     	        pScsiTarget->xferPeriod) != OK)
	{
	SCSI_ERROR_MSG ("templateThreadActivate: failed to set thread " \
			"params.\n", 0, 0, 0, 0, 0, 0);
	return (ERROR);
	}

    /* Update thread context; activate the thread */

    templateThreadUpdate (pThread);
    
    if (templateActivate (pSiop, pThread) != OK)
	{
	SCSI_ERROR_MSG ("templateThreadActivate: failed to activate thread.\n",
		        0, 0, 0, 0, 0, 0);
	return (ERROR);
	}

    pScsiCtrl->pThread = pScsiThread;

    templateThreadStateSet (pThread, SCSI_THREAD_ESTABLISHED);
    
    return (OK);
    }

/*******************************************************************************
*
* templateThreadAbort - abort a thread
*
* If the thread is not currently connected, do nothing and return FALSE to
* indicate that the SCSI manager should abort the thread.
*
* RETURNS: TRUE if the thread is being aborted by this driver (i.e. it is
* currently active on the controller, else FALSE.
*/

LOCAL BOOL templateThreadAbort
    (
    SIOP *          pSiop,		/* ptr to controller info */
    TEMPLATE_THREAD * pThread		/* ptr to thread info     */
    )
    {
    BOOL          tagged;
    SCSI_CTRL *   pScsiCtrl   = (SCSI_CTRL *)   pSiop;
    SCSI_THREAD * pScsiThread = &pThread->scsiThread;

    SCSI_DEBUG_MSG ("templateThreadAbort: thread 0x%08x (state = %d) "
		    "aborting\n", (int)pThread, pScsiThread->state, 0, 0, 0, 0);

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

	    templateAbort (pSiop);
    	    templateThreadStateSet (pThread, SCSI_THREAD_ABORTING);
	    break;
	}

    return (TRUE);
    }

/*******************************************************************************
*
* templateScsiBusControl - miscellaneous low-level SCSI bus control operations
*
* Currently supports only the SCSI_BUS_RESET operation; other operations are
* not used explicitly by the driver because they are carried out automatically
* by the script program.
*
* NOTE: after the SCSI bus has been reset, we expect the device
* to create a TEMPLATE_BUS_RESET event to be sent to the SCSI manager.
* See "templateIntr()".
*
* RETURNS: OK, or ERROR if an invalid operation is requested.
*
* NOMANUAL
*/

LOCAL STATUS templateScsiBusControl
    (
    SIOP * pSiop,   	    /* ptr to controller info                   */
    int    operation	    /* bitmask for operation(s) to be performed */
    )
    {
    if (operation & SCSI_BUS_RESET)
	{
	/* TODO - assert & deassert the RST line */

	return OK;
	}

    return ERROR; /* unknown operation */
    }

/*******************************************************************************
*
* templateXferParamsQuery - get synchronous transfer parameters
*
* Updates the synchronous transfer parameters suggested in the call to match
* the TEMPLATE SCSI Controller's capabilities.  Transfer period is in SCSI units
* (multiples * of 4 ns).
*
* RETURNS: OK
*/

LOCAL STATUS templateXferParamsQuery
    (
    SCSI_CTRL *pScsiCtrl,		/* ptr to controller info       */
    UINT8     *pOffset,			/* max REQ/ACK offset  [in/out] */
    UINT8     *pPeriod			/* min transfer period [in/out] */
    )
    {

    /* TODO - alter the offset and period variables to acceptable values */

    return (OK);
    }
    
/*******************************************************************************
*
* templateXferParamsSet - set transfer parameters
*
* Validate the requested parameters, convert to the TEMPLATE SCSI Controller's 
* native format and save in the current thread for later use (the chip's
* registers are not actually set until the next script activation for this 
* thread).
*
* Transfer period is specified in SCSI units (multiples of 4 ns).  An offset
* of zero specifies asynchronous transfer.
*
* RETURNS: OK if transfer parameters are OK, else ERROR.
*/

LOCAL STATUS templateXferParamsSet
    (
    SCSI_CTRL *pScsiCtrl,		/* ptr to controller info */
    UINT8      offset,			/* max REQ/ACK offset     */
    UINT8      period			/* min transfer period    */
    )
    {
    return (templateThreadParamsSet ((TEMPLATE_THREAD*)pScsiCtrl->pThread,
				    offset, period));
    }

/*******************************************************************************
*
* templateWideXferParamsQuery - get wide data transfer parameters
*
* Updates the wide data transfer parameters suggested in the call to match
* the TEMPLATE SCSI Controller's capabilities.  Transfer width is in the units 
* of the WIDE DATA TRANSFER message's transfer width exponent field. This is
* an 8 bit field where 0 represents a narrow transfer of 8 bits, 1 represents
* a wide transfer of 16 bits and 2 represents a wide transfer of 32 bits.
*
* RETURNS: OK
*/

LOCAL STATUS templateWideXferParamsQuery
    (
    SCSI_CTRL *pScsiCtrl,		/* ptr to controller info       */
    UINT8     *xferWidth		/* suggested transfer width     */
    )
    {
    if (*xferWidth > TEMPLATE_MAX_XFER_WIDTH)
	*xferWidth = TEMPLATE_MAX_XFER_WIDTH;
    
    templateWideXferParamsSet (pScsiCtrl, *xferWidth);

    return (OK);
    }

/*******************************************************************************
*
* templateWideXferParamsSet - set wide transfer parameters
*
* Assume valid parameters and set the TEMPLATE's thread parameters to the
* appropriate values. The actual registers are not written yet, but will
* be written from the thread values when it is activated.
*
* Transfer width is specified in SCSI transfer width exponent units. 
*
* RETURNS: OK 
*/

LOCAL STATUS templateWideXferParamsSet
    (
    SCSI_CTRL *pScsiCtrl,		/* ptr to controller info */
    UINT8      xferWidth 		/* wide data transfer width */
    )
    {

    /* TODO - update thread to enable/disable wide data xfers */

    return OK;
    }

/*******************************************************************************
*
* templateThreadParamsSet - set various parameters for a thread
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

LOCAL STATUS templateThreadParamsSet
    (
    TEMPLATE_THREAD * pThread,	    	/* thread to be affected  */
    UINT8           offset,		/* max REQ/ACK offset     */
    UINT8           period		/* min transfer period    */
    )
    {

    /* TODO - setup synch/asynch control bits */

    /* TODO - setup wide control bits */

    /* TODO - update device control registers if needed */

    return (OK);
    }


/******************************************************************************
*
* templateEventTypeGet - parse status registers at interrupt time
*
* This routine examines the device state to determine what type of event
* is pending.  If none is pending then a fatal error code is returned.
*
* RETURNS: Returns an interrupt (event) type code or TEMPLATE_FATAL_ERROR.
*/

LOCAL int templateEventTypeGet
    (
    SIOP * pSiop
    )
    {
    int key;

    /*
     * We should be locked in interrupt context while the status
     * registers are being read so that there is no contention between
     * a synchronous thread and a bus initiated thread (reselect)
     */

    key = intLock (); 

    /* TODO - Read and save device status registers */

    intUnlock (key);

    SCSI_INT_DEBUG_MSG ("templateEventTypeGet: \n", 0, 0, 0, 0, 0, 0);

    if (0) /* TODO - check for fatal error */
	return TEMPLATE_ABORT;

    /* TODO - try the rest (NB order of tests can be important !) */

    if  (0) /* TODO - check for SCSI Bus reset */
        {
	SCSI_INT_DEBUG_MSG ("templateEventTypeGet: SCSI bus reset.\n",
			    0, 0, 0, 0, 0, 0);

	/* TODO - clear FIFOs, etc. */
	
	return (SCSI_EVENT_BUS_RESET);
        }
     
    if  (0) /* TODO - check for unexpected disconnect */
        {
	SCSI_INT_DEBUG_MSG ("templateEventTypeGet: unexpected disconnection.\n",
			    0, 0, 0, 0, 0, 0);

	/* TODO - clear FIFOs, etc. */
	
	return (TEMPLATE_UNEXPECTED_DISCON);
        }

    if (0) /* TODO - check for scsi bus timeout */
	{
    	SCSI_INT_DEBUG_MSG ("templateEventTypeGet: SCSI bus timeout.\n",
			    0, 0, 0, 0, 0, 0);

	/* TODO - clear FIFOs, etc. */
	
	return (TEMPLATE_SCSI_TIMEOUT);
	}

    if (0) /* TODO - check for SCSI handshake timeout */
	{
    	SCSI_INT_DEBUG_MSG ("templateEventTypeGet: SCSI handshake timeout.\n",
			    0, 0, 0, 0, 0, 0);

	/* TODO - clear FIFOs, etc. */
	
	return (TEMPLATE_HANDSHAKE_TIMEOUT);
	}

    if (0) /* TODO - check for phase mismatch condition */
        {
	SCSI_INT_DEBUG_MSG ("templateEventTypeGet: phase mismatch.\n",
			0, 0, 0, 0, 0, 0);
	return (TEMPLATE_PHASE_MISMATCH);
        }
    
    if (0) /* TODO - check for a script abort */
        {
	SCSI_INT_DEBUG_MSG ("templateEventTypeGet: script aborted.\n",
		    	    0, 0, 0, 0, 0, 0);
	return (TEMPLATE_SCRIPT_ABORTED);
        }

    if (0) /* TODO - check for normal script completion */
	{
	SCSI_INT_DEBUG_MSG ("templateEventTypeGet: script completed.\n",
			    0, 0, 0, 0, 0, 0);
	return (TEMPLATE_SCSI_COMPLETE);
        }

    if (0) /* TODO - check for a single step event */
	{
	SCSI_INT_DEBUG_MSG ("templateEventTypeGet: single-step.\n",
			    0, 0, 0, 0, 0, 0);

	return (TEMPLATE_SINGLE_STEP);
	}

    /* No reason for the interrupt ! */
    
    return (TEMPLATE_FATAL_ERROR);
    }

/*******************************************************************************
*
* templateThreadEvent - SCSI Controller thread event processing routine
*
* Forward the event to the proper handler for the thread's current role.
*
* If the thread is still active, update the thread context (including
* shared memory area) and resume the thread.
*
* RETURNS: N/A
*/
LOCAL void templateThreadEvent
    (
    TEMPLATE_THREAD * pThread,
    TEMPLATE_EVENT *  pEvent
    )
    {
    SCSI_EVENT *  pScsiEvent  = &pEvent->scsiEvent;
    SCSI_THREAD * pScsiThread = &pThread->scsiThread;
    SIOP *        pSiop       = (SIOP *)        pScsiThread->pScsiCtrl;
    TEMPLATE_SCRIPT_ENTRY entryPt;
    
    SCSI_DEBUG_MSG ("templateThreadEvent: thread 0x%08x: received event %d\n",
		    (int) pThread, pScsiEvent->type, 0, 0, 0, 0);

    switch (pScsiThread->role)
	{
	case SCSI_ROLE_INITIATOR:
	    templateInitEvent (pThread, pEvent);

	    entryPt = TEMPLATE_SCRIPT_INIT_CONTINUE;
	    break;
	    
	case SCSI_ROLE_IDENT_INIT:
	    templateInitIdentEvent (pThread, pEvent);

	    entryPt = TEMPLATE_SCRIPT_INIT_CONTINUE;
	    break;

	case SCSI_ROLE_TARGET:
	default:
	    logMsg ("templateThreadEvent: thread 0x%08x: invalid role (%d)\n",
		    (int) pThread, pScsiThread->role, 0, 0, 0, 0);
	    entryPt = TEMPLATE_SCRIPT_TGT_DISCONNECT;
	    break;
	}

    /* Resume thread if it is still connected */

    switch (pScsiThread->state)
	{
	case SCSI_THREAD_INACTIVE:
	case SCSI_THREAD_WAITING:
	case SCSI_THREAD_DISCONNECTED:
	    break;

	default:
    	    templateThreadUpdate (pThread);

	    if (templateResume (pSiop, pThread, entryPt) != OK)
	    	{
	    	SCSI_ERROR_MSG("templateThreadEvent: failed to resume thread\n",
			    	0, 0, 0, 0, 0, 0);
	
		templateThreadFail (pThread, S_scsiLib_DISCONNECTED);
	    	}
	    break;
	}
    }

/*******************************************************************************
*
* templateInitEvent - device initiator thread event processing routine
*
* Parse the event type and handle it accordingly.  This may result in state
* changes for the thread, state variables being updated, etc.
*
* RETURNS: N/A
*/
LOCAL void templateInitEvent
    (
    TEMPLATE_THREAD * pThread,
    TEMPLATE_EVENT *  pEvent
    )
    {
    SCSI_EVENT *  pScsiEvent  = &pEvent->scsiEvent;
    SCSI_THREAD * pScsiThread = &pThread->scsiThread;
    SCSI_CTRL *   pScsiCtrl   = pScsiThread->pScsiCtrl;

    /* Update controller msg in/out state after script completes */

    pScsiCtrl->msgOutState = pThread->nMsgOutState;
    pScsiCtrl->msgInState  = pThread->nMsgInState;
    
    /*	Parse script exit status; handle as necessary */

    switch (pScsiEvent->type)
	{
	case SCSI_EVENT_DISCONNECTED:
    	    SCSI_DEBUG_MSG ("DISCONNECT message in\n", 0, 0, 0, 0, 0, 0);

	    scsiMgrThreadEvent (pScsiThread, SCSI_THREAD_EVENT_DISCONNECTED);

	    templateThreadStateSet (pThread, SCSI_THREAD_DISCONNECTED);
	    break;

	case TEMPLATE_SCSI_COMPLETE:
	    SCSI_DEBUG_MSG ("COMMAND COMPLETE message in\n", 0, 0, 0, 0, 0, 0);

	    templateThreadComplete (pThread);
	    break;

	case SCSI_EVENT_SELECTED:
	case SCSI_EVENT_RESELECTED:
    	    SCSI_DEBUG_MSG ("templateInitEvent: thread 0x%08x: (re)selection.\n",
		    	    (int) pThread, 0, 0, 0, 0, 0);
	    templateThreadDefer (pThread);
	    break;

    	case TEMPLATE_MESSAGE_OUT_SENT:
	    (void) scsiMsgOutComplete (pScsiCtrl, pScsiThread);
	    break;
	    
	case TEMPLATE_MESSAGE_IN_RECVD:
	    (void) scsiMsgInComplete (pScsiCtrl, pScsiThread);
	    break;
	    
	case TEMPLATE_NO_MSG_OUT:

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
	    
	case TEMPLATE_EXT_MESSAGE_SIZE:

	    /*
	     *  The SIOP has just read the length byte for an extended
	     *  message in.  The shared memory area is updated with the
	     *	appropriate length just before the thread is resumed (see
	     *	"templateThreadUpdate()".
	     */

	    break;
	    
	case TEMPLATE_PHASE_MISMATCH:
	    if (templatePhaseMismatch (pThread, pThread->nBusPhase,
				     pEvent->remCount) != OK)
		{
		templateThreadFail (pThread, errno);
		}
	    break;

	case TEMPLATE_SCSI_TIMEOUT:
	    SCSI_ERROR_MSG ("templateInitEvent: thread 0x%08x: " \
			    "select timeout.\n", (int) pThread, 0, 0, 0, 0, 0);
	    templateThreadFail (pThread, S_scsiLib_SELECT_TIMEOUT);
	    break;

	case TEMPLATE_SCRIPT_ABORTED:
	    SCSI_DEBUG_MSG ("templateInitEvent: thread 0x%08x: aborted\n",
			    (int) pThread, 0, 0, 0, 0, 0);
	    break;
	    
	case SCSI_EVENT_BUS_RESET:
    	    SCSI_DEBUG_MSG ("templateInitEvent: thread 0x%08x: bus reset\n",
		    	    (int) pThread, 0, 0, 0, 0, 0);

	    /*	Do not try to resume this thread.  SCSI mgr will tidy up. */

	    templateThreadStateSet (pThread, SCSI_THREAD_INACTIVE);
	    break;

	case TEMPLATE_UNEXPECTED_DISCON:

	    /* not really unexpected after an abort message ... */

    	    SCSI_ERROR_MSG ("templateInitEvent: thread 0x%08x: "
		    	    "unexpected disconnection\n",
		    	    (int) pThread, 0, 0, 0, 0, 0);

	    templateThreadFail (pThread, S_scsiLib_DISCONNECTED);
	    break;
	    
	case TEMPLATE_ILLEGAL_PHASE:
	    SCSI_ERROR_MSG ("templateInitEvent: thread 0x%08x: "
			    "illegal phase requested.\n",
			    (int) pThread, 0, 0, 0, 0, 0);

	    templateThreadFail (pThread, S_scsiLib_INVALID_PHASE);
	    break;

	default:
	    logMsg ("templateInitEvent: invalid event type (%d)\n",
		    pScsiEvent->type, 0, 0, 0, 0, 0);
	    break;
	}
    }


/******************************************************************************
*
* templateActivate - activate a script corresponding to a new thread
*
* Request activation of (the script for) a new thread, if possible; do not
* wait for the script to complete (or even start) executing.  Activation
* is requested by signalling the controller, which causes an interrupt.
* The script is started by the ISR in response to this event.
*
* NOTE: Interrupt locking is required to ensure that the correct action
* is taken once the controller state has been checked.
*
* RETURNS: OK, or ERROR if the controller is in an invalid state (this
* indicates a major software failure).
*/
LOCAL STATUS templateActivate
    (
    SIOP *          pSiop,
    TEMPLATE_THREAD * pThread
    )
    {
    STATUS status = OK;
    int key;

    key = intLock ();

    /* TODO - Activate Controller for the current thread */

    intUnlock (key);

    return (status);
    }


/*******************************************************************************
*
* templateAbort - abort the active script corresponding to the current thread
*
* Check that there is currently an active script running.  If so, set the
* SCSI Controller Abort flag which will halt the script and cause an interrupt.
*
* RETURNS: N/A
*/

LOCAL void templateAbort
    (
    SIOP* pSiop			/* ptr to controller info */
    )
    {
    int    key;

    key = intLock ();

    /* TODO - Abort the active script corresponding to the current thread */

    intUnlock (key);
    }

/*******************************************************************************
*
* templateScriptStart - start the SCSI Controller executing a script
*
* Restore the SCSI Controller register context, including the shared memory
* area, from the thread context.  Put the address of the script entry point
* into the DSP register.  If not in single-step mode, start the script.
*
* RETURNS: N/A
*/

LOCAL void templateScriptStart
    (
    SIOP*            pSiop,	 /* pointer to  SCSI Controller info */
    TEMPLATE_THREAD* pThread,	 /* ncr thread info */
    TEMPLATE_SCRIPT_ENTRY entryId	 /* routine address entry point */
    )
    {
    IMPORT ULONG templateWait[];	/* DUMMY entries to scripts */
    IMPORT ULONG templateInitStart[];
    IMPORT ULONG templateInitContinue[];
    IMPORT ULONG templateTgtDisconnect[];
    int key;

    static ULONG * templateScriptEntry [] =
	{
        templateWait,			/* wait for re-select or host cmd   */
    	templateInitStart,		/* start an initiator thread        */
    	templateInitContinue,		/* continue an initiator thread     */
	templateTgtDisconnect,     	/* disconnect a target thread       */
	};    

    key = intLock ();

    /* TODO - Restore the SCSI Controller  register context for this thread. */

    /*
     * TODO - Set the shared data address, load the script start address,
     * then start the SCSI Controller unless it's in single-step mode.
     */

    /* DUMMY CMD */
    TEMPLATE_REG_WRITE (pSiop, pCmd, (int)templateScriptEntry[entryId]);

    intUnlock (key);
    }



/*******************************************************************************
*
* templateThreadUpdate - update the thread structure for a current SCSI command
*
* Update the dynamic data (e.g. data pointers, transfer parameters) in
* the thread to reflect the latest state of the corresponding physical device.
*
* RETURNS: N/A
*
* NOMANUAL
*/

LOCAL void templateThreadUpdate
    (
    TEMPLATE_THREAD * pThread		/* thread info */
    )
    {
    SCSI_THREAD   * pScsiThread = &pThread->scsiThread;
    SCSI_CTRL     * pScsiCtrl   = pScsiThread->pScsiCtrl;
    UINT            msgOutSize;
    UINT            msgInSize;

    /*
     *  If there is an identification message, ensure ATN is asserted
     *	during (re)selection.
     */

    /* TODO - update device specific info in the thread structure */

    /* Update dynamic message in/out sizes */

    if (pScsiCtrl->msgOutState == SCSI_MSG_OUT_NONE)
	msgOutSize = 0;
    else
	msgOutSize = pScsiCtrl->msgOutLength;

    if (pScsiCtrl->msgInState != SCSI_MSG_IN_EXT_MSG_DATA)
	msgInSize = 0;
    
    else if ((msgInSize = pScsiCtrl->msgInBuf[SCSI_EXT_MSG_LENGTH_BYTE]) == 0)
	msgInSize = SCSI_EXT_MSG_MAX_LENGTH;

    /* TODO - Update commands in shared memory area */

    }

/*******************************************************************************
*
* templateThreadComplete - successfully complete execution of a client thread
*
* Set the thread status and errno appropriately, depending on whether or
* not the thread has been aborted.  Set the thread inactive, and notify
* the SCSI manager of the completion.
*
* RETURNS: N/A
*/

LOCAL void templateThreadComplete
    (
    TEMPLATE_THREAD * pThread
    )
    {
    SCSI_THREAD * pScsiThread = (SCSI_THREAD *) pThread;
    
    SCSI_DEBUG_MSG ("templateThreadComplete: thread 0x%08x completed\n",
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
    
    templateThreadStateSet (pThread, SCSI_THREAD_INACTIVE);

    scsiCacheSynchronize (pScsiThread, SCSI_CACHE_POST_COMMAND);

    scsiMgrThreadEvent (pScsiThread, SCSI_THREAD_EVENT_COMPLETED);
    }


/*******************************************************************************
*
* templateThreadDefer - defer execution of a thread
*
* Set the thread's state to INACTIVE and notify the SCSI manager of the
* deferral event.
*
* This routine is invoked when a re-selection event occurs.
*
* RETURNS: N/A
*/

LOCAL void templateThreadDefer
    (
    TEMPLATE_THREAD * pThread
    )
    {
    SCSI_THREAD * pScsiThread = (SCSI_THREAD *) pThread;
    
    SCSI_DEBUG_MSG ("templateThreadDefer: thread 0x%08x deferred\n",
		    (int) pThread, 0, 0, 0, 0, 0);

    templateThreadStateSet (pThread, SCSI_THREAD_INACTIVE);

    scsiMgrThreadEvent (pScsiThread, SCSI_THREAD_EVENT_DEFERRED);
    }
    
	
/*******************************************************************************
*
* templateThreadFail - complete execution of a thread, with error status
*
* Set the thread's status and errno according to the type of error.  Set
* the thread's state to INACTIVE, and notify the SCSI manager of the
* completion event.
*
* RETURNS: N/A
*/

LOCAL void templateThreadFail
    (
    TEMPLATE_THREAD * pThread,
    int             errNum
    )
    {
    SCSI_THREAD * pScsiThread = (SCSI_THREAD *) pThread;
    
    SCSI_DEBUG_MSG ("templateThreadFail: thread 0x%08x failed (errno = %d)\n",
		    (int) pThread, errNum, 0, 0, 0, 0);

    pScsiThread->status = ERROR;

    if (pScsiThread->state == SCSI_THREAD_ABORTING)
	pScsiThread->errNum = S_scsiLib_ABORTED;

    else
    	pScsiThread->errNum = errNum;
    
    templateThreadStateSet (pThread, SCSI_THREAD_INACTIVE);

    scsiMgrThreadEvent (pScsiThread, SCSI_THREAD_EVENT_COMPLETED);
    }
    
	
/******************************************************************************
*
* templateThreadStateSet - set the state of a thread
*
* This is really just a place-holder for debugging and possible future
* enhancements such as state-change logging.
*
* RETURNS: N/A
*/

LOCAL void templateThreadStateSet
    (
    TEMPLATE_THREAD *   pThread,		/* ptr to thread info */
    SCSI_THREAD_STATE state
    )
    {
    SCSI_THREAD * pScsiThread = (SCSI_THREAD *) pThread;
    
    SCSI_DEBUG_MSG ("templateThreadStateSet: thread 0x%08x: %d -> %d\n",
		    (int) pThread, pScsiThread->state, state, 0, 0, 0);

    pScsiThread->state = state;
    }

/*******************************************************************************
*
* templatePhaseMismatch - recover from a SCSI bus phase mismatch
*
* This routine does whatever is required to keep the pointers, counts, etc.
* used by task-level software in step when a SCSI phase mismatch occurs.
*
* The interrupt-level mismatch processing has stored the phase of the
* information transfer before the mismatch, and the number of bytes
* remaining to be transferred.  See "templateRemainderGet()".
*
* Note that the only phase mismatches supported at this level are:
*
* 1) during data in/out phases - presumably because the target has
* transferred as much data as it intends to before sending a message
* in (typically DISCONNECT or COMMAND COMPLETE).  Recovery consists
* of updating the active data pointer/count according to the number
* of data bytes actually transferred before the mismatch.
*
* 2) during a message out phase - presumably because the target does not
* understand our outgoing message and is sending a MESSAGE REJECT
* message, or similar.  No recovery is needed here - it's all done
* when the MESSAGE REJECT message has been received (see routine
* "scsiMsgOutReject()").
*
* 3) during a message in phase - presumably because we have asserted ATN
* to abort or reject an incoming message.  No recovery is needed here -
* it's done by the thread management code, which should have enough
* state information to know what to do.
*
* RETURNS: OK, or ERROR for an unsupported or invalid phase
*
* NOMANUAL
*/
LOCAL STATUS templatePhaseMismatch
    (
    TEMPLATE_THREAD  * pThread,		/* ptr to thread info           */
    int              phase,		/* bus phase before mismatch    */
    UINT             remCount		/* # bytes not yet transferred  */
    )
    {
    SCSI_THREAD *pScsiThread = (SCSI_THREAD *) pThread;
    UINT xferCount;

    /* TODO - compute nbr of bytes actually transferred. */

    xferCount = 1024 - remCount; /* DUMMY CODE */

    switch (phase)
	{
	case SCSI_DATA_IN_PHASE:
	case SCSI_DATA_OUT_PHASE:
	    pScsiThread->activeDataAddress += xferCount;
	    pScsiThread->activeDataLength  -= xferCount;
	    
	    SCSI_DEBUG_MSG ("templatePhaseMismatch: data transfer aborted "
			    "(%d bytes transferred).\n",
			    xferCount, 0, 0, 0, 0, 0);
	    break;
	    
	case SCSI_MSG_OUT_PHASE:
	    SCSI_DEBUG_MSG("templatePhaseMismatch: message out aborted "
			   "(%d of %d bytes sent).\n",
			   pScsiThread->pScsiCtrl->msgOutLength,
			   pScsiThread->pScsiCtrl->msgOutLength - remCount,
			   0, 0, 0, 0);
	    break;
	    
	case SCSI_MSG_IN_PHASE:
	    SCSI_DEBUG_MSG("templatePhaseMismatch: message in aborted "
			   "(%d bytes received).\n",
			   pScsiThread->pScsiCtrl->msgInLength,
			   0, 0, 0, 0, 0);
	    break;

	case SCSI_COMMAND_PHASE:
	case SCSI_STATUS_PHASE:
	    SCSI_ERROR_MSG ("templatePhaseMismatch: unsupported phase (%d).\n",
			    phase, 0, 0, 0, 0, 0);
	    return (ERROR);
	    
	default:
	    logMsg ("templatePhaseMismatch: invalid phase (%d).\n",
		    phase, 0, 0, 0, 0, 0);
	    return (ERROR);
        }

    return (OK);
    }

/*******************************************************************************
*
* templateInitIdentEvent - identification thread event processing 
*
* Parse the event type and handle it accordingly.  This may result in state
* changes for the thread, state variables being updated, etc.
*
* RETURNS: N/A
*/

LOCAL void templateInitIdentEvent
    (
    TEMPLATE_THREAD * pThread,
    TEMPLATE_EVENT *  pEvent
    )
    {
    SCSI_EVENT *  pScsiEvent  = (SCSI_EVENT *)  pEvent;
    SCSI_THREAD * pScsiThread = (SCSI_THREAD *) pThread;
    SCSI_CTRL *   pScsiCtrl   = pScsiThread->pScsiCtrl;

    /* Update controller msg in/out state after script completes */

    pScsiCtrl->msgOutState = pThread->nMsgOutState;
    pScsiCtrl->msgInState  = pThread->nMsgInState;
    
    /* Parse script exit status; handle as necessary */

    switch (pScsiEvent->type)
	{
	case SCSI_EVENT_RESELECTED:
    	    pScsiThread->nBytesIdent = pScsiEvent->nBytesIdent;

	    bcopy ((char *) pScsiCtrl->identBuf,
		   (char *) pScsiThread->identMsg,
		   pScsiThread->nBytesIdent);

	    templateThreadStateSet (pThread, SCSI_THREAD_IDENT_IN);

    	    templateIdentInContinue (pThread);
	    break;

	case TEMPLATE_MESSAGE_OUT_SENT:

	    /*
	     *	This will be after we have sent an "ABORT (TAG)" msg.
	     *	The target will disconnect any time; it may have already
	     *	done so, in which case we won't be able to resume the
	     *	thread, but no matter.
	     */

	    break;

	case TEMPLATE_MESSAGE_IN_RECVD:

	    /*
	     *  Continue parsing the identification message.  It
	     *  should by now be complete.
	     *
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

    	    templateIdentInContinue (pThread);
	    break;

	case TEMPLATE_SCRIPT_ABORTED:
	    SCSI_DEBUG_MSG ("templateInitIdentEvent: thread 0x%08x: aborted\n",
			    (int) pThread, 0, 0, 0, 0, 0);
	    break;

        case SCSI_EVENT_DISCONNECTED:
	    SCSI_DEBUG_MSG ("templateInitIdentEvent: thread 0x%08x:"
			    "disconnected\n",
			    (int) pThread, 0, 0, 0, 0, 0);

	    templateThreadFail (pThread, S_scsiLib_DISCONNECTED);
	    break;
	    
	case SCSI_EVENT_BUS_RESET:
    	    SCSI_DEBUG_MSG ("templateInitIdentEvent: thread 0x%08x: bus reset\n",
		    	    (int) pThread, 0, 0, 0, 0, 0);

	    /* Do not try to resume this thread.  SCSI mgr will tidy up.  */

	    templateThreadStateSet (pThread, SCSI_THREAD_INACTIVE);
	    break;

	case TEMPLATE_UNEXPECTED_DISCON:

	    /* not really unexpected after an abort message ... */

    	    SCSI_ERROR_MSG ("templateInitIdentEvent: thread 0x%08x: "
		    	    "unexpected disconnection\n",
		    	    (int) pThread, 0, 0, 0, 0, 0);

	    templateThreadFail (pThread, S_scsiLib_DISCONNECTED);
	    break;
	    
	case TEMPLATE_ILLEGAL_PHASE:
	    SCSI_ERROR_MSG ("templateInitIdentEvent: thread 0x%08x: "
			    "illegal phase requested.\n",
			    (int) pThread, 0, 0, 0, 0, 0);
	    
	    templateThreadFail (pThread, S_scsiLib_INVALID_PHASE);
	    break;

	default:
	    logMsg ("templateInitIdentEvent: invalid event type (%d)\n",
		    pScsiEvent->type, 0, 0, 0, 0, 0);
	    break;
	}
    }

    
/*******************************************************************************
*
* templateResume - resume a script corresponding to a suspended thread
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
*/

LOCAL STATUS templateResume
    (
    SIOP *           pSiop,	/* ptr to controller info          */
    TEMPLATE_THREAD* pThread,	/* ptr to thread info              */
    TEMPLATE_SCRIPT_ENTRY entryId	/* entry point of script to resume */
    )
    {
    STATUS status;
    int    key;

    /* Check validity of connection and start script if OK */

    key = intLock ();

    switch (pSiop->state)
	{
	case TEMPLATE_STATE_IDLE:
	    SCSI_INT_DEBUG_MSG ("templateResume: thread: 0x%08x:"
				" state %d -> %d\n",
			    	(int) pThread,
			    	TEMPLATE_STATE_IDLE, TEMPLATE_STATE_ACTIVE,
			    	0, 0, 0);

	    templateScriptStart (pSiop, pThread, entryId);

	    pSiop->state = TEMPLATE_STATE_ACTIVE;
	    status = OK;
	    break;

	case TEMPLATE_STATE_PASSIVE:
	case TEMPLATE_STATE_ACTIVE:
	default:
	    status = ERROR;
	    break;
	}

    intUnlock (key);

    return (status);
    }

/*******************************************************************************
*
* templateIdentInContinue - continue incoming identification
*
* Parse the message built up so far.  If it is not yet complete, do nothing.
* If the message is complete, attempt to reconnect the thread it identifies,
* and deactivate this thread (the identification thread is no longer active).
* Otherwise (identification has failed), abort the identification sequence.
*
* RETURNS: N/A
*/

LOCAL void templateIdentInContinue
    (
    TEMPLATE_THREAD * pThread
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
	    	templateThreadReconnect ((TEMPLATE_THREAD *) pNewThread);

	    	state = SCSI_THREAD_INACTIVE;
		}
	    break;

	case SCSI_IDENT_FAILED:
	    state = SCSI_THREAD_IDENT_ABORTING;
	    break;

	default:
	    logMsg ("templateIdentInContinue: invalid ident status (%d)\n",
    	    	    status, 0, 0, 0, 0, 0);
	    state = SCSI_THREAD_INACTIVE;
	    break;
	}

    if (state == SCSI_THREAD_IDENT_ABORTING)
	templateThreadAbort ((SIOP *) pScsiCtrl, pThread);

    templateThreadStateSet (pThread, state);
    }

/*******************************************************************************
*
* templateThreadReconnect - reconnect a thread
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
LOCAL void templateThreadReconnect
    (
    TEMPLATE_THREAD * pThread
    )
    {
    SCSI_THREAD * pScsiThread = (SCSI_THREAD *) pThread;
    SCSI_CTRL   * pScsiCtrl   = pScsiThread->pScsiCtrl;
    SIOP        * pSiop       = (SIOP *) pScsiCtrl;
    
    SCSI_DEBUG_MSG ("templateThreadReconnect: reconnecting thread 0x%08x\n",
		    (int) pThread, 0, 0, 0, 0, 0);

    pScsiCtrl->pThread = pScsiThread;

    /* Implied RESTORE POINTERS action: see "scsiMsgInComplete ()" */

    pScsiThread->activeDataAddress = pScsiThread->savedDataAddress;
    pScsiThread->activeDataLength  = pScsiThread->savedDataLength;

    templateThreadUpdate (pThread);

    if (templateResume (pSiop, pThread, TEMPLATE_SCRIPT_INIT_CONTINUE) != OK)
	{
	SCSI_ERROR_MSG ("templateThreadReconnect: failed to resume thread.\n",
			0, 0, 0, 0, 0, 0);

	templateThreadFail (pThread, S_scsiLib_DISCONNECTED);
	return;
	}

    templateThreadStateSet (pThread, SCSI_THREAD_ESTABLISHED);
    }


