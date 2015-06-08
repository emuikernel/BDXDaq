/* aic7880Lib.c - Adaptec 7880 SCSI Host Adapter Library File */

/* Copyright 1984-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01i,09nov01,pai  added memPartLib header file for Veloce KMEM_ALLOC
                 definition.
01h,12oct01,dat  merge from ae 1.1
01g,21nov00,jkf  SPR#62266 T3 doc update
01f,20jul00,jkf  use KMEM_ALLOC, added a AIC_7880_VIRT_TO_PHYS macro, stop
		 logging shared INTs as spurious,log valid INT only.
		 SPR#31810, 32101,32102.
01e,08feb99,fle  doc : removed some .IPs that were wrong here
01d,19aug98,fle  doc : removed the routine word from the aic7880ReadConfig
                 title line
01c,14oct97,dds  changed aic7880dFif0ThresholdSet to aic7880dFifoThresholdSet.
01b,11jul97,dds  added aic7880.h header file.
01a,10jul97,dds  written.
*/

/*
DESCRIPTION:
This is the I/O driver for the Adaptec AIC 7880 PCI Bus Master Single 
Chip SCSI Host Adapter. It is designed to work with scsi2Lib. This driver 
runs in conjunction with the HIM (Hardware Interface Module) supplied by 
Adaptec. The AIC 7880 SCSI Host Adapter driver supports the following features

.iP
Fast, Double Speed 20 MHz data transfers.
.iP
16 bit Wide Synchronous Data transfers.
.iP
Tagged Command Queueing.
.iP
Data FIFO threshold selection.
.iP
Disconnect / Reconnect support.
.iP
Multiple Initiator support.
.iP
Multiple Controller support.
.lE

In general, the SCSI system and this driver will automatically choose 
the best combination of these features to suit the target devices used. 
However, the default choices may be over-ridden by using the function 
"scsiTargetOptionsSet()" (see scsiLib).

To use this driver, enable the INCLUDE_AIC7880_SCSI component. (VxAE)

OPERATIONS OVERVIEW

The host processor initiates a SCSI I/O operation by programming a data
structure called SCB (SCSI Command Block). The SCB contains all the
relevant information needed by the Host Adapter to carry out the requested
SCSI operation. SCSI SCB's are passed to the HIM by this module which are
then sent to the AIC-7880 for execution. The AIC-7880 Sequencer or PhaseEngine
comprises the on-chip intelligence that allows the AIC-7880 to execute SCB
commands. The Sequencer is programmable and uses its own microcode program 
which is downloaded to AIC-7880 by the host at initialization.

The following is an example of how an SCB is delivered to the AIC-7880

.iP
Memory is allocated for the SCB structure and it is programmed 
with the necessary information required to execute a SCSI 
transaction.
.iP
The SCB is then sent to HIM.
.iP
The HIM pauses the Sequencer.
.iP
The Sequencer has internal registers that point to the area in 
system memory where the SCB resides.
.iP
The HIM unpauses the Sequencer.
.iP
The AIC-7880 Sequencer uses DMA to transfer the SCB into its 
internal memory.
.iP
The AIC-7880 executes the SCB.
.iP
Upon completion of the SCB command, the AIC-7880 Sequencer posts 
the pointer of the completed SCB into system memory.
.iP
The AIC-7880 generates an interupt.
.iP
The status of the completed SCB is then read by the host.

SCB PROCESSING

The AIC-7880 Sequencer uses DMA to transfer the SCB into its internal memory.
The Sequencer processes SCB's in the order they are received with new SCB's 
being started when older SCB operations are idle due to wait for selection 
or a SCSI bus disconnect. When operations for an Idle SCB reactivate, the 
sequencer scans the SCB array for the SCB corresponding to the Target/LUN 
reactivating. The Sequencer then restarts the SCB found until the next 
disconnect or SCB completion.

.SS "MAXIMUM NUMBER OF TAGGED SCB's"

The number of tagged SCB's per SCSI target that is handled by the Sequencer,
range from 1-32. The HIM supports only the External SCB Access mode. The
default number of tags handled by the Sequencer in this mode is 32. Changing
the field "Cf_MaxTagScbs" in the cfp_struct changes the maximum number of 
tagged SCB's.

.SS "MAXIMUM NUMBER OF  SCB's"

The number of SCB's that can be queued to the Sequencer, range from 1-254.
This value can be changed before calling the HIM routine "PH_GetConfig ()".
Changing the field "Cf_NumberScbs" in "cfp_struct" changes the maximum number
of SCB's to be used. The default max number of SCB's is 254. 

SYNCHRONOUS TRANSFER SUPPORT

If double speed SCSI mode is enabled, this driver supports transfer periods
of 50, 64 and 76 ns. In standard fast SCSI mode transfer periods of 100, 125,
150, 175, 200, 225, 250 and 275 are supported. Synchronous transfer parameters
for a target can be set using the SCSI library function "scsiTargetOptionsSet".

DOUBLE SPEED SCSI MODE

To enable/disable double speed SCSI mode the routine 
"aic7880EnableFast20" needs to be invoked with the following
two parameters:
.IP "(1)" 4
A pointer to the appropriate SCSI Controller structure 
.IP "(2)"
A BOOLEAN value which enables or disable double speed SCSI mode.
.LP

With double speed SCSI mode enabled the host adapter may be capable of 
transferring data at theoritcal transfer rates of 20 MB/s for an 8-bit 
device and 40 MB/s for a 16-bit device. Double Speed SCSI is disabled
by default.

DATA FIFO THRESHOLD

To set the data FIFO threshold the routine "aic7880dFifoThresholdSet"
needs to be invoked with the following two parameters:
.IP "(1)" 4
A pointer to the appropriate SCSI Controller structure
.IP "(2)"
The data FIFO threhold value.
.LP

For more information about the data FIFO threshold value refer the 
aic7880dFifoThresholdSet() routine

In order to initialize the driver from the BSP the following needs to
be done in the BSP specific routine sysScsiInit() in file sysScsi.c.

.iP
Find the SCSI Host Adapter.
.iP
Create the SCSI Controller Structure.
.iP
Connect the interrupt to Interupt Service Routine (ISR).
.iP
Enable the SCSI interupt
.LP

The following example shows the SCSI initialization sequence that need
to be done in the BSP.

.CS
STATUS sysScsiInit ()
    {
    int   busNo;          /@ PCI bus number          @/
    int   devNo;          /@ PCI device number       @/
    UWORD found = FALSE;  /@ host adapter found      @/
    int   numHa = 0;      /@ number of host adapters @/

    for (busNo=0; busNo < MAX_NO_OF_PCI_BUSES && !found; busNo++)
        for (devNo = 0; devNo < MAX_NO_OF_PCI_DEVICES; devNo++)
        {
        if ((found = sysScsiHostAdapterFind (busNo, devNo)) == HA_FOUND)

            {
            numHa++;

            /@ Create the SCSI controller @/

            if ((pSysScsiCtrl = (SCSI_CTRL *) aic7880CtrlCreate
                 (busNo, devNo, SCSI_DEF_CTRL_BUS_ID)) == NULL)
                {
                logMsg ("Could not create SCSI controller\n",
                         0, 0, 0, 0, 0, 0);
                return (ERROR);
                }

            /@ connect the SCSI controller's interrupt service routine @/

            if ((pciIntConnect (INUM_TO_IVEC (SCSI_INT_VEC), aic7880Intr,
                             (int) pSysScsiCtrl)) == ERROR)
                return (ERROR);

            /@ enable SCSI interupts @/

            sysIntEnablePIC (SCSI_INT_LVL);
            }
    return (OK);
    }
.CE

SEE ALSO: scsiLib, scsi2Lib, cacheLib,
.I "AIC-7880 Design In Handbook,"
.I "AIC-7880 Data Book,"
.I "Adaptec Hardware Interface Module (HIM) Specification,"
.pG "I/O System"
*/

