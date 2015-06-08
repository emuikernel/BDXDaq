/* if_ln.c - AMD Am7990 LANCE Ethernet network interface driver */

/* Copyright 1984-1997 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
02x,15jul97,spm  added ARP request to SIOCSIFADDR ioctl handler
02w,10jun97,spm  removed frame check sequence from length of incoming packets
02v,04jun97,spm  corrected errors introduced during merge from Sirocco tree
02u,19may97,spm  eliminated all compiler warnings
02t,15may97,spm  included changes from versions 02r and 02s of Sirocco tree
02s,23jan97,vin  upgraded to BSD44
02s,06feb96,dat  added check for inactive interrupt, at end of lnInt.
                 Added a real lnRestart function.
02r,11nov94,vin  incorporated the etherOutputHook routine.
02r,15jun94,dzb  [re]implemented buffer loaning, multiple unit support.
02q,11aug93,jmm  Changed ioctl.h and socket.h to sys/ioctl.h and sys/socket.h
02p,19feb93,jdi  documentation cleanup.
02o,13oct92,rfs  Added documentation.
02n,02oct92,rfs  ANSI warnings go away.
02m,02oct92,rfs  Made multiple attach calls per unit return OK.
02l,09sep92,gae  documentation tweaks.
02k,02aug92,rfs  Merged with cache changes from previous version, added
                 virtual address conversion and write pipe flushing.
                 Removed code that appeared to handle chained incoming packets,
                 but in fact could not do so.  Receive buffers must always be
                 large enough to hold a complete packet.  Removed promiscuous
                 support.  Moved all startup code to the attach routine.
                 Eliminated the init() routine and provided stub for a restart
                 routine that will handle the fatal errors.  Removed support
                 for buffer loaning, as it was wrong as well.  Revised the
                 ioctl() routine, which was also wrong.  Moved driver control
                 structure(s) to bss and changed name used for pointer.
                 Overhauled the transmit routines to discontinue the obsolete
                 usage of the output queue.  Added the transmit semaphore.
                 Many other misc.
02j,31jul92,dave Changed to new cacheLib.
02i,30jul92,rfs  Changed the way collisions are counted.
02h,29jul92,rfs  Typedef'd and renamed the driver control structure and,
                 changed all references to it.
02g,29jul92,rfs  Moved driver specific items here from the header file.
02f,17jul92,jwt  added missing cache funtion pointers for "default" case.
02e,13jul92,rdc  now uses cacheMalloc.
02d,12jul92,jcf  cacheMalloc() call uses malloc() for 68k 'til cacheMc68kLib.
02c,06jul92,jwt  removed SPARC sysCacheSet() function calls due to 02c;
                 forced conversion of &cacheFuncs to pointer for macros;
                 cleaned up some typos and compiler warning messages.
    02jul92,rfs  Added cache coherency support.  Added blank lines between
                 functions.
02b,26may92,rrr  the tree shuffle
                 -changed includes to have absolute path from h/
02a,27apr92,jwt  converted CPU==SPARC to CPU_FAMILY==SPARC.
01z,09oct91,jpb  rewrote to fix error handling for transmitter errors,
                 should no longer hang.
                 channel all chip accesses through new functions
                 lnCsrRead and lnCsrWrite, and minimize chip accesses
                 including eliminating transmitter kick-start in
                 lnStartOutput if selected with lnKickStartTx == FALSE.
                 modified to adapt to the LANCE chip alignment which
                 eliminated the need for the RAP padding flags.
                 fixed infinite loop on initialization failure to timeout.
                 cleaned up some ansification remnants.
01y,04oct91,rrr  passed through the ansification filter
                 -changed functions to ansi style
                 -changed includes to have absolute path from h/
                 -changed VOID to void
                 -changed copyright notice
01x,02oct91,jwt  restored lnCSR_3B global variable for Radstone Apex driver.
01w,25jul91,jwt  removed bit fields (added masks) for portability, correctness.
01v,06mar91,hvh  aligned buffers for SPARC optimizations.
01u,20feb91,jwt  added sysCacheSet() call for DMA and caching support.
01t,10apr91,jdi  documentation cleanup; doc review by elh.
01s,20sep90,dab  made lnInit() return int.
01r,10aug90,dnw  added forward declarations of void routines.
                 added include of if_subr.h.
01q,11jul90,hjb  removed references to ipintr().
01p,26jun90,hjb  copy_from_mbufs() & m_freem() fix; changed params to
                 bulid_cluster().
01o,11may90,yao  added missing modification history (01n) for the last checkin.
01n,09may90,yao  typecasted malloc to (char *).
01m,07may90,hjb  new RMD loans implementation to avoid hangups.
01l,19apr90,hjb  deleted param.h, added LN_RMD_GIVE_TO_LANCE, deleted
                 lnHandleXmitInt and ln_bcopy, added lnRmdFree to support
                 cluster-callback optimization, speed-ups in lnInt(),
                 use of LS_ flags, de-linted, modifications to receive
                 routines, added lnLoansPrint() debug routine.
01k,18mar90,hjb  reduction of redundant code and addition of cluster support.
                 added input error checking in RINT handling code to reduce
                 unnecessary netJobAdd's.  recoded the Tadpole padding check.
                 added crude LANCE data chaining support.
01j,08nov89,dab  added check for Tadpole cpu to enable padding of CSR structure.
01i,07aug89,gae  changed iv68k.h to iv.h.
01h,28jun89,hjb  fixed a bug in lnIoctl() -- SIOCSIFFLAGS handling was buggy.
01g,13jun89,hjb  changed mtu to ETHERMTU (from 1000).  check lnLogCount before
                 doing modulo operation to make sure it's not zero.
01f,12jun89,hjb  added a missing splx (s) in lnRecv ().
01e,24may89,jcf  reduced netJobAdd () calls by handling more receive packets
                 +dnw   at task level, and calling ipintr directly from lnRecv.
01d,14feb89,jcf  changed copy from/to interface to use appropriate data width.
                 changed lnattach to optionally take address of memory pool.
                 added macros ItoK(), KtoI(), etc.
                 removed superfluous lnConfig() call from lnattach().
                 +dnw  improved error reporting and recovery.
01c,07sep88,gae  added global variable lnLogCount so that dropped
                 interrupts are not normally reported.
01b,09aug88,gae  made lnattach have usual number of parameters.  Lint.
01a,28apr88,dfm  written based on other drivers (if_xx) & LANCE documentation.
*/

