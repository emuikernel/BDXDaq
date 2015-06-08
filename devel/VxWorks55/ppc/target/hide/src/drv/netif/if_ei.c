/* if_ei.c - Intel 82596 Ethernet network interface driver */

/* Copyright 1989-1997 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
03i,15jul97,spm  added ARP request to SIOCSIFADDR ioctl handler
03i,29aug97,dgp  doc: SPR 9184 - promiscuous mode not supported
03h,05feb97,dat  fixed wdog reset action to use netJobAdd(). SPR 7771
03j,07apr97,spm  code cleanup, corrected statistics, and upgraded to BSD 4.4
03i,12nov94,tmk  removed some IMPORTed function decs and included unixLib.h
03h,10oct94,caf  suppress buffer loaning for MIPS targets due to
		 problem with misaligned accesses.
03g,14oct93,wmd  Added initialization of pointer pDrvCtrl in eiWatchDog(),
		 SPR #2567.
03f,30sep93,wmd  Added changes to include timeout check of the receiver 
		 as done in the 5.0.6 release. Recreated eiInit() to allow
		 resetting the 596 if it locks up.
03e,11aug93,jmm  Changed ioctl.h and socket.h to sys/ioctl.h and sys/socket.h
03d,19feb93,jdi  documentation cleanup.
03c,15jan93,wmd  added setting of errno to S_ioslib_INVALID_ETHERNET_ADDRESS.
03b,11dec92,rfs  Removed dummy eiChipReset(), no longer needed for TP41.
03a,05oct92,rfs  Added documentation.
02z,02oct92,rfs  Made multiple attach calls per unit return OK, not ERROR.
02y,01oct92,rfs  Added 16 byte page alignment for SCP.
02x,28sep92,rfs  Modified the eiAction() routine to clear all events.  This
                 makes the diagnostics pass, for some undocumented reason.
02w,09sep92,gae  documentation tweaks.
02v,11aug92,rfs  Slight change to memory carving.  Also fixed receive bug
                 that was due to device manual incorrectly describing bit 7
                 of config byte 10 and added error checks to RFDs.  Changed
                 the device startup routine.
02u,26aug92,kdl  moved internal notes to file header (were just #ifdef'd out).
02t,19aug92,smb  changed systime.h to sys/times.h
02s,31jul92,dave Changed to new cacheLib.
02r,28jul92,rfs  Adapted to latest cache coherency model.
                 Renamed driver control structure and moved it to bss.
                 Combined the init() routine with the attach() routine and
                 made attach() reject multiple invocations.  Rewrote the
                 ioctl() routine to do the right stuff.  Reworked the action
                 command routine and the primary command routine.  Other misc.
02q,16jul92,rfs  Moved driver specific items from header file to here,
                 where they belong.
02p,16jul92,rfs  Moved internal comments to end of file, where they belong.
02o,03jul92,jwt  converted cacheClearEntry() calls to cacheClear() for 5.1.
02n,26jun92,rrr  make eiBusSnoop global again (needed by bsps)
02m,25jun92,rfs  backed out last mod; the manual is incorrect about the
                 state of the bit that configures CRC inclusion
02l,25jun92,rfs  bug fix, changed a receive config parm that selects whether
                 the CRC is included in the buffer
02k,23jun92,ajm  made eiChipReset global for tp41 BSP
02j,08jun92,ajm  modified so eiattach is only external function
                 got rid of MEM_ROUND_UP macro now pulled in with vxWorks.h
                 got rid of many ansi complaints
02i,26may92,rrr  the tree shuffle
                 -changed includes to have absolute path from h/
02h,22feb92,wmd  added conditional compiles for 040, ansi fixes.
02g,07jan92,shl  made driver cache safe. added eiBusSnoop(). eliminated some
                 ANSI warnings.
02f,12nov91,wmd  Added fix, courtesy of jrb-Intel, which added a watchdog to
                 timeout any stalls due to hardware lockup and resets the
                 82596.
02e,25oct91,rfs  Changed eiChipReset() to not depend on valid *es, and
                 made it global for TP41 usage. Also eliminated ANSI
                 warnings.
02d,04oct91,rrr  passed through the ansification filter
                 -changed functions to ansi style
                 -changed includes to have absolute path from h/
                 -changed VOID to void
                 -changed copyright notice
02c,15sep91,rfs  Bug fix. Eliminated lock condition when output queue full
                 and out of TFDs. Added major comment to eiTxStartup. Also,
                 major changes to eiChipReset() to make more robust. All
                 instances of intLock()/intUnlock() changed to
                 sys596IntDisable()/sys596IntEnable respectively. Statistic
                 counters now being used, which required moderate changes to
                 the transmit side design. More comments added.
02b,21aug91,rfs  changed method of obtaining Ethernet address.
02a,22jul91,jcf  completely rewritten to utilize simplified mode,
            rfs  added big endian support, reworked buffer loaning.
01h,01may91,elh  fixed ethernet addressing problem, upgraded to 5.0,
                 added buffer loanouts, fixed ifAddrSet bug by disabling
                 interrupts in eiHardReset.
01g,28apr91,del  added #define of ETHER_HEADER_SIZE as workaround for
                 gnu960 pading of struct ether_header.
01f,05jan91,gae  added global eiSCPSysConfig for board specific SCP parameters.
                 moved CDELAY here from if_ei.h and renamed eichkcca.
01e,03jan91,gae  more cleanup and merged with Intel's fixes.
                 Ethernet address (eiEnetAddr) is set in sysHwInit.
                 cleaned up error printing & no carrier message always printed.
                 changed printf's to printErr's (logMsg if at int level).
                 made most routines local -- not available outside this module.
01d,31dec90,gae  removed 68K stuff.
01c,14dec90,elh  more cleanup.
01b,12dec90,elh  fixed buffer problems, cleaned up, added etherhooks.
01a,06feb90,djk  written
*/

