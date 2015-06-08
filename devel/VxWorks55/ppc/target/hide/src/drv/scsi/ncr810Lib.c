/* ncr810Lib.c - NCR 53C8xx PCI SCSI I/O Processor (SIOP) library (SCSI-2) */

/* Copyright 1989-2002 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
02a,26apr02,pmr  SPR 76029: using intLock/intUnlock instead of
		 ncr810IntDisable/Restore.
01z,29oct01,pmr  modified ncr810XferParamsCvt() to update the period parameter
                 with the correct value (SPR 71337).
01y,24oct01,dat  merge from ae 1.1, lint removal using diab
01x,20sep01,rcs  Added ncr810IntDisable() and ncr810IntRestore() and used them
                 to replace intLock() / intUnlock() pairs in ncr810Abort(),
                 ncr810Activate(), ncr810Resume(), and ncr810EventTypeGet(). 
                 Also, added CACHE_PIPE_FLUSH() calls where nedded. SPR# 29734 
01w,21nov00,jkf  Fixed SPR#35801/35403, no NULL pointers if WIDE selected.
01v,03dec98,ihw  Modified handling of ident message for new thread:
		 add normal message out (if any) - e.g. sync/wide xfer.
		 Removed incorrect code in ncr810Activate() (used to fudge
		 controller state if reselection race occurred). (SPR 24089)
		 Added several CACHE_PIPE_FLUSH calls at strategic points.
01u,06oct98,dat  Fixed illegal device accesses, SPR 22597. Provided by luke.
01t,24aug98,sut  fixed SPR 21260
01s,18aug98,tpr  added PowerPC EC 603 support.
01r,23apr98,tm,  SPR 8384, 9025: Augmented ncr810RemainderGet() to support 
            dds    825/875. inOrderExecute() changed to CACHE_PIPE_FLUSH
01q,14oct97,dds  SPR 9543, modified "ncr810Intr" to return immediately if a
                 spurious interrupt is detected.
01p,28jul97,dds  SPR#9015: replaced logMsg with SCSI_INT_DEBUG_MSG.
01o,10jul97,mas  removed dependency on BSP-specific byte I/O routines.
01n,09jul97,mas  macros for all I/O; first delay count increased (SPR 8842).
01m,09jul97,dgp  doc: correct fonts per SPR 7853
01l,08may97,dat  temp fix for SPR 8540, VIRT_TO_PHYS problem
01k,10mar97,dds  SPR 7766: added changes for ncr875 SCSI adapter.
01j,06mar97,dds  SPR 7996/8091: added a delay to configure scsi devices
                 when used with a fast processor.
01i,20nov96,myz  added CACHE_DMA_VIRT_TO_PHYS for those missing
01h,29oct96,dgp  doc: editing for newly published SCSI libraries
01g,19aug96,dds  modified to generate man pages.
01f,29jul96,jds  added inOrderExecute() to resolve erratic behaviour when
		 instruction cache is turned on.
01e,25jul96,dds  SPR 6818: documentation correction.
01d,01may96,jds  more doc tweaks
01c,22apr96,jds  doc tweaks.
01b,22apr96,jds  made to work with multiple NCR 8XX controllers. 
01a,28jun95,jds  Created. Adapted from ncr710Lib.c
*/

/*
DESCRIPTION
This is the I/O driver for the NCR 53C8xx PCI SCSI I/O Processors (SIOP),
supporting the NCR 53C810 and the NCR 53C825 SCSI controllers.
It is designed to work with scsiLib and scsi2Lib.  This driver 
runs in conjunction with a script program for the NCR 53C8xx controllers.
These scripts use DMA transfers for all data, messages, and status.
This driver supports cache functions through cacheLib.

USER-CALLABLE ROUTINES
Most of the routines in this driver are accessible only through the I/O
system.  Three routines, however, must be called directly.  ncr810CtrlCreate()
creates a controller structure and ncr810CtrlInit() initializes it.
The NCR 53C8xx hardware registers need to be configured according to
the hardware implementation.  If the default configuration is not correct,
the routine ncr810SetHwRegister() must be used to properly configure
the registers.

PCI MEMORY ADDRESSING
The global variable ncr810PciMemOffset was created to provide the BSP
with a means of changing the VIRT_TO_PHYS mapping without changing the
functions in the cacheFuncs structures.  In generating physical addresses
for DMA on the PCI bus, local addresses are passed through the function
CACHE_DMA_VIRT_TO_PHYS and then the value of ncr810PciMemOffset is added.
For backward compatibility, the initial value of ncr810PciMemOffset comes
from the macro PCI_TO_MEM_OFFSET defined in ncr810.h.

I/O MACROS
All device access for input and output is done via macros which can be
customized for each BSP.  These routines are NCR810_IN_BYTE, NCR810_OUT_BYTE,
NCR810_IN_16, NCR810_OUT_16, NCR810_IN_32 and NCR810_OUT_32.
By default, these are defined as generic memory references.

INTERNAL
This driver supports multiple initiators, disconnect/reconnect, tagged
command queueing and synchronous data transfer protocol.  In general, the
SCSI system and this driver will automatically choose the best combination
of these features to suit the target devices used.  However, the default
choices may be over-ridden by using the function "scsiTargetOptionsSet()"
(see scsiLib).

There are debug variables to trace events in the driver.
<scsiDebug> scsiLib debug variable, trace event in scsiLib, ncr810ScsiPhase(),
and ncr810Transact().
<scsiIntsDebug> prints interrupt informations.

INCLUDE FILES
ncr810.h, ncr810Script.h and scsiLib.h

SEE ALSO: scsiLib, scsi2Lib, cacheLib, 
.I "SYM53C825 PCI-SCSI I/O Processor Data Manual,"
.I "SYM53C810 PCI-SCSI I/O Processor Data Manual," 
.I "NCR 53C8XX Family PCI-SCSI I/O Processors Programming Guide,"
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
#include "drv/scsi/ncr810.h"
#include "drv/scsi/ncr810Script.h"

/* defines */

#define SIOP_MAX_XFER_LENGTH  	((UINT) (0x00ffffff)) /* max data xfer length */

/* I/O macros */

#if ((CPU==PPC603) || (CPU == PPCEC603) || (CPU==PPC604))
# undef CACHE_PIPE_FLUSH
extern void  vxEieio (void);
# define CACHE_PIPE_FLUSH() vxEieio()
#endif

#ifndef NCR810_IN_BYTE
# define NCR810_IN_BYTE(adr)       (*(volatile UINT8 *)(adr))
#endif
#ifndef NCR810_OUT_BYTE
# define NCR810_OUT_BYTE(adr,val)  (*(volatile UINT8 *)(adr)) = ((UINT8)(val))
#endif
#ifndef NCR810_IN_16
# define NCR810_IN_16(adr)      (NCR810_SWAP_16(*(volatile UINT16 *)(adr)))
#endif
#ifndef NCR810_OUT_16
# define NCR810_OUT_16(adr,val) (*(volatile UINT16 *)(adr)) = NCR810_SWAP_16((UINT)(val))
#endif
#ifndef NCR810_IN_32
# define NCR810_IN_32(adr)      (NCR810_SWAP_32(*(UINT *)(adr)))
#endif
#ifndef NCR810_OUT_32
# define NCR810_OUT_32(adr,val) (*(volatile UINT *)(adr)) = NCR810_SWAP_32((UINT)(val))
#endif

#define NCR810_VIRT_TO_PHYS(x) \
	((UINT)CACHE_DMA_VIRT_TO_PHYS(x) + (UINT)ncr810PciMemOffset)

/* Data stream swapping macros */

#if (_BYTE_ORDER == _BIG_ENDIAN)
#   define NCR810_SWAP_32(x)     LONGSWAP((UINT)x)
#   define NCR810_SWAP_16(x)     ((LSB(x) << 8) | MSB(x))
#else
#   define NCR810_SWAP_32(x)    (x)
#   define NCR810_SWAP_16(x)    (x)
#endif 

/* typedefs */

typedef NCR_810_SCSI_CTRL SIOP;


/* External */

IMPORT BOOL scsiDebug;
IMPORT BOOL scsiErrors;
IMPORT BOOL scsiIntsDebug;

/* Global variables */

int delayCount=0;
int ncr810DelayCount = 10;
int ncr810PciMemOffset = (int)PCI_TO_MEM_OFFSET;

/*
 *  Configurable options
 */

int ncr810SingleStepSemOptions = SEM_Q_PRIORITY;

char* ncr810ScsiTaskName      = SCSI_DEF_TASK_NAME;
int   ncr810ScsiTaskOptions   = SCSI_DEF_TASK_OPTIONS;
int   ncr810ScsiTaskPriority  = SCSI_DEF_TASK_PRIORITY;
int   ncr810ScsiTaskStackSize = SCSI_DEF_TASK_STACK_SIZE;


/* forward declarations */

LOCAL void   ncr810HwInit        (SIOP *pSiop);
LOCAL int    ncr810BusIdGet      (SIOP *pSiop, UINT busIdBits);
LOCAL UINT   ncr810RemainderGet  (SIOP *pSiop, UINT phase);
LOCAL int    ncr810EventTypeGet  (SIOP *pSiop);
LOCAL STATUS ncr810PhaseMismatch (NCR810_THREAD * pThread,
				  int             phase,
				  UINT            remCount);
LOCAL STATUS ncr810ScsiBusControl (SIOP *pSiop, int operation);
LOCAL void   ncr810ScsiBusReset   (SIOP *pSiop);
LOCAL STATUS ncr810ThreadActivate (SIOP *pSiop, NCR810_THREAD *pThread);
LOCAL BOOL   ncr810ThreadAbort    (SIOP *pSiop, NCR810_THREAD *pThread);
LOCAL void   ncr810Event          (SIOP *pSiop, NCR810_EVENT  *pEvent);
LOCAL void   ncr810ThreadEvent   (NCR810_THREAD *pThread, NCR810_EVENT *pEvent);
LOCAL void   ncr810InitEvent     (NCR810_THREAD *pThread, NCR810_EVENT *pEvent);
LOCAL void   ncr810InitIdentEvent(NCR810_THREAD *pThread, NCR810_EVENT *pEvent);
LOCAL void   ncr810TargIdentEvent(NCR810_THREAD *pThread, NCR810_EVENT *pEvent);
LOCAL void   ncr810IdentInContinue (NCR810_THREAD *pThread);
LOCAL void   ncr810ThreadReconnect (NCR810_THREAD *pThread);
LOCAL void   ncr810SharedMemInit   (SIOP *pSiop, NCR810_SHARED *pShMem);
LOCAL STATUS ncr810ThreadInit      (SIOP *pSiop, NCR810_THREAD *pThread);
LOCAL STATUS ncr810IdentThreadInit (NCR810_THREAD *pThread);
LOCAL void   ncr810ThreadUpdate    (NCR810_THREAD *pThread);
LOCAL void   ncr810ThreadComplete  (NCR810_THREAD *pThread);
LOCAL void   ncr810ThreadDefer     (NCR810_THREAD *pThread);
LOCAL void   ncr810ThreadFail      (NCR810_THREAD *pThread, int errNum);
LOCAL void   ncr810ThreadStateSet  (NCR810_THREAD *pThread,
				    SCSI_THREAD_STATE state);
LOCAL STATUS ncr810Activate      (SIOP               *pSiop,
				  NCR810_THREAD      *pThread);
LOCAL STATUS ncr810Resume        (SIOP               *pSiop,
				  NCR810_THREAD      *pThread,
				  NCR810_SCRIPT_ENTRY entryId);
LOCAL void   ncr810Abort         (SIOP               *pSiop);
LOCAL void   ncr810ScriptStart   (SIOP               *pSiop,
				  NCR810_THREAD      *pThread,
				  NCR810_SCRIPT_ENTRY entryId);
LOCAL STATUS ncr810ThreadParamsSet (NCR810_THREAD * pThread,
				    UINT8      offset,
				    UINT8      period);
LOCAL STATUS ncr810XferParamsQuery (SCSI_CTRL *pScsiCtrl,
				    UINT8     *pOffset,
				    UINT8     *pPeriod);
LOCAL STATUS ncr810WideXferParamsQuery (SCSI_CTRL *pScsiCtrl,
				    	UINT8     *xferWidth);
LOCAL STATUS ncr810XferParamsSet   (SCSI_CTRL *pScsiCtrl,
				    UINT8      offset,
				    UINT8      period);
LOCAL STATUS ncr810WideXferParamsSet   (SCSI_CTRL *pScsiCtrl,
				        UINT8      wideXfer);
LOCAL BOOL   ncr810XferParamsCvt   (SIOP  *pSiop,
				    UINT8 *pOffset,
				    UINT8 *pPeriod,
				    UINT8 *pXferParams,
				    UINT8 *pClockDiv);
LOCAL void   ncr810Delay (void);
LOCAL void   ncr810ResetAtn (SCSI_CTRL *pScsiCtrl);