/*
This module implements the Advanced Micro Devices Am7990 LANCE Ethernet network
interface driver.

This driver is designed to be moderately generic, operating unmodified
across the range of architectures and targets supported by VxWorks.  To
achieve this, the driver must be given several target-specific parameters,
and some external support routines must be provided.  These parameters,
and the mechanisms used to communicate them to the driver, are detailed
below.  If any of the assumptions stated below are not true for your
particular hardware, this driver will probably not function correctly with
it.

This driver supports only one LANCE unit per CPU.  The driver can be
configured to support big-endian or little-endian architectures.  It
contains error recovery code to handle known device errata related to DMA
activity.

BOARD LAYOUT
This device is on-board.  No jumpering diagram is necessary.

EXTERNAL INTERFACE
This driver provides the standard external interface with the following
exceptions.  All initialization is performed within the attach routine;
there is no separate initialization routine.  Therefore, in the global interface
structure, the function pointer to the initialization routine is NULL.

The only user-callable routine is lnattach(), which publishes the `ln'
interface and initializes the driver and device.

TARGET-SPECIFIC PARAMETERS
.iP "bus mode"
This parameter is a global variable that can be modified at run-time.

The LANCE control register #3 determines the bus mode of the device,
allowing the support of big-endian and little-endian architectures.
This parameter, defined as "u_short lnCSR_3B", is the value that will
be placed into LANCE control register #3.  The default value supports
Motorola-type buses.  For information about changing this parameter, see 
the manual
.I "Advanced Micro Devices Local Area Network Controller Am7990 (LANCE)."

.iP "base address of device registers"
This parameter is passed to the driver by lnattach().  It 
indicates to the driver where to find the RDP register.

The LANCE presents two registers to the external interface, the RDP (register
data port) and RAP (register address port) registers.  This driver assumes 
that these two registers occupy two unique addresses in a memory space
that is directly accessible by the CPU executing this driver.  The driver
assumes that the RDP register is mapped at a lower address than the RAP
register; the RDP register is therefore considered the "base address."

.iP "interrupt vector"
This parameter is passed to the driver by lnattach().

This driver configures the LANCE device to generate hardware interrupts
for various events within the device; thus it contains
an interrupt handler routine.  The driver calls intConnect() to connect 
its interrupt handler to the interrupt vector generated as a result of 
the LANCE interrupt.

.iP "interrupt level"
This parameter is passed to the driver by lnattach().

Some targets use additional interrupt controller devices to help organize
and service the various interrupt sources.  This driver avoids all
board-specific knowledge of such devices.  During the driver's
initialization, the external routine sysLanIntEnable() is called to
perform any board-specific operations required to allow the servicing of a
LANCE interrupt.  For a description of sysLanIntEnable(), see "External
Support Requirements" below.

This parameter is passed to the external routine.

.iP "shared memory address"
This parameter is passed to the driver by lnattach().

The LANCE device is a DMA type of device and typically shares access to
some region of memory with the CPU.  This driver is designed for systems
that directly share memory between the CPU and the LANCE.  It
assumes that this shared memory is directly available to it
without any arbitration or timing concerns.

This parameter can be used to specify an explicit memory region for use
by the LANCE.  This should be done on hardware that restricts the LANCE
to a particular memory region.  The constant NONE can be used to indicate
that there are no memory limitations, in which case, the driver 
attempts to allocate the shared memory from the system space.

.iP "shared memory size"
This parameter is passed to the driver by lnattach().

This parameter can be used to explicitly limit the amount of shared
memory (bytes) this driver will use.  The constant NONE can be used to
indicate no specific size limitation.  This parameter is used only if
a specific memory region is provided to the driver.

.iP "shared memory width"
This parameter is passed to the driver by lnattach().

Some target hardware that restricts the shared memory region to a
specific location also restricts the access width to this region by
the CPU.  On these targets, performing an access of an invalid width
will cause a bus error.

This parameter can be used to specify the number of bytes of access
width to be used by the driver during access to the shared memory.
The constant NONE can be used to indicate no restrictions.

Current internal support for this mechanism is not robust; implementation 
may not work on all targets requiring these restrictions.

.iP "Ethernet address"
This parameter is obtained directly from a global memory location.

During initialization, the driver needs to know the Ethernet address for
the LANCE device.  The driver assumes that this address is available in
a global, six-byte character array, lnEnetAddr[].  This array is
typically created and stuffed by the BSP code.
.LP

EXTERNAL SUPPORT REQUIREMENTS
This driver requires one external support function:
.iP "void sysLanIntEnable (int level)" "" 9 -1
This routine provides a target-specific enable of the interrupt for
the LANCE device.  Typically, this involves interrupt controller hardware,
either internal or external to the CPU.

This routine is called once, from the lnattach() routine.
.LP

SYSTEM RESOURCE USAGE
When implemented, this driver requires the following system resources:

    - one mutual exclusion semaphore
    - one interrupt vector
    - 24 bytes in the initialized data section (data)
    - 208 bytes in the uninitialized data section (BSS)

The above data and BSS requirements are for the MC68020 architecture 
and may vary for other architectures.  Code size (text) varies greatly between
architectures and is therefore not quoted here.

If the driver is not given a specific region of memory via the lnattach()
routine, then it calls cacheDmaMalloc() to allocate the memory to be shared 
with the LANCE.  The size requested is 80,542 bytes.  If a memory region
is provided to the driver, the size of this region is adjustable to suit
user needs.

The LANCE can only be operated if the shared memory region is write-coherent
with the data cache.  The driver cannot maintain cache coherency
for the device for data that is written by the driver because fields
within the shared structures are asynchronously modified by both the driver
and the device, and these fields may share the same cache line.

SEE ALSO: ifLib, 
.I "Advanced Micro Devices Local Area Network Controller Am7990 (LANCE)"
*/

/* includes */

#include "vxWorks.h"
#include "stdlib.h"
#include "taskLib.h"
#include "logLib.h"
#include "intLib.h"
#include "netLib.h"
#include "stdio.h"
#include "stdlib.h"
#include "sysLib.h"
#include "iv.h"
#include "memLib.h"
#include "cacheLib.h"
#include "sys/ioctl.h"
#include "etherLib.h"
#include "net/mbuf.h"
#include "net/protosw.h"
#include "net/unixLib.h"
#include "sys/socket.h"
#include "errno.h"
#include "net/if.h"
#include "net/route.h"
#include "netinet/in.h"
#include "netinet/in_systm.h"
#include "netinet/in_var.h"
#include "netinet/ip.h"
#include "netinet/if_ether.h"
#include "net/if_subr.h"
#include "semLib.h"
#include "drv/netif/if_ln.h"            /* device description header */

/* defines */

#define LN_BUFSIZ	(ETHERMTU + SIZEOF_ETHERHEADER + 6)
#define LN_RMD_RLEN	5	/* default Rx ring size as a power of 2 (32) */
#define LN_TMD_TLEN	5	/* default Tx ring size as a power of 2 (32) */
#define LN_L_POOL	0x10	/* number of Rx loaner buffers in pool */
#define MAX_UNITS	4	/* maximum units supported */

/*
 * If LN_KICKSTART_TX is TRUE, the transmitter is kick-started to force a
 * read of the transmit descriptors, otherwise the internal polling (1.6msec)
 * will initiate a read of the descriptors.  This should be FALSE if there
 * is any chance of memory latency or chip accesses detaining the LANCE DMA,
 * which results in a transmitter UFLO error.  This can be changed with the
 * global lnKickStartTx below.
 */

#define LN_KICKSTART_TX TRUE

/* Cache macros */

#define LN_CACHE_INVALIDATE(address, len) \
        CACHE_DRV_INVALIDATE (&pDrvCtrl->cacheFuncs, (address), (len))

#define LN_CACHE_PHYS_TO_VIRT(address) \
        CACHE_DRV_PHYS_TO_VIRT (&pDrvCtrl->cacheFuncs, (address))

#define LN_CACHE_VIRT_TO_PHYS(address) \
        CACHE_DRV_VIRT_TO_PHYS (&pDrvCtrl->cacheFuncs, (address))

/* typedefs for external structures that are not typedef'd in their .h files */

typedef struct mbuf MBUF;
typedef struct arpcom IDR;		/* Interface Data Record wrapper */
typedef struct ifnet IFNET;		/* real Interface Data Record */
typedef struct sockaddr SOCK;
#ifndef BSD43_DRIVER
typedef struct ether_header ETH_HDR; 		
#endif /* BSD43_DRIVER */

/* the definition of the driver control structure */