/*
This module implements the Intel 82596 Ethernet network interface driver.

This driver is designed to be moderately generic, operating unmodified
across the range of architectures and targets supported by VxWorks.  
To achieve this, this driver must be given several
target-specific parameters, and some external support routines must be
provided.  These parameters, and the mechanisms used to communicate them to the
driver, are detailed below.

This driver can run with the device configured in either
big-endian or little-endian modes.  Error recovery code has been added to
deal with some of the known errata in the A0 version of the device.  This
driver supports up to four individual units per CPU.

BOARD LAYOUT
This device is on-board.  No jumpering diagram is necessary.

EXTERNAL INTERFACE
This driver provides the standard external interface with the following
exceptions.  All initialization is performed within the attach routine;
there is no separate initialization routine.  Therefore, in the global interface
structure, the function pointer to the initialization routine is NULL.

The only user-callable routine is eiattach(), which publishes the `ei'
interface and initializes the driver and device.

TARGET-SPECIFIC PARAMETERS
.iP "the <sysbus> value"
This parameter is passed to the driver by eiattach().

The Intel 82596 requires this parameter during initialization.  This
parameter tells the device about the system bus, hence the name "sysbus."
To determine the correct value for a target, refer to the document
.I "Intel 32-bit Local Area Network (LAN) Component User's Manual."
.iP "interrupt vector"
This parameter is passed to the driver by eiattach().

The Intel 82596 generates hardware interrupts for various events within
the device; thus it contains an interrupt handler routine.
This driver calls intConnect() to connect its interrupt handler to the 
interrupt vector generated as a result of the 82596 interrupt.
.iP "shared memory address"
This parameter is passed to the driver by eiattach().

The Intel 82596 device is a DMA type device and typically shares
access to some region of memory with the CPU.  This driver is designed
for systems that directly share memory between the CPU and the 82596.

This parameter can be used to specify an explicit memory region for use
by the 82596.  This should be done on targets that restrict the 82596
to a particular memory region.  The constant NONE can be used to indicate
that there are no memory limitations, in which case, the driver 
attempts to allocate the shared memory from the system space.
.iP "number of Receive and Transmit Frame Descriptors"
These parameters are passed to the driver by eiattach().

The Intel 82596 accesses frame descriptors in memory for each frame
transmitted or received.  The number of frame descriptors 
at run-time can be configured using these parameters.
.iP "Ethernet address"
This parameter is obtained by a call to an external support routine.

During initialization, the driver needs to know the Ethernet address for
the Intel 82596 device.  The driver calls the external support routine,
sysEnetAddrGet(), to obtain the Ethernet address.  For a description of
sysEnetAddrGet(), see "External Support Requirements" below.
.LP

EXTERNAL SUPPORT REQUIREMENTS
This driver requires seven external support functions:
.iP "STATUS sysEnetAddrGet (int unit, char *pCopy)" "" 9 -1
This routine provides the six-byte Ethernet address
used by <unit>.  It must copy the six-byte address
to the space provided by <pCopy>.  This routine 
returns OK, or ERROR if it fails.  The driver calls this 
routine, once per unit, using eiattach().
.iP "STATUS sys596Init (int unit)"
This routine performs any target-specific initialization
required before the 82596 is initialized.  Typically, it is empty.  
This routine must return OK, or ERROR if it fails.
The driver calls this routine, once per unit, using eiattach().
.iP "void sys596Port (int unit, int cmd, UINT32 addr)"
This routine provides access to the special port
function of the 82596.  It delivers the command and
address arguments to the port of the specified unit.
The driver calls this routine primarily during
initialization, but may also call it during error
recovery procedures.
.iP "void sys596ChanAtn (int unit)"
This routine provides the channel attention signal to
the 82596, for the specified <unit>.  The driver calls
this routine frequently throughout all phases of
operation.
.iP "void sys596IntEnable (int unit), void sys596IntDisable (int unit)"
These routines enable or disable the interrupt from
the 82596 for the specified <unit>.  Typically, this
involves interrupt controller hardware, either
internal or external to the CPU.  Since the 82596
itself has no mechanism for controlling its interrupt
activity, these routines are vital to the correct
operation of the driver.  The driver calls these
routines throughout normal operation to protect
certain critical sections of code from interrupt
handler intervention.
.iP "void sys596IntAck (int unit)"
This routine must perform any required interrupt acknowledgment 
or clearing.  Typically, this involves an operation to some interrupt
control hardware.  Note that the INT signal from the 82596
behaves in an "edge-triggered" mode; therefore, this routine 
typically clears a latch within the control circuitry.
The driver calls this routine from the interrupt handler.
.LP

SYSTEM RESOURCE USAGE
When implemented, this driver requires the following system resources:

    - one mutual exclusion semaphore
    - one interrupt vector
    - one watchdog timer.
    - 8 bytes in the initialized data section (data)
    - 912 bytes in the uninitialized data section (BSS)

The above data and BSS requirements are for the MC68020 architecture 
and may vary for other architectures.  Code size (text) varies greatly between
architectures and is therefore not quoted here.

The driver uses cacheDmaMalloc() to allocate memory to share with the 82596.  
The fixed-size pieces in this area total 160 bytes.  The variable-size 
pieces in this area are affected by the configuration parameters specified 
in the eiattach() call.  The size of one RFD (Receive Frame
Descriptor) is 1536 bytes.  The size of one TFD (Transmit Frame Descriptor) 
is 1534 bytes.  For more information about RFDs and TFDs, see the 
.I "Intel 82596 User's Manual."

The 82596 can be operated only if this shared memory region is non-cacheable
or if the hardware implements bus snooping.  The driver cannot maintain
cache coherency for the device because fields within the command
structures are asynchronously modified by both the driver and the device,
and these fields may share the same cache line.


TUNING HINTS
The only adjustable parameters are the number of TFDs and RFDs that will be
created at run-time.  These parameters are given to the driver when eiattach()
is called.  There is one TFD and one RFD associated with each transmitted
frame and each received frame respectively.  For memory-limited applications,
decreasing the number of TFDs and RFDs may be desirable.  Increasing the number
of TFDs will provide no performance benefit after a certain point.
Increasing the number of RFDs will provide more buffering before packets are
dropped.  This can be useful if there are tasks running at a higher priority
than the net task.

CAVEAT
This driver does not support promiscuous mode.

SEE ALSO: ifLib,
.I "Intel 82596 User's Manual,"
.I "Intel 32-bit Local Area Network (LAN) Component User's Manual"
*/

#include "vxWorks.h"
#include "wdLib.h"
#include "iv.h"
#include "vme.h"
#include "net/mbuf.h"
#include "net/unixLib.h"
#include "net/protosw.h"
#include "sys/socket.h"
#include "sys/ioctl.h"
#include "errno.h"
#include "memLib.h"
#include "intLib.h"
#include "net/if.h"
#include "net/route.h"
#include "iosLib.h"
#include "errnoLib.h"

#include "cacheLib.h"
#include "logLib.h"
#include "netLib.h"
#include "stdio.h"
#include "stdlib.h"
#include "sysLib.h"

#ifdef  INET
#include "netinet/in.h"
#include "netinet/in_systm.h"
#include "netinet/in_var.h"
#include "netinet/ip.h"
#include "netinet/if_ether.h"
#endif  /* INET */

#include "etherLib.h"
#include "net/systm.h"
#include "sys/times.h"
#include "drv/netif/if_ei.h"
#include "net/if_subr.h"


/***** LOCAL DEFINITIONS *****/

#undef EI_DEBUG                             /* Compiles debug output */

#define MAX_UNITS       4                   /* maximum units to support */
#define DEF_NUM_TFDS    32                  /* default number of TFDs */
#define DEF_NUM_RFDS    32                  /* default number of RFDs */

#if     (CPU_FAMILY == MIPS)
#define MAX_RFDS_LOANED 0                   /* suppress buffer loaning */
#else
#define MAX_RFDS_LOANED 8                   /* max RFDs that can be loaned */
#endif  /* (CPU_FAMILY == MIPS) */

/* Typedefs for external structures that are not typedef'd in their .h files */

typedef struct mbuf MBUF;
typedef struct arpcom IDR;                  /* Interface Data Record wrapper */
typedef struct ifnet IFNET;                 /* real Interface Data Record */
typedef struct sockaddr SOCK;
typedef struct ether_header ETH_HDR; 		

/* The definition of our linked list management structure */

typedef struct ei_list                       /* EI_LIST - 82596 queue head */
    {
    volatile EI_NODE *  head;       /* header of list */
    volatile EI_NODE *  tail;       /* tail of list */
    } EI_LIST;

/* The definition of the driver control structure */

typedef struct drv_ctrl
    {
    IDR                 idr;        /* interface data record */

    BOOL                attached;   /* indicates attach() called */
    char                *memBase;   /* 82596 memory pool base */
    SCP                 *pScp;      /* SCP ptr */
    ISCP                *pIscp;     /* ISCP ptr */
    SCB                 *pScb;      /* SCB ptr */
    CFD                 *pCfd;      /* synchronous command frame */
    RFD                 *rfdPool;   /* RFD pool */
    TFD                 *tfdPool;   /* TFD pool */
    RFD                 *pFreeRfd;  /* first empty RFD in rxQueue */
    volatile EI_LIST    rxQueue;    /* receive queue */
    volatile EI_LIST    txQueue;    /* to be sent queue */
    volatile EI_LIST    tfdQueue;   /* free transmit frame descriptors */
    volatile EI_LIST    cblQueue;   /* actual chip transmit queue */
    volatile EI_LIST    cleanQueue; /* queue of TFDs to cleanup */
    volatile BOOL       rcvHandling;/* flag, indicates netTask active */
    volatile BOOL       txCleaning; /* flag, indicates netTask active */
    volatile BOOL       txIdle;     /* flag, indicates idle transmitter */
    int                 nTFDs;      /* how many TFDs to create */
    int                 nRFDs;      /* how many RFDs to create */
    int                 nLoanRfds;  /* number of loanable RFDs left */
    UINT8               sysbus;     /* SCP sysbus value */
    int                 ivec;       /* interrupt vector */
    CACHE_FUNCS         cacheFuncs; /* cache descriptor */
    WDOG_ID             wid;        /* watchdog timer for transmit */
    int                 wdInterval; /* ticks between eiWatchDog runs */
    long		recvLocks;  /* count for receive lockup failures */
    long                transLocks; /* count for transmit lockup failures */
    volatile char	wdTxTimeout;/* watchdog runs with transmit hung */
    volatile char	wdRxTimeout;/* watchdog runs with receive hung */
    } DRV_CTRL;