/*******************************************************************************
*
* ncr810CtrlCreate - create a control structure for the NCR 53C8xx SIOP
*
* This routine creates an SIOP data structure and must be called before
* using an SIOP chip.  It must be called exactly once for a 
* specified SIOP controller. Since it allocates memory for a structure needed 
* by all routines in ncr810Lib, it must be called before any other 
* routines in the library. After calling this routine, ncr810CtrlInit() 
* must be called at least once before any SCSI transactions are 
* initiated using the SIOP.
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
* Several commonly-used values are defined in ncr810.h as follows:
*.CS
*	NCR810_1667MHZ  6000    /@ 16.67Mhz chip @/
*	NCR810_20MHZ    5000    /@ 20Mhz chip    @/
*	NCR810_25MHZ    4000    /@ 25Mhz chip    @/
*	NCR810_3750MHZ  2667    /@ 37.50Mhz chip @/
*	NCR810_40MHZ    2500    /@ 40Mhz chip    @/
*	NCR810_50MHZ    2000    /@ 50Mhz chip    @/
*	NCR810_66MHZ    1515    /@ 66Mhz chip    @/
*	NCR810_6666MHZ  1500    /@ 66.66Mhz chip @/
*.CE
* .iP <devType>
* the specific NCR 8xx device type. Current device types
* are defined in the header file ncr810.h.
*
* RETURNS: A pointer to the NCR_810_SCSI_CTRL structure, or NULL if memory 
* is unavailable or there are invalid parameters.
*/
NCR_810_SCSI_CTRL *ncr810CtrlCreate 
    (
    UINT8 *baseAdrs,		/* base address of the SIOP */
    UINT   clkPeriod,		/* clock controller period (nsec*100) */
    UINT16 devType		/* NCR8XX SCSI device type */
    )

    {
    FAST SIOP *pSiop;		    /* ptr to SIOP info */
    int        nBytes;	    	    /* total amount of cache-coherent memory */
    SCSI_CTRL *pScsiCtrl;
    BOOL       wideXfer;

    static NCR810_HW_REGS ncr810HwRegs = DEFAULT_810_HW_REGS;

    /* verify parameters */
    if (baseAdrs == ((UINT8 *) NULL))
	return ((SIOP *) NULL);
    
    if ((clkPeriod > NCR810_1667MHZ) ||  
        (clkPeriod < NCR810_6666MHZ))
        return ((SIOP *) NULL);
    
    /* check that dma buffers are cache-coherent */

    if (!CACHE_DMA_IS_WRITE_COHERENT () ||
	!CACHE_DMA_IS_READ_COHERENT ())
	{
	SCSI_MSG ("ncr810CtrlCreate: shared memory not cache coherent.\n",
		  0, 0, 0, 0, 0, 0);
        return ((SIOP *) NULL);
	}

    /* check to see if it is a supported NCR8XX device type */

    switch (devType)
        {
        case NCR810_DEVICE_ID :
            wideXfer = FALSE;
            break;
            
        case NCR825_DEVICE_ID :
            wideXfer = TRUE;
            break;

        case NCR875_DEVICE_ID :
            wideXfer = TRUE;
            break;

        default:
            logMsg ("Invalid NCR8XX device type. Not supported.\n",
							0, 0, 0, 0, 0, 0);
            return ((SIOP *) NULL);
        }

    /* cacheDmaMalloc the controller info structure and two shared data areas */

    nBytes = sizeof (SIOP) + 2 * sizeof (NCR810_SHARED);

    if ((pSiop = (SIOP *) cacheDmaMalloc (nBytes)) == (SIOP *) NULL)
        return ((SIOP *) NULL);

    bzero ((char *) pSiop, nBytes);

    pSiop->pIdentShMem  = (NCR810_SHARED *) (pSiop + 1);
    pSiop->pClientShMem = pSiop->pIdentShMem + 1;

    /* set up configuration variables */

    pScsiCtrl = (SCSI_CTRL *) pSiop;
    
    pScsiCtrl->eventSize  = sizeof (NCR810_EVENT);
    pScsiCtrl->threadSize = sizeof (NCR810_THREAD);

    /* fill in virtual functions used by SCSI library */
    
    pScsiCtrl->maxBytesPerXfer = SIOP_MAX_XFER_LENGTH;

    pScsiCtrl->scsiTransact = (FUNCPTR) scsiTransact;

    pScsiCtrl->scsiEventProc       = (VOIDFUNCPTR) ncr810Event;
    pScsiCtrl->scsiThreadInit      = (FUNCPTR)     ncr810ThreadInit;
    pScsiCtrl->scsiThreadActivate  = (FUNCPTR)     ncr810ThreadActivate;
    pScsiCtrl->scsiThreadAbort     = (FUNCPTR)     ncr810ThreadAbort;
    pScsiCtrl->scsiBusControl      = (FUNCPTR)     ncr810ScsiBusControl;
    pScsiCtrl->scsiXferParamsQuery = (FUNCPTR)     ncr810XferParamsQuery;
    pScsiCtrl->scsiXferParamsSet   = (FUNCPTR)     ncr810XferParamsSet;
    pScsiCtrl->scsiWideXferParamsQuery = (FUNCPTR) ncr810WideXferParamsQuery;
    pScsiCtrl->scsiWideXferParamsSet   = (FUNCPTR) ncr810WideXferParamsSet;


    /* the following virtual functions are not used with the NCR 53C810 */
    
    pScsiCtrl->scsiDevSelect = NULL;
    pScsiCtrl->scsiInfoXfer  = NULL;

    /* 
     * The default scsiLib behaviour is to turn wide transfers off. However
     * for the NCR825, the default behaviour should be to turn wide transfers 
     * on. It is still possible to turn off / on wide transfers for individual 
     * physical devices via the scsiTargetOptionsSet() routine defined
     * by scsiLib.
     */

    pScsiCtrl->wideXfer = wideXfer;

    /* fill in generic SCSI info for this controller */

    scsiCtrlInit (&pSiop->scsiCtrl);

    /* fill in SIOP specific data for this controller */

    pSiop->pScntl0    = baseAdrs + OFF_SCNTL0;
    pSiop->pScntl1    = baseAdrs + OFF_SCNTL1;
    pSiop->pScntl2    = baseAdrs + OFF_SCNTL2;
    pSiop->pScntl3    = baseAdrs + OFF_SCNTL3;
    pSiop->pScid      = baseAdrs + OFF_SCID;
    pSiop->pSxfer     = baseAdrs + OFF_SXFER;
    pSiop->pSdid      = baseAdrs + OFF_SDID;
    pSiop->pGpreg     = baseAdrs + OFF_GPREG;
    pSiop->pSfbr      = baseAdrs + OFF_SFBR;
    pSiop->pSocl      = baseAdrs + OFF_SOCL;
    pSiop->pSsid      = baseAdrs + OFF_SSID;
    pSiop->pSbcl      = baseAdrs + OFF_SBCL;
    pSiop->pDstat     = baseAdrs + OFF_DSTAT;
    pSiop->pSstat0    = baseAdrs + OFF_SSTAT0;
    pSiop->pSstat1    = baseAdrs + OFF_SSTAT1;
    pSiop->pSstat2    = baseAdrs + OFF_SSTAT2;
    pSiop->pDsa       = (UINT *) (baseAdrs + OFF_DSA);
    pSiop->pIstat     = baseAdrs + OFF_ISTAT;
    pSiop->pCtest0    = baseAdrs + OFF_CTEST0;
    pSiop->pCtest1    = baseAdrs + OFF_CTEST1;
    pSiop->pCtest2    = baseAdrs + OFF_CTEST2;
    pSiop->pCtest3    = baseAdrs + OFF_CTEST3;
    pSiop->pTemp      = (UINT *) (baseAdrs + OFF_TEMP);
    pSiop->pDfifo     = baseAdrs + OFF_DFIFO;
    pSiop->pCtest4    = baseAdrs + OFF_CTEST4;
    pSiop->pCtest5    = baseAdrs + OFF_CTEST5;
    pSiop->pCtest6    = baseAdrs + OFF_CTEST6;
    pSiop->pDbc       = (UINT *) (baseAdrs + OFF_DBC);
    pSiop->pDcmd      = baseAdrs + OFF_DCMD;
    pSiop->pDnad      = (UINT *) (baseAdrs + OFF_DNAD);
    pSiop->pDsp       = (UINT *) (baseAdrs + OFF_DSP);
    pSiop->pDsps      = (UINT *) (baseAdrs + OFF_DSPS);
    pSiop->pScratcha0 = baseAdrs + OFF_SCRATCHA0;
    pSiop->pScratcha1 = baseAdrs + OFF_SCRATCHA1;
    pSiop->pScratcha2 = baseAdrs + OFF_SCRATCHA2;
    pSiop->pScratcha3 = baseAdrs + OFF_SCRATCHA3;
    pSiop->pDmode     = baseAdrs + OFF_DMODE;
    pSiop->pDien      = baseAdrs + OFF_DIEN;
    pSiop->pDwt       = baseAdrs + OFF_DWT;
    pSiop->pDcntl     = baseAdrs + OFF_DCNTL;
    pSiop->pAdder     = (UINT *) (baseAdrs + OFF_ADDER);
    pSiop->pSien0     = baseAdrs + OFF_SIEN0;
    pSiop->pSien1     = baseAdrs + OFF_SIEN1;
    pSiop->pSist0     = baseAdrs + OFF_SIST0;
    pSiop->pSist1     = baseAdrs + OFF_SIST1;
    pSiop->pSlpar     = baseAdrs + OFF_SLPAR;
    pSiop->pMacntl    = baseAdrs + OFF_MACNTL;
    pSiop->pGpcntl    = baseAdrs + OFF_GPCNTL;
    pSiop->pStime0    = baseAdrs + OFF_STIME0;
    pSiop->pStime1    = baseAdrs + OFF_STIME1;
    pSiop->pRespid    = baseAdrs + OFF_RESPID;
    pSiop->pStest0    = baseAdrs + OFF_STEST0;
    pSiop->pStest1    = baseAdrs + OFF_STEST1;
    pSiop->pStest2    = baseAdrs + OFF_STEST2;
    pSiop->pStest3    = baseAdrs + OFF_STEST3;
    pSiop->pSidl      = (UINT16 *) (baseAdrs + OFF_SIDL);
    pSiop->pSodl      = (UINT16 *) (baseAdrs + OFF_SODL);
    pSiop->pSbdl      = (UINT16 *) (baseAdrs + OFF_SBDL);
    pSiop->pScratchb  = (UINT *) (baseAdrs + OFF_SCRATCHB);


    pSiop->devType   = devType;
    pSiop->clkPeriod = clkPeriod;

    /*
     *	Initialise hardware-dependent registers to default values.
     */
    bcopy ((char *)&ncr810HwRegs, (char *)&pSiop->hwRegs,
	                          sizeof (NCR810_HW_REGS));

    /*
     *	Create synchronisation semaphore for single-step support
     */
    if ((pSiop->singleStepSem = semBCreate(ncr810SingleStepSemOptions,
					   SEM_EMPTY)) == NULL)
	{
	SCSI_MSG ("ncr810CtrlCreate: semBCreate of singleStepSem failed.\n"
		  ,0, 0, 0, 0, 0, 0);
	goto failed;
        }

    /*
     *	Initialise controller state variables
     */
    pSiop->state      = NCR810_STATE_IDLE;
    pSiop->cmdPending = FALSE;

    /*
     *	Initialize fixed fields in client shared data area
     */
    ncr810SharedMemInit (pSiop, pSiop->pClientShMem);

    /*
     *	Identification thread has been created by the generic initialisation.
     *	Initialise it for use with the NCR 53C810.
     */
    ncr810IdentThreadInit ((NCR810_THREAD *) pScsiCtrl->pIdentThread);

    /* spawn SCSI manager - use generic code from "scsiLib.c" */

    pScsiCtrl->scsiMgrId = taskSpawn (ncr810ScsiTaskName,
		       	    	      ncr810ScsiTaskPriority,
		       	    	      ncr810ScsiTaskOptions,
		       	    	      ncr810ScsiTaskStackSize,
		       	    	      (FUNCPTR) scsiMgr,
		       	    	      (int) pSiop, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    if (pScsiCtrl->scsiMgrId == ERROR)
	{
	SCSI_MSG ("ncr810CtrlCreate: can't spawn SCSI manager task\n",
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
* ncr810CtrlInit - initialize a control structure for the NCR 53C8xx SIOP
*
* This routine initializes an SIOP structure, after the structure is created
* with ncr810CtrlCreate().  This structure must be initialized before the
* SIOP can be used.  It may be called more than once if needed;  however,
* it must only be called while there is no activity on the SCSI interface.
*
* A detailed description of the input parameters follows:
* .iP <pSiop>
* a pointer to the NCR_810_SCSI_CTRL structure created with
* ncr810CtrlCreate().
* .iP <scsiCtrlBusId>
* the SCSI bus ID of the SIOP.  Its value is somewhat arbitrary:  seven (7),
* or highest priority, is conventional.  The value must be in the range 0 - 7.
*
* RETURNS: OK, or ERROR if parameters are out of range.
*/

STATUS ncr810CtrlInit
    (
    FAST NCR_810_SCSI_CTRL *pSiop,  /* ptr to SIOP struct */
    int scsiCtrlBusId		    /* SCSI bus ID of this SIOP */
    )

    {
    SCSI_CTRL * pScsiCtrl = (SCSI_CTRL *) pSiop;
    volatile int delay;

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
    ncr810HwInit (pSiop);
    delay = ncr810DelayCount;
    ncr810DelayCount *= 35;	/* First delay must be longer */

    ncr810ScriptStart (pSiop,
		       (NCR810_THREAD *) pScsiCtrl->pIdentThread,
		       NCR810_SCRIPT_WAIT);

    pSiop->state = NCR810_STATE_PASSIVE;
    ncr810DelayCount = delay;	/* Restore original delay count */

    return (OK);
    }


/*******************************************************************************
*
* ncr810HwInit - initialize the SIOP chip to a known state
*
* This routine puts the SIOP into a known quiescent state.  Because the value
* of some registers depend upon the hardware implementation of the chip you 
* may have to use ncr810SetHwRegister() to adjust them.
*
* This routine does not touch the bits in registers that are set by 
* the ncr810SetHwRegister() routine.
*
* See the NCR 53C810 Data Manual for fuller details of the registers
* programmed by this routine.
*
* SEE ALSO 
*	ncr810SetHwRegister()
*
* NOMANUAL
*/

LOCAL void ncr810HwInit
    (
    FAST SIOP *pSiop			/* ptr to an SIOP info structure */
    )

    {
    UINT clkPeriod;

    /*
     *	Software reset the SIOP
     */
    NCR810_OUT_BYTE(pSiop->pIstat, ISTAT_SOFTRST);
    taskDelay (2);
    NCR810_OUT_BYTE(pSiop->pIstat, 0); 		/* clear the software reset */

    /*
     *	Initialise hardware-dependent registers
     */

    ncr810SetHwRegister (pSiop, &pSiop->hwRegs);

    /*
     *	DMA mode, control and watchdog timer registers
     *
     *	Use manual start and NCR 53C700 compatibility mode.
     *	Set clock conversion factor (the clkPeriod has been checked in
     *	"ncr810CtrlCreate()") and save async clock period for use by
     *	"ncr810XferParamsCvt()".  Disable the watchdog timer.
     *
     * Note:
     * Manual Start Mode. Setting this bit prevents automatic fetching and
     * execution of scripts when the DSP register is written.
     */

    CACHE_PIPE_FLUSH();
    NCR810_OUT_BYTE(pSiop->pDmode, NCR810_IN_BYTE(pSiop->pDmode) | DMODE_MAN);

    /* NCR 53C700 Compatability is turned off */

    CACHE_PIPE_FLUSH();
    NCR810_OUT_BYTE(pSiop->pDcntl, NCR810_IN_BYTE(pSiop->pDcntl) | DCNTL_COM);

    clkPeriod = pSiop->clkPeriod;

    if (clkPeriod >= NCR810_25MHZ)   	    	    /* 16.67 - 25.00 MHz */
    	{
	/* set the async and sync period to be the same */

        pSiop->clkDiv = NCR810_16MHZ_ASYNC_DIV;
        NCR810_OUT_BYTE(pSiop->pScntl3, NCR810_16MHZ_ASYNC_DIV);
    	}
    else if (clkPeriod >= NCR810_3750MHZ)    	    /* 25.01 - 37.50 MHz */
    	{
	/* set the async and sync period to be the same */

        pSiop->clkDiv = NCR810_25MHZ_ASYNC_DIV;
        NCR810_OUT_BYTE(pSiop->pScntl3, NCR810_25MHZ_ASYNC_DIV);
    	}
    else if (clkPeriod >= NCR810_50MHZ) 	    /* 37.51 - 50.00 MHz */
    	{
	/* set the async and sync period to be the same */

        pSiop->clkDiv = NCR810_50MHZ_ASYNC_DIV;
        NCR810_OUT_BYTE(pSiop->pScntl3, NCR810_50MHZ_ASYNC_DIV);
    	}
    else /* (clkPeriod >= NCR810_6666MHZ) */ 	    /* 50.01 - 66.67 MHz */
    	{
	/* set the async and sync period to be the same */

        pSiop->clkDiv = NCR810_66MHZ_ASYNC_DIV;
        NCR810_OUT_BYTE(pSiop->pScntl3, NCR810_66MHZ_ASYNC_DIV);
    	}

    /*
     *	SCSI control registers
     *
     *	If disconnect/reconnect is enabled, use full arbitration.
     *	Always generate parity; check for parity if required to do so.
     *	Set slow cable mode if specified.
     */

    if (pSiop->scsiCtrl.disconnect) 
	{
        CACHE_PIPE_FLUSH();
        NCR810_OUT_BYTE(pSiop->pScntl0, NCR810_IN_BYTE(pSiop->pScntl0) |
                        (SCNTL0_ARB1 | SCNTL0_ARB0)); /* full arbitration */
	}
   
    /* Enable select with attention */

    CACHE_PIPE_FLUSH();
    NCR810_OUT_BYTE(pSiop->pScntl0, NCR810_IN_BYTE(pSiop->pScntl0) |
                    SCNTL0_WATN);

#if FALSE
    /* Parity not supported yet */
    if (pSiop->parityCheckEnbl)
        {
        CACHE_PIPE_FLUSH();
        NCR810_OUT_BYTE(pSiop->pScntl0, NCR810_IN_BYTE(pSiop->pScntl0) |
                        B_EPC); /* Enable parity checking */
        }
#endif
     

    /*
     *	Set own SCSI bus ID (checked in "ncr810CtrlInit()")
     */
    CACHE_PIPE_FLUSH();
    NCR810_OUT_BYTE(pSiop->pScid, NCR810_IN_BYTE(pSiop->pScid) |
                    pSiop->scsiCtrl.scsiCtrlBusId);
   
    NCR810_OUT_BYTE(pSiop->pRespid, (1 << pSiop->scsiCtrl.scsiCtrlBusId));

    /* Enable response to bus initiated selection and reselection */

    CACHE_PIPE_FLUSH();
    NCR810_OUT_BYTE(pSiop->pScid, NCR810_IN_BYTE(pSiop->pScid) |
                    SCID_RRE | SCID_SRE);

    /*
     *	Enable relevant SCSI and DMA interrupts (see "ncr810EventTypeGet()")
     *
     *	Note: in target mode the phase mismatch interrupt (B_MA) is used to
     *	signal assertion of ATN.  Currently we cannot handle this either in
     *	this library or in the script.  Therefore, the script automatically
     *	disables this interrupt in target mode and enables it (for phase
     *	mismatch) in initiator mode.  Also see "ncr810EventTypeGet()".
     */

    NCR810_OUT_BYTE(pSiop->pSien0, (B_SGE | B_UDC | B_RST | B_PAR)); 
    NCR810_OUT_BYTE(pSiop->pSien1, (B_STO | B_HTH)); 
    NCR810_OUT_BYTE(pSiop->pDien,  (B_BF | B_ABT | B_SIR | B_IID));

    /* CTEST registers to set in order to get the right initialisation */

    CACHE_PIPE_FLUSH();
    NCR810_OUT_BYTE(pSiop->pCtest3, NCR810_IN_BYTE(pSiop->pCtest3) |CTEST3_CLF);
    }


/*******************************************************************************
*
* ncr810SetHwRegister - set hardware-dependent registers for the NCR 53C8xx SIOP
*
* This routine sets up the registers used in the hardware
* implementation of the chip.  Typically, this routine is called by 
* the sysScsiInit() routine from the BSP. 
*
* The input parameters are as follows:
* .iP <pSiop> 4
* a pointer to the NCR_810_SCSI_CTRL structure created with
* ncr810CtrlCreate().
* .iP <pHwRegs>
* a pointer to a NCR810_HW_REGS structure that is filled with the logical
* values 0 or 1 for each bit of each register described below.
*
* This routine includes only the bit registers that can be used to modify 
* the behavior of the chip. The default configuration used during 
* ncr810CtlrCreate() and ncr810CrtlInit() is {0,0,0,0,0,1,0,0,0,0,0}.
*
*.CS
* typedef struct
*   {
*   int stest1Bit7;             /@ Disable external SCSI clock  @/
*   int stest2Bit7;             /@ SCSI control enable          @/
*   int stest2Bit5;             /@ Enable differential SCSI bus @/
*   int stest2Bit2;             /@ Always WIDE SCSI             @/
*   int stest2Bit1;             /@ Extend SREQ/SACK filtering   @/
*   int stest3Bit7;             /@ TolerANT enable              @/
*   int dmodeBit7;              /@ Burst Length transfer bit 1  @/
*   int dmodeBit6;              /@ Burst Length transfer bit 0  @/
*   int dmodeBit5;              /@ Source I/O memory enable     @/
*   int dmodeBit4;              /@ Destination I/O memory enable@/
*   int scntl1Bit7;             /@ Slow cable mode              @/
*   } NCR810_HW_REGS;
*
*.CE
* For a more detailed explanation of the register bits, see the appropriate 
* NCR 53C8xx data manuals.
*
* NOTE
* Because this routine writes to the NCR 53C8xx chip registers, it cannot be
* used when there is any SCSI bus activity.
*
* RETURNS: OK, or ERROR if any input parameter is NULL 
*
* SEE ALSO: ncr810.h, ncr810CtlrCreate()
*/

STATUS ncr810SetHwRegister
    (
    FAST SIOP	   *pSiop,	/* pointer to SIOP info */
    NCR810_HW_REGS *pHwRegs     /* pointer to a NCR810_HW_REGS info */
    )

    {
    /* check input parameters */

    if ((pSiop == NULL) || (pHwRegs == NULL))
	return (ERROR);

    /* fill the SIOP structure with new parameters */

    bcopy ((char *)pHwRegs, (char *) &pSiop->hwRegs, sizeof(NCR810_HW_REGS));

    /* Set each bit register to the new value */

    ((pHwRegs->stest1Bit7) ?
     (NCR810_OUT_BYTE(pSiop->pStest1, NCR810_IN_BYTE(pSiop->pStest1) |
		      STEST1_SCLK)) :
     (NCR810_OUT_BYTE(pSiop->pStest1, NCR810_IN_BYTE(pSiop->pStest1) &
		      ~STEST1_SCLK)));
    ((pHwRegs->stest2Bit7) ?
     (NCR810_OUT_BYTE(pSiop->pStest2, NCR810_IN_BYTE(pSiop->pStest2) |
		      STEST2_SCE)) :
     (NCR810_OUT_BYTE(pSiop->pStest2, NCR810_IN_BYTE(pSiop->pStest2) &
		      ~STEST2_SCE)));

    /* NCR825 specific initialisation */

    if ((pSiop->devType == NCR825_DEVICE_ID) || 
	(pSiop->devType == NCR875_DEVICE_ID))
	{
        CACHE_PIPE_FLUSH();
        ((pHwRegs->stest2Bit5) ?
	 (NCR810_OUT_BYTE(pSiop->pStest2, NCR810_IN_BYTE(pSiop->pStest2) |
			  STEST2_DIF)) :
	 (NCR810_OUT_BYTE(pSiop->pStest2, NCR810_IN_BYTE(pSiop->pStest2) &
		 	  ~STEST2_DIF)));
        CACHE_PIPE_FLUSH();
        ((pHwRegs->stest2Bit2) ?
	 (NCR810_OUT_BYTE(pSiop->pStest2, NCR810_IN_BYTE(pSiop->pStest2) |
			  STEST2_AWS)) :
	 (NCR810_OUT_BYTE(pSiop->pStest2, NCR810_IN_BYTE(pSiop->pStest2) &
			  ~STEST2_AWS)));
        }

    CACHE_PIPE_FLUSH();
    ((pHwRegs->stest2Bit1) ?
     (NCR810_OUT_BYTE(pSiop->pStest2, NCR810_IN_BYTE(pSiop->pStest2) |
		      STEST2_EXT)) :
     (NCR810_OUT_BYTE(pSiop->pStest2, NCR810_IN_BYTE(pSiop->pStest2) &
	 	      ~STEST2_EXT)));
    CACHE_PIPE_FLUSH();
    ((pHwRegs->stest3Bit7) ?
     (NCR810_OUT_BYTE(pSiop->pStest3, NCR810_IN_BYTE(pSiop->pStest3) |
		      STEST3_TE)) :
     (NCR810_OUT_BYTE(pSiop->pStest3, NCR810_IN_BYTE(pSiop->pStest3) &
		      ~STEST3_TE)));
    CACHE_PIPE_FLUSH();
    ((pHwRegs->dmodeBit7)  ?
     (NCR810_OUT_BYTE(pSiop->pDmode, NCR810_IN_BYTE(pSiop->pDmode) |
		      DMODE_BL1)) :
     (NCR810_OUT_BYTE(pSiop->pDmode, NCR810_IN_BYTE(pSiop->pDmode) &
		      ~DMODE_BL1)));
    CACHE_PIPE_FLUSH();
    ((pHwRegs->dmodeBit6)  ?
     (NCR810_OUT_BYTE(pSiop->pDmode, NCR810_IN_BYTE(pSiop->pDmode) |
		      DMODE_BL0)) :
     (NCR810_OUT_BYTE(pSiop->pDmode, NCR810_IN_BYTE(pSiop->pDmode) &
		      ~DMODE_BL0)));
    CACHE_PIPE_FLUSH();
    ((pHwRegs->dmodeBit5)  ?
     (NCR810_OUT_BYTE(pSiop->pDmode, NCR810_IN_BYTE(pSiop->pDmode) |
		      DMODE_SIOM)) :
     (NCR810_OUT_BYTE(pSiop->pDmode, NCR810_IN_BYTE(pSiop->pDmode) &
		      ~DMODE_SIOM)));
    CACHE_PIPE_FLUSH();
    ((pHwRegs->dmodeBit4)  ?
     (NCR810_OUT_BYTE(pSiop->pDmode, NCR810_IN_BYTE(pSiop->pDmode) |
		      DMODE_DIOM)) :
     (NCR810_OUT_BYTE(pSiop->pDmode, NCR810_IN_BYTE(pSiop->pDmode) &
		      ~DMODE_DIOM)));
    CACHE_PIPE_FLUSH();
    ((pHwRegs->scntl1Bit7) ?
     (NCR810_OUT_BYTE(pSiop->pScntl1, NCR810_IN_BYTE(pSiop->pScntl1) |
		      SCNTL1_EXC)) :
     (NCR810_OUT_BYTE(pSiop->pScntl1, NCR810_IN_BYTE(pSiop->pScntl1) &
		      ~SCNTL1_EXC)));
    return (OK);
    }

/*******************************************************************************
*
* ncr810ScsiBusControl - miscellaneous low-level SCSI bus control operations
*
* Currently supports only the SCSI_BUS_RESET operation; other operations are
* not used explicitly by the driver because they are carried out automatically
* by the script program.
*
* NOTE: after the SCSI bus has been reset, the SIOP generates an interrupt
* which causes an NCR810_BUS_RESET event to be sent to the SCSI manager.
* See "ncr810Intr()".
*
* RETURNS: OK, or ERROR if an invalid operation is requested.
*
* NOMANUAL
*/

LOCAL STATUS ncr810ScsiBusControl
    (
    SIOP * pSiop,   	    /* ptr to controller info                   */
    int    operation	    /* bitmask for operation(s) to be performed */
    )
    {
    if ((operation & ~SCSI_BUS_RESET) != 0)
	return (ERROR);
	
    if (operation & SCSI_BUS_RESET)
	ncr810ScsiBusReset (pSiop);

    return (OK);
    }


/*******************************************************************************
*
* ncr810ScsiBusReset - assert the RST line on the SCSI bus 
*
* Issue a SCSI Bus Reset command to the NCR 810. This should put all devices
* on the SCSI bus in an initial quiescent state.
*
* The bus reset will generate an interrupt which is handled by the normal
* ISR (see "ncr810Intr()").
*
* RETURNS: N/A
*
* NOMANUAL
*/

LOCAL void ncr810ScsiBusReset
    (
    FAST SIOP *pSiop	/* ptr to SIOP info */
    )

    {
    NCR810_OUT_BYTE(pSiop->pScntl1, NCR810_IN_BYTE(pSiop->pScntl1) |
		    SCNTL1_RST);

    taskDelay (2);			/* pause for at least 250 us */

    NCR810_OUT_BYTE(pSiop->pScntl1, NCR810_IN_BYTE(pSiop->pScntl1) &
		    ~SCNTL1_RST);
    }


/*******************************************************************************
*
* ncr810Intr - interrupt service routine for the SIOP
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

void ncr810Intr
    (
    SIOP *pSiop
    )
    {
    NCR810_EVENT    event;
    SCSI_EVENT *    pScsiEvent = (SCSI_EVENT *) &event;
    SCSI_CTRL *     pScsiCtrl  = (SCSI_CTRL *)  pSiop;
    NCR810_THREAD * pThread    = (NCR810_THREAD *) pSiop->pHwThread;

    BOOL connected = FALSE;
    BOOL notify    = TRUE;
    int  oldState  = (int) pSiop->state;

    CACHE_PIPE_FLUSH();

    /*
     *	Save (partial) SIOP register context in current thread
     */
    pThread->scratcha0 = NCR810_IN_BYTE(pSiop->pScratcha0);
    pThread->scratcha1 = NCR810_IN_BYTE(pSiop->pScratcha1);
    pThread->scratcha2 = NCR810_IN_BYTE(pSiop->pScratcha2);
    pThread->scratcha3 = NCR810_IN_BYTE(pSiop->pScratcha3);

    if ((pScsiEvent->type = ncr810EventTypeGet (pSiop))
		== (SCSI_EVENT_TYPE)ERROR)
	return;

    SCSI_INT_DEBUG_MSG ("ncr810Intr: DSA = 0x%08x, DSP = 0x%08x, type = %d.\n",
			NCR810_IN_32(pSiop->pDsa), 
			NCR810_IN_32(pSiop->pDsp), 
			pScsiEvent->type, 0, 0, 0);

    /*
     *  Synchronise with single-step routine, if enabled.
     */
    if (pSiop->singleStep)
    	semGive (pSiop->singleStepSem);

    if (pScsiEvent->type == NCR810_SINGLE_STEP)
	return;
    
    /*
     *	Handle interrupt according to type.
     */
    switch (pScsiEvent->type)
	{
	/*
	 *  Following cases result from completion of a script run.
	 */
	case NCR810_CMD_COMPLETE:
	case NCR810_DISCONNECTED:
	    connected = FALSE;
	    break;

    	case NCR810_MESSAGE_OUT_SENT:
    	case NCR810_MESSAGE_IN_RECVD:
	case NCR810_EXT_MESSAGE_SIZE:
	case NCR810_NO_MSG_OUT:
	case NCR810_SCRIPT_ABORTED:
	    connected = TRUE;
	    break;

	case NCR810_PHASE_MISMATCH:
	    event.remCount = ncr810RemainderGet (pSiop, pThread->nBusPhase);
	    
	    connected = TRUE;
	    break;

	/*
	 *  Following cases are asynchronous conditions, i.e. not resulting
	 *  directly from running a script.
	 */
    	case NCR810_READY:
	    connected = FALSE;
	    notify    = FALSE;
	    break;
	    
    	case NCR810_SELECTED:
    	case NCR810_RESELECTED:
	    pScsiEvent->busId = ncr810BusIdGet (pSiop, pThread->nBusIdBits);

	    pScsiEvent->nBytesIdent =
		(pThread->nHostFlags & FLAGS_IDENTIFY) ? 1 : 0;

	    connected = TRUE;
	    break;

	case NCR810_SCSI_BUS_RESET:
	    connected = FALSE;
	    break;

	/*
	 *  Following cases are error conditions (mixture of synchronous
	 *  and asynchronous).
	 */
	case NCR810_SCSI_TIMEOUT:
	case NCR810_HANDSHAKE_TIMEOUT:
	    connected = FALSE;
	    break;

	case NCR810_ILLEGAL_PHASE:
	    connected = TRUE;
	    break;

	case NCR810_UNEXPECTED_DISCON:
	    connected = FALSE;
	    break;

    	case NCR810_NO_IDENTIFY:
	    logMsg ("ncr810Intr: no valid IDENTIFY message at (re)select.\n",
		    0, 0, 0, 0, 0, 0);
	    connected = TRUE;
	    break;
	    
    	case NCR810_SPURIOUS_CMD:
	    logMsg ("ncr810Intr: spurious command interrupt.\n",
		    0, 0, 0, 0, 0, 0);
	    connected = FALSE;
	    break;

	case NCR810_FATAL_ERROR:
	    logMsg ("ncr810Intr: unrecoverable error - re-starting SIOP.\n",
		    0, 0, 0, 0, 0, 0);
	    ncr810HwInit (pSiop);
	    connected = FALSE;
	    break;

	default:
	    SCSI_INT_DEBUG_MSG ("ncr810Intr: unexpected interrupt (%d).\n",
		    pScsiEvent->type, 0, 0, 0, 0, 0);
	    break;
	}

    /*
     *  Controller is now idle: if possible, make it run a script.
     *
     *	If a SCSI thread is suspended and must be processed at task-level,
     *	leave the SIOP idle.  It will be re-started by the SCSI manager
     *	calling "ncr810Resume()".
     *
     *	Otherwise, if there's a new SCSI thread to start (i.e., the SCSI
     *	manager has called "ncr810Activate()"), start the appropriate script.
     *
     *	Otherwise, start a script which puts the SIOP into passive mode
     *	waiting for re-selection, selection or a host command.
     *
     *	In all cases, clear any request to start a new thread.  The only
     *	tricky case is when there was a request pending and the SIOP is
     *	left IDLE.  This should only ever occur when the current event is
     *	selection or reselection, in which case the SCSI manager will retry
     *	the activation request.  (Also see "ncr810Activate ()".)
     */
    if (connected)
	{
    	pSiop->state = NCR810_STATE_IDLE;
	}
    else if (pSiop->cmdPending)
	{
	ncr810ScriptStart (pSiop,
			   (NCR810_THREAD *) pSiop->pNewThread,
			   NCR810_SCRIPT_INIT_START);

	pSiop->state = NCR810_STATE_ACTIVE;
	}
    else
	{
	ncr810ScriptStart (pSiop,
			   (NCR810_THREAD *) pScsiCtrl->pIdentThread,
			   NCR810_SCRIPT_WAIT);
	
	pSiop->state = NCR810_STATE_PASSIVE;
	}

    pSiop->cmdPending = FALSE;

    SCSI_INT_DEBUG_MSG ("ncr810Intr: state %d -> %d\n",
			oldState, pSiop->state, 0, 0, 0, 0);

    /*
     *	Send the event to the SCSI manager to be processed.
     */
    if (notify)
    	scsiMgrEventNotify ((SCSI_CTRL *) pSiop, pScsiEvent, sizeof (event));
    }


/*******************************************************************************
*
* ncr810BusIdGet - find bus ID of device which selected/reselected the SIOP.
*
* Get the Bus ID of the selecting/reselecting device by looking at SSID
* 
* RETURNS: bus ID of connected peer device
*
* NOTE: This routine is called at interrupt level.
*
* NOMANUAL
*/
LOCAL int ncr810BusIdGet
    (
    SIOP * pSiop,			/* ptr to controller info       */
    UINT   busIdBits			/* bits corresponding to bus ID */
    )
    {
    UINT8 devBusId;

    CACHE_PIPE_FLUSH();

    /* HELP: decode busIdBits, don't read the device ??? */
    devBusId = NCR810_IN_BYTE(pSiop->pSsid) & ((UINT8) SSID_ENC_MASK);
    
    return (devBusId);
    }


/*******************************************************************************
*
* ncr810RemainderGet - get remaining xfer count and clean up after mismatch
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

LOCAL UINT ncr810RemainderGet
    (
    SIOP *pSiop,			/* pointer to controller info     */
    UINT  phase				/* phase terminated by mismatch   */
    )
    {
    UINT  remCount;
    UINT8 tmpCount;
    UINT  tmpCountWord;
    UINT8 countFifo;
    UINT8 syncMode;

    CACHE_PIPE_FLUSH();

    /* determine xfer mode */
    syncMode = NCR810_IN_BYTE(pSiop->pSxfer) & SXFER_OFFSET;

    /*
     *	Find remaining byte count (may be corrected later).  For a fuller
     *	explanation, see Chapter 2 of the NCR 53C810 Data Manual.
     */
    remCount = NCR810_IN_32(pSiop->pDbc) & NCR810_COUNT_MASK;

    /*
     *   check for data in fifo and output registers
     *	(SODR & SODL), count from DBC minus DFIFO count and 0x7f
     *	(see NCR 53C810 Data Manual).
     */

    /* look for deep fifo mode on 825/875 */

    tmpCount = 0;
    tmpCountWord = 0;

    if ( ((pSiop->devType == NCR825_DEVICE_ID)  ||
	  (pSiop->devType == NCR875_DEVICE_ID)) &&
	 (NCR810_IN_BYTE(pSiop->pCtest5) & CTEST5_DFS) ) 
	{
	tmpCountWord = ((((UINT)NCR810_IN_BYTE(pSiop->pCtest5)) & 0x0003) << 8);
	tmpCountWord += (UINT)NCR810_IN_BYTE(pSiop->pDfifo);
	tmpCountWord -= (remCount & 0x03FF);
	}
    else
	{
	countFifo = NCR810_IN_BYTE(pSiop->pDfifo) & 0x7f;
	tmpCount  = (UINT8)(countFifo - tmpCount) & 0x7f;
	}

    switch (phase)
        {
        case PHASE_DATA_IN:
        case PHASE_MSG_IN:
	case PHASE_STATUS:

	    if (!syncMode)
		{

		/* Asynchronous Read */

		if (NCR810_IN_BYTE(pSiop->pSstat0) & SSTAT0_ILF)
		    tmpCount++;		

		/* 825/875 Extensions */

		if ((pSiop->devType == NCR825_DEVICE_ID) ||
		    (pSiop->devType == NCR875_DEVICE_ID))
		    {

		    if (NCR810_IN_BYTE(pSiop->pSstat0) & SSTAT0_ILF)
			tmpCount++;

		    if (NCR810_IN_BYTE(pSiop->pSstat2) & SSTAT2_ILF)
			tmpCount++;		

		    }

		}
	    else
		{

		/* Synchronous Read */

		countFifo = (UINT8)((NCR810_IN_BYTE(pSiop->pSstat1) & FIFO_MASK)
			    >> 4);
		countFifo &= 0x0f;
		    
		/* 825/875 Extensions */

		if ((pSiop->devType == NCR825_DEVICE_ID) ||
		    (pSiop->devType == NCR875_DEVICE_ID))
		    {
		    if (NCR810_IN_BYTE(pSiop->pSstat2) & SSTAT2_FF4)
			countFifo |= 0x1f;
		    }

		tmpCount += countFifo;

		}
		
	    /* Check wide SCSI on 825/875 */

	    if ((pSiop->devType == NCR825_DEVICE_ID) ||
		(pSiop->devType == NCR875_DEVICE_ID))
		{
		if (NCR810_IN_BYTE(pSiop->pScntl2) & SCNTL2_WSR)
		    tmpCount++;
		}

	    remCount += tmpCount;
    	    break;

        case PHASE_DATA_OUT:
        case PHASE_MSG_OUT:
	case PHASE_COMMAND:

	    /* Asynch/Synchronous write */

	    if (NCR810_IN_BYTE(pSiop->pSstat0) & SSTAT0_OLF)
		tmpCount++;

	    if ((pSiop->devType == NCR825_DEVICE_ID) ||
		(pSiop->devType == NCR875_DEVICE_ID))
		{
		if (NCR810_IN_BYTE(pSiop->pSstat2) & SSTAT2_OLF)
		    tmpCount++;
		}
		
	    if (syncMode)
		{

		/* Synchronous write */

		if (NCR810_IN_BYTE(pSiop->pSstat0) & SSTAT0_ORF)
		    tmpCount++;

		/* 825/875 Extensions */

		if ((pSiop->devType == NCR825_DEVICE_ID) ||
		    (pSiop->devType == NCR875_DEVICE_ID))
		    {
		    if (NCR810_IN_BYTE(pSiop->pSstat2) & SSTAT2_ORF)
			tmpCount++;
		    } /* 825/875 */

		}
	   
	    remCount += tmpCount;
	    break;
    
        default:
            logMsg ("ncr810RemainderGet: invalid phase.\n", 0, 0, 0, 0, 0, 0);
	    break;
        }

    /* Add in any deep fifo adjustments; this is zero for 88-byte fifos */

    remCount += tmpCountWord;

    /* Clear data FIFOs */

    NCR810_OUT_BYTE(pSiop->pCtest3, NCR810_IN_BYTE(pSiop->pCtest3) |
		    CTEST3_CLF);

    return (remCount);
    }


/******************************************************************************
*
* ncr810EventTypeGet - parse SCSI and DMA status registers at interrupt time
*
* NOTE
* Only status bits which have corresponding interrupts enabled are checked !
*
* RETURNS: an interrupt (event) type code
*
* NOMANUAL
*/

LOCAL int ncr810EventTypeGet
    (
    SIOP * pSiop
    )
    {
    UINT8 istat;
    UINT8 dmaStatus;
    UINT8 sist0;
    UINT8 sist1;
    int key;

    /*
     *	Read interrupt status registers
     *
     *	Note: reading the interrupt status register as a long word
     *  causes the status register to read as NULL. This should not
     *  be the case, however, the workaround is to read the status
     *  registers as 8 bit registers and insert 12 SCLK delays
     *  in between successive reads, as explained in the data manual.
     */

    
    /*
     * Interrupts need to be locked in interrupt context while the status
     * registers are being read so that there is no contention between
     * a synchronous thread and a bus initiated thread (reselect)
     *
     * HELP: Not certain that we need locking in this routine.  We only
     * get here as a result of a SCSI interrupt.  The SIOP should be stopped.
     * Investigate all uses of intLock and IntDisable/IntRestore.
     */

    key = intLock();

    CACHE_PIPE_FLUSH ();

    istat = NCR810_IN_BYTE(pSiop->pIstat); 

    if ((istat & (ISTAT_SIP | ISTAT_DIP)) != 0) 
        {
	if (istat & ISTAT_ABRT)
	    NCR810_OUT_BYTE(pSiop->pIstat, 0x00);

	if ((istat & ISTAT_SIP) && (istat & ISTAT_DIP))
	    {
	    SCSI_INT_DEBUG_MSG ("Both SIP and DIP occurred simultaneously\n",
	                     0,0,0,0,0,0);
	    }

        CACHE_PIPE_FLUSH ();

	dmaStatus  = NCR810_IN_BYTE(pSiop->pDstat);

        CACHE_PIPE_FLUSH ();

        ncr810Delay ();

        sist0 = NCR810_IN_BYTE(pSiop->pSist0);

        CACHE_PIPE_FLUSH ();

        ncr810Delay ();

        sist1 = NCR810_IN_BYTE(pSiop->pSist1);

	intUnlock (key);
        }
    else
	{
	intUnlock (key);

	return (ERROR);
	}

    SCSI_INT_DEBUG_MSG ("ncr810EventTypeGet:"
			"Interrupt status istat = 0x%02x, "
			"SCSI status0 = 0x%02x, "
			"SCSI status1 = 0x%02x"
			"DMA status = 0x%02x\n",
		    	istat, sist0, sist1, dmaStatus, 0, 0);

    /*
     *	Check for fatal errors (first !)
     */
    if  (sist0 & B_SGE)
        {
	logMsg ("ncr810: SCSI bus gross error.\n", 0, 0, 0, 0, 0, 0);
	return (NCR810_FATAL_ERROR);
        }

    if  (sist0 & B_PAR)
        {
	logMsg ("ncr810: parity error.\n", 0, 0, 0, 0, 0, 0);
	return (NCR810_FATAL_ERROR);
        }

    if (dmaStatus & B_IID)
        {
	logMsg ("ncr810: illegal instruction (DSP = 0x%08x).\n",
		NCR810_IN_32(pSiop->pDsp), 0, 0, 0, 0, 0);
	return (NCR810_FATAL_ERROR);
        }

    if (dmaStatus & B_BF)
        {
	logMsg ("ncr810: bus fault (DSPS = 0x%08x) DBC = (0x%08x).\n",
		NCR810_IN_32(pSiop->pDsps), 
		NCR810_IN_32(pSiop->pDbc) & 0xffffff, 0, 0, 0, 0);
	return (NCR810_FATAL_ERROR);
        }

    /*
     *	No fatal errors; try the rest (NB order of tests is important !)
     */
    if  (sist0 & B_RST)
        {
	SCSI_INT_DEBUG_MSG ("ncr810EventTypeGet: SCSI bus reset.\n",
			    0, 0, 0, 0, 0, 0);
        CACHE_PIPE_FLUSH();

	NCR810_OUT_BYTE(pSiop->pCtest3, NCR810_IN_BYTE(pSiop->pCtest3) |
			CTEST3_CLF);	/* clear FIFOs */
	
	return (NCR810_SCSI_BUS_RESET);
        }

    if  (sist0 & B_UDC)
        {
	SCSI_INT_DEBUG_MSG ("ncr810EventTypeGet: unexpected disconnection.\n",
			    0, 0, 0, 0, 0, 0);

	/* clear FIFOs */

        CACHE_PIPE_FLUSH();

	NCR810_OUT_BYTE(pSiop->pCtest3, NCR810_IN_BYTE(pSiop->pCtest3) |
			CTEST3_CLF);
	
	return (NCR810_UNEXPECTED_DISCON);
        }

    if (sist1 & B_STO)
	{
    	SCSI_INT_DEBUG_MSG ("ncr810EventTypeGet: SCSI bus timeout.\n",
			    0, 0, 0, 0, 0, 0);

	/* clear FIFOs */

        CACHE_PIPE_FLUSH();

	NCR810_OUT_BYTE(pSiop->pCtest3, NCR810_IN_BYTE(pSiop->pCtest3) |
			CTEST3_CLF);
	
	return (NCR810_SCSI_TIMEOUT);
	}

    if (sist1 & B_HTH)
	{
    	SCSI_INT_DEBUG_MSG ("ncr810EventTypeGet: SCSI handshake timeout.\n",
			    0, 0, 0, 0, 0, 0);

	/* clear FIFOs */

        CACHE_PIPE_FLUSH();

	NCR810_OUT_BYTE(pSiop->pCtest3, NCR810_IN_BYTE(pSiop->pCtest3) |
			CTEST3_CLF);
	
	return (NCR810_HANDSHAKE_TIMEOUT);
	}

    if (sist0 & B_MA)
        {
	/*
	 *  The "phase mismatch" interrupt is used to indicate assertion of
	 *  ATN in target mode.  This interrupt should have been disabled
	 *  by the script when in target mode, because the current code does
	 *  not know how to handle it.
	 */

        CACHE_PIPE_FLUSH();

	if (NCR810_IN_BYTE(pSiop->pScntl0) & SCNTL0_TRG)
	    {
	    if (NCR810_IN_BYTE(pSiop->pSien0) & B_MA)
		{
		logMsg ("ncr810EventTypeGet: enabled ATN interrupt!\n",
			0, 0, 0, 0, 0, 0);
		return (NCR810_FATAL_ERROR);
		}
	    }
	else
	    {
	    SCSI_INT_DEBUG_MSG ("ncr810EventTypeGet: phase mismatch.\n",
			    0, 0, 0, 0, 0, 0);
	    return (NCR810_PHASE_MISMATCH);
	    }
        }

    if (dmaStatus & B_ABT)
        {
	SCSI_INT_DEBUG_MSG ("ncr810EventTypeGet: script aborted.\n",
		    	    0, 0, 0, 0, 0, 0);
	return (NCR810_SCRIPT_ABORTED);
        }

    if (dmaStatus & B_SIR)
	{
	SCSI_INT_DEBUG_MSG ("ncr810EventTypeGet: script completed.\n",
			    0, 0, 0, 0, 0, 0);
	/* HELP: This doesn't look like a valid return value */
	return ( NCR810_IN_32(pSiop->pDsps) );
        }

    if (dmaStatus & B_SSI)
	{
	SCSI_INT_DEBUG_MSG ("ncr810EventTypeGet: single-step.\n",
			    0, 0, 0, 0, 0, 0);

	return (NCR810_SINGLE_STEP);
	}

    /*
     *	No (expected) reason for the interrupt !
     */
    logMsg ("ncr810EventTypeGet: spurious interrupt !\n", 0, 0, 0, 0, 0, 0);
    logMsg ("\tistat: (0x%x) dstat: (0x%x)\n", istat, dmaStatus, 0, 0, 0, 0); 
    logMsg ("\tsist0: (0x%x) sist1: (0x%x)\n", sist0, sist1, 0, 0, 0, 0);
    
    return (NCR810_FATAL_ERROR);
    }


/*******************************************************************************
*
* ncr810ThreadActivate - activate a SCSI connection for an initiator thread
*
* Set whatever thread/controller state variables need to be set.  Ensure that
* all buffers used by the thread are coherent with the contents of the
* system caches (if any).
*
* Set transfer parameters for the thread based on what its target device
* last negotiated.
*
* Update the thread context (including shared memory area) and note that
* there is a new client script to be activated (see "ncr810Activate()").
*
* Set the thread's state to ESTABLISHED.  This is strictly a bit premature,
* but there is no distinction as far as this driver is concerned between a
* thread which is connecting and one which has connected (i.e., the script
* just runs to completion and we have to examine its exit status to determine
* how far through the process the thread got).
*
* Do not wait for the script to be activated.  Completion of the script will
* be signalled by an event which is handled by "ncr810Event()".
*
* RETURNS: OK or ERROR
*
* NOMANUAL
*/
LOCAL STATUS ncr810ThreadActivate
    (
    SIOP *          pSiop,		/* ptr to controller info */
    NCR810_THREAD * pThread		/* ptr to thread info     */
    )
    {
    SCSI_CTRL *   pScsiCtrl   = (SCSI_CTRL *)   pSiop;
    SCSI_THREAD * pScsiThread = (SCSI_THREAD *) pThread;
    SCSI_TARGET * pScsiTarget = pScsiThread->pScsiTarget;
    SCSI_THREAD * pOrigThread;
    
    SCSI_DEBUG_MSG ("ncr810ThreadActivate: thread 0x%08x: activating\n",
		    (int) pThread, 0, 0, 0, 0, 0);

    scsiCacheSynchronize (pScsiThread, SCSI_CACHE_PRE_COMMAND);

    /*
     *	Reset controller state variables: set sync xfer parameters
     */
    pScsiCtrl->msgOutState = SCSI_MSG_OUT_NONE;
    pScsiCtrl->msgInState  = SCSI_MSG_IN_NONE;
    
    pOrigThread = pScsiCtrl->pThread;
    pScsiCtrl->pThread = pScsiThread;

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

    if (ncr810ThreadParamsSet (pThread, pScsiTarget->xferOffset,
			     	        pScsiTarget->xferPeriod) != OK)
	{
	SCSI_ERROR_MSG ("ncr810ThreadActivate: failed to set thread params.\n",
			0, 0, 0, 0, 0, 0);
        pScsiCtrl->pThread = pOrigThread; 
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

    bcopy ((char *)pScsiThread->identMsg,
           (char *)pSiop->identMsg,
           pScsiThread->identMsgLength);

    pSiop->identMsgLength += pScsiThread->identMsgLength;

    if (pScsiCtrl->msgOutState == SCSI_MSG_OUT_PENDING)
        {
        bcopy ((char *)pScsiCtrl->msgOutBuf,
               (char *)pSiop->identMsg + pSiop->identMsgLength,
               pScsiCtrl->msgOutLength);

        pSiop->identMsgLength += pScsiCtrl->msgOutLength;
        }

    /*
     *	Update thread context; activate the thread
     */

    ncr810ThreadUpdate (pThread);
    
    if (ncr810Activate (pSiop, pThread) != OK)
	{
	SCSI_ERROR_MSG ("ncr810ThreadActivate: failed to activate thread.\n",
		        0, 0, 0, 0, 0, 0);
        pScsiCtrl->pThread = pOrigThread; 
	return (ERROR);
	}

    pScsiCtrl->pThread = pScsiThread;

    ncr810ThreadStateSet (pThread, SCSI_THREAD_ESTABLISHED);
    
    return (OK);
    }


/*******************************************************************************
*
* ncr810ThreadAbort - abort a thread
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
LOCAL BOOL ncr810ThreadAbort
    (
    SIOP *          pSiop,		/* ptr to controller info */
    NCR810_THREAD * pThread		/* ptr to thread info     */
    )
    {
    BOOL          tagged;
    SCSI_CTRL *   pScsiCtrl   = (SCSI_CTRL *)   pSiop;
    SCSI_THREAD * pScsiThread = (SCSI_THREAD *) pThread;

    SCSI_DEBUG_MSG ("ncr810ThreadAbort: thread 0x%08x (state = %d) aborting\n",
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
	    tagged = (pScsiThread->tagNumber != (UINT)NONE);

	    pScsiCtrl->msgOutBuf[0] = tagged ? SCSI_MSG_ABORT_TAG
		    	    	    	     : SCSI_MSG_ABORT;
	    pScsiCtrl->msgOutLength = 1;
	    pScsiCtrl->msgOutState  = SCSI_MSG_OUT_PENDING;

	    ncr810Abort (pSiop);
	    
    	    ncr810ThreadStateSet (pThread, SCSI_THREAD_ABORTING);
	    break;
	}

    return (TRUE);
    }


/*******************************************************************************
*
* ncr810Event - NCR 53C810 SCSI controller event processing routine
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
LOCAL void ncr810Event
    (
    SIOP *         pSiop,
    NCR810_EVENT * pEvent
    )
    {
    SCSI_CTRL  *    pScsiCtrl  = (SCSI_CTRL *)  pSiop;
    SCSI_EVENT *    pScsiEvent = (SCSI_EVENT *) pEvent;
    NCR810_THREAD * pThread    = (NCR810_THREAD *) pScsiCtrl->pThread;
    
    SCSI_DEBUG_MSG ("ncr810Event: received event %d (thread = 0x%08x)\n",
		    pScsiEvent->type, (int) pThread, 0, 0, 0, 0);

    /*
     *	Do controller-level event processing
     */
    switch (pScsiEvent->type)
	{
	case NCR810_SELECTED:
	case NCR810_RESELECTED:
	    /*
	     *	Forward event to current thread, if any (it should defer)
	     *	then install a reserved thread for identification purposes.
	     */
    	    if (pThread != 0)
	    	ncr810ThreadEvent (pThread, pEvent);

    	    pScsiCtrl->peerBusId = pScsiEvent->busId;

    	    pScsiCtrl->pThread = pScsiCtrl->pIdentThread;
	    
	    pScsiCtrl->pThread->role = (pScsiEvent->type == NCR810_SELECTED)
		    	    	     ? SCSI_ROLE_IDENT_TARG
				     : SCSI_ROLE_IDENT_INIT;

	    pThread = (NCR810_THREAD *) pScsiCtrl->pThread;

	    scsiMgrCtrlEvent (pScsiCtrl, SCSI_EVENT_CONNECTED);
	    break;

	case NCR810_DISCONNECTED:
	case NCR810_CMD_COMPLETE:
	case NCR810_UNEXPECTED_DISCON:
	case NCR810_SCSI_TIMEOUT:
    	    pScsiCtrl->peerBusId = NONE;
    	    pScsiCtrl->pThread   = 0;

	    scsiMgrCtrlEvent (pScsiCtrl, SCSI_EVENT_DISCONNECTED);
	    
	    break;

	case NCR810_SCSI_BUS_RESET:
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
	    	logMsg ("ncr810Event: invalid event type (%d)\n",
		    	pScsiEvent->type, 0, 0, 0, 0, 0);
		}
	    break;
	}
    /*
     *	If there's a thread on the controller, forward the event to it
     */
    if (pThread != 0)
	ncr810ThreadEvent (pThread, pEvent);
    }

    