#include "vxWorks.h"
#include "memLib.h"
#include "memPartLib.h"
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

#include "drv/scsi/him/him_scb.h"
#include "drv/scsi/aic7880.h"
#include "drv/scsi/him/him_equ.h"
#include "drv/scsi/him/seq_off.h"
#include "drv/scsi/him/sequence.h"
#include "drv/scsi/him/him_rel.h"

#include "him/him_init.c"
#include "him/him.c"
#include "him/himd.c"
#include "him/himdinit.c"
#include "him/himdiopt.c"
#include "him/himdopt.c"

#define SIOP_MAX_XFER_LENGTH    ((UINT) (0x00ffffff))

#define AIC_7880_VIRT_TO_PHYS(x) \
	((UINT)CACHE_DMA_VIRT_TO_PHYS(x))

typedef AIC_7880_SCSI_CTRL SIOP;

int himDebug = 0;

/* globals */

AIC_7880_SCSI_CTRL * sysScsiCtrl;

char * aic7880ScsiTaskName      = SCSI_DEF_TASK_NAME;
int    aic7880ScsiTaskOptions   = SCSI_DEF_TASK_OPTIONS;
int    aic7880ScsiTaskPriority  = SCSI_DEF_TASK_PRIORITY;
int    aic7880ScsiTaskStackSize = SCSI_DEF_TASK_STACK_SIZE;

IMPORT void PH_ScbSend (sp_struct *);
IMPORT VOIDFUNCPTR intEOI;            /* pointer to function sysIntEOI() */

/* function declarations */

VOID   aic7880EnableFast20         (SCSI_CTRL * pScsiCtrl, BOOL enable);
VOID   aic7880ScbCompleted         (sp_struct * pScb);
STATUS aic7880dFifoThresholdSet    (SCSI_CTRL * pScsiCtrl, UBYTE threshHold);
DWORD  aic7880ReadConfig           (cfp_struct * configPtr, UBYTE busNo,
				    UBYTE devNo, UBYTE regNo);
DWORD  aic7880WriteConfig          (cfp_struct * configPtr, UBYTE busNo, 
				    UBYTE devNo, UBYTE regNo, DWORD regVal);
DWORD aic7880GetNumOfBuses         ();

LOCAL STATUS aic7880ThreadInit     (SIOP * pSiop, AIC_7880_THREAD * pThread);
LOCAL BOOL aic7880ThreadAbort      (SIOP * pSiop, AIC_7880_THREAD * pThread);
LOCAL STATUS aic7880ThreadActivate (SIOP *pSiop, AIC_7880_THREAD * pThread);
LOCAL VOID   aic7880ThreadUpdate   (SIOP *pSiop, AIC_7880_THREAD * pThread);
LOCAL VOID   aic7880Activate       (AIC_7880_THREAD * pThread);
LOCAL VOID   aic7880ThreadComplete (AIC_7880_THREAD * pThread);
LOCAL VOID   aic7880ThreadFail     (AIC_7880_THREAD * pThread, int errNum);
LOCAL VOID   aic7880Event          (SIOP * pSiop, SCSI_EVENT * pScsiEvent);
LOCAL VOID   aic7880InitEvent      (AIC_7880_THREAD * pThread,
                                    SCSI_EVENT * pScsiEvent);
LOCAL VOID   aic7880ThreadEvent    (AIC_7880_THREAD * pThread,
				    SCSI_EVENT * pScsiEvent);
LOCAL VOID   aic7880ThreadStateSet (AIC_7880_THREAD * pThread,
				    SCSI_THREAD_STATE state);
LOCAL VOID   aic7880DummyHandler ();
LOCAL STATUS aic7880ScsiBusControl (SIOP * pSiop, int operation);
LOCAL VOID   aic7880XferParamsCvt  (SIOP * pSiop, UINT8 * pOffset,
				    UINT8 * pPeriod, UINT8 * pXferRate);
LOCAL STATUS aic7880XferParamsSet  (SCSI_CTRL * pScsiCtrl, UINT8 offset,
				    UINT8  period);

LOCAL STATUS aic7880ThreadParamsSet   (AIC_7880_THREAD * pThread, UINT8 offset,
				       UINT8 period);
LOCAL STATUS aic7880WideXferParamsSet (SCSI_CTRL * pScsiCtrl, UINT8 xferWidth);
LOCAL STATUS aic7880XferParamsQuery   (SCSI_CTRL * pScsiCtrl, UINT8 * pOffset, 
				       UINT8 *pPeriod);

/*******************************************************************************
*
* aic7880CtrlCreate - create a control structure for the AIC 7880
*
* This routine creates an AIC_7880_SCSI_CTRL structure and must be called 
* before using the SCSI Host Adapter chip. It must be called exactly once for 
* a specified Host Adapter. 
*
* RETURNS: A pointer to the AIC_7880_SCSI_CTRL structure, or NULL if memory
* is unavailable or there are invalid parameters.
*/