#define DRV_CTRL_SIZ  sizeof(DRV_CTRL)
#define EI_RX_TIMEOUT 3         /* # watchdog runs for receive timeout */
#define EI_TX_TIMEOUT 2         /* # watchdog runs for transmit timeout */


/***** GLOBALS *****/

/* Function declarations not in any header files */

IMPORT STATUS sysEnetAddrGet (int unit, char addr[]);
IMPORT STATUS sys596Init (int unit);
IMPORT STATUS sys596IntAck (int unit);
IMPORT STATUS sys596IntEnable (int unit);
IMPORT void   sys596IntDisable (int unit);
IMPORT void   sys596Port (int unit, int cmd, UINT32 addr);
IMPORT void   sys596ChanAtn (int unit);


/***** LOCALS *****/

/* The array of driver control structs */

LOCAL DRV_CTRL drvCtrl [MAX_UNITS];


/* forward function declarations */

static STATUS eiInit (int unit);
static void eiReset (int unit);
static int eiIoctl (IFNET *pIfNet, int cmd, caddr_t data);
static void eiTxStartup (DRV_CTRL * pDrvCtrl);
static void eiInt (DRV_CTRL *pDrvCtrl);
static void eiTxCleanQ (DRV_CTRL *pDrvCtrl);
static void eiHandleRecvInt (DRV_CTRL *pDrvCtrl);
static STATUS eiReceive (DRV_CTRL *pDrvCtrl, RFD *pRfd);
static void eiLoanFree (DRV_CTRL *pDrvCtrl, RFD *pRfd);
static void eiDiag (int unit);
static void eiConfig (int unit);
static void eiIASetup (int unit);
static void eiRxStartup (DRV_CTRL *pDrvCtrl);
static void eiAction (int unit, UINT16 action);
static STATUS eiCommand (DRV_CTRL *pDrvCtrl, UINT16 cmd);
static void eiTxQPut (DRV_CTRL *pDrvCtrl, TFD *pTfd);
static void eiTxQFlush (DRV_CTRL *pDrvCtrl);
static void eiRxQPut (DRV_CTRL *pDrvCtrl, RFD *pRfd);
static RFD *eiRxQGet (DRV_CTRL *pDrvCtrl);
static BOOL eiRxQFull (DRV_CTRL *pDrvCtrl);
static void eiQInit (EI_LIST *pHead);
static EI_NODE *eiQGet (EI_LIST *pQueue);
static void eiQPut (int unit, EI_LIST *pQueue, EI_NODE *pNode);
static STATUS eiDeviceStart (int unit);
static void eiWatchDog(int unit);

/*******************************************************************************
*
* eiattach - publish the `ei' network interface and initialize the driver and device
*
* This routine publishes the `ei' interface by filling in a network interface
* record and adding this record to the system list.  This routine also
* initializes the driver and the device to the operational state.
*
* The 82596 shares a region of memory with the driver.  The caller of this
* routine can specify the address of this memory region, or can specify that
* the driver must obtain this memory region from the system resources.
*
* The <sysbus> parameter accepts values as described in the Intel manual
* for the 82596.  A default number of transmit/receive frames of 32 can be
* selected by passing zero in the parameters <nTfds> and <nRfds>.
* In other cases, the number of frames selected should be greater than two.
*
* The <memBase> parameter is used to inform the driver about the shared
* memory region.  If this parameter is set to the constant "NONE," then this
* routine will attempt to allocate the shared memory from the system.  Any
* other value for this parameter is interpreted by this routine as the address
* of the shared memory region to be used.
*
* If the caller provides the shared memory region, then the driver assumes
* that this region does not require cache coherency operations, nor does it
* require conversions between virtual and physical addresses.
*
* If the caller indicates that this routine must allocate the shared memory
* region, then this routine will use cacheDmaMalloc() to obtain
* some  non-cacheable memory.  The attributes of this memory will be checked,
* and if the memory is not both read and write coherent, this routine will
* abort and return ERROR.
*
* RETURNS: OK or ERROR.
*
* SEE ALSO: ifLib,
* .I "Intel 82596 User's Manual"
*/

STATUS eiattach
    (
    int         unit,       /* unit number */
    int         ivec,       /* interrupt vector number */
    UINT8       sysbus,     /* sysbus field of SCP */
    char *      memBase,    /* address of memory pool or NONE */
    int         nTfds,      /* no. of transmit frames (0 = default) */
    int         nRfds       /* no. of receive frames (0 = default) */
    )
    {
    DRV_CTRL    *pDrvCtrl;
    UINT        size;                           /* temporary size holder */
    UINT        sizeScp;
    UINT        sizeIscp;
    UINT        sizeScb;
    UINT        sizeCfd;
    RFD *       pRfd;                   /* pointer to RFD's */
    int         ix;

    static char *errMsg1 = "\neiattach: could not obtain memory\n";
    static char *errMsg2 = "\neiattach: shared memory not cache coherent\n";

    sizeScp  = MEM_ROUND_UP (sizeof (SCP));
    sizeIscp = MEM_ROUND_UP (sizeof (ISCP));
    sizeScb  = MEM_ROUND_UP (sizeof (SCB));
    sizeCfd  = MEM_ROUND_UP (sizeof (CFD));

    /* Sanity check the unit number */

    if (unit < 0 || unit >= MAX_UNITS)
        return (ERROR);

    /* Ensure single invocation per system life */

    pDrvCtrl = & drvCtrl [unit];
    if (pDrvCtrl->attached)
        return (OK);

    /* Determine number of Tx and Rx descriptors to use */

    pDrvCtrl->nTFDs = nTfds ? nTfds : DEF_NUM_TFDS;
    pDrvCtrl->nRFDs = nRfds ? nRfds : DEF_NUM_RFDS;


    /* calculate the total size of 82596 memory pool */

    size =
            16 +                                /* allow for alignment */
            sizeScp +
            sizeIscp +
            sizeScb +
            sizeCfd +                           /* synch'ed command frame */
            (sizeof (RFD) * pDrvCtrl->nRFDs) +  /* pool of receive frames */
            (sizeof (TFD) * pDrvCtrl->nTFDs);   /* pool of transmit frames */

    /* Establish the memory area that we will share with the device.  If
     * the caller has provided an area, then we assume it is non-cacheable
     * and will not require the use of the special cache routines.
     * If the caller did not provide an area, then we must obtain it from
     * the system, using the cache savvy allocation routine.
     */

    switch ((int) memBase)
        {
        case NONE :                            /* we must obtain it */

            /* this driver can't handle incoherent caches */

            if (!CACHE_DMA_IS_WRITE_COHERENT () ||
                !CACHE_DMA_IS_READ_COHERENT ())
                {
                printf (errMsg2);
                goto error;
                }

            pDrvCtrl->memBase = cacheDmaMalloc (size);

            if (pDrvCtrl->memBase == NULL)    /* no memory available */
                {
                printf (errMsg1);
                goto error;
                }

            pDrvCtrl->cacheFuncs = cacheDmaFuncs;
            break;

        default :                               /* the user provided an area */
            pDrvCtrl->memBase = memBase;        /* use the provided address */
            pDrvCtrl->cacheFuncs = cacheNullFuncs;
            break;
        }

    /* Save other passed-in parameters */

    pDrvCtrl->sysbus = sysbus;                  /* remember sysbus value */
    pDrvCtrl->ivec = ivec;                      /* interrupt vector */

    /* Carve up the shared memory region into the specific sections */

    bzero ((char *) pDrvCtrl->memBase, size);
    pDrvCtrl->pScp  = (SCP *)                /* align to first 16 byte page */
                      ( ((u_long) pDrvCtrl->memBase + 0xf) & ~0xf );
    pDrvCtrl->pIscp = (ISCP *)((int)pDrvCtrl->pScp   + sizeScp);
    pDrvCtrl->pScb  = (SCB *) ((int)pDrvCtrl->pIscp  + sizeIscp);
    pDrvCtrl->pCfd  = (CFD *) ((int)pDrvCtrl->pScb   + sizeScb);
    pDrvCtrl->rfdPool   = (RFD *) ((int)pDrvCtrl->pCfd   + sizeCfd);
    pDrvCtrl->tfdPool = (TFD *)
            ((int)pDrvCtrl->rfdPool+ (sizeof (RFD) * pDrvCtrl->nRFDs));

    for (ix = 0, pRfd = pDrvCtrl->rfdPool; ix < DEF_NUM_RFDS; ix++, pRfd++)
        {
        pRfd->refCnt	= 0;                    /* initialize ref cnt */
        }

    /* Init the watchdog that will patrol for device lockups */
 
    pDrvCtrl->transLocks = 0;
    pDrvCtrl->recvLocks = 0;
    pDrvCtrl->wid = wdCreate ();                      /* create watchdog */
    if (pDrvCtrl->wid == NULL)                        /* no resource */
        goto error;
    pDrvCtrl->wdInterval = sysClkRateGet() >> 1;

    /* get our enet addr */

    if (sysEnetAddrGet (unit, (char *)pDrvCtrl->idr.ac_enaddr) == ERROR)
	{
	errnoSet (S_iosLib_INVALID_ETHERNET_ADDRESS);
        goto error;
	}

    /* Publish the interface record */
    ether_attach    (
                    (IFNET *) & pDrvCtrl->idr,
                    unit,
                    "ei",
                    (FUNCPTR) NULL,
                    (FUNCPTR) eiIoctl,
                    (FUNCPTR) ether_output, /* generic ether_output */
                    (FUNCPTR) eiReset
                    );

    pDrvCtrl->idr.ac_if.if_start = (FUNCPTR)eiTxStartup;

    /* initialize the unit */

    if (eiInit (unit) == ERROR)
        goto error;

    /* Set our flag */

    pDrvCtrl->attached = TRUE;

    /* Set status flags in IDR */

    pDrvCtrl->idr.ac_if.if_flags |= (IFF_UP | IFF_RUNNING | IFF_NOTRAILERS);

    /* Successful return */

    return (OK);

    /***** Handle error cases *****/

    error:

        {
        /* Release system memory */

        if (((int) memBase == NONE) && ((int) pDrvCtrl->memBase != NULL))
            cacheDmaFree (pDrvCtrl->memBase);

        /* Release watchdog */

        if (pDrvCtrl->wid != NULL)
            wdDelete (pDrvCtrl->wid);

        return (ERROR);
        }
    }