/*******************************************************************************
*
* ncr810ThreadEvent - NCR 53C810 thread event processing routine
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
LOCAL void ncr810ThreadEvent
    (
    NCR810_THREAD * pThread,
    NCR810_EVENT *  pEvent
    )
    {
    SCSI_EVENT *  pScsiEvent  = (SCSI_EVENT *)  pEvent;
    SCSI_THREAD * pScsiThread = (SCSI_THREAD *) pThread;
    SIOP *        pSiop       = (SIOP *)        pScsiThread->pScsiCtrl;
    NCR810_SCRIPT_ENTRY entryPt;
    
    SCSI_DEBUG_MSG ("ncr810ThreadEvent: thread 0x%08x: received event %d\n",
		    (int) pThread, pScsiEvent->type, 0, 0, 0, 0);

    switch (pScsiThread->role)
	{
	case SCSI_ROLE_INITIATOR:
	    ncr810InitEvent (pThread, pEvent);

	    entryPt = NCR810_SCRIPT_INIT_CONTINUE;
	    break;
	    
	case SCSI_ROLE_IDENT_INIT:
	    ncr810InitIdentEvent (pThread, pEvent);

	    entryPt = NCR810_SCRIPT_INIT_CONTINUE;
	    break;

	case SCSI_ROLE_IDENT_TARG:
	    ncr810TargIdentEvent (pThread, pEvent);

	    entryPt = NCR810_SCRIPT_TGT_DISCONNECT;
	    break;
	    
	case SCSI_ROLE_TARGET:
	default:
	    logMsg ("ncr810ThreadEvent: thread 0x%08x: invalid role (%d)\n",
		    (int) pThread, pScsiThread->role, 0, 0, 0, 0);
	    entryPt = NCR810_SCRIPT_TGT_DISCONNECT;
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
    	    ncr810ThreadUpdate (pThread);

	    if (ncr810Resume (pSiop, pThread, entryPt) != OK)
	    	{
	    	SCSI_ERROR_MSG ("ncr810ThreadEvent: failed to resume thread\n",
			    	0, 0, 0, 0, 0, 0);
	
		ncr810ThreadFail (pThread, S_scsiLib_DISCONNECTED);
	    	}
	    break;
	}
    }


/*******************************************************************************
*
* ncr810InitEvent - NCR 53C810 initiator thread event processing routine
*
* Parse the event type and handle it accordingly.  This may result in state
* changes for the thread, state variables being updated, etc.
*
* RETURNS: N/A
*
* NOMANUAL
*/
LOCAL void ncr810InitEvent
    (
    NCR810_THREAD * pThread,
    NCR810_EVENT *  pEvent
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
	case NCR810_DISCONNECTED:
    	    SCSI_DEBUG_MSG ("DISCONNECT message in\n", 0, 0, 0, 0, 0, 0);

	    scsiMgrThreadEvent (pScsiThread, SCSI_THREAD_EVENT_DISCONNECTED);

	    ncr810ThreadStateSet (pThread, SCSI_THREAD_DISCONNECTED);
	    break;

	case NCR810_CMD_COMPLETE:
	    SCSI_DEBUG_MSG ("COMMAND COMPLETE message in\n", 0, 0, 0, 0, 0, 0);

	    ncr810ThreadComplete (pThread);
	    break;

	case NCR810_SELECTED:
	case NCR810_RESELECTED:
    	    SCSI_DEBUG_MSG ("ncr810InitEvent: thread 0x%08x: (re)selection.\n",
		    	    (int) pThread, 0, 0, 0, 0, 0);
	    ncr810ThreadDefer (pThread);
	    break;

    	case NCR810_MESSAGE_OUT_SENT:
	    (void) scsiMsgOutComplete (pScsiCtrl, pScsiThread);
	    break;
	    
	case NCR810_MESSAGE_IN_RECVD:
	    (void) scsiMsgInComplete (pScsiCtrl, pScsiThread);
	    break;
	    
	case NCR810_NO_MSG_OUT:
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
	    
	case NCR810_EXT_MESSAGE_SIZE:
	    /*
	     *  The SIOP has just read the length byte for an extended
	     *  message in.  The shared memory area is updated with the
	     *	appropriate length just before the thread is resumed (see
	     *	"ncr810ThreadUpdate()".
	     */
	    break;
	    
	case NCR810_PHASE_MISMATCH:
	    if (ncr810PhaseMismatch (pThread,
				     pThread->nBusPhase,
				     pEvent->remCount) != OK)
		{
		ncr810ThreadFail (pThread, errno);
		}
	    break;

	case NCR810_SCSI_TIMEOUT:
	    SCSI_ERROR_MSG ("ncr810InitEvent: thread 0x%08x: select timeout.\n",
			    (int) pThread, 0, 0, 0, 0, 0);

	    ncr810ThreadFail (pThread, S_scsiLib_SELECT_TIMEOUT);
	    break;

	case NCR810_SCRIPT_ABORTED:
	    SCSI_DEBUG_MSG ("ncr810InitEvent: thread 0x%08x: aborted\n",
			    (int) pThread, 0, 0, 0, 0, 0);
	    break;
	    
	case NCR810_SCSI_BUS_RESET:
    	    SCSI_DEBUG_MSG ("ncr810InitEvent: thread 0x%08x: bus reset\n",
		    	    (int) pThread, 0, 0, 0, 0, 0);
	    /*
	     *	Do not try to resume this thread.  SCSI mgr will tidy up.
	     */
	    ncr810ThreadStateSet (pThread, SCSI_THREAD_INACTIVE);
	    break;

	case NCR810_UNEXPECTED_DISCON:
	    /* not really unexpected after an abort message ... */
    	    SCSI_ERROR_MSG ("ncr810InitEvent: thread 0x%08x: "
		    	    "unexpected disconnection\n",
		    	    (int) pThread, 0, 0, 0, 0, 0);

	    ncr810ThreadFail (pThread, S_scsiLib_DISCONNECTED);
	    break;
	    
	case NCR810_ILLEGAL_PHASE:
	    SCSI_ERROR_MSG ("ncr810InitEvent: thread 0x%08x: "
			    "illegal phase requested.\n",
			    (int) pThread, 0, 0, 0, 0, 0);

	    ncr810ThreadFail (pThread, S_scsiLib_INVALID_PHASE);
	    break;

	default:
	    logMsg ("ncr810InitEvent: invalid event type (%d)\n",
		    pScsiEvent->type, 0, 0, 0, 0, 0);
	    break;
	}
    }

    
