/* sym895Lib.c - SCSI-2 driver for Symbios SYM895 SCSI Controller. */

/* Copyright 1989-2002 Wind River Systems, Inc. */

#include "copyright_wrs.h"

/*
modification history
--------------------
01f,31jan02,pmr  SPR 71337: properly setting transfer period in
                 sym895XferParamsCvt(), fixed warnings.
01e,30jul01,rcs  documented sym895DelayCount. SPR# 69249 
01d,18jun01,dtr  sysInWord,sysOutWord,sysInLong,sysOutLong are usually byte
                 swapped in the BSP.So the byteswapping is removed for
                 _BYTE_ORDER == _BIG_ENDIAN. SPR# 68497
01c,19jul00,rcs  corrected include path for sym895Lib.h
01b,23jun00,bsp  modified IO access macros, as per sysLib.h
01a,03mar00,bsp  written (from templateSCSI2.c, ver 01p).
*/

/*
DESCRIPTION
The SYM53C895 PCI-SCSI I/O Processor (SIOP) brings Ultra2 SCSI performance
to Host adapter, making it easy to add a high performance SCSI Bus to any PCI
System.  It supports Ultra-2 SCSI rates and allows increased SCSI connectivity
and cable length Low Voltage Differential (LVD) signaling for SCSI.  This 
driver runs in conjunction with SCRIPTS Assembly program for the Symbios SCSI
controllers.  These scripts use DMA transfers for all data, messages, and status
transfers.

For each controller device a manager task is created to manage SCSI threads
on each bus.  A SCSI thread represents each unit of SCSI work.

This driver supports multiple initiators, disconnect/reconnect, tagged
command queuing and synchronous data transfer protocol.  In general, the
SCSI system and this driver will automatically choose the best combination
of these features to suit the target devices used.  However, the default
choices may be over-ridden by using the function "scsiTargetOptionsSet()"
(see scsiLib).

Scatter/ Gather memory support: Scatter-Gather transfers are used when data 
scattered across memory must be transferred across the SCSI bus together 
with out CPU intervention.  This is achieved by a chain of block move script 
instructions together with the support from the driver. The driver is expected
to provide a set of addresses and byte counts for the SCRIPTS code.  However 
there is no support as such from vxworks SCSI Manager for this kind of data 
transfers.  So the implementation, as of today, is not completely integrated 
with vxworks, and assumes support from SCSI manager in the form of array of
pointers. The macro SCATTER_GATHER in sym895.h is thus not defined to avoid
compilation errors.

Loopback mode allows 895 chip to control all SCSI signals, regardless of 
whether it is in initiator or target role.  This mode insures proper SCRIPTS 
instructions fetches and data paths.  SYM895 executes initiator instructions 
through the SCRIPTS, and the target role is implemented in sym895Loopback by 
asserting and polling the appropriate SCSI signals in the SOCL, SODL, SBCL, 
and SBDL registers.

USER-CALLABLE ROUTINES
Most of the routines in this driver are accessible only through the I/O
system.  Three routines, however, must be called directly sym895CtrlCreate() 
to create a controller structure, and sym895CtrlInit() to initialize it.
If the default configuration is not correct, the routine sym895SetHwRegister() 
must be used to properly configure the registers.

Critical events, which are to be logged anyway irrespective of whether
debugging is being done or not, can be logged by using the SCSI_MSG macro.

PCI MEMORY ADDRESSING
The global variable sym895PciMemOffset was created to provide the BSP
with a means of changing the VIRT_TO_PHYS mapping without changing the
functions in the cacheFuncs structures.  In generating physical addresses
for DMA on the PCI bus, local addresses are passed through the function
CACHE_DMA_VIRT_TO_PHYS and then the value of sym895PciMemOffset is added.
For backward compatibility, the initial value of sym895PciMemOffset comes
from the macro PCI_TO_MEM_OFFSET.

INTERFACE
The BSP must connect the interrupt service routine for the controller device
to the appropriate interrupt system.  The routine to be called is
sym895Intr(), and the argument is the pointer to the controller device pSiop.
i.e.

.CS
    pSiop = sym895CtrlCreate (...);
    intConnect (XXXX, sym895Intr, pSiop);
    sym895CtrlInit (pSiop, ...);
.CE

HARDWARE ACCESS
All hardware access is to be done through macros.  The default definition
of the SYM895_REGx_READ() and SYM895_REGx_WRITE() macros (where x stands for 
the width of the register being accessed ) assumes an I/O mapped model.  
Register access mode can be set to either IO/memory using SYM895_IO_MAPPED
macro in sym895.h.  The macros can be redefined as necessary to accommodate 
other models, and situations where timing and write pipe considerations 
need to be addressed.  In IO mapped mode, BSP routines sysInByte(), sysOutByte()
are used for accessing SYM895 registers.  If these standard calls are not 
supported, the calls supported by respective BSP are to be mapped to these 
standard calls.  Memory mapped mode makes use of pointers to register offsets.

The macro SYM895_REGx_READ(pDev, reg) is used to read a
register of width x bits.  The two arguments are the device structure
pointer and the register offset.

The macro SYM895_REGx_WRITE(pDev, reg,data) is used to write data to the
specified register address.  These macros presume memory mapped I/O by
default.  Both macros can be redefined to tailor the driver to some other
I/O model.

The global variable sym895Delaycount provides the control count for 
the sym895 's delay loop. This variable is global in order to allow 
BSPs to adjust its value if necessary. The default value is 10 but it
may be set to a higher value as system clock speeds dictate.  

INCLUDE FILES
scsiLib.h, sym895.h, and sym895Script.c

SEE ALSO scsiLib, scsi2Lib, cacheLib, 
.I "SYM53C895 PCI-SCSI I/O Processor Data Manual Version 3.0,"
.I "Symbios Logic PCI-SCSI I/O Processors Programming Guide Version 2.1,"
*/

#define INCLUDE_SCSI2	/* This is a SCSI2 driver */
#include "vxWorks.h"
#include "sysLib.h"
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

#include "drv/scsi/sym895.h"

/* Include SCRIPTS Assembly code */

#include "sym895Script.c"

/* defines */

/*
 * short hand. All through the documentation, 
 * "SIOP" is used extensively for "SCSI Controller"
 */

typedef SYM895_SCSI_CTRL SIOP;	

#define PCI_TO_MEM_OFFSET	0x00

#if (_BYTE_ORDER == _BIG_ENDIAN)

#define SYM895_SWAP_32(x)  LONGSWAP((UINT)x)
#define SYM895_SWAP_16(x)  ((LSB(x) << 8)|MSB(x))

#else   /* _BYTE_ORDER == _BIG_ENDIAN   */

/* 
 * Byte order is Little endian 
 * Don't need to swap because the chip is little endian inherently.
 */

#define SYM895_SWAP_32   
#define SYM895_SWAP_16

#endif    /* _BYTE_ORDER == _BIG_ENDIAN   */

#define SYM895_SCSI_DEBUG_MSG SCSI_DEBUG_MSG

#define SYM895_VIRT_TO_PHYS(x) \
	((UINT)CACHE_DMA_VIRT_TO_PHYS(x) + (UINT)sym895PciMemOffset)

#define SYM895_PHYS_TO_VIRT(x) \
	((UINT)CACHE_DMA_PHYS_TO_VIRT(x) + (UINT)sym895PciMemOffset)

/* Hardware Abstraction macros hide all actual access to the chip */

#ifdef SYM895_IO_MAPPED

#if (_BYTE_ORDER==_LITTLE_ENDIAN)

#define SYM895_REG8_READ(pSiop, reg) \
	sysInByte((UINT32)((pSiop->baseAddress) + (reg)))

#define SYM895_REG8_WRITE(pSiop, reg, data) \
	sysOutByte((UINT32)((pSiop->baseAddress) + (reg)),(UINT8)data)

#define SYM895_REG16_READ(pSiop, reg) \
	SYM895_SWAP_16(sysInWord((UINT32)((pSiop->baseAddress) + (reg))))

#define SYM895_REG16_WRITE(pSiop, reg, data) \
	sysOutWord((UINT32)((pSiop->baseAddress) + (reg)), \
                   SYM895_SWAP_16((UINT16)data))

#define SYM895_REG32_READ(pSiop, reg) \
	SYM895_SWAP_32(sysInLong((UINT32)((pSiop->baseAddress) + (reg))))

#define SYM895_REG32_WRITE(pSiop, reg, data) \
	sysOutLong((UINT32)((pSiop->baseAddress) + (reg)), \
                   SYM895_SWAP_32((UINT32)data))

#else

#define SYM895_REG8_READ(pSiop, reg) \
	sysInByte((ULONG)((pSiop->baseAddress) + (reg)))

#define SYM895_REG8_WRITE(pSiop, reg, data) \
	sysOutByte((ULONG)((pSiop->baseAddress) + (reg)),(UINT8)data)

#define SYM895_REG16_READ(pSiop, reg) \
	sysInWord((ULONG)((pSiop->baseAddress) + (reg)))

#define SYM895_REG16_WRITE(pSiop, reg, data) \
	sysOutWord((ULONG)((pSiop->baseAddress) + (reg)), \
                   (UINT16)data)

#define SYM895_REG32_READ(pSiop, reg) \
	sysInLong((ULONG)((pSiop->baseAddress) + (reg)))

#define SYM895_REG32_WRITE(pSiop, reg, data) \
	sysOutLong((ULONG)((pSiop->baseAddress) + (reg)), \
                   (UINT32)data)

#endif 

#else	/* SYM895_IO_MAPPED */

#define SYM895_REG8_READ(pSiop, reg) \
	(*(UINT8 *)((pSiop->baseAddress) + (reg)))

#define SYM895_REG8_WRITE(pSiop, reg, data) \
	(*(UINT8 *)((pSiop->baseAddress) + (reg)) = (UINT8)data)

#define SYM895_REG16_READ(pSiop, reg) \
	SYM895_SWAP_16(*(UINT16 *)((pSiop->baseAddress) + (reg)))

#define SYM895_REG16_WRITE(pSiop, reg, data) \
	(*(UINT16 *)((pSiop->baseAddress) + (reg)) = SYM895_SWAP_16((UINT)data))

#define SYM895_REG32_READ(pSiop, reg) \
	SYM895_SWAP_32(*(UINT32 *)((pSiop->baseAddress) + (reg)))

#define SYM895_REG32_WRITE(pSiop, reg, data) \
	(*(UINT32 *)((pSiop->baseAddress) + (reg)) = SYM895_SWAP_32((UINT)data))

#endif   /* SYM895_IO_MAPPED */

/* Other miscellaneous defines */

#define SYM895_MAX_XFER_LENGTH 0xffffff	/* 16MB max transfer */

IMPORT BOOL scsiDebug;

/* Configurable options, scsi manager task */

int     sym895SingleStepSemOptions = SEM_Q_PRIORITY;
char *  sym895ScsiTaskName         = SCSI_DEF_TASK_NAME;
int     sym895ScsiTaskOptions      = SCSI_DEF_TASK_OPTIONS;
int     sym895ScsiTaskPriority     = SCSI_DEF_TASK_PRIORITY;
int     sym895ScsiTaskStackSize    = SCSI_DEF_TASK_STACK_SIZE;

/* Globals */

int     sym895PciMemOffset = PCI_TO_MEM_OFFSET;
int     sym895DelayCount   = 10;

ULONG * sym895ScriptEntry [] =  {
				sym895Wait,
				sym895InitStart,
				sym895InitContinue,
				sym895TgtDisconnect,
                                sym895Diag
				};

/* 
 * The variable "sym895Instructions" needs to be defined for PPC family. As  
 * the memory is Big-Endian and PCI bus being little endian,the sym895 
 * scripts need to be byte swapped in memory. However, they should only be 
 * swapped the first time that sysSym895PciInit() is invoked. This variable is
 * used in BSP file sysScsi.c to swap the scripts in memory.
 * 
 * vxEieio () is needed for PPC family to make sure that SIOP registers
 * are written in order.
 */

#if (CPU_FAMILY==PPC)
ULONG sym895Instructions = SYM895_INSTRUCTIONS;
#endif

#if ((CPU==PPC603) || (CPU == PPCEC603) || (CPU==PPC604))
# undef CACHE_PIPE_FLUSH
extern void  vxEieio ();
# define CACHE_PIPE_FLUSH() vxEieio()
#endif

/* forward declarations */

LOCAL STATUS sym895HwInit (SIOP * pSiop);
LOCAL void   sym895SharedMemInit ( SIOP * pSiop, SYM895_SHARED * pShMem);
LOCAL STATUS sym895IdentThreadInit (SYM895_THREAD * pThread);
STATUS       sym895SetHwOptions (SIOP * pSiop, SYM895_HW_OPTIONS * pHwOptions);
LOCAL void   sym895Delay (void);
LOCAL void   sym895ScsiBusReset (FAST SIOP * pSiop);
LOCAL UINT   sym895RemainderGet ( SIOP * pSiop, UINT phase);
LOCAL void   sym895Event (SIOP * pSiop, SYM895_EVENT * pEvent);
LOCAL STATUS sym895ThreadInit (SIOP * pSiop, SYM895_THREAD * pThread);
LOCAL STATUS sym895ThreadActivate (SIOP * pSiop, SYM895_THREAD * pThread);
LOCAL BOOL   sym895ThreadAbort (SIOP * pSiop, SYM895_THREAD * pThread);
LOCAL STATUS sym895ScsiBusControl (SIOP * pSiop, int operation);
LOCAL STATUS sym895XferParamsQuery (SCSI_CTRL * pScsiCtrl, UINT8 * pOffset,
                                    UINT8 * pPeriod);
LOCAL STATUS sym895XferParamsSet (SCSI_CTRL * pScsiCtrl, UINT8 offset,
                                  UINT8 period);
LOCAL BOOL   sym895XferParamsCvt (SIOP * pSiop, UINT8 * pOffset, UINT8 * pPeriod,  
                                  UINT8 * pXferParams, UINT8 * pClockDiv);
LOCAL void   sym895TargIdentEvent (SYM895_THREAD * pThread, 
                                   SYM895_EVENT * pEvent);
LOCAL STATUS sym895WideXferParamsQuery (SCSI_CTRL * pScsiCtrl,
                                        UINT8 * xferWidth);
LOCAL STATUS sym895WideXferParamsSet (SCSI_CTRL * pScsiCtrl, UINT8 xferWidth);
LOCAL void   sym895ScriptStart (SIOP * pSiop, SYM895_THREAD * pThread,
                                SYM895_SCRIPT_ENTRY entryId);
LOCAL int    sym895EventTypeGet (SIOP * pSiop);
LOCAL STATUS sym895ThreadParamsSet (SYM895_THREAD *, UINT8, UINT8);
LOCAL STATUS sym895Activate (SIOP * pSiop, SYM895_THREAD * pThread);
LOCAL void   sym895ThreadStateSet (SYM895_THREAD * pThread,
                                   SCSI_THREAD_STATE state);
LOCAL void   sym895Abort (SIOP * pSiop);
LOCAL void   sym895ThreadEvent (SYM895_THREAD * pThread, 
                                SYM895_EVENT *  pEvent);
LOCAL void   sym895InitEvent (SYM895_THREAD * pThread,
                              SYM895_EVENT *  pEvent);
LOCAL void   sym895ThreadUpdate (SYM895_THREAD * pThread);
LOCAL void   sym895InitIdentEvent (SYM895_THREAD * pThread,
                                   SYM895_EVENT * pEvent);
LOCAL void   sym895IdentInContinue (SYM895_THREAD * pThread);
LOCAL STATUS sym895Resume (SIOP * pSiop, SYM895_THREAD * pThread,
                           SYM895_SCRIPT_ENTRY entryId);
LOCAL void   sym895ThreadFail (SYM895_THREAD * pThread, int errNum);
LOCAL void   sym895ThreadComplete (SYM895_THREAD * pThread);
LOCAL void   sym895ThreadDefer (SYM895_THREAD * pThread);
LOCAL STATUS sym895PhaseMismatch (SYM895_THREAD * pThread, int phase,
				  UINT remCount);
LOCAL void   sym895ThreadReconnect (SYM895_THREAD * pThread);

/*******************************************************************************
*
* sym895CtrlCreate - create a structure for a SYM895 device.
*
* This routine creates a SCSI Controller data structure and must be called 
* before using a SCSI Controller chip.  It should be called once and only 
* once for a specified SCSI Controller. Since it allocates memory for a 
* structure needed by all routines in sym895Lib, it must be called
* before any other routines in the library. After calling this routine,
* sym895CtrlInit() should be called at least once before any SCSI
* transactions are initiated using the SCSI Controller.
*
* A Detailed description of parameters follows.
*
* .IP <siopBaseAdrs>
* base address of the SCSI controller.
* .IP <clkPeriod>
* clock controller period (nsec*100).This is used to determine the clock period
* for the SCSI core and affects the timing of both asynchronous and synchronous
* transfers. Several Commonly used values are
*
* .CS
* SYM895_1667MHZ  6000    16.67Mhz chip 
* SYM895_20MHZ    5000    20Mhz chip 
* SYM895_25MHZ    4000    25Mhz chip 
* SYM895_3750MHZ  2667    37.50Mhz chip 
* SYM895_40MHZ    2500    40Mhz chip 
* SYM895_50MHZ    2000    50Mhz chip 
* SYM895_66MHZ    1515    66Mhz chip 
* SYM895_6666MHZ  1500    66Mhz chip 
* SYM895_75MHZ    1333    75Mhz chip 
* SYM895_80MHZ    1250    80Mhz chip 
* SYM895_160MHZ    625    40Mhz chip with Quadrupler 
* .CE
*
* .IP <devType>
* SCSI sym8xx device type 
* .IP <siopRamBaseAdrs>
* base address of the internal scripts RAM
* .IP <flags>
* various device/debug options for the driver. Commonly used values are
*
* .CS
* SYM895_ENABLE_PARITY_CHECK      0x01
* SYM895_ENABLE_SINGLE_STEP       0x02
* SYM895_COPY_SCRIPTS             0x04
* .CE
*
* RETURNS: A pointer to SYM895_SCSI_CTRL structure, or NULL if memory 
* is unavailable or there are invalid parameters.
*
* ERRORS: N/A
*
*/