AIC_7880_SCSI_CTRL * aic7880CtrlCreate 
    (
    int busNo,        /* PCI bus Number */
    int devNo,        /* PCI device Number */
    int scsiBusId     /* SCSI Host Adapter Bus Id */
    )
    {
    cfp_struct         * pCfpStruct;   /* ptr to HA confiuration struct    */
    AIC_7880_SCSI_CTRL * aic7880Ctrl;  /* ptr to aic7880 Controller struct */
    SCSI_CTRL          * pScsiCtrl;    /* ptr to generic SCSI Controller   */
    int                  nBytes;       /* number of bytes                  */
    int                  status;       /* status information               */
    int                  i;

    /* create the generic SCSI controller structure */

    if ((aic7880Ctrl = (AIC_7880_SCSI_CTRL *) \
	KMEM_ALLOC((sizeof (AIC_7880_SCSI_CTRL)))) == NULL)
	{
	SCSI_DEBUG_MSG ("Could not allocate memory for SCSI controlle\n",
		       0, 0, 0, 0, 0, 0);
	return (NULL);
	}
    
    /* create the host adapter configuration structure */

    if ((pCfpStruct = (cfp_struct *)KMEM_ALLOC(sizeof (cfp_struct))) == NULL)
        {
	SCSI_DEBUG_MSG ("Could not allocate HIM configuration structure\n",
		       0, 0, 0, 0, 0, 0);
        return (NULL);
        }

    nBytes = sizeof (cfp_struct);
    bzero ((char *) pCfpStruct, nBytes);

    /* 
     * the bus number and the device number are required by the
     * HIM before the call to "PH_GetConfig".
     */

    pCfpStruct->Cf_BusNumber    = busNo;
    pCfpStruct->Cf_DeviceNumber = devNo;

    /* 
     * Get host adapter configuration information for cfp struct.
     * Upon return the pCfpStruct contains the AIC-7880 host adapter
     * configuration information. (e.g. base address, Id, rev level etc.)
     */

    PH_GetConfig (pCfpStruct);

    aic7880Ctrl->aic7880CfpStruct = pCfpStruct;
    sysScsiCtrl = aic7880Ctrl;

    /* allocate memory for the host adapter data structure */

    nBytes = pCfpStruct->Cf_HimDataSize;

    if ((pCfpStruct->CFP_HaDataPtr = 
	(hsp_struct *) KMEM_ALLOC(nBytes * sizeof (char))) == NULL)
	SCSI_DEBUG_MSG ("Could not allocate memory HIM data structure\n",
		       0, 0, 0, 0, 0, 0);

    /* 
     * virtual and physical pointers to the host adapter data structure
     * are the same. 
     */

    pCfpStruct->Cf_HaDataPhy = \
	AIC_7880_VIRT_TO_PHYS (pCfpStruct->CFP_HaDataPtr);

    /* 
     * Initialize the SCSI Host adapter, by reseting the SCSI bus and
     * suppressing wide/sync data transfer negotiations initially.
     */

    pCfpStruct->CFP_InitNeeded = TRUE;
    pCfpStruct->CFP_ResetBus = TRUE;
    pCfpStruct->CFP_SuppressNego = TRUE;
    pCfpStruct->Cf_ScsiId = scsiBusId;

    /* 
     * "PH_InitHA" initializes the SCSI Host Adapter, pauses the sequencer
     * and  downloads the sequencer code to sequencer RAM which resides on 
     * the AIC-7880. The sequencer code is then un-paused and the AIC-7880
     * Host Adapter interupt is enabled.
     */

    if ((status = PH_InitHA (pCfpStruct)) == 0)
	SCSI_DEBUG_MSG ("Host Adapter Initialization Successful\n",
		       0, 0, 0, 0, 0, 0);

    /* initialize the negotiation options for each target */

    for (i = 0; i < pCfpStruct->Cf_MaxTargets; i++)
        pCfpStruct->Cf_ScsiOption [i] = 0;

    pScsiCtrl = (SCSI_CTRL *) aic7880Ctrl;

    /* validate and set bus ID */    

    pScsiCtrl->scsiCtrlBusId = pCfpStruct->Cf_ScsiId;

    pScsiCtrl->eventSize  = sizeof (SCSI_EVENT);
    pScsiCtrl->threadSize = sizeof (AIC_7880_THREAD);
    pScsiCtrl->maxBytesPerXfer = SIOP_MAX_XFER_LENGTH;

    /* fill in virtual functions used by SCSI library */

    pScsiCtrl->scsiEventProc       = (VOIDFUNCPTR) aic7880Event;
    pScsiCtrl->scsiTransact        = (FUNCPTR)     scsiTransact;
    pScsiCtrl->scsiThreadInit      = (FUNCPTR)     aic7880ThreadInit;
    pScsiCtrl->scsiThreadAbort     = (FUNCPTR)     aic7880ThreadAbort;
    pScsiCtrl->scsiThreadActivate  = (FUNCPTR)     aic7880ThreadActivate;
    pScsiCtrl->scsiBusControl      = (FUNCPTR)     aic7880ScsiBusControl;
    pScsiCtrl->scsiSpecialHandler  = (FUNCPTR)     aic7880DummyHandler;
    pScsiCtrl->scsiXferParamsQuery = (FUNCPTR)     aic7880XferParamsQuery;
    pScsiCtrl->scsiXferParamsSet   = (FUNCPTR)     aic7880XferParamsSet;
    pScsiCtrl->scsiWideXferParamsSet   = (FUNCPTR) aic7880WideXferParamsSet;

    /* SCSI Host Adapter supports 16 bit wide bus */

    pScsiCtrl->wideXfer = TRUE;

    /* fill in the generic SCSI info for this controller */

    scsiCtrlInit (&aic7880Ctrl->scsiCtrl); 

    pScsiCtrl->scsiMgrId = taskSpawn (aic7880ScsiTaskName,
                                      aic7880ScsiTaskPriority,
                                      aic7880ScsiTaskOptions,
                                      aic7880ScsiTaskStackSize,
                                      (FUNCPTR) scsiMgr,
                                      (int) aic7880Ctrl,
				      0, 0, 0, 0, 0, 0, 0, 0, 0); 

    return (aic7880Ctrl);
    }

/*******************************************************************************
*
* aic7880ThreadInit - initialize a client thread structure
*
* Initialize the fixed data for a thread. Memory for the SCB (SCSI Command
* Block) structure within the thread is allocated and initialized. Memory
* for the scatter / gather  list structure is also allocated in this routine.
* This routine is called once when a thread structure is first created. Memory
* for a thread structure is allocated in function scsiThreadArrayCreate in 
* scsi2Lib.c
*
* RETURNS: OK, or ERROR if memory is unavailable.
*/

LOCAL STATUS aic7880ThreadInit
    (
    SIOP            * pSiop,   /* ptr to aic7880 Controller struct */
    AIC_7880_THREAD * pThread  /* ptr to an aic7880 thread structure */
    )
    {
    sp_struct * pScb;
    SEG_PTR   * segPtr;
    int nBytes;

    if (scsiThreadInit (&pThread->scsiThread) != OK)
        return (ERROR);

    /* allocate a SCSI Command Block structure */

    if ((pScb = (sp_struct *) KMEM_ALLOC(sizeof (sp_struct))) == NULL)
        {
        SCSI_DEBUG_MSG ("Could not allocate memory for sp_struct\n",
		       0, 0, 0, 0, 0, 0);
        return (ERROR);
        }

    nBytes = sizeof (sp_struct);
    bzero ((char *) pScb, nBytes);

    /* driver can determine which SCSI thread belongs to the SCB */
 
    pScb->pThread = pThread;

    /* allocate memory for the scatter/gather list */

    if ((segPtr = (SEG_PTR *) KMEM_ALLOC(sizeof (SEG_PTR))) == NULL)
	{
	SCSI_DEBUG_MSG ("Could not allocate memory for Scatter/Gather list\n",
                       0, 0, 0, 0, 0, 0);
        return (ERROR);
        }

    /* initialize the thread pointers to the SCB and the scatter/gather list */

    pThread->pScb = pScb;
    pThread->segPtr = segPtr;

    return (OK);
    }

/*******************************************************************************
*
* aic7880ThreadActivate - activate a SCSI connection for an initiator thread
*
* This routine activates the client thread after updating the contents of the
* SCB with information needed to execute a SCSI transaction. Cache Coherency
* is ensured prior to thread execution. Sync/Wide Xfer parameters are 
* re-negotiated if neccessary prior to the activation of each thread. 
* After the SCSI thread has been activated the state of the thread is set to 
* reflect the current status.
*
* RETURNS: OK or ERROR if unable to activate thread.
*/

LOCAL STATUS aic7880ThreadActivate
    (
    SIOP            * pSiop,            /* ptr to controller info */
    AIC_7880_THREAD * pThread           /* ptr to thread info     */
    )
    {
    SCSI_CTRL   * pScsiCtrl   = (SCSI_CTRL *)   pSiop;
    SCSI_THREAD * pScsiThread = (SCSI_THREAD *) pThread;
    SCSI_TARGET * pScsiTarget  = pScsiThread->pScsiTarget;

    SCSI_DEBUG_MSG ("aic7880ThreadActivate: thread 0x%08x: activating\n",
                    (int) pThread, 0, 0, 0, 0, 0);

    scsiCacheSynchronize (pScsiThread, SCSI_CACHE_PRE_COMMAND);

    aic7880ThreadUpdate (pSiop, pThread);

    /* 
     * set the thread pointer in the generic SCSI Controller to 
     * point to the thread being activated.
     */

    pScsiCtrl->pThread = pScsiThread;
    pSiop->pHwThread   = pThread;

    scsiWideXferNegotiate (pScsiCtrl, pScsiTarget, WIDE_XFER_NEW_THREAD);
    scsiSyncXferNegotiate (pScsiCtrl, pScsiTarget, SYNC_XFER_NEW_THREAD); 

    aic7880Activate (pThread);

    aic7880ThreadStateSet (pThread, SCSI_THREAD_ESTABLISHED);

    return (OK);
    }