typedef struct drv_ctrl
    {
    IDR		idr;			/* Interface Data Record */
    char * 	pMemPool;               /* start of LANCE memory pool */
    BOOL	attached;		/* indicates unit is attached */
    int		rmdIndex;		/* current RMD index */
    int		rringSize;		/* RMD ring size */
    int         rringLen;               /* RMD ring length (bytes) */
    ln_rmd * 	rring;			/* RMD ring start */
    char * 	rBufBase;               /* Rx buffer base address */
    int		tmdIndex;		/* current TMD index */
    int		tmdIndexC;		/* current TMD index */
    int		tringSize;		/* TMD ring size */
    int         tringLen;               /* TMD ring length (bytes) */
    ln_tmd * 	tring;			/* TMD ring start */
    char * 	tBufBase;		/* Tx buffer base address */
    SEM_ID	TxSem;			/* transmitter semaphore */
    u_char	flags;			/* misc control flags */
    int		ivec;			/* interrupt vector */
    int		ilevel;			/* interrupt level */
    LN_DEVICE * devAdrs;		/* device structure address */
    int		memWidth;		/* width of data port */
    CACHE_FUNCS	cacheFuncs;		/* cache function pointers */
    int		nLoanRx;		/* number of Rx buffers left to loan */
    char * 	lPool[LN_L_POOL];	/* receive loaner pool ptrs */
    UINT8 * 	pRefCnt[LN_L_POOL];	/* stack of reference count pointers */
    UINT8	refCnt[LN_L_POOL];	/* actual reference count values */
    } DRV_CTRL;

#define DRV_CTRL_SIZ	sizeof(DRV_CTRL)
#define DC_SIZ		sizeof(LN_DC_BUF)
#ifdef BSD43_DRIVER
#define ENET_HDR_SIZ	sizeof(ETH_HDR)
#endif 	/* BSD43_DRIVER */
#define RMD_SIZ		sizeof(ln_rmd)
#define TMD_SIZ		sizeof(ln_tmd)
#define IB_SIZ		sizeof(ln_ib)

/* definitions for the flags field */

#define LN_PROMISCUOUS_FLAG	0x1
#define LN_RX_HANDLING_FLAG	0x2

/* globals */

IMPORT void sysLanIntEnable ();
#ifdef BSD43_DRIVER
IMPORT BOOL arpresolve ();
#endif 	/* BSD43_DRIVER */

IMPORT unsigned char lnEnetAddr [];	/* Ethernet addr to load into lance */

u_short lnCSR_3B = lncsr3_BSWP ;	/* allows ext setting of bus modes */
BOOL lnKickStartTx = LN_KICKSTART_TX;

/* locals */

LOCAL DRV_CTRL drvCtrl [MAX_UNITS];	/* array of driver control structures */

LOCAL int lnTsize = LN_TMD_TLEN;	/* deflt xmit ring size as power of 2 */
LOCAL int lnRsize = LN_RMD_RLEN;	/* deflt recv ring size as power of 2 */
LOCAL int lnLPool = LN_L_POOL;

/* initial word offsets from LANCE base address to access these registers */

LOCAL u_int CSROffset = 0;
LOCAL u_int RAPOffset = 1;

/* forward declarations */

#ifdef __STDC__

LOCAL void lnReset (int unit);
LOCAL void lnInt (DRV_CTRL *pDrvCtrl);
LOCAL void lnHandleRecvInt (DRV_CTRL *pDrvCtrl);
LOCAL STATUS lnRecv (DRV_CTRL *pDrvCtrl);
#ifdef BSD43_DRIVER
LOCAL int lnOutput (IDR *ifp, MBUF *m0, SOCK *dst);
#else
LOCAL void lnStartOutput (DRV_CTRL *	pDrvCtrl);
#endif /* BSD43_DRIVER */
LOCAL int lnIoctl (IDR *ifp, int cmd, caddr_t data);
LOCAL void lnChipReset (DRV_CTRL *pDrvCtrl);
LOCAL BOOL lnGetFullRMD (DRV_CTRL *pDrvCtrl);
LOCAL u_short lnCsr0Read (DRV_CTRL * pDrvCtrl);
LOCAL void lnCsr0Write (DRV_CTRL * pDrvCtrl, u_short value);
LOCAL void lnCsrWrite (DRV_CTRL * pDrvCtrl, int reg, u_short value);
LOCAL void lnRestart (int unit);
LOCAL void lnRestartSetup (DRV_CTRL * pDrvCtrl);
#ifdef BSD43_DRIVER
LOCAL BOOL convertDestAddr (IDR *pIDR, SOCK *pDestSktAddr, char *pDestEnetAddr,
			     u_short *pPacketType, MBUF *pMbuf);
#endif /* BSD43_DRIVER */
LOCAL void lnLoanFree (DRV_CTRL *pDrvCtrl, char *pRxBuf, UINT8 *pRef);

#else	/* __STDC__ */

LOCAL void lnReset ();
LOCAL void lnInt ();
LOCAL void lnHandleRecvInt ();
LOCAL STATUS lnRecv ();
#ifdef BSD43_DRIVER
LOCAL int lnOutput ();
#else
LOCAL void lnStartOutput ();
#endif /* BSD43_DRIVER */
LOCAL int lnIoctl ();
LOCAL void lnChipReset ();
LOCAL BOOL lnGetFullRMD ();
LOCAL u_short lnCsr0Read ();
LOCAL void lnCsr0Write ();
LOCAL void lnCsrWrite ();
LOCAL void lnRestart ();
LOCAL BOOL convertDestAddr ();
LOCAL void lnLoanFree ();

#endif	/*  __STDC__ */

/*******************************************************************************
*
* lnattach - publish the `ln' network interface and initialize driver structures
*
* This routine publishes the `ln' interface by filling in a network interface
* record and adding this record to the system list.  This routine also
* initializes the driver and the device to the operational state.
*
* The <memAdrs> parameter can be used to specify the location of the
* memory that will be shared between the driver and the device.  The value
* NONE is used to indicate that the driver should obtain the memory.
*
* The <memSize> parameter is valid only if the <memAdrs> parameter is not
* set to NONE, in which case <memSize> indicates the size of the
* provided memory region.
*
* The <memWidth> parameter sets the memory pool's data port width (in bytes);
* if it is NONE, any data width is used.
*
* BUGS
* To zero out LANCE data structures, this routine uses bzero(), which
* ignores the <memWidth> specification and uses any size data access to
* write to memory.
*
* RETURNS: OK or ERROR.
*/

