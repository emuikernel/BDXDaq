/* ln97xEnd.c - END style AMD Am79C97X PCnet-PCI Ethernet driver */

/* Copyright 1984-2002 Wind River Systems, Inc., and Cetia Inc. */

#include "copyright_wrs.h"

/*
modification history
--------------------
01n,30may02,sru  Add run-time control of byte-swapping for PCI master
		 operations.
01m,25jan02,pai  Reworked polled-mode code (SPR #72034).  Corrected
                 ln97xConfig() abuse.  Removed unused DRV_PRINT().
                 Removed gratuitous header file inclusions.  Replaced most
                 driver macros with inline functions.  Reworked logical
                 address support.
01l,14jan02,dat  Removing warnings from Diab compiler
01k,17oct01,pai  Added support for custom device description string.
01j,27mar01,tlc  Add configuration routine to set up the PCI bus endianess and
                 interuppt level expected by the driver.
01i,20nov00,jkf  SPR#62266 T3 doc update
01h,10nov00,mks  passing in correct type of pointer in strtok_r. SPR 62224
01g,11jun00,ham  removed reference to etherLib.
01f,19may00,pai  Implemented accumulated driver bug fixes (SPR #31349).
01e,27jan00,dat  fixed use of NULL
01d,13oct99,stv  Fixed the bug of failing to release the transmit
                 semaphore (SPR #29094).
01c,08dec98,snk  made architecture independant, by teamF1 Inc.
		 tested with x86 & power Pc architectures.
01b,21sep98,dat  modified to stand alone
01a,01aug98,dmb  written from if_lnPci.c, ver 01e
*/

/*
DESCRIPTION
This module implements the Advanced Micro Devices Am79C970A, Am79C971,
Am79C972, and Am79C973 PCnet-PCI Ethernet 32-bit network interface driver.

The PCnet-PCI ethernet controller is inherently little-endian because
the chip is designed to operate on a PCI bus which is a little-endian
bus. The software interface to the driver is divided into three parts.
The first part is the PCI configuration registers and their set up. 
This part is done at the BSP level in the various BSPs which use this
driver. The second and third part are dealt with in the driver. The second
part of the interface is comprised of the I/O control registers and their
programming. The third part of the interface is comprised of the descriptors
and the buffers. 

This driver is designed to be moderately generic, operating unmodified
across the range of architectures and targets supported by VxWorks.  To
achieve this, the driver must be given several target-specific parameters,
and some external support routines must be provided. These target-specific
values and the external support routines are described below.

This driver supports multiple units per CPU.  The driver can be
configured to support big-endian or little-endian architectures.  It
contains error recovery code to handle known device errata related to DMA
activity. 

Some big-endian processors may be connected to a PCI bus through a
host/PCI bridge which performs byte swapping during data phases.  On such
platforms, the PCnet-PCI controller need not perform byte swapping during a
DMA access to memory shared with the host processor.

BOARD LAYOUT
This device is on-board.  No jumpering diagram is necessary.

EXTERNAL INTERFACE
The drvier provides one standard external interface, ln97xEndLoad().  As
input, this routine takes a string of colon-separated parameters.  The
parameters should be specified in hexadecimal (optionally preceded by '0x'
or a minus sign '-').  The parameter string is parsed using strtok_r().

TARGET-SPECIFIC PARAMETERS
The format of the parameter string is:

<unit:devMemAddr:devIoAddr:pciMemBase:vecNum:intLvl:
memAdrs:memSize:memWidth:csr3b:offset:flags>

\is
\i <unit>
The unit number of the device.  Unit numbers start at zero and increase for
each device controlled by the same driver.  The driver does not use this
value directly.  The unit number is passed through the MUX API where it is
used to differentiate between multiple instances of a particular driver.

\i <devMemAddr>
This parameter is the memory mapped I/O base address of the device registers
in the memory map of the CPU.  The driver will locate device registers as
offsets from this base address.

The PCnet presents two registers to the external interface, the RDP (Register
Data Port) and RAP (Register Address Port) registers.  This driver assumes 
that these two registers occupy two unique addresses in a memory space
that is directly accessible by the CPU executing this driver.  The driver
assumes that the RDP register is mapped at a lower address than the RAP
register; the RDP register is therefore derived from the "base address."
This is a required parameter.

\i <devIoAddr>
This parameter specifies the I/O base address of the device registers in the
I/O map of some CPUs. It indicates to the driver where to find the RDP
register.  This parameter is no longer used, but is retained so that the
load string format will be compatible with legacy initialization routines.
The driver will always use memory mapped I/O registers specified via the
<devMemAddr> parameter.

\i <pciMemBase>
This parameter is the base address of the host processor memory as seen
from the PCI bus. This parameter is zero for most Intel architectures.

\i <vecNum>
This parameter is the vector associated with the device interrupt.
This driver configures the PCnet device to generate hardware interrupts
for various events within the device; thus it contains
an interrupt handler routine.  The driver calls pciIntConnect() to connect
its interrupt handler to the interrupt vector generated as a result of the
PCnet interrupt.

\i <intLvl>
Some targets use additional interrupt controller devices to help organize
and service the various interrupt sources.  This driver avoids all
board-specific knowledge of such devices.  During the driver's
initialization, the external routine sysLan97xIntEnable() is called to
perform any board-specific operations required to allow the servicing of a
PCnet interrupt.  For a description of sysLan97xIntEnable(), see "External
Support Requirements" below.

\i <memAdrs>
This parameter gives the driver the memory address to carve out its
buffers and data structures. If this parameter is specified to be
NONE then the driver allocates cache coherent memory for buffers
and descriptors from the system memory pool.
The PCnet device is a DMA type of device and typically shares access to
some region of memory with the CPU.  This driver is designed for systems
that directly share memory between the CPU and the PCnet.  It
assumes that this shared memory is directly available to it
without any arbitration or timing concerns.

\i <memSize>
This parameter can be used to explicitly limit the amount of shared
memory (bytes) this driver will use.  The constant NONE can be used to
indicate no specific size limitation.  This parameter is used only if
a specific memory region is provided to the driver.

\i <memWidth>
Some target hardware that restricts the shared memory region to a
specific location also restricts the access width to this region by
the CPU.  On these targets, performing an access of an invalid width
will cause a bus error.

This parameter can be used to specify the number of bytes of access
width to be used by the driver during access to the shared memory.
The constant NONE can be used to indicate no restrictions.

Current internal support for this mechanism is not robust; implementation 
may not work on all targets requiring these restrictions.

\i <csr3b>
The PCnet-PCI Control and Status Register 3 (CSR3) controls, among
other things, big-endian and little-endian modes of operation.  When
big-endian mode is selected, the PCnet-PCI controller will swap the
order of bytes on the AD bus during a data phase on access to the FIFOs
only:  AD[31:24] is byte 0, AD[23:16] is byte 1, AD[15:8] is byte 2 and
AD[7:0] is byte 3.  In order to select the big-endian mode, set this
parameter to (0x0004).  Most implementations, including natively
big-endian host architectures, should set this parameter to (0x0000) in
order to select little-endian access to the FIFOs, as the driver is
currently designed to perform byte swapping as appropriate to the host
architecture.

\i <offset>
This parameter specifies a memory alignment offset.  Normally this
parameter is zero except for architectures which can only access 32-bit
words on 4-byte aligned address boundaries.  For these architectures the
value of this offset should be 2.

\i <flags>
This is parameter is used for future use.  Currently its value should be
zero.
\ie

EXTERNAL SUPPORT REQUIREMENTS
This driver requires five externally defined support functions that can
be customized by modifying global pointers.  The function pointer types
and default "bindings" are specified below.  To change the defaults, the
BSP should create an appropriate routine and set the function pointer
before first use.  This would normally be done within sysHwInit2().

Note that all of the pointers to externally defined functions <must> be
set to a valid executable code address.  Also, note that
sysLan97xIntEnable(), sysLan97xIntDisable(), and sysLan97xEnetAddrGet()
must be defined in the BSP.  This was done so that the driver would be
compatible with initialization code and support routines in existing BSPs.

The function pointer convention has been introduced to facilitate future
driver versions that do not explicitly reference a named BSP-defined
function.  Among other things, this would allow a BSP designer to define,
for example, one endIntEnable() routine to support multple END drivers.

\is
\i 'ln97xIntConnect'
\cs
    IMPORT STATUS (* ln97xIntConnect)
        (
        VOIDFUNCPTR * vector,     /@ interrupt vector to attach to     @/
        VOIDFUNCPTR   routine,    /@ routine to be called              @/
        int           parameter   /@ parameter to be passed to routine @/
        );

    /@ default setting @/

    ln97xIntConnect = pciIntConnect;
\ce
The ln97xIntConnect pointer specifies a function used to connect the
driver interrupt handler to the appropriate vector.  By default it is
the <pciIntLib> routine pciIntConnect().

\i 'ln97xIntDisconnect'
\cs
    IMPORT STATUS (* ln97xIntDisconnect)
        (
        VOIDFUNCPTR * vector,     /@ interrupt vector to attach to     @/
        VOIDFUNCPTR   routine,    /@ routine to be called              @/
        int           parameter   /@ routine parameter                 @/
        );

    /@ default setting @/

    ln97xIntDisconnect = pciIntDisconnect2;
\ce
The ln97xIntDisconnect pointer specifies a function used to disconnect
the interrupt handler prior to unloading the driver.  By default it is
the <pciIntLib> routine pciIntDisconnect2().

\i 'ln97xIntEnable'
\cs
    IMPORT STATUS (* ln97xIntEnable)
        (
        int level                 /@ interrupt level to be enabled @/
        );

    /@ default setting @/

    ln97xIntEnable = sysLan97xIntEnable;
\ce
The ln97xIntEnable pointer specifies a function used to enable the
interrupt level for the END device.  It is called once during
initialization.  By default it is a BSP routine named sysLan97xIntEnable(). 
The implementation of this routine can vary between architectures, and
even between BSPs for a given architecture family.  Generally, the
parameter to this routine will specify an interrupt <level> defined for an
interrupt controller on the host platform.  For example, MIPS and PowerPC
BSPs may implement this routine by invoking the WRS intEnable() library
routine.  WRS Intel Pentium BSPs may implement this routine via
sysIntEnablePIC().

\i 'ln97xIntDisable'
\cs
    IMPORT STATUS (* ln97xIntDisable)
        (
        int level                 /@ interrupt level to be disabled @/
        );

    /@ default setting @/

    ln97xIntDisable = sysLan97xIntDisable;
\ce
The ln97xIntDisable pointer specifies a function used to disable the
interrupt level for the END device.  It is called during stop.  By default
it is a BSP routine named sysLan97xIntDisable().  The implementation of
this routine can vary between architectures, and even between BSPs for a
given architecture family.  Generally, the parameter to this routine will
specify an interrupt <level> defined for an interrupt controller on the
host platform.  For example, MIPS and PowerPC BSPs may implement this
routine by invoking the WRS intDisable() library routine.  WRS Intel
Pentium BSPs may implement this routine via sysIntDisablePIC().

\i 'ln97xEnetAddrGet'
\cs
    IMPORT STATUS (* ln97xEnetAddrGet)
        (LN_97X_DRV_CTRL * pDrvCtrl, char * pStationAddr);

    /@ default setting @/

    ln97xEnetAddrGet = sysLan97xEnetAddrGet;
\ce
The ln97xEnetAddrGet pointer specifies a function used to get the Ethernet
(IEEE station) address of the device.  By default it is a BSP routine
named sysLan97xEnetAddrGet().
\ie

SYSTEM RESOURCE USAGE
When implemented, this driver requires the following system resources:

    - one mutual exclusion semaphore
    - one interrupt vector
    - 14240 bytes in text for a PENTIUM3 target
    - 120 bytes in the initialized data section (data)
    - 0 bytes in the uninitialized data section (BSS)

The driver allocates clusters of size 1520 bytes for receive frames and
and transmit frames.

SEE ALSO: muxLib, endLib, netBufLib

\tb "Network Protocol Toolkit User's Guide"

\tb "PCnet-PCI II Single-Chip Full-Duplex Ethernet Controller
for PCI Local Bus Product"

\tb "PCnet-FAST Single-Chip Full-Duplex 10/100 Mbps Ethernet Controller
for PCI Local Bus Product"
*/

#include "vxWorks.h"
#include "taskLib.h"
#include "stdio.h"
#include "stdlib.h"
#include "sysLib.h"
#include "intLib.h"
#include "iv.h"
#include "netLib.h"

#define END_MACROS

#include "etherMultiLib.h"
#include "endLib.h"
#include "drv/end/ln97xEnd.h"


/* defines */

/*
 * If LN_KICKSTART_TX is TRUE the transmitter is kick-started to force a
 * read of the transmit descriptors, otherwise the internal polling (1.6msec)
 * will initiate a read of the descriptors.  This should be FALSE if there
 * is any chance of memory latency or chip accesses detaining the PCnet DMA,
 * which results in a transmitter UFLO error.  This can be changed with the
 * global lnKickStartTx below.
 */

#define LN_KICKSTART_TX TRUE


/* Cache macros */

#define LN_CACHE_INVALIDATE(address, len) \
    CACHE_DRV_INVALIDATE (&pDrvCtrl->cacheFuncs, (address), (len))

#define LN_CACHE_VIRT_TO_PHYS(address) \
    CACHE_DRV_VIRT_TO_PHYS (&pDrvCtrl->cacheFuncs, (address))

#define LN_CACHE_PHYS_TO_VIRT(address) \
    CACHE_DRV_PHYS_TO_VIRT (&pDrvCtrl->cacheFuncs, (address))


/* memory to PCI address translation macros */

#define PCI_TO_MEM_PHYS(pciAdrs) \
    ((UINT)(pciAdrs) - (pDrvCtrl->pciMemBase))

#define MEM_TO_PCI_PHYS(memAdrs) \
    ((UINT)(memAdrs) + (pDrvCtrl->pciMemBase))
    
#define SYS_OUT_LONG(pDrvCtrl, addr, value)     \
    if (pDrvCtrl->flags & LS_MODE_MEM_IO_MAP)   \
        *((volatile UINT32 *)(addr)) = (value); \
    else                                        \
        sysOutLong ((int)(addr), (value));      \