/*******************************************************************************
*
* ncr810InitIdentEvent - NCR 53C810 identification thread event processing 
*
* Parse the event type and handle it accordingly.  This may result in state
* changes for the thread, state variables being updated, etc.
*
* RETURNS: N/A
*
* NOMANUAL
*/
LOCAL void ncr810InitIdentEvent
    (
    NCR810_THREAD * pThread,
    NCR810_EVENT *  pEvent
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
	case NCR810_RESELECTED:
    	    pScsiThread->nBytesIdent = pScsiEvent->nBytesIdent;

	    bcopy ((char *) pScsiCtrl->identBuf,
		   (char *) pScsiThread->identMsg,
		   pScsiThread->nBytesIdent);

	    ncr810ThreadStateSet (pThread, SCSI_THREAD_IDENT_IN);

    	    ncr810IdentInContinue (pThread);
	    break;

	case NCR810_MESSAGE_OUT_SENT:
	    /*
	     *	This will be after we have sent an "ABORT (TAG)" msg.
	     *	The target will disconnect any time; it may have already
	     *	done so, in which case we won't be able to resume the
	     *	thread, but no matter.
	     */
	    break;

	case NCR810_MESSAGE_IN_RECVD:
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

    	    ncr810IdentInContinue (pThread);
	    break;

	case NCR810_SCRIPT_ABORTED:
	    SCSI_DEBUG_MSG ("ncr810InitIdentEvent: thread 0x%08x: aborted\n",
			    (int) pThread, 0, 0, 0, 0, 0);
	    break;

        case NCR810_DISCONNECTED:
	    SCSI_DEBUG_MSG ("ncr810InitIdentEvent: thread 0x%08x:"
			    "disconnected\n",
			    (int) pThread, 0, 0, 0, 0, 0);

	    ncr810ThreadFail (pThread, S_scsiLib_DISCONNECTED);
	    break;
	    
	case NCR810_SCSI_BUS_RESET:
    	    SCSI_DEBUG_MSG ("ncr810InitIdentEvent: thread 0x%08x: bus reset\n",
		    	    (int) pThread, 0, 0, 0, 0, 0);

	    /*
	     *	Do not try to resume this thread.  SCSI mgr will tidy up.
	     */
	    ncr810ThreadStateSet (pThread, SCSI_THREAD_INACTIVE);
	    break;

	case NCR810_UNEXPECTED_DISCON:
	    /* not really unexpected after an abort message ... */
    	    SCSI_ERROR_MSG ("ncr810InitIdentEvent: thread 0x%08x: "
		    	    "unexpected disconnection\n",
		    	    (int) pThread, 0, 0, 0, 0, 0);

	    ncr810ThreadFail (pThread, S_scsiLib_DISCONNECTED);
	    break;
	    
	case NCR810_ILLEGAL_PHASE:
	    SCSI_ERROR_MSG ("ncr810InitIdentEvent: thread 0x%08x: "
			    "illegal phase requested.\n",
			    (int) pThread, 0, 0, 0, 0, 0);
	    
	    ncr810ThreadFail (pThread, S_scsiLib_INVALID_PHASE);
	    break;

	default:
	    logMsg ("ncr810InitIdentEvent: invalid event type (%d)\n",
		    pScsiEvent->type, 0, 0, 0, 0, 0);
	    break;
	}
    }

    