SYM895_SCSI_CTRL * sym895CtrlCreate
    (
    UINT8 *  siopBaseAdrs,        /* base address of the SCSI Controller */
    UINT     clkPeriod,           /* clock controller period (nsec*100)  */
    UINT16   devType,             /* SCSI device type */
    UINT8 *  siopRamBaseAdrs,     /* on Chip Ram Address */
    UINT16   flags                /* options */
    )
    {
    FAST SIOP *  pSiop;           /* pointer to SCSI Controller info */
    SCSI_CTRL *  pScsiCtrl;
    UINT nBytes;                  /* Required memory */
    
    static SYM895_HW_OPTIONS sym895Options = SYM895_FAST_SCSI_OPTIONS;

     /* Verify parameters */

    if ((siopBaseAdrs == NULL) || (siopRamBaseAdrs == NULL))
        return ((SIOP *)NULL);

    if ((clkPeriod > SYM895_1667MHZ) || (clkPeriod < SYM895_160MHZ))
        return ((SIOP *)NULL);

    /* Check that the DM Buffers are cache coherent */

    if (!CACHE_DMA_IS_WRITE_COHERENT () || !CACHE_DMA_IS_READ_COHERENT ())
        {
        SCSI_MSG ("sym895CtrlCreate : Shared memory not cache coherent. \n",
                  0, 0, 0, 0, 0, 0);
        return ((SIOP *)NULL);
        }

    switch (devType)
        {
        case NCR810_DEVICE_ID   :
        case NCR825_DEVICE_ID   :
        case NCR875_DEVICE_ID   :
        case SYM895_DEVICE_ID	:
        case NCR895A_DEVICE_ID  :	
             break;

        default	                :
             SCSI_MSG (" sym895CtrlCreate : Invalid Device type %x\n",
                       devType, 0, 0, 0, 0, 0);
             return ((SIOP *)NULL);
        }
	
    /* cacheDmaMalloc the controller struct and the two shared data areas */

    nBytes = sizeof (SIOP) + 2 * sizeof (SYM895_SHARED);

    if ((pSiop = (SIOP *) cacheDmaMalloc (nBytes)) == (SIOP *) NULL)
        return ((SIOP *) NULL);

    memset ((char *) pSiop, 0, nBytes);

    pSiop->pIdentShMem = (SYM895_SHARED *) (pSiop + 1);
    pSiop->pClientShMem = pSiop->pIdentShMem + 1;

    pScsiCtrl = &(pSiop->scsiCtrl);

    /* fill in generic SCSI info for this controller */

    pScsiCtrl->eventSize 	   = sizeof (SYM895_EVENT);
    pScsiCtrl->threadSize 	   = sizeof (SYM895_THREAD);
    pScsiCtrl->maxBytesPerXfer     = SYM895_MAX_XFER_LENGTH;

    pScsiCtrl->scsiTransact 	   = (FUNCPTR)	   scsiTransact;
    pScsiCtrl->scsiEventProc       = (VOIDFUNCPTR) sym895Event;
    pScsiCtrl->scsiThreadInit      = (FUNCPTR)     sym895ThreadInit;
    pScsiCtrl->scsiThreadActivate  = (FUNCPTR)     sym895ThreadActivate;
    pScsiCtrl->scsiThreadAbort     = (FUNCPTR)     sym895ThreadAbort;
    pScsiCtrl->scsiBusControl      = (FUNCPTR)     sym895ScsiBusControl;
    pScsiCtrl->scsiXferParamsQuery = (FUNCPTR)     sym895XferParamsQuery;
    pScsiCtrl->scsiXferParamsSet   = (FUNCPTR)     sym895XferParamsSet;
    pScsiCtrl->scsiWideXferParamsQuery = (FUNCPTR) sym895WideXferParamsQuery;
    pScsiCtrl->scsiWideXferParamsSet   = (FUNCPTR) sym895WideXferParamsSet;

    pScsiCtrl->wideXfer 	   = TRUE;	/* for 895 and above */

    scsiCtrlInit (&(pSiop->scsiCtrl));
	
    pSiop->baseAddress        = siopBaseAdrs;
    pSiop->devType            = devType;
    pSiop->clkPeriod          = clkPeriod;
    pSiop->siopRamBaseAddress = siopRamBaseAdrs;

    memcpy (&(pSiop->hwOptions), &(sym895Options), sizeof (SYM895_HW_OPTIONS));

    /* Create synchronisation semaphore for single-step support  */
	
    if (flags & SYM895_ENABLE_SINGLE_STEP)
        {
        pSiop->singleStepSem = semBCreate (sym895SingleStepSemOptions, SEM_EMPTY);
        if(pSiop->singleStepSem == NULL)
            {
            SCSI_MSG ("sym895CtrlCreate: Error Creating Single step semaphore\n", 
                     0, 0, 0, 0, 0, 0);

            (void) cacheDmaFree ((char *)pSiop);
             return (NULL);
            }
        }
    else
        pSiop->singleStepSem = NULL;

    pSiop->state        = SYM895_STATE_IDLE;
    pSiop->isCmdPending = FALSE;

    /* Set the user chosen options */

    pSiop->isParityCheck  = (flags & SYM895_ENABLE_PARITY_CHECK)? TRUE:FALSE;
    pSiop->isSingleStep   = (flags & SYM895_ENABLE_SINGLE_STEP)? TRUE:FALSE; 
    pSiop->isScriptsCopy  = (flags & SYM895_COPY_SCRIPTS)? TRUE:FALSE;

    /* Initialize fields in client shared data area */

    sym895SharedMemInit (pSiop,pSiop->pClientShMem);

    /* Initialize the Identification Thread */

    sym895IdentThreadInit ((SYM895_THREAD *)pScsiCtrl->pIdentThread);

    /* spawn SCSI manager - use generic code from "scsiLib.c" */

    pScsiCtrl->scsiMgrId = taskSpawn (sym895ScsiTaskName,
                                      sym895ScsiTaskPriority,
                                      sym895ScsiTaskOptions,
                                      sym895ScsiTaskStackSize*2,
                                      (FUNCPTR) scsiMgr,
                                      (int) pSiop, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    
    if (pScsiCtrl->scsiMgrId != ERROR)
        {
        return (pSiop);
        }

    SCSI_MSG ("sym895CtrlCreate: can't spawn SCSI Manager \n",
              0, 0, 0, 0, 0, 0);

    if (pSiop->singleStepSem != NULL)
        (void) semDelete (pSiop->singleStepSem);

    (void) cacheDmaFree ((char *)pSiop);

    return (NULL);
    }

/*******************************************************************************
*
* sym895CtrlInit - initialize a SCSI Controller Structure.
*
* This routine initializes an  SCSI Controller structure, after the structure
* is created with sym895CtrlCreate().  This structure must be initialized
* before the SCSI Controller can be used.  It may be called more than once
* if needed; however, it should only be called while there is no activity on
* the SCSI interface.
*
* A Detailed description of parameters follows.
*
* .IP <pSiop>
* pointer to the SCSI controller structure created with sym895CtrlCreate()
* .IP <scsiCtrlBusId>
* SCSI Bus Id of the SIOP. 
*
* RETURNS OK, or ERROR if parameters are out of range.
*
* ERRORS: N/A
*/

STATUS sym895CtrlInit
    (
    FAST SIOP * pSiop, 	/* pointer to SCSI Controller structure  */
    UINT scsiCtrlBusId	/* SCSI bus ID of this SCSI Controller   */
    )
    {
    volatile int delay;
    SCSI_CTRL *  pScsiCtrl = &pSiop->scsiCtrl;

    /* validate parameters */

    if (pSiop == (SIOP *)NULL)	
        return (ERROR);

    if ((scsiCtrlBusId < SCSI_MIN_BUS_ID) || (scsiCtrlBusId > SCSI_MAX_BUS_ID))
        return (ERROR);

    pScsiCtrl->scsiCtrlBusId = scsiCtrlBusId;
	
    /* Initialise and reset the SCSI Controller */
	
    if (sym895HwInit (pSiop) == ERROR)
        { 
        SCSI_MSG ("Error in sym895HwInit()\n", 0, 0, 0, 0, 0, 0);
        return (ERROR);
        }

    delay = sym895DelayCount;
    sym895DelayCount *= 35;	/* First delay must be longer */    
    sym895ScriptStart (pSiop, (SYM895_THREAD *)pScsiCtrl->pIdentThread,
                       SYM895_SCRIPT_WAIT); 
    sym895DelayCount = delay;

    pSiop->state = SYM895_STATE_PASSIVE;

    return (OK);
    }


/*******************************************************************************
*
* sym895HwInit - hardware initialization for the 895 Chip.
*
* This function puts the SIOP in a known quiescent state.  Also, if copying of 
* SCSI scripts is enabled, this routine copies entire SCRIPTS code from host 
* memory to On-Chip SCRIPTS RAM.  This routine does not modify any of the 
* registers that are set by sym895SetHwOptions().  
*
* For details of the register bits initialised here, refer to SYM53C895 data 
* manual Version 3.0.
*
* .IP <pSiop>
* pointer to the SIOP structure
*
* RETURNS: OK, or ERROR if parameters are out of range.
*
* ERRORS: N/A
*/

STATUS sym895HwInit
    (
    FAST SIOP * pSiop         /* pointer to the SIOP structure */
    )
    {
	
    /* local variables for the copy of SCRIPTs */

    ULONG *     dst;          /* pointer for SCRIPTS destination */
    ULONG       nLongs;       /* number of longs to copy across SCRIPTS RAM */
    int         status;       /* status of copying SCRIPTS */
    char  *     src;          /* pointer for SCRIPTS source */
    UINT8       curScid;      /* store for current chip's SCSI ID */

    /* validate the parameters */

    if (pSiop == (SIOP *)NULL)	
        return (ERROR);

    /* Soft-Reset the SIOP */

    SYM895_REG8_WRITE (pSiop,SYM895_OFF_ISTAT, SYM895_ISTAT_SOFTRST);
    taskDelay (2);
    SYM895_REG8_WRITE (pSiop,SYM895_OFF_ISTAT,0);

    /* Initialize the Driver default Options for the Chip */

    if (sym895SetHwOptions (pSiop,&pSiop->hwOptions) != OK)
        {
        SCSI_MSG ("sym895SetHwOptions failed\n", 0, 0, 0, 0, 0, 0);
        return (ERROR);
        }
   
    /* DMA Manual Mode. Automatic Fetching of scripts disabled */
	
    SYM895_REG8_WRITE (pSiop,SYM895_OFF_DMODE,
                       SYM895_REG8_READ (pSiop,SYM895_OFF_DMODE) |
                       SYM895_DMODE_MAN);	

    /* SYM (NCR)700 compatibility mode turned off */

    SYM895_REG8_WRITE (pSiop,SYM895_OFF_DCNTL,
                       SYM895_REG8_READ (pSiop,SYM895_OFF_DCNTL)| 
                       SYM895_DCNTL_COM);	

    /* Check clock period and get the clock division factor */

    if (pSiop->clkPeriod >= SYM895_25MHZ)            /* 16.67 - 25 MHz */
        {
        pSiop->clkDiv = SYM895_16MHZ_ASYNC_DIV;
        }
    else if (pSiop->clkPeriod >= SYM895_3750MHZ)     /* 25.01 MHz  - 37.5MHz */
        {
        pSiop->clkDiv = SYM895_25MHZ_ASYNC_DIV;
        }
    else if (pSiop->clkPeriod >= SYM895_50MHZ)       /* 37.51 MHz  - 50 MHz */
        {
        pSiop->clkDiv = SYM895_3750MHZ_ASYNC_DIV;
        }	
    else if (pSiop->clkPeriod >= SYM895_75MHZ)       /* 50.01MHz  - 75 MHz */
        {
        pSiop->clkDiv = SYM895_50MHZ_ASYNC_DIV;
        }
    else if (pSiop->clkPeriod >= SYM895_80MHZ)       /* 75.01MHz  - 80 MHz */
        {
        pSiop->clkDiv = SYM895_75MHZ_ASYNC_DIV;
        }
    else                               /* 75.01 Mhz - 160 Mhz (Quadrapler) */
        {
        pSiop->clkDiv = SYM895_160MHZ_ASYNC_DIV;
        }	

    SYM895_REG8_WRITE (pSiop,SYM895_OFF_SCNTL3,
                       SYM895_REG8_READ (pSiop,SYM895_OFF_SCNTL3) |
                       pSiop->clkDiv);	
	
    /* 
     * If disconect/reconnect is enabled, use full arbitration.
     * Always generate parity, check for parity if enabled.
     */

    if ( pSiop->scsiCtrl.disconnect)
        {
        SYM895_REG8_WRITE (pSiop,SYM895_OFF_SCNTL0,
                           SYM895_REG8_READ (pSiop,SYM895_OFF_SCNTL0)|
                           (SYM895_SCNTL0_ARB1 | SYM895_SCNTL0_ARB0));			
        }
		
    /* Enable Select with ATN signal asserted */

    SYM895_REG8_WRITE (pSiop,SYM895_OFF_SCNTL0,
                       SYM895_REG8_READ (pSiop,SYM895_OFF_SCNTL0)|
                       (SYM895_SCNTL0_WATN));			

    /* Enable parity Checking if enabled*/

    if ( pSiop->isParityCheck)	/* if parity Check enabled */
        {
        SYM895_REG8_WRITE (pSiop,SYM895_OFF_SCNTL0,
                           SYM895_REG8_READ (pSiop,SYM895_OFF_SCNTL0) |
                           (SYM895_B_PAR));			
        }

    /* Set Bus Id */
	
    curScid = SYM895_REG8_READ (pSiop,SYM895_OFF_SCID) & 0xf0;
	
    SYM895_REG8_WRITE (pSiop, SYM895_OFF_SCID, 
                       curScid | pSiop->scsiCtrl.scsiCtrlBusId);

    SYM895_REG16_WRITE (pSiop, SYM895_OFF_RESPID0,
                        1<<pSiop->scsiCtrl.scsiCtrlBusId);

    /* Enable Response to Bus Initiated Selection/Reselection */

    SYM895_REG8_WRITE (pSiop, SYM895_OFF_SCID, 	
                       SYM895_REG8_READ (pSiop,SYM895_OFF_SCID)|
                       (SYM895_SCID_RRE | SYM895_SCID_SRE));

    /* Enable Interrupts - SCSI and DMA */

    /* 
     * Note : SCSI ATN Interrupt is used (in target mode) to signal the
     * assetion of the ATN signal. In initiator mode, it is the phase mismatch
     * interrupt. In target mode, this interrupt is not handled. This is
     * enabled only in Initiator mode.
     */

    SYM895_REG8_WRITE (pSiop, SYM895_OFF_SIEN0, 
                       (SYM895_B_SGE | SYM895_B_UDC | 
                       SYM895_B_RST | SYM895_B_PAR));
    SYM895_REG8_WRITE (pSiop, SYM895_OFF_SIEN1, 
                       (SYM895_B_STO | SYM895_B_HTH |
                       SYM895_B_SBMC | SYM895_B_GEN));
    SYM895_REG8_WRITE (pSiop, SYM895_OFF_DIEN, 
                       (SYM895_B_BF | SYM895_B_ABT | 
                       SYM895_B_SIR | SYM895_B_IID));

    /* Clear DMA FIFO */
		
    SYM895_REG8_WRITE (pSiop, SYM895_OFF_CTEST3,	
                       SYM895_REG8_READ (pSiop,SYM895_OFF_CTEST3) | 
                       SYM895_CTEST3_CLF);
 
    /* 
     * Copy scripts to the on chip script RAM. 
     * Each 895 instruction is 2 longs wide 
     */

    if ((pSiop->isScriptsCopy == TRUE) && (pSiop->siopRamBaseAddress != NULL))
        {
        nLongs = SYM895_INSTRUCTIONS * 2;

        /* Check whether scripts exceed the size of SCRIPT RAM (4K for 895) */

        if (SYM895_SCRIPT_RAM_SIZE < nLongs * sizeof (ULONG))
            {
            SCSI_MSG (" sym895HwInit: Total Scripts length exceeded \n",
                      0, 0, 0, 0, 0, 0);
            return (ERROR);
	    }
	
        dst = (ULONG *)pSiop->siopRamBaseAddress;
        src = (char *)sym895Wait;

        /* Now Copy the scripts and validate the copying after checking */

        bcopyLongs ((char *)sym895Wait, (char *)pSiop->siopRamBaseAddress, 
                    nLongs);

        status = bcmp ((char *) sym895Wait, (char *)pSiop->siopRamBaseAddress, 
                      nLongs*4);

        if (status != 0)
            {
            SCSI_MSG (" Copy to Scripts Ram Failed \n", 
                      0, 0, 0, 0, 0, 0);
            return (ERROR);
            }

        /* Fill in the script Entries */

        sym895ScriptEntry [SYM895_SCRIPT_WAIT] = (ULONG *) ((UINT) dst);
        sym895ScriptEntry [SYM895_SCRIPT_INIT_START] = 
            (ULONG *) ((UINT)sym895InitStart - (UINT)sym895Wait + (UINT)dst);
        sym895ScriptEntry [SYM895_SCRIPT_INIT_CONTINUE] = 
            (ULONG *) ((UINT)sym895InitContinue - (UINT)sym895Wait + (UINT)dst);
        sym895ScriptEntry [SYM895_SCRIPT_TGT_DISCONNECT] = 
            (ULONG *) ((UINT)sym895TgtDisconnect - (UINT)sym895Wait + (UINT)dst);
        sym895ScriptEntry [SYM895_SCRIPT_DIAG] = 
            (ULONG *) ((UINT)sym895Diag - (UINT)sym895Wait + (UINT)dst);
        }
    else
        {

        sym895ScriptEntry [SYM895_SCRIPT_WAIT] = (ULONG *) (sym895Wait);
        sym895ScriptEntry [SYM895_SCRIPT_INIT_START] = 
            (ULONG *)((UINT)sym895InitStart);
        sym895ScriptEntry [SYM895_SCRIPT_INIT_CONTINUE] = 
            (ULONG *) ((UINT) sym895InitContinue);
        sym895ScriptEntry [SYM895_SCRIPT_TGT_DISCONNECT] = 
            (ULONG *) ((UINT) sym895TgtDisconnect);
        sym895ScriptEntry [SYM895_SCRIPT_DIAG] = 
            (ULONG *) ((UINT) sym895Diag);
        }

    /* HwInit Successful */

    return (OK);

    }


/*******************************************************************************
*
* sym895SetHwOptions - sets the Sym895 chip Options.
*			
* This function sets optional bits required for tweaking the performance
* of 895 to the Ultra2 SCSI. The routine should be called with SYM895_HW_OPTIONS
* structure as defined in sym895.h file. 
*
* The input parameters are
*
* .IP <pSiop> 
* pointer to the SIOP structure
* .IP <pHwOptions>
* pointer to the a SYM895_HW_OPTIONS structure.
* 
* .CS
*    struct sym895HWOptions
*    {
*    int    SCLK    : 1;      /@ STEST1:b7,if false, uses PCI Clock for SCSI  @/
*    int    SCE     : 1;      /@ STEST2:b7, enable assertion of SCSI thro SOCL@/
*                             /@ and SODL registers                           @/
*    int    DIF     : 1;      /@ STEST2:b5, enable differential SCSI          @/
*    int    AWS     : 1;      /@ STEST2:b2, Always Wide SCSI                  @/
*    int    EWS     : 1;      /@ SCNTL3:b3, Enable Wide SCSI                  @/
*    int    EXT     : 1;      /@ STEST2:b1, Extend SREQ/SACK filtering        @/
*    int    TE      : 1;      /@ STEST3:b7, TolerANT Enable                   @/
*    int    BL      : 3;      /@ DMODE:b7,b6, CTEST5:b2 : Burst length        @/ 
*                             /@ when set to any of 32/64/128 burst length    @/
*                             /@ transfers, requires the DMA Fifo size to be  @/
*                             /@ 816 bytes (ctest5:b5 = 1).                   @/  
*    int    SIOM    : 1;      /@ DMODE:b5, Source I/O Memory Enable           @/
*    int    DIOM    : 1;      /@ DMODE:b4, Destination I/O Memory Enable      @/
*    int    EXC     : 1;      /@ SCNTL1:b7, Slow Cable Mode                   @/
*    int    ULTRA   : 1;      /@ SCNTL3:b7, Ultra Enable                      @/
*    int    DFS     : 1;      /@ CTEST5:b5, DMA Fifo size 112/816 bytes       @/
*    } SYM895_HW_OPTIONS;
* .CE
*
* This routine should not be called when there is SCSI Bus Activity as 
* this modifies the SIOP Registers.
*
* RETURNS: OK or ERROR if any of the input parameters is not valid.
*
* ERRORS: N/A
*
* SEE ALSO: sym895.h, sym895CtrlCreate()
*
*/

STATUS sym895SetHwOptions
    (
    FAST SIOP *          pSiop,	        /* pointer to the SIOP structure    */
    SYM895_HW_OPTIONS *	 pHwOptions     /* pointer to the Options Structure */
    )
    {

    /* temporary store to hold current register values */
   
    UINT8 dmode;
    UINT8 stest2;
    UINT8 scntl3;
    UINT8 ctest5;
    UINT8 stest1;
    UINT8 stest3;
    UINT8 scntl1;

    /* validate input parameters */

    if ((pSiop == (SIOP *)NULL) || (pHwOptions == (SYM895_HW_OPTIONS *)NULL))
        return (ERROR);

    /* Fill in the SIOP structure with new HwOptions */

    memcpy ((char *)&(pSiop->hwOptions), (char *)pHwOptions,
            sizeof(SYM895_HW_OPTIONS));
	
    /* store the current values */
	
    dmode  = SYM895_REG8_READ (pSiop, SYM895_OFF_DMODE);
    stest1 = SYM895_REG8_READ (pSiop, SYM895_OFF_STEST1);
    stest2 = SYM895_REG8_READ (pSiop, SYM895_OFF_STEST2);
    stest3 = SYM895_REG8_READ (pSiop, SYM895_OFF_STEST3);
    scntl1 = SYM895_REG8_READ (pSiop, SYM895_OFF_SCNTL1);
    scntl3 = SYM895_REG8_READ (pSiop, SYM895_OFF_SCNTL3);
    ctest5 = SYM895_REG8_READ (pSiop, SYM895_OFF_CTEST5);

    /* Manipulate according to the options set */

    (pHwOptions->DIOM)?
    (dmode |= SYM895_DMODE_DIOM):(dmode &= ~SYM895_DMODE_DIOM);
    (pHwOptions->SIOM)?
    (dmode |= SYM895_DMODE_SIOM):(dmode &= ~SYM895_DMODE_SIOM);
    (pHwOptions->SCLK)?
    (stest1 |= SYM895_STEST1_SCLK):(stest1 &= ~SYM895_STEST1_SCLK);
    (pHwOptions->DIF)?
    (stest2 |= SYM895_STEST2_DIF):(stest2 &= ~SYM895_STEST2_DIF);		
    (pHwOptions->SCE)?
    (stest2 |= SYM895_STEST2_SCE):(stest2 &= ~SYM895_STEST2_SCE);		
    (pHwOptions->AWS)?
    (stest2 |= SYM895_STEST2_AWS):(stest2 &= ~SYM895_STEST2_AWS);				
    (pHwOptions->EXTEND)?
    (stest2 |= SYM895_STEST2_EXT):(stest2 &= ~SYM895_STEST2_EXT);		
    (pHwOptions->TE)?
    (stest3 |= SYM895_STEST3_TE):(stest3 &= ~SYM895_STEST3_TE);
    (pHwOptions->EXC)?
    (scntl1 |= SYM895_SCNTL1_EXC):(scntl1 &= ~SYM895_SCNTL1_EXC);
    (pHwOptions->ULTRA)?
    (scntl3 |= SYM895_SCNTL3_ULTRA):(scntl3 &= ~SYM895_SCNTL3_ULTRA);
    (pHwOptions->EWS)?
    (scntl3 |= SYM895_SCNTL3_EWS):(scntl3 &= ~SYM895_SCNTL3_EWS);
    (pHwOptions->DFS)?
    (ctest5 |= SYM895_CTEST5_DFS):(ctest5 &= ~SYM895_CTEST5_DFS);	

    /*
     * The Burst lengths 32,64 and 128 transfers are valid only if FIFO 
     * size is set to 816 bytes. Avoid setting CTEST5:BL2 when DMA FIFO 
     * is set to 112 byes (CTEST5:DFS == 0). 
     */

    if (!(pHwOptions->DFS))
        {
        if ((pHwOptions->BL) & 0x04)
            {
            SCSI_MSG("sym895SetHwOptions: Erroneous Burstlength size for \
                     112 byte FIFO \n", 0, 0, 0, 0, 0, 0);
            }

        /* Forcing the burst length to a lesser value */

        ctest5 &= ~SYM895_CTEST5_BL2;
        }
    else
        {
        ((pHwOptions->BL) & 0x4)?
        (ctest5 |= SYM895_CTEST5_BL2):(ctest5 &= ~SYM895_CTEST5_BL2);
        }

    ((pHwOptions->BL) & 0x1)?
     (dmode |= SYM895_DMODE_BL0):(dmode &= ~SYM895_DMODE_BL0);
    ((pHwOptions->BL) & 0x2)?
     (dmode |= SYM895_DMODE_BL1):(dmode &= ~SYM895_DMODE_BL1);		

    /* Now Update the Registers with user Options */

    SYM895_REG8_WRITE (pSiop, SYM895_OFF_DMODE,  dmode);
    SYM895_REG8_WRITE (pSiop, SYM895_OFF_STEST1, stest1);
    SYM895_REG8_WRITE (pSiop, SYM895_OFF_STEST2, stest2);
    SYM895_REG8_WRITE (pSiop, SYM895_OFF_STEST3, stest3);
    SYM895_REG8_WRITE (pSiop, SYM895_OFF_SCNTL1, scntl1);
    SYM895_REG8_WRITE (pSiop, SYM895_OFF_SCNTL3, scntl3);
    SYM895_REG8_WRITE (pSiop, SYM895_OFF_CTEST5, ctest5); 

    /* 
     * Note that the SCLK bit needs to be reset if Quadrapler is to be 
     * enabled.
     */

    if ((pHwOptions->QEN) && !(stest1 & SYM895_STEST1_SCLK))
        {
        stest1 |= SYM895_STEST1_QEN;
        SYM895_REG8_WRITE (pSiop, SYM895_OFF_STEST1, stest1);			
			
        /* Poll STEST4:LOCK to ensure that the chip is locked to 160MHz.*/

	while (!(SYM895_REG8_READ (pSiop, SYM895_OFF_STEST4) &  
                 SYM895_STEST4_LOCK))
            ;

        /* Halt the SCSI Clock */

        stest3 |= SYM895_STEST3_HSC;
        SYM895_REG8_WRITE (pSiop, SYM895_OFF_STEST3, stest3);

        /* 
         * Set the CCF and SCF bits. Now since the SCF is dependent on the
         * target, a default value is set which can be changed later by
         * sym895ThreadActivate()
         */

        SYM895_REG8_WRITE (pSiop,SYM895_OFF_SCNTL3,
                           SYM895_REG8_READ (pSiop, SYM895_OFF_SCNTL3) | 
                           pSiop->clkDiv);	
	
        /* Set the SCLK Quadrapler select bit */

        stest1 |= SYM895_STEST1_QSEL;
        SYM895_REG8_WRITE (pSiop, SYM895_OFF_STEST1, stest1);			

        /* Clear the halt on SCSI Clock */

        stest3 &= ~SYM895_STEST3_HSC;
        SYM895_REG8_WRITE (pSiop, SYM895_OFF_STEST3, stest3);

        }

    return (OK);

    }

/*******************************************************************************
*
* sym895Intr - interrupt service routine for the SCSI Controller.
*
* The first thing to determine is whether the device is generating an interrupt
* If not, then this routine must exit as quickly as possible.
*
* Find the event type corresponding to this interrupt, and carry out any
* actions which must be done before the SCSI Controller is re-started.  
* Determine  whether or not the SCSI Controller is connected to the bus 
* (depending on the event type - see note below).  If not, start a client 
* script if possible or else just make the SCSI Controller wait for something 
* else to happen.
*
* The "connected" variable, at the end of switch statement, reflects the status
* of the currently executing thread. If it is TRUE it means that the thread is
* suspended and must be processed at the task level. Set the state of 
* SIOP to IDLE and leave the control to the SCSI Manager. The SCSI Manager, 
* in turn invokes the driver through a "resume" call.
*
* Notify the SCSI manager of a controller event.
*
* RETURNS N/A
*/

void sym895Intr
    (
    SIOP * pSiop        /* pointer to the SIOP structure    */
    )
    {     
    SYM895_EVENT        event;
    SCSI_EVENT    *     pScsiEvent = (SCSI_EVENT *)&event.scsiEvent;
    SCSI_CTRL     *     pScsiCtrl  = (SCSI_CTRL *)pSiop;
    SYM895_THREAD *     pThread    = (SYM895_THREAD *)pSiop->pCurThread;

    BOOL          connected = FALSE;
    BOOL          notify    = TRUE;
    SYM895_STATE  oldState  = (int)(pSiop->state);

    static   UINT32 curBusMode = SYM895_BUSMODE_LVD;
    UINT8    tempBusMode;
    UINT8    stime1;

    CACHE_PIPE_FLUSH();

    /* Save (partial) Controller register context in current thread */

    pThread->nHostFlags = SYM895_REG8_READ (pSiop, SYM895_OFF_SCRATCHA0);
    pThread->msgOutStatus = SYM895_REG8_READ (pSiop, SYM895_OFF_SCRATCHA1);
    pThread->msgInStatus = SYM895_REG8_READ (pSiop, SYM895_OFF_SCRATCHA2);
    pThread->targetId = SYM895_REG8_READ (pSiop, SYM895_OFF_SSID); 
    pThread->busPhase = SYM895_REG8_READ (pSiop, SYM895_OFF_SCRATCHA3); 
 
    pThread->sxfer  = SYM895_REG8_READ (pSiop, SYM895_OFF_SXFER);
    pThread->scntl3 = SYM895_REG8_READ (pSiop, SYM895_OFF_SCNTL3);

#ifdef SCATTER_GATHER

    /* support for scatter gather memory */

    pThread->isScatTransfer = SYM895_REG8_READ (pSiop, SYM895_OFF_SCRATCHC0);
    pThread->totalScatElements = SYM895_REG8_READ (pSiop, SYM895_OFF_SCRATCHC1);
    pThread->noRemaining = SYM895_REG8_READ (pSiop, SYM895_OFF_SCRATCHC2);	
#endif

    if ((pScsiEvent->type = sym895EventTypeGet (pSiop)) == ERROR)
	return;

 
    SYM895_SCSI_DEBUG_MSG ("sym895Intr: DSA=0x%08x, DSP=0x%08x, type = %d. \n",
                           SYM895_REG32_READ(pSiop, SYM895_OFF_DSA),
                           SYM895_REG32_READ(pSiop, SYM895_OFF_DSP),
                           pScsiEvent->type,0,0,0);

    /* Synchronise with single-step routine, if enabled. */

    if (pSiop->isSingleStep)
    	semGive (pSiop->singleStepSem);

    if (pScsiEvent->type == SYM895_SINGLE_STEP)
        return;
    
    /* Process the events */

    switch (pScsiEvent->type)
        {

        /*
         * For events where the bus is not connected to a target,
         * set connected=FALSE.
         * For events where the Thread is disconnected / completed
         * set isThreadRunning = FALSE else set it to TRUE
         * For events not requiring scsi manager, set notify = FALSE.
         */

        /* The following are the events generated by the Scripts */
		
        case SYM895_CMD_COMPLETE       :
        case SYM895_DISCONNECTED       :	
	
             connected = FALSE;
             break;

        case SYM895_MESSAGE_OUT_SENT   :
        case SYM895_MESSAGE_IN_RECVD   :
        case SYM895_EXT_MESSAGE_SIZE   :
        case SYM895_NO_MSG_OUT         :
        case SYM895_SCRIPT_ABORTED     :

             connected = TRUE;
             break;

        case SYM895_PHASE_MISMATCH     :

             event.remCount = sym895RemainderGet (pSiop, pThread->busPhase);  
             connected = TRUE;
             break;
		
        /* 
         * Folowing result from asynchronous conditions. Not directly from 
         * SCRIPTS 
         */

        case SYM895_READY             :

             connected = FALSE;
             notify = FALSE;
             break;
		
        case SYM895_SELECTED          :
        case SYM895_RESELECTED        :
		
             pScsiEvent->busId = (pThread->targetId) & SYM895_SSID_ENC_MASK;
             pScsiEvent->nBytesIdent = 
                        (pThread->nHostFlags & SYM895_FLAGS_IDENTIFY) ? 1 : 0;	
             connected = TRUE;
             break;

        case SYM895_SCSI_BUS_RESET     :
		
             connected = FALSE;
             break;

        case SYM895_BUSMODE_CHANGED    :

             /* Don't run the scripts until, Bus Mode is set properly */

             connected = TRUE;

             /* 
              * Wait for 100 ms before deciding that SCSI bus mode has
              * really changed.It would have been ideal had it been implemented
              * in Hardware.The GEN timer in STIME1 is used for 100ms.
              */
			
             /* First, reset the timer */

             stime1 = SYM895_REG8_READ (pSiop, SYM895_OFF_STIME1);
             stime1 &= ~SYM895_STIME1_GEN_MASK;
             SYM895_REG8_WRITE (pSiop,SYM895_OFF_STIME1, stime1);

             /* Now, set the timer for 100 MS */

             stime1 |= SYM895_GEN_128MS;
             SYM895_REG8_WRITE (pSiop,SYM895_OFF_STIME1, stime1);

             /* Now wait for the Timer interrupt */

             /* 
              * If another Bus mode change interrupt occurs before 
              * the timer expires, re-initialize the timer .              
              */			

             notify = FALSE;
             break;

        case SYM895_GEN_TIMER_EXPIRED  :

             /* Change the Bus mode. */
			
             tempBusMode = (SYM895_REG8_READ (pSiop, SYM895_OFF_STEST4) & 
                            SYM895_STEST4_SMODE_MASK)>>SYM895_BUSMODE_SHIFT;
			
             if (tempBusMode == SYM895_BUSMODE_HVD)
                 SYM895_REG8_WRITE (pSiop, SYM895_OFF_STEST2,
                                    SYM895_REG8_READ (pSiop, SYM895_OFF_STEST2) |
                                    SYM895_STEST2_DIF);
             else if( curBusMode == SYM895_BUSMODE_HVD)
                 SYM895_REG8_WRITE (pSiop, SYM895_OFF_STEST2,
                                    SYM895_REG8_READ (pSiop, SYM895_OFF_STEST2) & 
                                    ~SYM895_STEST2_DIF);
		
             curBusMode = tempBusMode;
		
             /* Please refer to page 2-16, 2-17 of data manual */

             SYM895_REG8_WRITE( pSiop, SYM895_OFF_STEST0,
                                SYM895_REG8_READ (pSiop, SYM895_OFF_STEST0) |
                                (tempBusMode << SYM895_BUSMODE_SHIFT));

             /* Now the Bus Mode is set properly.*/

             /* reset the timer */

             stime1 = SYM895_REG8_READ (pSiop, SYM895_OFF_STIME1);
             stime1 &= ~SYM895_STIME1_GEN_MASK;
             SYM895_REG8_WRITE (pSiop,SYM895_OFF_STIME1, stime1);

             /* 
              * If curBusMode is Single-ended check for the register bits 
              * for Quadrupler, Ultra Enable . As these features are valid 
              * for LVD and HVD, these should be disabled for Single-Ended 
              * devices. Also, lower down the asynchronous
              * transfer rate.
              */

             if (curBusMode == SYM895_BUSMODE_SE)
                 {

                 if ((SYM895_REG8_READ (pSiop, SYM895_OFF_STEST1) & 
                                        SYM895_STEST1_QEN)) 
                     {

                     /* Disable Clock Quadrupler */

                     SYM895_REG8_WRITE (pSiop, SYM895_OFF_STEST1, 
                                        SYM895_REG8_READ (pSiop, 
                                                          SYM895_OFF_STEST1) &
                                        (~(SYM895_STEST1_QEN |
                                         SYM895_STEST1_QSEL)));
                     }
				
                 		
                 /* 
                  * Ultra SCSI transfer rates are not possible with
                  * single-ended devices. So lower the async. clock
                  * conversion factor by hard coding to 40Mhz.
                  */

                 pSiop->clkPeriod = SYM895_40MHZ;
                 pSiop->clkDiv = SYM895_3750MHZ_ASYNC_DIV;
                 SYM895_REG8_WRITE (pSiop,SYM895_OFF_SCNTL3,
                                    SYM895_REG8_READ (pSiop, SYM895_OFF_SCNTL3) |
                                    pSiop->clkDiv);	

					
                 } /* SYM895_BUSMODE_SE */

		
             /* If the current bus mode is HVD, Bit5 in STEST2 should be set */	

             else if (curBusMode == SYM895_BUSMODE_HVD)
                 {
                 SYM895_REG8_WRITE (pSiop,SYM895_OFF_STEST2,
                                    SYM895_REG8_READ (pSiop, SYM895_OFF_STEST2)|
                                    SYM895_STEST2_DIF);
                 } 
		
            /* run the scripts now */

            connected = FALSE;
            notify = FALSE;
            break;					
			
        /* Following are error conditions, both asynchronous and synchronous */
		
        case SYM895_SCSI_TIMEOUT        :
        case SYM895_HANDSHAKE_TIMEOUT   :

             connected = FALSE;
             break;

        case SYM895_ILLEGAL_PHASE       :
	
             connected = TRUE;
             break;

        case SYM895_UNEXPECTED_DISCON   :
	
             connected = FALSE;
             break;

        case SYM895_NO_IDENTIFY         :

             SCSI_MSG ("sym895Intr: No Valid Identify message at (re)select. \n",
                       0, 0, 0, 0, 0, 0);
             connected = TRUE;		
             break;

        case SYM895_SPURIOUS_CMD        :

             SCSI_MSG(" sym895Intr: Spurious Command Interrupt. \n",
                      0, 0, 0, 0, 0, 0);
             connected = FALSE;		
             break;
			
        case SYM895_FATAL_ERROR         :

             SCSI_MSG (" sym895Intr: Fatal Error, Restarting SIOP... \n",
                       0, 0, 0, 0, 0, 0);
             sym895HwInit (pSiop);
             connected = FALSE;		
             break;
			
	default	    :

             SCSI_MSG ("sym895Intr: Un known interrupt (%d) \n",
                       pScsiEvent->type, 0 ,0 ,0 ,0 ,0);
             connected = FALSE;
             break;
	}

    /*
     *  Controller is now idle if possible, make it run a script.
     *
     *	If a SCSI thread is suspended and must be processed at task-level,
     *	leave the device idle.  It will be re-started by the SCSI manager
     *	calling "sym895Resume()".
     *
     *	Otherwise, if there's a new SCSI thread to start (i.e., the SCSI
     *	manager has called "sym895Activate()"), start the appropriate script.
     *
     *	Otherwise, start a script which puts the device into passive mode
     *	waiting for re-selection, selection or a host command.
     *
     *	In all cases, clear any request to start a new thread.  The only
     *	tricky case is when there was a request pending and the device is
     *	left IDLE.  This should only ever occur when the current event is
     *	selection or reselection, in which case the SCSI manager will retry
     *	the activation request.  (Also see "sym895Activate ()".)
     */

    if (connected)
        {
    	pSiop->state = SYM895_STATE_IDLE;
        }
    else if (pSiop->isCmdPending)
        {
        sym895ScriptStart (pSiop, pSiop->pNewThread,
                           SYM895_SCRIPT_INIT_START);

        pSiop->state = SYM895_STATE_ACTIVE;
        }
    else
        {
        sym895ScriptStart (pSiop, (SYM895_THREAD *) pScsiCtrl->pIdentThread,
                           SYM895_SCRIPT_WAIT);
	
        pSiop->state = SYM895_STATE_PASSIVE;
        }

    pSiop->isCmdPending = FALSE;

    SYM895_SCSI_DEBUG_MSG ("sym895Intr state %d -> %d\n",
               oldState, pSiop->state, 0, 0, 0, 0);

    /* Send the event to the SCSI manager to be processed. */

    if (notify)
    	scsiMgrEventNotify ((SCSI_CTRL *) pSiop, pScsiEvent, sizeof (event));

    }

/*******************************************************************************
*
* sym895RemainderGet - get the number of bytes remaining in "Data Path" because of
* phase mismatch.
*
* The Information transfer phase which halted because of the mismatch has been
* saved as a part of the SIOP's register context.
* 
* 895 has few registers helping us out in calculating how many bytes are 
* left not transferred because of some error condition.
* The Registers are.. DFIFO, DBC, SSAT0, SSAT1, SIDx, SODx.
*
* DFIFO register contains the number of bytes transferred between DMA core and
* the SCSI Bus. The DBC register contains the number of bytes transferred 
* across the HostBus. The difference between these two will give the number of 
* bytes in the DMA Fifo....But hold on..There are more issues involved.
* Please refer to chapter 2 of the 895 data manual.
*
* RETURNS: number of bytes remained in the data path.
*
* Note : This function is called at interrupt level.
*
* NOMANUAL
*/

LOCAL UINT sym895RemainderGet 
    (
    SIOP * pSiop,            /* pointer to controller information */
    UINT   phase             /* phase, terminated by mismatch */
    )
    {
    UINT      remCount;
    UINT16    tmpCount;
    UINT16    countFifo;
    UINT16    fifoMask;

    /* Read the Number of bytes transferred across the host bus */

    remCount = SYM895_REG32_READ (pSiop,SYM895_OFF_DBC) & SYM895_COUNT_MASK;
	
    /* 
     * check if the dma fifo is 816 bytes or 112 bytes. 
     * accordingly the mask being applied will change 
     */
 
    if (SYM895_REG8_READ (pSiop,SYM895_OFF_CTEST5) & SYM895_CTEST5_BL2)
        fifoMask = SYM895_FIFO_816_MASK;
    else
        fifoMask = SYM895_FIFO_112_MASK;

    tmpCount  = (remCount & fifoMask);
    countFifo = SYM895_REG8_READ (pSiop,SYM895_OFF_DFIFO);
			
    if (fifoMask == SYM895_FIFO_816_MASK)	/* Fifo size is 816 bytes */
        {

        /* Extract b0,b1 from CTEST5 and concatenate with countfifo.*/

        countFifo |= ((UINT16)(SYM895_REG8_READ (pSiop, SYM895_OFF_CTEST5) & 
                               0x03) << 8);
        }
	
    countFifo &= fifoMask;
	
    /* Now DMA FIFO byte offset counter is in "countFifo" */

    tmpCount = (countFifo - remCount) & fifoMask;

    /* byte count is in "tmpCount" now */		

    /* 
     * Now it depends on the direction of transfer. To know the direction
     * of transfer, check the phase.
     */
	
    switch (phase)
        {
        case PHASE_DATA_OUT     :
        case PHASE_MSG_OUT      :
        case PHASE_COMMAND      :

            /* Output phases : Check for data in output registers (SODx) */
			
            /* 
             * check the SSTAT0 and SSTAT2 registers to check if any bytes
             * are left in the SODL/SODR registers.
             */

            /* Check to see any data is there in SODL register */
				
            if (SYM895_REG8_READ (pSiop, SYM895_OFF_SSTAT0) & SYM895_SSTAT0_OLF)
                tmpCount++;

            if (SYM895_REG8_READ (pSiop, SYM895_OFF_SSTAT2) & SYM895_SSTAT2_OLF1)
                tmpCount++;

            /* 
             * Check to see any data is there in SODR register 
             * Note that SODR reg. is bypassed for asynchronous transfers
             */

            /* if synchronous Send */

            if (((pSiop->pCurThread->sxfer) & SYM895_SXFER_OFF_MASK) != 0)
                {
                if (SYM895_REG8_READ (pSiop, SYM895_OFF_SSTAT0) & 
                                      SYM895_SSTAT0_OLF)
                    tmpCount++;
                if (SYM895_REG8_READ (pSiop, SYM895_OFF_SSTAT2) & 
                                      SYM895_SSTAT2_OLF1)
                    tmpCount++;
                }
			
            /* Finally, add this to "remCount" to get the complete count */

            remCount += tmpCount;

            break;

        case PHASE_DATA_IN	:
        case PHASE_MSG_IN	:
        case PHASE_STATUS	:

            /* These are input phases check for data in input registers */
				
            /* The following for asynchronous transfers */

            if (((pSiop->pCurThread->sxfer) & SYM895_SXFER_OFF_MASK) == 0)
                {
                if (SYM895_REG8_READ (pSiop, SYM895_OFF_SSTAT0) & 
                                      SYM895_SSTAT0_ILF)
                    tmpCount++;

                if (SYM895_REG8_READ (pSiop, SYM895_OFF_SSTAT2) & 
                                      SYM895_SSTAT2_ILF1)
                    tmpCount++;
                }
            else
                {
		
		/* 
                 * For Sync. transfers add the total count, the number of bytes
		 * in the sync FIFO.
	 	 */
		
		tmpCount += (((SYM895_REG8_READ (pSiop, SYM895_OFF_SSTAT1) & 
                                                 0xf0)>>4) |
                             (SYM895_REG8_READ (pSiop, SYM895_OFF_SSTAT2) & 
                                                0x10));
                }

                break;

        default     :
			
            SCSI_MSG ("sym895RemainderGet: Invalid Phase %d\n", phase,
                       0, 0, 0, 0, 0);
			
        }

    /* Clear FiFos */

    SYM895_REG8_WRITE (pSiop, SYM895_OFF_CTEST3,	
                       SYM895_REG8_READ (pSiop,SYM895_OFF_CTEST3)| 
                       SYM895_CTEST3_CLF);
		
    return (remCount);
	
    }
		

/*******************************************************************************
*
* sym895Event - SCSI Controller event processing routine
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
* RETURNS N/A
*/
LOCAL void sym895Event
    (
    SIOP *         pSiop,
    SYM895_EVENT * pEvent
    )
    {
    SCSI_CTRL  *    pScsiCtrl  = (SCSI_CTRL *)  pSiop;
    SCSI_EVENT *    pScsiEvent = &pEvent->scsiEvent;
    SYM895_THREAD * pThread    = (SYM895_THREAD *) pScsiCtrl->pThread;
 
    SYM895_SCSI_DEBUG_MSG ("sym895Event:received event %d (thread = 0x%08x)\n",
                           pScsiEvent->type, (int) pThread, 0, 0, 0, 0);

    /*	Do controller-level event processing */

    switch (pScsiEvent->type)
        {
        case SYM895_SELECTED:
        case SYM895_RESELECTED:

            /*
             * Forward event to current thread, if any (it should defer)
             * then install a reserved thread for identification purposes.
             */

    	    if (pThread != 0)
                sym895ThreadEvent (pThread, pEvent);

    	    pScsiCtrl->peerBusId = pScsiEvent->busId;

    	    pScsiCtrl->pThread = pScsiCtrl->pIdentThread;
	    
            pScsiCtrl->pThread->role = (pScsiEvent->type == SCSI_EVENT_SELECTED)
		    	    	     ? SCSI_ROLE_IDENT_TARG :
                                       SCSI_ROLE_IDENT_INIT;

            pThread = (SYM895_THREAD *) pScsiCtrl->pThread;

            scsiMgrCtrlEvent (pScsiCtrl, SCSI_EVENT_CONNECTED);
            break;

        case SYM895_DISCONNECTED:
        case SYM895_CMD_COMPLETE:
        case SYM895_UNEXPECTED_DISCON:
        case SYM895_SCSI_TIMEOUT:

            pScsiCtrl->peerBusId = NONE;
    	    pScsiCtrl->pThread   = 0;
            scsiMgrCtrlEvent (pScsiCtrl, SCSI_EVENT_DISCONNECTED);
   
            break;

        case SYM895_SCSI_BUS_RESET:
	
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
                SCSI_MSG ("sym895Event invalid event type (%d)\n",
                          pScsiEvent->type, 0, 0, 0, 0, 0);
                }
            break;
        }

    /* If there's a thread on the controller, forward the event to it */
	
    if (pThread != 0)
        sym895ThreadEvent (pThread, pEvent);
    }
    