#define SYS_IN_LONG(pDrvCtrl, addr, data)       \
    if (pDrvCtrl->flags & LS_MODE_MEM_IO_MAP)   \
        ((data) = *((volatile UINT32 *)(addr)));\
    else                                        \
        ((data) = sysInLong ((int) (addr)));    \


/* A shortcut for getting the hardware address from the MIB II stuff. */

/* RFC 1213 (old) */

#define END_HADDR(pEnd) \
    ((pEnd)->mib2Tbl.ifPhysAddress.phyAddress)

#define END_HADDR_LEN(pEnd) \
    ((pEnd)->mib2Tbl.ifPhysAddress.addrLength)

/* RFC 2233 (new - not implemented in this driver yet)
 *
 * #define END_HADDR(pEnd) \
 * ((pEnd)->pMib2Tbl->m2Data.mibIfTbl.ifPhysAddress.phyAddress)
 *
 * #define END_HADDR_LEN(pEnd) \
 * ((pEnd)->pMib2Tbl->m2Data.mibIfTbl.ifPhysAddress.addrLength)
 */


/* imports */

IMPORT int    endMultiLstCnt (END_OBJ *);


/* bind external support interface to default BSP implementation */

IMPORT STATUS pciIntConnect (VOIDFUNCPTR *, VOIDFUNCPTR, int);
IMPORT STATUS pciIntDisconnect2 (VOIDFUNCPTR *, VOIDFUNCPTR, int);
IMPORT STATUS sysLan97xIntEnable (int);
IMPORT STATUS sysLan97xIntDisable (int);
IMPORT STATUS sysLan97xEnetAddrGet (LN_97X_DRV_CTRL *, char *);

STATUS (* ln97xIntConnect)
       (VOIDFUNCPTR *, VOIDFUNCPTR, int) = pciIntConnect;
STATUS (* ln97xIntDisconnect)
       (VOIDFUNCPTR *, VOIDFUNCPTR, int) = pciIntDisconnect2;

STATUS (* ln97xIntEnable)(int)           = sysLan97xIntEnable;
STATUS (* ln97xIntDisable)(int)          = sysLan97xIntDisable;
STATUS (* ln97xEnetAddrGet)
       (LN_97X_DRV_CTRL *, char *)       = sysLan97xEnetAddrGet;


#ifdef DRV_DEBUG        /* if debugging driver */

int ln97xDebug = DRV_DEBUG_LOAD | DRV_DEBUG_TX | DRV_DEBUG_RX;

#define DRV_LOG(FLG, X0, X1, X2, X3, X4, X5, X6)       \
    if (ln97xDebug & FLG)                              \
        logMsg((char *) X0, (int)X1, (int)X2, (int)X3, \
                            (int)X4, (int)X5, (int)X6);

#else /* DRV_DEBUG */

#define DRV_LOG(DBG_SW, X0, X1, X2, X3, X4, X5, X6)

#endif /* DRV_DEBUG */


/* locals */

LOCAL int lnTsize = LN_TMD_TLEN;    /* deflt xmit ring size as power of 2 */
LOCAL int lnRsize = LN_RMD_RLEN;    /* deflt recv ring size as power of 2 */

LOCAL volatile int ln97xBusEndianess   = LN_LITTLE_ENDIAN;
LOCAL volatile int ln97xInterruptLevel = LN_ACTIVE_LOW;

LOCAL BOOL lnKickStartTx = LN_KICKSTART_TX;

/* default description for the END device */

LOCAL char * pDescription = "AMD 79C97x PCnet-PCI Enhanced Network Driver";


/* forward declarations */

LOCAL STATUS    ln97xReset (LN_97X_DRV_CTRL * pDrvCtrl);
LOCAL void      ln97xInt (LN_97X_DRV_CTRL * pDrvCtrl);
LOCAL void      ln97xHandleRecvInt (LN_97X_DRV_CTRL * pDrvCtrl);
LOCAL STATUS    ln97xRecv (LN_97X_DRV_CTRL * pDrvCtrl, LN_RMD *rmd);
LOCAL LN_RMD *  ln97xFullRMDGet (LN_97X_DRV_CTRL * pDrvCtrl);
LOCAL void      ln97xRestart (LN_97X_DRV_CTRL * pDrvCtrl);
LOCAL STATUS    ln97xRestartSetup (LN_97X_DRV_CTRL * pDrvCtrl);
LOCAL STATUS    ln97xAddrFilterMod (LN_97X_DRV_CTRL *, const UINT8 *,
                                    FILTER_MOD_CMD);
LOCAL void      ln97xAddrFilterSet (LN_97X_DRV_CTRL * pDrvCtrl);
LOCAL void      ln97xTRingScrub (LN_97X_DRV_CTRL * pDrvCtrl);
LOCAL STATUS    ln97xMemInit (LN_97X_DRV_CTRL * pDrvCtrl);
LOCAL void      ln97xHandleError (LN_97X_DRV_CTRL * pDrvCtrl, UINT32 errs);

/* END-Specific interfaces */

LOCAL STATUS    ln97xStart (LN_97X_DRV_CTRL * pDrvCtrl);
LOCAL STATUS    ln97xStop (LN_97X_DRV_CTRL * pDrvCtrl);
LOCAL STATUS    ln97xUnload (LN_97X_DRV_CTRL * pDrvCtrl);
LOCAL int       ln97xIoctl (LN_97X_DRV_CTRL * pDrvCtrl, int cmd, caddr_t data);
LOCAL STATUS    ln97xSend (LN_97X_DRV_CTRL * pDrvCtrl, M_BLK_ID pBuf);
LOCAL STATUS    ln97xMCastAddrAdd (LN_97X_DRV_CTRL* pDrvCtrl, char * pAddress);
LOCAL STATUS    ln97xMCastAddrDel (LN_97X_DRV_CTRL * pDrvCtrl,
                                   char * pAddress);
LOCAL STATUS    ln97xMCastAddrGet (LN_97X_DRV_CTRL * pDrvCtrl,
                                    MULTI_TABLE * pTable);
LOCAL STATUS    ln97xPollSend (LN_97X_DRV_CTRL * pDrvCtrl, M_BLK_ID pBuf);
LOCAL STATUS    ln97xPollReceive (LN_97X_DRV_CTRL * pDrvCtrl, M_BLK_ID pBuf);
LOCAL STATUS    ln97xPollStart (LN_97X_DRV_CTRL * pDrvCtrl);
LOCAL STATUS    ln97xPollStop (LN_97X_DRV_CTRL * pDrvCtrl);

/*
 * Declare our function table.  This is static across all driver
 * instances.
 */
LOCAL NET_FUNCS ln97xFuncTable =
    {
    (FUNCPTR) ln97xStart,             /* Function to start the device. */
    (FUNCPTR) ln97xStop,              /* Function to stop the device. */
    (FUNCPTR) ln97xUnload,            /* Unloading function for the driver. */
    (FUNCPTR) ln97xIoctl,             /* Ioctl function for the driver. */
    (FUNCPTR) ln97xSend,              /* Send function for the driver. */
    (FUNCPTR) ln97xMCastAddrAdd,      /* Multicast address add  */
    (FUNCPTR) ln97xMCastAddrDel,      /* Multicast address delete */
    (FUNCPTR) ln97xMCastAddrGet,      /* Multicast table retrieve */
    (FUNCPTR) ln97xPollSend,          /* Polling send function  */
    (FUNCPTR) ln97xPollReceive,       /* Polling receive function */
    endEtherAddressForm,              /* Put address info into a packet.  */
    endEtherPacketDataGet,            /* Get a pointer to packet data. */
    endEtherPacketAddrGet             /* Get packet addresses. */
    };


/* inline functions */

/*******************************************************************************
*
* csrWrite - select and write a CSR register
*
* This routine selects a register to write, through the RAP register and
* then writes the CSR value to the RDP register.
*
* CAVEATS
* Assumes the most-significant 16-bits of <value> are reserved and should
* be written as zero.  This is true of many, but not all, PCnet CSR
* registers.
*
* RETURNS: N/A
*
* NOMANUAL
*/
__inline__ LOCAL void csrWrite
    (
    const LN_97X_DRV_CTRL * pDrvCtrl,  /* pointer to DRV_CTRL structure */
    int                     reg,       /* CSR register to select */
    UINT32                  value      /* CSR value to write */
    )
    {
    SYS_OUT_LONG (pDrvCtrl, 
		  pDrvCtrl->pRap, REG_SWAP (pDrvCtrl, reg));
    SYS_OUT_LONG (pDrvCtrl, 
		  pDrvCtrl->pRdp, REG_SWAP (pDrvCtrl, (value & 0x0000ffff)));
    }

/*******************************************************************************
*
* csrRead - select and read a CSR register
*
* This routine selects a register to read, through the RAP register and
* then reads the CSR value from the RDP register.
*
* CAVEATS
* Assumes the most-significant 16-bits of register are reserved and should
* be returned as zero.  This is true of many, but not all, PCnet CSR
* registers.
*
* RETURNS: The least significant 16-bits of the specified CSR register.
*
* NOMANUAL
*/
__inline__ LOCAL UINT32 csrRead
    (
    const LN_97X_DRV_CTRL * pDrvCtrl,  /* pointer to DRV_CTRL structure */
    int                     reg        /* register to select */
    )
    {
    UINT32                  result;

    SYS_OUT_LONG (pDrvCtrl, pDrvCtrl->pRap, REG_SWAP (pDrvCtrl, reg));
    SYS_IN_LONG (pDrvCtrl, pDrvCtrl->pRdp, result);

    return (REG_SWAP (pDrvCtrl, result) & 0x0000ffff);
    }

/*******************************************************************************
*
* csrLockedWrite - select and write a CSR register (interrupts locked)
*
* This routine selects a register to write, through the RAP register and
* then writes the CSR value to the RDP register.
*
* CAVEATS
* Assumes the most-significant 16-bits of <value> are reserved and should
* be written as zero.  This is true of many, but not all, PCnet CSR
* registers.
*
* RETURNS: N/A
*
* NOMANUAL
*/
__inline__ LOCAL void csrLockedWrite
    (
    const LN_97X_DRV_CTRL * pDrvCtrl,  /* pointer to DRV_CTRL structure */
    int                     reg,       /* CSR register to select */
    UINT32                  value      /* CSR value to write */
    )
    {
    int level = intLock ();

    SYS_OUT_LONG (pDrvCtrl, 
		  pDrvCtrl->pRap, REG_SWAP (pDrvCtrl, reg));
    SYS_OUT_LONG (pDrvCtrl, 
		  pDrvCtrl->pRdp, REG_SWAP (pDrvCtrl, (value & 0x0000ffff)));

    intUnlock (level);
    }

/*******************************************************************************
*
* csrLockedRead - select and read a CSR register (interrupts locked)
*
* This routine selects a register to read, through the RAP register and
* then reads the CSR value from the RDP register.
*
* CAVEATS
* Assumes the most-significant 16-bits of register are reserved and should
* be returned as zero.  This is true of many, but not all, PCnet CSR
* registers.
*
* RETURNS: The least significant 16-bits of the specified CSR register.
*
* NOMANUAL
*/
__inline__ LOCAL UINT32 csrLockedRead
    (
    const LN_97X_DRV_CTRL * pDrvCtrl,  /* pointer to DRV_CTRL structure */
    int                     reg        /* register to select */
    )
    {
    UINT32                  result;

    int level = intLock ();

    SYS_OUT_LONG (pDrvCtrl, pDrvCtrl->pRap, REG_SWAP (pDrvCtrl, reg));
    SYS_IN_LONG (pDrvCtrl, pDrvCtrl->pRdp, result);

    intUnlock (level);

    return (REG_SWAP (pDrvCtrl, result) & 0x0000ffff);
    }

/*******************************************************************************
*
* bcrWrite - select and write a BCR register
*
* This routine writes the bus configuration register. It first selects the
* BCR register to write through the RAP register and then it writes the value
* to the BDP register.
*
* CAVEATS
* Assumes the most-significant 16-bits of <value> are reserved and should
* be written as zero.  This is true of many, but not all, PCnet BCR
* registers.
*
* RETURNS: N/A
*
* NOMANUAL
*/
__inline__ LOCAL void bcrWrite
    (
    const LN_97X_DRV_CTRL * pDrvCtrl,  /* pointer to DRV_CTRL structure */
    int                     reg,       /* BCR register to select */
    UINT32                  value      /* BCR value to write */
    )
    {
    SYS_OUT_LONG (pDrvCtrl, 
		  pDrvCtrl->pRap, REG_SWAP (pDrvCtrl, reg));
    SYS_OUT_LONG (pDrvCtrl, 
		  pDrvCtrl->pBdp, REG_SWAP (pDrvCtrl, (value & 0x0000ffff)));
    }

/*******************************************************************************
*
* bcrRead - select and read a BCR register
*
* This routine reads the bus configuration register. It first selects the
* BCR register to read through the RAP register and then it reads the value
* from the BDP register.
*
* CAVEATS
* Assumes the most-significant 16-bits of register are reserved and should
* be returned as zero.  This is true of many, but not all, PCnet BCR
* registers.
*
* RETURNS: The least significant 16-bits of the specified BCR register.
*
* NOMANUAL
*/
__inline__ LOCAL UINT32 bcrRead
    (
    const LN_97X_DRV_CTRL * pDrvCtrl,  /* pointer to DRV_CTRL structure */
    int                     reg        /* register to select */
    )
    {
    UINT32                  result;

    SYS_OUT_LONG (pDrvCtrl, pDrvCtrl->pRap, REG_SWAP (pDrvCtrl, reg));
    SYS_IN_LONG (pDrvCtrl, pDrvCtrl->pBdp, result);

    return (REG_SWAP (pDrvCtrl, result) & 0x0000ffff);
    }

/******************************************************************************
*
* chipIdMake - get an Am79C97x PCI controller chip ID
*
* This routine returns a unique chip ID value derived from registers CSR88
* and CSR89.  The chip ID value represented by the returned constant is
* formed as follows:
*
*     chipId = (CSR(88) | (CSR(89) << 16))
*     chipId = (chipId >> 12) & 0xffff
*
* RETURNS:
* A chip ID value represented by one of the following constants
* \is
* \i "CHIP_ID_AM79C970A"
* Am79C970A PCnet-PCI II
* \i "CHIP_ID_AM79C971"
* Am79C971 PCnet-FAST
* \i "CHIP_ID_AM79C972"
* Am79C972 PCnet-FAST +
* \i "CHIP_ID_AM79C973"
* Am79C973 PCnet-FAST III
* \i "CHIP_ID_AM79C975"
* Am79C975 PCnet Fast III
* \ie
*
* NOMANUAL
*/
__inline__ LOCAL UINT32 chipIdMake
    (
    const LN_97X_DRV_CTRL * pDrvCtrl  /* specifies device control structure */
    )
    {
    return (((csrRead (pDrvCtrl, CSR(88)) |
             (csrRead (pDrvCtrl, CSR(89)) << 16)) >> 12) & 0x0000ffff);
    }