/*******************************************************************************
*
* aic7880ThreadUpdate - update the thread structure for a current SCSI command
*
* This routine fills up the SCB (SCSI Command Block) with the information 
* needed by the Sequencer to execute a SCSI transaction.
*
* RETURNS: N/A
*/

LOCAL VOID aic7880ThreadUpdate 
    (
    SIOP            * pSiop,     /* ptr to controller info */
    AIC_7880_THREAD * pThread    /* thread info */
    )
    {
    SCSI_THREAD   * pScsiThread  = (SCSI_THREAD *) pThread;
    SCSI_PHYS_DEV * pScsiPhysDev = pScsiThread->pScsiPhysDev;
    SCSI_TARGET   * pScsiTarget  = pScsiThread->pScsiTarget;
    UINT            busId        = pScsiTarget->scsiDevBusId;
    UINT            devLun       = pScsiPhysDev->scsiDevLUN;

    sp_struct * pScb;   /* pointer to the SCSI Command Block */

    pScb = pThread->pScb; 

    /*
     * fill in the Scatter/Gather list Segment pointer with 
     * the address of the first byte to be transferred and
     * the length of the data to be transferred during the
     * data phase of the SCSI command.
     */

    pThread->segPtr->dataPtr = \
	AIC_7880_VIRT_TO_PHYS(pScsiThread->dataAddress);

    pThread->segPtr->dataLen = pScsiThread->dataLength;

    /* 
     * virtual pointer to the config data structure associated
     * with the AIC 7880 to which the SCB is loaded.
     */
    
    pScb->Sp_config.ConfigPtr = (struct cfp *) \
	AIC_7880_VIRT_TO_PHYS(pSiop->aic7880CfpStruct);

    /* internal SCB Access SCSI command */

    pScb->Sp_control.Cmd = EXEC_SCB; 

    pScb->SP_Next     = 0;
    pScb->SP_ResCnt   = 0;

    pScb->SP_HaStat   = 0;
    pScb->SP_TargStat = 0;

    /* disable automatic Req Sense */

    pScb->Sp_control.AutoSense = 0;

    /* do not report underrun as an error */

    pScb->Sp_control.NoUnderrun = 1;

    /*
     * indicate the specific target/Logical unit for which
     * the SCB is to be executed.
     */

    pScb->SP_Tarlun = (busId << 4) | devLun;

    /* 
     * enable or disable disconnects and command queue 
     * tagging. Also set the appropriate tag type 
     */

    if ((pScsiThread->tagType == SCSI_TAG_SIMPLE) ||
	(pScsiThread->tagType == SCSI_TAG_ORDERED) ||
	(pScsiThread->tagType == SCSI_TAG_HEAD_OF_Q))
	{
	if (pScsiThread->tagType == SCSI_TAG_SIMPLE)
	    pScb->SP_TagType = SIMPLE_QUEUE_TAG;
	else if (pScsiThread->tagType == SCSI_TAG_ORDERED)
	    pScb->SP_TagType = ORDERED_QUEUE_TAG;
	else if (pScsiThread->tagType == SCSI_TAG_HEAD_OF_Q)
	    pScb->SP_TagType = HEAD_OF_QUEUE_TAG;

	pScb->SP_TagEnable = 1;
	if (pScsiTarget->disconnect)
	    pScb->SP_DisEnable = 1;
	else
	    pScb->SP_DisEnable = 0;
	}
    else
	{
	pScb->SP_TagEnable = 0;
        pScb->SP_DisEnable = 0;
	}

    /* 
     * A value of 1 indicates that the data to be transferred from
     * host memory is contiguous. In VxWorks since there is a one 
     * to one correspondence between virtual and physical memory and
     * this field is set to one for all data transfers.
     */

    if (pScsiThread->dataLength)
	pScb->SP_SegCnt = 1;
    else
	pScb->SP_SegCnt = 0;

    /* 
     * This field indicates to the sequencer whether the data to be
     * transferred to or from host memory is contiguous. This bit
     * should be set to one if the SegCnt field is greater than one
     */

    if (pScb->SP_SegCnt > 1)
	pScb->SP_RejectMDP = 1;
    else
	pScb->SP_RejectMDP = 0;

    pScb->SP_SegPtr = \
	AIC_7880_VIRT_TO_PHYS(pThread->segPtr);

    /* fill in the command descriptor block */

    bcopy (pScsiThread->cmdAddress, pScb->Sp_CDB, pScsiThread->cmdLength);

    /* 
     * set the command length and the command pointer fields to 
     * point to the appropriate SCSI command.
     */

    pScb->SP_CDBLen = (DWORD) pScsiThread->cmdLength;
    pScb->SP_CDBPtr = AIC_7880_VIRT_TO_PHYS(pScb->Sp_CDB);

    }

/******************************************************************************
*
* aic7880Activate - activate an SCB corresponding to a new thread
*
* This routine activates the SCB corresponding to the new thread and then 
* calls the HIM function PH_ScbSend with a pointer to the SCB. If the HIM is 
* busy the SCB is queued for later execution, otherwise it begins execution
* of the SCB immediately.
*
* RETURNS: N/A
*/

LOCAL VOID aic7880Activate
    (
    AIC_7880_THREAD * pThread  /* ptr to thread info */
    )
    {
    sp_struct * pScb;   /* pointer to the SCSI Command Block */

    pScb = pThread->pScb;

    /* send the thread for execution to the Sequencer */
    PH_ScbSend (pScb);
    }

/******************************************************************************
*
* aic7880ThreadStateSet - set the state of a thread
*
* This routine sets the state of the current thread. The SCSI Thread
* maintains only four states for this driver SCSI_THREAD_WAITING, 
* SCSI_THREAD_ESTABLISHED, SCSI_THREAD_INACTIVE and SCSI_THREAD_ABORTING. 
* Disconnects / Reconnects are handled by the Sequencer.
*
* RETURNS: N/A
*/

LOCAL VOID aic7880ThreadStateSet
    (
    AIC_7880_THREAD   *  pThread,          /* ptr to thread info */
    SCSI_THREAD_STATE    state             /* thread state */
    )
    {
    SCSI_THREAD * pScsiThread = (SCSI_THREAD *) pThread;

    SCSI_DEBUG_MSG ("aic78800ThreadStateSet: thread 0x%08x: %d -> %d\n",
                    (int) pThread, pScsiThread->state, state, 0, 0, 0);

    pScsiThread->state = state;
    }

/*******************************************************************************
*
* aic7880Intr - interrupt service routine for the SIOP
*
* This routine is invoked when an aic7880 Host Adapter interrupt occurs.
* Although most interrupts are due to a Command Completed interrupt, the
* Sequencer code could also interrupt the host to handle abnormal SCSI phases
* and errors. This routine calls the HIM "PH_IntHandler" routine with the 
* cfp_struct pointer associated with appropriate AIC-7880 passed in. The HIM
* handles the interrupt and returns with a status. The function then issues
* an EOI if the returned status from "PH_IntHandler" indicates that it handled
* the interrupt. In case of shared IRQ's the returned status may indicate that
* no interupt was handled for this host adapter.
*
* RETURNS: N/A.
*
* NOMANUAL
*/