/******************************************************************************
*
* sym895ThreadInit - initialize a client thread structure
*
* Initialize the fixed data for a thread (i.e., independent of the command).
* Called once when a thread structure is first created.
*
* RETURNS OK, or ERROR if an error occurs
*
* NOMANUAL
*/

LOCAL STATUS sym895ThreadInit
    (
    SIOP *          pSiop,
    SYM895_THREAD * pThread
    )
    {
    
    if (scsiThreadInit (&pThread->scsiThread) != OK)
        return (ERROR);
	
    pThread->pShMem = pSiop->pClientShMem;
    pThread->scntl3 = pSiop->clkDiv;	

    return (OK);
    }

/******************************************************************************
*
* sym895IdentThreadInit - initialize an identification thread structure
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

LOCAL STATUS sym895IdentThreadInit
    (
    SYM895_THREAD * pThread
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
    pThread->msgOutStatus = SCSI_MSG_OUT_NONE;
    pThread->msgInStatus  = SCSI_MSG_IN_NONE;

    pThread->sxfer = SYM895_ASYNC_SXFER;

    /*
     * The value set in scntl3 for the ident message is chosen based on the.
     * following reasons ...
     * 1. Ident message is Async by default. So the bits SCFx donot matter now.
     * 2. The target can be an Ultra SCSI or just FAST SCSI, so can't
     *    enable those ULTRA bits now, effectively ruling out the possibility
     *	  of using 160/80 MHz SCSI Clocks for ident message.
     * 3. We chose to hardcode the value of 50 Mhz into scntl3 for ident 
     *    message. After negotiation, these values will suitably be changed.
     */

    pThread->scntl3 = SYM895_3750MHZ_ASYNC_DIV;  /* Hard Code */

    /* pThread->scntl3 = pSiop->clkDiv; */

    /* Initialise shared memory area */

    pThread->pShMem = pSiop->pIdentShMem;
    
    sym895SharedMemInit (pSiop, pThread->pShMem);

    sym895ThreadUpdate (pThread);
    
    return (OK);
    }