/******************************************************************************
*
* suspendModeEnter - enter Am79C97x suspend mode
*
* This routine will put the specified device in suspend mode by setting
* the SPND bit in CSR0.  Suspend mode allows updating certain registers
* without setting the STOP bit.
*
* Upon entering suspend mode, the controller will finish all on-going
* transmit activity and update the corresponding transmit descriptor
* entries.  The controller will then finish all on-going receive activity
* and update the corresponding receive descriptor entries.
*
* CAVEATS
* Do not call this routine with the STOP bit set in CSR0.  Do not set
* the STOP bit in CSR0 after this routine is called and before
* suspendModeLeave() is called.
*
* RETURNS:  A suspend-mode key for the controller state prior to the call.
*
* SEE ALSO: suspendModeLeave()
*
* NOMANUAL
*/
__inline__ LOCAL UINT32 suspendModeEnter
    (
    const LN_97X_DRV_CTRL * pDrvCtrl  /* specifies device control structure */
    )
    {
    int intLevel = intLock();

    UINT32 key = (csrRead (pDrvCtrl, CSR(5)) & CSR5_RESRVD_MASK);
    csrWrite (pDrvCtrl, CSR(5), key | CSR5_SPND);

    intUnlock (intLevel);

    do  {
        ;  /* poll for suspend mode entry */
        }
    while ((csrLockedRead (pDrvCtrl, CSR(5)) & CSR5_SPND) == 0);

    return key;
    }

/******************************************************************************
*
* suspendModeLeave - leave Am79C97x suspend mode
*
* This routine will take a specified device out of suspend mode by
* clearing the SPND bit in CSR0.  Upon entering suspend mode, the
* device should have been stopped with an orderly termination of network
* activity via suspendModeEnter().   The controller will leave the
* suspend mode and continue at the transmit and receive descriptor ring
* locations where it had left off.
*
* RETURNS: N/A
*
* SEE ALSO: suspendModeEnter()
*
* NOMANUAL
*/
__inline__ LOCAL void suspendModeLeave
    (
    const LN_97X_DRV_CTRL * pDrvCtrl,  /* specifies device control structure */
    UINT32                  key        /* key obtained when suspend entered */
    )
    {
    csrWrite (pDrvCtrl, CSR(5), key);
    }

/******************************************************************************
*
* rmdBuffAddrSet - set a receive descriptor's packet buffer address
*
* This routine sets the input packet buffer address for an RMD, specified
* by <pRmd>, to the address specified by <pBuf>.
*
* RETURNS: N/A
*
* NOMANUAL
*/
__inline__ LOCAL void rmdBuffAddrSet
    (
    const LN_97X_DRV_CTRL * pDrvCtrl,  /* specifies device control structure */
    volatile LN_RMD *       pRmd,      /* specifies an RMD structure */
    const char *            pBuf       /* buffer addr to associate with RMD */
    )
    {
    UINT32 pTmp  = MEM_TO_PCI_PHYS (LN_CACHE_VIRT_TO_PHYS ((UINT32) pBuf));
    pRmd->lnRMD0 = (UINT32) PCI_SWAP (pTmp);
    }

/******************************************************************************
*
* tmdBuffAddrSet - set a transmit descriptor's packet buffer address
*
* This routine sets the output packet buffer address for a TMD, specified
* by <pTmd>, to the address specified by <pBuf>.
*
* RETURNS: N/A
*
* NOMANUAL
*/
__inline__ LOCAL void tmdBuffAddrSet
    (
    const LN_97X_DRV_CTRL * pDrvCtrl,  /* specifies device control structure */
    volatile LN_TMD *       pTmd,      /* specifies a TMD structure */
    const char *            pBuf       /* buffer addr to associate with TMD */
    )
    {
    UINT32 pTmp  = MEM_TO_PCI_PHYS (LN_CACHE_VIRT_TO_PHYS ((UINT32) pBuf));
    pTmd->lnTMD0 = (UINT32) PCI_SWAP (pTmp);
    }

/******************************************************************************
*
* tmdBuffAddrGet - get a transmit descriptor's packet buffer address
*
* RETURNS:  The packet buffer address associated with the specified TMD.
*
* NOMANUAL
*/
__inline__ LOCAL char * tmdBuffAddrGet
    (
    const LN_97X_DRV_CTRL * pDrvCtrl,  /* specifies device control structure */
    const volatile LN_TMD * pTmd       /* specifies a TMD structure */
    )
    {
    UINT32 pTemp = PCI_TO_MEM_PHYS (PCI_SWAP (pTmd->lnTMD0));
    return (char *)(LN_CACHE_PHYS_TO_VIRT (pTemp));
    }

/******************************************************************************
*
* rmdBuffAddrGet - get a receive descriptor's packet buffer address
*
* RETURNS:  The packet buffer address associated with the specified RMD.
*
* NOMANUAL
*/
__inline__ LOCAL char * rmdBuffAddrGet
    (
    const LN_97X_DRV_CTRL * pDrvCtrl,  /* specifies device control structure */
    const volatile LN_RMD * pRmd       /* specifies a RMD structure */
    )
    {
    UINT32 pTemp = PCI_TO_MEM_PHYS (PCI_SWAP (pRmd->lnRMD0));
    return (char *)(LN_CACHE_PHYS_TO_VIRT (pTemp));
    }

/******************************************************************************
*
* txDescClean - clean a transmit message descriptor (TMD)
*
* RETURNS: N/A
*
* NOMANUAL
*/
__inline__ LOCAL void txDescClean
    (
    volatile LN_TMD * pTmd   /* specifies the TMD to "clean" */
    )
    {
    pTmd->lnTMD2 = 0;        /* clear error bit fields */
    pTmd->lnTMD1 = PCI_SWAP (TMD1_CNST | TMD1_ENP | TMD1_STP);
    }

/******************************************************************************
*
* rxDescClean - clean a receive message descriptor (RMD)
*
* RETURNS: N/A
*
* NOMANUAL
*/
__inline__ LOCAL void rxDescClean
    (
    volatile LN_RMD * pRmd   /* specifies the RMD to "clean" */
    )
    {
    UINT32 temp = (RMD1_BCNT_MSK & -(LN_BUFSIZ)) |
                  (RMD1_OWN | RMD1_CNST | RMD1_STP | RMD1_ENP);

    pRmd->lnRMD2 = 0;        /* reset the message byte count */
    pRmd->lnRMD1 = PCI_SWAP (temp);
    }

/******************************************************************************
*
* ibRxRingAddrSet - set RMD ring base address in the init block
*
* RETURNS: N/A
*
* NOMANUAL
*/
__inline__ LOCAL void ibRxRingAddrSet
    (
    LN_97X_DRV_CTRL * pDrvCtrl  /* specifies device control structure */
    )
    {
    UINT32 pTemp = MEM_TO_PCI_PHYS (LN_CACHE_VIRT_TO_PHYS(pDrvCtrl->pRring));
    (pDrvCtrl->ib->lnIBRdra) = PCI_SWAP (pTemp);

    pTemp  = PCI_SWAP (pDrvCtrl->ib->lnIBMode);
    pTemp |= IB_MODE_RLEN_MSK & ((pDrvCtrl->rringLen) << 20);

    (pDrvCtrl->ib->lnIBMode) = PCI_SWAP (pTemp);
    }

/******************************************************************************
*
* ibTxRingAddrSet - set TMD ring base address in the init block
*
* RETURNS: N/A
*
* NOMANUAL
*/
__inline__ LOCAL void ibTxRingAddrSet
    (
    LN_97X_DRV_CTRL * pDrvCtrl  /* specifies device control structure */
    )
    {
    UINT32 pTemp = MEM_TO_PCI_PHYS (LN_CACHE_VIRT_TO_PHYS(pDrvCtrl->pTring));
    (pDrvCtrl->ib->lnIBTdra) = PCI_SWAP (pTemp);

    pTemp  = PCI_SWAP (pDrvCtrl->ib->lnIBMode);
    pTemp |= IB_MODE_TLEN_MSK & ((pDrvCtrl->tringLen) << 28);

    (pDrvCtrl->ib->lnIBMode) = PCI_SWAP (pTemp);
    }

/******************************************************************************
*
* ibAddrFilterClear - clear the initialization block logical address filter
*
* This routine clears the logical address filter in the specified
* initialization block memory, <pIb>.  The logical address filter
* registers, CSR8 - CSR11, are not cleared in this routine.
*
* RETURNS: N/A
*
* NOMANUAL
*/
__inline__ LOCAL void ibAddrFilterClear
    (
    volatile LN_IB * pIb  /* initialization block base address */
    )
    {
    /* alias <lnIBLadrf> for a more efficient data MOV */

    UINT32 * const pLadrf = (UINT32 *)(&(pIb->lnIBLadrf[0]));

    pLadrf[0] = 0x00000000;
    pLadrf[1] = 0x00000000;
    }

/******************************************************************************
*
* ibAddrFilterSet - set the initialization block logical address filter
*
* The routine sets the logical address filter in the specified
* initialization block memory, <pIb>, such that a new multicast address
* will be accepted as input to the receiver unit.
*
* RETURNS: N/A
*
* NOMANUAL
*/
__inline__ LOCAL void ibAddrFilterSet
    (
    volatile LN_IB * pIb,  /* initialization block base address */
    UINT32           crc   /* CRC filter bit mask */
    )
    {
    ((pIb->lnIBLadrf[(crc & 0x0000003f) >> 3]) |= (1 << (crc & 0x7)));
    }

/******************************************************************************
*
* rmdHasErrors - test a Received Message Descriptor's error bits
*
* RETURNS:
* TRUE if the RMD ERR bit is set or if the associated packet data is not
* contained entirely within the RMD packet buffer.
*
* NOMANUAL
*/
__inline__ LOCAL BOOL rmdHasErrors
    (
    const volatile LN_RMD * pRmd
    )
    {
    ULONG rmd1 = PCI_SWAP (pRmd->lnRMD1);

    return ((rmd1 & RMD1_ERR) ||
        ((rmd1 & (RMD1_STP | RMD1_ENP)) != (RMD1_STP | RMD1_ENP)));
    }

/*******************************************************************************
*
* ln97xTMDGet - get the next transmit message descriptor (TMD)
*
* RETURNS: an available transmit descriptor, otherwise NULL.
*
* NOMANUAL
*/
__inline__ LOCAL LN_TMD * ln97xTMDGet
    (
    LN_97X_DRV_CTRL * pDrvCtrl  /* points to device control structure */
    )
    {
    volatile LN_TMD * pTmd = pDrvCtrl->pTring + pDrvCtrl->tmdIndex;

    LN_CACHE_INVALIDATE (pTmd, TMD_SIZ);

    /* check if this descriptor is owned by the chip or is out of bounds */
    
    if ((LN_TMD_OWNED (pTmd)) ||
        (((pDrvCtrl->tmdIndex + 1) % (pDrvCtrl->tringSize)) ==
           pDrvCtrl->tmdIndexC))
        {
        return (LN_TMD *) NULL;
        }
        
    return (LN_TMD *)(pTmd);
    }

/*******************************************************************************
*
* ln97xFullRMDGet - get the next received message descriptor (RMD)
*
* RETURNS:
* A pointer to the next receive descriptor to process, or NULL if none ready.
*
* NOMANUAL
*/
__inline__ LOCAL LN_RMD * ln97xFullRMDGet
    (
    LN_97X_DRV_CTRL * pDrvCtrl  /* points to DRV_CTRL structure */
    )
    {
    /* form a pointer to the next receive descriptor */

    volatile LN_RMD * pRmd = pDrvCtrl->pRring + pDrvCtrl->rmdIndex;

    LN_CACHE_INVALIDATE (pRmd, RMD_SIZ);

    /* If receive buffer has been released to us, return it */

    return ((LN_RMD_OWNED (pRmd) == 0) ? ((LN_RMD *) pRmd) : ((LN_RMD *) NULL));
    }

/******************************************************************************
*
* ln97xEndLoad - initialize the driver and device
*
* This routine initializes the driver and the device to the operational state.
* All of the device-specific parameters are passed in <initString>, which
* expects a string of the following format:
*
* <unit:devMemAddr:devIoAddr:pciMemBase:vecnum:intLvl:memAdrs
* :memSize:memWidth:csr3b:offset:flags>
*
* This routine can be called in two modes. If it is called with an empty but
* allocated string, it places the name of this device (that is, "lnPci") into 
* the <initString> and returns 0.
*
* If the string is allocated and not empty, the routine attempts to load
* the driver using the values specified in the string.
*
* RETURNS: An END object pointer, or NULL on error, or 0 and the name of the
* device if the <initString> was NULL.
*/
END_OBJ * ln97xEndLoad
    (
    char * initString            /* string to be parse by the driver */
    )
    {
    LN_97X_DRV_CTRL * pDrvCtrl;


    if (initString == NULL)
        {
        return (NULL);
        }

    if (initString [0] == 0)
        {
        bcopy ((char *)LN_97X_DEV_NAME, initString, LN_97X_DEV_NAME_LEN);
        return ((END_OBJ *) OK);
        }


    /* allocate the device structure */

    if ((pDrvCtrl = (LN_97X_DRV_CTRL *) calloc (sizeof (LN_97X_DRV_CTRL), 1))
        == NULL)
        {
        goto errorExit;
        }


    /* parse the init string, filling in the device structure */

    if (ln97xInitParse (pDrvCtrl, initString) == ERROR)
        {
        goto errorExit;
        }

    /* Have the BSP hand us our address. */

    (* ln97xEnetAddrGet) (pDrvCtrl, (char *) &(pDrvCtrl->enetAddr [0]));

    /* initialize the END and MIB2 parts of the structure */

    if (END_OBJ_INIT (&pDrvCtrl->endObj, (DEV_OBJ *)pDrvCtrl, LN_97X_DEV_NAME,
                      pDrvCtrl->unit, &ln97xFuncTable, pDescription) == ERROR
     || END_MIB_INIT (&pDrvCtrl->endObj, M2_ifType_ethernet_csmacd,
                      &pDrvCtrl->enetAddr[0], 6, ETHERMTU, LN_SPEED) == ERROR)
        {
        goto errorExit;
        }


    /* Perform memory allocation */

    if (ln97xMemInit (pDrvCtrl) == ERROR)
        {
        goto errorExit;
        }


    /* Perform memory distribution and reset and reconfigure the device */

    if (ln97xRestartSetup (pDrvCtrl) == ERROR)
        {
        goto errorExit;
        }


    /* set the flags to indicate readiness */

    END_OBJ_READY (&pDrvCtrl->endObj, IFF_NOTRAILERS | IFF_BROADCAST |
                                      IFF_MULTICAST  | IFF_SIMPLEX);

    return (&pDrvCtrl->endObj);

errorExit:
    if (pDrvCtrl != NULL)
        free ((char *)pDrvCtrl);

    return ((END_OBJ *)NULL);
    }