/*******************************************************************************
*
* eiInit - Initialize the interface.
*
* Initialization of interface; clear recorded pending operations.
* Called at boot time and by eiWatchDog() if a reset is required.
*
* RETURNS: OK or ERROR
*/

static STATUS eiInit 
    (
    int unit			/* unit number */
    )
    {
    DRV_CTRL    *pDrvCtrl = & drvCtrl [unit];

    pDrvCtrl->rcvHandling   = FALSE;  /* netTask not running our receive job */
    pDrvCtrl->txCleaning    = FALSE;  /* netTask not running our clean job */
    pDrvCtrl->txIdle        = TRUE;         /* transmitter idle */
    eiQInit ((EI_LIST *)&pDrvCtrl->rxQueue);    /* to be received queue */
    eiQInit ((EI_LIST *)&pDrvCtrl->txQueue);    /* to be sent queue */
    eiQInit ((EI_LIST *)&pDrvCtrl->tfdQueue);   /* free tfds to add to send q */
    eiQInit ((EI_LIST *)&pDrvCtrl->cblQueue);   /* actively sending queue */
    eiQInit ((EI_LIST *)&pDrvCtrl->cleanQueue); /* queue of TFDs to clean */

    pDrvCtrl->wdTxTimeout = 0;
    pDrvCtrl->wdRxTimeout = 0;


    { /***** Perform device initialization *****/

    /* Block local variables */

    int ix;

    sys596IntDisable (unit);            /* disable device interrupts */
    (void) sys596Init (unit);           /* do board specific init */

    /* Connect the interrupt handler */

    if (intConnect ((VOIDFUNCPTR *)INUM_TO_IVEC (pDrvCtrl->ivec),
		    eiInt, (int)pDrvCtrl) == ERROR)
        return (ERROR);

    /* Start the device */

    if ( eiDeviceStart (unit) == ERROR )
        return (ERROR);

    eiDiag (unit);                             /* run diagnostics */
    eiConfig (unit);                           /* configure chip */
    eiIASetup (unit);                          /* setup address */

    for (ix = 0; ix < pDrvCtrl->nTFDs; ix ++)  /* tank up the free tfd queue */
        {
        eiQPut  (
                unit,
                (EI_LIST *) & pDrvCtrl->tfdQueue,
                (EI_NODE *) & pDrvCtrl->tfdPool [ix]
                );
        }

    pDrvCtrl->pFreeRfd  = NULL;                  /* first free RFD */
    pDrvCtrl->nLoanRfds = MAX_RFDS_LOANED;       /* number of loanable RFD's */

    for (ix = 0; ix < pDrvCtrl->nRFDs; ix ++)   /* tank up the receive queue */
	{
	if (pDrvCtrl->rfdPool[ix].refCnt == 0)
	        eiRxQPut (pDrvCtrl, & pDrvCtrl->rfdPool [ix]);
	}

    sys596IntAck (unit);                     /* clear any pended dev ints */
    sys596IntEnable (unit);                  /* enable interrupts at system */

    } /* End block */

    wdCancel(pDrvCtrl->wid);                  /* in case re-initializing */
    wdStart(pDrvCtrl->wid, 
	    (int) pDrvCtrl->wdInterval, 
	    (FUNCPTR) eiWatchDog, 
	    pDrvCtrl->idr.ac_if.if_unit
	   );

    return (OK);
    }

/*******************************************************************************
*
* eiReset - reset the ei interface
*
* Mark interface as inactive and reset the chip.
*/

static void eiReset
    (
    int unit
    )
    {
    DRV_CTRL *pDrvCtrl;

    pDrvCtrl = & drvCtrl [unit];

    pDrvCtrl->idr.ac_if.if_flags = 0;
    sys596IntDisable (unit);                    /* disable ints from the dev */
    sys596Port (unit, PORT_RESET, NULL);        /* reset the 596 chip */

    }

/*******************************************************************************
*
* eiIoctl - interface ioctl procedure
*
* Process an interface ioctl request.
*/

static int eiIoctl
    (
    IFNET * 	pIfNet,
    int 	cmd,
    caddr_t 	data
    )
    {
    int error;

#ifdef EI_DEBUG
    printf ("ei: ioctl: pIDR=%x cmd=%x data=%x\n", pIDR, cmd, data);
#endif /* EI_DEBUG */

    error = 0;

    switch (cmd)
        {
        case SIOCSIFADDR:
            ((struct arpcom *)pIfNet)->ac_ipaddr = IA_SIN (data)->sin_addr;
            arpwhohas (pIfNet, &IA_SIN (data)->sin_addr);
            break;

        case SIOCSIFFLAGS:
            /* Flags are set outside this module. No additional work to do. */
            break;

        default:
            error = EINVAL;
        }

    return (error);
    }