STATUS lnattach
    (
    int       unit,		/* unit number */
    char     *devAdrs,		/* LANCE I/O address */
    int       ivec,		/* interrupt vector */
    int       ilevel,		/* interrupt level */
    char     *memAdrs,		/* address of memory pool (-1 = malloc it) */
    ULONG     memSize,		/* only used if memory pool is NOT malloc()'d */
    int       memWidth,		/* byte-width of data (-1 = any width)     */
    int       spare,		/* not used */
    int       spare2		/* not used */
    )
    {
    DRV_CTRL     *pDrvCtrl;
    char         *pShMem;	/* start of the LANCE memory pool */
    char         *buf;
    ln_rmd       *rmd;
    unsigned int sz;
    int          ix;

    /* sanity check the unit number */

    if (unit < 0 || unit >= MAX_UNITS)
        return (ERROR);

    /* ensure single invocation per system life */

    pDrvCtrl = & drvCtrl [unit];
    if (pDrvCtrl->attached)
        return (OK);

    /* publish the interface data record */
#ifdef BSD43_DRIVER
    ether_attach (& pDrvCtrl->idr.ac_if, unit, "ln", (FUNCPTR) NULL,
                  (FUNCPTR) lnIoctl, (FUNCPTR) lnOutput, (FUNCPTR) lnReset);
#else

    /* Publish the interface record */
    ether_attach    (
                    (IFNET *) & pDrvCtrl->idr,
                    unit,
                    "ln",
                    (FUNCPTR) NULL,
                    (FUNCPTR) lnIoctl,
                    (FUNCPTR) ether_output, /* generic ether_output */
                    (FUNCPTR) lnReset
                    );

    pDrvCtrl->idr.ac_if.if_start = (FUNCPTR)lnStartOutput;

#endif

    /* create the transmit semaphore. */

    if ((pDrvCtrl->TxSem = semMCreate (SEM_Q_PRIORITY | SEM_DELETE_SAFE |
	SEM_INVERSION_SAFE)) == NULL)
        {
        printf ("ln: error creating transmitter semaphore\n");
        return (ERROR);
        }

    /* establish size of shared memory region we require */

    if ((int) memAdrs != NONE)  /* specified memory pool */
        {
        /* we want to maximize lnRsize and lnTsize */

        sz = (memSize - (RMD_SIZ + TMD_SIZ + (lnLPool * LN_BUFSIZ) +
	     IB_SIZ)) / ((2 * LN_BUFSIZ) + RMD_SIZ + TMD_SIZ);

        sz >>= 1;		/* adjust for roundoff */

        for (lnRsize = 0; sz != 0; lnRsize++, sz >>= 1)
            ;

        lnTsize = lnRsize;	/* lnTsize = lnRsize for convenience */
        }

    /* limit ring sizes to reasonable values */

    lnRsize = max (lnRsize, 2);		/* 4 Rx buffers is reasonable min */
    lnRsize = min (lnRsize, 7);		/* 128 Rx buffers is max for chip */
    lnTsize = max (lnTsize, 2);		/* 4 Tx buffers is reasonable min */
    lnTsize = min (lnTsize, 7);		/* 128 Tx buffers is max for chip */
    lnLPool = min (lnLPool, LN_L_POOL);	/* L_POOL is max for Rx loaner pool */

    /* add it all up - allow extra bytes for alignment adjustments */

    sz = (((1 << lnRsize) + 1) * RMD_SIZ ) + (LN_BUFSIZ << lnRsize) +
	 (((1 << lnTsize) + 1) * TMD_SIZ ) + (LN_BUFSIZ << lnTsize) +
	 (LN_BUFSIZ * lnLPool) + IB_SIZ + 24;

    /* establish a region of shared memory */

    /*
     * OK. We now know how much shared memory we need.  If the caller
     * provides a specific memory region, we check to see if the provided
     * region is large enough for our needs.  If the caller did not
     * provide a specific region, then we attempt to allocate the memory
     * from the system, using the cache aware allocation system call.
     */

    switch ( (int) memAdrs )
        {
        default :			/* caller provided memory */
            if ( memSize < sz )		/* not enough space */
                {
                printf ( "ln: not enough memory provided\n" );
                return ( ERROR );
                }

            pShMem = memAdrs;		/* set the beginning of pool */

            /* assume pool is cache coherent, copy null structure */

            pDrvCtrl->cacheFuncs = cacheNullFuncs;
            break;

        case NONE :     /* get our own memory */

            /*
	     * Because the structures that are shared between the device
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
                printf ( "ln: device requires cache coherent memory\n" );
                return (ERROR);
                }

            pShMem = (char *) cacheDmaMalloc (sz);
            if ((int) pShMem == NULL)
                {
                printf ( "ln: system memory unavailable\n" );
                return (ERROR);
                }

            /* copy the DMA structure */

            pDrvCtrl->cacheFuncs = cacheDmaFuncs;
            break;
        }

    /* Align pool on 4 byte boundary - uses up to 8 bytes of excess memory. */

    pShMem = (char *) ( ( (int)pShMem + 3) & ~3);

    /* save some things */

    pDrvCtrl->flags    = 0;			/* initialize the LANCE flags */
    pDrvCtrl->memWidth = memWidth;
    pDrvCtrl->ivec     = ivec;                  /* interrupt vector */
    pDrvCtrl->ilevel   = ilevel;                /* interrupt level */
    pDrvCtrl->devAdrs  = (LN_DEVICE *) devAdrs; /* LANCE I/O address */
    pDrvCtrl->pMemPool = pShMem;                /* start of memory pool */

    /* obtain our Ethernet address and save it */

    bcopy ((char *) lnEnetAddr, (char *) pDrvCtrl->idr.ac_enaddr, 6);

    /*
     *                        MEMORY MAPPING
     *
     *                          low memory
     *
     *             |-------------------------------------|
     *             |       The initialization block      |
     *             |              (IB_SIZ)               |
     *             |-------------------------------------|
     *             |         The Rx descriptors          |
     *             |   ((1 << lnRsize) + 1) * RMD_SIZ    |
     *             |-------------------------------------|
     *             |          The receive buffers        |
     *             |       (LN_BUFSIZ << lnRsize)        |
     *             |-------------------------------------|
     *             |         The Rx loaner pool          |
     *             |        (LN_BUFSIZ * lnLPool)        |
     *             |-------------------------------------|
     *             |         The Tx descriptors          |
     *             |    ((1 << lnTsize) + 1)* TMD_SIZ    |
     *             |-------------------------------------|
     *             |        The transmit buffers         |
     *             |       (LN_BUFSIZ << lnTsize)        |
     *             |-------------------------------------|
     */

    /* first let's clear memory */

    bzero ((char *) pShMem, (int) sz); /* XXX deal with memWidth */

    /* carve Rx memory structure */

    pDrvCtrl->rring     = (ln_rmd *) ((int)pShMem + IB_SIZ);
    pDrvCtrl->rringLen  = lnRsize;
    pDrvCtrl->rringSize = 1 << lnRsize;
    pDrvCtrl->rmdIndex	= 0;

    /* setup Rx buffer descriptors */

    rmd = pDrvCtrl->rring;

    /* 
     * Set the Rx buffer misaligned so that the ether header will align the
     * IP header on a 4 byte boundary. 
     */

    buf = ( (char *) (rmd + (pDrvCtrl->rringSize +1))) + 2;

    /* Skip over the receive buffer pool to the Rx loaner pool. */

    buf += LN_BUFSIZ << lnRsize;

    /* setup Rx loaner pool */

    pDrvCtrl->nLoanRx = lnLPool;
    for (ix = 0; ix < lnLPool; ix++)
	{
        pDrvCtrl->lPool[ix] = buf + (ix * LN_BUFSIZ);
        pDrvCtrl->refCnt[ix] = 0;
        pDrvCtrl->pRefCnt[ix] = & pDrvCtrl->refCnt[ix];
	}

    /* carve Tx memory structure */

    /* Note: +2 offset aligns IP header on 4 byte boundary. */
     
    pDrvCtrl->tring     = (ln_tmd *)(buf + (ix * LN_BUFSIZ) + 2);
    pDrvCtrl->tringLen  = lnTsize;
    pDrvCtrl->tringSize = 1 << lnTsize;
    pDrvCtrl->tmdIndex	= 0;
    pDrvCtrl->tmdIndexC	= 0;

    lnChipReset (pDrvCtrl);                           /* reset LANCE */
 
    if (intConnect ((VOIDFUNCPTR *) INUM_TO_IVEC (ivec), lnInt, (int) pDrvCtrl)
        == ERROR)
        return (ERROR);

    lnRestartSetup (pDrvCtrl);

    pDrvCtrl->attached = TRUE;			/* Set our flag */

    return (OK);
    }

/*******************************************************************************
*
* lnReset - reset the interface
*
* Mark interface as inactive & reset the chip
*/

LOCAL void lnReset
    (
    int unit
    )
    {
    DRV_CTRL *pDrvCtrl = & drvCtrl [unit];

    pDrvCtrl->idr.ac_if.if_flags = 0;
    lnChipReset (pDrvCtrl);		/* reset LANCE */
    }

/*******************************************************************************
*
* lnInt - handle controller interrupt
*
* This routine is called at interrupt level in response to an interrupt from
* the controller.
*/