VOID aic7880Intr 
    (
    SIOP * pSiop    /* ptr to controller info */
    )
    {
    cfp_struct      * pCfpStruct;
    UBYTE             intrStatus;

    pCfpStruct = pSiop->aic7880CfpStruct;

    intrStatus = PH_IntHandler (pCfpStruct);

    intrStatus &= INTMASK;

    /* send  EOI (End of Interrupt) signal. */

     if (intrStatus)
	SCSI_DEBUG_MSG ("aic7880Intr pSiop = 0x%lx\n",(int) pSiop, 
			0, 0, 0, 0, 0);
    }    

/*******************************************************************************
*
* aic7880ScbCompleted - successfully completed execution of a client thread
*
* This routine is called from within the context of the ISR. The HIM calls
* this routine passing in the pointer of the of the completed SCB. This
* routine sets the thread status, handles the completed SCB and returns
* program control back to the HIM which then returns from the PH_IntHandler 
* routine.
*
* This routine could be called more than once from the same PH_IntHandler
* call. Each call to this routine indicates the completion of an SCB. For
* each SCB completed, this routine sets the event type and calls the 
* appropriate AIC-7880 event handler routines which sets the SCSI Controller,
* SCSI Physical Device and SCSI Thread, state variables appropriately.
* This routine also handles synchronization with the SCSI Manager so that
* the next runnable thread can be scheduled for execution.
*
* RETURNS: N/A
*
*/

VOID aic7880ScbCompleted 
    (
    sp_struct * pScb  /* ptr to completed SCSI Command Block */
    )
    {
    AIC_7880_THREAD * pThread = pScb->pThread;
    SCSI_THREAD     * pScsiThread = (SCSI_THREAD *) pThread;
    SCSI_THREAD     * pSavedThread;
    SCSI_CTRL       * pScsiCtrl = pScsiThread->pScsiCtrl;
    SIOP            * pSiop = (SIOP *) pScsiCtrl;

    SCSI_EVENT        pScsiEvent;
    BOOL              notify    = TRUE;
    DWORD             cmdStatus;         /* SCB command status */
    DWORD             haStatus;          /* host adapter status */

    /* update the status of the SCSI thread */

    pScsiThread->status = (STATUS) pScb->SP_TargStat;
    *pScsiThread->statusAddress = pScsiThread->status;

    /* save the current H/W thread that may have been activated */
    
    pSavedThread = (SCSI_THREAD *) pSiop->pHwThread;

    /* 
     * set the generic SCSI C0ntrollers thread to point to the
     * thread that has just compeleted execution. 
     */

    pScsiCtrl->pThread = pScsiThread;
    
    /* check the SCB command status */
    
    cmdStatus = pScb->SP_Stat;

    switch (cmdStatus) 
	{
	case AIC_7880_CMD_COMPLETE:
	    pScsiEvent.type = AIC_7880_CMD_COMPLETE;
            break;
	    
        case AIC_7880_CMD_COMP_WITH_ERROR:
	    haStatus = pScb->SP_HaStat;

	    switch (haStatus)
		{
		case AIC_7880_NO_STATUS:
		    SCSI_DEBUG_MSG ("No Status\n", 0, 0, 0, 0, 0, 0);
		    pScsiEvent.type = AIC_7880_CMD_COMPLETE;
		    break;

		case AIC_7880_CMD_ABORT:
		case AIC_7880_CMD_ABORT_BY_HA:
		    SCSI_DEBUG_MSG ("SCSI Command Abort\n", 0,0,0,0,0,0);
		    pScsiEvent.type = AIC_7880_CMD_COMPLETE;
		    break;

		case AIC_7880_UNEXPECTED_BUS_FREE:
		    SCSI_DEBUG_MSG ("Unexpected Bus Free\n", 0,0,0,0,0,0);
		    pScsiEvent.type = AIC_7880_SCSI_BUS_RESET;
		    break;
			
		case AIC_7880_PHASE_MISMATCH:
		    SCSI_DEBUG_MSG ("Phase Mismatch occured\n", 0,0,0,0,0,0);
		    pScsiEvent.type = AIC_7880_SCSI_BUS_RESET;
		    break;

		case AIC_7880_HA_HW_ERROR:
		    SCSI_DEBUG_MSG ("Host Adapter H/W error\n", 0,0,0,0,0,0);
		    pScsiEvent.type = AIC_7880_SCSI_BUS_RESET;
		    break;
			  
		case AIC_7880_BUS_RESET:
		case AIC_7880_BUS_RESET_OTHER_DEV:
		    SCSI_DEBUG_MSG ("SCSI Bus Reset\n", 0,0,0,0,0,0);
		    pScsiEvent.type = AIC_7880_SCSI_BUS_RESET;
		    break;

		case AIC_7880_SELECT_TIMEOUT:
		    SCSI_DEBUG_MSG ("Device Selction Timeout\n", 0,0,0,0,0,0);
		    pScsiEvent.type = AIC_7880_SELECT_TIMEOUT;
		    break;

		case AIC_7880_REQ_SENSE_FAILED:
		    SCSI_DEBUG_MSG ("Request Sense Failed\n",0,0,0,0,0,0);
		    notify = FALSE;
		    break;
		default:
		    SCSI_DEBUG_MSG ("aic7880Intr: unexpt'd host adap'r intr\n",
				   0,0,0,0,0,0);
		    break;
		}
	    break;
	    
	default:
	    notify = FALSE;
            SCSI_DEBUG_MSG ("aic7880ScbCompleted: unexpected interupt\n", 
			    0, 0, 0, 0, 0, 0); 
            break; 
        }

    if (notify)
	{
	aic7880Event (pSiop, &pScsiEvent);
	semGive (pScsiCtrl->actionSem);
	}
    
    /* restore the current H/W thread */

    pScsiCtrl->pThread = pSavedThread;
    }

/*******************************************************************************
*
* aic7880Event - AIC 788 SCSI controller event processing routine
*
* Parse the event type and act accordingly.  Controller-level events are
* handled within this function, and the event is then passed to the completed
* thread for thread-level processing. The SCSI Controllers state variables
* are set to indicate if the controller is connected or disconnected. When
* a thread is activated the Controllers state is set to CONNECTED and when
* the thread has completed execution or if a SCSI Bus Reset has occured the
* Controllers state is set to DISCONNECTED.
*
* RETURNS: N/A
*/

LOCAL VOID aic7880Event
    (
    SIOP       * pSiop,         /* ptr to controller info */
    SCSI_EVENT * pScsiEvent     /* ptr to SCSI EVENT      */
    )
    {
    SCSI_CTRL       * pScsiCtrl  = (SCSI_CTRL *)  pSiop;
    AIC_7880_THREAD * pThread    = (AIC_7880_THREAD *) pScsiCtrl->pThread;

    SCSI_DEBUG_MSG ("aic7880Event: received event %d (thread = 0x%08x)\n",
                     pScsiEvent->type, (int) pThread, 0, 0, 0, 0);

    switch (pScsiEvent->type)
	{
	case AIC_7880_CMD_COMPLETE:
	case AIC_7880_SELECT_TIMEOUT:
            pScsiCtrl->peerBusId = NONE;
            pScsiCtrl->pThread   = 0;
            scsiMgrCtrlEvent (pScsiCtrl, SCSI_EVENT_DISCONNECTED);
            break;

        case AIC_7880_SCSI_BUS_RESET:
            pScsiCtrl->peerBusId = NONE;
            pScsiCtrl->pThread   = 0;
            scsiMgrBusReset (pScsiCtrl);
            break;
	    
        default:
	    logMsg ("aic7880Event: invalid event type.\n", 0, 0, 0, 0, 0, 0);
            break;
	}

    /* If there's a thread on the controller, forward the event to it */

    if (pThread != 0)
        aic7880ThreadEvent (pThread, pScsiEvent);
    }