/******************************************************************************
*
* sym895SharedMemInit - initialize the fields in a shared memory area
*
* Initialise pointers and counts for all message transfers.  These are
* always directed to buffers provided by the SCSI_CTRL structure.
*
* RETURNS: N/A
*
* NOMANUAL
*/
LOCAL void sym895SharedMemInit
    (
    SIOP *          pSiop,
    SYM895_SHARED * pShMem
    )
    {

    /*
     * Note: All the values being set in the shared memory need to be
     * 32 bit byte swapped. Actual swapping takes place when, when memory
     * is big endian (since the SYM895 is a little endian PCI device. 
     * Otherwise, if memory is little endian then the byte swapping 
     * does not do anything. The "endianness" is configurable at compile
     * time.
     *
     * Similarly, a PCI offset is added to all addresses so that the
     * SYM895 PCI chip can get to memory. This value is also configurable
     * at compile time.
     */

    pShMem->identIn.size = SYM895_SWAP_32 (1);
    pShMem->identIn.addr = (UINT8 *)SYM895_SWAP_32 (SYM895_VIRT_TO_PHYS 
                                                   (pSiop->scsiCtrl.identBuf));

    pShMem->msgOut.size = SYM895_SWAP_32 (0);	/* set dynamically */
    pShMem->msgOut.addr = (UINT8 *)SYM895_SWAP_32 (SYM895_VIRT_TO_PHYS 
                                                  (pSiop->scsiCtrl.msgOutBuf));
    
    pShMem->msgIn.size  = SYM895_SWAP_32 (1);
    pShMem->msgIn.addr  = (UINT8 *)SYM895_SWAP_32 (SYM895_VIRT_TO_PHYS 
                                                  (pSiop->scsiCtrl.msgInBuf));

    pShMem->msgInSecond.size = SYM895_SWAP_32 (1);
    pShMem->msgInSecond.addr = (UINT8 *)SYM895_SWAP_32 (SYM895_VIRT_TO_PHYS 
                                                     (pSiop->scsiCtrl.msgInBuf)
                                                     + (UINT) 1);

    pShMem->msgInRest.size = SYM895_SWAP_32 (0);  /* set dynamically */
    pShMem->msgInRest.addr = (UINT8 *)SYM895_SWAP_32 (SYM895_VIRT_TO_PHYS
                                                     (pSiop->scsiCtrl.msgInBuf)
                                                      + (UINT) 2);
    }


/******************************************************************************
*
* sym895ThreadActivate - activate a SCSI connection for an initiator thread
*
* Set whatever thread/controller state variables need to be set.  Ensure that
* all buffers used by the thread are coherent with the contents of the
* system caches (if any).
*
* Set transfer parameters for the thread based on what its target device
* last negotiated.
*
* Update the thread context (including shared memory area) and note that
* there is a new client script to be activated (see "sym895Activate()").
*
* Set the thread's state to ESTABLISHED.
* Do not wait for the script to be activated.  Completion of the script will
* be signalled by an event which is handled by "sym895Event()".
*
* RETURNS OK or ERROR
*
* NOMANUAL
*/

LOCAL STATUS sym895ThreadActivate
    (
    SIOP *          pSiop,		/* pointer to SIOP structure */
    SYM895_THREAD * pThread		/* pointer to thread structure */
    )
    {
    SCSI_CTRL *   pScsiCtrl   = (SCSI_CTRL *) pSiop;
    SCSI_THREAD * pScsiThread = &pThread->scsiThread;
    SCSI_TARGET * pScsiTarget = pScsiThread->pScsiTarget;
    
    SYM895_SCSI_DEBUG_MSG ("sym895ThreadActivate: thread 0x%08x activating\n",
              (int) pThread, 0, 0, 0, 0, 0);

    /* Ensure buffers used by the thread are coherent with system cache */

    scsiCacheSynchronize (pScsiThread, SCSI_CACHE_PRE_COMMAND);

    /* Reset controller state variables set sync xfer parameters */

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

    if (sym895ThreadParamsSet (pThread, pScsiTarget->xferOffset,
                               pScsiTarget->xferPeriod) != OK)
        {
        SYM895_SCSI_DEBUG_MSG ("sym895ThreadActivate failed to set thread  "
                  "params.\n", 0, 0, 0, 0, 0, 0);
        return (ERROR);
        }

    /*
     *  Concatenate the ident message with a pending 'normal' message out,
     *  if possible.  This allows the script to send the first message out
     *  within the same MSG OUT phase as the IDENTIFY message - needed on
     *  some target systems (e.g. DG Clariion RAID) to avoid the message
     *  being rejected(!). The identMsg buffer in pSiop structure is 
     *  initialised here which will be used for the current thread's IdentOut
     *  buffer.
     */

    pSiop->identMsgLength = 0;

    bcopy ((char *) pScsiThread->identMsg,
           (char *) pSiop->identMsg,
           pScsiThread->identMsgLength);

    pSiop->identMsgLength += pScsiThread->identMsgLength;

    if (pScsiCtrl->msgOutState == SCSI_MSG_OUT_PENDING)
        {
        bcopy ((char *) pScsiCtrl->msgOutBuf,
               (char *) pSiop->identMsg + pSiop->identMsgLength,
               pScsiCtrl->msgOutLength);

        pSiop->identMsgLength += pScsiCtrl->msgOutLength;
        }


    /* Update thread context; activate the thread */

    sym895ThreadUpdate (pThread);
    
    if (sym895Activate (pSiop, pThread) != OK)
        {
        SYM895_SCSI_DEBUG_MSG ("sym895ThreadActivate: failed to \
                                activate thread.\n", 0, 0, 0, 0, 0, 0);
        return (ERROR);
        }

    pScsiCtrl->pThread = pScsiThread;

    sym895ThreadStateSet (pThread, SCSI_THREAD_ESTABLISHED);
    
    return (OK);
    }


/******************************************************************************
*
* sym895ThreadAbort - abort a thread
*
* If the thread is not currently connected, do nothing and return FALSE to
* indicate that the SCSI manager should abort the thread.
*
* If the thread is active, build an ABORT or ABORT TAG message which will be 
* sent (eventually), causing the target to disconnect. Abort the current 
* script being run so that this message can be sent. Update the thread state
* and return TRUE to indicate that the controller driver will handle the abort
* process. 
* 
* RETURNS TRUE if the thread is being aborted by this driver (i.e. it is
* currently active on the controller, else FALSE.
*
* NOMANUAL
*/

LOCAL BOOL sym895ThreadAbort
    (   
    SIOP *          pSiop,		/* pointer to controller structure */
    SYM895_THREAD * pThread		/* pointer to thread structure     */
    )
    {
    BOOL          tagged;
    SCSI_CTRL *   pScsiCtrl   = (SCSI_CTRL *)   pSiop;
    SCSI_THREAD * pScsiThread = &pThread->scsiThread;


    SYM895_SCSI_DEBUG_MSG ("sym895ThreadAbort: thread 0x%08x (state = %d) "
              "aborting\n", (int)pThread, pScsiThread->state, 0, 0, 0, 0);

    /* If the requested thread is the not the currently run thread */

    if (pScsiThread != pScsiCtrl->pThread)
	return (FALSE);
    
    switch (pScsiThread->state)
        {
        case SCSI_THREAD_INACTIVE:
        case SCSI_THREAD_WAITING:
        case SCSI_THREAD_DISCONNECTED:

            /* If thread not connected, do nothing.*/

            return (FALSE);
            break;

        default:

            /*
             * Build an ABORT (or ABORT TAG) message.  When this has been
             * sent, the target should disconnect.  Mark the thread aborted
             * and continue until disconnection.
             */
            
            /*  Check whether a tagged message is requested */

            tagged = (pScsiThread->tagNumber != NONE);

            pScsiCtrl->msgOutBuf[0] = tagged ? SCSI_MSG_ABORT_TAG :
                                               SCSI_MSG_ABORT;
            pScsiCtrl->msgOutLength = 1;
            pScsiCtrl->msgOutState  = SCSI_MSG_OUT_PENDING;

            /* Abort the current SCRIPT */

            sym895Abort (pSiop);

            /* Update the thread state */  

    	    sym895ThreadStateSet (pThread, SCSI_THREAD_ABORTING);
		    break;
        }

    return (TRUE);
    }

/******************************************************************************
*
* sym895ScsiBusControl - miscellaneous low-level SCSI bus control operations
*
* Currently supports only the SCSI_BUS_RESET operation; other operations are
* not used explicitly by the driver because they are carried out automatically
* by the script program.
*
* NOTE after the SCSI bus has been reset, the SIOP generates a
* SYM895_BUS_RESET event via an interrupt,to be sent to the SCSI manager.
* See "sym895Intr()".
*
* RETURNS OK, or ERROR if an invalid operation is requested.
*
* NOMANUAL
*/

LOCAL STATUS sym895ScsiBusControl
    (
    SIOP * pSiop,   	    /* pointer to controller info               */
    int    operation	    /* bitmask for operation(s) to be performed */
    )
    {
    if (operation & SCSI_BUS_RESET)
        {
        sym895ScsiBusReset (pSiop);
        return (OK);
        }

    return (ERROR); /* unknown operation */
    }


/******************************************************************************
*
* sym895ScsiBusReset - assert the RST line on the SCSI bus 
*
* Issue a SCSI Bus Reset command to the SYM 895. This should put all devices
* on the SCSI bus in an initial quiescent state.
*
* The bus reset will generate an interrupt which is handled by the normal
* ISR (see "sym895Intr()").
*
* RETURNS: N/A
*
* NOMANUAL
*/

LOCAL void sym895ScsiBusReset
    (
    FAST SIOP * pSiop	/* pointer to SIOP info */
    )
    {

    int key;
    key = intLock ();
	
    /* assert the reset bit */

    SYM895_REG8_WRITE (pSiop,SYM895_OFF_SCNTL1,
                       SYM895_REG8_READ(pSiop, SYM895_OFF_SCNTL1) | 
                       SYM895_SCNTL1_RST);

   /* do it again to make sure that it took */

    SYM895_REG8_WRITE (pSiop,SYM895_OFF_SCNTL1,
                       SYM895_REG8_READ(pSiop, SYM895_OFF_SCNTL1) | 
                       SYM895_SCNTL1_RST);

    intUnlock (key);

    /* The RST signal should be asserted for a min. duration of 25u sec */

    taskDelay (1);			

    /* Now clear the Reset signal..(de-assertion) */

    SYM895_REG8_WRITE (pSiop,SYM895_OFF_SCNTL1,
                       SYM895_REG8_READ(pSiop, SYM895_OFF_SCNTL1) & 
                       ~SYM895_SCNTL1_RST);
    }

/******************************************************************************
*
* sym895XferParamsQuery - get synchronous transfer parameters
*
* Updates the synchronous transfer parameters suggested in the call to match
* the  SYM 895 SCSI Controller's capabilities.  Transfer period is in SCSI units
* (multiples * of 4 ns).
*
* Note: The transfer period is made longer and the offset is made smaller if
* the SYM895 cannot handle the specified values.
*
* RETURNS OK
*/

LOCAL STATUS sym895XferParamsQuery
    (
    SCSI_CTRL * pScsiCtrl,         /* pointer to controller structure */
    UINT8     * pOffset,           /* max REQ/ACK offset  [in/out]    */
    UINT8     * pPeriod	           /* min transfer period [in/out]    */
    )
    {

    UINT8 unused; 

    (void)sym895XferParamsCvt ((SIOP *)pScsiCtrl, pOffset, pPeriod, 
                               &unused, &unused );
    return (OK);
    }
    
/******************************************************************************
*
* sym895XferParamsSet - set transfer parameters
*
* Validate the requested parameters, convert to the SYM895 SCSI Controller's 
* native format and save in the current thread for later use (the chip's
* registers are not actually set until the next script activation for this 
* thread).
*
* Transfer period is specified in SCSI units (multiples of 4 ns).  An offset
* of zero specifies asynchronous transfer.
*
* RETURNS OK if transfer parameters are OK, else ERROR.
*/

LOCAL STATUS sym895XferParamsSet
    (
    SCSI_CTRL * pScsiCtrl,		/* pointer to controller info */
    UINT8       offset,			/* max REQ/ACK offset         */
    UINT8       period			/* min transfer period        */
    )
    {
    SYM895_THREAD * pThread = (SYM895_THREAD *) pScsiCtrl->pThread;
    return (sym895ThreadParamsSet (pThread, offset, period));
    }

/******************************************************************************
*
* sym895WideXferParamsQuery - get wide data transfer parameters
*
* Updates the wide data transfer parameters suggested in the call to match
* the SYM895 SCSI Controller's capabilities.  Transfer width is in the units 
* of the WIDE DATA TRANSFER message's transfer width exponent field. This is
* an 8 bit field where 0 represents a narrow transfer of 8 bits, 1 represents
* a wide transfer of 16 bits and 2 represents a wide transfer of 32 bits.
*
* NOTE: The transfer width is made smaller if the SYM895 cannot handle the 
* specified value.
*
* RETURNS OK
*/

LOCAL STATUS sym895WideXferParamsQuery
    (
    SCSI_CTRL * pScsiCtrl,		/* pointer to controller structure  */
    UINT8     * xferWidth		/* suggested transfer width         */
    )
    {
    if (*xferWidth > SYM895_MAX_XFER_WIDTH)
	*xferWidth = SYM895_MAX_XFER_WIDTH;
    
    sym895WideXferParamsSet (pScsiCtrl, *xferWidth);

    return (OK);
    }

/******************************************************************************
*
* sym895WideXferParamsSet - set wide transfer parameters
*
* Assume valid parameters and set the SYM895's thread parameters to the
* appropriate values. The actual registers are not written yet, but will
* be written from the thread values when it is activated.
*
* Transfer width is specified in SCSI transfer width exponent units. 
*
* RETURNS OK 
*/

LOCAL STATUS sym895WideXferParamsSet
    (
    SCSI_CTRL * pScsiCtrl,		/* pointer to controller structure */
    UINT8       xferWidth 		/* wide data transfer width        */
    )
    {

    SYM895_THREAD * pThread = (SYM895_THREAD *) pScsiCtrl->pThread;
     
    if ( xferWidth == SCSI_WIDE_XFER_SIZE_NARROW )
        {
           pThread->scntl3 &= ~SYM895_SCNTL3_EWS; /* Disable wide SCSI */
        }
    else 
        {
           pThread->scntl3 |= SYM895_SCNTL3_EWS;  /* Enable wide SCSI */
        }

    return (OK);
    }

/******************************************************************************
*
* sym895ThreadParamsSet - set various parameters for a thread
*
* Parameters include transfer offset and period, as well as the ID of the
* target device.  All of these end up as encoded values stored either in
* the thread's register context or its associated shared memory area.
*
* Transfer period is specified in SCSI units (multiples of 4 ns).  An offset
* of zero specifies asynchronous transfer.
*
* RETURNS OK if parameters are OK, else ERROR.
*
* NOMANUAL
*/

LOCAL STATUS sym895ThreadParamsSet
    (
    SYM895_THREAD * pThread,	    	/* thread to be affected  */
    UINT8           offset,		/* max REQ/ACK offset     */
    UINT8           period		/* min transfer period    */
    )
    {
     
    SCSI_THREAD * pScsiThread = (SCSI_THREAD *) pThread;
    SIOP *        pSiop       = (SIOP *) pScsiThread->pScsiCtrl;
    SCSI_TARGET * pScsiTarget = pScsiThread->pScsiTarget;

    UINT          busId       = pScsiTarget->scsiDevBusId;
    UINT8         xferWidth   = pScsiTarget->xferWidth;
     
    if ( !sym895XferParamsCvt (pSiop, &offset, &period, &pThread->sxfer,
                               &pThread->scntl3))
  	{
     	errnoSet(S_scsiLib_ILLEGAL_PARAMETER);
     	return  (ERROR); 
     	}
   
    /* Check whether WIDE transfer is requested */

    if ( xferWidth == SCSI_WIDE_XFER_SIZE_DEFAULT )
        {
  	pThread->scntl3 |= SYM895_SCNTL3_EWS;
    	}

    pThread->pShMem->device = SYM895_SWAP_32
                      (
                      (pThread->sxfer << SYM895_XFER_PARAMS_SHIFT) |
                      (busId << SYM895_TARGET_BUS_ID_SHIFT)        |
                      (pThread->scntl3 << 24)
                      );

    return (OK);
    }


/******************************************************************************
*
* sym895XferParamsCvt - convert transfer period from SCSI to SYM 53C895 units
*
* Given a "suggested" REQ/ACK offset and transfer period (in SCSI units of
* 4 ns), return the nearest offset and transfer period the SYM 53C895 is
* capable of using.  Also return the corresponding values of the SYM 53C895's
* Synchronous Transfer and Synchronous Bus Control registers.
*
* An offset of zero specifies asynchronous transfer, in which case the period
* is irrelevant.  Otherwise, the offset may be clipped to be within the
* maximum limit the SYM 53C885 is capable of.
*
* The transfer period is normally rounded towards longer periods if the SYM
* 53C895 is not capable of using the exact specified value.  The period may
* also be clipped to be within the SYM 53C895's maximum and minimum limits
* according to its clock period.
*
* If either the offset or period need to be clipped, the value FALSE is
* retuned as this may reflect an error condition.
*
* RETURNS: TRUE if input period is within SYM 53C895's range, else FALSE
*
* NOMANUAL
*/

LOCAL BOOL sym895XferParamsCvt
    (
    FAST SIOP  * pSiop,	        /* pointer to controller info        */
    FAST UINT8 * pOffset,       /* REQ/ACK offset                    */
    FAST UINT8 * pPeriod,       /* xfer period, SCSI units (x 4 ns)  */
    FAST UINT8 * pXferParams,	/* corresponding Sync Xfer Reg value */
    FAST UINT8 * pClockDiv      /* corresponding clock division bits */
    )
    {
    UINT  offset  = (UINT) *pOffset;
    UINT  period  = (UINT) *pPeriod;
    UINT  xferp;
    UINT  sccpDiv;
    UINT8 clkDiv=1;
    UINT8 xferParams  = 0;		/* initialised to avoid warnings */
    UINT8 clockDivide = 0;		/* initialised to avoid warnings */
    UINT  sccp;					/* Scsi Core Clock Period */

    if (offset == SCSI_SYNC_XFER_ASYNC_OFFSET)
        {
        xferParams  = SYM895_ASYNC_SXFER;
        clockDivide = pSiop->clkDiv; 

        offset = SCSI_SYNC_XFER_ASYNC_OFFSET;
        period = 0;
        }
    else
	{

	SYM895_SCSI_DEBUG_MSG ("sym895XferParamsCvt: requested: "
                               "offset = %d, period = %d\n",
                                offset, period, 0, 0, 0, 0);

        /* Default sccpDiv and clkDiv values for SCLK <= 50MHz */

	sccpDiv = 100;
	clkDiv = 1;

        /* 
         * For SCLK > 66.7MHz. To acheive ultra 2 rates, Clock Quadrupler must
         * be turned on (SCLK=160MHz)& clock divider must be set to /1. Also,
         * the scsi options for a particular target must be set to have minimum
         * transfer period of 25ns. This can be done using "scsiTargetOptionsSet".
         */

        if (pSiop->clkPeriod < SYM895_6666MHZ)
            {
	    sccpDiv = 100;
            clkDiv = 1; 
            }	  

       /* Unless the SCLK is > 50MHz but < 66.7MHz */

        else if (pSiop->clkPeriod < SYM895_50MHZ) /* then faster than 50MHZ */
            {
	    sccpDiv = 67;
            clkDiv = 2;
            }

        /* convert all variables to ns */

        period *= 4; /* ns */
        sccp = pSiop->clkPeriod / sccpDiv; /* ns */

        xferp = period / sccp;
	
	/* adjust xferp to fit the allowable range */

	if (xferp < SYM895_MIN_XFERP)
	    xferp = SYM895_MIN_XFERP;

        else if (xferp > SYM895_MAX_XFERP) 
	    xferp = SYM895_IDEAL_XFERP;

        /* update the period using the adjusted xferp */

        period = xferp * sccp;

        /* adjust the synchronous offset to fit the allowable range */

	if (offset < SYM895_MIN_SYNC_OFFSET)
	    offset = SYM895_MIN_SYNC_OFFSET;

        else if (offset > SYM895_MAX_SYNC_OFFSET)
	    offset = SYM895_MAX_SYNC_OFFSET;

        xferParams = (((UINT8) xferp - SYM895_MIN_XFERP) & 0x07) << 
                      SYM895_SYNC_XFER_PERIOD_SHIFT; 

        xferParams |= ((UINT8) offset & 0x1f);

        /* 
	 * The clock dividers for scntl3 remain fixed because this is a static
  	 * value that does not change on a per transfer basis. The divider
 	 * is based on the SCLK frequency from the board to the SCSI core 
 	 */

	clockDivide = (pSiop->clkDiv);
    	clockDivide = clockDivide | (clkDiv << 4); 

        /* convert the period back to 4ns units */

        period /= 4;

        } /* else */

    SYM895_SCSI_DEBUG_MSG ("sym895XferParamsCvt: converted to: "
                           "offset     = %d, period      = %d\n"
                           "xferParams = %x, clockDivide = %x\n",
                           offset, period, xferParams, clockDivide, 0, 0);

    *pOffset     = offset;
    *pPeriod     = period;
    *pXferParams = xferParams;
    *pClockDiv   = clockDivide;

    return (TRUE);
    }

/******************************************************************************
*
* sym895EventTypeGet - parse status registers at interrupt time
*
* This routine examines the device state to determine what type of event
* is pending.  If none is pending then a fatal error code is returned.
*
* RETURNS Returns an interrupt (event) type code or SYM895_FATAL_ERROR.
*/