/*******************************************************************************
*
* eiTxStartup - start output on the chip
*
* Looks for any action on the queue, and begins output if there is anything
* there.  This routine is called from several possible threads.  Each will be
* described below.
*
* The first, and most common thread, is when a user task requests the
* transmission of data.  This will cause eiOutput() to be called, which will
* cause ether_output() to be called, which will cause this routine to be
* called (usually).  This routine will not be called if ether_output() finds
* that our interface output queue is full.  In this case, the outgoing data
* will be thrown out.
*
* The second, and most obscure thread, is when the reception of certain
* packets causes an immediate (attempted) response.  For example, ICMP echo
* packets (ping), and ICMP "no listener on that port" notifications.  All
* functions in this driver that handle the reception side are executed in the
* context of netTask().  Always.  So, in the case being discussed, netTask() 
* will receive these certain packets, cause IP to be stimulated, and cause the
* generation of a response to be sent.  We then find ourselves following the
* thread explained in the second example, with the important distinction that
* the context is that of netTask().
*
* The third thread occurs when this routine runs out of TFDs and returns.  If
* this occurs when our output queue is not empty, this routine would typically
* not get called again until new output was requested.  Even worse, if the
* output queue was also full, this routine would never get called again and
* we would have a lock state.  It DOES happen.  To guard against this, the
* transmit clean-up handler detects the out-of-TFDs state and calls this
* function.  The clean-up handler also runs from netTask.
*
* Note that this function is ALWAYS called between an splnet() and an splx().
* This is true because netTask(), and ether_output() take care of
* this when calling this function.  Therefore, no calls to these spl functions
* are needed anywhere in this output thread.
*/

void eiTxStartup
    (
    DRV_CTRL *pDrvCtrl
    )
    {
    MBUF * pMbuf;
    int length;
    TFD * pTfd;

    /*
     * Loop until there are no more packets ready to send or we
     * have insufficient resources left to send another one.
     */
    while (pDrvCtrl->idr.ac_if.if_snd.ifq_head)
        {
        IF_DEQUEUE (&pDrvCtrl->idr.ac_if.if_snd, pMbuf);  /* dequeue a packet */
        /* get free tfd */
        pTfd = (TFD *) eiQGet ((EI_LIST *)&pDrvCtrl->tfdQueue);

        if (pTfd == NULL)
	    {
	    m_freem (pMbuf);
            break;                              /* out of TFD's */
	    }

        copy_from_mbufs ((ETH_HDR *)pTfd->enetHdr, pMbuf, length);
        length = max (ETHERSMALL, length);

        if ((etherOutputHookRtn != NULL) &&
            (* etherOutputHookRtn)
            (&pDrvCtrl->idr, (ETH_HDR *)pTfd->enetHdr, length))
            continue;

        pTfd->count = length & ~0xc000;     /* fill in length */

        eiTxQPut (pDrvCtrl, pTfd);
        }
    }

/*******************************************************************************
*
* eiInt - entry point for handling interrupts from the 82596
*
* The interrupting events are acknowledged to the device, so that the device
* will deassert its interrupt signal.  The amount of work done here is kept
* to a minimum; the bulk of the work is defered to the netTask.  Several flags
* are used here to synchronize with task level code and eliminate races.
*/

static void eiInt
    (
    DRV_CTRL *pDrvCtrl
    )
    {
    UINT16  event;
    SCB *pScb;
    int unit;
    pScb = pDrvCtrl->pScb;
    unit = pDrvCtrl->idr.ac_if.if_unit;

    event = pScb->scbStatus & (SCB_S_CX | SCB_S_FR | SCB_S_CNA | SCB_S_RNR);

#ifdef EI_DEBUG
    logMsg ("ei: interrupt: event=%x\n", event, 0, 0, 0, 0, 0);
#endif /* EI_DEBUG */

    eiCommand (pDrvCtrl, event);                        /* ack the events */

    sys596IntAck (unit);                                /* ack 596 interrupt */

    /* Handle transmitter interrupt */

    if (event & SCB_S_CNA)                              /* reclaim tx tfds */
        {
        pDrvCtrl->wdTxTimeout = 0;              /* reset timeout count */

        if (pDrvCtrl->cleanQueue.head == NULL)      /* clean queue empty */
            {
            pDrvCtrl->cleanQueue.head = pDrvCtrl->cblQueue.head; /* new head */
            pDrvCtrl->cleanQueue.tail = pDrvCtrl->cblQueue.tail; /* new tail */
            }
        else                                            /* concatenate queues */
            {
            pDrvCtrl->cleanQueue.tail->pNext =
                        (EI_NODE*) pDrvCtrl->cblQueue.head;
            pDrvCtrl->cleanQueue.tail = pDrvCtrl->cblQueue.tail;
            }

        if (!pDrvCtrl->txCleaning)                          /* not cleaning? */
            {
            pDrvCtrl->txCleaning = TRUE;                    /* set flag */
            netJobAdd ((FUNCPTR) eiTxCleanQ, (int) pDrvCtrl, 0, 0, 0, 0);
                            /* defer cleanup */
            }

        if (pDrvCtrl->txQueue.head != NULL)             /* anything to flush? */
            eiTxQFlush (pDrvCtrl);                      /* flush the tx q */
        else
            pDrvCtrl->txIdle = TRUE;                    /* transmitter idle */
        }

    /* Handle receiver interrupt */

    if (event & SCB_S_FR)
	{
	pDrvCtrl->wdRxTimeout = 0;                 	/* reset timeout count */
	if (!(pDrvCtrl->rcvHandling))
            {
            pDrvCtrl->rcvHandling = TRUE;
            (void) netJobAdd ((FUNCPTR) eiHandleRecvInt, (int) pDrvCtrl,0, 0, 0, 0);         /* netTask processes */
	    }
        }
    }

/*******************************************************************************
*
* eiTxCleanQ - checks errors in completed TFDs and moves TFDs to free queue
*
* This routine is executed by netTask.  It "cleans" the TFDs on the clean-up
* queue by checking each one for errors and then returning the TFD to the
* "free TFD" queue.  The startup routine is sometimes called here to eliminate
* the lock-out case where the driver input queue is full but there are no
* TFDs available.
*/

static void eiTxCleanQ
    (
    DRV_CTRL *pDrvCtrl
    )
    {
    TFD *pTfd;
    BOOL needTxStart;
    int unit;

    unit = pDrvCtrl->idr.ac_if.if_unit;

    do
        {
        pDrvCtrl->txCleaning = TRUE;

        if (pDrvCtrl->tfdQueue.head == NULL)              /* tfd queue empty */
            needTxStart = TRUE;                           /* set flag */
        else
            needTxStart = FALSE;

        /* process transmitted frames */

        while (1)
            {

            /* Get TFD. No ints allowed while manipulating this queue. */

            sys596IntDisable (unit);
            pTfd = (TFD*) eiQGet ((EI_LIST *)&pDrvCtrl->cleanQueue);
            sys596IntEnable (unit);
            if (pTfd == NULL)
                break;

            pDrvCtrl->idr.ac_if.if_collisions +=         /* add any colls */
                (pTfd->status & CFD_S_RETRY) ? 16 :  /* excessive colls */
                (pTfd->status & CFD_S_COLL_MASK);    /* some colls */

            if (!(pTfd->status & CFD_S_OK))          /* packet not sent */
                {
                pDrvCtrl->idr.ac_if.if_oerrors++;        /* incr err count */
                pDrvCtrl->idr.ac_if.if_opackets--;       /* decr sent count */
                }

            /* return to tfdQ */
            eiQPut (unit,(EI_LIST *)&pDrvCtrl->tfdQueue, (EI_NODE*)pTfd);
            }

        if (needTxStart)                                  /* check flag */
            eiTxStartup (pDrvCtrl);

        pDrvCtrl->txCleaning = FALSE;
        }
    while (pDrvCtrl->cleanQueue.head != NULL);            /* check again */
    }