/*******************************************************************************
*
* ncr810TargIdentEvent - NCR 53C810 identification thread event processing 
*
* Parse the event type and handle it accordingly.  This may result in state
* changes for the thread, state variables being updated, etc.
*
* RETURNS: N/A
*
* NOMANUAL
*/
LOCAL void ncr810TargIdentEvent
    (
    NCR810_THREAD * pThread,
    NCR810_EVENT *  pEvent
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
	case NCR810_SELECTED:
    	    pScsiThread->nBytesIdent = pScsiEvent->nBytesIdent;

	    bcopy ((char *) pScsiCtrl->identBuf,
		   (char *) pScsiThread->identMsg,
		   pScsiThread->nBytesIdent);

	    ncr810ThreadStateSet (pThread, SCSI_THREAD_IDENT_IN);
	    break;
	    
        case NCR810_DISCONNECTED:
	    SCSI_DEBUG_MSG ("ncr810TargIdentEvent: thread 0x%08x:"
		       	    " disconnected\n",
			    (int) pThread, 0, 0, 0, 0, 0);

	    ncr810ThreadStateSet (pThread, SCSI_THREAD_INACTIVE);
	    break;
	    
	case NCR810_SCSI_BUS_RESET:
    	    SCSI_DEBUG_MSG ("ncr810TargIdentEvent: thread 0x%08x: bus reset\n",
		    	    (int) pThread, 0, 0, 0, 0, 0);
	    
	    /*
	     *	Do not try to resume this thread.  SCSI mgr will tidy up.
	     */
	    ncr810ThreadStateSet (pThread, SCSI_THREAD_INACTIVE);
	    break;

	case NCR810_UNEXPECTED_DISCON:
    	    SCSI_ERROR_MSG ("ncr810TargIdentEvent: thread 0x%08x: "
		    	    "unexpected disconnection\n",
		    	    (int) pThread, 0, 0, 0, 0, 0);

	    ncr810ThreadStateSet (pThread, SCSI_THREAD_INACTIVE);
	    break;
	    
	default:
	    logMsg ("ncr810TargIdentEvent: invalid event type (%d)\n",
		    pScsiEvent->type, 0, 0, 0, 0, 0);
	    break;
	}

    }

    