/*******************************************************************************
*
* aic7880ThreadEvent - AIC 7880 thread event processing routine
*
* Forward the event to the proper handler for the thread's current role.
*
* RETURNS: N/A
*/

LOCAL VOID aic7880ThreadEvent
    (
    AIC_7880_THREAD * pThread,    /* ptr to aci7880 thread info */
    SCSI_EVENT      * pScsiEvent  /* ptr to SCSI EVENT */
    )
    {
    SCSI_THREAD * pScsiThread = (SCSI_THREAD *) pThread;

    switch (pScsiThread->role)
        {
        case SCSI_ROLE_INITIATOR:
            aic7880InitEvent (pThread, pScsiEvent);
            break;

        default:
            logMsg ("aic7880ThreadEvent: thread 0x%08x: invalid role (%d)\n",
                    (int) pThread, pScsiThread->role, 0, 0, 0, 0);
            break;
	}
    }

/*******************************************************************************
*
* aic7880InitEvent - AIC 7880 initiator thread event processing routine
*
* Parse the event type and handle it accordingly.  This may result in state
* changes for the thread, state variables being updated, etc.
*
* RETURNS: N/A
*/

LOCAL VOID aic7880InitEvent
    (
    AIC_7880_THREAD * pThread,    /* ptr to aci7880 thread info */
    SCSI_EVENT      * pScsiEvent  /* ptr to SCSI EVENT */

    )
    {
    switch (pScsiEvent->type)
	{
        case AIC_7880_CMD_COMPLETE:
            aic7880ThreadComplete (pThread);
            break;

	case AIC_7880_SELECT_TIMEOUT:
            SCSI_ERROR_MSG ("aic7880InitEvent: thread 0x%08x: timeout.\n",
                            (int) pThread, 0, 0, 0, 0, 0);
	    
	    aic7880ThreadFail (pThread, S_scsiLib_SELECT_TIMEOUT);
	    break;

        default:
            logMsg ("aic7880InitEvent: invalid event type (%d)\n",
                     pScsiEvent->type, 0, 0, 0, 0, 0);
            break;
	}
    }

/*******************************************************************************
*
* aic7880DummyHandler - used by scsi2Lib.c to do special handling 
*
* This routine is used by scsiMgrLib.c and scsi2Lib.c to perform certain
* functions specific to the AIC-7880 driver. Wide / Synchronous transfer
* negotiations and thread activation are handled differently by scsi2Lib for 
* this driver.
*
* RETURNS N/A
*/

LOCAL VOID aic7880DummyHandler
    (
    )
    {

    }

/*******************************************************************************
*
* aic7880ThreadComplete - successfully complete execution of a client thread
*
* Set the thread status and errno appropriately, depending on whether or
* not the thread has been aborted.  Set the thread inactive, and notify
* the SCSI manager of the completion.
*
* RETURNS: N/A
*/

LOCAL VOID aic7880ThreadComplete
    (
    AIC_7880_THREAD * pThread    /* ptr to aci7880 thread info */
    )
    {
    SCSI_THREAD * pScsiThread = (SCSI_THREAD *) pThread;

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

    aic7880ThreadStateSet (pThread, SCSI_THREAD_INACTIVE);

    scsiCacheSynchronize (pScsiThread, SCSI_CACHE_POST_COMMAND);

    scsiMgrThreadEvent (pScsiThread, SCSI_THREAD_EVENT_COMPLETED);
    }

/*******************************************************************************
*
* aic7880ThreadFail - complete execution of a thread, with error status
*
* Set the thread's status and errno according to the type of error.  Set
* the thread's state to INACTIVE, and notify the SCSI manager of the
* completion event.
*
* RETURNS: N/A
*/

LOCAL VOID aic7880ThreadFail
    (
    AIC_7880_THREAD * pThread,   /* ptr to aci7880 thread info */
    int               errNum     /* error number */
    )
    {
    SCSI_THREAD * pScsiThread = (SCSI_THREAD *) pThread;

    SCSI_DEBUG_MSG ("aic7880ThreadFail: thread 0x%08x failed (errno = %d)\n",
                    (int) pThread, errNum, 0, 0, 0, 0);

    pScsiThread->status = ERROR;

    if (pScsiThread->state == SCSI_THREAD_ABORTING)
        pScsiThread->errNum = S_scsiLib_ABORTED;

    else
        pScsiThread->errNum = errNum;

    aic7880ThreadStateSet (pThread, SCSI_THREAD_INACTIVE);

    scsiMgrThreadEvent (pScsiThread, SCSI_THREAD_EVENT_COMPLETED);
    }

/*******************************************************************************
*
* aic7880ScsiBusControl - low-level SCSI bus control operations
*
* Currently supports only the SCSI_BUS_RESET operation. After a SCSI Bus
* Reset has occured the transfer parameters are re negotiated when a
* new SCSI thread begins execution.
*
* RETURNS: OK, or ERROR if an invalid operation is requested.
*/

LOCAL STATUS aic7880ScsiBusControl
    (
    SIOP * pSiop,           /* ptr to controller info                   */
    int    operation        /* bitmask for operation(s) to be performed */
    )
    {
    SCSI_CTRL       * pScsiCtrl  = (SCSI_CTRL *)  pSiop;
    cfp_struct      * configPtr;
    int               status;
    int               i;

    if ((operation & ~SCSI_BUS_RESET) != 0)
        return (ERROR);
    
    configPtr = pSiop->aic7880CfpStruct;

    if (operation & SCSI_BUS_RESET)
        if ((status = PH_Special (HARD_HA_RESET, configPtr, NULL))
	    != OK)
	    return (ERROR);

    for (i = 0; i < configPtr->Cf_MaxTargets; i++)
	configPtr->Cf_ScsiOption [i] = 0;
    
    scsiMgrBusReset (pScsiCtrl);

    return (OK);
    }

/*******************************************************************************
*
* aic7880XferParamsQuery - get synchronous transfer parameters
*
* Query the AIC 7880 and see if it is capable of handling the specified
* transfer parameters. If the AIC 7880 does not support the specified 
* period and offset this routine returns the offset and period values
* the AIC 7880 is capable of handling.
*
* RETURNS: OK
*/

LOCAL STATUS aic7880XferParamsQuery
    (
    SCSI_CTRL * pScsiCtrl,               /* ptr to controller info       */
    UINT8     * pOffset,                 /* max REQ/ACK offset  [in/out] */
    UINT8     * pPeriod                  /* min transfer period [in/out] */
    )
    {
    UINT8 unused;
    
    (VOID) aic7880XferParamsCvt ((SIOP *) pScsiCtrl, pOffset, pPeriod,
				 &unused);

    return (OK);
    }

/*******************************************************************************
*
* aic7880XferParamsCvt - convert transfer period from SCSI to AIC 7880 units.
*
* Given a "suggested" REQ/ACK offset and transfer period (in SCSI units of
* 4 ns), return the nearest offset and transfer period the AIC 7880 is
* capable of using.
*
* An offset of zero specifies asynchronous transfer, in which case the period
* is irrelevant.  Otherwise, the offset specified is used if it lies within
* the permissible range allowed by the AIC 7880.
*
* The transfer period is normally rounded towards longer periods if the AIC
* 7880 is not capable of using the exact specified value.
*
* RETURNS: N/A.
*
* NOMANUAL
*/