/*******************************************************************************
*
* eiHandleRecvInt - task level interrupt service for input packets
*
* This routine is called at task level indirectly by the interrupt
* service routine to do any message received processing.
*/

static void eiHandleRecvInt
    (
    DRV_CTRL *pDrvCtrl
    )
    {
    RFD *pRfd;

    do
        {
        pDrvCtrl->rcvHandling = TRUE;             /* interlock with eiInt() */

        while ((pRfd = eiRxQGet (pDrvCtrl)) != NULL)
            if (eiReceive (pDrvCtrl, pRfd) == OK)
                eiRxQPut (pDrvCtrl, pRfd);

        pDrvCtrl->rcvHandling = FALSE;            /* interlock with eiInt() */
        }
    while (eiRxQFull (pDrvCtrl));              /* make sure rx q still empty */

    }

/*******************************************************************************
*
* eiReceive - pass a received frame to the next layer up
*
* Strips the Ethernet header and passes the packet to the appropriate
* protocol.  The return value indicates if buffer loaning was used to hold
* the data.  A return value of OK means that loaning was not done, and it
* is therefore 'ok' to return the RFD to the Rx queue.  A return value of ERROR
* means that buffer loaning was employed, and so the RFD is still in use and
* should not be returned to the Rx queue.  In this latter case, the RFD will
* eventually be returned by the protocol, via a call to our eiLoanFree().
*/

static STATUS eiReceive
    (
    DRV_CTRL *pDrvCtrl,
    RFD *pRfd
    )
    {
    ETH_HDR     *pEh;
    u_char      *pData;
    int         len;
    UINT16      etherType;
    MBUF        *m = NULL;
    BOOL        rfdLoaned = FALSE;


    /* Check packet for errors.  This should be completely unnecessary,
     * but since Intel does not seem capable of explaining the exact
     * functioning of the 'save bad frames' config bit, we will look for
     * errors.
     */

    if  (
        ( pRfd->status & ( RFD_S_OK | RFD_S_COMPLETE ) ) !=
        ( RFD_S_OK | RFD_S_COMPLETE )
        )
        {
        ++pDrvCtrl->idr.ac_if.if_ierrors;            /* bump error counter */
        eiRxQPut (pDrvCtrl, pRfd);                   /* free the RFD */
        return (ERROR);
        }

    /* Bump input packet counter. */

    ++pDrvCtrl->idr.ac_if.if_ipackets;

    len = pRfd->actualCnt & ~0xc000;        /* get frame length */
    pEh = (ETH_HDR *)pRfd->enetHdr;         /* get ptr to ethernet header */

    /* Service input hook */

    if ( (etherInputHookRtn != NULL) )
        {
        if  ( (* etherInputHookRtn) (&pDrvCtrl->idr, (char *)pEh, len) )
            {
            eiRxQPut (pDrvCtrl, pRfd);                  /* free the RFD */
            return (OK);
            }
        }

    len -= EH_SIZE;
    pData = (u_char *) pRfd->enetData;
    etherType = ntohs (pEh->ether_type);

    /* we can loan out receive frames from 82596 receive queue if:
     *
     * 1) the threshold of loanable frames has not been exceeded
     * 2) size of the input ethernet frame is large enough to be used with
     *    clustering.
     */
    if ((pDrvCtrl->nLoanRfds > 0) && 
        ((m = build_cluster (pData, len, &pDrvCtrl->idr, MC_EI, &pRfd->refCnt,
                 eiLoanFree, (int) pDrvCtrl, (int) pRfd, NULL)) != NULL))
        {
        pDrvCtrl->nLoanRfds --;             /* one less to loan */
        rfdLoaned = TRUE;               /* we loaned a frame */
        }
    else 
        m = copy_to_mbufs (pData, len, 0, &pDrvCtrl->idr);

    if (m != NULL)
#ifdef BSD43_DRIVER
        do_protocol_with_type (etherType, m, &pDrvCtrl->idr, len);
#else
        do_protocol (pEh, m, &pDrvCtrl->idr, len);
#endif 
    else
        pDrvCtrl->idr.ac_if.if_ierrors++;
        
    return ((rfdLoaned) ? ERROR : OK);
    }

/*******************************************************************************
*
* eiLoanFree - return a loaned receive frame descriptor
*
* This routine is called by the protocol code when it has completed use of
* an RFD that we loaned to it.
*/

static void eiLoanFree
    (
    DRV_CTRL *pDrvCtrl,
    RFD *pRfd
    )
    {
    eiRxQPut (pDrvCtrl, pRfd);
    pDrvCtrl->nLoanRfds ++;
    }

/*******************************************************************************
*
* eiDeviceStart - reset and start the device
*
* This routine assumes interrupts from the device have been disabled, and
* that the driver control structure has been initialized.
*/

static STATUS eiDeviceStart
    (
    int unit                              /* physical unit number */
    )
    {
    void     *pTemp;
    DRV_CTRL *pDrvCtrl;
    SCP      *pScp;                       /* system config ptr */
    ISCP     *pIscp;                      /* intermediate system config ptr */
    SCB      *pScb;                       /* system control block ptr */

    /* Get pointers */

    pDrvCtrl = & drvCtrl [unit];
    pScp = pDrvCtrl->pScp;
    pIscp = pDrvCtrl->pIscp;
    pScb = pDrvCtrl->pScb;

    /* Issue the reset operation to the device */

    sys596Port (unit, PORT_RESET, NULL);

    /* Initialize the SCP */

    pScp->scpRsv1 =
    pScp->scpRsv2 =
    pScp->scpRsv3 = 0;
    pScp->scpSysbus = pDrvCtrl->sysbus;

    pTemp = CACHE_DRV_VIRT_TO_PHYS (&pDrvCtrl->cacheFuncs, pIscp);
    LINK_WR (&pScp->pIscp, pTemp);              /* point SCP to ISCP */

    /* Initialize the ISCP */

    pIscp->iscpBusy = 1;
    pIscp->iscpRsv1 = 0;

    pTemp = CACHE_DRV_VIRT_TO_PHYS (&pDrvCtrl->cacheFuncs, pScb);
    LINK_WR (&pIscp->pScb, pTemp);              /* point ISCP to SCB */

    /* Initialize the SCB */

    bzero ((char *)pScb, sizeof (SCB));

    /* Tell the device where the SCP is located */

    pTemp = CACHE_DRV_VIRT_TO_PHYS (&pDrvCtrl->cacheFuncs, pScp);
    sys596Port (unit, PORT_NEWSCP, (UINT32) pTemp);
    sys596ChanAtn (unit);

    /*
     * The device will now read our SCP and ISCP. It will clear the busy
     * flag in the ISCP.
     */

    taskDelay (50);
    if ( pIscp->iscpBusy == 1 )
        {
        printf ("\nei: device did not initialize\n");
        return (ERROR);
        }

    return (OK);
    }

/*******************************************************************************
*
* eiDiag - format and issue a diagnostic command
*/

static void eiDiag
    (
    int unit
    )
    {
    DRV_CTRL *pDrvCtrl;

    pDrvCtrl = & drvCtrl [unit];

    bzero ((char *)pDrvCtrl->pCfd, sizeof (CFD));       /* zero command frame */

    eiAction (unit, CFD_C_DIAG);                /* run diagnostics */

    if (!(pDrvCtrl->pCfd->cfdStatus & CFD_S_OK))
        printErr ("eiDiag: i82596 diagnostics failed.\n");
    }

/*******************************************************************************
*
* eiConfig - format and issue a config command
*/