/*******************************************************************************
*
* ncr810PhaseMismatch - recover from a SCSI bus phase mismatch
*
* This routine does whatever is required to keep the pointers, counts, etc.
* used by task-level software in step when a SCSI phase mismatch occurs.
*
* The interrupt-level mismatch processing has stored the phase of the
* information transfer before the mismatch, and the number of bytes
* remaining to be transferred.  See "ncr810RemainderGet()".
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
LOCAL STATUS ncr810PhaseMismatch
    (
    NCR810_THREAD  * pThread,		/* ptr to thread info           */
    int              phase,		/* bus phase before mismatch    */
    UINT             remCount		/* # bytes not yet transferred  */
    )
    {
    SCSI_THREAD *pScsiThread = (SCSI_THREAD *) pThread;
    SCSI_CTRL *   pScsiCtrl   = pScsiThread->pScsiCtrl;    
    UINT xferCount;

    switch (phase)
	{
	case SCSI_DATA_IN_PHASE:
	case SCSI_DATA_OUT_PHASE:
	    xferCount = NCR810_SWAP_32(pThread->pShMem->data.size) - remCount;
	    pScsiThread->activeDataAddress += xferCount;
	    pScsiThread->activeDataLength  -= xferCount;
	    
	    SCSI_DEBUG_MSG ("ncr810PhaseMismatch: data transfer aborted "
			    "(%d of %d bytes transferred).\n",
			    xferCount, 
			    NCR810_SWAP_32(pThread->pShMem->data.size), 
			    0, 0, 0, 0);
	    break;
	    
	case SCSI_MSG_OUT_PHASE:
            
	    SCSI_DEBUG_MSG("ncr810PhaseMismatch: message out aborted "
			   "(%d of %d bytes sent).\n",
			   pScsiThread->pScsiCtrl->msgOutLength,
			   pScsiThread->pScsiCtrl->msgOutLength - remCount,
			   0, 0, 0, 0);
            
	    /* Abort the Out message */

	    pScsiCtrl->msgOutLength = 0;
	    pScsiCtrl->msgOutState  = SCSI_MSG_OUT_NONE;
	    ncr810ResetAtn (pScsiCtrl);
            
	    break;
	    
	case SCSI_MSG_IN_PHASE:
	    SCSI_DEBUG_MSG("ncr810PhaseMismatch: message in aborted "
			   "(%d bytes received).\n",
			   pScsiThread->pScsiCtrl->msgInLength,
			   0, 0, 0, 0, 0);
	    break;

	case SCSI_COMMAND_PHASE:
	case SCSI_STATUS_PHASE:
	    SCSI_ERROR_MSG ("ncr810PhaseMismatch: unsupported phase (%d).\n",
			    phase, 0, 0, 0, 0, 0);
	    return (ERROR);
	    
	default:
	    logMsg ("ncr810PhaseMismatch: invalid phase (%d).\n",
		    phase, 0, 0, 0, 0, 0);
	    return (ERROR);
        }

    return (OK);
    }


/*******************************************************************************
*
* ncr810IdentInContinue - continue incoming identification
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
LOCAL void ncr810IdentInContinue
    (
    NCR810_THREAD * pThread
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
	    	ncr810ThreadReconnect ((NCR810_THREAD *) pNewThread);

	    	state = SCSI_THREAD_INACTIVE;
		}
	    break;

	case SCSI_IDENT_FAILED:
	    state = SCSI_THREAD_IDENT_ABORTING;
	    break;

	default:
	    logMsg ("ncr810IdentInContinue: invalid ident status (%d)\n",
    	    	    status, 0, 0, 0, 0, 0);
	    state = SCSI_THREAD_INACTIVE;
	    break;
	}

    if (state == SCSI_THREAD_IDENT_ABORTING)
	ncr810ThreadAbort ((SIOP *) pScsiCtrl, pThread);

    ncr810ThreadStateSet (pThread, state);
    }


/*******************************************************************************
*
* ncr810ThreadReconnect - reconnect a thread
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
LOCAL void ncr810ThreadReconnect
    (
    NCR810_THREAD * pThread
    )
    {
    SCSI_THREAD * pScsiThread = (SCSI_THREAD *) pThread;
    SCSI_CTRL   * pScsiCtrl   = pScsiThread->pScsiCtrl;
    SIOP        * pSiop       = (SIOP *) pScsiCtrl;
    
    SCSI_DEBUG_MSG ("ncr810ThreadReconnect: reconnecting thread 0x%08x\n",
		    (int) pThread, 0, 0, 0, 0, 0);

    pScsiCtrl->pThread = pScsiThread;

    /* Implied RESTORE POINTERS action: see "scsiMsgInComplete ()" */

    pScsiThread->activeDataAddress = pScsiThread->savedDataAddress;
    pScsiThread->activeDataLength  = pScsiThread->savedDataLength;

    ncr810ThreadUpdate (pThread);

    if (ncr810Resume (pSiop, pThread, NCR810_SCRIPT_INIT_CONTINUE) != OK)
	{
	SCSI_ERROR_MSG ("ncr810ThreadReconnect: failed to resume thread.\n",
			0, 0, 0, 0, 0, 0);

	ncr810ThreadFail (pThread, S_scsiLib_DISCONNECTED);
	return;
	}

    ncr810ThreadStateSet (pThread, SCSI_THREAD_ESTABLISHED);
    }


/*******************************************************************************
*
* ncr810SharedMemInit - initialize the fields in a shared memory area
*
* Initialise pointers and counts for all message transfers.  These are
* always directed to buffers provided by the SCSI_CTRL structure.
*
* RETURNS: N/A
*
* NOMANUAL
*/
LOCAL void ncr810SharedMemInit
    (
    SIOP *          pSiop,
    NCR810_SHARED * pShMem
    )
    {

    /*
     * Note: All the values being set in the shared memory need to be
     * 32 bit byte swapped. Actual swapping takes place when, when memory
     * is big endian (since the NCR8xx is a little endian PCI device. 
     * Otherwise, if memory is little endian then the byte swapping 
     * does not do anything. The "endianness" is configurable at compile
     * time.
     *
     * Similarly, a PCI offset is added to all addresses so that the
     * NCR8xx PCI chip can get to memory. This value is also configurable
     * at compile time.
     */

    pShMem->identIn.size = NCR810_SWAP_32(1);
    pShMem->identIn.addr = (UINT8 *) NCR810_SWAP_32(
					 NCR810_VIRT_TO_PHYS 
					            (pSiop->scsiCtrl.identBuf)
					 );

    pShMem->msgOut.size = NCR810_SWAP_32(0);		/* set dynamically */
    pShMem->msgOut.addr = (UINT8 *) NCR810_SWAP_32(
				        NCR810_VIRT_TO_PHYS 
						   (pSiop->scsiCtrl.msgOutBuf)
					);
    
    pShMem->msgIn.size  = NCR810_SWAP_32(1);
    pShMem->msgIn.addr  = (UINT8 *) NCR810_SWAP_32(
					NCR810_VIRT_TO_PHYS 
						   (pSiop->scsiCtrl.msgInBuf)
					);

    pShMem->msgInSecond.size = NCR810_SWAP_32(1);
    pShMem->msgInSecond.addr = (UINT8 *) NCR810_SWAP_32(
					     NCR810_VIRT_TO_PHYS 
					             (pSiop->scsiCtrl.msgInBuf)
						      + (UINT) 1
					     );

    pShMem->msgInRest.size   = NCR810_SWAP_32(0);  	/* set dynamically */
    pShMem->msgInRest.addr   = (UINT8 *) NCR810_SWAP_32
					     (
					     NCR810_VIRT_TO_PHYS
						     (pSiop->scsiCtrl.msgInBuf)
						     + (UINT) 2
					     );
    }


/*******************************************************************************
*
* ncr810ThreadInit - initialize a client thread structure
*
* Initialize the fixed data for a thread (i.e., independent of the command).
* Called once when a thread structure is first created.
*
* RETURNS: OK, or ERROR if an error occurs
*
* NOMANUAL
*/

LOCAL STATUS ncr810ThreadInit
    (
    SIOP *          pSiop,
    NCR810_THREAD * pThread
    )
    {
    if (scsiThreadInit (&pThread->scsiThread) != OK)
	return (ERROR);
    
    pThread->pShMem = pSiop->pClientShMem;
    pThread->scntl3 = pSiop->clkDiv;

    return (OK);
    }

    
/*******************************************************************************
*
* ncr810IdentThreadInit - initialize an identification thread structure
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

LOCAL STATUS ncr810IdentThreadInit
    (
    NCR810_THREAD * pThread
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

    pThread->sxfer = NCR810_SYNC_XFER_PARAMS_ASYNC;

/* XXX hard code for now. Will need to REDISIGN */
  /*  pThread->scntl3 = pSiop->clkDiv; */
    pThread->scntl3 = 0x3; 

    /*
     *	Initialise shared memory area
     */
    pThread->pShMem = pSiop->pIdentShMem;
    
    ncr810SharedMemInit (pSiop, pThread->pShMem);

    ncr810ThreadUpdate (pThread);
    
    return (OK);
    }


/*******************************************************************************
*
* ncr810ThreadUpdate - update the thread structure for a current SCSI command
*
* Update the dynamic data (e.g. data pointers, transfer parameters) in
* the thread to reflect the latest state of the corresponding physical device.
*
* RETURNS: N/A
*
* NOMANUAL
*/

LOCAL void ncr810ThreadUpdate
    (
    NCR810_THREAD * pThread		/* thread info */
    )
    {
    SCSI_THREAD   * pScsiThread = (SCSI_THREAD *) pThread;
    SCSI_CTRL     * pScsiCtrl   = pScsiThread->pScsiCtrl;
    SIOP          * pSiop       = (SIOP *) pScsiCtrl;
    NCR810_SHARED * pShMem      = pThread->pShMem;
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
    pThread->nHostFlags   = (UCHAR)flags;
    pThread->nMsgOutState = (UCHAR)pScsiCtrl->msgOutState;
    pThread->nMsgInState  = (UCHAR)pScsiCtrl->msgInState;

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
    pShMem->command.size  = NCR810_SWAP_32(pScsiThread->cmdLength);
    pShMem->command.addr  = (UINT8 *) NCR810_SWAP_32
					  (
					  (UINT32) NCR810_VIRT_TO_PHYS
					  ((UINT) pScsiThread->cmdAddress)
					  );
    pShMem->data.size     = NCR810_SWAP_32(pScsiThread->activeDataLength);
    pShMem->data.addr     = (UINT8 *) NCR810_SWAP_32( 
					  (UINT32) NCR810_VIRT_TO_PHYS
					((UINT) pScsiThread->activeDataAddress)
					  );
    pShMem->status.size   = NCR810_SWAP_32(pScsiThread->statusLength);
    pShMem->status.addr   = (UINT8 *) NCR810_SWAP_32
					  (
					  (UINT32) NCR810_VIRT_TO_PHYS
					  ((UINT) pScsiThread->statusAddress)
					  );

    /* These used to be pScsiThread->identMsg/identMsgLength */

    pShMem->identOut.size = NCR810_SWAP_32(pSiop->identMsgLength);
    pShMem->identOut.addr = (UINT8 *) NCR810_SWAP_32
					  (
					  (UINT32) NCR810_VIRT_TO_PHYS
					  ((UINT) pSiop->identMsg)
					  );

    pShMem->msgOut.size    = NCR810_SWAP_32(msgOutSize);
    pShMem->msgInRest.size = NCR810_SWAP_32(msgInSize);
    }