LOCAL void lnInt
    (
    DRV_CTRL  *pDrvCtrl
    )
    {
    ln_tmd	*tmd;
    u_short	stat;
    int 	i;

    stat = lnCsr0Read (pDrvCtrl);	/* Read the device status register */

    if (!(stat & lncsr_INTR))		/* If false interrupt, return. */
        return;

    /*
     * Enable interrupts, clear receive and/or transmit interrupts, and clear
     * any errors that may be set.
     */

    lnCsr0Write (pDrvCtrl, ((stat & (lncsr_BABL | lncsr_CERR | lncsr_MISS |
	lncsr_MERR | lncsr_RINT | lncsr_TINT | lncsr_IDON)) | lncsr_INEA));

    /* Check for errors */

    if (stat & (lncsr_BABL | lncsr_MISS | lncsr_MERR))
        {
        ++pDrvCtrl->idr.ac_if.if_ierrors;
        if (stat & lncsr_MERR)		/* restart chip on memory error */
            {
            logMsg ("lnInt: memory error detected\n",0, 0, 0, 0, 0, 0);
            pDrvCtrl->idr.ac_if.if_flags &= ~(IFF_UP | IFF_RUNNING);
            (void) netJobAdd ((FUNCPTR) lnRestart, pDrvCtrl->idr.ac_if.if_unit,
                              0,0,0,0);
            }
        }

    /* schedule netTask to handle any Rx packets */

    if ((stat & lncsr_RINT) && (stat & lncsr_RXON) &&
	!(pDrvCtrl->flags & LN_RX_HANDLING_FLAG))
	{
        pDrvCtrl->flags |= LN_RX_HANDLING_FLAG;
        (void) netJobAdd ((FUNCPTR)lnHandleRecvInt, (int)pDrvCtrl, 0,0,0,0);
        }
 
    /*
     * Did LANCE update any of the TMD's?
     * If not then don't bother continuing with transmitter stuff
     */  

    if (!(stat & lncsr_TINT))
        {
        /* Flush the write pipe */

        CACHE_PIPE_FLUSH ();

        /* allow interrupt line to go inactive */

        i = 3;
        while (i-- && (lnCsr0Read (pDrvCtrl) & lncsr_INTR))
           ;
        return;
        }

    while (pDrvCtrl->tmdIndexC != pDrvCtrl->tmdIndex)
        {
        /* disposal has not caught up */
 
        tmd = pDrvCtrl->tring + pDrvCtrl->tmdIndexC;
 
        /* if the buffer is still owned by LANCE, don't touch it */
 
        LN_CACHE_INVALIDATE (tmd, TMD_SIZ);

        if (tmd->tbuf_stat & TMD_OWN)
            break;

        /*
         * lntmd1_ERR is an "OR" of LCOL, LCAR, UFLO or RTRY.
         * Note that BUFF is not indicated in lntmd1_ERR.
         * We should therefore check both lntmd1_ERR and lntmd3_BUFF
         * here for error conditions.
         */

        if ((tmd->tbuf_stat & lntmd1_ERR) || (tmd->tbuf_err & lntmd3_BUFF))
            {
            pDrvCtrl->idr.ac_if.if_oerrors++;     /* output error */
            pDrvCtrl->idr.ac_if.if_opackets--;

            /* 
             * If error was due to excess collisions, bump the collision
             * counter.  The LANCE does not keep an individual counter of
             * collisions, so in this driver, the collision statistic is not
             * an accurate count of total collisions.
             */

            if (tmd->tbuf_err & lntmd3_RTRY)	/* assume DRTY bit not set */
                pDrvCtrl->idr.ac_if.if_collisions += 16;

            /* check for no carrier */

            if (tmd->tbuf_err & TMD_LCAR)
                logMsg ("ln%d: no carrier\n", pDrvCtrl->idr.ac_if.if_unit,
                         0, 0, 0, 0, 0);

            /* Restart chip on fatal errors.
             * The following code handles the situation where the transmitter
             * shuts down due to an underflow error.  This is a situation that
             * will occur if the DMA cannot keep up with the transmitter.
             * It will occur if the LANCE is being held off from DMA access
             * for too long or due to significant memory latency.  DRAM
             * refresh or slow memory could influence this.  Many
             * implementations use a dedicated LANCE buffer.  This can be
             * static RAM to eliminate refresh conflicts; or dual-port RAM
             * so that the LANCE can have free run of this memory during its
             * DMA transfers.
             */
    
            if ((tmd->tbuf_err & lntmd3_BUFF) || (tmd->tbuf_err & TMD_UFLO))
                {
                pDrvCtrl->idr.ac_if.if_flags &= ~(IFF_UP | IFF_RUNNING);
                logMsg ("lnInt: transmitter underflow\n", 0, 0, 0, 0, 0, 0);
                (void) netJobAdd ((FUNCPTR) lnRestart,
	        		   pDrvCtrl->idr.ac_if.if_unit, 0,0,0,0);

                /* Flush the write pipe */

                CACHE_PIPE_FLUSH ();

                /* allow interrupt line to go inactive */

                i = 3;
                while (i-- && (lnCsr0Read (pDrvCtrl) & lncsr_INTR))
                    ;

                return;

                }
            }

        tmd->tbuf_stat &= 0x00ff; 	/* reset status bits */
        tmd->tbuf_err = 0; 		/* clear error bits */

       /* now bump the tmd disposal index pointer around the ring */

        pDrvCtrl->tmdIndexC = (pDrvCtrl->tmdIndexC + 1) &
                              (pDrvCtrl->tringSize - 1);

        }

    CACHE_PIPE_FLUSH ();		/* flush the write pipe */

    /* allow interrupt line to go inactive */

    i = 3;
    while (i-- && (lnCsr0Read (pDrvCtrl) & lncsr_INTR))
        ;
    }

/*******************************************************************************
*
* lnHandleRecvInt - task level interrupt service for input packets
*
* This routine is called at task level indirectly by the interrupt
* service routine to do any message received processing.
*/

LOCAL void lnHandleRecvInt
    (
    DRV_CTRL *pDrvCtrl
    )
    {
    do {
        pDrvCtrl->flags |= LN_RX_HANDLING_FLAG;

        while (lnGetFullRMD (pDrvCtrl))
            lnRecv (pDrvCtrl);

        /*
         * There is a RACE right here.  The ISR could add a receive packet
         * and check the boolean below, and decide to exit.  Thus the
         * packet could be dropped if we don't double check before we
         * return.
         */

        pDrvCtrl->flags &= ~LN_RX_HANDLING_FLAG;
        }
    while (lnGetFullRMD (pDrvCtrl)); /* double check for RACE */
    }

/*******************************************************************************
*
* lnGetFullRMD - check next RMD for ownership
*
*/

LOCAL BOOL lnGetFullRMD
    (
    DRV_CTRL *pDrvCtrl
    )
    {
    int status = FALSE;
    ln_rmd *rmd;

    if  ((pDrvCtrl->idr.ac_if.if_flags & (IFF_UP | IFF_RUNNING)) ==
        (IFF_UP | IFF_RUNNING))
	{
        rmd = pDrvCtrl->rring + pDrvCtrl->rmdIndex;

        LN_CACHE_INVALIDATE (rmd, RMD_SIZ);

        if ((rmd->rbuf_stat & lnrmd1_OWN) == 0)
            status = TRUE;
	}

    return (status);
    }

/*******************************************************************************
*
* lnRecv - process the next incoming packet
*
*/