LOCAL int sym895EventTypeGet
    (
    SIOP * pSiop
    )
    {
    int key;

    UINT8 intrStatus   = 0;
    UINT8 dmaStatus    = 0;
    UINT8 scsiStatus_0 = 0;
    UINT8 scsiStatus_1 = 0;

    /*
     * We should be locked in interrupt context while the status
     * registers are being read so that there is no contention between
     * a synchronous thread and a bus initiated thread (reselect)
     */

    key = intLock (); 

    CACHE_PIPE_FLUSH();

    /*
     * Read the interrupt status register.
     * For reading the DSTAT, SIST0 and SIST1 registers,
     * each register should be read with a delay of 12 SCLK delays between
     * successive reads. Please refer to data manual for additional details
     */
	
    intrStatus = SYM895_REG8_READ (pSiop, SYM895_OFF_ISTAT);

    SYM895_SCSI_DEBUG_MSG (" istat is 0x%x..\n", intrStatus,0,0,0,0,0);

    /* Check for the INTF bit and clear it if set */

    if (intrStatus & SYM895_ISTAT_INTF)
 	SYM895_REG8_WRITE (pSiop,SYM895_OFF_ISTAT, SYM895_ISTAT_INTF);

    /* check if there is an interrupt */	

    if ((intrStatus & (SYM895_ISTAT_SIP | SYM895_ISTAT_DIP)) != 0)  
	{

	/* if ISTAT:ABRT bit is set, reset before reading the DMA status reg */

	if (intrStatus & SYM895_ISTAT_ABRT)
 	    SYM895_REG8_WRITE(pSiop, SYM895_OFF_ISTAT, 0x00);
		
        if ((intrStatus & SYM895_ISTAT_SIP) && (intrStatus & SYM895_ISTAT_DIP))
            SYM895_SCSI_DEBUG_MSG (" sym895EventTypeGet:  \
                   Both DMA and SCSI Interrupts \n", 0, 0, 0, 0, 0, 0);

        /* 
         * Read the interrupt status registers if that type of 
         * interrupt occurs 
         */		
        
        CACHE_PIPE_FLUSH();  

	if (intrStatus & SYM895_ISTAT_DIP)
 	    {
            dmaStatus = SYM895_REG8_READ (pSiop, SYM895_OFF_DSTAT);
            sym895Delay ();
            }
     
        CACHE_PIPE_FLUSH();

        if (intrStatus & SYM895_ISTAT_SIP)
            {
            scsiStatus_0 = SYM895_REG8_READ (pSiop, SYM895_OFF_SIST0);
            sym895Delay ();
            scsiStatus_1 = SYM895_REG8_READ (pSiop, SYM895_OFF_SIST1);
            }
		
     	intUnlock (key);
	
	}
    else
	{
	intUnlock (key);
	return (ERROR);
	}

    SYM895_SCSI_DEBUG_MSG ("sym895EventTypeGet: ISTAT : 0x%02x SIST0 : 0x%02x \
              SIST1 : 0x%02x DSTAT : 0x%02x \n\n", intrStatus, scsiStatus_0,
              scsiStatus_1,dmaStatus,0,0);
	
    /* Now start checking for the source of interrupts */	
	
    /* Fatal conditions first !! */
	
    if (scsiStatus_0 & SYM895_B_SGE)
	{
	SCSI_MSG ("sym895EventTypeGet: SCSI Gross Error\n ", 0, 0, 0, 0, 0, 0);
	return (SYM895_FATAL_ERROR);
	}

    if (scsiStatus_0 & SYM895_B_PAR)
	{
	SCSI_MSG ("sym895EventTypeGet: SCSI Parity Error\n", 0, 0, 0, 0, 0, 0);
	return (SYM895_FATAL_ERROR);
	}

    if (dmaStatus & SYM895_B_IID)
	{
	SCSI_MSG ("sym895EventTypeGet : IIlegal Instruction detected\n ",
	 	 SYM895_REG32_READ (pSiop,SYM895_OFF_DSP), 0, 0, 0, 0, 0);
	return (SYM895_FATAL_ERROR);
	}

    if (dmaStatus & SYM895_B_BF)
	{
	SCSI_MSG ("sym895EventTypeGet : SCSI Bus Fault\n",
		SYM895_REG32_READ (pSiop,SYM895_OFF_DSP),
		SYM895_REG32_READ (pSiop,SYM895_OFF_DBC) & 0xffffff, 0, 0, 0, 0);
	return (SYM895_FATAL_ERROR);
	}

    /* Now, non-fatal error checking: The order of checking can be important */

    if (scsiStatus_0 & SYM895_B_RST)
	{
	SYM895_SCSI_DEBUG_MSG ("sym895EventTypeGet : BusReset \n", 
                               0, 0, 0, 0, 0, 0);
		
	/* Clear DMA FIFO */
	
	SYM895_REG8_WRITE (pSiop, SYM895_OFF_CTEST3,	
		           SYM895_REG8_READ (pSiop,SYM895_OFF_CTEST3) | 
                           SYM895_CTEST3_CLF);
		
	return (SYM895_SCSI_BUS_RESET);
	}
	
    if (scsiStatus_0 & SYM895_B_UDC)
	{
        SYM895_SCSI_DEBUG_MSG ("sym895EventTypeGet : Unexpected Disconnection \n",
                  0, 0, 0, 0, 0, 0);
		
        /* Clear DMA FIFO */
	
	SYM895_REG8_WRITE (pSiop, SYM895_OFF_CTEST3,	
	                   SYM895_REG8_READ (pSiop,SYM895_OFF_CTEST3) | 
                           SYM895_CTEST3_CLF);
		
        return (SYM895_UNEXPECTED_DISCON);
        }

    if (scsiStatus_1 & SYM895_B_STO)
	{
	SYM895_SCSI_DEBUG_MSG ("sym895EventTypeGet : Bus Timeout \n", 
                               0, 0, 0, 0, 0, 0);
		
	/* Clear DMA FIFO */
	
	SYM895_REG8_WRITE (pSiop, SYM895_OFF_CTEST3,	
	                   SYM895_REG8_READ (pSiop,SYM895_OFF_CTEST3) | 
                           SYM895_CTEST3_CLF);
		
        return (SYM895_SCSI_TIMEOUT);
        }

    if (scsiStatus_1 &SYM895_B_HTH)
        {
        SYM895_SCSI_DEBUG_MSG ("sym895EventTypeGet : Bus handshake Timeout \n",
                  0, 0, 0, 0, 0, 0);
		
	/* Clear DMA FIFO */
	
	SYM895_REG8_WRITE (pSiop, SYM895_OFF_CTEST3,	
			   SYM895_REG8_READ (pSiop,SYM895_OFF_CTEST3) | 
                           SYM895_CTEST3_CLF);
		
        return (SYM895_HANDSHAKE_TIMEOUT);
        }

    if (scsiStatus_1 & SYM895_B_SBMC)
        {
	SYM895_SCSI_DEBUG_MSG ("sym895EventTypeGet : Bus Mode Change Detected \n",
                  0, 0, 0, 0, 0, 0);
		
        return (SYM895_BUSMODE_CHANGED);
	}

    if (scsiStatus_1 & SYM895_B_GEN)
	{
	SYM895_SCSI_DEBUG_MSG ("sym895EventTypeGet : General Timer Expired \n",
                  0, 0, 0, 0, 0, 0);
		
	return (SYM895_GEN_TIMER_EXPIRED);
	}

    if (scsiStatus_0 & SYM895_B_MA)
	{

	/* 
         * This is a Phase Misatch interrupt when in Initiator mode and 
	 * signals arrertion of ATN in Target Mode.
	 */

	/* 
         * if in Target mode..Fatal error as this intr is supposed to be 
	 * disabled earlier. 
	 */

	if (SYM895_REG8_READ (pSiop, SYM895_OFF_SCNTL0) & SYM895_SCNTL0_TRG)
            {

            SCSI_MSG (" sym895EventTypeGet : ATN intr. in target mode \n",
                      0, 0, 0, 0, 0, 0);
            return (SYM895_FATAL_ERROR);
            }
	else
	    {

	    /* Initiator mode */

            SYM895_SCSI_DEBUG_MSG (" sym895EventTypeGet : Phase Mismatch \n",
                      0, 0, 0, 0, 0, 0);
            return (SYM895_PHASE_MISMATCH);
            }
	}

    if (dmaStatus & SYM895_B_ABT)
	{
	SYM895_SCSI_DEBUG_MSG (" sym895EventTypeGet : Script Aborted \n",
                  0, 0, 0, 0, 0, 0);
	return (SYM895_SCRIPT_ABORTED);
	}

    if (dmaStatus & SYM895_B_SIR)
	{
	SYM895_SCSI_DEBUG_MSG (" sym895EventTypeGet : Script Interrupt \n",
                  0, 0, 0, 0, 0, 0);		

	/* return the stored interrupt vector */

	return SYM895_REG32_READ (pSiop,SYM895_OFF_DSPS);
	}

    if (dmaStatus & SYM895_B_SSI)
	{
	SYM895_SCSI_DEBUG_MSG (" sym895EventTypeGet : Single Step Interrupt \n",
                  0, 0, 0, 0, 0, 0);		

	return (SYM895_SINGLE_STEP);
	}

    /* No reason for the interrupt ! */

    SCSI_MSG (" sym895EventTypeGet : Spurious Interrupt \n",
              0, 0, 0, 0, 0, 0);		
    
    return (SYM895_FATAL_ERROR);
    }



/******************************************************************************
*
* sym895ThreadEvent - SCSI Controller thread event processing routine
*
* Forward the event to the proper handler for the thread's current role.
*
* If the thread is still active, update the thread context (including
* shared memory area) and resume the thread.
*
* RETURNS N/A
*/
LOCAL void sym895ThreadEvent
    (
    SYM895_THREAD * pThread,
    SYM895_EVENT *  pEvent
    )
    {
    SCSI_EVENT *  pScsiEvent  = &pEvent->scsiEvent;
    SCSI_THREAD * pScsiThread = &pThread->scsiThread;
    SIOP *        pSiop       = (SIOP *) pScsiThread->pScsiCtrl;
    SYM895_SCRIPT_ENTRY entryPt;
    
    SYM895_SCSI_DEBUG_MSG ("sym895ThreadEvent thread 0x%08x received event %d\n",
              (int) pThread, pScsiEvent->type, 0, 0, 0, 0);

    switch (pScsiThread->role)
	{
	case SCSI_ROLE_INITIATOR:
    	    
            sym895InitEvent (pThread, pEvent);
	    entryPt = SYM895_SCRIPT_INIT_CONTINUE;
            break;
	    
	case SCSI_ROLE_IDENT_INIT:

            sym895InitIdentEvent (pThread, pEvent);
	
	    entryPt = SYM895_SCRIPT_INIT_CONTINUE;
	    break;
 
        case SCSI_ROLE_IDENT_TARG:

	    sym895TargIdentEvent (pThread, pEvent);
	    entryPt = SYM895_SCRIPT_TGT_DISCONNECT;
	    break;

	case SCSI_ROLE_TARGET:
	default:

            SCSI_MSG ("sym895ThreadEvent thread 0x%08x invalid role (%d)\n",
                      (int) pThread, pScsiThread->role, 0, 0, 0, 0);

            entryPt = SYM895_SCRIPT_TGT_DISCONNECT;
            break;
	}

    /* Resume thread if it is still connected */

    switch (pScsiThread->state)
	{
	case SCSI_THREAD_INACTIVE	:
	case SCSI_THREAD_WAITING	:
	case SCSI_THREAD_DISCONNECTED	:
	    break;

	default:
    	    sym895ThreadUpdate (pThread);

	    if (sym895Resume (pSiop, pThread, entryPt) != OK)
	    	{
	   	SYM895_SCSI_DEBUG_MSG ("sym895ThreadEvent failed to \
                                       resume thread\n",
		    	               0, 0, 0, 0, 0, 0);
	
		sym895ThreadFail (pThread, S_scsiLib_DISCONNECTED);
	    	}
	    break;
	}
    }


/******************************************************************************
*
* sym895InitEvent - device initiator thread event processing routine
*
* Parse the event type and handle it accordingly.  This may result in state
* changes for the thread, state variables being updated, etc.
*
* RETURNS N/A
*/
LOCAL void sym895InitEvent
    (
    SYM895_THREAD * pThread,
    SYM895_EVENT  *  pEvent
    )
    {
    SCSI_EVENT *  pScsiEvent  = &pEvent->scsiEvent;
    SCSI_THREAD * pScsiThread = &pThread->scsiThread;
    SCSI_CTRL *   pScsiCtrl   = pScsiThread->pScsiCtrl;

    /* Update controller msg in/out state after script completes */

    pScsiCtrl->msgOutState = pThread->msgOutStatus;
    pScsiCtrl->msgInState  = pThread->msgInStatus;
    
    /*	Parse script exit status; handle as necessary */

    switch (pScsiEvent->type)
	{
	case SYM895_DISCONNECTED:

 	    SYM895_SCSI_DEBUG_MSG ("DISCONNECT message in\n", 0, 0, 0, 0, 0, 0);

	    scsiMgrThreadEvent (pScsiThread, SCSI_THREAD_EVENT_DISCONNECTED);
	    sym895ThreadStateSet (pThread, SCSI_THREAD_DISCONNECTED);
	    break;

	case SYM895_CMD_COMPLETE:

	    SYM895_SCSI_DEBUG_MSG ("COMMAND COMPLETE message in\n", 
                                   0, 0, 0, 0, 0, 0);

	    sym895ThreadComplete (pThread);
	    break;

	case SYM895_SELECTED:
	case SYM895_RESELECTED:
    
	    SYM895_SCSI_DEBUG_MSG ("sym895InitEvent thread 0x%08x \
                           (re)selection.\n", (int) pThread, 0, 0, 0, 0, 0);
 	    sym895ThreadDefer (pThread);
    	    break;

    	case SYM895_MESSAGE_OUT_SENT:

	    (void) scsiMsgOutComplete (pScsiCtrl, pScsiThread);
	    break;
	    
	case SYM895_MESSAGE_IN_RECVD:

	    (void) scsiMsgInComplete (pScsiCtrl, pScsiThread);
	    break;
   
	case SYM895_NO_MSG_OUT:

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
	    
	case SYM895_EXT_MESSAGE_SIZE:

            /*
     	     *  The SIOP has just read the length byte for an extended
             *  message in.  The shared memory area is updated with the
             *	appropriate length just before the thread is resumed (see
             *	"sym895ThreadUpdate()".
             */

             break;
	    
	case SYM895_PHASE_MISMATCH:

            if (sym895PhaseMismatch (pThread, pThread->busPhase,
				     pEvent->remCount) != OK)
		{
		sym895ThreadFail (pThread, errno);
		}  
	    	break;

	case SYM895_SCSI_TIMEOUT:

	    SYM895_SCSI_DEBUG_MSG ("sym895InitEvent thread 0x%08x \
                       select timeout.\n", (int) pThread, 0, 0, 0, 0, 0);
 	    sym895ThreadFail (pThread, S_scsiLib_SELECT_TIMEOUT);
  	    break;

        /* 
         * There is no error no. equivalent to this timeout. 
         * So S_scsiLib_SELECT_TIMEOUT is used.
         */

	case SYM895_HANDSHAKE_TIMEOUT:
	   
            SYM895_SCSI_DEBUG_MSG ("sym895InitEvent thread 0x%08x  \
                      Bus Handshake timeout.\n", (int) pThread, 0, 0, 0, 0, 0);
 	    sym895ThreadFail (pThread, S_scsiLib_SELECT_TIMEOUT);
  	    break;

	case SYM895_SCRIPT_ABORTED:

	    SYM895_SCSI_DEBUG_MSG ("sym895InitEvent thread 0x%08x aborted\n",
			    (int) pThread, 0, 0, 0, 0, 0);
	    break;

	case SYM895_NO_IDENTIFY:
	   
            SYM895_SCSI_DEBUG_MSG ("sym895InitEvent thread 0x%08x  \
                      No Identify message.\n", (int) pThread, 0, 0, 0, 0, 0);

            /* 
             * There should be an ErrorNumber for this event. Scsi2Lib.h
             * does'nt define any number for this. So use this for the moment.
             */

 	    sym895ThreadFail (pThread, S_scsiLib_SELECT_TIMEOUT);
  	    break;

	    
	case SCSI_EVENT_BUS_RESET:

    	    SYM895_SCSI_DEBUG_MSG ("sym895InitEvent thread 0x%08x bus reset\n",
		    	    (int) pThread, 0, 0, 0, 0, 0);

    	    /*	Do not try to resume this thread.  SCSI mgr will tidy up. */

            sym895ThreadStateSet (pThread, SCSI_THREAD_INACTIVE);
            break;

	case SYM895_UNEXPECTED_DISCON:

	    /* not really unexpected after an abort message ... */

    	    SYM895_SCSI_DEBUG_MSG ("sym895InitEvent thread 0x%08x \
                                   unexpected disconnection\n",
                                   (int) pThread, 0, 0, 0, 0, 0);

            sym895ThreadFail (pThread, S_scsiLib_DISCONNECTED);
            break;
	    
	case SYM895_ILLEGAL_PHASE:

            SYM895_SCSI_DEBUG_MSG ("sym895InitEvent thread 0x%08x illegal phase \
                            requested.\n", (int) pThread, 0, 0, 0, 0, 0);

            sym895ThreadFail (pThread, S_scsiLib_INVALID_PHASE);
	    break;

	default:
	    SCSI_MSG ("sym895InitEvent invalid event type (%d)\n",
		      pScsiEvent->type, 0, 0, 0, 0, 0);
  		break;
	}

    }


/******************************************************************************
*
* sym895Activate - activate a script corresponding to a new thread
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
* NOTE Interrupt locking is required to ensure that the correct action
* is taken once the controller state has been checked.
*
* RETURNS OK, or ERROR if the controller is in an invalid state (this
* indicates a major software failure).
*/

LOCAL STATUS sym895Activate
    (
    SIOP *          pSiop,     /* pointer to SIOP structure   */
    SYM895_THREAD * pThread    /* pointer to thread structure */
    )
    {
    STATUS status = OK;
    int key;

    if (pSiop->isCmdPending)
        {
	SCSI_MSG ("sym895Activate: activation request already pending !\n",
                  0, 0, 0, 0, 0, 0);
    	return (ERROR);
    	}

    /* 
     * Check Controller state, set a new command pending and signal it 
     * if necessary.
     */

    key = intLock ();

    /* 
     * If the SIOP is connected and it is in the passive state, means that an 
     * asynchronous bus event like a select or reselect just occurred. This 
     * event has priority over this thread, therefore change the state of
     * the controller to ACTIVE.
     */

    if ((pSiop->state == SYM895_STATE_PASSIVE) && 
	(SYM895_REG8_READ (pSiop,SYM895_OFF_ISTAT) & SYM895_ISTAT_CON))
        pSiop->state = SYM895_STATE_IDLE;

    switch (pSiop->state)
	{
	case SYM895_STATE_IDLE:
            status = OK;
  	    break;

	case SYM895_STATE_PASSIVE: 	/* do nothing */
	    pSiop->pNewThread = pThread;
	    pSiop->isCmdPending = TRUE;

            /* 
     	     * Signal the wait for (re) select script which then jumps to
	     * the relative alternate address. 
	     */

    	    SYM895_REG8_WRITE (pSiop, SYM895_OFF_ISTAT, 
                              (SYM895_REG8_READ (pSiop, SYM895_OFF_ISTAT) | 
                               SYM895_ISTAT_SIGP));
            status = OK;
            break;
	    
 	case SYM895_STATE_ACTIVE:
	default:
	    status = ERROR;
	    break;
	}


    intUnlock (key);

    if (status != OK)
	{
    	SCSI_MSG ("sym895Activate: invalid controller state. \n",
                 pSiop->state, 0, 0, 0, 0, 0);
    	errnoSet (S_scsiLib_SOFTWARE_ERROR);
    	}

    return (status);

    }



/******************************************************************************
*
* sym895Abort - abort the active script corresponding to the current thread
*
* Check that there is currently an active script running.  If so, set the
* SCSI Controller Abort flag which will halt the script and cause an interrupt.
*
* RETURNS N/A
*/

LOCAL void sym895Abort
    (
    SIOP * pSiop		/* pointer to controller structure */
    )
    {
    int    key;
    STATUS status;

    key = intLock ();

    switch (pSiop->state)
      {
      case SYM895_STATE_IDLE:
      case SYM895_STATE_PASSIVE:
           status = OK;
           break;
  
      case SYM895_STATE_ACTIVE:
           SYM895_REG8_WRITE(pSiop, SYM895_OFF_ISTAT, 
                             (SYM895_REG8_READ (pSiop,SYM895_OFF_ISTAT) | 
                             SYM895_ISTAT_ABRT));
           status = OK;
           break;

      default:
           status = ERROR;
           break;
      }
      
    intUnlock (key);

    if (status != OK)
      {
      SCSI_MSG("sym895Abort: thread = 0x%0xx: invalid state (%d) \n",
      			pSiop->state, 0, 0, 0, 0, 0);
      }

    }

/******************************************************************************
*
* sym895ScriptStart - start the SCSI Controller executing a script
*
* Restore the SCSI Controller register context, including the shared memory
* area, from the thread context.  Put the address of the script entry point
* into the DSP register.  If not in single-step mode, start the script.
*
* RETURNS N/A
*/