/*******************************************************************************
*
* ln97xInitParse - parse the initialization string
*
* Parse the input string. This routine is called from ln97xEndLoad() which
* intializes some values in the driver control structure with the values
* passed in the intialization string.
*
* The initialization string format is:
*
* <unit:devMemAddr:devIoAddr:pciMemBase:vecNum:intLvl:memAdrs
* :memSize:memWidth:csr3b:offset:flags>
*
* \is
* \i <unit>
* The device unit number.  Unit numbers are integers starting at zero and
* increasing for each device contolled by the driver.
* \i <devMemAddr>
* The device memory mapped I/O register base address.  Device registers
* must be mapped into the host processor address space in order for the
* driver to be functional.  Thus, this is a required parameter.
* \i <devIoAddr>
* Device register base I/O address (obsolete).
* \i <pciMemBase>
* Base address of PCI memory space.
* \i <vecNum>
* Interrupt vector number.
* \i <intLvl>
* Interrupt level.  Generally, this value specifies an interrupt level
* defined for an external interrupt controller.
* \i <memAdrs>
* Memory pool address or NONE.
* \i <memSize>
* Memory pool size or zero.
* \i <memWidth>
* Memory system size, 1, 2, or 4 bytes (optional).
* \i <CSR3>
* Control and Status Register 3 (CSR3) options.
* \i <offset>
* Memory alignment offset.
* \i <flags>
* Device specific flags reserved for future use.
* \ie
*
* RETURNS: OK, or ERROR if any arguments are invalid.
*/
STATUS ln97xInitParse
    (
    LN_97X_DRV_CTRL *   pDrvCtrl,       /* pointer to the control structure */
    char *              initString      /* initialization string */
    )
    {
    char *      tok;
    char *      pHolder = NULL;
    UINT32      devMemAddr;
    UINT32      devIoAddr;


    /* unit number */

    if ((tok = strtok_r (initString, ":", &pHolder)) == NULL)
        return ERROR;

    pDrvCtrl->unit = atoi (tok);
    DRV_LOG (DRV_DEBUG_LOAD, "Unit : %d ...\n", pDrvCtrl->unit, 2, 3, 4, 5, 6);

    /* memory mapped IO address base */

    if ((tok = strtok_r (NULL, ":", &pHolder)) == NULL)
        return ERROR;

    devMemAddr = (UINT32) strtoul (tok, NULL, 16);
    DRV_LOG (DRV_DEBUG_LOAD, "devMemAddr : 0x%X ...\n", devMemAddr,
             2, 3, 4, 5, 6);

    /* Intel IO space address base */

    if ((tok = strtok_r (NULL, ":", &pHolder)) == NULL)
        return ERROR;

    devIoAddr = (UINT32) strtoul (tok, NULL, 16);
    DRV_LOG (DRV_DEBUG_LOAD, "devIoAddr : 0x%X ...\n", devIoAddr,
             2, 3, 4, 5, 6);


    /* always use memory mapped IO if provided, else use IO map */
    
    if ((devMemAddr == NONE) && (devIoAddr == NONE))
        {
        return (ERROR);
        }
    else if (devMemAddr != NONE)
        {
        pDrvCtrl->devAdrs = devMemAddr;
        pDrvCtrl->flags  |= LS_MODE_MEM_IO_MAP;
        }
    else
        {
        pDrvCtrl->devAdrs = devIoAddr;
        }


    /* PCI memory base address as seen from the CPU */
    
    if ((tok = strtok_r (NULL, ":", &pHolder)) == NULL)
        return ERROR;

    pDrvCtrl->pciMemBase = strtoul (tok, NULL, 16);
    DRV_LOG (DRV_DEBUG_LOAD, "Pci : 0x%X ...\n", pDrvCtrl->pciMemBase,
             2, 3, 4, 5, 6);

    /* interrupt vector */

    if ((tok = strtok_r (NULL, ":", &pHolder)) == NULL)
        return ERROR;

    pDrvCtrl->inum = atoi (tok);
    DRV_LOG (DRV_DEBUG_LOAD, "inum : 0x%X ...\n", pDrvCtrl->inum,
             2, 3, 4, 5, 6);

    /* interrupt level */

    if ((tok = strtok_r (NULL, ":", &pHolder)) == NULL)
        return ERROR;

    pDrvCtrl->ilevel = atoi (tok);
    DRV_LOG (DRV_DEBUG_LOAD, "ilevel : 0x%X ...\n", pDrvCtrl->ilevel,
             2, 3, 4, 5, 6);

    /* caller supplied memory address */

    if ((tok = strtok_r (NULL, ":", &pHolder)) == NULL)
        return ERROR;

    pDrvCtrl->memAdrs = (char *)strtoul (tok, NULL, 16);
    DRV_LOG (DRV_DEBUG_LOAD, "memAdrs : 0x%X ...\n", (int)pDrvCtrl->memAdrs,
             2, 3, 4, 5, 6);

    /* caller supplied memory size */

    if ((tok = strtok_r (NULL, ":", &pHolder)) == NULL)
        return ERROR;

    pDrvCtrl->memSize = strtoul (tok, NULL, 16);
    DRV_LOG (DRV_DEBUG_LOAD, "memSize : 0x%X ...\n", pDrvCtrl->memSize,
             2, 3, 4, 5, 6);

    /* caller supplied memory width */

    if ((tok = strtok_r (NULL, ":", &pHolder)) == NULL)
        return ERROR;

    pDrvCtrl->memWidth = atoi (tok);
    DRV_LOG (DRV_DEBUG_LOAD, "memWidth : 0x%X ...\n", pDrvCtrl->memWidth,
             2, 3, 4, 5, 6);

    /* CSR3B value */

    if ((tok = strtok_r (NULL, ":", &pHolder)) == NULL)
        return ERROR;

    pDrvCtrl->csr3B = strtoul (tok, NULL, 16);
    DRV_LOG (DRV_DEBUG_LOAD, "CSR3b : 0x%X ...\n", pDrvCtrl->csr3B,
             2, 3, 4, 5, 6);

    /* caller supplied alignment offset */

    if ((tok = strtok_r (NULL, ":", &pHolder)) == NULL)
        return ERROR;

    pDrvCtrl->offset = atoi (tok);
    DRV_LOG (DRV_DEBUG_LOAD, "Offset : 0x%X ...\n", pDrvCtrl->offset,
             2, 3, 4, 5, 6);

    /* caller supplied flags */

    if ((tok = strtok_r (NULL, ":", &pHolder)) == NULL)
        return ERROR;

    pDrvCtrl->flags |= strtoul (tok, NULL, 16);
    DRV_LOG (DRV_DEBUG_LOAD, "flags : 0x%X ...\n", pDrvCtrl->flags,
             2, 3, 4, 5, 6);

    /* custom device description string */

    if ((tok = strtok_r (NULL, ":", &pHolder)) != NULL)
        {
        pDescription = *((char **)(strtoul (tok, NULL, 16)));
        }

    return OK;
    }

/*******************************************************************************
*
* ln97xMemInit - initialize memory for Lance chip
*
* Using data in the control structure, setup and initialize the memory
* areas needed.  If the memory address is not already specified, then allocate
* cache safe memory.
*
* RETURNS: OK or ERROR.
*/
LOCAL STATUS ln97xMemInit
    (
    LN_97X_DRV_CTRL * pDrvCtrl  /* device to be initialized */
    )
    {
    UINT32            sz;       /* temporary size holder */
    int               ix;
    volatile LN_RMD * pRmd;
    char *            pTempBuf;

    /* Remember register addresses */

    pDrvCtrl->pRdp   = LN_97X_RDP;
    pDrvCtrl->pRap   = LN_97X_RAP;
    pDrvCtrl->pReset = LN_97X_RST;
    pDrvCtrl->pBdp   = LN_97X_BDP;

    /***** Establish size of shared memory region we require *****/

    if ((int) pDrvCtrl->memAdrs != NONE)  /* specified memory pool */
        {
        /*
         * With a specified memory pool we want to maximize
         * lnRsize and lnTsize
         */

        sz = (pDrvCtrl->memSize - (RMD_SIZ + TMD_SIZ + sizeof (LN_IB)))
               / ((2 * LN_BUFSIZ) + RMD_SIZ + TMD_SIZ);

        sz >>= 1;               /* adjust for roundoff */

        for (lnRsize = 0; sz != 0; lnRsize++, sz >>= 1)
            ;

        lnTsize = lnRsize;      /* lnTsize = lnRsize for convenience */
        }

    /* limit ring sizes to reasonable values */

    lnRsize = max (lnRsize, LN_RMD_MIN); /* 4 Rx buffers is reasonable min */
    lnRsize = min (lnRsize, LN_RMD_MAX); /* 512 Rx buffers is max for chip */
    lnTsize = max (lnTsize, LN_TMD_MIN); /* 4 Tx buffers is reasonable min */
    lnTsize = min (lnTsize, LN_TMD_MAX); /* 512 Tx buffers is max for chip */

    /* Add it all up */

    sz = (((1 << lnRsize) + 1) * RMD_SIZ) +
         (((1 << lnTsize) + 1) * TMD_SIZ) + IB_SIZ + 0x10;

    /***** Establish a region of shared memory *****/

    /* OK. We now know how much shared memory we need.  If the caller
     * provides a specific memory region, we check to see if the provided
     * region is large enough for our needs.  If the caller did not
     * provide a specific region, then we attempt to allocate the memory
     * from the system, using the cache aware allocation system call.
     */

    switch ((int) pDrvCtrl->memAdrs)
        {
	default :       /* caller provided memory */
	    if (pDrvCtrl->memSize < sz)     /* not enough space */
		{
		DRV_LOG (DRV_DEBUG_LOAD, "ln97x: not enough memory provided\n"
			 "ln97x: need %ul got %d\n",
			 sz, pDrvCtrl->memSize, 3, 4, 5, 6);
		return (ERROR);
                }

	    /* set the beginning of pool */

	    pDrvCtrl->pShMem = pDrvCtrl->memAdrs;

	    /* assume pool is cache coherent, copy null structure */

	    pDrvCtrl->cacheFuncs = cacheNullFuncs;
	    break;

        case NONE :     /* get our own memory */

	    /* Because the structures that are shared between the device
	     * and the driver may share cache lines, the possibility exists
	     * that the driver could flush a cache line for a structure and
	     * wipe out an asynchronous change by the device to a neighboring
	     * structure. Therefore, this driver cannot operate with memory
	     * that is not write coherent.  We check for the availability of
	     * such memory here, and abort if the system did not give us what
	     * we need.
	     */

	    if (!CACHE_DMA_IS_WRITE_COHERENT ())
		{
		printf ( "ln97x: device requires cache coherent memory\n" );
		return (ERROR);
		}

	    pDrvCtrl->pShMem = (char *) cacheDmaMalloc (sz);

	    if (pDrvCtrl->pShMem == NULL)
		{
		printf ( "ln97x: system memory unavailable\n" );
		return (ERROR);
		}

	    /* copy the DMA structure */

	    pDrvCtrl->cacheFuncs = cacheDmaFuncs;

            break;
        }

    /*                        Turkey Carving
     *                        --------------
     *
     *                          LOW MEMORY
     *
     *             |-------------------------------------|
     *             |       The initialization block      |
     *             |         (sizeof (LN_IB))            |
     *             |-------------------------------------|
     *             |         The Rx descriptors          |
     *             | (1 << lnRsize) * sizeof (LN_RMD)    |
     *             |-------------------------------------|
     *             |         The Tx descriptors          |
     *             | (1 << lnTsize) * sizeof (LN_TMD)    |
     *             |-------------------------------------|
     */

    /* Save some things */

    pDrvCtrl->memBase = (char *)((UINT32) pDrvCtrl->pShMem & 0xff000000);

    if ((int) pDrvCtrl->memAdrs == NONE)
        pDrvCtrl->flags |= LS_MEM_ALLOC_FLAG;

    /* first let's clear memory */

    bzero ((char *) pDrvCtrl->pShMem, (int) sz);


    /* initialize a 16-byte aligned RMD ring base address */

    pDrvCtrl->pRring    = (LN_RMD *) ((int) pDrvCtrl->pShMem + IB_SIZ);
    pDrvCtrl->pRring    = (LN_RMD *) (((int) pDrvCtrl->pRring + 0xf) & ~0xf);

    pDrvCtrl->rringLen  = lnRsize;
    pDrvCtrl->rringSize = 1 << lnRsize;
    pDrvCtrl->rmdIndex  = 0;

    /* initialize a 16-byte aligned TMD ring base address */

    pDrvCtrl->pTring = (LN_TMD *) ((int) pDrvCtrl->pShMem + IB_SIZ +
                       ((1 << lnRsize) + 1) * RMD_SIZ + 0xf);
    pDrvCtrl->pTring = (LN_TMD *) (((int) pDrvCtrl->pTring + 0xf) & ~0xf);

    pDrvCtrl->tringLen  = lnTsize;
    pDrvCtrl->tringSize = 1 << lnTsize;
    pDrvCtrl->tmdIndex  = 0;
    pDrvCtrl->tmdIndexC = 0;


    /* allocate pool structure for mblks, clBlk, and clusters */

    if ((pDrvCtrl->endObj.pNetPool = malloc (sizeof (NET_POOL))) == NULL)
	return (ERROR);

    
    pDrvCtrl->clDesc.clNum    = pDrvCtrl->rringSize * 2;
    pDrvCtrl->mClCfg.clBlkNum = pDrvCtrl->clDesc.clNum;
    pDrvCtrl->mClCfg.mBlkNum  = pDrvCtrl->mClCfg.clBlkNum * 2;

    /* total memory size for mBlks and clBlks */
    
    pDrvCtrl->mClCfg.memSize =
        (pDrvCtrl->mClCfg.mBlkNum  * (MSIZE + sizeof (long))) +
        (pDrvCtrl->mClCfg.clBlkNum * (CL_BLK_SZ + sizeof (long)));

    /* total memory for mBlks and clBlks */

    if ((pDrvCtrl->mClCfg.memArea = (char *) memalign
        (sizeof (long), pDrvCtrl->mClCfg.memSize)) == NULL)
        {
        return (ERROR);
        }

    /* total memory size for all clusters */

    pDrvCtrl->clDesc.clSize  = LN_BUFSIZ;
    pDrvCtrl->clDesc.memSize =
        (pDrvCtrl->clDesc.clNum * (pDrvCtrl->clDesc.clSize + 8)) + sizeof (int);

    /* Do we hand over our own memory? */

    if (pDrvCtrl->memAdrs != (char *) NONE)
        {
        pDrvCtrl->clDesc.memArea =
            (char *)(pDrvCtrl->pTring + pDrvCtrl->tringSize);
        }
    else
        {
        pDrvCtrl->clDesc.memArea = cacheDmaMalloc (pDrvCtrl->clDesc.memSize);
            
        if (pDrvCtrl->clDesc.memArea == NULL)
            {
            return (ERROR);
            }
        }

    /* initialize the device net pool */
    
    if (netPoolInit (pDrvCtrl->endObj.pNetPool, &pDrvCtrl->mClCfg,
                     &pDrvCtrl->clDesc, 1, NULL) == ERROR)
        {
        return (ERROR);
        }
    
    /* Store the cluster pool ID as others need it later. */

    pDrvCtrl->pClPoolId = clPoolIdGet (pDrvCtrl->endObj.pNetPool,
                                       LN_BUFSIZ, FALSE);


    pRmd = pDrvCtrl->pRring;

    for (ix = 0; ix < pDrvCtrl->rringSize; ++ix, ++pRmd)
        {
        if ((pTempBuf = netClusterGet
            (pDrvCtrl->endObj.pNetPool, pDrvCtrl->pClPoolId)) == NULL)
            {
            return (ERROR);
            }

        pTempBuf += pDrvCtrl->offset;
        rmdBuffAddrSet (pDrvCtrl, pRmd, pTempBuf);
        }

    return OK;
    }