LOCAL STATUS lnRecv
    (
    DRV_CTRL *pDrvCtrl
    )
    {
    ETH_HDR	*pEnetHdr;
    MBUF	*pMbuf = NULL;
    ln_rmd	*rmd = pDrvCtrl->rring + pDrvCtrl->rmdIndex;
    u_char	*pData;
    u_long	pPhys;
    int		len;
    u_short	type;

    /* packet must be checked for errors or possible data chaining */

    if ((rmd->rbuf_stat & lnrmd1_ERR) || ((rmd->rbuf_stat &
	(lnrmd1_STP | lnrmd1_ENP)) != (lnrmd1_STP | lnrmd1_ENP)))
        {
        ++pDrvCtrl->idr.ac_if.if_ierrors;	/* bump error stat */
        goto cleanRXD;				/* skip to clean up */
        }

    len = rmd->rbuf_mcnt;			/* get packet length */
    len = len - 4; 				/* omit frame check sequence */

    /* get pointer to the receive buffer */

    pPhys = ((ULONG)pDrvCtrl->pMemPool & 0xff000000) |
            ((rmd->rbuf_ladr | (rmd->rbuf_hadr << 16)) & 0x00ffffff);
    pEnetHdr = (ETH_HDR *) LN_CACHE_PHYS_TO_VIRT (pPhys);

    LN_CACHE_INVALIDATE (pEnetHdr, len);	/* make the pkt data coherent */

    /* call input hook if any */

    if ((etherInputHookRtn == NULL) || ((*etherInputHookRtn)
       (& pDrvCtrl->idr.ac_if, (char *) pEnetHdr, len)) == 0)
        {
        len -= SIZEOF_ETHERHEADER;	/* adjust length to size of data only */
        type = ntohs (pEnetHdr->ether_type);	/* save type field */
        pData = ((u_char *) pEnetHdr) + SIZEOF_ETHERHEADER;

        /* OK to loan out buffer ? -> build an mbuf cluster */

#ifdef BSD43_DRIVER    
	if ((pDrvCtrl->nLoanRx > 0) && (USE_CLUSTER (len)))
	    pMbuf = build_cluster (pData, len, & pDrvCtrl->idr, MC_LANCE,
	        pDrvCtrl->pRefCnt[(pDrvCtrl->nLoanRx - 1)], lnLoanFree,
		(int) pDrvCtrl, (int) pEnetHdr,
		(int) pDrvCtrl->pRefCnt[(pDrvCtrl->nLoanRx - 1)]);

        /* if buffer was successfully turned into mbuf cluster */

        if (pMbuf != NULL)
	    {
            pPhys = (u_long) LN_CACHE_VIRT_TO_PHYS
		    (pDrvCtrl->lPool[--pDrvCtrl->nLoanRx]);
            rmd->rbuf_ladr = pPhys;
            rmd->rbuf_hadr = (pPhys >> 16) & lnrmd1_HADR;
	    }
        else
            {
            if ((pMbuf = bcopy_to_mbufs (pData, len, 0, & pDrvCtrl->idr.ac_if,
                pDrvCtrl->memWidth)) == NULL)
		{
                ++pDrvCtrl->idr.ac_if.if_ierrors;	/* bump error stat */
                goto cleanRXD;
		}
            }
        /* send on up... */

        do_protocol_with_type (type, pMbuf, & pDrvCtrl->idr, len);

#else 	/* BSD43_DRIVER */
	if ((pDrvCtrl->nLoanRx > 0) && (USE_CLUSTER (len)) &&
	    ((pMbuf = build_cluster (pData, len, &pDrvCtrl->idr.ac_if, MC_LANCE,
                                    pDrvCtrl->pRefCnt[(pDrvCtrl->nLoanRx - 1)],
                                    (FUNCPTR)lnLoanFree, (int)pDrvCtrl, 
                                    (int)pEnetHdr, 
                    (int)pDrvCtrl->pRefCnt[(pDrvCtrl->nLoanRx - 1)])) != NULL))
	    {
            pPhys = (u_long) LN_CACHE_VIRT_TO_PHYS
		    (pDrvCtrl->lPool[--pDrvCtrl->nLoanRx]);
            rmd->rbuf_ladr = pPhys;
            rmd->rbuf_hadr = (pPhys >> 16) & lnrmd1_HADR;
	    }
	else 
	    pMbuf = bcopy_to_mbufs (pData, len, 0, &pDrvCtrl->idr.ac_if, 
                                    pDrvCtrl->memWidth);

	if (pMbuf != NULL)
	    do_protocol (pEnetHdr, pMbuf, &pDrvCtrl->idr, len);
#endif	/* BSD43_DRIVER */ 
        ++pDrvCtrl->idr.ac_if.if_ipackets;	/* bump statistic */
        }

    /* Done with descriptor, clean up and give it to the device. */

cleanRXD:

    rmd->rbuf_mcnt = 0;			/* reset count */
    rmd->rbuf_stat &= 0xff; 		/* reset status bits */
    rmd->rbuf_stat |= lnrmd1_OWN;	/* give device ownership */

    CACHE_PIPE_FLUSH ();		/* flush the write pipe */

    /* advance our management index */

    pDrvCtrl->rmdIndex = (pDrvCtrl->rmdIndex + 1) & (pDrvCtrl->rringSize - 1);

    return (OK);
    }

#ifdef BSD43_DRIVER
/*******************************************************************************
*
* lnOutput - the driver output routine
*
*/

LOCAL int lnOutput
    (
    IDR  *pIDR,
    MBUF *pMbuf,
    SOCK *pDest
    )
    {
    DRV_CTRL	*pDrvCtrl;
    char	destEnetAddr [6];	/* space for enet addr */
    u_short	packetType;		/* type field for the packet */
    ln_tmd	*tmd;
    char	*buf;
    int		len;

    /* check ifnet flags - return error if incorrect. */

    if ((pIDR->ac_if.if_flags & (IFF_UP | IFF_RUNNING)) !=
	(IFF_UP | IFF_RUNNING))
        return (ENETDOWN);

    /*
     * Attempt to convert socket addr into enet addr and packet type.
     * Note that if ARP resolution of the address is required, the ARP
     * module will call our routine again to transmit the ARP request
     * packet.  This means we may actually call ourselves recursively!
     */

    if (convertDestAddr (pIDR,pDest, destEnetAddr, &packetType, pMbuf) == FALSE)
        return (OK);    /* I KNOW returning OK is stupid, but it is correct */

    /* get driver control pointer */

    pDrvCtrl = & drvCtrl [pIDR->ac_if.if_unit];

    /*
     * Obtain exclusive access to transmitter.  This is necessary because
     * certain events can cause netTask to run a job that attempts to transmit
     * a packet.  We can only allow one task here at a time.
     */

    semTake (pDrvCtrl->TxSem, WAIT_FOREVER);

    tmd = pDrvCtrl->tring + pDrvCtrl->tmdIndex;
    LN_CACHE_INVALIDATE (tmd, TMD_SIZ);

    if ((tmd->tbuf_stat & lntmd1_OWN) || (((pDrvCtrl->tmdIndex + 1) &
        (pDrvCtrl->tringSize - 1)) == pDrvCtrl->tmdIndexC))
        {
        m_freem (pMbuf);			/* discard data */
        pDrvCtrl->idr.ac_if.if_oerrors++;	/* output error */
        goto outputDone;
        }

    buf = pDrvCtrl->tBufBase + (pDrvCtrl->tmdIndex * LN_BUFSIZ);
    bcopy_from_mbufs ((buf + SIZEOF_ETHERHEADER), pMbuf, len,
		      pDrvCtrl->memWidth);

    bcopy (destEnetAddr, buf, 6);
    bcopy ((char *) pIDR->ac_enaddr, (buf + 6), 6);
    ((ETH_HDR *) buf)->ether_type = packetType;

    len += SIZEOF_ETHERHEADER;
    len = max (ETHERSMALL, len);

    if ((etherOutputHookRtn != NULL) &&
        (* etherOutputHookRtn) (&pDrvCtrl->idr.ac_if,buf,len))
        {
        CACHE_PIPE_FLUSH ();    /* done as a safety */
        goto outputDone;        /* goto the end */
        }

    tmd->tbuf_bcnt = -len;
    tmd->tbuf_err = 0;

    /* advance our management index */

    pDrvCtrl->tmdIndex = (pDrvCtrl->tmdIndex + 1) & (pDrvCtrl->tringSize - 1);

    tmd->tbuf_stat |= lntmd1_OWN | lntmd1_STP | lntmd1_ENP;
    tmd->tbuf_stat &= ~lntmd1_DEF & ~lntmd1_MORE & ~lntmd1_ERR;

    CACHE_PIPE_FLUSH ();		/* flush the write pipe */

    /* kick start the transmitter, if selected */

    if (lnKickStartTx)
        lnCsr0Write (pDrvCtrl, (lncsr_INEA | lncsr_TDMD));

    pIDR->ac_if.if_opackets++;		/* bump the statistic counter. */

outputDone:

    semGive (pDrvCtrl->TxSem);		/* release exclusive access. */
    return (OK);
    }