LOCAL void sym895ScriptStart
    (
    SIOP *            	pSiop,		/* pointer to  SCSI Controller info */
    SYM895_THREAD *	pThread,	/* SM895 thread info */
    SYM895_SCRIPT_ENTRY entryId		/* routine address entry point */
    )
    {

    int key;

    BOOL isManualMode;

    SYM895_SCSI_DEBUG_MSG ("sym895ScriptStart: %x    %d \n",
              (UINT32)pThread, (UINT32)entryId, 0, 0, 0, 0);

    key = intLock ();

    /* Set the Current Thread Pointer */

    pSiop->pCurThread = pThread;

    /* Restore the SCSI Controller  register context for this thread. */
    
    SYM895_REG8_WRITE (pSiop, SYM895_OFF_SCRATCHA0, pThread->nHostFlags);
    SYM895_REG8_WRITE (pSiop, SYM895_OFF_SCRATCHA1, pThread->msgOutStatus);
    SYM895_REG8_WRITE (pSiop, SYM895_OFF_SCRATCHA2, pThread->msgInStatus);
    SYM895_REG8_WRITE (pSiop, SYM895_OFF_SCRATCHA3, pThread->targetId); 
    SYM895_REG8_WRITE (pSiop, SYM895_OFF_SCRATCHA3, pThread->busPhase); 

    SYM895_REG8_WRITE (pSiop, SYM895_OFF_SXFER, pThread->sxfer);
    SYM895_REG8_WRITE (pSiop, SYM895_OFF_SCNTL3, pThread->scntl3);   

    /* 
     * Scatter Gather operations have to be initiated by Scsi Manager task.
     * As of now, this feature is only implemented, but not tested. The 
     * SCSI Scripts include BlockMOVE instructions to perform these operations.
     */           

    SYM895_REG8_WRITE (pSiop, SYM895_OFF_SCRATCHC0, pThread->isScatTransfer);	
    SYM895_REG8_WRITE (pSiop, SYM895_OFF_SCRATCHC1, pThread->totalScatElements);
    SYM895_REG8_WRITE (pSiop, SYM895_OFF_SCRATCHC2, pThread->noRemaining);
    
    /*
     * Set the shared data address, load the script start address,
     * then start the SCSI Controller unless it's in single-step mode.
     */

    SYM895_REG32_WRITE (pSiop, SYM895_OFF_DSA, 
		       (UINT32)SYM895_VIRT_TO_PHYS (pThread->pShMem));

    /* 
     * check if the SCRIPT DMA fetch is manual. If manual, scripts will not 
     * start until DMA start bit in DCNTL register is set
     */
		
    isManualMode = ((SYM895_REG8_READ (pSiop, SYM895_OFF_DMODE) & 
                     SYM895_DMODE_MAN) == SYM895_DMODE_MAN) ? TRUE : FALSE;	
 	
    SYM895_REG32_WRITE (pSiop, SYM895_OFF_DSP, 
		        (UINT32)SYM895_VIRT_TO_PHYS (
                        (UINT32)sym895ScriptEntry [entryId]));

    /* 
     * Check if Single stepping is enabled. Otherwise issue the START SCRIPTS
     * command by writing the DMA start bit in DCNTL register.
     */
		
    if (pSiop->isSingleStep)
	{
	SCSI_MSG ("sym895ScriptStart: Single Step required to start script.\n",
		 0, 0, 0, 0, 0, 0);
	}
    else
	{
        CACHE_PIPE_FLUSH(); 
         
	if (isManualMode)
	    {

	    /* Write the DMA Start bit */

            SYM895_REG8_WRITE (pSiop, SYM895_OFF_DCNTL, 
			       SYM895_REG8_READ (pSiop, SYM895_OFF_DCNTL) | 
                               SYM895_DCNTL_STD);

            /*
             * vxEieio() is a PPC family specific routine that makes sure that
             * the SCSI registers written above are executed in order and 
             * completely before continuing on. In the case of the faster PPC 
             * processors, it has been found that without this flushing of the 
             * pipes the processor is much faster than the SIOP, and causes 
             * erratic behaviour like bus errors and invalid events.
             */

            CACHE_PIPE_FLUSH();
            sym895Delay (); /* XXX - needs to be tuned better for fast PPCs */
	    CACHE_PIPE_FLUSH();
            }
	}

    intUnlock (key);

    CACHE_PIPE_FLUSH();
	
    }


/*******************************************************************************
*
* sym895ThreadUpdate - update the thread structure for a current SCSI command
*
* Update the dynamic data (e.g. data pointers, transfer parameters) in
* the thread to reflect the latest state of the corresponding physical device.
*
* RETURNS N/A
*
* NOMANUAL
*/

LOCAL void sym895ThreadUpdate
    (
    SYM895_THREAD * pThread		/* pointer thread structure */
    )
    {
    SCSI_THREAD *   pScsiThread = &pThread->scsiThread;
    SCSI_CTRL   *   pScsiCtrl   = pScsiThread->pScsiCtrl;
    SYM895_SHARED * pShMem      = pThread->pShMem; 
    UINT            msgOutSize;
    UINT            msgInSize;
    UINT            flags	= 0;
    SIOP  *         pSiop       = (SIOP *) pScsiCtrl;

#ifdef SCATTER_GATHER
    UINT count;
#endif

    /*
     *  If there is an identification message, ensure ATN is asserted
     *	during (re)selection.
     */

     if (pScsiThread->identMsgLength != 0)
         flags |= SYM895_FLAGS_IDENTIFY;

    /* update SIOP register context */

     pThread->nHostFlags = flags;
     pThread->msgOutStatus = pScsiCtrl->msgOutState;
     pThread->msgInStatus  = pScsiCtrl->msgInState;

    /* Update dynamic message in/out sizes */

    if (pScsiCtrl->msgOutState == SCSI_MSG_OUT_NONE)
	msgOutSize = 0;
    else
	msgOutSize = pScsiCtrl->msgOutLength;

    if (pScsiCtrl->msgInState != SCSI_MSG_IN_EXT_MSG_DATA)
	msgInSize = 0;
    
    else if ((msgInSize = pScsiCtrl->msgInBuf [SCSI_EXT_MSG_LENGTH_BYTE]) == 0)
	msgInSize = SCSI_EXT_MSG_MAX_LENGTH;

    /* Update commands in shared memory area */

    pShMem->command.size = SYM895_SWAP_32 (pScsiThread->cmdLength);
    pShMem->command.addr = (UINT8 *) SYM895_SWAP_32 
                                     (
                                       (UINT32) SYM895_VIRT_TO_PHYS
                                       ((UINT) pScsiThread->cmdAddress)
                                     );

#ifdef SCATTER_GATHER	/*  scatter gather memory support */

    
    /* 
     * For Scatter-Gather moves across SCSI Bus, a series of addresses and
     * byte counts from these addresses are needed. Addresses indicate from
     * where all in memory, data is to be transferred. Byte counts indicate
     * the amount of data to be transferred from individual addresses. These 
     * addresses and byte counts are assumed to be residing in contiguous 
     * memory. We assume support from SCSI Manager, to provide with the above
     * addresses & counts.Also, the structure elements "activeDataElements" is
     * assumed to be part of SCSI_THREAD structure (which is not as of now).
     */

    /* 
     * "activeDataElements" is assumed to be in SCSI_THREAD and is supposed 
     * to hold the number of scattered locations from where the data is to 
     * be transferred. Its responsibilty of SCSI Manager to initialise this 
     * once this is part of SCSI_THREAD.
     */

     pShMem->scatData.noElems = pScsiThread->activeDataElements;	 

    /* 
     * Now initialise the pointers from where data is to transferred.
     * These are set of addresses, which are assumed to contain the 
     * data locations. They do not exist now. Compiling this will
     * result in error. 
     */

    for (count = 0; count < pShMem->scatData.noElems; count++ )
        {
        pShMem->scatData.Elements[count].size=
                         (UINT8 *) SYM895_SWAP_32 
                         (
                         (UINT32) SYM895_VIRT_TO_PHYS
                         ((UINT)  pScsiThread->activeDataLength[count])
                         );

        pShMem->scatData.Elements[count].addr=
                         (UINT8 *) SYM895_SWAP_32 
                         (
                         (UINT32) SYM895_VIRT_TO_PHYS
                         ((UINT)  pScsiThread->activeDataAddress[count])
                         );
        }


    /* 
     * The way this feature was tested with out any support from SCSI lib...
     * For any data transfer across scsi BUS, the current thread structure
     * contains a pointer and count of the data to be transferred. To emulate
     * scattered operation, the data is split in to 2 parts. Now the byte
     * count will be half of the number as passed by library. The pointers to
     * the data can be obtained by adding the byte count to the pointer 
     * initially passed. In SCRIPTS assembly. when it comes to the DATA OUT
     * phase, check whether scattered data transfer is requested or not 
     * by checking the value passed in SCRATCHC0. If yes, the SCRIPTS will 
     * transfer data from various locations (though contiguous but not 
     * scattered).
     */  
#if 0
    pShMem->scatData.noElems = 2;
    pShMem->scatData.Elements[0].size = (SYM895_SWAP_32 
                                        (pScsiThread->activeDataLength))/2;

    pShMem->scatData.Elements[0].addr = (UINT8 *) SYM895_SWAP_32 
                                      (
                                       (UINT32) SYM895_VIRT_TO_PHYS
                                       ((UINT) pScsiThread->activeDataAddress)
                                      );

    pShMem->scatData.Elements[1].size = (SYM895_SWAP_32 
                                       (pScsiThread->activeDataLength))/2;
    pShMem->scatData.Elements[1].addr = (UINT8 *) SYM895_SWAP_32 
                                      (
                                       (UINT32) SYM895_VIRT_TO_PHYS
                                       ((UINT)(pScsiThread->activeDataAddress +
                                        (pScsiThread->activeDataLength)/2))
                                      );

#endif
  
    /* Tell scripts that now it is scattered data to be transferred.. */

    pThread->isScatTransfer = TRUE;
    pThread->totalScatElements = pShMem->scatData.noElems;
    pThread->noRemaining = pShMem->scatData.noElems;
 

#else


    pShMem->dataOut.size    = SYM895_SWAP_32 (pScsiThread->activeDataLength);
    pShMem->dataOut.addr    = (UINT8 *) SYM895_SWAP_32 
                                     (
                                       (UINT32) SYM895_VIRT_TO_PHYS
                                       ((UINT) pScsiThread->activeDataAddress)
                                     );

    pThread->isScatTransfer = FALSE;
    pThread->totalScatElements = 0;
    pThread->noRemaining = 0; 

#endif  /* SCATTER_GATHER */

    pShMem->dataIn.size    = SYM895_SWAP_32 (pScsiThread->activeDataLength);
    pShMem->dataIn.addr    = (UINT8 *) SYM895_SWAP_32 
                                     (
                                       (UINT32) SYM895_VIRT_TO_PHYS
                                       ((UINT) pScsiThread->activeDataAddress)
                                     );

    
    pShMem->status.size  = SYM895_SWAP_32 (pScsiThread->statusLength);
    pShMem->status.addr  = (UINT8 *) SYM895_SWAP_32
                                     (
                                       (UINT32) SYM895_VIRT_TO_PHYS
                                       ((UINT) pScsiThread->statusAddress)
                                     );

   /* 
    * The identOut pointer and buffer are initialised to the buffer allocated
    * in the pSiop structure. These used to be intialised to
    * pScsiThread->identMsg/identMsgLength. To suppport Wide/Sync. transfers
    * the MAX message length is increased. Ideally, this should have been taken
    * in Scsi2Lib.h where the MAX Message length is defined. 
    */

    pShMem->identOut.size = SYM895_SWAP_32 (pSiop->identMsgLength);
    pShMem->identOut.addr = (UINT8 *) SYM895_SWAP_32
                                      (
                                        (UINT32) SYM895_VIRT_TO_PHYS
                                        ((UINT) pSiop->identMsg)
                                      );

    pShMem->msgOut.size    =  SYM895_SWAP_32 (msgOutSize);
    pShMem->msgInRest.size =  SYM895_SWAP_32 (msgInSize);
                                                   
    }

/******************************************************************************
*
* sym895ThreadComplete - successfully complete execution of a client thread
*
* Set the thread status and errno appropriately, depending on whether or
* not the thread has been aborted.  Set the thread inactive, and notify
* the SCSI manager of the completion.
*
* RETURNS N/A
*/

LOCAL void sym895ThreadComplete
    (
    SYM895_THREAD * pThread
    )
    {
    SCSI_THREAD * pScsiThread = (SCSI_THREAD *) pThread;
    
    SYM895_SCSI_DEBUG_MSG ("sym895ThreadComplete thread 0x%08x completed\n",
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
    
    sym895ThreadStateSet (pThread, SCSI_THREAD_INACTIVE);

    scsiCacheSynchronize (pScsiThread, SCSI_CACHE_POST_COMMAND);

    scsiMgrThreadEvent (pScsiThread, SCSI_THREAD_EVENT_COMPLETED);
    }


/******************************************************************************
*
* sym895ThreadDefer - defer execution of a thread
*
* Set the thread's state to INACTIVE and notify the SCSI manager of the
* deferral event.
*
* This routine is invoked when a re-selection event occurs.
*
* RETURNS N/A
*/

LOCAL void sym895ThreadDefer
    (
    SYM895_THREAD * pThread
    )
    {
    SCSI_THREAD * pScsiThread = (SCSI_THREAD *) pThread;
    
    SYM895_SCSI_DEBUG_MSG ("sym895ThreadDefer thread 0x%08x deferred\n",
		    (int) pThread, 0, 0, 0, 0, 0);

    sym895ThreadStateSet (pThread, SCSI_THREAD_INACTIVE);

    scsiMgrThreadEvent (pScsiThread, SCSI_THREAD_EVENT_DEFERRED);
    }
    
	
/******************************************************************************
*
* sym895ThreadFail - complete execution of a thread, with error status
*
* Set the thread's status and errno according to the type of error.  Set
* the thread's state to INACTIVE, and notify the SCSI manager of the
* completion event.
*
* RETURNS N/A
*/

LOCAL void sym895ThreadFail
    (
    SYM895_THREAD * pThread,
    int             errNum
    )
    {
    SCSI_THREAD * pScsiThread = (SCSI_THREAD *) pThread;
    
    SYM895_SCSI_DEBUG_MSG ("sym895ThreadFail thread 0x%08x failed (errno = %d)\n",
		    (int) pThread, errNum, 0, 0, 0, 0);

    pScsiThread->status = ERROR;

    if (pScsiThread->state == SCSI_THREAD_ABORTING)
		pScsiThread->errNum = S_scsiLib_ABORTED;
    else
    	pScsiThread->errNum = errNum;
    
    sym895ThreadStateSet (pThread, SCSI_THREAD_INACTIVE);

    scsiMgrThreadEvent (pScsiThread, SCSI_THREAD_EVENT_COMPLETED);
    }
    

	
	
/******************************************************************************
*
* sym895ThreadStateSet - set the state of a thread
*
* This is really just a place-holder for debugging and possible future
* enhancements such as state-change logging.
*
* RETURNS N/A
*/

LOCAL void sym895ThreadStateSet
    (
    SYM895_THREAD *   pThread,		/* pointer to thread structure */
    SCSI_THREAD_STATE state
    )
    {
    SCSI_THREAD * pScsiThread = (SCSI_THREAD *) pThread;
    
    SYM895_SCSI_DEBUG_MSG ("sym895ThreadStateSet: thread 0x%08x %d -> %d\n",
	      (int) pThread, pScsiThread->state, state, 0, 0, 0);

    pScsiThread->state = state;
    }

/******************************************************************************
*
* sym895PhaseMismatch - recover from a SCSI bus phase mismatch
*
* This routine does whatever is required to keep the pointers, counts, etc.
* used by task-level software in step when a SCSI phase mismatch occurs.
*
* The interrupt-level mismatch processing has stored the phase of the
* information transfer before the mismatch, and the number of bytes
* remaining to be transferred.  See "sym895RemainderGet()".
*
* Note that the only phase mismatches supported at this level are
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
* 3) during a message in phase - presumably because ATN is asserted
* to abort or reject an incoming message.  No recovery is needed here -
* it's done by the thread management code, which should have enough
* state information to know what to do.
*
* RETURNS OK, or ERROR for an unsupported or invalid phase
*
* NOMANUAL
*/
LOCAL STATUS sym895PhaseMismatch
    (
    SYM895_THREAD * pThread,		/* pointer to thread structure  */
    int             phase,		/* bus phase before mismatch    */
    UINT            remCount		/* # bytes not yet transferred  */
    )
    {
    SCSI_THREAD * pScsiThread = (SCSI_THREAD *) pThread;
    UINT xferCount;
    
    switch (phase)
	{
	case SCSI_DATA_IN_PHASE:

            xferCount = SYM895_SWAP_32(pThread->pShMem->dataIn.size) - remCount;
	    pScsiThread->activeDataAddress += xferCount;
	    pScsiThread->activeDataLength  -= xferCount;
	    
	    SYM895_SCSI_DEBUG_MSG ("sym895PhaseMismatch data transfer aborted \
			           (%d bytes transferred).\n",
			           xferCount, 0, 0, 0, 0, 0);
	    break;

	case SCSI_DATA_OUT_PHASE:

            xferCount = SYM895_SWAP_32(pThread->pShMem->dataOut.size) - remCount;
	    pScsiThread->activeDataAddress += xferCount;
	    pScsiThread->activeDataLength  -= xferCount;
	    
	    SYM895_SCSI_DEBUG_MSG ("sym895PhaseMismatch data transfer aborted \
			           (%d bytes transferred).\n",
			           xferCount, 0, 0, 0, 0, 0);
	    break;
	    
	case SCSI_MSG_OUT_PHASE:
	
    	    SYM895_SCSI_DEBUG_MSG ("sym895PhaseMismatch message out aborted "
			           "(%d of %d bytes sent).\n",
		   		   pScsiThread->pScsiCtrl->msgOutLength,
				   pScsiThread->pScsiCtrl->msgOutLength - remCount,
			   	   0, 0, 0, 0);
	    break;
	    
	case SCSI_MSG_IN_PHASE:

            SYM895_SCSI_DEBUG_MSG("sym895PhaseMismatch message in aborted "
			          "(%d bytes received).\n",
			          pScsiThread->pScsiCtrl->msgInLength,
			          0, 0, 0, 0, 0);
            break;

	case SCSI_COMMAND_PHASE:
	case SCSI_STATUS_PHASE:

	    SYM895_SCSI_DEBUG_MSG ("sym895PhaseMismatch unsupported \
                                   phase (%d).\n", phase, 0, 0, 0, 0, 0);
	    return (ERROR);
	    
	default:

	    SCSI_MSG ("sym895PhaseMismatch invalid phase (%d).\n",
                      phase, 0, 0, 0, 0, 0);
            return (ERROR);
        }

    return (OK);

    }


/******************************************************************************
*
* sym895InitIdentEvent - identification thread event processing 
*
* Parse the event type and handle it accordingly.  This may result in state
* changes for the thread, state variables being updated, etc.
*
* RETURNS N/A
*/

LOCAL void sym895InitIdentEvent
    (
    SYM895_THREAD * pThread,
    SYM895_EVENT *  pEvent
    )
    {
    SCSI_EVENT *  pScsiEvent  = (SCSI_EVENT *)  pEvent;
    SCSI_THREAD * pScsiThread = (SCSI_THREAD *) pThread;
    SCSI_CTRL *   pScsiCtrl   = pScsiThread->pScsiCtrl;

    /* Update controller msg in/out state after script completes */

    pScsiCtrl->msgOutState = pThread->msgOutStatus;
    pScsiCtrl->msgInState  = pThread->msgInStatus;
    
    /* Parse script exit status; handle as necessary */

    switch (pScsiEvent->type)
	{
	case SYM895_RESELECTED:

            pScsiThread->nBytesIdent = pScsiEvent->nBytesIdent;

            bcopy ((char *) pScsiCtrl->identBuf,
                   (char *) pScsiThread->identMsg,
                   pScsiThread->nBytesIdent);

            sym895ThreadStateSet (pThread, SCSI_THREAD_IDENT_IN);

    	    sym895IdentInContinue (pThread);
	    break;

	case SYM895_MESSAGE_OUT_SENT:

            /*
             *	This will be after "ABORT (TAG)" msg has sent.
     	     *	The target will disconnect any time; it may have already
	     *	done so, in which case we won't be able to resume the
	     * 	thread, but no matter.
	     */

	    break;

	case SYM895_MESSAGE_IN_RECVD:

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
	   	   (char *) pScsiThread->identMsg + pScsiThread->nBytesIdent,2);
	    
    	    pScsiThread->nBytesIdent += 2;

    	    sym895IdentInContinue (pThread);
	    break;

	case SYM895_SCRIPT_ABORTED:

	    SYM895_SCSI_DEBUG_MSG ("sym895InitIdentEvent thread 0x%08x \
                                   aborted\n", (int) pThread, 0, 0, 0, 0, 0);
	    break;

        case SYM895_DISCONNECTED:

	    SYM895_SCSI_DEBUG_MSG ("sym895InitIdentEvent thread 0x%08x"
	         		   "disconnected\n",
			           (int) pThread, 0, 0, 0, 0, 0);

	    sym895ThreadFail (pThread, S_scsiLib_DISCONNECTED);
	    break;
   
	case SYM895_SCSI_BUS_RESET:

    	    SYM895_SCSI_DEBUG_MSG ("sym895InitIdentEvent thread 0x%08x bus \
                                   reset\n", (int) pThread, 0, 0, 0, 0, 0);

    	    /* Do not try to resume this thread.  SCSI mgr will tidy up.  */

	    sym895ThreadStateSet (pThread, SCSI_THREAD_INACTIVE);
	    break;

	case SYM895_UNEXPECTED_DISCON:

	    /* not really unexpected after an abort message ... */

    	    SYM895_SCSI_DEBUG_MSG ("sym895InitIdentEvent thread 0x%08x "
                      "unexpected disconnection\n",
                      (int) pThread, 0, 0, 0, 0, 0);

	    sym895ThreadFail (pThread, S_scsiLib_DISCONNECTED);
	    break;
	    
	case SYM895_ILLEGAL_PHASE:

	    SYM895_SCSI_DEBUG_MSG ("sym895InitIdentEvent thread 0x%08x "
                      "illegal phase requested.\n",
                      (int) pThread, 0, 0, 0, 0, 0);
	    
    	    sym895ThreadFail (pThread, S_scsiLib_INVALID_PHASE);
    	    break;

	default:

  	    SCSI_MSG ("sym895InitIdentEvent invalid event type (%d)\n",
	    pScsiEvent->type, 0, 0, 0, 0, 0);
	    break;
	}
  
  }

/******************************************************************************
*
* sym895TargIdentEvent - SYM 53C895 identification thread event processing 
*
* Parse the event type and handle it accordingly.  This may result in state
* changes for the thread, state variables being updated, etc.
*
* RETURNS: N/A
*
* NOMANUAL
*/
LOCAL void sym895TargIdentEvent
    (
    SYM895_THREAD * pThread,
    SYM895_EVENT *  pEvent
    )
    {
    SCSI_EVENT *  pScsiEvent  = (SCSI_EVENT *)  pEvent;
    SCSI_THREAD * pScsiThread = (SCSI_THREAD *) pThread;
    SCSI_CTRL *   pScsiCtrl   = pScsiThread->pScsiCtrl;

    /*
     *  Update controller msg in/out state after script completes
     */

    pScsiCtrl->msgOutState = pThread->msgOutStatus;
    pScsiCtrl->msgInState  = pThread->msgOutStatus;
    
    /*
     *	Parse script exit status; handle as necessary
     */

    switch (pScsiEvent->type)
	{
	case SYM895_SELECTED:

 	    pScsiThread->nBytesIdent = pScsiEvent->nBytesIdent;

	    bcopy ((char *) pScsiCtrl->identBuf,
		   (char *) pScsiThread->identMsg,
		   pScsiThread->nBytesIdent);

    	    sym895ThreadStateSet (pThread, SCSI_THREAD_IDENT_IN);
    	    break;
	    
        case SYM895_DISCONNECTED:

      	    SYM895_SCSI_DEBUG_MSG ("sym895TargIdentEvent: thread 0x%08x:"
		       	    " disconnected\n",
			    (int) pThread, 0, 0, 0, 0, 0);

  	    sym895ThreadStateSet (pThread, SCSI_THREAD_INACTIVE);
		break;
	    
	case SYM895_SCSI_BUS_RESET:
 
     	    SYM895_SCSI_DEBUG_MSG ("sym895TargIdentEvent: thread 0x%08x: \
                                   bus reset\n",(int) pThread, 0, 0, 0, 0, 0);
	    
            /* Do not try to resume this thread.  SCSI mgr will tidy up. */

	    sym895ThreadStateSet (pThread, SCSI_THREAD_INACTIVE);
	    break;

	case SYM895_UNEXPECTED_DISCON:

    	    SYM895_SCSI_DEBUG_MSG ("sym895TargIdentEvent: thread 0x%08x: "
		    	    "unexpected disconnection\n",
		    	    (int) pThread, 0, 0, 0, 0, 0);

	    sym895ThreadStateSet (pThread, SCSI_THREAD_INACTIVE);
	    break;
	    
	default:

 	    SCSI_MSG ("sym895TargIdentEvent: invalid event type (%d)\n",
		     pScsiEvent->type, 0, 0, 0, 0, 0);
	    break;
	}


    }

    