LOCAL VOID aic7880XferParamsCvt
    (
    FAST SIOP  * pSiop,            /* ptr to controller info            */
    FAST UINT8 * pOffset,          /* REQ/ACK offset                    */
    FAST UINT8 * pPeriod,          /* xfer period, SCSI units (x 4 ns)  */
    FAST UINT8 * pXferRate         /* corresponding Sync Xfer Reg value */
    )
    {
    cfp_struct * configPtr = pSiop->aic7880CfpStruct;
    UINT  offset  = (UINT) * pOffset;
    UINT  period  = (UINT) * pPeriod;

    /* asynchronous xfer requested */

    if (offset == SCSI_SYNC_XFER_ASYNC_OFFSET)
        period = 0;

    else
        {
        SCSI_DEBUG_MSG ("aic7880XferParamsCvt: requested: "
			"offset = %d, period = %d\n",
			 offset, period, 0, 0, 0, 0);
	
	/* convert to nano seconds */

	period *= 4;
	
	if (configPtr->CFP_EnableFast20)
	    {
	    if (period <= 50)
		{
		period = AIC_7880_DBLSPD_50;
		*pXferRate = 0x00;
		}
	    else if ((period > 50) && (period <= 64))
		{
		period = AIC_7880_DBLSPD_64;
		*pXferRate = 0x10;
		}
	    else if (period > 65)
		{
		period = AIC_7880_DBLSPD_75;
		*pXferRate = 0x20;
		}
	    }
	
	else
	    {
	    /* standard fast SCSI mode */	
     	    
	    if (period <= 100)
		{
		period    = AIC_7880_PERIOD_100;
		*pXferRate = 0x00;
		}

	    else if ((period > 100) && (period <= 125))
		{
		period = AIC_7880_PERIOD_125;
		*pXferRate = 0x10;
		}

	    else if ((period > 125) && (period <= 150))
		{
		period = AIC_7880_PERIOD_150;
		*pXferRate = 0x20;
		}

	    else if ((period > 150) && (period <= 175))
		{
		period = AIC_7880_PERIOD_175;
		*pXferRate = 0x30;
		}
	    
	    else if ((period > 175) && (period <= 200))
		{
		period = AIC_7880_PERIOD_200;
		*pXferRate = 0x40;
		}

	    else if ((period > 200) && (period <= 225))
		{
		period = AIC_7880_PERIOD_225;
		*pXferRate = 0x50;
		}
	    
	    else if ((period > 225) && (period <= 250))
		{
		period = AIC_7880_PERIOD_250;
		*pXferRate = 0x60;
		}
	    
	    else if (period > 250)
		{
		period = AIC_7880_PERIOD_275;
		*pXferRate = 0x70;
		}
	    }

	/* adjust the synchronous offset to fit the allowable range */
	    
	if (offset < AIC_7880_MIN_REQ_ACK_OFFSET)
	    offset = AIC_7880_MIN_REQ_ACK_OFFSET;
	
	else if (offset > AIC_7880_MAX_REQ_ACK_OFFSET)
	    offset = AIC_7880_MAX_REQ_ACK_OFFSET;
	
	}

    SCSI_DEBUG_MSG ("aic7880XferParamsCvt: converted to: "
		    "offset = %d, period = %d\n",
  		     offset, period, 0,0, 0, 0);
    
    *pOffset   = offset;
    *pPeriod   = period;
    }

/*******************************************************************************
*
* aic7880XferParamsSet - set transfer parameters
*
* Set the transfer parameters specific to a thread.
*
* Transfer period is specified in SCSI units (multiples of 4 ns).  An offset
* of zero specifies asynchronous transfer.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS aic7880XferParamsSet
    (
    SCSI_CTRL * pScsiCtrl,		/* ptr to controller info */
    UINT8       offset,			/* max REQ/ACK offset     */
    UINT8       period			/* min transfer period    */
    )
    {
    AIC_7880_THREAD * pThread = (AIC_7880_THREAD *) pScsiCtrl->pThread;

    return (aic7880ThreadParamsSet (pThread, offset, period));
    }

/*******************************************************************************
*
* aic7880ThreadParamsSet - set various parameters for a thread
*
* Parameters include transfer offset and period, as well as the ID of the
* target device.  All of these end up as encoded values stored either in
* the thread's register context or its associated shared memory area.
*
* Transfer period is specified in SCSI units (multiples of 4 ns).  An offset
* of zero specifies asynchronous transfer.
*
* RETURNS: OK or ERROR if it failed negotiation.
*/

LOCAL STATUS aic7880ThreadParamsSet
    (
    AIC_7880_THREAD * pThread,	    	/* thread to be affected  */
    UINT8            offset,		/* max REQ/ACK offset     */
    UINT8            period		/* min transfer period    */
    )
    {
    SCSI_THREAD   * pScsiThread  = (SCSI_THREAD *) pThread;

    SIOP          * pSiop        = (SIOP *) pScsiThread->pScsiCtrl;
    cfp_struct    * pCfpStruct;
    sp_struct     * pScb;
    UINT8           xferRate;
    UINT8           devBusId;
    STATUS          status;

    pScb = pThread->pScb;

    devBusId = (pScb->SP_Tarlun >> 4) & 0x0f;

    pCfpStruct = pSiop->aic7880CfpStruct;
    
    aic7880XferParamsCvt (pSiop, &offset, &period, &xferRate);

    offset = (offset - 1) << 1;
    offset &= SOFS;

    pCfpStruct->Cf_ScsiOption [devBusId] &= 0x80;

    pCfpStruct->Cf_ScsiOption [devBusId] |= (xferRate | offset | SYNC_MODE);

    pCfpStruct->CFP_SuppressNego = 0;

    if ((status = (PH_Special (FORCE_RENEGOTIATE, pCfpStruct, pScb))) != OK)
	{
	SCSI_DEBUG_MSG ("aic7880ThreadParamsSet: Sync Negotiation Failed.\n",
			 0, 0, 0, 0, 0, 0);
	return (ERROR);
	}

    return (OK);
    }

/*******************************************************************************
*
* aic7880WideXferParamsSet - set wide transfer parameters
*
* Assume valid parameters and set the AIC 7880's thread parameters to the
* appropriate values.
*
* RETURNS: OK or ERROR if it failed wide negotiation.
*/

LOCAL STATUS aic7880WideXferParamsSet
    (
    SCSI_CTRL * pScsiCtrl,		/* ptr to controller info */
    UINT8       xferWidth               /* wide data transfer width */
    )
    {
    AIC_7880_THREAD * pThread = (AIC_7880_THREAD *) pScsiCtrl->pThread;

    SCSI_THREAD   * pScsiThread  = (SCSI_THREAD *) pThread;
    SIOP          * pSiop        = (SIOP *) pScsiThread->pScsiCtrl;
    cfp_struct    * pCfpStruct;
    sp_struct     * pScb;
    UINT8           devBusId;
    STATUS          status;

    pScb = pThread->pScb;

    devBusId = (pScb->SP_Tarlun >> 4) & 0x0f;

    pCfpStruct = pSiop->aic7880CfpStruct;

    if (xferWidth != SCSI_WIDE_XFER_SIZE_NARROW)
	{
	pCfpStruct->Cf_ScsiOption [devBusId] |= WIDE_MODE;

	SCSI_DEBUG_MSG ("aic7880WideXferParamsSet: %x %x\n", devBusId, 
			pCfpStruct->Cf_ScsiOption [devBusId], 0, 0, 0, 0);
	
	pCfpStruct->CFP_SuppressNego = 0;
	
	if ((status = (PH_Special (FORCE_RENEGOTIATE, pCfpStruct, pScb))) != OK)
	    {
	    SCSI_DEBUG_MSG ("aic7880WideParamsSet: Wide Negotiation Failed.\n",
			    0, 0, 0, 0, 0, 0);
	    return (ERROR);
	    }   
	}
    else
	pCfpStruct->Cf_ScsiOption [devBusId] = NARROW_MODE;    

    return (OK);
    }