static void eiConfig
    (
    int unit
    )
    {
    DRV_CTRL *pDrvCtrl;

    pDrvCtrl = & drvCtrl [unit];

    bzero ((char *)pDrvCtrl->pCfd, sizeof (CFD));       /* zero command frame */

    /* Recommeded i82596 User's Manual configuration values.  Note that
     * the original manual, #296443-001, was full of errors.  Errors in the
     * description of the config bytes that I am aware of are noted below.
     * It is possible there are further errors.  Intel has replaced the
     * manual with #296853-001, which does correct some errors, but not all.
     */

    pDrvCtrl->pCfd->cfdConfig.ccByte8  = 0x8e;
    pDrvCtrl->pCfd->cfdConfig.ccByte9  = 0xc8;

    /* The manual is wrong about bit 7 in byte 10.
     * A '0' allows reception of bad packets.
     * A '1' causes rejection of bad packets.
     */
    pDrvCtrl->pCfd->cfdConfig.ccByte10 = 0xc0;

    pDrvCtrl->pCfd->cfdConfig.ccByte11 = 0x2e;      /* loopback, NSAI */
    pDrvCtrl->pCfd->cfdConfig.ccByte12 = 0x00;
    pDrvCtrl->pCfd->cfdConfig.ccByte13 = 0x60;
    pDrvCtrl->pCfd->cfdConfig.ccByte14 = 0x00;
    pDrvCtrl->pCfd->cfdConfig.ccByte15 = 0xf2;
    pDrvCtrl->pCfd->cfdConfig.ccByte16 = 0x00;      /* promiscuous off */
    pDrvCtrl->pCfd->cfdConfig.ccByte17 = 0x00;
    pDrvCtrl->pCfd->cfdConfig.ccByte18 = 0x40;

    /* The manual is wrong about 2 bits in byte 19.
     * Bit 5, multicast, a '1' disables.
     * Bit 2, include CRC, a '1' disables.
     */
    pDrvCtrl->pCfd->cfdConfig.ccByte19 = 0xff;

    pDrvCtrl->pCfd->cfdConfig.ccByte20 = 0x00;
    pDrvCtrl->pCfd->cfdConfig.ccByte21 = 0x3f;


    eiAction (unit, CFD_C_CONFIG);          /* configure the chip */
    }

/*******************************************************************************
*
* eiIASetup - format and issue an interface address command
*/

static void eiIASetup
    (
    int unit
    )
    {
    DRV_CTRL *pDrvCtrl;

    pDrvCtrl = & drvCtrl [unit];

    bzero ((char *)pDrvCtrl->pCfd, sizeof (CFD));       /* zero command frame */

    bcopy   (
            (char *)pDrvCtrl->idr.ac_enaddr,
            (char *)pDrvCtrl->pCfd->cfdIASetup.ciAddress,
            6
            );

    eiAction (unit, CFD_C_IASETUP);         /* setup the address */
    }

/*******************************************************************************
*
* eiRxStartup - start up the Receive Unit
*
* Starts up the Receive Unit.  Assumes that the receive structures are set up.
*/

static void eiRxStartup
    (
    DRV_CTRL *pDrvCtrl
    )
    {
    SCB *pScb = pDrvCtrl->pScb;
    void * pTemp;

    if (pScb->scbStatus & SCB_S_RUREADY)        /* already running */
        return;

    pTemp = CACHE_DRV_VIRT_TO_PHYS (&pDrvCtrl->cacheFuncs, pDrvCtrl->pFreeRfd);
    LINK_WR (&pScb->pRF, pTemp);            /* point to free Rfd */

    sys596IntDisable (pDrvCtrl->idr.ac_if.if_unit);

    if ((pScb->scbStatus & SCB_S_RUMASK) != SCB_S_RUIDLE)
        eiCommand (pDrvCtrl, SCB_C_RUABORT);            /* abort if not idle */

    eiCommand (pDrvCtrl, SCB_C_RUSTART);              /* start receive unit */
    sys596IntEnable (pDrvCtrl->idr.ac_if.if_unit);

    }

/*******************************************************************************
*
* eiAction - execute the specified action with the CFD pointed to in pDrvCtrl
*
* Do the command contained in the CFD synchronously, so that we know
* it's complete upon return.  This routine assumes that interrupts from the
* device have been disabled.
*/

static void eiAction
    (
    int    unit,
    UINT16 action
    )
    {
    void     *pTemp;
    CFD      *pCfd;
    SCB      *pScb;
    DRV_CTRL *pDrvCtrl;

    pDrvCtrl = & drvCtrl [unit];
    pCfd = pDrvCtrl->pCfd;
    pScb = pDrvCtrl->pScb;

    while (1)                   /* wait for idle command unit */
        {
        if ((pScb->scbStatus & SCB_S_CUMASK) == SCB_S_CUIDLE)
            break;
        }

    { /* Prepare and issue the command to the device */

    /* fill in CFD */

    pCfd->cfdStatus  = 0;                       /* clear status */
    pCfd->cfdCommand = CFD_C_EL | action;       /* fill in action */
    LINK_WR (&pCfd->link, NULL);                /* terminate link */

    /* and the SCB */

    pScb->scbCommand =
                        SCB_S_CX |              /* ack any events */
                        SCB_S_FR |
                        SCB_S_CNA |
                        SCB_S_RNR |
                        SCB_C_CUSTART;          /* start command unit */

    pTemp = CACHE_DRV_VIRT_TO_PHYS (&pDrvCtrl->cacheFuncs, pCfd);
    LINK_WR (&pScb->pCB, pTemp);                /* point chip at CFD */

    sys596ChanAtn (unit);               /* notify device of new command */
    }

    while (1)               /* wait for command acceptance and interrupt */
        {
        if ((pScb->scbCommand == 0) && (pScb->scbStatus & SCB_S_CNA))
            break;
        }

    /* Acknowledge the event to the device */

    pScb->scbCommand = (SCB_S_CX | SCB_S_CNA);
    sys596ChanAtn (unit);

    while (1)               /* wait for acknowledge acceptance */
        {
        if (pScb->scbCommand == 0)
            break;
        }
    }

/*******************************************************************************
*
* eiCommand - deliver a command to the 82596 via SCB
*
* This function causes the device to execute a command.  It should be called
* with interrupts from the device disabled.  An error status is returned if
* the command field does not return to zero, from a previous command, in a
* reasonable amount of time.
*/

static STATUS eiCommand
    (
    DRV_CTRL *pDrvCtrl,
    UINT16    cmd
    )
    {
    int loopy;
    SCB * pScb;

    pScb = pDrvCtrl->pScb;

    for (loopy = 0x8000; loopy--;)
        {
        if (pScb->scbCommand == 0)                  /* wait for cmd zero */
            break;
        }

    if (loopy > 0)
        {
        pScb->scbCommand = cmd;                     /* fill in command */
        sys596ChanAtn (pDrvCtrl->idr.ac_if.if_unit);    /* channel attention */
        return (OK);
        }
    else
        {
        logMsg("ei driver: command field frozen\n", 0, 0, 0, 0, 0, 0);
        return (ERROR);
        }
    }

/*******************************************************************************
*
* eiTxQPut - place a transmit frame on the transmit queue
*
* The TFD has been filled in with the network pertinent data.  This
* routine will enqueue the TFD for transmission and attempt to feed
* the queue to the device.
*/

static void eiTxQPut
    (
    DRV_CTRL *pDrvCtrl,
    TFD *pTfd
    )
    {
    int unit;

    unit = pDrvCtrl->idr.ac_if.if_unit;

    pTfd->status    = 0;                    /* fill in TFD fields */
    pTfd->command   = CFD_C_XMIT;           /* EL set later */
    pTfd->count     |= TFD_CNT_EOF;         /* data kept in frame */
    pTfd->reserved  = 0;                    /* must be zero */

    LINK_WR (& pTfd->lBufDesc, NULL);                /* TBDs not used */

    sys596IntDisable (pDrvCtrl->idr.ac_if.if_unit);  /* disable dev ints */

    /* enqueue the TFD */
    eiQPut (unit,(EI_LIST *)&pDrvCtrl->txQueue, (EI_NODE*)pTfd);

    if (pDrvCtrl->txIdle)                             /* transmitter idle */
        eiTxQFlush (pDrvCtrl);                        /* flush txQueue */

    sys596IntEnable (pDrvCtrl->idr.ac_if.if_unit);  /* enable dev ints */
    }