/*******************************************************************************
*
* ln97xStart - start the device
*
* This function calls BSP functions to connect interrupts and start the
* device running in interrupt mode.
*
* RETURNS: OK or ERROR if the interrupt handler could not be connected.
*/
LOCAL STATUS ln97xStart
    (
    LN_97X_DRV_CTRL * pDrvCtrl  /* device to be initialized */
    )
    {
    STATUS        result = ERROR;

    pDrvCtrl->txCleaning = FALSE;
    pDrvCtrl->txBlocked  = FALSE;

    result = (* ln97xIntConnect)((VOIDFUNCPTR *) INUM_TO_IVEC(pDrvCtrl->inum),
                                 ln97xInt, (int)(pDrvCtrl));

    if (result == OK)
        {
        /* mark the interface as up */

        END_FLAGS_SET (&pDrvCtrl->endObj, (IFF_UP | IFF_RUNNING));

        (* ln97xIntEnable) (pDrvCtrl->ilevel);

        /* Enable interrupts on the device-side */

        csrWrite (pDrvCtrl, CSR(0), CSR0_INEA);

        DRV_LOG (DRV_DEBUG_LOAD, "interrupt enabled.\n", 0, 0, 0, 0, 0, 0);
        }

    return (result);
    }

/*******************************************************************************
*
* ln97xHandleError - handle controller interrupt errors
*
* This routine performs task level error handling for controller errors
* detected in interrupt context.
*
* RETURNS: N/A.
*/
LOCAL void ln97xHandleError
    (
    LN_97X_DRV_CTRL * pDrvCtrl,   /* pointer to DRV_CTRL structure */
    UINT32            csr0Errors  /* holds error bits read from CSR0 */
    )
    {
    pDrvCtrl->errorHandling = FALSE;

    if (csr0Errors & CSR0_MERR)
        {
        pDrvCtrl->lastError.errCode = END_ERR_WARN;
        pDrvCtrl->lastError.pMesg   = "Bus master REQ timeout";
        muxError (&pDrvCtrl->endObj, &pDrvCtrl->lastError);
        }

    if (csr0Errors & CSR0_BABL)
        {
        END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_ERRS, +1);

        pDrvCtrl->lastError.errCode = END_ERR_WARN;
        pDrvCtrl->lastError.pMesg   = "Babbling";
        muxError (&pDrvCtrl->endObj, &pDrvCtrl->lastError);
        }

    if (csr0Errors & CSR0_MISS)
        {
        END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);

        pDrvCtrl->lastError.errCode = END_ERR_WARN;
        pDrvCtrl->lastError.pMesg   = "Missing";
        muxError (&pDrvCtrl->endObj, &pDrvCtrl->lastError);
        }
    }

/*******************************************************************************
*
* ln97xInt - handle controller interrupt
*
* This routine is called at interrupt level in response to an interrupt from
* the controller.
*
* RETURNS: N/A
*/
LOCAL void ln97xInt
    (
    LN_97X_DRV_CTRL * pDrvCtrl  /* pointer to DRV_CTRL structure */
    )
    {
    /* assume no errors */

    UINT32 errs = 0;

    /* Read the device status register */

    UINT32 stat = csrLockedRead (pDrvCtrl, CSR(0));


    /* If false interrupt, return. */

    if (! (stat & CSR0_INTR))
        {
        return;
        }


    /* clear interrupt status, error status, and interrupt enable bits */

    csrLockedWrite (pDrvCtrl, CSR(0), stat & (~CSR0_INEA));


    /* check for any one, or a combination, of (3) error types */

    errs = (stat & (CSR0_BABL | CSR0_MISS | CSR0_MERR));

    if ((errs) && !(pDrvCtrl->errorHandling))
        {
        /* put a task level error handler on the netTask job queue */

        if (netJobAdd ((FUNCPTR) ln97xHandleError,
           (int) pDrvCtrl, (int) errs, 0, 0, 0) == OK)
            {
            pDrvCtrl->errorHandling = TRUE;
            }
        }

    /* process received frame descriptors */

    if ((stat & CSR0_RINT) && !(pDrvCtrl->flags & LS_RCV_HANDLING_FLAG))
        {
        /* put a task level input packet handler on the netTask job queue */

        if (netJobAdd ((FUNCPTR) ln97xHandleRecvInt,
           (int) pDrvCtrl, 0,0,0,0) == OK)
            {
            pDrvCtrl->flags |= LS_RCV_HANDLING_FLAG;
            }
        }

    /* process transmitted frame descriptors */

    if ((stat & CSR0_TINT) && !(pDrvCtrl->txCleaning))
        {
        if (netJobAdd ((FUNCPTR) ln97xTRingScrub,
           (int) pDrvCtrl, 0, 0, 0, 0) == OK)
            {
            pDrvCtrl->txCleaning = TRUE;
            }
        }

    if (pDrvCtrl->txBlocked)    /* cause a MUX restart */
        {
        if (netJobAdd ((FUNCPTR) muxTxRestart,
           (int) &pDrvCtrl->endObj, 0, 0, 0, 0) == OK)
            {
            pDrvCtrl->txBlocked = FALSE;
            }
        }


    /* reenable device-side interrupts */

    csrWrite (pDrvCtrl, CSR(0), CSR0_INEA);

    /* Flush the write pipe */

    CACHE_PIPE_FLUSH ();
    }

/*******************************************************************************
*
* ln97xHandleRecvInt - task level interrupt service for input packets
*
* This routine is called at task level indirectly by the interrupt
* service routine to do any message received processing.
*/
LOCAL void ln97xHandleRecvInt
    (
    LN_97X_DRV_CTRL * pDrvCtrl  /* points to DRV_CTRL structure */
    )
    {
    LN_RMD *          pRmd = (LN_RMD *) NULL;

    do
        {
        pDrvCtrl->flags |= LS_RCV_HANDLING_FLAG;

        while ((pRmd = ln97xFullRMDGet (pDrvCtrl)) != (LN_RMD *) NULL) 
            {
            ln97xRecv (pDrvCtrl, pRmd);
            }

        /*
         * There is a RACE right here.  The ISR could add a receive packet
         * and check the boolean below, and decide to exit.  Thus the
         * packet could be dropped if we don't double check before we
         * return.
         */

        pDrvCtrl->flags &= ~LS_RCV_HANDLING_FLAG;
        }
    while (ln97xFullRMDGet (pDrvCtrl) != NULL);
    /* this double check solves the RACE */
    }

/*******************************************************************************
*
* ln97xRecv - process the next incoming packet
*
* RETURNS: OK or ERROR.
*/
LOCAL STATUS ln97xRecv
    (
    LN_97X_DRV_CTRL * pDrvCtrl,  /* points to device control structure */
    LN_RMD *          pRmd
    )
    {
    int               len;
    CL_BLK_ID         pClBlk;
    M_BLK_ID          pMblk;

    char *            pCluster;
    char *            pNewCluster;

    /* alias frequently referenced device control fields */

    const int         offset   = pDrvCtrl->offset;
    END_OBJ * const   pEndObj  = &(pDrvCtrl->endObj);
    NET_POOL_ID const pNetPool = pEndObj->pNetPool;


    DRV_LOG (DRV_DEBUG_RX, "Recv : index %d rmd = 0x%X\n",
             pDrvCtrl->rmdIndex, (int) pRmd, 3, 4, 5, 6);


    /* If error flag OR if packet is not completely in one buffer */

    if  (rmdHasErrors (pRmd))
        {
        END_ERR_ADD (pEndObj, MIB2_IN_ERRS, +1);
        goto cleanRXD;
        }

    /* If we cannot get a buffer to loan then bail out */

    if ((pNewCluster = netClusterGet (pNetPool, pDrvCtrl->pClPoolId)) == NULL)
        {
#if defined(END_ERR_NO_BUF)
        pDrvCtrl->lastError.errCode = END_ERR_NO_BUF;
        muxError (pEndObj, &pDrvCtrl->lastError);
#endif  /* defined(END_ERR_NO_BUF) */
        END_ERR_ADD (pEndObj, MIB2_IN_ERRS, +1);
        goto cleanRXD;
        }

    if ((pClBlk = netClBlkGet (pNetPool, M_DONTWAIT)) == NULL)
        {
        netClFree (pNetPool, (UCHAR *) pNewCluster);

        END_ERR_ADD (pEndObj, MIB2_IN_ERRS, +1);
        goto cleanRXD;
        }

    /* get an <mBlk> and join it to the cluster from the RMD */

    if ((pMblk = mBlkGet (pNetPool, M_DONTWAIT, MT_DATA)) == NULL)
        {
        netClBlkFree (pNetPool, pClBlk);
        netClFree (pNetPool, (UCHAR *) pNewCluster);

        END_ERR_ADD (pEndObj, MIB2_IN_ERRS, +1);
        goto cleanRXD;
        }

    /* get the packet length & a pointer to the packet */

    len = LN_PKT_LEN_GET (pRmd);

    pCluster  = rmdBuffAddrGet (pDrvCtrl, pRmd);
    pCluster -= offset;

    END_ERR_ADD (pEndObj, MIB2_IN_UCAST, +1);

    /* Join the cluster to the MBlock */

    netClBlkJoin (pClBlk, pCluster, len, NULL, 0, 0, 0);
    netMblkClJoin (pMblk, pClBlk);

    /* make the packet data coherent */

    LN_CACHE_INVALIDATE (pMblk->mBlkHdr.mData, len);

    pMblk->mBlkHdr.mData  += offset;
    pMblk->mBlkHdr.mLen    = len;
    pMblk->mBlkHdr.mFlags |= M_PKTHDR;
    pMblk->mBlkPktHdr.len  = len;

    /* Give receiver a new buffer */

    pNewCluster += offset;
    rmdBuffAddrSet (pDrvCtrl, pRmd, pNewCluster);

    /* Call the upper layer's receive routine */

    END_RCV_RTN_CALL (pEndObj, pMblk);

cleanRXD:

    /* return a clean descriptor to the controller */

    rxDescClean (pRmd);

    CACHE_PIPE_FLUSH ();

    /* increment index to RMD ring */

    pDrvCtrl->rmdIndex = ((pDrvCtrl->rmdIndex + 1) % (pDrvCtrl->rringSize));

    return (OK);
    }

/*******************************************************************************
*
* ln97xSend - the driver send routine
*
* This routine takes a M_BLK_ID sends off the data in the M_BLK_ID.
* The buffer must already have the addressing information properly installed
* in it.  This is done by a higher layer.
*
* CAVEATS
* Assumes that the TMD returned from ln97xTMDGet() has been "cleaned".
* Assumes that the TMD will not be inadvertently cleaned while the
* TMD is being constructed and before the controller has been given
* ownersip of the TMD.
*
* RETURNS: OK or ERROR.
*/
LOCAL STATUS ln97xSend
    (
    LN_97X_DRV_CTRL * pDrvCtrl,  /* device to be initialized */
    M_BLK_ID          pMblk      /* data to send */
    )
    {
    volatile LN_TMD * pTmd;
    UINT32            ltmd1;
    char *            pBuf;
    int               len = 0;

    /* check device mode */

    if (pDrvCtrl->flags & LS_POLLING)
        {
        netMblkClChainFree (pMblk);
        errno = EINVAL;

        return (ERROR);
        }

    /*
     * Obtain exclusive access to transmitter.  This is necessary because
     * we might have more than one stack transmitting at once.
     */

    END_TX_SEM_TAKE (&pDrvCtrl->endObj, WAIT_FOREVER);

    pTmd = ln97xTMDGet (pDrvCtrl);

    DRV_LOG (DRV_DEBUG_TX, "Send : index %d tmd = 0x%X\n",
             pDrvCtrl->tmdIndex, (int) pTmd, 3, 4, 5, 6);

    if (pTmd == NULL)
        {
        pDrvCtrl->txBlocked = TRUE;

        END_TX_SEM_GIVE (&pDrvCtrl->endObj);

        return (END_ERR_BLOCK);
        }

    if ((pBuf = netClusterGet
        (pDrvCtrl->endObj.pNetPool, pDrvCtrl->pClPoolId)) == NULL)
        {
        pDrvCtrl->txBlocked = TRUE;

        END_TX_SEM_GIVE (&pDrvCtrl->endObj);

        return (END_ERR_BLOCK);
        }

    pBuf += pDrvCtrl->offset;
    len = netMblkToBufCopy (pMblk, pBuf, NULL);

    /* increment the unicast output counter */

    END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_UCAST, +1);

    netMblkClChainFree (pMblk);

    tmdBuffAddrSet (pDrvCtrl, pTmd, pBuf);

    ltmd1 = (TMD1_BCNT_MSK & -(max (len, ETHERSMALL))) |
            (TMD1_OWN | TMD1_CNST | TMD1_STP | TMD1_ENP);


    /* increment index to TMD ring */

    pDrvCtrl->tmdIndex = ((pDrvCtrl->tmdIndex + 1) % (pDrvCtrl->tringSize));

    /* write transmit descriptor (with OWN-bit set) to actual register */

    pTmd->lnTMD1 = PCI_SWAP (ltmd1);

    CACHE_PIPE_FLUSH ();


    if (lnKickStartTx)
        {
        csrLockedWrite (pDrvCtrl, CSR(0), (CSR0_INEA | CSR0_TDMD));
        }

    END_TX_SEM_GIVE (&pDrvCtrl->endObj);

    return (OK);
    }