/*******************************************************************************
*
* aic7880EnableFast20 - enable double speed SCSI data transfers
*
* This routine enables double speed SCSI data transfers for the SCSI host
* adapter. This allows the host adapter to transfer data upto 20 MB/s for
* an 8 bit device and upto 40 MB/s for a 16 bit device.
*
* RETURNS: N/A
*/

VOID aic7880EnableFast20
    (
    SCSI_CTRL * pScsiCtrl,  /* ptr to SCSI controller */
    BOOL        enable      /* enable = 1 / disable = 0 */
    )
    {
    SIOP        * pSiop  = (SIOP *) pScsiCtrl;
    cfp_struct  * configPtr = pSiop->aic7880CfpStruct;

    if (enable)
	configPtr->CFP_EnableFast20 = 1;
    else
	configPtr->CFP_EnableFast20 = 0;
    }

/*******************************************************************************
*
* aic7880dFifoThresholdSet - set the data FIFO threshold.
*
* This routine specifies to the AIC-7880 host adapter how to manage its
* data FIFO. Below is a description of the threshold  values for SCSI
* reads and writes.
*
* SCSI READS
* .iP
* 0 Xfer data from FIFO as soon as it is available.
* .iP
* 1 Xfer data from FIFO as soon as the FIFO is half full.
* .iP
* 2 Xfer data from FIFO as soon as the FIFO is 75%  full.
* .iP
* 3 Xfer data from FIFO as soon as the FIFO is 100% full.
*
* SCSI WRITES
* .iP
* 0 Xfer data as soon as there is room in the FIFO.
* .iP
* 1 Xfer data to FIFO as soon as it is 50% empty. 
* .iP
* 2 Xfer data to FIFO as soon as it is 75% empty.
* .iP
* 3 Xfer data to FIFO as soon as the FIFO is empty.
*
* RETURNS: OK or ERROR if the threshold value is not within the valid range.
*/

STATUS aic7880dFifoThresholdSet 
    (
    SCSI_CTRL * pScsiCtrl,   /* ptr to SCSI controller */
    UBYTE       threshHold   /* data FIFO threshold value */
    )
    {
    SIOP        * pSiop  = (SIOP *) pScsiCtrl;
    cfp_struct  * configPtr = pSiop->aic7880CfpStruct;

    if (threshHold > 3)
	return (ERROR);

    configPtr->Cf_Threshold = threshHold;

    return (OK);
    }

/*******************************************************************************
*
* aic7880ThreadAbort - abort a thread
*
* If the thread is not currently connected, do nothing and return FALSE to
* indicate that the SCSI manager should abort the thread. Otherwise the
* thread is active on the controller. Invoke the PH_Special routine with the 
* Abort SCB opcode passing in the address of the SCB to be aborted. The HIM 
* reacts as follows:
*
* If the SCB has not become active, the status is set to aborted and
* it is  returned. If the SCB has already completed, it returns completion 
* status. If the SCB is active and connected, the abort is issued and the SCB
* is returned with an aborted status. If the target fails to transition to
* the Message out or Bus Free phase, a SCSI Reset is issued. If the SCB is
* active but disconnected, the HIM selects the target and issues an abort 
* message. If the target does not respond correctly to the selection and
* abort sequence a SCSI Bus Reset is issued. If the HIM does not find a 
* valid SCB at the specified address, the abort command is simply ignored.
*
* RETURNS: TRUE if the thread is being aborted by this driver (i.e. it is
* currently active on the controller) else FALSE.
*/

LOCAL BOOL aic7880ThreadAbort
    (
    SIOP            * pSiop,      /* ptr to controller info */
    AIC_7880_THREAD * pThread     /* ptr to SCSI controller thread */
    )
    {
    SCSI_CTRL   * pScsiCtrl   = (SCSI_CTRL *)   pSiop;
    SCSI_THREAD * pScsiThread = (SCSI_THREAD *) pThread;
    sp_struct   * pScb = pThread->pScb;
    cfp_struct  * configPtr = pSiop->aic7880CfpStruct;    

    SCSI_DEBUG_MSG ("aic7880ThreadAbort: thread 0x%08x state = %d aborting\n",
	    (int) pThread, pScsiThread->state, 0, 0, 0, 0);

    if (pScsiThread != pScsiCtrl->pThread)
        return (FALSE);

    switch (pScsiThread->state)
        {
        case SCSI_THREAD_INACTIVE:
            return (FALSE);
            break;

	default:
	    PH_Special (ABORT_SCB, configPtr, pScb);
	    aic7880ThreadStateSet (pThread, SCSI_THREAD_ABORTING);
	    break;
	}
    return (TRUE);
    }


/*******************************************************************************
*
* aic7880GetNumOfBuses - perform a PCI bus scan
*
* This routine provides a callback mechanism from the HIM to the OSM
* It allows the OSM to scan the PCI bus, before the HIM is allowed 
* to perform the bus scan.
*
* RETURNS: 0x55555555 if the OSM is not able to conduct its own bus scan
*/

DWORD aic7880GetNumOfBuses ()
    {
    return (0x55555555);
    }

/*******************************************************************************
*
* aic7880ReadConfig  - read from PCI config space
*
* This routine provides a callback mechanism from the HIM to the OSM.
* The purpose of this routine is to allow the OSM to do its own Read
* access of the PCI configuration space. If the OSM cannot successfully
* complete the Read access, the OSM returns 0x55555555. If this happens
* the HIM attempts to conduct the configuration space Read access.
*
* RETURNS: value read or 0x55555555, if the OSM is not able to conduct 
* read access to the PCI configuration space.
*/

DWORD aic7880ReadConfig 
    (
    cfp_struct * configPtr, /* ptr to cf_struct */
    UBYTE        busNo,     /* PCI bus number */
    UBYTE        devNo,     /* PCI device number */
    UBYTE        regNo      /* register */
    )
    {
    int          funcNo = 0;
    DWORD        regVal;

    if ((pciConfigInLong  (busNo, devNo, funcNo, (int) regNo,
			   &regVal)) != OK)
        {
        logMsg ("aic7880ReadConfig: PCI read failed\n",
                 0, 0, 0, 0, 0, 0);
	return (0x55555555);
        }
    else
	return (regVal);
    }

/*******************************************************************************
*
* aic7880WriteConfig - read to PCI config space
*
* This routine provides a callback mechanism from the HIM to the OSM.
* The purpose of this routine is to allow the OSM to do its own write
* access of the PCI configuration space. If the OSM cannot successfully
* complete the write access, the OSM returns 0x55555555. If this happens
* the HIM attempts to conduct the configuration space write access.
*
* RETURNS: OK or 0x55555555, if the OSM is not able to conduct write access 
* to the PCI configuration space.
*/

DWORD aic7880WriteConfig 
    (
    cfp_struct * config_ptr,  /* ptr to cf_struct */
    UBYTE        busNo,       /* PCI bus number */
    UBYTE        devNo,       /* PCI device number */
    UBYTE        regNo,       /* register */
    DWORD        regVal       /* register value */
    )
    {
    int funcNo = 0;

    if ((pciConfigOutLong  (busNo, devNo, funcNo, (int) regNo,
			    regVal)) != OK)
        {
        logMsg ("aic7880WriteConfig: PCI read failed\n",
                 0, 0, 0, 0, 0, 0);
	return (0x55555555);
        }
    else
	return (OK);
    }