/******************************************************************************
*
* sym895Resume - resume a script corresponding to a suspended thread
*
* NOTE the script can only be resumed if the controller is currently idle.
* To avoid races, interrupts must be locked while this is checked and the
* script re-started.
*
* Reasons why the controller might not be idle include SCSI bus reset and
* unexpected disconnection, both of which might occur in practice.  Hence
* this is not considered to be a major software error.
*
* RETURNS OK, or ERROR if the controller is in an invalid state (this
* should not be treated as a major software failure).
*/

LOCAL STATUS sym895Resume
    (
    SIOP *           pSiop,	/* pointer to controller structure */
    SYM895_THREAD *  pThread,	/* pointer to thread structure     */
    SYM895_SCRIPT_ENTRY entryId	/* entry point of script to resume */
    )
    {
    STATUS status;
    int    key;

    /* Check validity of connection and start script if OK */

    key = intLock ();

    switch (pSiop->state)
	{
	case SYM895_STATE_IDLE:

	    SYM895_SCSI_DEBUG_MSG ("sym895Resume thread 0x%08x"
				   " state %d -> %d\n",
			      	   (int) pThread,
			    	   SYM895_STATE_IDLE, SYM895_STATE_ACTIVE,
			    	   0, 0, 0);

 	    sym895ScriptStart (pSiop, pThread, entryId);

	    pSiop->state = SYM895_STATE_ACTIVE;
	    status = OK;
 	    break;

	case SYM895_STATE_PASSIVE:
	case SYM895_STATE_ACTIVE:
	default:
 	    status = ERROR;
	    break;
	}

    intUnlock (key);

    return (status);
    }

/******************************************************************************
*
* sym895IdentInContinue - continue incoming identification
*
* Parse the message built up so far.  If it is not yet complete, do nothing.
* If the message is complete, attempt to reconnect the thread it identifies,
* and deactivate this thread (the identification thread is no longer active).
* Otherwise (identification has failed), abort the identification sequence.
*
* RETURNS N/A
*/

LOCAL void sym895IdentInContinue
    (
    SYM895_THREAD * pThread
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
                 sym895ThreadReconnect ((SYM895_THREAD *) pNewThread);

                 state = SCSI_THREAD_INACTIVE;
                 }
             break;

	case SCSI_IDENT_FAILED:
             state = SCSI_THREAD_IDENT_ABORTING;
 	     break;

	default:
    	     SCSI_MSG ("sym895IdentInContinue invalid ident status (%d)\n",
    	               status, 0, 0, 0, 0, 0);
    	     state = SCSI_THREAD_INACTIVE;
	     break;
	}

    if (state == SCSI_THREAD_IDENT_ABORTING)
	sym895ThreadAbort ((SIOP *) pScsiCtrl, pThread);

    sym895ThreadStateSet (pThread, state);

    }


/******************************************************************************
*
* sym895ThreadReconnect - reconnect a thread
*
* Restore the SCSI pointers for the thread (this really should be in a more
* generic section of code - perhaps part of the SCSI manager's thread event
* procesing ?).  Update the newly-connected thread's context (including
* shared memory area) and resume it.  Set the thread's state to ESTABLISHED.
*
* RETURNS N/A
*
* NOMANUAL
*/
LOCAL void sym895ThreadReconnect
    (
    SYM895_THREAD * pThread
    )
    {
    SCSI_THREAD * pScsiThread = (SCSI_THREAD *) pThread;
    SCSI_CTRL   * pScsiCtrl   = pScsiThread->pScsiCtrl;
    SIOP        * pSiop       = (SIOP *) pScsiCtrl;
    
    SYM895_SCSI_DEBUG_MSG ("sym895ThreadReconnect reconnecting thread 0x%08x\n",
                           (int) pThread, 0, 0, 0, 0, 0);

    pScsiCtrl->pThread = pScsiThread;

    /* Implied RESTORE POINTERS action see "scsiMsgInComplete ()" */

    pScsiThread->activeDataAddress = pScsiThread->savedDataAddress;
    pScsiThread->activeDataLength  = pScsiThread->savedDataLength;

    sym895ThreadUpdate (pThread);

    if (sym895Resume (pSiop, pThread, SYM895_SCRIPT_INIT_CONTINUE) != OK)
        {
	SYM895_SCSI_DEBUG_MSG ("sym895ThreadReconnect failed to resume thread.\n",
                  0, 0, 0, 0, 0, 0);

	sym895ThreadFail (pThread, S_scsiLib_DISCONNECTED);
	return;
	}

    sym895ThreadStateSet (pThread, SCSI_THREAD_ESTABLISHED);
    }

/******************************************************************************
*
* sym895StepEnable - Enable/disable script single step 
*
* Enable/disable single step facility on SYM895 chip.  Also, unmask/mask single 
* step interrupt in Dien register. Before any SCSI execution you just have to 
* use sym895StepEnable (pSiop, TRUE) to run in single step mode.  
*
* To run a script step use sym895SingleStep(pSiop).
* To disable use sym895StepEnable (pSiop, FALSE).
* 
* RETURNS: N/A
*
* NOMANUAL
*/

void sym895StepEnable 
    (
    SIOP * pSiop,		/* pointer to SIOP info       */
    BOOL   enable		/* TRUE => enable single-step */
    )

    {
    if (enable)
	{

	/* enable single-step interrupt */

	SYM895_REG8_WRITE (pSiop, SYM895_OFF_DIEN, 
                           (SYM895_REG8_READ (pSiop, SYM895_OFF_DIEN) | 
                           SYM895_B_SSI));

	/* set single-step mode */

	SYM895_REG8_WRITE (pSiop, SYM895_OFF_DCNTL, 
                           (SYM895_REG8_READ (pSiop, SYM895_OFF_DCNTL) | 
                           SYM895_DCNTL_SSM));

	/* disable manual Start */

	SYM895_REG8_WRITE (pSiop, SYM895_OFF_DMODE, 
                           (SYM895_REG8_READ (pSiop, SYM895_OFF_DMODE) & 
                           ~SYM895_DMODE_MAN)); 
	}
    else 
	{	

	/* unset single-step mode */

	SYM895_REG8_WRITE (pSiop, SYM895_OFF_DCNTL, 
                           (SYM895_REG8_READ (pSiop, SYM895_OFF_DCNTL) & 
                           ~SYM895_DCNTL_SSM));

	/* disable single-step interrupt */

	SYM895_REG8_WRITE (pSiop, SYM895_OFF_DIEN, 
                           (SYM895_REG8_READ (pSiop, SYM895_OFF_DIEN) & 
                           ~SYM895_B_SSI));

	/* enable manual Start */

	SYM895_REG8_WRITE (pSiop, SYM895_OFF_DMODE, 
                           (SYM895_REG8_READ (pSiop, SYM895_OFF_DMODE) | 
                           SYM895_DMODE_MAN));
     	}

    pSiop->isSingleStep = enable;

    }


/******************************************************************************
*
* sym895SingleStep - Perform a single step 
*
* Perform a single step by writing the STD bit in the DCNTL register.
* The parameter is a pointer to the SIOP information. 
* 
* RETURNS: N/A
* 
* NOMANUAL
*/

void sym895SingleStep
    (
    SIOP * pSiop, 		/* pointer to SIOP info */ 
    BOOL   verbose		/* show all registers   */
    )
    {

    UINT * siopPc;

    /*
     *	Start the SIOP: if not in single-step mode it will then continue
     *	the normal instruction sequence.  If in single-step mode, wait
     *	until one instruction has been executed.
     */

    SYM895_REG8_WRITE (pSiop, SYM895_OFF_DCNTL, 
                       (SYM895_REG8_READ (pSiop, SYM895_OFF_DCNTL) |
		       SYM895_DCNTL_STD));

    if (!pSiop->isSingleStep)
	return;

    semTake (pSiop->singleStepSem, WAIT_FOREVER);

    /*
     *	Allow any tasks using SCSI to get a look-in - they may change the
     *	controller state, in which case we'd like to see that change now
     *	rather than next time.
     */

    taskDelay (sysClkRateGet()/ 20); /* this needs to be tuned */

    /* Show what the SIOP is about to do */ 

    if (pSiop->state == SYM895_STATE_IDLE)
	SCSI_MSG("SYM895 is idle.\n\n", 0, 0, 0, 0, 0, 0);
    else
	{
	siopPc = (UINT *)SYM895_REG32_READ (pSiop, SYM895_OFF_DSP);

 	/* mask PCI's view of memory */ 

	siopPc = (UINT *)((UINT) siopPc & 0x7fffffff);

	printf ("SYM895 [%d] Next Opcode (0x%08x) => 0x%08x 0x%08x\n\n",
	pSiop->state, (int) siopPc, SYM895_SWAP_32(siopPc[0]), 
				    SYM895_SWAP_32(siopPc[1]) );
	}
    
    /* Dump the SIOP registers, if in verbose mode */

    if (verbose)
    	sym895Show (pSiop);
    }


/******************************************************************************
*
* sym895Show - display values of all readable SYM 53C8xx SIOP registers.
*
* This routine displays the state of the SIOP registers in a user-friendly way.
* It is useful primarily for debugging. The input parameter is the pointer to 
* the SIOP information structure returned by the sym895CtrlCreate() call.
*
* NOTE
* The only readable register during a script execution is the Istat register. 
* If you use this routine during the execution of a SCSI command, the result 
* could be unpredictable.
*
* EXAMPLE:
* .CS
* -> sym895Show
*SYM895 Registers 
*---------------- 
* Scntl0   = 0xd0 Scntl1   = 0x00 Scntl2   = 0x00 Scntl3   = 0x00
* Scid     = 0x67 Sxfer    = 0x00 Sdid     = 0x00 Gpreg    = 0x0f
* Sfbr     = 0x0f Socl     = 0x00 Ssid     = 0x00 Sbcl     = 0x00
* Dstat    = 0x80 Sstat0   = 0x00 Sstat1   = 0x0f Sstat2   = 0x02
* Dsa      = 0x07ea9538
* Istat    = 0x00
* Ctest0   = 0x00 Ctest1   = 0xf0 Ctest2   = 0x35 Ctest3   = 0x10
* Temp     = 0x001d0c54
* Dfifo    = 0x00
* Dbc0:23-Dcmd24:31  = 0x54000000
* Dnad     = 0x001d0c5c
* Dsp      = 0x001d0c5c
* Dsps     = 0x000000a0
* Scratch0  = 0x01 Scratch1  = 0x00 Scratch2  = 0x00 Scratch3  = 0x00
* Dmode    = 0x81 Dien     = 0x35 Dwt      = 0x00 Dcntl    = 0x01
* Sien0    = 0x0f Sien1    = 0x17 Sist0    = 0x00 Sist1    = 0x00
* Slpar    = 0x4c Swide    = 0x00 Macntl   = 0xd0 Gpcntl   = 0x0f
* Stime0   = 0x00 Stime1   = 0x00 Respid0  = 0x80 Respid1  = 0x00 
* Stest0   = 0x07 Stest1   = 0x00 Stest2   = 0x00 Stest3   = 0x80
* Sidl     = 0x0000 Sodl     = 0x0000 Sbdl     = 0x0000
* Scratchb = 0x00000200
* value = 0 = 0x0
* .CE
*
* RETURNS: OK, or ERROR if <pScsiCtrl> and <pSysScsiCtrl> are both NULL.
*
* SEE ALSO: sym895CtrlCreate()
*/

STATUS sym895Show 
    (
    SIOP * pSiop	 /* pointer to SCSI controller */
    )
    {
   
    if (pSiop == NULL)
	{
	SCSI_MSG (" Invalid SCSI Controller Info", 0, 0, 0, 0, 0, 0);
	return (ERROR);
	}

    printf ("SYM895 Registers \n");
    printf ("---------------- \n");
    printf ("Scntl0   = 0x%02x ",  SYM895_REG8_READ (pSiop, SYM895_OFF_SCNTL0)); 
    printf ("Scntl1   = 0x%02x ",  SYM895_REG8_READ (pSiop, SYM895_OFF_SCNTL1)); 
    printf ("Scntl2   = 0x%02x ",  SYM895_REG8_READ (pSiop, SYM895_OFF_SCNTL2)); 
    printf ("Scntl3   = 0x%02x\n", SYM895_REG8_READ (pSiop, SYM895_OFF_SCNTL3)); 

    printf ("Scid     = 0x%02x ",  SYM895_REG8_READ (pSiop, SYM895_OFF_SCID)); 
    printf ("Sxfer    = 0x%02x ",  SYM895_REG8_READ (pSiop, SYM895_OFF_SXFER)); 
    printf ("Sdid     = 0x%02x ",  SYM895_REG8_READ (pSiop, SYM895_OFF_SDID)); 
    printf ("Gpreg    = 0x%02x\n", SYM895_REG8_READ (pSiop, SYM895_OFF_GPREG)); 
    printf ("Sfbr     = 0x%02x ",  SYM895_REG8_READ (pSiop, SYM895_OFF_SFBR)); 
    printf ("Socl     = 0x%02x ",  SYM895_REG8_READ (pSiop, SYM895_OFF_SOCL)); 
    printf ("Ssid     = 0x%02x ",  SYM895_REG8_READ (pSiop, SYM895_OFF_SSID)); 
    printf ("Sbcl     = 0x%02x\n", SYM895_REG8_READ (pSiop, SYM895_OFF_SBCL)); 

    /* Reading this register clears the interrupts */

    printf ("Dstat    = 0x%02x ",  SYM895_REG8_READ (pSiop, SYM895_OFF_DSTAT)); 

    printf ("Sstat0   = 0x%02x ",  SYM895_REG8_READ (pSiop, SYM895_OFF_SSTAT0)); 
    printf ("Sstat1   = 0x%02x ",  SYM895_REG8_READ (pSiop, SYM895_OFF_SSTAT1)); 
    printf ("Sstat2   = 0x%02x\n", SYM895_REG8_READ (pSiop, SYM895_OFF_SSTAT2)); 

    printf ("Dsa      = 0x%08x\n", (UINT32)SYM895_REG32_READ (pSiop,  
                                                              SYM895_OFF_DSA)); 

    printf ("Istat    = 0x%02x\n", SYM895_REG8_READ (pSiop, SYM895_OFF_ISTAT));

    printf ("Ctest0   = 0x%02x ",  SYM895_REG8_READ (pSiop, SYM895_OFF_CTEST0));
    printf ("Ctest1   = 0x%02x ",  SYM895_REG8_READ (pSiop, SYM895_OFF_CTEST1));
    printf ("Ctest2   = 0x%02x ",  SYM895_REG8_READ (pSiop, SYM895_OFF_CTEST2));
    printf ("Ctest3   = 0x%02x\n", SYM895_REG8_READ (pSiop, SYM895_OFF_CTEST3));
    
    printf ("Temp     = 0x%08x\n", (UINT32)SYM895_REG32_READ (pSiop, 
                                                              SYM895_OFF_TEMP)); 

    printf ("Dfifo    = 0x%02x\n", SYM895_REG8_READ(pSiop, SYM895_OFF_DFIFO)); 

    printf ("Dbc0:23-Dcmd24:31  = 0x%08x\n", 
            (UINT32)SYM895_REG32_READ (pSiop, SYM895_OFF_DBC)); 

    printf ("Dnad     = 0x%08x\n", 
            (UINT32)SYM895_REG32_READ (pSiop, SYM895_OFF_DNAD)); 

    printf ("Dsp      = 0x%08x\n", 
            (UINT32)SYM895_REG32_READ (pSiop, SYM895_OFF_DSP)); 

    printf ("Dsps     = 0x%08x\n", 
            (UINT32)SYM895_REG32_READ (pSiop, SYM895_OFF_DSPS)); 

    printf ("Scratch0  = 0x%02x ",  SYM895_REG8_READ (pSiop, 
                                                      SYM895_OFF_SCRATCHA0)); 
    printf ("Scratch1  = 0x%02x ",  SYM895_REG8_READ (pSiop, 
                                                      SYM895_OFF_SCRATCHA1)); 
    printf ("Scratch2  = 0x%02x ",  SYM895_REG8_READ (pSiop, 
                                                      SYM895_OFF_SCRATCHA2)); 
    printf ("Scratch3  = 0x%02x\n", SYM895_REG8_READ (pSiop, 
                                                      SYM895_OFF_SCRATCHA3)); 

    printf ("Dmode    = 0x%02x ",  SYM895_REG8_READ (pSiop, SYM895_OFF_DMODE));
    printf ("Dien     = 0x%02x ",  SYM895_REG8_READ (pSiop, SYM895_OFF_DIEN));
    printf ("Dwt      = 0x%02x ",  SYM895_REG8_READ (pSiop, SYM895_OFF_DWT));
    printf ("Dcntl    = 0x%02x\n", SYM895_REG8_READ (pSiop, SYM895_OFF_DCNTL));

    printf ("Sien0    = 0x%02x ", SYM895_REG8_READ (pSiop, SYM895_OFF_SIEN0)); 
    printf ("Sien1    = 0x%02x ", SYM895_REG8_READ (pSiop, SYM895_OFF_SIEN1)); 

    /* Reading these registers clears the interrupts */

    printf ("Sist0    = 0x%02x ", SYM895_REG8_READ (pSiop, SYM895_OFF_SIST0)); 
    printf ("Sist1    = 0x%02x",  SYM895_REG8_READ (pSiop, SYM895_OFF_SIST1)); 

    printf ("\n");
    printf ("Slpar    = 0x%02x ", SYM895_REG8_READ (pSiop, SYM895_OFF_SLPAR)); 

    printf ("Swide    = 0x%02x ", SYM895_REG8_READ (pSiop, SYM895_OFF_SWIDE)); 

    printf ("Macntl   = 0x%02x ",  SYM895_REG8_READ (pSiop, SYM895_OFF_MACNTL)); 
    printf ("Gpcntl   = 0x%02x\n", SYM895_REG8_READ (pSiop, SYM895_OFF_GPCNTL)); 

    printf ("Stime0   = 0x%02x ", SYM895_REG8_READ (pSiop, SYM895_OFF_STIME0)); 
    printf ("Stime1   = 0x%02x ", SYM895_REG8_READ (pSiop, SYM895_OFF_STIME1)); 

    printf ("Respid0  = 0x%02x ", SYM895_REG8_READ (pSiop, SYM895_OFF_RESPID0)); 
    printf ("Respid1  = 0x%02x ", SYM895_REG8_READ (pSiop, SYM895_OFF_RESPID1)); 

    printf ("\n");

    printf ("Stest0   = 0x%02x ",  SYM895_REG8_READ (pSiop, SYM895_OFF_STEST0)); 
    printf ("Stest1   = 0x%02x ",  SYM895_REG8_READ (pSiop, SYM895_OFF_STEST1)); 
    printf ("Stest2   = 0x%02x ",  SYM895_REG8_READ (pSiop, SYM895_OFF_STEST2)); 
    printf ("Stest3   = 0x%02x\n", SYM895_REG8_READ (pSiop, SYM895_OFF_STEST3)); 
    printf ("Sidl     = 0x%04x ",  SYM895_REG16_READ (pSiop, SYM895_OFF_SIDL)); 
    printf ("Sodl   = 0x%04x ",  SYM895_REG16_READ (pSiop, SYM895_OFF_SODL)); 
    printf ("Sbdl   = 0x%04x\n", SYM895_REG16_READ (pSiop, SYM895_OFF_SBDL)); 

    printf ("Scratchb = 0x%08x\n", (UINT32)SYM895_REG32_READ (pSiop, 
                                                      SYM895_OFF_SCRATCHB0)); 
    printf ("Scratchc = 0x%08x\n", (UINT32)SYM895_REG32_READ (pSiop, 
                                                      SYM895_OFF_SCRATCHC0));
    printf ("Scratchd = 0x%08x\n", (UINT32)SYM895_REG32_READ (pSiop, 
                                                      SYM895_OFF_SCRATCHD));
    return (OK);
    }




/*******************************************************************************
*
* sym895Delay - Delays the execution for a predefined period;
*
* This delay is required for successive reads from the SCSI and DMA status
* registers. The data manual recommends a delay of 12 SCLK between such 
* successive reads. We are trying to emulate the 12 SCLKs by this routine
*
* NOMANUAL
*/

LOCAL void sym895Delay()
    {
    volatile int i = 0;
	
    for (i=0; i<sym895DelayCount; i++)
        ;

    }

/*******************************************************************************
*
* sym895GPIOConfig - configures general purpose pins GPIO 0-4.
*
* This routine uses the GPCNTL register to configure the general purpose pins 
* available on Sym895 chip.  Bits 0-4 of GPCNTL register map to GPIO 0-4 pins. 
* A bit set in GPCNTL configures corresponding pin as input and a bit reset 
* configures the pins as output.
*
* .IP <pSiop>
*  pointer to the SIOP structure.
*
* .IP <ioEnable>
*  bits 0-4 of this parameter configure GPIO 0-4 pins.  1 => input, 0 => output.   
*
* .IP <mask>
*  bits 0-4 of this parameter identify valid bits in <ioEnable> parameter. Only
*  those pins are configured, which have a corresonding bit set in this parameter. 
*/

STATUS sym895GPIOConfig
    (
    SIOP * pSiop,      /* pointer to SIOP structure   */
    UINT8 ioEnable,    /* bits indicate input/output  */
    UINT8 mask         /* mask for ioEnable parameter */
    )
    {

    if (pSiop == NULL)
	{
	SCSI_MSG (" Invalid SCSI Controller Info", 0, 0, 0, 0, 0, 0);
	return (ERROR);
	}

    SYM895_REG8_WRITE (pSiop, SYM895_OFF_GPCNTL, 
                       ((SYM895_REG8_READ(pSiop,SYM895_OFF_GPCNTL) & ~mask) | 
                        ioEnable)); 
    return (OK);
    }