/*******************************************************************************
*
* eiTxQFlush - make cmd unit of device start processing cmds
*
* This routine flushes the contents of the txQ to the cblQ and starts the
* device transmitting the cblQ.  Called only if transmit queue is not empty.
* Sometimes called from interrupt handler.
*/

static void eiTxQFlush
    (
    DRV_CTRL *pDrvCtrl
    )
    {
    void * pTemp;
    extern int sysClkRateGet();     /* we call this */

    ((TFD*)pDrvCtrl->txQueue.tail)->command |= CFD_C_EL;  /* EL terminate q */
    pDrvCtrl->cblQueue.head = pDrvCtrl->txQueue.head;   /* remember cbl head */
    pDrvCtrl->cblQueue.tail = pDrvCtrl->txQueue.tail;   /* remember cbl tail */
    eiQInit ((EI_LIST *)&pDrvCtrl->txQueue);            /* tx queue now empty */

    pTemp = CACHE_DRV_VIRT_TO_PHYS (&pDrvCtrl->cacheFuncs,
                                    pDrvCtrl->cblQueue.head);
    LINK_WR (&pDrvCtrl->pScb->pCB, pTemp);          /* point CU to head */

    pDrvCtrl->txIdle = FALSE;                   /* transmitter busy */

    /* start command unit */

    eiCommand (pDrvCtrl, SCB_C_CUSTART);
    }

/*******************************************************************************
*
* eiRxQPut - return a RFD to the receive queue for use by the device
*/

static void eiRxQPut
    (
    DRV_CTRL *pDrvCtrl,
    RFD *pRfd
    )
    {
    int unit;
    RFD *pTail;

    unit = pDrvCtrl->idr.ac_if.if_unit;

    pRfd->status    = 0;                    /* clear status */
    pRfd->command   = CFD_C_EL;             /* new end of list */
    pRfd->actualCnt  = 0;                   /* clear actual count */
    pRfd->bufSize   = ETHERMTU + EH_SIZE;   /* fill in size */
    pRfd->refCnt    = 0;                    /* initialize ref cnt */

    LINK_WR (& pRfd->lBufDesc, 0xffffffff);         /* no RBD used */

    pTail = (RFD *) pDrvCtrl->rxQueue.tail;         /* remember tail */

    /* Put the RFD on the list */

    eiQPut (unit, (EI_LIST *) & pDrvCtrl->rxQueue, (EI_NODE *) pRfd);

    if (pTail != NULL)
        {
        pDrvCtrl->wdRxTimeout = 0;          /* reset timeout count */

        pTail->command &= ~CFD_C_EL;        /* clear old tail EL */

        if (pTail->status & (CFD_S_COMPLETE | CFD_S_BUSY))
            {
            pDrvCtrl->pFreeRfd = pRfd;          /* link questionable */
            }
        else if (!(pDrvCtrl->pScb->scbStatus & SCB_S_RUREADY))
            /* receiver dormant */
            {
            eiRxStartup (pDrvCtrl);             /* start receive unit */
            }
        }
    else
        {
        pDrvCtrl->pFreeRfd = pRfd;              /* first free RFD */
        }
    }

/*******************************************************************************
*
* eiRxQGet - get a successfully received frame from the receive queue
*
* RETURNS: ptr to valid RFD, or NULL if none available
*/

static RFD *eiRxQGet
    (
    DRV_CTRL *pDrvCtrl
    )
    {
    RFD *pRfd = NULL;

    if (eiRxQFull (pDrvCtrl))
        pRfd = (RFD *) eiQGet ((EI_LIST *)&pDrvCtrl->rxQueue);

    return (pRfd);
    }

/*******************************************************************************
*
* eiRxQFull - boolean function to determine fullness of receive queue
*
* RETURNS: TRUE if completely received frame is available, FALSE otherwise.
*/

static BOOL eiRxQFull
    (
    DRV_CTRL *pDrvCtrl
    )
    {
    return ((pDrvCtrl->rxQueue.head != NULL) &&
        (((RFD*)pDrvCtrl->rxQueue.head)->status & CFD_S_COMPLETE));
    }

/*******************************************************************************
*
* eiQInit - initialize a singly linked node queue
*/

static void eiQInit
    (
    EI_LIST *pQueue
    )
    {
    pQueue->head = pQueue->tail = NULL;         /* init head & tail */
    }

/*******************************************************************************
*
* eiQGet - get a node from the head of a node queue
*
* RETURNS: ptr to useable node, or NULL ptr if none available
*/

static EI_NODE *eiQGet
    (
    EI_LIST *pQueue
    )
    {
    EI_NODE *pNode;

    if ((pNode = (EI_NODE *) pQueue->head) != NULL)     /* if list not empty */
        pQueue->head = pNode->pNext;                    /* advance ptr */

    return (pNode);
    }

/*******************************************************************************
*
* eiQPut - put a node on the tail of a node queue
*/

static void eiQPut
    (
    int unit,
    EI_LIST *pQueue,
    EI_NODE *pNode
    )
    {
    void * pTemp;
    DRV_CTRL *pDrvCtrl;

    pDrvCtrl = & drvCtrl [unit];

    LINK_WR (&pNode->lNext, NULL);                    /* mark "end of list" */
    pNode->pNext = NULL;

    if (pQueue->head == NULL)                        /* if list empty */
        pQueue->tail = pQueue->head = pNode;         /* set both ptrs */
    else
        {
        pTemp = CACHE_DRV_VIRT_TO_PHYS (&pDrvCtrl->cacheFuncs, pNode);
        LINK_WR (&pQueue->tail->lNext, pTemp);       /* link node on tail */
        pQueue->tail->pNext = pNode;
        pQueue->tail = pNode;                        /* update tail ptr */
        }
    }

/*******************************************************************************
*
* eiWatchDog - if the watchdog timer fired off, we've hung during a transmit
*
* Check the scb command to verify and if so, reinit.
*/

static void eiWatchDog
    (
    int unit        /* unit number */
    )

    {
    DRV_CTRL *pDrvCtrl = &drvCtrl [unit];
    SCB *pScb;
    int reset = FALSE;
    pScb = pDrvCtrl->pScb;


    /* Test for transmit timeout.
     *
     * Timeout occurs if the scb status indicates that CU (transmit) 
     * remains active for EI_TX_TIMEOUT iterations of eiWatchDog.
     * It might make sense to loop through the cfd's to look for
     * a complete bit as a sanity check, but given that transmit
     * was active, we will go ahead and do a reset.
     */
    if ((pDrvCtrl->txIdle == FALSE)
        && (pScb->scbStatus & SCB_S_CUACTIVE))
        {
        if (++(pDrvCtrl->wdTxTimeout) > EI_TX_TIMEOUT)
            {
            pDrvCtrl->transLocks++;			  /* failure count */
            pDrvCtrl->idr.ac_if.if_oerrors++;             /* incr err count */
            pDrvCtrl->idr.ac_if.if_opackets--;            /* decr sent count */
            reset = TRUE;
            }
        }

    /* Test for receive timeout.
     *
     * Timeout occurs if the scb status indicates that RU (receive) 
     * remains out of resources for EI_RX_TIMEOUT iterations of eiWatchDog.
     */
    if (pScb->scbStatus & SCB_S_RUNORSRC)
        {
        if (++(pDrvCtrl->wdRxTimeout) > EI_RX_TIMEOUT)
            {
            pDrvCtrl->recvLocks++;			  /* failure count */
            reset = TRUE;
            }
        }

    /* reinitialize the unit or restart the watchdog */

    if (reset)
	netJobAdd ((FUNCPTR) eiInit, unit, 0, 0, 0, 0);
    else
        wdStart (pDrvCtrl->wid, 
		(int) pDrvCtrl->wdInterval, 
		(FUNCPTR) eiWatchDog, 
		unit);
    }

/******************************************************************************/

/* END OF FILE */