#else	/* BSD43_DRIVER */
/*******************************************************************************
*
* lnStartOutput - the driver output routine
*
*/

LOCAL void lnStartOutput
    (
    DRV_CTRL *	pDrvCtrl	/* pointer to driver control structure */
    )
    {
    struct mbuf * 	pMbuf = NULL;
    ln_tmd *		tmd;
    char *		buf;
    int			len;

    while (pDrvCtrl->idr.ac_if.if_snd.ifq_head)
        {

	/* dequeue a packet from the transmit queue */
        IF_DEQUEUE (&pDrvCtrl->idr.ac_if.if_snd, pMbuf);  

	tmd = pDrvCtrl->tring + pDrvCtrl->tmdIndex;
	LN_CACHE_INVALIDATE (tmd, TMD_SIZ);

	if ((tmd->tbuf_stat & lntmd1_OWN) || (((pDrvCtrl->tmdIndex + 1) &
					       (pDrvCtrl->tringSize - 1)) == 
					      pDrvCtrl->tmdIndexC))
	    {
	    pDrvCtrl->idr.ac_if.if_oerrors++;	/* output error */
	    if (pMbuf != NULL)
		m_freem (pMbuf); 	/* drop the packet */
	    break;
	    }

	buf = pDrvCtrl->tBufBase + (pDrvCtrl->tmdIndex * LN_BUFSIZ);

	bcopy_from_mbufs (buf, pMbuf, len, pDrvCtrl->memWidth);

        len = max (ETHERSMALL, len);

        if ((etherOutputHookRtn != NULL) &&
            (* etherOutputHookRtn) (&pDrvCtrl->idr.ac_if,buf,len))
            {
            CACHE_PIPE_FLUSH ();    /* done as a safety */
            break; 
            }

	tmd->tbuf_bcnt = -len;
	tmd->tbuf_err = 0;

	/* advance our management index */

	pDrvCtrl->tmdIndex = ((pDrvCtrl->tmdIndex + 1) & 
			      (pDrvCtrl->tringSize - 1));

	tmd->tbuf_stat |= lntmd1_OWN | lntmd1_STP | lntmd1_ENP;
        tmd->tbuf_stat &= ~lntmd1_DEF & ~lntmd1_MORE & ~lntmd1_ERR;

	CACHE_PIPE_FLUSH ();		/* flush the write pipe */

	/* kick start the transmitter, if selected */
	if (lnKickStartTx)
	    lnCsr0Write (pDrvCtrl, (lncsr_INEA | lncsr_TDMD));

        pDrvCtrl->idr.ac_if.if_opackets++; /* bump the statistic counter. */
        }
    }
#endif 	/* BSD43_DRIVER */

/*******************************************************************************
*
* lnIoctl - the driver I/O control routine
*
* Process an ioctl request.
*/

LOCAL int lnIoctl
    (
    IDR		*ifp,
    int		cmd,
    caddr_t	data
    )
    {
    int error = 0;

    switch (cmd)
        {
        case SIOCSIFADDR:
           ((struct arpcom *)ifp)->ac_ipaddr = IA_SIN (data)->sin_addr;
            arpwhohas (ifp, &IA_SIN (data)->sin_addr);
            break;

        case SIOCSIFFLAGS:
            /* No further work to be done */
            break;

        default:
            error = EINVAL;
        }

    return (error);
    }

/*******************************************************************************
*
* lnChipReset - hardware reset of chip (stop it)
*/

LOCAL void lnChipReset
    (
    DRV_CTRL  *pDrvCtrl
    )
    {
    u_short *dv = (u_short *)pDrvCtrl->devAdrs;
    u_int RAPIndx;
    u_int CSRIndx;


    /*@RICK The following loops attempt to magically determine the exact
     * location of the two LANCE registers.  Not only is this algorithm
     * poorly documented, it is poor practice to poke at unknown memory
     * locations.  This will be revised.
     */

    /* find control registers and stop LANCE */

    for (CSRIndx=CSROffset; CSRIndx < 3; CSRIndx++)
        {
        for (RAPIndx=CSRIndx+1; RAPIndx <= 4; RAPIndx++)
            {
            /* setup CSR0 */
            *(dv+RAPIndx) = (u_short)0;

            if (!(u_short)*(dv+RAPIndx))
                {
                /* write stop */
                *(dv+CSRIndx) = (u_short)lncsr_STOP;
                /* might want to delay here */
                }
            /* check stop condition */
            if ((!(u_short)*(dv+RAPIndx)) &&
                ((u_short)*(dv+CSRIndx) == (u_short)lncsr_STOP)) break;
            }
        /* check stop condition */
        if ((!(u_short)*(dv+RAPIndx)) &&
            ((u_short)*(dv+CSRIndx) == (u_short)lncsr_STOP)) break;
        }

    CSROffset = CSRIndx;
    RAPOffset = RAPIndx;
    }

/*******************************************************************************
*
* lnCsr0Read - read CSR 0 register
*
* Assumes normal operation, where the RAP register has been left selecting
* CSR0.
*/

LOCAL u_short lnCsr0Read
    (
    DRV_CTRL * pDrvCtrl			/* driver control */
    )
    {
    u_short *dv = (u_short *) pDrvCtrl->devAdrs;

    return (*(dv + CSROffset));		/* get contents of CSR */
    }

/*******************************************************************************
*
* lnCsr0Write - write CSR 0 register
*
* Assumes normal operation, where the RAP register has been left selecting
* CSR0.
*/

LOCAL void lnCsr0Write
    (
    DRV_CTRL * pDrvCtrl,		/* driver control */
    u_short value			/* value to write */
    )
    {
    u_short *dv = (u_short *) pDrvCtrl->devAdrs;

    *(dv + CSROffset) = (u_short) value;	/* write value to CSR */
    }

/*******************************************************************************
*
* lnCsrWrite - select and write a CSR register
*
*/

LOCAL void lnCsrWrite
    (
    DRV_CTRL * pDrvCtrl,		/* driver control */
    int reg,				/* register to select */
    u_short value			/* value to write */
    )
    {
    u_short *dv = (u_short *) pDrvCtrl->devAdrs;

    *(dv + RAPOffset) = (u_short) reg;		/* select CSR */
    *(dv + CSROffset) = (u_short) value;	/* write value to CSR */
    }

/*******************************************************************************
*
* lnRestartSetup - setup memory descriptors and turn on chip
*
* Initializes all the shared memory structures and turns on the chip.
*/