/*******************************************************************************
*
* sym895GPIOCtrl - controls general purpose pins GPIO 0-4.
*
* This routine uses the GPREG register to set/reset of the general purpose 
* pins available on Sym895 chip.
*
* .IP <pSiop>
*  pointer to the SIOP structure.
*
* .IP <ioState>
*  bits 0-4 of this parameter controls GPIO 0-4 pins.  1 => set, 0 => reset.   
*
* .IP <mask>
*  bits 0-4 of this parameter identify valid bits in <ioState> parameter. Only
*  those pins are activated, which have a corresonding bit set in this parameter. 
*/

STATUS sym895GPIOCtrl
    (
    SIOP * pSiop,      /* pointer to SIOP structure   */
    UINT8  ioState,    /* bits indicate set/reset */
    UINT8  mask        /* mask for ioState parameter */
    )
    {

    if (pSiop == NULL)
	{
	SCSI_MSG (" Invalid SCSI Controller Info", 0, 0, 0, 0, 0, 0);
	return (ERROR);
	}

    SYM895_REG8_WRITE(pSiop, SYM895_OFF_GPREG, 
                      ((SYM895_REG8_READ(pSiop, SYM895_OFF_GPREG) & ~mask) | 
                                         ioState));
    return (OK);    
    }

/******************************************************************************
*
* sym895Loopback - This routine performs loopback diagnotics on 895 chip.
*
* Loopback mode allows 895 chip to control all signals, regardless of whether
* it is in initiator or target role. This mode insures proper SCRIPTS instruction
* fetches and data paths.  SYM895 executes initiator instructions through the
* SCRIPTS, and this routine implements the target role by asserting and polling
* the appropriate SCSI signals in the SOCL, SODL, SBCL, and SBDL registers.
* 
* To configure 895 in loopback mode,
*
* (1) Bits 3 and 4 of STEST2 should be set to put SCSI pins in High-Impedance 
*     mode, so that signals are not asserted on to the SCSI bus.
*
* (2) Bit 4 of DCNTL should be set to turn on single step mode.  This allows the
*     target program (this routine) to monitor when an initiator SCRIPTS   
*     instruction has completed.
*
* In this routine, SELECTION, MSG_OUT and DATA_OUT phases are checked. This will  
* ensure that data and control paths are proper.
* 
*/

STATUS sym895Loopback
    (
    SIOP * pSiop   /* pointer to SIOP controller structure */
    )
    {
 
    UINT8  stest2;  /* store for existing STEST2 value */ 
    UINT8  dcntl ;  
    UINT8  sien0 ;  
    UINT8  sien1 ;
    UINT8  dien  ;
    UINT8  temp;
    int    msgInData;
    UINT8  msgBuf   = LOOPBACK_MSG_BYTE;
    UINT16 dataBuf  = LOOPBACK_DATA;
    STATUS retVal   = ERROR;    

    SYM895_LOOPBACK  loopback;

    SCSI_CTRL *  pScsiCtrl = &(pSiop->scsiCtrl);
 
    if (pSiop == NULL)
	{
	SCSI_MSG ("Invalid SCSI Controller Info", 0, 0, 0, 0, 0, 0);
	return (ERROR);
	}

    /* 
     * Initialise the addresss space from where the "sym895Diag" Scripts  
     * fetches the table indirect information. This scripts routine selects
     * 895 in loopback mode and then sends out IDENTIFY message.
     */

    loopback.device = SYM895_SWAP_32(
                      (SYM895_REG8_READ (pSiop, SYM895_OFF_SCNTL3) << 24) | 
                      ((SYM895_REG8_READ (pSiop, SYM895_OFF_SCID) 
                       & SYM895_SCID_ENC_MASK) << 16) |
                      (SYM895_REG8_READ (pSiop, SYM895_OFF_SXFER) << 8));  

    loopback.identOut.size = SYM895_SWAP_32(0x01);
    loopback.identOut.addr = (UINT8 *) SYM895_SWAP_32
                             ((UINT32)SYM895_VIRT_TO_PHYS(&msgBuf));

    loopback.dataOut.size = SYM895_SWAP_32(0x02);
    loopback.dataOut.addr = (UINT8 *) SYM895_SWAP_32
                             ((UINT32)SYM895_VIRT_TO_PHYS(&dataBuf));

    stest2 = SYM895_REG8_READ (pSiop, SYM895_OFF_STEST2);
    dcntl  = SYM895_REG8_READ (pSiop, SYM895_OFF_DCNTL);
    sien0  = SYM895_REG8_READ (pSiop, SYM895_OFF_SIEN0);
    sien1  = SYM895_REG8_READ (pSiop, SYM895_OFF_SIEN1);
    dien   = SYM895_REG8_READ (pSiop, SYM895_OFF_DIEN);
    
    /* Configure 895 chip in loop back mode */
   
    /* 
     * First, disable all interrupts. DSTAT & SIST0,1 are polled as and when
     * needed determine the possible event. This insures that no interrupts
     * are routed to the sym895 handler and no events get notified to scsi 
     * manager.
     */ 
      
    SYM895_REG8_WRITE (pSiop, SYM895_OFF_DIEN,  0x00);	
    SYM895_REG8_WRITE (pSiop, SYM895_OFF_SIEN0, 0x00);	    
    SYM895_REG8_WRITE (pSiop, SYM895_OFF_SIEN1, 0x00);	

    /* 
     * Abort any SCRIPT currently running, to run diagnostic SCRIPTS
     * If the diagnostic SCRIPTS are run once on POR before other scripts are 
     * run, then aborting is not needed.
     */

    SYM895_REG8_WRITE (pSiop, SYM895_OFF_ISTAT, 
                       SYM895_REG8_READ (pSiop, SYM895_OFF_ISTAT) | 
                                         SYM895_ISTAT_ABRT);

    while ((SYM895_REG8_READ (pSiop, SYM895_OFF_ISTAT) & 
                              (SYM895_ISTAT_SIP | SYM895_ISTAT_DIP)) == 0)
        ;

    if (SYM895_REG8_READ (pSiop, SYM895_OFF_ISTAT) & (SYM895_ISTAT_DIP) )
        SYM895_REG8_WRITE (pSiop, SYM895_OFF_ISTAT,0x00);

    temp = SYM895_REG8_READ (pSiop, SYM895_OFF_DSTAT); 
 
    /* Set the STEST2 register loopback mode bits. */
  
    SYM895_REG8_WRITE (pSiop, SYM895_OFF_STEST2, 
                       SYM895_REG8_READ (pSiop, SYM895_OFF_STEST2) |
                       (SYM895_STEST2_SCE | SYM895_STEST2_SLB | 
                        SYM895_STEST2_SZM));	

    /* Set Sym895 in Single-Step Mode */ 
  
    SYM895_REG8_WRITE (pSiop, SYM895_OFF_DCNTL, 
                       SYM895_REG8_READ (pSiop, SYM895_OFF_DCNTL) |
                                         SYM895_DCNTL_SSM);     

    /* disable manual Start */

    SYM895_REG8_WRITE (pSiop, SYM895_OFF_DMODE, 
                       (SYM895_REG8_READ (pSiop, SYM895_OFF_DMODE) & 
                                          ~SYM895_DMODE_MAN)); 

    /* 
     * By now, the SCSI pins are in High-Impedance mode. Start with SELECTION
     * in loopback mode. This is initiated by a SCRIPT instruction.SYM895 chip 
     * selects itself. The target functionality is implemented here.
     */ 

    /*
     * Set the shared data address, load the script start address,
     * then start the SCSI Controller unless it's in single-step mode.
     */

    SYM895_REG32_WRITE (pSiop, SYM895_OFF_DSA, 
                        (UINT32)SYM895_VIRT_TO_PHYS (&loopback));

    SYM895_REG32_WRITE (pSiop, SYM895_OFF_DSP, 
		        (UINT32)SYM895_VIRT_TO_PHYS (
                        (UINT32)sym895ScriptEntry [SYM895_SCRIPT_DIAG]));

    SYM895_REG8_WRITE (pSiop, SYM895_OFF_DCNTL, 
                       (SYM895_REG8_READ (pSiop, SYM895_OFF_DCNTL) |
		       SYM895_DCNTL_STD));

    /* 
     * Wait till the above SCRIPTS instruction is fetched, and host bus is free. 
     * This delay is system-dependent because of chip clock speed, host
     * arbitration times.
     */            
    
    CACHE_PIPE_FLUSH();
    taskDelay (sysClkRateGet()/ 10000); /* This needs to be tuned - now 100us */  
    CACHE_PIPE_FLUSH();

    /* 
     * Wait for the SEL signal to go HIGH and BSY to go LOW. The above SELECT
     * instruction asserts SEL signal after succeeding in arbitration. It then
     * asserts data lines corresponding to sym895's SCSI ID and the ID of target 
     * (which in this case are 7).Then initiator (SELECT instruction) releases
     * the BSY signal. 
     */      

    temp = 0;

    while ((SYM895_REG8_READ (pSiop, SYM895_OFF_SBCL) & 
          (SYM895_SBCL_BSY | SYM895_SBCL_SEL)) != 0x10) 
        {      
        temp++;
        if ( temp == 0)
            {
            SCSI_MSG ("selection timeout\n", 0, 0, 0, 0, 0, 0);
            retVal = ERROR;
            goto despatch;
            }
        }
 
    SCSI_MSG ("sym895Loopback: Selecting target ID = %x\n", 
               SYM895_REG16_READ (pSiop, SYM895_OFF_SBDL), 0, 0, 0, 0, 0); 

    /* 
     * Check whether correct bits are driven on the SCSI data bus during 
     * selection. 
     */

    if ( SYM895_REG16_READ (pSiop, SYM895_OFF_SBDL) != 
         SYM895_REG16_READ (pSiop, SYM895_OFF_RESPID0))
        {
        SCSI_MSG ("sym895LoopBack: Error in Data Paths \n",
                   0, 0, 0, 0, 0, 0);
        retVal = ERROR;
        goto despatch;
        }
 
    /* Now to respond to selection, BSY signal should be asserted */

    SYM895_REG8_WRITE (pSiop, SYM895_OFF_SOCL, 
                       (SYM895_REG8_READ(pSiop,SYM895_OFF_SOCL) | 
                                         SYM895_SOCL_BSY));

    /* 
     * Once BSY is asserted, the initiator confirms its selection by releasing
     * SEL signal. Wait for SEL to be deasserted.
     */

    temp =0;

    while ((SYM895_REG8_READ (pSiop, SYM895_OFF_SBCL) & SYM895_SBCL_SEL) != 0)
        {
        temp++;
        if ( temp == 0)
            {
            SCSI_MSG ("selection timeout\n", 0, 0, 0, 0, 0, 0);
            retVal = ERROR;
            goto despatch;
            }
        }  

    /* 
     * if the initiator has asserted ATN while selecting, assert BSY and
     * MSG_OUT phase. If not ATN, Selection phase is complete.
     */

    if ( SYM895_REG8_READ (pSiop, SYM895_OFF_SBCL) & SYM895_SBCL_ATN )
        { 

        SCSI_MSG ("sym895Loopback: Changing phase to MSG_OUT\n",
                  0, 0, 0, 0, 0, 0);
        SYM895_REG8_WRITE (pSiop, SYM895_OFF_SOCL,
                           SYM895_REG8_READ (pSiop, SYM895_OFF_SOCL) |  
                           (SYM895_SOCL_BSY | SYM895_SOCL_MSG | SYM895_SOCL_CD)
                          );    
  
        /* Selection Phase (with ATN) is now complete */     
    
        /* 
         * Wait for the Single-step status bit to set in DSTAT. This bit is set
         * after successful execution of each SCRIPTS instruction. 
         */
      
        while ((SYM895_REG8_READ (pSiop, SYM895_OFF_DSTAT) & SYM895_B_SSI) == 0)
            SCSI_MSG ("Sym895Loopback: Waiting for completion of SCRIPT \
                      Instruction 1\n", 0, 0, 0, 0, 0, 0);
     
        /* 
         * Clear all interrupts just in case. Any pending interrupts whether
         * valid or stray will inhibit the execution of further SCRIPTS 
         * instruction.
         */

        temp = SYM895_REG8_READ (pSiop, SYM895_OFF_ISTAT);  
        sym895Delay();
        CACHE_PIPE_FLUSH();
        temp = SYM895_REG8_READ (pSiop, SYM895_OFF_SIST0);    
        sym895Delay();
        CACHE_PIPE_FLUSH();
        temp = SYM895_REG8_READ (pSiop, SYM895_OFF_SIST1); 
        sym895Delay();
        CACHE_PIPE_FLUSH();

        temp = SYM895_REG8_READ (pSiop, SYM895_OFF_DSTAT);  
       
        /* 
         * Set the Start DMA bit in DCNTL register. This restarts execution
         * of SCRIPTS. Now that SELECT instruction is successfully completed 
         * and SCSI phase is MSG_OUT, the next SCRIPT instruction sends the
         * IDENTIFY message to target.
         * 
         * MOVE FROM IDENT_BUF, WHEN MSG_OUT
         */

        SCSI_MSG ("sym895Loopback: Sending 8-bit Message 0x%x\n",
                  msgBuf, 0, 0, 0, 0, 0);

        SYM895_REG8_WRITE (pSiop, SYM895_OFF_DCNTL, 
                           (SYM895_REG8_READ (pSiop, SYM895_OFF_DCNTL) |
                           SYM895_DCNTL_STD));

        /* Wait for the SCRIPT instruction to finish using the host bus */

        CACHE_PIPE_FLUSH();        
        taskDelay (sysClkRateGet()/ 1000); /* This needs to be tuned */ 
                                            /* now 100us */           
        CACHE_PIPE_FLUSH();
    
        /* 
         * The MOVE instruction waits for a phase change indicated by 
         * asserting of SREQ signal by the target. So assert SREQ.
         */
    
        SYM895_REG8_WRITE (pSiop, SYM895_OFF_SOCL,0xae); /* Hard code */
       
        /* Wait for the ACK to be set by the initiator */

        temp = 0;

        while ((SYM895_REG8_READ (pSiop, SYM895_OFF_SBCL) & SYM895_SBCL_ACK) == 0) 
            {
            temp++;
            if ( temp == 0)
                {
                SCSI_MSG ("ACK timeout\n", 0, 0, 0, 0, 0, 0);
                retVal = ERROR;
                goto despatch;
                }
            }

        msgInData = SYM895_REG8_READ (pSiop, SYM895_OFF_SBDL); /* read data */ 
       
        /* Deassert SREQ signal */
 
        SYM895_REG8_WRITE (pSiop, SYM895_OFF_SOCL, 
                           (SYM895_REG8_READ (pSiop, SYM895_OFF_SOCL) & 
                           ~SYM895_SOCL_REQ));

        /* Wait for the ACK signal to be deasserted by initiator */
 
        temp = 0; 
        while ((SYM895_REG8_READ (pSiop, SYM895_OFF_SBCL) & SYM895_SBCL_ACK) != 0)
            {
            temp++;
            if ( temp == 0)
                {
                SCSI_MSG ("ACK timeout\n", 0, 0, 0, 0, 0, 0);
                retVal = ERROR;
                goto despatch;
                }
            }

        SCSI_MSG ("sym895Loopback: Received Message In data = 0x%x\n",
                  msgInData, 0, 0, 0, 0, 0);
       
        if ( msgInData != msgBuf)
            {                 
            SCSI_MSG ("sym895Loopback: Error reading SCSI D0-D7 data bus\n",
                      0, 0, 0, 0, 0, 0);
            retVal = ERROR;
            goto despatch;
            }

        while ((SYM895_REG8_READ (pSiop, SYM895_OFF_DSTAT) & SYM895_B_SSI) == 0)
            logMsg("Sym895Loopback: Waiting for completion of SCRIPT \
                   Instruction 2\n", 0, 0, 0, 0, 0, 0);

        SCSI_MSG ("sym895Loopback: Changing phase to DATA_OUT\n",
                  0, 0, 0, 0, 0, 0);
        SYM895_REG8_WRITE (pSiop, SYM895_OFF_SOCL, 
                           SYM895_REG8_READ (pSiop, SYM895_OFF_SOCL) &
                           ~(SYM895_SOCL_MSG | SYM895_SOCL_CD |
                           SYM895_SOCL_IO) ); 

        /* 
         * Set the Start DMA bit in DCNTL register. This restarts execution
         * of SCRIPTS. Now that SELECT instruction is successfully completed 
         * and SCSI phase is MSG_OUT, the next SCRIPT instruction sends the
         * 16-bit Data to target . 
         * 
         * MOVE FROM DATA_BUF, WHEN DATA_OUT
         */

        SYM895_REG8_WRITE (pSiop, SYM895_OFF_SCNTL3, 
                           (SYM895_REG8_READ (pSiop, SYM895_OFF_SCNTL3) | 
                           SYM895_SCNTL3_EWS));

        SCSI_MSG ("sym895Loopback: Sending 16-bit Data 0x%x\n",
                  dataBuf, 0, 0, 0, 0, 0);
        SYM895_REG8_WRITE (pSiop, SYM895_OFF_DCNTL, 
                           (SYM895_REG8_READ (pSiop, SYM895_OFF_DCNTL) | 
                           SYM895_DCNTL_STD));

        /* Wait for the SCRIPT instruction to finish using the host bus */

        CACHE_PIPE_FLUSH();
        taskDelay (sysClkRateGet()/ 1000); /* This needs to be tuned */ 
                                            /* now 100us */           
        CACHE_PIPE_FLUSH();
    
        /* 
         * The MOVE instruction waits for a phase change indicated by 
         * asserting of SREQ signal by the target. Assert SREQ.
         */
        
        SYM895_REG8_WRITE (pSiop, SYM895_OFF_SOCL , 0xa0);   /* Hard code */    

        /* Wait for the ACK to be set by the initiator */

        temp =0;

        while ((SYM895_REG8_READ (pSiop, SYM895_OFF_SBCL) & 
                                  SYM895_SBCL_ACK) == 0)
            {
            temp++;
            if ( temp == 0)
                {
                SCSI_MSG ("ACK timeout\n", 0, 0, 0, 0, 0, 0);
                retVal = ERROR;
                goto despatch;
                }
            }
 
        msgInData = SYM895_REG16_READ (pSiop, SYM895_OFF_SBDL); /* read data bus */

        /* Deassert SREQ signal */
 
        SYM895_REG8_WRITE (pSiop, SYM895_OFF_SOCL, 
                           (SYM895_REG8_READ (pSiop, SYM895_OFF_SOCL) & 
                           ~SYM895_SOCL_REQ));

        /* Wait for the ACK signal to be deasserted by initiator */
  
        temp = 0;
        while ((SYM895_REG8_READ (pSiop, SYM895_OFF_SBCL) & 
                                  SYM895_SBCL_ACK) != 0)
            {
            temp++;
            if ( temp == 0)
                {
                SCSI_MSG ("ACK timeout\n", 0, 0, 0, 0, 0, 0);
                retVal = ERROR;
                goto despatch;
                }
            }

        SCSI_MSG ("sym895Loopback: Received Data In = 0x%x\n",
                  msgInData, 0, 0, 0, 0, 0);

        if ( msgInData != dataBuf)
            {
            SCSI_MSG ("sym895Loopback: Error reading SCSI D0-D15 data bus\n",
                      0, 0, 0, 0, 0, 0);
            retVal = ERROR;
            goto despatch;
            }
 
	SCSI_MSG("sym895Loopback: Loopback diagnostics "
                 "successful.\n", 0, 0, 0, 0, 0, 0);
        retVal = OK;
        }
    else    /* select without ATN */
        { 
        SCSI_MSG ("sym895Loopback: Error ATN not asserted during selection\n",
                  0, 0, 0, 0, 0, 0);
        retVal = ERROR;
        }

despatch:
 
    /* Set the SCSI control lines to Bus free state */

    SYM895_REG8_WRITE (pSiop, SYM895_OFF_SOCL,
                       (SYM895_REG8_READ(pSiop,SYM895_OFF_SOCL) & 
                       ~(SYM895_SOCL_CD | SYM895_SOCL_MSG | SYM895_SOCL_IO)));

    SYM895_REG8_WRITE ( pSiop, SYM895_OFF_SCNTL2,
                        SYM895_REG8_READ (pSiop,SYM895_OFF_SCNTL2) & 
                        ~(SYM895_SCNTL2_SDU) );

    SYM895_REG8_WRITE ( pSiop, SYM895_OFF_SCNTL1,
                        SYM895_REG8_READ (pSiop,SYM895_OFF_SCNTL1) & 
                        ~(SYM895_SCNTL1_CON) );

    SYM895_REG8_WRITE (pSiop, SYM895_OFF_SOCL,0x00);

  
    /* Enable manual Start */

    SYM895_REG8_WRITE (pSiop, SYM895_OFF_DMODE, 
                       (SYM895_REG8_READ (pSiop, SYM895_OFF_DMODE) | 
                       SYM895_DMODE_MAN)); 

    /* Clear off all interrupts. */

    temp = SYM895_REG8_READ (pSiop, SYM895_OFF_ISTAT);
    sym895Delay();
    CACHE_PIPE_FLUSH();
    temp = SYM895_REG8_READ (pSiop, SYM895_OFF_SIST0);    
    sym895Delay();
    CACHE_PIPE_FLUSH();
    temp = SYM895_REG8_READ (pSiop, SYM895_OFF_SIST1); 
    sym895Delay();
    CACHE_PIPE_FLUSH();
    temp = SYM895_REG8_READ (pSiop, SYM895_OFF_DSTAT);

    /* Set back the initial interrupt enable bits and bring to normal mode */
 
    SYM895_REG8_WRITE (pSiop, SYM895_OFF_DIEN,  dien);	
    SYM895_REG8_WRITE (pSiop, SYM895_OFF_SIEN0, sien0);	    
    SYM895_REG8_WRITE (pSiop, SYM895_OFF_SIEN1, sien1);	
    SYM895_REG8_WRITE (pSiop, SYM895_OFF_STEST2, stest2);	    
    SYM895_REG8_WRITE (pSiop, SYM895_OFF_DCNTL, dcntl);     

    /* 
     * Start the WAIT script.. Again this is needed only if "sym895Loopback" is 
     * called while any scripts are running. If the WAIT scripts are not 
     * restarted, all SCSI transactions will be aborted as thread activation 
     * do not take place.
     */

    sym895ScriptStart (pSiop, (SYM895_THREAD *)pScsiCtrl->pIdentThread,
                       SYM895_SCRIPT_WAIT);  

    return (retVal);
    } 