/*******************************************************************************
*
* ln97xIoctl - the driver I/O control routine
*
* Process an ioctl request.
*
* RETURNS OK or ERROR value
*/
LOCAL int ln97xIoctl
    (
    LN_97X_DRV_CTRL * pDrvCtrl, /* device to be initialized */
    int               cmd,      /* ioctl command to execute */
    caddr_t           data      /* data to get or set */
    )
    {
    int    error = 0;

    switch ((UINT)cmd)
        {
        case EIOCSADDR:
            if (data == NULL)
                return (EINVAL);

            bcopy ((char *)data, (char *)END_HADDR(&pDrvCtrl->endObj),
                   END_HADDR_LEN (&pDrvCtrl->endObj));
            break;

        case EIOCGADDR:
            if (data == NULL)
                return (EINVAL);

            bcopy ((char *)END_HADDR (&pDrvCtrl->endObj), (char *)data,
                   END_HADDR_LEN (&pDrvCtrl->endObj));
            break;

        case EIOCSFLAGS:
            {
            long value    = (long) data;
            long oldFlags = END_FLAGS_GET (&pDrvCtrl->endObj);
            long newFlags;

            if (value < 0)
                {
                value = -value;
                --value;
                END_FLAGS_CLR (&pDrvCtrl->endObj, value);
                }
            else
                {
                END_FLAGS_SET (&pDrvCtrl->endObj, value);
                }

            /* Handle IFF_XXX flags set or cleared by command */
              
            newFlags = END_FLAGS_GET (&pDrvCtrl->endObj);

            /* IFF_ALLMULTI */

            if ((newFlags & IFF_ALLMULTI) && !(oldFlags & IFF_ALLMULTI))
                {
                ln97xAddrFilterMod (pDrvCtrl, (UINT8 *)NULL, LN_LADRF_ADD_ALL);
                }
            else if ((oldFlags & IFF_ALLMULTI) && !(newFlags & IFF_ALLMULTI))
                {
                ln97xAddrFilterMod (pDrvCtrl, (UINT8 *)NULL, LN_LADRF_DEL_ALL);
                }

            /* IFF_PROMISC */
              
            if ((newFlags & IFF_PROMISC) && !(oldFlags & IFF_PROMISC))
                {
                UINT32 spndKey = suspendModeEnter (pDrvCtrl);

                csrWrite (pDrvCtrl, CSR(15), CSR15_PROM);
                pDrvCtrl->flags |= LS_PROMISCUOUS_FLAG; 

                suspendModeLeave (pDrvCtrl, spndKey);
                }
            else if ((oldFlags & IFF_PROMISC) && !(newFlags & IFF_PROMISC))
                {
                UINT32 spndKey = suspendModeEnter (pDrvCtrl);

                csrWrite (pDrvCtrl, CSR(15), 0);
                pDrvCtrl->flags &= ~LS_PROMISCUOUS_FLAG; 

                suspendModeLeave (pDrvCtrl, spndKey);
                }
            }
            break;

        case EIOCGFLAGS:
            *(int *)data = END_FLAGS_GET (&pDrvCtrl->endObj);
            break;

        case EIOCMULTIADD:
            error = ln97xMCastAddrAdd (pDrvCtrl, (char *) data);
            break;

        case EIOCMULTIDEL:
            error = ln97xMCastAddrDel (pDrvCtrl, (char *) data);
            break;

        case EIOCMULTIGET:
            error = ln97xMCastAddrGet (pDrvCtrl, (MULTI_TABLE *) data);
            break;

        case EIOCPOLLSTART:
            error = ln97xPollStart (pDrvCtrl);
            break;

        case EIOCPOLLSTOP:
            error = ln97xPollStop (pDrvCtrl);
            break;

        case EIOCGMIB2:
            if (data == NULL)
                return (EINVAL);
            bcopy((char *)&pDrvCtrl->endObj.mib2Tbl, (char *)data,
                  sizeof(pDrvCtrl->endObj.mib2Tbl));
            break;

        case EIOCGFBUF:
            if (data == NULL)
                return (EINVAL);
            *(int *)data = LN_MIN_FBUF;
            break;

        case EIOCGMWIDTH:
            if (data == NULL)
                return (EINVAL);
            *(int *)data = pDrvCtrl->memWidth;
            break;

        case EIOCGHDRLEN:
            if (data == NULL)
                return (EINVAL);
            *(int *)data = SIZEOF_ETHERHEADER;
            break;

        default:
            error = EINVAL;
        }

    return (error);
    }

/*******************************************************************************
*
* ln97xReset - hardware reset of chip (stop it)
*
* This routine is responsible for resetting the device and switching into
* 32 bit mode.  This routine will return with the STOP bit set.
*
* Note that all transmit and receive descriptor OWN bits will be cleared
* and descriptor ring pointers will be reset in this routine.
*
* INTERNAL
* Note that CSR0 bits (except STOP) are all effectively cleared by setting
* the STOP bit.
*
* The Bus Timeout register (CSR100) is no longer explicitly set.  The CSR0
* memory error bit (MERR) is set by the controller when it requests the use
* of the bus by asserting REQ and GNT has not been asserted after a
* programmable length of time.  The length of time in microseconds before
* MERR is asserted will depend upon the setting of CSR100.
*
* The value of CSR100 is given in 0.1 us increments.  For example, the
* value 0600h (1536 decimal) will cause a MERR to be indicated after 153.6
* us of bus latency.  A value of zero will allow infinitely long bus
* latency.
*
* As of PCnet-PCI II (Am79C970A), the default value of CSR100 was changed
* to 0600h (153.6 us) from 0200h (51.2 us) to adjust to the larger FIFO.
* In a seemingly arbitrary choice, the original WRS driver adjusted CSR100
* to 01000h (409.6 us) to "allow other stuff to hog the bus a bit more".
* This bit of hackery has been removed for the time being.  Moreover, the
* logic to restart the device in the case of MERR assertion has been
* removed.  The LANCE did require a restart in the case of MERR, but the
* PCI PCnet devices do not.
*
* An inspection of the documentation for each device suggests that the LED
* functions in supported devices have suitable default programming.  The
* LED programming code has been removed in favor of the device defaults:
*
*     Register   Description    Default Value
*     ---------------------------------------
*     BCR4       LED 0 Status     0x00C0
*     BCR5       LED 1 Status     0x0084
*     BCR6       LED 2 Status     0x0088
*     BCR7       LED 3 Status     0x0090
*
* RETURNS: OK or ERROR.
*/
LOCAL STATUS ln97xReset
    (
    LN_97X_DRV_CTRL * pDrvCtrl  /* device to be initialized */
    )
    {
    UINT32            resetTmp;

    int               ix;
    const int         offs = pDrvCtrl->offset;

    volatile LN_RMD * pRmd = pDrvCtrl->pRring;
    volatile LN_TMD * pTmd = pDrvCtrl->pTring;

    /* terminate pending bus master requests in an orderly sequence */

    csrLockedWrite (pDrvCtrl, CSR(0), CSR0_STOP);

    /* Generate a soft-reset of the controller */

    SYS_IN_LONG (pDrvCtrl, pDrvCtrl->pReset, resetTmp);

    /* prevent the compiler from optimizing out SYS_IN_LONG() above */

    if (resetTmp == 0x12345678)
        return (ERROR);

    /* give the hardware some time to perform its reset */

    taskDelay (sysClkRateGet() / 4);

    /* Enable 32 bit access by doing a 32 bit write */

    SYS_OUT_LONG (pDrvCtrl, pDrvCtrl->pRdp, 0);

    /* set the bus mode */

    csrWrite (pDrvCtrl, CSR(3), pDrvCtrl->csr3B | CSR3_DXSUFLO);

    /* identify the controller via chip ID registers */

    pDrvCtrl->chipId = chipIdMake (pDrvCtrl);

    /* enable "software style 3" so we can use 32-bit structs & bursting */

    bcrWrite (pDrvCtrl, BCR(20), BCR20_SWSTYLE_PCNET_II);

    /* disable DMA burst xfer counter in CSR80 - AMD suggested for PCI */

    csrWrite (pDrvCtrl, CSR(4), CSR4_DMAPLUS | CSR4_TFC_MASK);

    /* enable burst read and write operatations - AMD suggested for PCI */

    bcrWrite (pDrvCtrl, BCR(18), BCR18_BSBC_MASK);

    /* Autoselect port tye - 10BT or AUI and configure interrupt pin */

    if (ln97xInterruptLevel == LN_ACTIVE_LOW)
        bcrWrite (pDrvCtrl, BCR(2),  BCR2_AUTO_SELECT | BCR2_ACTIVE_LOW);
    else
        bcrWrite (pDrvCtrl, BCR(2), (BCR2_AUTO_SELECT | BCR2_ACTIVE_HIGH));

    /* read BCR */

    resetTmp = bcrRead (pDrvCtrl, BCR(20));



    /* reset software indeces to transmit & receive descriptor rings */

    pDrvCtrl->rmdIndex  = 0;
    pDrvCtrl->tmdIndex  = 0;
    pDrvCtrl->tmdIndexC = 0;

    for (ix = 0; ix < pDrvCtrl->rringSize; ++ix, ++pRmd)
        {
        rxDescClean (pRmd);
        }

    for (ix = 0; ix < pDrvCtrl->tringSize; ++ix, ++pTmd)
        {
        /* Are there associated send-clusters to clean up ? */

        if (pTmd->lnTMD0 != 0)
            {
            char * pCluster = tmdBuffAddrGet (pDrvCtrl, pTmd);

            netClFree (pDrvCtrl->endObj.pNetPool,
                      (UCHAR *)(pCluster - offs));

            pTmd->lnTMD0 = 0;
            }

        txDescClean (pTmd);
        }


    return (OK);
    }

/*******************************************************************************
*
* ln97xRestartSetup - setup memory descriptors and turn on chip
*
* This routine initializes all the shared memory structures and turns on
* the chip.
*
* RETURNS OK or ERROR.
*/
LOCAL STATUS ln97xRestartSetup
    (
    LN_97X_DRV_CTRL * pDrvCtrl  /* device to be initialized */
    )
    {
    UINT16            stat;
    void *            pTemp;
    int               timeoutCount = 0;

    UINT32            ibModeBits;


    /* reset the device (sets the STOP bit) */

    ln97xReset (pDrvCtrl);


    /* setup the initialization block */

    pDrvCtrl->ib = (LN_IB *) pDrvCtrl->pShMem;


    /* make sure that the transmitter & receiver will be enabled */

    ibModeBits = PCI_SWAP(pDrvCtrl->ib->lnIBMode);
    ibModeBits &= ~(CSR15_DTX | CSR15_DRX);


    /* set promiscuous mode if it's asked for */

    if (END_FLAGS_GET (&pDrvCtrl->endObj) & IFF_PROMISC)
        {
        ibModeBits      |= CSR15_PROM;  /* SPR 27827: CSR15, bit 15 */
        pDrvCtrl->flags |= LS_PROMISCUOUS_FLAG; 
        }
    else
        {
        ibModeBits      &= ~CSR15_PROM;
        pDrvCtrl->flags &= ~LS_PROMISCUOUS_FLAG; 
        }

    pDrvCtrl->ib->lnIBMode = PCI_SWAP(ibModeBits);


    /* set up address filter for multicasting */

    if (END_MULTI_LST_CNT (&pDrvCtrl->endObj) > 0)
        {
        ln97xAddrFilterSet (pDrvCtrl);
        }


    /* ethernet addr need to be byte-swapped in the initialization block? */

    bcopy ((char *) END_HADDR(&pDrvCtrl->endObj),
           (char *) pDrvCtrl->ib->lnIBPadr, 6);

    /* This is specific to a hardware bug in MIPS bdmr4120 */

    if (ln97xBusEndianess != LN_LITTLE_ENDIAN)
        {
        UCHAR temp1 = pDrvCtrl->ib->lnIBPadr[0];
        UCHAR temp2 = pDrvCtrl->ib->lnIBPadr[1];
        pDrvCtrl->ib->lnIBPadr[0] = pDrvCtrl->ib->lnIBPadr[3];
        pDrvCtrl->ib->lnIBPadr[1] = pDrvCtrl->ib->lnIBPadr[2];
        pDrvCtrl->ib->lnIBPadr[2] = temp2;
        pDrvCtrl->ib->lnIBPadr[3] = temp1;
        }

    /* point to Rx ring */

    ibRxRingAddrSet (pDrvCtrl);

    /* point to Tx ring */

    ibTxRingAddrSet (pDrvCtrl);


    CACHE_PIPE_FLUSH ();



    /* store initialization block physical address in controller regs */

    pTemp = LN_CACHE_VIRT_TO_PHYS (pDrvCtrl->ib);
    pTemp = (void *)(MEM_TO_PCI_PHYS((ULONG)pTemp));

    csrWrite (pDrvCtrl, CSR(2), (UINT32)(pTemp) >> 16);
    csrWrite (pDrvCtrl, CSR(1), (UINT32) pTemp);

    /* initialize the chip (read IB) */

    csrWrite (pDrvCtrl, CSR(0), CSR0_INIT);

    /* hang until Initialization DONe, ERRor, or timeout */

    while (((stat = csrRead (pDrvCtrl, 0)) & (CSR0_IDON | CSR0_ERR)) == 0)
        {
        if (timeoutCount++ > 0x100)
            break;

        taskDelay (2 * timeoutCount);
        }

    /* chip initialization failure? */

    if (((stat & CSR0_ERR) == CSR0_ERR) || ((stat & CSR0_IDON) != CSR0_IDON))
        {
        return (ERROR);
        }


    /* sets the STRT bit and clears the IDON bit */

    csrWrite (pDrvCtrl, CSR(0), CSR0_STRT | CSR0_IDON);


    return (OK);
    }