LOCAL void lnRestartSetup
    (
    DRV_CTRL * 	pDrvCtrl
    )
    {
    char * 	buf;
    void * 	pTemp;
    ln_rmd * 	rmd;
    ln_tmd * 	tmd;
    ln_ib * 	ib;
    int 	ix;
    int 	rsize;
    int 	tsize;
    u_short 	stat;
    int 	timeoutCount = 0;

    /* setup Rx buffer descriptors */
    
    rsize = pDrvCtrl->rringLen;
    rmd = pDrvCtrl->rring;
    buf = ( (char *) (rmd + (pDrvCtrl->rringSize + 1))) + 2;
    rmd = (ln_rmd *) ( ( (int)rmd + 7) & ~7);   /* align on 000 boundary */
    pDrvCtrl->rring = rmd;

    pDrvCtrl->rBufBase = buf;
    for (ix = 0; ix < pDrvCtrl->rringSize; ix++, rmd++, buf += LN_BUFSIZ)
        {
        pTemp = LN_CACHE_VIRT_TO_PHYS (buf);    /* convert to physical addr */
        rmd->rbuf_ladr = (u_long) pTemp;        /* bits 15:00 of buffer addr */
        rmd->rbuf_hadr = (((u_long) pTemp >> 16) & 0xff) | lnrmd1_OWN;
                                                /* bits 23:16 of buffer addr */
        rmd->rbuf_bcnt = -(LN_BUFSIZ);          /* neg of buffer byte count */
        rmd->rbuf_mcnt = 0;                     /* no message byte count yet */
        }

    /* setup Tx buffer descriptors */

    tsize = pDrvCtrl->tringLen;
    tmd = pDrvCtrl->tring;
    buf = ( (char *) (tmd + (pDrvCtrl->tringSize + 1))) + 2;
    tmd = (ln_tmd *) ( ( (int)tmd + 7) & ~7);   /* align on 000 boundary */
    pDrvCtrl->tring = tmd;
 
    pDrvCtrl->tBufBase = buf;
    for (ix = 0; ix < pDrvCtrl->tringSize; ix++, tmd++, buf += LN_BUFSIZ)
        {
        pTemp = LN_CACHE_VIRT_TO_PHYS (buf);    /* convert to phys addr */
        tmd->tbuf_ladr = (u_long)pTemp;        /* bits 15:00 of buf addr */
        tmd->tbuf_hadr = (((u_long)pTemp >> 16) & 0xff) | lntmd1_ENP; 
                                               /* bits 23:16 of buf addr */
        tmd->tbuf_bcnt = 0;                     /* no message byte count yet */
        tmd->tbuf_err = 0;                      /* no error status yet */
        }

    /* setup the initialization block */

    ib = (ln_ib *)pDrvCtrl->pMemPool;
    ib->lnIBMode = 0;           /* chip will be in normal receive mode */

    swab ((char *) pDrvCtrl->idr.ac_enaddr, ib->lnIBPadr, 6);

    pTemp = LN_CACHE_VIRT_TO_PHYS (pDrvCtrl->rring);    /* point to Rx ring */
    ib->lnIBRdraLow = (u_long) pTemp;
    ib->lnIBRdraHigh = (((u_long) pTemp >> 16) & 0xff) | (rsize << 13);

    pTemp = LN_CACHE_VIRT_TO_PHYS (pDrvCtrl->tring);    /* point to Tx ring */
    ib->lnIBTdraLow = (u_long) pTemp;
    ib->lnIBTdraHigh = (((u_long) pTemp >> 16) & 0xff) | (tsize << 13);

    CACHE_PIPE_FLUSH ();                        /* Flush the write pipe */

    lnCsrWrite (pDrvCtrl, 0, lncsr_STOP);       /* set the stop bit */

    /* set the bus modes */

#if (CPU_FAMILY == SPARC)
    lnCsrWrite (pDrvCtrl, 3, lncsr3_BSWP | lncsr3_ACON | lncsr3_BCON);
#else
    /* lnCSR_3B is either set above, or overridden externally */
    lnCsrWrite (pDrvCtrl, 3, lnCSR_3B);
#endif

    /* point the device to the initialization block */

    pTemp = LN_CACHE_VIRT_TO_PHYS (ib);
    lnCsrWrite (pDrvCtrl, 2, (u_short)(((u_long)pTemp >> 16) & 0xff));
    lnCsrWrite (pDrvCtrl, 1, (u_long) pTemp);
    lnCsrWrite (pDrvCtrl, 0, lncsr_INIT);       /* init chip (read IB) */

    /* hang until Initialization DONe, ERRor, or timeout */

    while (((stat = lnCsr0Read (pDrvCtrl)) & (lncsr_IDON | lncsr_ERR)) == 0)
        {
        if (timeoutCount++ > 0x10000) 
            break;
        taskDelay (100);
        }

    /* log chip initialization failure */

    if ((stat & lncsr_ERR) || (timeoutCount >= 0x10000) )
        {
        logMsg ("ln%d: Device initialization failed\n",
                    pDrvCtrl->idr.ac_if.if_unit, 0, 0, 0, 0, 0);
        return;
        }

    /*
     * Device is initialized.  Start transmitter and receiver.  The device
     * RAP register is left selecting CSR 0.
     */
    lnCsrWrite (pDrvCtrl, 0, lncsr_IDON | lncsr_INEA | lncsr_STRT);

    sysLanIntEnable (pDrvCtrl->ilevel);         /* enable LANCE interrupts */
    pDrvCtrl->idr.ac_if.if_flags |= (IFF_UP | IFF_RUNNING | IFF_NOTRAILERS);
    }

/*******************************************************************************
*
* lnRestart - restart the device after a fatal error
*
* This routine takes care of all the messy details of a restart.  The device
* is reset and re-initialized.  The driver state is re-synchronized.
*/

LOCAL void lnRestart
    (
    int unit
    )
    {
    DRV_CTRL * pDrvCtrl = &drvCtrl [unit];

    printf("ln: RESTART called\n");

    lnChipReset (pDrvCtrl);

    lnRestartSetup (pDrvCtrl);

    /* Set our flag. */

    pDrvCtrl->attached = TRUE;

    return;
    }

#ifdef BSD43_DRIVER
/*******************************************************************************
*
* convertDestAddr - converts socket addr into enet addr and packet type
*
*/

LOCAL BOOL convertDestAddr
    (
    IDR *pIDR,                          /* ptr to i/f data record */
    SOCK *pDestSktAddr,                 /* ptr to a generic sock addr */
    char *pDestEnetAddr,                /* where to write enet addr */
    u_short *pPacketType,               /* where to write packet type */
    MBUF *pMbuf                         /* ptr to mbuf data */
    )
    {

    /***** Internet family *****/

    {
    struct in_addr destIPAddr;              /* not used */
    int trailers;                           /* not supported */

     if (pDestSktAddr->sa_family == AF_INET)
        {
        *pPacketType = ETHERTYPE_IP;            /* stuff packet type */
        destIPAddr = ((struct sockaddr_in *) pDestSktAddr)->sin_addr;

        if (!arpresolve (pIDR, pMbuf, &destIPAddr, pDestEnetAddr, &trailers))
            return (FALSE);     /* if not yet resolved */

        return (TRUE);
        }
    }

    /***** Generic family *****/

    {
    ETH_HDR *pEnetHdr;

    if (pDestSktAddr->sa_family == AF_UNSPEC)
        {
        pEnetHdr = (ETH_HDR *) pDestSktAddr->sa_data;      /* ptr to hdr */
        bcopy ((char *) pEnetHdr->ether_dhost, pDestEnetAddr, 6);
        *pPacketType = pEnetHdr->ether_type;               /* copy type */
        return (TRUE);
        }
    }

    /* Unsupported family */

    return (FALSE);

    } /* End of convertDestAddr() */
#endif /* BSD43_DRIVER */

/*******************************************************************************
*
* lnLoanFree - return the given buffer to loaner pool
*
* This routine returns <pRxBuf> to the pool of available loaner buffers.
* It also returns <pRef> to the pool of available loaner reference counters,
* then zeroes the reference count.
*
* RETURNS: N/A
*/
 
LOCAL void lnLoanFree
    (
    DRV_CTRL	*pDrvCtrl,
    char	*pRxBuf,
    UINT8	*pRef
    )
    {
    /* return loaned buffer to pool */
 
    pDrvCtrl->lPool[pDrvCtrl->nLoanRx] = pRxBuf;
 
    /* return loaned reference count to pool */
 
    pDrvCtrl->pRefCnt[pDrvCtrl->nLoanRx++] = pRef;
 
    /* reset reference count - should have been done from above, but... */
 
    *pRef = 0;
    }