/*******************************************************************************
*
* ncr810ThreadComplete - successfully complete execution of a client thread
*
* Set the thread status and errno appropriately, depending on whether or
* not the thread has been aborted.  Set the thread inactive, and notify
* the SCSI manager of the completion.
*
* RETURNS: N/A
*
* NOMANUAL
*/
LOCAL void ncr810ThreadComplete
    (
    NCR810_THREAD * pThread
    )
    {
    SCSI_THREAD * pScsiThread = (SCSI_THREAD *) pThread;
    
    SCSI_DEBUG_MSG ("ncr810ThreadComplete: thread 0x%08x completed\n",
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
    
    ncr810ThreadStateSet (pThread, SCSI_THREAD_INACTIVE);

    scsiCacheSynchronize (pScsiThread, SCSI_CACHE_POST_COMMAND);

    scsiMgrThreadEvent (pScsiThread, SCSI_THREAD_EVENT_COMPLETED);
    }


/*******************************************************************************
*
* ncr810ThreadDefer - defer execution of a thread
*
* Set the thread's state to INACTIVE and notify the SCSI manager of the
* deferral event.
*
* RETURNS: N/A
*
* NOMANUAL
*/
LOCAL void ncr810ThreadDefer
    (
    NCR810_THREAD * pThread
    )
    {
    SCSI_THREAD * pScsiThread = (SCSI_THREAD *) pThread;
    
    SCSI_DEBUG_MSG ("ncr810ThreadDefer: thread 0x%08x deferred\n",
		    (int) pThread, 0, 0, 0, 0, 0);

    ncr810ThreadStateSet (pThread, SCSI_THREAD_INACTIVE);

    scsiMgrThreadEvent (pScsiThread, SCSI_THREAD_EVENT_DEFERRED);
    }
    
	
/*******************************************************************************
*
* ncr810ThreadFail - complete execution of a thread, with error status
*
* Set the thread's status and errno according to the type of error.  Set
* the thread's state to INACTIVE, and notify the SCSI manager of the
* completion event.
*
* RETURNS: N/A
*
* NOMANUAL
*/
LOCAL void ncr810ThreadFail
    (
    NCR810_THREAD * pThread,
    int             errNum
    )
    {
    SCSI_THREAD * pScsiThread = (SCSI_THREAD *) pThread;
    
    SCSI_DEBUG_MSG ("ncr810ThreadFail: thread 0x%08x failed (errno = %d)\n",
		    (int) pThread, errNum, 0, 0, 0, 0);

    pScsiThread->status = ERROR;

    if (pScsiThread->state == SCSI_THREAD_ABORTING)
	pScsiThread->errNum = S_scsiLib_ABORTED;

    else
    	pScsiThread->errNum = errNum;
    
    ncr810ThreadStateSet (pThread, SCSI_THREAD_INACTIVE);

    scsiMgrThreadEvent (pScsiThread, SCSI_THREAD_EVENT_COMPLETED);
    }
    
	
/******************************************************************************
*
* ncr810ThreadStateSet - set the state of a thread
*
* This is really just a place-holder for debugging and possible future
* enhancements such as state-change logging.
*
* RETURNS: N/A
*
* NOMANUAL
*/
LOCAL void ncr810ThreadStateSet
    (
    NCR810_THREAD *   pThread,		/* ptr to thread info */
    SCSI_THREAD_STATE state
    )
    {
    SCSI_THREAD * pScsiThread = (SCSI_THREAD *) pThread;
    
    SCSI_DEBUG_MSG ("ncr810ThreadStateSet: thread 0x%08x: %d -> %d\n",
		    (int) pThread, pScsiThread->state, state, 0, 0, 0);

    pScsiThread->state = state;
    }


/******************************************************************************
*
* ncr810Activate - activate a script corresponding to a new thread
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
LOCAL STATUS ncr810Activate
    (
    SIOP *          pSiop,
    NCR810_THREAD * pThread
    )
    {
    STATUS status;
    BOOL   sienMasked = TRUE; 
    int key;


    if (pSiop->cmdPending)
	{
	logMsg ("ncr810Activate: activation request already pending !\n",
		0, 0, 0, 0, 0, 0);
	return (ERROR);
	}
    
    /*
     *	Check controller state, set a new command pending and signal it
     *  if necessary.
     */

    key = intLock();
    
    switch (pSiop->state)
	{
	case NCR810_STATE_IDLE:
	    status = OK;
	    break;

	case NCR810_STATE_PASSIVE: 	/* do nothing */
	    pSiop->pNewThread = pThread;
	    pSiop->cmdPending = TRUE;

            /* 
	     * Signal the wait for (re) select script which then jumps to
	     * the relative alternate address. 
	     */

	    intUnlock (key);

            CACHE_PIPE_FLUSH();

	    NCR810_OUT_BYTE(pSiop->pIstat, NCR810_IN_BYTE(pSiop->pIstat) |
			    ISTAT_SIGP);
	    status = OK;
            sienMasked = FALSE;
	    break;
	    
	case NCR810_STATE_ACTIVE:
	default:
	    status = ERROR;
	    break;
	}

    if (sienMasked)
        {
        intUnlock (key);
        }

    if (status != OK)
	{
	logMsg ("ncr810Activate: invalid controller state.\n",
		pSiop->state, 0, 0, 0, 0, 0);

	errnoSet (S_scsiLib_SOFTWARE_ERROR);
	}

    return (status);
    }

/*******************************************************************************
*
* ncr810Resume - resume a script corresponding to a suspended thread
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

LOCAL STATUS ncr810Resume
    (
    SIOP *              pSiop,		/* ptr to controller info          */
    NCR810_THREAD *     pThread,	/* ptr to thread info              */
    NCR810_SCRIPT_ENTRY entryId		/* entry point of script to resume */
    )
    {
    STATUS status;
    BOOL   sienMasked = TRUE; 
    int key;

    /*
     *	Check validity of connection and start script if OK
     */

    key = intLock();

    switch (pSiop->state)
	{
	case NCR810_STATE_IDLE:
            SCSI_INT_DEBUG_MSG ("ncr810Resume: thread: 0x%08x:"
                                " state %d -> %d\n",
                                (int) pThread,
                                NCR810_STATE_IDLE, NCR810_STATE_ACTIVE,
                                0, 0, 0);

	    intUnlock (key);

	    ncr810ScriptStart (pSiop, pThread, entryId);
            
	    pSiop->state = NCR810_STATE_ACTIVE;
            sienMasked = FALSE;
	    status = OK;
	    break;

	case NCR810_STATE_PASSIVE:
	case NCR810_STATE_ACTIVE:
	default:
	    status = ERROR;
	    break;
	}

    if (sienMasked)
        {
        intUnlock (key);
        }


    return (status);
    }

/*******************************************************************************
*
* ncr810Abort - abort the active script corresponding to the current thread
*
* Check that there is currently an active script running.  If so, set the
* SIOP's Abort flag which will halt the script and cause an interrupt.
*
* RETURNS: N/A
*
* NOMANUAL
*/

LOCAL void ncr810Abort
    (
    SIOP * pSiop			/* ptr to controller info */
    )
    {
    STATUS status;
    int key;

    key = intLock();

    switch (pSiop->state)
	{
	case NCR810_STATE_IDLE:
	case NCR810_STATE_PASSIVE:
	    status = OK;
	    break;

	case NCR810_STATE_ACTIVE:

            CACHE_PIPE_FLUSH();

	    NCR810_OUT_BYTE(pSiop->pIstat, NCR810_IN_BYTE(pSiop->pIstat) |
			    ISTAT_ABRT);
	    status = OK;
	    break;
	    
	default:
	    status = ERROR;
	    break;
	}

    intUnlock (key);

    if (status != OK)
	{
	logMsg ("ncr810Abort: thread = 0x%08x: invalid state (%d)\n",
		pSiop->state, 0, 0, 0, 0, 0);
	}
    }

/*******************************************************************************
*
* ncr810ScriptStart - start the SIOP executing a script
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

LOCAL void ncr810ScriptStart
    (
    SIOP               *pSiop,		/* pointer to  SIOP info */
    NCR810_THREAD      *pThread,	/* ncr thread info */
    NCR810_SCRIPT_ENTRY entryId		/* routine address entry point */
    )

    {
    /*
     *	Script entry point addresses.  These are resolved by PROCs
     *	defined in the script itself (see "ncr810init.n").
     *
     *	NOTE: The number and order of members of this array _must_ match
     *	the enum NCR810_SCRIPT_ENTRY.
     */
    static ULONG * ncr810ScriptEntry [] =
	{
        ncr810Wait,			/* wait for re-select or host cmd   */
    	ncr810InitStart,		/* start an initiator thread        */
    	ncr810InitContinue,		/* continue an initiator thread     */
	ncr810TgtDisconnect, 	    	/* disconnect a target thread       */
	};    

    /*
     *	Set current hardware thread pointer for ISR
     */

    pSiop->pHwThread = pThread;

    /*
     *	Restore the SIOP register context for this thread.
     */
    NCR810_OUT_BYTE(pSiop->pScratcha0, pThread->scratcha0);
    NCR810_OUT_BYTE(pSiop->pScratcha1, pThread->scratcha1);
    NCR810_OUT_BYTE(pSiop->pScratcha2, pThread->scratcha2);
    NCR810_OUT_BYTE(pSiop->pScratcha3, pThread->scratcha3);

    NCR810_OUT_BYTE(pSiop->pScntl3, pThread->scntl3);
    NCR810_OUT_BYTE(pSiop->pSxfer,  pThread->sxfer);

    /*
     *	Set the shared data address, load the script start address,
     *	then start the SIOP unless it's in single-step mode.
     */
    NCR810_OUT_32(pSiop->pDsa, (UINT32)NCR810_VIRT_TO_PHYS (pThread->pShMem));

    NCR810_OUT_32(pSiop->pDsp, (UINT32)NCR810_VIRT_TO_PHYS(
					(UINT32)ncr810ScriptEntry[entryId]));


    if (pSiop->singleStep)
	{
	logMsg ("ncr810ScriptStart: single-step required to start script\n",
		0, 0, 0, 0, 0, 0);
	}
    else
	{
        CACHE_PIPE_FLUSH();
	NCR810_OUT_BYTE(pSiop->pDcntl, NCR810_IN_BYTE(pSiop->pDcntl) |
			DCNTL_STD); 

        /*
         * inOrderExecute() is a BSP specific routine that makes sure that
         * the SCSI registers written above are executed in order and completely
         * before continuing on. In the case of the faster PPC processors, it
         * has been found that without this flushing of the pipes the processor
         * is much faster than the SIOP, and causes erratic behaviour like bus
         * errors and invalid events.
         */

        CACHE_PIPE_FLUSH();
        ncr810Delay (); /* XXX - needs to be understood better for fast PPCs */
	CACHE_PIPE_FLUSH();
	}
    }


/*******************************************************************************
*
* ncr810ThreadParamsSet - set various parameters for a thread
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

LOCAL STATUS ncr810ThreadParamsSet
    (
    NCR810_THREAD * pThread,	    	/* thread to be affected  */
    UINT8           offset,		/* max REQ/ACK offset     */
    UINT8           period		/* min transfer period    */
    )
    {
    SCSI_THREAD *   pScsiThread = (SCSI_THREAD *) pThread;
    SIOP *          pSiop       = (SIOP *) pScsiThread->pScsiCtrl;
    SCSI_TARGET *   pScsiTarget = pScsiThread->pScsiTarget;
    UINT            busId       = pScsiTarget->scsiDevBusId;
    UINT8	    xferWidth   = pScsiTarget->xferWidth;
    NCR810_SHARED * pShMem      = pThread->pShMem;

    if (!ncr810XferParamsCvt (pSiop, &offset, &period, &pThread->sxfer,
			                               &pThread->scntl3))
	{				/* should never happen */
	errnoSet (S_scsiLib_ILLEGAL_PARAMETER);
	return (ERROR);
	}

    /* Do we want a wide (16 bit) transfer ? */

    if (xferWidth == SCSI_WIDE_XFER_SIZE_DEFAULT)
        {
        pThread->scntl3 |= SCNTL3_EWS;
        }

    pShMem->device = NCR810_SWAP_32(
		         (pThread->sxfer <<  NCR810_XFER_PARAMS_SHIFT) |
                         (busId << NCR810_TARGET_BUS_ID_SHIFT) |
		         (pThread->scntl3 << 24)
			 );

    return (OK);
    }


/*******************************************************************************
*
* ncr810XferParamsQuery - get (synchronous) transfer parameters
*
* Updates the synchronous transfer parameters suggested in the call to match
* the NCR 53C810's capabilities.  Transfer period is in SCSI units (multiples
* of 4 ns).
*
* Note: the transfer period is made longer and the offset is made smaller if
* the NCR 53C810 cannot handle the specified values.
*
* RETURNS: OK
*
* NOMANUAL
*/

LOCAL STATUS ncr810XferParamsQuery
    (
    SCSI_CTRL *pScsiCtrl,		/* ptr to controller info       */
    UINT8     *pOffset,			/* max REQ/ACK offset  [in/out] */
    UINT8     *pPeriod			/* min transfer period [in/out] */
    )
    {
    UINT8 unused;

    (void) ncr810XferParamsCvt ((SIOP *) pScsiCtrl, pOffset, pPeriod,
				&unused, &unused);
    
    return (OK);
    }


/*******************************************************************************
*
* ncr810WideXferParamsQuery - get wide data transfer parameters
*
* Updates the wide data transfer parameters suggested in the call to match
* the NCR 53C810's capabilities.  Transfer width is in the units 
* of the WIDE DATA TRANSFER message's transfer width exponent field. This is
* an 8 bit field where 0 represents a narrow transfer of 8 bits, 1 represents
* a wide transfer of 16 bits and 2 represents a wide transfer of 32 bits.
*
* Note: the transfer width is made smaller if the NCR 53C810 cannot handle 
* the specified value.
*
* RETURNS: OK
*
* NOMANUAL
*/

LOCAL STATUS ncr810WideXferParamsQuery
    (
    SCSI_CTRL *pScsiCtrl,		/* ptr to controller info       */
    UINT8     *xferWidth		/* suggested transfer width     */
    )
    {
    if (*xferWidth > NCR810_MAX_XFER_WIDTH)
	*xferWidth = NCR810_MAX_XFER_WIDTH;
    
    ncr810WideXferParamsSet (pScsiCtrl, *xferWidth);

    return (OK);
    }


/*******************************************************************************
*
* ncr810XferParamsSet - set transfer parameters
*
* Validate the requested parameters, convert to the NCR 53C810's native
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

LOCAL STATUS ncr810XferParamsSet
    (
    SCSI_CTRL *pScsiCtrl,		/* ptr to controller info */
    UINT8      offset,			/* max REQ/ACK offset     */
    UINT8      period			/* min transfer period    */
    )
    {
    NCR810_THREAD * pThread = (NCR810_THREAD *) pScsiCtrl->pThread;

    return (ncr810ThreadParamsSet (pThread, offset, period));
    }


/*******************************************************************************
*
* ncr810WideXferParamsSet - set wide transfer parameters
*
* Assume valid parameters and set the NCR810's thread parameters to the
* appropriate values. The actual registers are not written yet, but will
* be written from the thread values when it is activated.
*
* Transfer width is specified in SCSI transfer width exponent units. 
*
* RETURNS: OK 
*
* NOMANUAL
*/

LOCAL STATUS ncr810WideXferParamsSet
    (
    SCSI_CTRL *pScsiCtrl,		/* ptr to controller info */
    UINT8      xferWidth 		/* wide data transfer width */
    )
    {
    NCR810_THREAD * pThread = (NCR810_THREAD *) pScsiCtrl->pThread;

    if (NULL == pThread) 
        {
        logMsg("pThread is NULL POINTER",0,0,0,0,0,0); 
        return(ERROR); 
        }

    if (xferWidth == SCSI_WIDE_XFER_SIZE_NARROW)
	{
	pThread->scntl3 &= ~SCNTL3_EWS; /* Disable wide scsi data transfers */
	}
    else
	{
        pThread->scntl3 |= SCNTL3_EWS;	/* Enable wide scsi data transfers */
	}

    return (OK);
    }


/*******************************************************************************
*
* ncr810XferParamsCvt - convert transfer period from SCSI to NCR 53C810 units
*
* Given a "suggested" REQ/ACK offset and transfer period (in SCSI units of
* 4 ns), return the nearest offset and transfer period the NCR 53C810 is
* capable of using.  Also return the corresponding values of the NCR 53C810's
* Synchronous Transfer and Synchronous Bus Control registers.
*
* An offset of zero specifies asynchronous transfer, in which case the period
* is irrelevant.  Otherwise, the offset may be clipped to be within the
* maximum limit the NCR 53C810 is capable of.
*
* The transfer period is normally rounded towards longer periods if the NCR
* 53C810 is not capable of using the exact specified value.  The period may
* also be clipped to be within the NCR 53C810's maximum and minimum limits
* according to its clock period.
*
* If either the offset or period need to be clipped, the value FALSE is
* retuned as this may reflect an error condition.
*
* RETURNS: TRUE if input period is within NCR 53C810's range, else FALSE
*
* NOMANUAL
*/