/*******************************************************************************
*
* ln97xRestart - restart the device after a fatal error
*
* This routine takes care of all the messy details of a restart.  The device
* is reset and re-initialized.  The driver state is re-synchronized.
*
* RETURNS: N/A
*/
LOCAL void ln97xRestart
    (
    LN_97X_DRV_CTRL * pDrvCtrl  /* device to be initialized */
    )
    {
    if (ln97xRestartSetup (pDrvCtrl) == OK)
        {
        /* set the flags to indicate readiness */

        END_OBJ_READY (&pDrvCtrl->endObj, IFF_UP | IFF_RUNNING |
                       IFF_NOTRAILERS | IFF_BROADCAST | IFF_MULTICAST);
        }
    }

/******************************************************************************
*
* ln97xAddrFilterMod - modify the logical address filter registers
*
* This routine modifies the logical address filter registers for one
* logical address.  Assume the specified address, <pAddr>, is a valid
* logical address - that is, the lest significant bit of <pAddr> is
* set.  When the <command> parameter is set to LN_LADRF_ADD, the logical
* address filter will be modified to accept packets from the specified
* <pAddr>.  The LN_LADRF_DEL <command> indicates that the logical address
* filter should be modified such that packets from the specified <pAddr>
* are not accepted.
*
* The <command> parameters LN_LADRF_ADD_ALL and LN_LADRF_DEL_ALL can
* be used to set the filter for all or no logical addresses respectively.
* In this case, the <pAddr> parameter may be set to NULL, as it will not
* be used.  These functions help the driver support an ioctl(EIOCSFLAGS)
* command that sets or clears the IFF_ALLMULTI flag.
*
* RETURNS: OK, else ERROR.
*
* SEE ALSO:  ln97xAddrFilterSet()
*/
LOCAL STATUS ln97xAddrFilterMod
    (
    LN_97X_DRV_CTRL * pDrvCtrl,  /* pointer to DRV_CTRL structure */
    const UINT8 *     pAddr,     /* logical address to add or delete */
    FILTER_MOD_CMD    command    /* specifies "add" or "delete" functions */
    )
    {
    ADDR_FILTER filter;             /* temporary logical address filter */
    UINT8       mcast [LN_LA_LEN];  /* temporary logical address */

    int         byte;               /* index logical address byte array */
    int         bit;                /* index logical address bit array */

    int         intermediate;       /* store CRC intermediate remainder */
    UINT32      crc = 0xffffffff;   /* initial CRC remainder */


    /* edit LADRF by first entering suspend mode */

    UINT32 spndKey = suspendModeEnter (pDrvCtrl);


    /* device receives all multicast packets? */

    if (command == LN_LADRF_ADD_ALL)
        {
        /* set all LADRF to 0xffff */

        csrWrite (pDrvCtrl,  CSR(8),  CSR8_LADRF0);
        csrWrite (pDrvCtrl,  CSR(9),  CSR9_LADRF1);
        csrWrite (pDrvCtrl, CSR(10), CSR10_LADRF2);
        csrWrite (pDrvCtrl, CSR(11), CSR11_LADRF3);

        /* leave suspend mode */

        suspendModeLeave (pDrvCtrl, spndKey);

        return (OK);
        }
    else if (command == LN_LADRF_DEL_ALL)
        {
        /* set all LADRF to 0x0000 */

        csrWrite (pDrvCtrl,  CSR(8), 0);
        csrWrite (pDrvCtrl,  CSR(9), 0);
        csrWrite (pDrvCtrl, CSR(10), 0);
        csrWrite (pDrvCtrl, CSR(11), 0);

        /* leave suspend mode */

        suspendModeLeave (pDrvCtrl, spndKey);

        return (OK);
        }


    /* copy the specified address, as we'll destroy it with shifts below */

    mcast[0] = pAddr[0];
    mcast[1] = pAddr[1];
    mcast[2] = pAddr[2];
    mcast[3] = pAddr[3];
    mcast[4] = pAddr[4];
    mcast[5] = pAddr[5];


    filter.lafWords[0] = (UINT16) csrRead (pDrvCtrl,  CSR(8));
    filter.lafWords[1] = (UINT16) csrRead (pDrvCtrl,  CSR(9));
    filter.lafWords[2] = (UINT16) csrRead (pDrvCtrl, CSR(10));
    filter.lafWords[3] = (UINT16) csrRead (pDrvCtrl, CSR(11));


    /* process each logical address bit in the order of transmission */

    for (bit = 0; bit < BITS_PER_LA; ++bit)
        {
        byte = (bit >> 3);  /* byte index is bit index divided by 8 */

        intermediate = ((mcast[byte] ^ crc) & 0x01);

        crc >>= 1;

        if (intermediate)
            crc ^= LN_CRC_POLYNOMIAL;

        (mcast[byte]) >>= 1;
        }


    /* just want the 6 most significant bits */

    crc = LN_CRC_TO_LAF_IX (crc);

    if (command == LN_LADRF_ADD)
        {
        /* turn on the corresponding bit in the filter */

        filter.lafBytes[(crc & 0x0000003f) >> 3] |= (1 << (crc & 0x7));
        }
    else
        {
        /* turn off the corresponding bit in the filter */

        filter.lafBytes[(crc & 0x0000003f) >> 3] &= ~(1 << (crc & 0x7));
        }


    csrWrite (pDrvCtrl,  CSR(8), filter.lafWords[0]);
    csrWrite (pDrvCtrl,  CSR(9), filter.lafWords[1]);
    csrWrite (pDrvCtrl, CSR(10), filter.lafWords[2]);
    csrWrite (pDrvCtrl, CSR(11), filter.lafWords[3]);

    /* leave suspend mode */

    suspendModeLeave (pDrvCtrl, spndKey);


    return (OK);
    }

/******************************************************************************
*
* ln97xAddrFilterSet - set the address filter for multicast addresses
*
* This routine goes through all of the multicast addresses on the list
* of addresses (added with the ln97xAddrAdd() routine) and sets the
* device's logical address filter in the initialization block.
*
* CAVEATS
* This routine is only used when the controller is being initialized or
* reinitialized via its initialization block.  The logical address filter
* registers, CSR8 - CSR11, are not modified in this routine.
*
* RETURNS: N/A
*
* SEE ALSO:  ln97xAddrFilterMod()
*/
LOCAL void ln97xAddrFilterSet
    (
    LN_97X_DRV_CTRL * pDrvCtrl             /* pointer to DRV_CTRL structure */
    )
    {
    UINT8             mcast [LN_LA_LEN];   /* temporary logical address */

    int               byte;                /* index logical addr byte array */
    int               bit;                 /* index logical addr bit array */

    int               intermediate;        /* CRC intermediate remainder */
    UINT32            crc;                 /* initial CRC remainder */

    volatile LN_IB *  pIb   = pDrvCtrl->ib;

    ETHER_MULTI *     pCurr = END_MULTI_LST_FIRST (&pDrvCtrl->endObj);


    ibAddrFilterClear (pIb);


    while (pCurr != NULL)
        {
        /* copy the specified address, as we'll destroy it with shifts below */

        mcast[0] = pCurr->addr[0];
        mcast[1] = pCurr->addr[1];
        mcast[2] = pCurr->addr[2];
        mcast[3] = pCurr->addr[3];
        mcast[4] = pCurr->addr[4];
        mcast[5] = pCurr->addr[5];

        crc = 0xffffffff;

        /* process each logical address bit in the order of transmission */

        for (bit = 0; bit < BITS_PER_LA; ++bit)
            {
            byte = (bit >> 3);  /* byte index is bit index divided by 8 */

            intermediate = ((mcast[byte] ^ crc) & 0x01);

            crc >>= 1;

            if (intermediate)
                crc ^= LN_CRC_POLYNOMIAL;

            (mcast[byte]) >>= 1;
            }


        /* Just want the 6 most significant bits. */

        crc = LN_CRC_TO_LAF_IX (crc);

        /* Turn on the corresponding bit in the filter. */

        ibAddrFilterSet (pIb, crc);

        pCurr = END_MULTI_LST_NEXT(pCurr);
        }
    }

/*******************************************************************************
*
* ln97xPollReceive - routine to receive a packet in polled mode.
*
* This routine is called by a user to try and get a packet from the
* device. This routine return OK if it is successful in getting the packet
*
* RETURNS: OK or EAGAIN.
*/
LOCAL STATUS ln97xPollReceive
    (
    LN_97X_DRV_CTRL * pDrvCtrl,  /* points to a device control structure */
    M_BLK_ID          pMblk      /* mBlk chain stores received data */
    )
    {
    volatile LN_RMD * pRmd       = NULL;
    BOOL              gotAPacket = FALSE;


    if ((pMblk->mBlkHdr.mFlags & M_EXT) != M_EXT)
        {
        return (EAGAIN);
        }


    /* get the next available (driver owned) receive descriptor */

    if ((pRmd = ln97xFullRMDGet (pDrvCtrl)) != NULL)
        {
        UINT32  saveCsr0;

        int     len = LN_PKT_LEN_GET (pRmd);

        /* clear error status bits if we missed input packets */

        if ((saveCsr0 = csrRead (pDrvCtrl, CSR(0))) & CSR0_MISS)
            {
            csrWrite (pDrvCtrl, CSR(0), saveCsr0);
            END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);
            }

        /* check the packet for errors */

        if (rmdHasErrors (pRmd))
            {
            END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);
            }
        else if (pMblk->mBlkHdr.mLen >= len)
            {
            /* get a pointer to the packet and make the data coherent */

            const char * pPacket = rmdBuffAddrGet (pDrvCtrl, pRmd);
            LN_CACHE_INVALIDATE (pPacket, len);

            pMblk->mBlkHdr.mData  += pDrvCtrl->offset;
            pMblk->mBlkHdr.mLen    = len;
            pMblk->mBlkHdr.mFlags |= M_PKTHDR;
            pMblk->mBlkPktHdr.len  = len;

            /* upper layer provides the buffer */

            bcopy (pPacket, (char *)(pMblk->mBlkHdr.mData), len);

            END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_UCAST, +1);
            gotAPacket = TRUE;
            }

        /* return a clean descriptor to the controller */

        rxDescClean (pRmd);

        CACHE_PIPE_FLUSH ();

        /* increment index to RMD ring */

        pDrvCtrl->rmdIndex = ((pDrvCtrl->rmdIndex + 1) % (pDrvCtrl->rringSize));
        }

    return (gotAPacket ? OK : EAGAIN);
    }

/*******************************************************************************
*
* ln97xPollSend - send a packet in polled mode.
*
* This routine is called by a user to try and send a packet on the
* device.
*
* CAVEATS
* Assumes that the TMD returned from ln97xTMDGet() has been "cleaned".
* Assumes that the TMD will not be inadvertently cleaned while the
* TMD is being constructed and before the controller has been given
* ownersip of the TMD.
*
* INTERNAL
* A return value of OK indicates that the packet has been sent.  In this
* case, the upper level protocol (WDB for example) will free the mBlk and
* associated cluster.
*
* RETURNS: OK or EAGAIN.
*/
LOCAL STATUS ln97xPollSend
    (
    LN_97X_DRV_CTRL * pDrvCtrl,  /* points to device control structure */
    M_BLK_ID          pMblk      /* mBlk chain stores transmitted data */
    )
    {
    BOOL              putAPacket = FALSE;

    volatile LN_TMD * pTmd = ln97xTMDGet (pDrvCtrl);

    char *            pBuf = netClusterGet (pDrvCtrl->endObj.pNetPool,
                                            pDrvCtrl->pClPoolId);


    if ((pTmd == NULL) || (pBuf == NULL))
        {
        if (pBuf)
            netClFree (pDrvCtrl->endObj.pNetPool, (UCHAR *) pBuf);

        END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_ERRS, +1);
        }
    else
        {
        UINT32  ltmd1;
        int     len;

        pBuf += pDrvCtrl->offset;
        len   = netMblkToBufCopy (pMblk, pBuf, NULL);

        /* increment the unicast output counter */

        END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_UCAST, +1);

        /* XXX - DO NOT call netMblkClChainFree() here */

        tmdBuffAddrSet (pDrvCtrl, pTmd, pBuf);

        ltmd1 = (TMD1_BCNT_MSK & -(max (len, ETHERSMALL))) |
                (TMD1_OWN | TMD1_CNST | TMD1_STP | TMD1_ENP);


        /* increment index to TMD ring */

        pDrvCtrl->tmdIndex = ((pDrvCtrl->tmdIndex + 1) % (pDrvCtrl->tringSize));

        /* write transmit descriptor (with OWN-bit set) to actual register */

        pTmd->lnTMD1 = PCI_SWAP (ltmd1);

        CACHE_PIPE_FLUSH ();


        /* kick start the transmitter, if selected */

        if (lnKickStartTx)
            {
            csrWrite (pDrvCtrl, CSR(0), CSR0_TDMD);
            }

        /* read the device status register */

        while (!(csrRead (pDrvCtrl, CSR(0)) & CSR0_TINT))
            {
            ;  /* spin until the packet is transmitted */
            }

        /* clear the transmit interrupt bit */

        csrWrite (pDrvCtrl, CSR(0), CSR0_TINT);

        putAPacket = TRUE;
        }


    /*
     * Free output clusters, return TMDs to controller,
     * and check for output errors.
     */

    if (!pDrvCtrl->txCleaning)
        {
        ln97xTRingScrub (pDrvCtrl);
        }
    
    return (putAPacket ? OK : EAGAIN);
    }

/*****************************************************************************
*
* ln97xMCastAddrAdd - add a multicast address for the device
*
* This routine adds a multicast address to whatever the driver
* is already listening for.  It then resets the address filter.
*
* NOTE
* etherMultiAdd() returns EINVAL for invalid multicast addresses.
*
* RETURNS: OK or ERROR.
*/
LOCAL STATUS ln97xMCastAddrAdd
    (
    LN_97X_DRV_CTRL * pDrvCtrl,  /* pointer to DRV_CTRL structure */
    char *            pAddress   /* address to be added */
    )
    {
    /*
     * ENETRESET is returned to inform the driver that the list has
     * changed and its reception filter should be adjusted accordingly.
     */
 
    int retVal = etherMultiAdd (&pDrvCtrl->endObj.multiList, pAddress);

    if (retVal == ENETRESET)
        {
        /* edit Logical Addresss Filter Registers */

        return (ln97xAddrFilterMod (pDrvCtrl, (UINT8 *)pAddress, LN_LADRF_ADD));
        }

    return ((retVal == OK) ? OK : ERROR);
    }

/*****************************************************************************
*
* ln97xMCastAddrDel - delete a multicast address for the device
*
* This routine removes a multicast address from whatever the driver
* is listening for.  It then resets the address filter.
*
* NOTE
* etherMultiAdd() returns EINVAL for invalid multicast addresses.
*
* RETURNS: OK or ERROR.
*/
LOCAL STATUS ln97xMCastAddrDel
    (
    LN_97X_DRV_CTRL * pDrvCtrl,  /* pointer to DRV_CTRL structure */
    char *            pAddress   /* address to be deleted */
    )
    {
    /*
     * ENETRESET is returned to inform the driver that the list has
     * changed and its reception filter should be adjusted accordingly.
     */
 
    int retVal = etherMultiDel (&pDrvCtrl->endObj.multiList, pAddress);

    if (retVal == ENETRESET)
        {
        /* edit Logical Addresss Filter Registers */

        return (ln97xAddrFilterMod (pDrvCtrl, (UINT8 *)pAddress, LN_LADRF_DEL));
        }

    return ((retVal == OK) ? OK : ERROR);
    }

/*****************************************************************************
*
* ln97xMCastAddrGet - get the multicast address list for the device
*
* This routine gets the multicast list of whatever the driver
* is already listening for.
*
* RETURNS: OK/ERROR
*/
LOCAL STATUS ln97xMCastAddrGet
    (
    LN_97X_DRV_CTRL * 	pDrvCtrl,  /* pointer to DRV_CTRL structure */
    MULTI_TABLE * 	pTable     /* table where address will be copied */
    )
    {
    return (etherMultiGet (&pDrvCtrl->endObj.multiList, pTable));
    }

/*******************************************************************************
*
* ln97xStop - stop the device
*
* This function calls BSP functions to disconnect interrupts and stop
* the device from operating in interrupt mode.
*
* NOTE
* There is no need to explicitly clear the device-side interrupt enable
* bit (CSR0, bit 6) after setting the STOP bit in CSR0.  The interrupt
* enable bit is automatically cleared (device-side interrupts disabled)
* by controller hard reset, controller soft reset, or by setting the
* STOP bit in CSR0.
*
* RETURNS: OK or ERROR if the interrupt handler could not be disconnected.
*/
LOCAL STATUS ln97xStop
    (
    LN_97X_DRV_CTRL * pDrvCtrl  /* device control structure */
    )
    {
    STATUS            result = OK;

    /* The STOP bit disables the chip, including device-side interrupts. */

    csrLockedWrite (pDrvCtrl, CSR(0), CSR0_STOP);

    (* ln97xIntDisable) (pDrvCtrl->ilevel);

    /* mark the interface as down */

    END_FLAGS_CLR (&pDrvCtrl->endObj, (IFF_UP | IFF_RUNNING));

    /* disconnect the interrupt handler */

    result = (* ln97xIntDisconnect) ((VOIDFUNCPTR *)
             INUM_TO_IVEC (pDrvCtrl->inum), ln97xInt, (int)(pDrvCtrl));


    return (result);
    }

/******************************************************************************
*
* ln97xUnload - unload a driver from the system
*
* This function first brings down the device, and then frees any
* stuff that was allocated by the driver in the load function. The controller
* structure should be free by who ever is calling this function.
*
* RETURNS: OK
*/
LOCAL STATUS ln97xUnload
    (
    LN_97X_DRV_CTRL * pDrvCtrl  /* device control structure */
    )
    {
    END_OBJECT_UNLOAD (&pDrvCtrl->endObj);

    /* Free the shared DMA memory. */

    if (pDrvCtrl->flags & LS_MEM_ALLOC_FLAG)
        cacheDmaFree ((void *)(pDrvCtrl->pShMem));

    /* Free the shared DMA memory allocated for clusters */
    
    if (pDrvCtrl->flags & LS_MEM_ALLOC_FLAG)
        cacheDmaFree (pDrvCtrl->clDesc.memArea);

    /* Free the memory allocated for mBlks and clBlks */
    
    if (pDrvCtrl->mClCfg.memArea != NULL)
        free (pDrvCtrl->mClCfg.memArea);

    /* Free the memory allocated for driver pool structure */

    if (pDrvCtrl->endObj.pNetPool != NULL)
        free (pDrvCtrl->endObj.pNetPool);

    return (OK);
    }

/*******************************************************************************
*
* ln97xPollStart - start polled mode operations
*
* RETURNS: OK always.
*/
LOCAL STATUS ln97xPollStart
    (
    LN_97X_DRV_CTRL * pDrvCtrl    /* pointer to DRV_CTRL structure */
    )
    {
    /* transition to polled mode by first suspending the device */

    UINT32 spndKey = suspendModeEnter (pDrvCtrl);

    /* clear CSR0 interrupt enable bit without clearing status bits */

    csrWrite (pDrvCtrl, CSR(0), 0);
    pDrvCtrl->flags |= LS_POLLING;

    /* leave suspend mode */

    suspendModeLeave (pDrvCtrl, spndKey);

    DRV_LOG (DRV_DEBUG_POLL, "POLLING STARTED\n", 0, 0, 0, 0, 0, 0);

    return (OK);
    }

/*******************************************************************************
*
* ln97xPollStop - stop polled mode operations
*
* RETURNS: OK always.
*/
LOCAL STATUS ln97xPollStop
    (
    LN_97X_DRV_CTRL * pDrvCtrl    /* pointer to DRV_CTRL structure */
    )
    {
    int intLevel = intLock();

    /* set CSR0 interrupt enable bit without clearing status bits */

    csrWrite (pDrvCtrl, CSR(0), CSR0_INEA);
    pDrvCtrl->flags &= ~LS_POLLING;

    intUnlock (intLevel);

    DRV_LOG (DRV_DEBUG_POLL, "POLLING STOPPED\n", 0, 0, 0, 0, 0, 0);

    return (OK);
    }

/******************************************************************************
*
* ln97xTRingScrub - clean the transmit ring
*
* This routine frees driver clusters associated with TMDs that have used
* the clusters for transmitting packets.  In the process of "cleaning"
* each TMD, this routine checks the TMD error status bits and updates MIB
* II counters appropriately.  In the case of fatal errors on transmission,
* the driver and controller are restarted.
*
* Buffer underflow error on transmission is considered fatal.  In the case
* of an underflow error, the transmitter is shut down.  This is a situation
* that will occur if DMA transfers cannot keep up with the transmitter.
* For example, an underflow error could occur if the PCnet is held off
* from DMA access for too long due to memory latency resulting from DRAM
* refresh or slow memory devices.  Many implementations use a dedicated
* PCnet buffer.  This buffer can be static RAM to eliminate refresh
* conflicts; or dual-port RAM so that the PCnet can have free run of this
* memory during its DMA transfers.
*
* RETURNS: N/A
*
* NOMANUAL
*/
LOCAL void ln97xTRingScrub
    (
    LN_97X_DRV_CTRL * pDrvCtrl  /* pointer to DRV_CTRL structure */
    )
    {
    volatile LN_TMD * pTmd;

    UINT32            tmpTmd1;
    UINT32            tmpTmd2;

    int               m2ErrorCnt = 0;
    int               m2UcastCnt = 0;

    pDrvCtrl->txCleaning = TRUE;
    
    while (pDrvCtrl->tmdIndexC != pDrvCtrl->tmdIndex)
        {
        /* point to the next TMD to "clean" */

        pTmd = pDrvCtrl->pTring + pDrvCtrl->tmdIndexC;
        LN_CACHE_INVALIDATE (pTmd, TMD_SIZ);

        tmpTmd1 = PCI_SWAP (pTmd->lnTMD1);
        tmpTmd2 = PCI_SWAP (pTmd->lnTMD2);

        /* TMD1_ERR is an "OR" of LCOL, LCAR, UFLO, RTRY or BPE.
         * BUFF (buffer error) is indicated in TMD2.
         */

        if ((tmpTmd1 & TMD1_ERR) || (tmpTmd2 & TMD2_BUFF))
            {
            ++m2ErrorCnt;
            --m2UcastCnt;

            /* Assuming DRTY (disable retry on collision) is not set,
             * increment the error counter for collisions (16 failed
             * attempts to transmit a frame).
             */

            if (tmpTmd2 & TMD2_RTRY)
                ++m2ErrorCnt;

            /* Restart controller on fatal buffer underflow error */

            if (tmpTmd2 & (TMD2_BUFF | TMD2_UFLO))
                {
                END_FLAGS_CLR (&pDrvCtrl->endObj, (IFF_UP | IFF_RUNNING));
                END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_ERRS, m2ErrorCnt);
                END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_UCAST, m2UcastCnt);

                pDrvCtrl->txCleaning = FALSE;
                ln97xRestart (pDrvCtrl);
                return;
                }
            }

        /* if the buffer is still owned by PCnet, don't touch it */

        if (tmpTmd1 & TMD1_OWN)
            break;

        /* update MIB-II errors for the current TMD */

        if (m2ErrorCnt)
            {
            END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_ERRS, m2ErrorCnt);
            END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_UCAST, m2UcastCnt);
            m2ErrorCnt = m2UcastCnt = 0;
            }

        /* free any clusters allocated to this TMD */

        if (pTmd->lnTMD0 != 0)
            {
            char * pCluster = tmdBuffAddrGet (pDrvCtrl, pTmd);

            netClFree (pDrvCtrl->endObj.pNetPool,
                      (UCHAR *)(pCluster - (pDrvCtrl->offset)));

            pTmd->lnTMD0 = 0;
            }

        /* clear the TMD error status bits */

        tmpTmd1     &= ~(TMD1_ERR | TMD1_BPE);
        pTmd->lnTMD2 = 0;
        pTmd->lnTMD1 = PCI_SWAP (tmpTmd1);

        /* now increment the TMD cleaning index pointer */

        pDrvCtrl->tmdIndexC = ((pDrvCtrl->tmdIndexC + 1) %
                               (pDrvCtrl->tringSize));
        }

    pDrvCtrl->txCleaning = FALSE;
    }

/*******************************************************************************
*
* ln97xParamInit - set global parameters for endianess and interrupt level
*
* This routine takes the values handed to it via the INCLUDE_79XEND
* component and uses them to initialize the driver-global variables
* describing the pci bus endianess on the target hardware and the
* interrupt level for the PCnet device.  The default value of these
* parameters, seen in INCLUDE_79XEND, will work on most every target.
* 
* RETURNS: OK Always.
*
* NOMANUAL
*/
STATUS ln97xParamInit
    (
    int pciBusEndianess,
    int intLevel
    )
    {
    ln97xBusEndianess   = pciBusEndianess;
    ln97xInterruptLevel = intLevel;

    return (OK);
    }

#if FALSE /* use this if no board level support */

/*******************************************************************************
*
* ln97XEnetAddrGet - get Am79C97x Ethernet (IEEE station) address
*
* This routine provides an interface for accessing an Am79C97x device
* Ethernet (IEEE station address) address in the device's Address PROM
* (APROM).  A handle to the specific device control structure is specified
* in the <pDrvCtrl> parameter.  The 6-byte IEEE station address will be
* copied to the memory location specified by the <enetAdrs> parameter.
*
* CAVEATS
* Assumes the device control structure has been initialized with a
* memory mapped I/O base address.
*
* INTERNAL
* The 6 bytes of the IEEE station address occupy the first 6 locations of
* the Address PROM space.  The driver must copy the station address from
* the Address PROM space to the initialization block or to CSR12-14 in order
* for the receiver to accept unicast frames directed to the device.
*
* Bytes 14 and 15 of APROM should each be ASCII 'W' (57h).  The above
* requirements must be met in order to be compatible with AMD driver
* software.
*
* The APROM is 32 bytes, and there is no need to read all of that here.
* So, this routine reads half of the APROM to get the Ethernet address
* and test for ASCII 'W's.
*
* RETURNS: OK, or ERROR if could not be obtained.
*
* NOMANUAL
*/
STATUS ln97XEnetAddrGet
    (
    LN_97X_DRV_CTRL * pDrvCtrl,   /* Driver control structure */
    char *            enetAdrs    /* specifies storage for station address */
    )
    {
    char          aprom [LN_97X_APROM_SIZE];   /* store a copy of APROM */

    int           numBytes = (LN_97X_APROM_SIZE >> 1);
    int           ix;

    /* get I/O address of unit */

    char * const ioaddr = (char *)(pDrvCtrl->devAdrs);

    /* load APROM into an array (loop partially unrolled) */

    for (ix = 0; ix < numBytes; ix += 4)
        {
        aprom[ix  ] = ioaddr[ix  ];
        aprom[ix+1] = ioaddr[ix+1];
        aprom[ix+2] = ioaddr[ix+2];
        aprom[ix+3] = ioaddr[ix+3];
        }

    /* check for ASCII 'W's at APROM bytes 14 and 15 */

    if ((aprom [0xe] != 'W') || (aprom [0xf] != 'W'))
        {
        return ERROR;
        }

    enetAdrs[0] = aprom[0];
    enetAdrs[1] = aprom[1];
    enetAdrs[2] = aprom[2];
    enetAdrs[3] = aprom[3];
    enetAdrs[4] = aprom[4];
    enetAdrs[5] = aprom[5];

    return OK;
    }
#endif /* XXX */