LOCAL BOOL ncr810XferParamsCvt
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
    UINT  xferp;
    UINT  sccpDiv;
    UINT8 clkDiv;
    UINT8 xferParams;
    UINT8 clockDivide;
    UINT  sccp;				/* Scsi Core Clock Period */

    if (offset == SCSI_SYNC_XFER_ASYNC_OFFSET)
	{
	xferParams  = NCR810_SYNC_XFER_PARAMS_ASYNC;
	clockDivide = pSiop->clkDiv; 

	offset = SCSI_SYNC_XFER_ASYNC_OFFSET;
	period = 0;
    	}
    else
	{

	SCSI_DEBUG_MSG ("ncr810XferParamsCvt: requested: "
			"offset = %d, period = %d\n",
		    	offset, period, 0, 0, 0, 0);

        /* Default sccpDiv and clkDiv values for SCLK <= 50MHz */

	sccpDiv = 100;
	clkDiv = 1;

        /* Unless the SCLK is > 50MHz but < 66.7MHz */ 

        if (pSiop->clkPeriod < NCR810_50MHZ) /* then faster than 50MHZ */
            {
	    sccpDiv  = 67;
	    clkDiv = 2;
	    }

        /* convert all variables to ns */

	period *= 4; /* ns */
        sccp = pSiop->clkPeriod / sccpDiv; /* ns */

        xferp = period / sccp;

	/* adjust xferp to fit the allowable range */

	if (xferp < NCR810_MIN_XFERP)
	    xferp = NCR810_MIN_XFERP;

        else if (xferp > NCR810_MAX_XFERP)
	    xferp = NCR810_MAX_XFERP;

        /* update the period using the adjusted xferp */

	period = xferp * sccp;

        /* adjust the synchronous offset to fit the allowable range */

	if (offset < NCR810_MIN_REQ_ACK_OFFSET)
	    offset = NCR810_MIN_REQ_ACK_OFFSET;

        else if (offset > NCR810_MAX_REQ_ACK_OFFSET)
	    offset = NCR810_MAX_REQ_ACK_OFFSET;

        xferParams = (UINT8)(((xferp - NCR810_MIN_XFERP) & 0x07) << 
						NCR810_SYNC_XFER_PERIOD_SHIFT); 

        xferParams |= (UINT8)(offset & 0x0f);

        /* 
	 * The clock dividers for scntl3 remain fixed because this is a static
	 * value that does not change on a per transfer basis. The divider
	 * is based on the SCLK frequency from the board to the SCSI core 
	 */

	clockDivide = (pSiop->clkDiv & 0x0f); 
	clockDivide = (UINT8)(clockDivide | (clkDiv << 4));

        /* convert the period back to 4ns units */

	period /= 4;

        } /* else */

    SCSI_DEBUG_MSG ("ncr810XferParamsCvt: converted to: "
 		    "offset     = %d, period      = %d\n"
		    "xferParams = %d, clockDivide = %d\n",
		    offset, period, xferParams, clockDivide, 0, 0);

    *pOffset     = (UINT8)offset;
    *pPeriod     = (UINT8)period;
    *pXferParams = xferParams;
    *pClockDiv   = clockDivide;

    return (TRUE);
    }


/*******************************************************************************
*
* ncr810StepEnable - Enable/disable script single step 
*
* Enable/disable single step facility on ncr chip.  Also, unmask/mask single 
* step interrupt in Dien register.
* Before any SCSI execution you just have to use 
* ncr810StepEnable (pSiop, TRUE) to run in single step mode.  
* To run a script step use ncr810SingleStep(pSiop).
* To disable use ncr810StepEnable (pSiop, FALSE).
* 
* RETURNS: N/A
*
* NOMANUAL
*/

void ncr810StepEnable 
    (
    SIOP *pSiop,		/* pointer to SIOP info       */
    BOOL enable	    	    	/* TRUE => enable single-step */
    )

    {
    if  (enable)
	{
	/* enable single-step interrupt */

        CACHE_PIPE_FLUSH(); 
        NCR810_OUT_BYTE(pSiop->pDien, NCR810_IN_BYTE(pSiop->pDien) |
			B_SSI);

	/* set single-step mode */

        CACHE_PIPE_FLUSH(); 
	NCR810_OUT_BYTE(pSiop->pDcntl, NCR810_IN_BYTE(pSiop->pDcntl) |
			DCNTL_SSM);

	/* disable manual Start */

        CACHE_PIPE_FLUSH(); 
  	NCR810_OUT_BYTE(pSiop->pDmode, NCR810_IN_BYTE(pSiop->pDmode) &
			~DMODE_MAN); 
	}
    else 
	{
	/* unset single-step mode */

        CACHE_PIPE_FLUSH(); 
	NCR810_OUT_BYTE(pSiop->pDcntl, NCR810_IN_BYTE(pSiop->pDcntl) &
			~DCNTL_SSM);

	/* disable single-step interrupt */

        CACHE_PIPE_FLUSH(); 
        NCR810_OUT_BYTE(pSiop->pDien, NCR810_IN_BYTE(pSiop->pDien) & ~B_SSI);

	/* enable manual Start */

        CACHE_PIPE_FLUSH(); 
	NCR810_OUT_BYTE(pSiop->pDmode, NCR810_IN_BYTE(pSiop->pDmode) |
			DMODE_MAN);
        }

    pSiop->singleStep = enable;
    }


/*******************************************************************************
*
* ncr810SingleStep - Perform a single step 
*
* Perform a single step by writing the STD bit in the DCNTL register.
* The parameter is a pointer to the SIOP information. 
* 
* RETURNS: N/A
* 
* NOMANUAL
*/

void ncr810SingleStep
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
    CACHE_PIPE_FLUSH(); 
    NCR810_OUT_BYTE(pSiop->pDcntl, NCR810_IN_BYTE(pSiop->pDcntl) |
		    DCNTL_STD);

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
    if (pSiop->state == NCR810_STATE_IDLE)
	printf ("NCR810 is idle.\n\n");
    else
	{
        CACHE_PIPE_FLUSH();
	siopPc = (UINT *) NCR810_IN_32(pSiop->pDsp);

	 /* mask PCI's view of memory */ 

	siopPc = (UINT *) ((UINT) siopPc & 0x7fffffff);

	printf ("NCR810 [%d] Next Opcode (0x%08x) => 0x%08x 0x%08x\n\n",
		pSiop->state, (int) siopPc, NCR810_SWAP_32(siopPc[0]), 
					    NCR810_SWAP_32(siopPc[1]) );
	}
    
    /*
     *	Dump the SIOP registers, if in verbose mode
     */
    if (verbose)
    	ncr810Show ((SCSI_CTRL *) pSiop);
    }


/*******************************************************************************
*
* ncr810Show - display values of all readable NCR 53C8xx SIOP registers
*
* This routine displays the state of the SIOP registers in a user-friendly way.
* It is useful primarily for debugging. The input parameter is the pointer to 
* the SIOP information structure returned by the ncr810CtrlCreate() call.
*
* NOTE
* The only readable register during a script execution is the Istat register. 
* If you use this routine during the execution of a SCSI command, the result 
* could be unpredictable.
*
* EXAMPLE:
* .CS
* -> ncr810Show
* NCR810 Registers 
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
* SEE ALSO: ncr810CtrlCreate()
*/

STATUS ncr810Show 
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

    CACHE_PIPE_FLUSH();
 
    printf ("NCR810 Registers \n");
    printf ("---------------- \n");
    printf ("0x%08x: ", (int) pSiop->pScntl0);
    printf ("Scntl0   = 0x%02x ",  NCR810_IN_BYTE(pSiop->pScntl0)); 
    printf ("Scntl1   = 0x%02x ",  NCR810_IN_BYTE(pSiop->pScntl1)); 
    printf ("Scntl2   = 0x%02x ",  NCR810_IN_BYTE(pSiop->pScntl2)); 
    printf ("Scntl3   = 0x%02x\n", NCR810_IN_BYTE(pSiop->pScntl3)); 

    printf ("0x%08x: ", (int) pSiop->pScid);
    printf ("Scid     = 0x%02x ",  NCR810_IN_BYTE(pSiop->pScid)); 
    printf ("Sxfer    = 0x%02x ",  NCR810_IN_BYTE(pSiop->pSxfer)); 
    printf ("Sdid     = 0x%02x\n", NCR810_IN_BYTE(pSiop->pSdid)); 

    /* No need to print Gpreg (not used) */

    printf ("0x%08x: ", (int) pSiop->pSfbr);
    printf ("Sfbr    = 0x%02x ",  NCR810_IN_BYTE(pSiop->pSfbr)); 
    printf ("Socl    = 0x%02x ",  NCR810_IN_BYTE(pSiop->pSocl)); 
    printf ("Ssid    = 0x%02x ",  NCR810_IN_BYTE(pSiop->pSsid)); 
    printf ("Sbcl    = 0x%02x\n", NCR810_IN_BYTE(pSiop->pSbcl)); 

    printf ("0x%08x: ", (int) pSiop->pDstat);

    /* Reading this register clears the interrupts */
    printf ("Dstat   = 0x%02x ",  NCR810_IN_BYTE(pSiop->pDstat)); 
    printf ("Sstat0  = 0x%02x ",  NCR810_IN_BYTE(pSiop->pSstat0)); 
    printf ("Sstat1  = 0x%02x ",  NCR810_IN_BYTE(pSiop->pSstat1)); 
    printf ("Sstat2  = 0x%02x\n", NCR810_IN_BYTE(pSiop->pSstat2)); 

    printf ("0x%08x: ", (int) pSiop->pDsa);
    printf ("Dsa     = 0x%08x\n", NCR810_IN_32(pSiop->pDsa)); 

    printf ("0x%08x: ", (int) pSiop->pIstat);
    printf ("Istat   = 0x%02x\n", NCR810_IN_BYTE(pSiop->pIstat));

    printf ("0x%08x: ", (int) pSiop->pCtest0);
    printf ("Ctest0  = 0x%02x ",  NCR810_IN_BYTE(pSiop->pCtest0));
    printf ("Ctest1  = 0x%02x ",  NCR810_IN_BYTE(pSiop->pCtest1));
    printf ("Ctest2  = 0x%02x ",  NCR810_IN_BYTE(pSiop->pCtest2));
    printf ("Ctest3  = 0x%02x\n", NCR810_IN_BYTE(pSiop->pCtest3));
    
    printf ("0x%08x: ", (int) pSiop->pTemp);
    printf ("Temp    = 0x%08x\n", NCR810_IN_32(pSiop->pTemp)); 

    printf ("0x%08x: ", (int) pSiop->pDfifo);
    printf ("Dfifo   = 0x%02x\n", NCR810_IN_BYTE(pSiop->pDfifo)); 
    /* XXX Forget Ctest4-6 for now */
    printf ("Dbc0:23-Dcmd24:31   = 0x%08x\n", NCR810_IN_32(pSiop->pDbc)); 

    printf ("0x%08x: ", (int) pSiop->pDnad);
    printf ("Dnad    = 0x%08x\n", NCR810_IN_32(pSiop->pDnad)); 

    printf ("0x%08x: ", (int) pSiop->pDsp);
    printf ("Dsp     = 0x%08x\n", NCR810_IN_32(pSiop->pDsp)); 

    printf ("0x%08x: ", (int) pSiop->pDsps);
    printf ("Dsps    = 0x%08x\n", NCR810_IN_32(pSiop->pDsps)); 

    printf ("0x%08x: ", (int) pSiop->pScratcha0);
    printf ("Scratch0= 0x%02x ",  NCR810_IN_BYTE(pSiop->pScratcha0)); 
    printf ("Scratch1= 0x%02x ",  NCR810_IN_BYTE(pSiop->pScratcha1)); 
    printf ("Scratch2= 0x%02x ",  NCR810_IN_BYTE(pSiop->pScratcha2)); 
    printf ("Scratch3= 0x%02x\n", NCR810_IN_BYTE(pSiop->pScratcha3)); 

    printf ("0x%08x: ", (int) pSiop->pDmode);
    printf ("Dmode   = 0x%02x ",  NCR810_IN_BYTE(pSiop->pDmode));
    printf ("Dien    = 0x%02x ",  NCR810_IN_BYTE(pSiop->pDien));
    printf ("Dwt     = 0x%02x ",  NCR810_IN_BYTE(pSiop->pDwt));
    printf ("Dcntl   = 0x%02x\n", NCR810_IN_BYTE(pSiop->pDcntl));

    printf ("0x%08x: ", (int) pSiop->pSien0);
    printf ("Sien0    = 0x%02x ", NCR810_IN_BYTE(pSiop->pSien0)); 
    printf ("Sien1    = 0x%02x ", NCR810_IN_BYTE(pSiop->pSien1)); 

    /* Reading these registers clears the interrupts */

    printf ("Sist0    = 0x%02x ", NCR810_IN_BYTE(pSiop->pSist0)); 
    printf ("Sist1    = 0x%02x",  NCR810_IN_BYTE(pSiop->pSist1)); 

    printf ("\n");
    printf ("0x%08x: ", (int) pSiop->pSlpar);
    printf ("Slpar    = 0x%02x ", NCR810_IN_BYTE(pSiop->pSlpar)); 

    if ((pSiop->devType == NCR825_DEVICE_ID) ||
	(pSiop->devType == NCR875_DEVICE_ID))
	{
        printf ("Swide    = 0x%02x ", NCR810_IN_BYTE(pSiop->pSwide)); 
	}

    printf ("Macntl   = 0x%02x ",  NCR810_IN_BYTE(pSiop->pMacntl)); 
    printf ("Gpcntl   = 0x%02x\n", NCR810_IN_BYTE(pSiop->pGpcntl)); 

    printf ("0x%08x: ", (int) pSiop->pStime0);
    printf ("Stime0   = 0x%02x ", NCR810_IN_BYTE(pSiop->pStime0)); 
    printf ("Stime1   = 0x%02x ", NCR810_IN_BYTE(pSiop->pStime1)); 

    if (pSiop->devType == NCR825_DEVICE_ID)
	{
	#if FALSE /* XXX fix later */
        printf ("Respid0  = 0x%02x ", NCR810_IN_BYTE(pSiop->pRespid0)); 
        printf ("Respid1  = 0x%02x ", NCR810_IN_BYTE(pSiop->pRespid1)); 
	#endif
        printf ("Respid   = 0x%02x ", NCR810_IN_BYTE(pSiop->pRespid)); 
	}
    else 
	{
        printf ("Respid   = 0x%02x ", NCR810_IN_BYTE(pSiop->pRespid)); 
	}

    printf ("\n");

    printf ("0x%08x: ", (int) pSiop->pStest0);
    printf ("Stest0   = 0x%02x ",  NCR810_IN_BYTE(pSiop->pStest0)); 
    printf ("Stest1   = 0x%02x ",  NCR810_IN_BYTE(pSiop->pStest1)); 
    printf ("Stest2   = 0x%02x ",  NCR810_IN_BYTE(pSiop->pStest2)); 
    printf ("Stest3   = 0x%02x\n", NCR810_IN_BYTE(pSiop->pStest3)); 
    printf ("Sidl     = 0x%04x ",  NCR810_IN_16(pSiop->pSidl)); 
    printf ("Sodl     = 0x%04x ",  NCR810_IN_16(pSiop->pSodl)); 
    printf ("Sbdl     = 0x%04x\n", NCR810_IN_16(pSiop->pSbdl)); 
    printf ("Scratchb = 0x%08x\n", NCR810_IN_32(pSiop->pScratchb)); 

    return (OK);
    }


/*******************************************************************************
*
* ncr810Delay - do an arbitrary scsi delay
*
* XXX This is only a temporary routine.
*
* RETURNS: N/A
*/
LOCAL void ncr810Delay (void)
    {
    int delayCount;

    for (delayCount=0; delayCount < ncr810DelayCount; delayCount++)
	/* do nothing */ ;
    }

/*******************************************************************************
*
* ncr810ResetAtn - Resets the ATN signal
*
* This routine resets the ATN signal of the specifiedcontroller
*
* RETURNS : N/A.
*/

LOCAL void ncr810ResetAtn
    (
    SCSI_CTRL *pScsiCtrl	/* controller */
    )
    {
    SIOP *	pSiop;		/* ptr to SIOP info */
    UINT8	cSocl; 

    pSiop = (SIOP *) pScsiCtrl;

    cSocl = NCR810_IN_BYTE(pSiop->pSocl);
    cSocl &= ~B_ATN;
    NCR810_OUT_BYTE(pSiop->pSocl,cSocl);
    }
