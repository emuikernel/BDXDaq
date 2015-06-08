/* if_lnPci.c - AMD Am79C970 PCnet-PCI Ethernet network interface driver */

/* Copyright 1984-2001 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01j,19oct01,dat  Documentation formatting
01i,17oct00,spm  merged from version 01j of tor2_0_x branch (base version 01h):
                 removed obsolete ether_attach prototype
01h,26aug98,sut  Renamed lnCSR_3B to lnPciCSR_3B DOC required
01g,15jul97,spm  added ARP request to SIOCSIFADDR ioctl handler
01f,19may97,spm  removed compiler warnings and unused function lnBcrRead()
01e,15may97,spm  reverted to bcopy routines for mbufs in BSD 4.4
01d,13may97,map  made ethernet addr copy, and ether packet type endian safe;
                 reinit BCNT for Rx descriptors  [SPR# 8431]
01c,28apr97,map  added lnRestart functionality [SPR# 8223]
01b,01apr97,spm  code cleanup, corrected statistics, and upgraded to BSD 4.4
01a,11jan95,vin	 written from 02q of if_ln.c with minimal clean up.
*/

/*
This module implements the Advanced Micro Devices Am79C970 PCnet-PCI
Ethernet 32 bit network interface driver.

The PCnet-PCI ethernet controller is inherently little endian because
the chip is designed to operate on a PCI bus which is a little endian
bus. The software interface to the driver is divided into three parts.
The first part is the PCI configuration registers and their set up. 
This part is done at the BSP level in the various BSPs which use this
driver. The second and third part are dealt in the driver. The second
part of the interface comprises of the I/O control registers and their
programming. The third part of the interface comprises of the descriptors
and the buffers. 

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

Big endian processors can be connected to the PCI bus through some controllers
which take care of hardware byte swapping. In such cases all the registers 
which the chip DMA s to have to be swapped and written to, so that when the
hardware swaps the accesses, the chip would see them correctly. The chip still
has to be programmed to operated in little endian mode as it is on the PCI bus.
If the cpu board hardware automatically swaps all the accesses to and from the
PCI bus, then input and output byte stream need not be swapped. 

BOARD LAYOUT
This device is on-board.  No jumpering diagram is necessary.

EXTERNAL INTERFACE
This driver provides the standard external interface with the following
exceptions.  All initialization is performed within the attach routine;
there is no separate initialization routine.  Therefore, in the global interface
structure, the function pointer to the initialization routine is NULL.

The only user-callable routine is lnPciattach(), which publishes the `lnPci'
interface and initializes the driver and device.

TARGET-SPECIFIC PARAMETERS
.iP "bus mode"
This parameter is a global variable that can be modified at run-time.

The LANCE control register #3 determines the bus mode of the device,
allowing the support of big-endian and little-endian architectures.
This parameter, defined as "u_long lnPciCSR_3B", is the value that will
be placed into LANCE control register #3.  The default value supports
Motorola-type buses.  For information about changing this parameter, see 
the manual
.I "Advanced Micro Devices Local Area Network Controller Am79C970 (PCnet-PCI)."

.iP "base address of device registers"
This parameter is passed to the driver by lnPciattach().  It 
indicates to the driver where to find the RDP register.

The LANCE presents two registers to the external interface, the RDP (register
data port) and RAP (register address port) registers.  This driver assumes 
that these two registers occupy two unique addresses in a memory space
that is directly accessible by the CPU executing this driver.  The driver
assumes that the RDP register is mapped at a lower address than the RAP
register; the RDP register is therefore considered the "base address."

.iP "interrupt vector"
This parameter is passed to the driver by lnPciattach().

This driver configures the LANCE device to generate hardware interrupts
for various events within the device; thus it contains
an interrupt handler routine.  The driver calls intConnect() to connect 
its interrupt handler to the interrupt vector generated as a result of 
the LANCE interrupt.

.iP "interrupt level"
This parameter is passed to the driver by lnPciattach().

Some targets use additional interrupt controller devices to help organize and
service the various interrupt sources.  This driver avoids all board-specific
knowledge of such devices.  During the driver's initialization, the external
routine sysLanIntEnable() is called to perform any board-specific operations
required to turn on LANCE interrupt generation. A similar routine,
sysLanIntDisable(), is called by the driver before a LANCE reset to perform
board-specific operations required to turn off LANCE interrupt generation.
For a description of sysLanIntEnable(), and sysLanIntDisable(), see "External
Support Requirements" below.

This parameter is passed to the external routine.

.iP "shared memory address"
This parameter is passed to the driver by lnPciattach().

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
This parameter is passed to the driver by lnPciattach().

This parameter can be used to explicitly limit the amount of shared
memory (bytes) this driver will use.  The constant NONE can be used to
indicate no specific size limitation.  This parameter is used only if
a specific memory region is provided to the driver.

.iP "shared memory width"
This parameter is passed to the driver by lnPciattach().

Some target hardware that restricts the shared memory region to a
specific location also restricts the access width to this region by
the CPU.  On these targets, performing an access of an invalid width
will cause a bus error.

This parameter can be used to specify the number of bytes of access
width to be used by the driver during access to the shared memory.
The constant NONE can be used to indicate no restrictions.

Current internal support for this mechanism is not robust; implementation 
may not work on all targets requiring these restrictions.

.iP "shared memory buffer size"
This parameter is passed to the driver by lnPciattach().

The driver and LANCE device exchange network data in buffers.  This
parameter permits the size of these individual buffers to be limited.
A value of zero indicates that the default buffer size should be used.
The default buffer size is large enough to hold a maximum-size Ethernet
packet.

Use of this parameter should be rare.  Network performance
will be affected, since the target will no longer be able to receive
all valid packet sizes.

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

This routine provides a target-specific enable of the interrupt for the LANCE
device.  Typically, this involves programming an interrupt controller
hardware, either internal or external to the CPU.

This routine is called during chip initialization, at startup and each LANCE
device reset.

.iP "void sysLanIntDisable (int level)" "" 9 -1

This routine provides a target-specific disable of the interrupt for the LANCE
device.  Typically, this involves programming an interrupt controller
hardware, either internal or external to the CPU.

This routine is called before a LANCE device reset.

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

If the driver is not given a specific region of memory via the lnPciattach()
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
.I "Advanced Micro Devices PCnet-PCI Ethernet Controller for PCI."
*/

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

#ifndef DOC             /* don't include when building documentation */
#include "net/mbuf.h"
#endif  /* DOC */

#include "net/protosw.h"
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

#include "drv/netif/if_lnPci.h"		/* device description header */


/***** LOCAL DEFINITIONS *****/

/* descriptor size */
#define RMD_SIZ  sizeof(ln_rmd)
#define TMD_SIZ  sizeof(ln_tmd)

/* Configuration items */

#define LN_MIN_FIRST_DATA_CHAIN 96      /* min size of 1st buf in data-chain */
#define LN_MAX_MDS              128     /* max number of [r|t]md's for LANCE */

#define LN_BUFSIZ      (ETHERMTU + ENET_HDR_REAL_SIZ + 6)

#define LN_RMD_RLEN     5       /* ring size as a power of 2 -- 32 RMD's */
#define LN_TMD_TLEN     5       /* same for transmit ring    -- 32 TMD's */

#define MAX_UNITS       1       /* maximum units supported */

/*
 * If LN_KICKSTART_TX is TRUE the transmitter is kick-started to force a
 * read of the transmit descriptors, otherwise the internal polling (1.6msec)
 * will initiate a read of the descriptors.  This should be FALSE is there
 * is any chance of memory latency or chip accesses detaining the LANCE DMA,
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
	((pciAdrs) - (pDrvCtrl->pciMemBase))
	
#define MEM_TO_PCI_PHYS(memAdrs) \
	((memAdrs) + (pDrvCtrl->pciMemBase))

/* Typedefs for external structures that are not typedef'd in their .h files */

typedef struct mbuf MBUF;
typedef struct arpcom IDR;                  /* Interface Data Record wrapper */
typedef struct ifnet IFNET;                 /* real Interface Data Record */
typedef struct sockaddr SOCK;

#ifdef BSD43_DRIVER

/* The Ethernet header */

typedef struct enet_hdr
    {
    char dst [6];
    char src [6];
    u_short type;
    } ENET_HDR;

#define ENET_HDR_SIZ        sizeof(ENET_HDR)

#endif            /* BSD43_DRIVER */

#define ENET_HDR_REAL_SIZ   14

/* The definition of the driver control structure */

typedef struct drv_ctrl
    {
    IDR         idr;                /* Interface Data Record */

    ln_ib       *ib;                 /* ptr to Initialization Block */

    struct
        {
        int     r_po2;                  /* RMD ring size as a power of 2! */
        int     r_size;                 /* RMD ring size (power of 2!) */
        int     r_index;                /* index into RMD ring */
        ln_rmd  *r_ring;                /* RMD ring */
        char    *r_bufs;                /* receive buffers base */
        } rmd_ring;

    struct
        {
        int     t_po2;                  /* TMD ring size as a power of 2! */
        int     t_size;                 /* TMD ring size (power of 2!) */
        int     t_index;                /* index into TMD ring */
        int     d_index;                /* index into TMD ring */
        ln_tmd  *t_ring;                /* TMD ring */
        char    *t_bufs;                /* transmit buffers base */
        } tmd_ring;

    BOOL        attached;               /* indicates unit is attached */
    SEM_ID      TxSem;                  /* transmitter semaphore */
    u_char      flags;                  /* misc control flags */
    int         ivec;                   /* interrupt vector */
    int         ilevel;                 /* interrupt level */
    LN_DEVICE   *devAdrs;               /* device structure address */
    char        *memBase;               /* LANCE memory pool base */
    int         memWidth;               /* width of data port */
    ULONG	pciMemBase;		/* memory base as seen from PCI*/
    int         bufSize;                /* size of buffer in the LANCE ring */
    CACHE_FUNCS cacheFuncs;             /* cache function pointers */

    } DRV_CTRL;

#define DRV_CTRL_SIZ    sizeof(DRV_CTRL)

/* Definitions for the flags field */

#define LS_PROMISCUOUS_FLAG     0x1
#define LS_MEM_ALLOC_FLAG       0x2
#define LS_PAD_USED_FLAG        0x4
#define LS_RCV_HANDLING_FLAG    0x8
#define LS_START_OUTPUT_FLAG    0x10

/* Shorthand structure references */

#define rpo2         rmd_ring.r_po2
#define rsize        rmd_ring.r_size
#define rindex       rmd_ring.r_index
#define rring        rmd_ring.r_ring

#define tpo2         tmd_ring.t_po2
#define tsize        tmd_ring.t_size
#define tindex       tmd_ring.t_index
#define dindex       tmd_ring.d_index
#define tring        tmd_ring.t_ring


/***** GLOBALS *****/

IMPORT unsigned char lnEnetAddr []; /* Ethernet address to load into lance */

ULONG lnPciCSR_3B = 0 ;    /* allows external setting of bus modes */

void panic ();
void sysLanIntEnable (int ilevel);
void sysLanIntDisable (int ilevel);
BOOL arpresolve ();


/***** LOCALS *****/

static int lnTsize = LN_TMD_TLEN;    /* deflt xmit ring size as power of 2 */
static int lnRsize = LN_RMD_RLEN;    /* deflt recv ring size as power of 2 */
static BOOL lnKickStartTx = LN_KICKSTART_TX;


/* The array of driver control structures */

static DRV_CTRL  drvCtrl [MAX_UNITS];

/* initial word offsets from LANCE base address to access these registers */

static u_int CSROffset = 0;
static u_int RAPOffset = 1;
static u_int BDPOffset = 3;

/* forward static functions */

static void 	lnReset (int unit);
static void 	lnInt (DRV_CTRL *pDrvCtrl);
static void 	lnHandleRecvInt (DRV_CTRL *pDrvCtrl);
static STATUS 	lnRecv (DRV_CTRL *pDrvCtrl, ln_rmd *rmd);
#ifdef BSD43_DRIVER
static int 	lnOutput (IDR *ifp, MBUF *m0, SOCK *dst);
#else
LOCAL  void 	lnPciStartOutput (DRV_CTRL * 	pDrvCtrl);
#endif /* BSD43_DRIVER */
static int 	lnIoctl (IDR *ifp, int cmd, caddr_t data);
static int 	lnChipReset (DRV_CTRL *pDrvCtrl);
static ln_rmd *	lnGetFullRMD (DRV_CTRL *pDrvCtrl);
static void 	lnCsrWrite (DRV_CTRL * pDrvCtrl, int reg, ULONG value);
static ULONG 	lnCsrRead (DRV_CTRL * pDrvCtrl, int reg);
static void 	lnBcrWrite (DRV_CTRL * pDrvCtrl, int reg, ULONG value);
static STATUS   lnRestartSetup (DRV_CTRL *pDrvCtrl);
static void     lnRestart (DRV_CTRL *pDrvCtrl);
#ifdef BSD43_DRIVER
static BOOL 	convertDestAddr (IDR *pIDR, SOCK *pDestSktAddr,
                                 char *pDestEnetAddr, u_short *pPacketType,
                                 MBUF *pMbuf);
#endif

/*******************************************************************************
*
* lnPciattach - publish the `lnPci' network interface and initialize the driver and device
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

STATUS lnPciattach
    (
    int		unit,		/* unit number */
    char *	devAdrs,	/* LANCE I/O address */
    int		ivec,		/* interrupt vector */
    int		ilevel,		/* interrupt level */
    char *	memAdrs,	/* address of memory pool (-1 = malloc it) */
    ULONG	memSize,	/* used if memory pool is NOT malloc()'d */
    int		memWidth,	/* byte-width of data (-1 = any width)     */
    ULONG    	pciMemBase,	/* memory base as seen from PCI*/
    int		spare2		/* not used */
    )
    {
    DRV_CTRL     *pDrvCtrl;
    unsigned int sz;           /* temporary size holder */
    char         *pTurkey;     /* start of the LANCE memory pool */


    /* Sanity check the unit number */

    if (unit < 0 || unit >= MAX_UNITS)
        return (ERROR);

    /* Ensure single invocation per system life */

    pDrvCtrl = & drvCtrl [unit];

    if (pDrvCtrl->attached)
        return (OK);

    /* Publish the interface data record */

#ifdef BSD43_DRIVER
    ether_attach (& pDrvCtrl->idr.ac_if, unit, "lnPci", (FUNCPTR) NULL,
                  (FUNCPTR) lnIoctl, (FUNCPTR) lnOutput, (FUNCPTR) lnReset);
#else
    ether_attach    (
                    &pDrvCtrl->idr.ac_if,
                    unit,
                    "lnPci",
                    (FUNCPTR) NULL,
                    (FUNCPTR) lnIoctl,
                    (FUNCPTR) ether_output,  /* generic output for Ethernet */
                    (FUNCPTR) lnReset
                    );

    pDrvCtrl->idr.ac_if.if_start = (FUNCPTR)lnPciStartOutput;

#endif

    /* Create the transmit semaphore. */

    pDrvCtrl->TxSem = semMCreate    (
                                    SEM_Q_PRIORITY |
                                    SEM_DELETE_SAFE |
                                    SEM_INVERSION_SAFE
                                    );

    if (pDrvCtrl->TxSem == NULL)
        {
        printf ("lnPci: error creating transmitter semaphore\n");
        return (ERROR);
        }


    { /***** Establish size of shared memory region we require *****/

    if ((int) memAdrs != NONE)  /* specified memory pool */
        {
        /*
         * With a specified memory pool we want to maximize
         * lnRsize and lnTsize
         */

        sz = (memSize - (RMD_SIZ + TMD_SIZ + sizeof (ln_ib)))
               / ((2 * LN_BUFSIZ) + RMD_SIZ + TMD_SIZ);

        sz >>= 1;               /* adjust for roundoff */

        for (lnRsize = 0; sz != 0; lnRsize++, sz >>= 1)
            ;

        lnTsize = lnRsize;      /* lnTsize = lnRsize for convenience */
        }

    /* limit ring sizes to reasonable values */

    if (lnRsize < 2)
        lnRsize = 2;            /* 4 buffers is reasonable min */

    if (lnRsize > 7)
        lnRsize = 7;            /* 128 buffers is max for chip */

    /* limit ring sizes to reasonable values */

    if (lnTsize < 2)
        lnTsize = 2;            /* 4 buffers is reasonable min */

    if (lnTsize > 7)
        lnTsize = 7;            /* 128 buffers is max for chip */


    /* Add it all up */

    sz = (sizeof (ln_ib)) +
         ( ((1 << lnRsize) + 1) * RMD_SIZ ) +
         (LN_BUFSIZ << lnRsize) +
         ( ((1 << lnTsize) + 1) * TMD_SIZ ) +
         (LN_BUFSIZ << lnTsize) +
         6;                        /* allow for alignment adjustment */
    }


    { /***** Establish a region of shared memory *****/

    /* OK. We now know how much shared memory we need.  If the caller
     * provides a specific memory region, we check to see if the provided
     * region is large enough for our needs.  If the caller did not
     * provide a specific region, then we attempt to allocate the memory
     * from the system, using the cache aware allocation system call.
     */

    switch ( (int) memAdrs )
        {
        default :       /* caller provided memory */
            if ( memSize < sz )     /* not enough space */
                {
                printf ( "lnPci: not enough memory provided\n" );
                return ( ERROR );
                }
            pTurkey = memAdrs;             /* set the beginning of pool */

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
                printf ( "lnPci: device requires cache coherent memory\n" );
                return (ERROR);
                }

            pTurkey = (char *) cacheDmaMalloc ( sz );

            if ((int)pTurkey == NULL)
                {
                printf ( "lnPci: system memory unavailable\n" );
                return (ERROR);
                }

            /* copy the DMA structure */

            pDrvCtrl->cacheFuncs = cacheDmaFuncs;

            break;
        }
    }

    /*                        Turkey Carving
     *                        --------------
     *
     *                          LOW MEMORY
     *
     *             |-------------------------------------|
     *             |       The initialization block      |
     *             |         (sizeof (ln_ib))            |
     *             |-------------------------------------|
     *             |         The Rx descriptors          |
     *             | (1 << lnRsize) * sizeof (ln_rmd)|
     *             |-------------------------------------|
     *             |          The receive buffers        |
     *             |       (LN_BUFSIZ << lnRsize)        |
     *             |-------------------------------------|
     *             |         The Tx descriptors          |
     *             | (1 << lnTsize) * sizeof (ln_tmd)|
     *             |-------------------------------------|
     *             |           The transmit buffers      |
     *             |       (LN_BUFSIZ << lnTsize)        |
     *             |-------------------------------------|
     */

    /* Save some things */

    pDrvCtrl->memBase  = (char *)((ULONG)pTurkey & 0xff000000);
    pDrvCtrl->memWidth = memWidth;
    pDrvCtrl->bufSize = LN_BUFSIZ;
    pDrvCtrl->pciMemBase  = pciMemBase;		/* pci memory base */

    if ((int) memAdrs == NONE)
        pDrvCtrl->flags |= LS_MEM_ALLOC_FLAG;

    { /***** Carve up the turkey *****/

    /* First let's clean the whole turkey */

    bzero ( (char *) pTurkey, (int) sz );

    /* carve out initialization block */

    pDrvCtrl->ib = (ln_ib *)pTurkey;
    sz = sizeof (ln_ib);                /* size of initialization block */

    /* carve out receive message descriptor (RMD) ring structure */

    pDrvCtrl->rpo2 = lnRsize;              /* remember for lnConfig */
    pDrvCtrl->rsize = (1 << lnRsize);      /* receive ring size */

    /* make it 16 byte aligned */

    pDrvCtrl->rring = (ln_rmd *) (((int)pDrvCtrl->ib + sz + 0x0f) & ~0x0f);

    sz = (1 << lnRsize) * RMD_SIZ;

    pDrvCtrl->rmd_ring.r_bufs = (char *)((int)pDrvCtrl->rring + sz);

    sz = (LN_BUFSIZ << lnRsize);   /* room for all the receive buffers */

    /* carve out transmit message descriptor (TMD) ring structure */

    pDrvCtrl->tpo2 = lnTsize;              /* remember for lnConfig */
    pDrvCtrl->tsize = (1 << lnTsize);      /* transmit ring size */

    pDrvCtrl->tring = (ln_tmd *) (((int)pDrvCtrl->rmd_ring.r_bufs + sz + 0x0f)
				  & ~0x0f);

    sz = (1 << lnTsize) * TMD_SIZ;

    /* carve out transmit buffer space */

    pDrvCtrl->tmd_ring.t_bufs = (char *)((int)pDrvCtrl->tring + sz);
    }


    /* Save some values */

    pDrvCtrl->ivec       = ivec;                 /* interrupt vector */
    pDrvCtrl->ilevel     = ilevel;               /* interrupt level */
    pDrvCtrl->devAdrs    = (LN_DEVICE *)devAdrs; /* LANCE I/O address */

    /* Obtain our Ethernet address and save it */

    bcopy ((char *) lnEnetAddr, (char *)pDrvCtrl->idr.ac_enaddr, 6);

    /***** Device Initializations *****/

    if (lnChipReset (pDrvCtrl) == ERROR)	/* reset lance device */
    	{
        logMsg ("lnPci: Device failed to reset\n", 0,0,0,0,0,0);
 	return (ERROR);
	}

    if (intConnect ((VOIDFUNCPTR *)INUM_TO_IVEC(ivec),lnInt,(int)pDrvCtrl)
        == ERROR)
        return (ERROR);

    if (lnRestartSetup (pDrvCtrl) == ERROR)
        return (ERROR);

    pDrvCtrl->attached = TRUE;

    return (OK);
    }

/*******************************************************************************
*
* lnReset - reset the interface
*
* Mark interface as inactive & reset the chip
*/

static void lnReset
    (
    int unit
    )
    {
    DRV_CTRL *pDrvCtrl = & drvCtrl [unit];

    pDrvCtrl->idr.ac_if.if_flags = 0;
    lnChipReset (pDrvCtrl);                           /* reset LANCE */
    }

/*******************************************************************************
*
* lnInt - handle controller interrupt
*
* This routine is called at interrupt level in response to an interrupt from
* the controller.
*/

static void lnInt
    (
    DRV_CTRL  *pDrvCtrl
    )
    {
    ln_tmd         *tmd;
    int            *pDindex;
    int            *pTindex;
    int            *pTsize;
    ln_tmd         *pTring;
    ULONG	   stat;


    /* Read the device status register */

    stat = lnCsrRead (pDrvCtrl, 0);

    /* If false interrupt, return. */

    if ( ! (stat & CSR0_INTR) )
        return;

    /*
     * enable interrupts, clear receive and/or transmit interrupts, and clear
     * any errors that may be set.
     */

    lnCsrWrite (pDrvCtrl, 0, ((stat &
            (CSR0_BABL|CSR0_CERR|CSR0_MISS|CSR0_MERR|
             CSR0_RINT|CSR0_TINT|CSR0_IDON)) | CSR0_INEA));

    /* Check for errors */

    if (stat & (CSR0_BABL | CSR0_MISS | CSR0_MERR))
        {
        ++pDrvCtrl->idr.ac_if.if_ierrors;

        /* restart chip on fatal error */

        if (stat & CSR0_MERR)        /* memory error */
            {
            pDrvCtrl->idr.ac_if.if_flags &= ~(IFF_UP | IFF_RUNNING);
            (void) netJobAdd (
                             (FUNCPTR)lnRestart,
                             pDrvCtrl->idr.ac_if.if_unit,
                             0,0,0,0
                             );
            }
        }

    /* Have netTask handle any input packets */

    if ((stat & CSR0_RINT) && (stat & CSR0_RXON))
        {
        if ( ! (pDrvCtrl->flags & LS_RCV_HANDLING_FLAG) )
            {
            pDrvCtrl->flags |= LS_RCV_HANDLING_FLAG;
            (void) netJobAdd (
                             (FUNCPTR)lnHandleRecvInt,
                             (int)pDrvCtrl,
                             0,0,0,0
                             );
            }
        }

    /*
     * Did LANCE update any of the TMD's?
     * If not then don't bother continuing with transmitter stuff
     */

    if (!(stat & CSR0_TINT))
        return;

    pDindex = &pDrvCtrl->dindex;
    pTindex = &pDrvCtrl->tindex;
    pTsize  = &pDrvCtrl->tsize;
    pTring  = pDrvCtrl->tring;

    while (*pDindex != *pTindex)
        {
        /* disposal has not caught up */

        tmd = pTring + *pDindex;

        /* if the buffer is still owned by LANCE, don't touch it */

        LN_CACHE_INVALIDATE (tmd, TMD_SIZ);

        if (PCISWAP(tmd->tbuf_tmd1) & TMD1_OWN)
            break;

        /*
         * tbuf_err (TMD1.ERR) is an "OR" of LCOL, LCAR, UFLO or RTRY.
         * Note that BUFF is not indicated in TMD1.ERR.
         * We should therefore check both tbuf_err and tbuf_buff
         * here for error conditions.
         */

        if ((PCISWAP(tmd->tbuf_tmd1) & TMD1_ERR) || 
	    (PCISWAP(tmd->tbuf_tmd2) & TMD2_BUFF))
            {

            pDrvCtrl->idr.ac_if.if_oerrors++;     /* output error */
            pDrvCtrl->idr.ac_if.if_opackets--;

            /* If error was due to excess collisions, bump the collision
             * counter.  The LANCE does not keep an individual counter of
             * collisions, so in this driver, the collision statistic is not
             * an accurate count of total collisions.
             */

            if (PCISWAP(tmd->tbuf_tmd2) & TMD2_RTRY)
                pDrvCtrl->idr.ac_if.if_collisions++;

            /* check for no carrier */

            if (PCISWAP(tmd->tbuf_tmd2) & TMD2_LCAR)
                logMsg ("ln%d: no carrier\n",
                    pDrvCtrl->idr.ac_if.if_unit, 0,0,0,0,0);

            /* Restart chip on fatal errors.
             * The following code handles the situation where the transmitter
             * shuts down due to an underflow error.  This is a situation that
             * will occur if the DMA cannot keep up with the transmitter.
             * It will occur if the LANCE is being held off from DMA access
             * for too long or due to significant memory latency.  DRAM
             * refresh or slow memory could influence this.  Many
             * implementation use a dedicated LANCE buffer.  This can be
             * static RAM to eliminate refresh conflicts; or dual-port RAM
             * so that the LANCE can have free run of this memory during its
             * DMA transfers.
             */

            if ((PCISWAP(tmd->tbuf_tmd2) & TMD2_BUFF) || 
		(PCISWAP(tmd->tbuf_tmd2) & TMD2_UFLO))
                {
                pDrvCtrl->idr.ac_if.if_flags &= ~(IFF_UP | IFF_RUNNING);
                (void) netJobAdd (
                                 (FUNCPTR)lnRestart,
                                 pDrvCtrl->idr.ac_if.if_unit,
                                 0,0,0,0
                                 );
                return;
                }
            }

        tmd->tbuf_tmd1 = 0;		/* clear tmd1 */
        tmd->tbuf_tmd2 = 0;		/* clear all error & stat stuff */

        /* now bump the tmd disposal index pointer around the ring */

        *pDindex = (*pDindex + 1) & (*pTsize - 1);
        }

    /* Flush the write pipe */

    CACHE_PIPE_FLUSH ();

    }

/*******************************************************************************
*
* lnHandleRecvInt - task level interrupt service for input packets
*
* This routine is called at task level indirectly by the interrupt
* service routine to do any message received processing.
*/

static void lnHandleRecvInt
    (
    DRV_CTRL *pDrvCtrl
    )
    {
    ln_rmd *rmd;

    do
        {
        pDrvCtrl->flags |= LS_RCV_HANDLING_FLAG;

        while ((rmd = lnGetFullRMD (pDrvCtrl)) != NULL)
            lnRecv (pDrvCtrl, rmd);

        /*
         * There is a RACE right here.  The ISR could add a receive packet
         * and check the boolean below, and decide to exit.  Thus the
         * packet could be dropped if we don't double check before we
         * return.
         */

        pDrvCtrl->flags &= ~LS_RCV_HANDLING_FLAG;
        }
    while (lnGetFullRMD (pDrvCtrl) != NULL);
    /* this double check solves the RACE */
    }

/*******************************************************************************
*
* lnGetFullRMD - get next received message RMD
*
* Returns ptr to next Rx desc to process, or NULL if none ready.
*/

static ln_rmd *lnGetFullRMD
    (
    DRV_CTRL  *pDrvCtrl
    )
    {
    ln_rmd   *rmd;

    /* Refuse to do anything if flags are down */

    if  (
        (pDrvCtrl->idr.ac_if.if_flags & (IFF_UP | IFF_RUNNING) ) !=
        (IFF_UP | IFF_RUNNING)
        )
        return ((ln_rmd *) NULL);

    rmd = pDrvCtrl->rring + pDrvCtrl->rindex;       /* form ptr to Rx desc */

    LN_CACHE_INVALIDATE (rmd, RMD_SIZ);

    if ((PCISWAP(rmd->rbuf_rmd1) & RMD1_OWN) == 0)
        return (rmd);
    else
        return ((ln_rmd *) NULL);
    }

/*******************************************************************************
*
* lnRecv - process the next incoming packet
*
*/

static STATUS lnRecv
    (
    DRV_CTRL *pDrvCtrl,
    ln_rmd *rmd
    )
    {
    struct ether_header * 	pEnetHdr;
    MBUF        *pMbuf;
    u_char      *pData;
    int         len;
    u_short     ether_type;
    BOOL        hookAte;


    /* Packet must be checked for errors. */

    if  (
        /* If error flag */
        (PCISWAP(rmd->rbuf_rmd1) & (RMD1_ERR | RMD1_FRAM)) ||

        /* OR if packet is not completely in one buffer */
        (
        (PCISWAP(rmd->rbuf_rmd1) & (RMD1_STP | RMD1_ENP) ) !=
        (RMD1_STP | RMD1_ENP)
        )

        )
        {
        ++pDrvCtrl->idr.ac_if.if_ierrors;       /* bump error stat */
        goto cleanRXD;                          /* skip to clean up */
        }

    len = PCISWAP(rmd->rbuf_mcnt) & RMD2_MCNT_MSK;	/* get packet length */

    /* Get pointer to packet */

    pEnetHdr = (struct ether_header *)
                (pDrvCtrl->rmd_ring.r_bufs + (pDrvCtrl->rindex * LN_BUFSIZ));
    LN_CACHE_INVALIDATE (pEnetHdr, len);   /* make the packet data coherent */

    ++pDrvCtrl->idr.ac_if.if_ipackets;		/* bump statistic */

    /* call input hook if any */

    hookAte = FALSE;
    if  (etherInputHookRtn != NULL)
        {
        if  (
            (* etherInputHookRtn) (&pDrvCtrl->idr.ac_if, (char *)pEnetHdr, len)
            )
            hookAte = TRUE;
        }

    /* Normal path; send packet upstairs */

    if (hookAte == FALSE)
        {
        /* Adjust length to size of data only */

        len -= ENET_HDR_REAL_SIZ;

        /* Get pointer to packet data */

        pData = ((u_char *) pEnetHdr) + ENET_HDR_REAL_SIZ;

        ether_type = ntohs ( pEnetHdr->ether_type );

#ifdef BSD43_DRIVER

        /* Copy packet data into MBUFs, using the specified width */

        pMbuf = bcopy_to_mbufs (pData, len, 0, (IFNET *) & pDrvCtrl->idr.ac_if,
                            pDrvCtrl->memWidth);

        if (pMbuf != NULL)
            do_protocol_with_type (ether_type, pMbuf, & pDrvCtrl->idr, len);
        else
            ++pDrvCtrl->idr.ac_if.if_ierrors;    /* bump error statistic */
#else
        pMbuf = bcopy_to_mbufs (pData, len, 0, &pDrvCtrl->idr.ac_if, 
                                pDrvCtrl->memWidth);
        if (pMbuf != NULL)
            do_protocol (pEnetHdr, pMbuf, &pDrvCtrl->idr, len);
        else
            ++pDrvCtrl->idr.ac_if.if_ierrors;    /* bump error statistic */
#endif
        }

    /* Done with descriptor, clean up and give it to the device. */

    cleanRXD:
    /* reset statistics counters and message byte count */
    rmd->rbuf_mcnt = 0;

    /* reset status bits, reinitialize buffer size, and give device ownership */
    rmd->rbuf_rmd1 = PCISWAP((RMD1_BCNT_MSK & - (pDrvCtrl->bufSize)) |
                              RMD1_CNST | RMD1_OWN);

    /* Flush the write pipe */

    CACHE_PIPE_FLUSH ();

    /* Advance our management index */

    pDrvCtrl->rindex = (pDrvCtrl->rindex + 1) & (pDrvCtrl->rsize - 1);

    return (OK);
    }

#ifdef BSD43_DRIVER

/*******************************************************************************
*
* lnOutput - the driver output routine
*
*/

static int lnOutput
    (
    IDR  *pIDR,
    MBUF *pMbuf,
    SOCK *pDest
    )
    {
    char destEnetAddr [6];                  /* space for enet addr */
    u_short packetType;                     /* type field for the packet */
    DRV_CTRL    *pDrvCtrl;
    ln_tmd      *tmd;
    char        *buf;
    int         len;
    int         oldLevel;

    /* Check ifnet flags. Return error if incorrect. */

    if  (
        (pIDR->ac_if.if_flags & (IFF_UP | IFF_RUNNING)) !=
        (IFF_UP | IFF_RUNNING)
        )
        return (ENETDOWN);

    /* Attempt to convert socket addr into enet addr and packet type.
     * Note that if ARP resolution of the address is required, the ARP
     * module will call our routine again to transmit the ARP request
     * packet.  This means we may actually call ourselves recursively!
     */

    if (convertDestAddr (pIDR,pDest, destEnetAddr, &packetType, pMbuf) == FALSE)
        return (OK);    /* I KNOW returning OK is stupid, but it is correct */

    /* Get driver control pointer */

    pDrvCtrl = & drvCtrl [pIDR->ac_if.if_unit];

    /* Obtain exclusive access to transmitter.  This is necessary because
     * certain events can cause netTask to run a job that attempts to transmit
     * a packet.  We can only allow one task here at a time.
     */

    semTake (pDrvCtrl->TxSem, WAIT_FOREVER);

    pDrvCtrl->flags |= LS_START_OUTPUT_FLAG;

    /* See if next TXD is available */

    tmd = pDrvCtrl->tring + pDrvCtrl->tindex;

    LN_CACHE_INVALIDATE (tmd, TMD_SIZ);

    if  (
        ((PCISWAP(tmd->tbuf_tmd1) & TMD1_OWN) != 0) ||
        (
        ((pDrvCtrl->tindex + 1) & (pDrvCtrl->tsize - 1)) ==
        pDrvCtrl->dindex
        )
        )
        {
        m_freem (pMbuf);            /* Discard data */
        pIDR->ac_if.if_oerrors++;
        goto outputDone;
        }

    /* Get pointer to transmit buffer */

    buf = (char *)
        (pDrvCtrl->tmd_ring.t_bufs + (pDrvCtrl->tindex * LN_BUFSIZ));

    /* Fill in the Ethernet header */

    bcopy (destEnetAddr, buf, 6);
    bcopy ( (char *) pIDR->ac_enaddr, (buf + 6), 6);
    ((ENET_HDR *)buf)->type = htons (packetType);

    /* Copy packet from MBUFs to our transmit buffer.  MBUFs are
     * transparently freed.
     */

    bcopy_from_mbufs    (
                        (buf + ENET_HDR_REAL_SIZ),
                        pMbuf,
                        len,                            /* stuffed by macro */
                        pDrvCtrl->memWidth
                        );

    /* Ensure we send a legal size frame. */

    len += ENET_HDR_REAL_SIZ;
    len = max (ETHERSMALL, len);

    /* place a transmit request */

    oldLevel = intLock ();          /* disable ints during update */

    tmd->tbuf_tmd3 = 0;                     /* clear buffer error status */

    /* negative message byte count */
    tmd->tbuf_tmd1 &= PCISWAP(~TMD1_BCNT_MSK);
    tmd->tbuf_tmd1 |= PCISWAP((TMD1_CNST | TMD1_BCNT_MSK) & -len);

    tmd->tbuf_tmd1 |= PCISWAP(TMD1_ENP);    /* buffer is end of packet */
    tmd->tbuf_tmd1 |= PCISWAP(TMD1_STP);    /* buffer is start of packet */
    tmd->tbuf_tmd1 &= PCISWAP(~TMD1_DEF);   /* clear status bit */
    tmd->tbuf_tmd1 &= PCISWAP(~TMD1_MORE);
    tmd->tbuf_tmd1 &= PCISWAP(~TMD1_ERR);
    tmd->tbuf_tmd1 |= PCISWAP(TMD1_OWN);

    /* Flush the write pipe */

    CACHE_PIPE_FLUSH ();

    intUnlock (oldLevel);   /* now lnInt won't get confused */

    /* Advance our management index */

    pDrvCtrl->tindex = (pDrvCtrl->tindex + 1) & (pDrvCtrl->tsize - 1);

    /* kick start the transmitter, if selected */

    if (lnKickStartTx)
        lnCsrWrite (pDrvCtrl, 0, (CSR0_INEA | CSR0_TDMD));

    pDrvCtrl->flags &= ~LS_START_OUTPUT_FLAG;

    /* Bump the statistic counter. */

    pIDR->ac_if.if_opackets++;

    outputDone:

    /* Release exclusive access. */

    semGive (pDrvCtrl->TxSem);

    return (OK);

    }

#else           /* BSD43_DRIVER */

/*******************************************************************************
*
* lnPciStartOutput - the driver output routine
*
*/

LOCAL void lnPciStartOutput
    (
    DRV_CTRL *  pDrvCtrl        /* pointer to driver control structure */
    )
    {
    struct mbuf * pMbuf;
    ln_tmd      *tmd;
    char        *buf;
    int         len;
    int         oldLevel;

    pDrvCtrl->flags |= LS_START_OUTPUT_FLAG;        /* ??? */

    /* See if next TXD is available */

   while (pDrvCtrl->idr.ac_if.if_snd.ifq_head)
        {
        /* Dequeue a packet from the transmit queue. */
  
        IF_DEQUEUE (&pDrvCtrl->idr.ac_if.if_snd, pMbuf);

        tmd = pDrvCtrl->tring + pDrvCtrl->tindex;

        LN_CACHE_INVALIDATE (tmd, TMD_SIZ);

        if ( ( (PCISWAP(tmd->tbuf_tmd1) & TMD1_OWN) != 0) ||
              ( ( (pDrvCtrl->tindex + 1) & (pDrvCtrl->tsize - 1)) ==
                                                           pDrvCtrl->dindex))
            {
            m_freem (pMbuf);
            pDrvCtrl->idr.ac_if.if_oerrors++;
            break;
            }

        /* Get pointer to transmit buffer */

        buf = (char *)
            (pDrvCtrl->tmd_ring.t_bufs + (pDrvCtrl->tindex * LN_BUFSIZ));

        /* Copy packet from MBUFs to transmit buffer. */

        bcopy_from_mbufs (buf, pMbuf, len, pDrvCtrl->memWidth);

        /* Ensure we send a legal size frame. */

        len = max (ETHERSMALL, len);

        /* place a transmit request */

        oldLevel = intLock ();          /* disable ints during update */

        tmd->tbuf_tmd3 = 0;             /* clear buffer error status */

        /* negative message byte count */

        tmd->tbuf_tmd1 &= PCISWAP(~TMD1_BCNT_MSK);
        tmd->tbuf_tmd1 |= PCISWAP((TMD1_CNST | TMD1_BCNT_MSK) & -len);

        tmd->tbuf_tmd1 |= PCISWAP(TMD1_ENP);    /* buffer is end of packet */
        tmd->tbuf_tmd1 |= PCISWAP(TMD1_STP);    /* buffer is start of packet */
        tmd->tbuf_tmd1 &= PCISWAP(~TMD1_DEF);   /* clear status bit */
        tmd->tbuf_tmd1 &= PCISWAP(~TMD1_MORE);
        tmd->tbuf_tmd1 &= PCISWAP(~TMD1_ERR);
        tmd->tbuf_tmd1 |= PCISWAP(TMD1_OWN);

        /* Flush the write pipe */

        CACHE_PIPE_FLUSH ();

        intUnlock (oldLevel);   /* now lnInt won't get confused */

        /* Advance our management index */

        pDrvCtrl->tindex = (pDrvCtrl->tindex + 1) & (pDrvCtrl->tsize - 1);

        /* kick start the transmitter, if selected */

        if (lnKickStartTx)
            lnCsrWrite (pDrvCtrl, 0, (CSR0_INEA | CSR0_TDMD));

        pDrvCtrl->flags &= ~LS_START_OUTPUT_FLAG;

        /* Bump the statistic counter. */

        pDrvCtrl->idr.ac_if.if_opackets++;   /* bump the statistic counter. */
        }
    return;
    }

#endif

/*******************************************************************************
*
* lnIoctl - the driver I/O control routine
*
* Process an ioctl request.
*/

static int lnIoctl
    (
    IDR  *ifp,
    int            cmd,
    caddr_t        data
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

static int lnChipReset
    (
    DRV_CTRL  *pDrvCtrl
    )
    {
    ULONG * dv = (ULONG *)pDrvCtrl->devAdrs;

    /* Enable 32 bit access by doing a 32 bit write */

    * (dv) = 0;

    lnCsrWrite (pDrvCtrl, CSR(0), CSR0_STOP);

    lnBcrWrite (pDrvCtrl, BCR(20), BCR20_SWSTYLE_PCNET);

    return (OK);
    }

/*******************************************************************************
*
* lnCsrWrite - select and write a CSR register
*
*/

static void lnCsrWrite
    (
    DRV_CTRL * 	pDrvCtrl,		/* driver control */
    int 	reg,			/* register to select */
    ULONG 	value			/* value to write */
    )
    {
    ULONG *dv = (ULONG *)pDrvCtrl->devAdrs;

    /* select CSR */

    *(dv+RAPOffset) = PCISWAP(((ULONG)reg & 0x000000FF));

    /* write val to CSR */

    *(dv+CSROffset) = PCISWAP(((ULONG)value & 0x0000FFFF));
    }

/*******************************************************************************
*
* lnCsrRead - select and read a CSR register
*
*/

static ULONG lnCsrRead
    (
    DRV_CTRL *	pDrvCtrl,		/* driver control */
    int		reg			/* register to select */
    )
    {
    ULONG *dv = (ULONG *)pDrvCtrl->devAdrs;

    *(dv+RAPOffset) = PCISWAP(((ULONG)reg & 0x000000FF));	/* select CSR */

    /* get contents of CSR */

    return ( PCISWAP(*(dv+CSROffset)) & (0x0000FFFF));
    }

/*******************************************************************************
*
* lnBcrWrite - select and write a CSR register
*
*/

static void lnBcrWrite
    (
    DRV_CTRL * 	pDrvCtrl,		/* driver control */
    int 	reg,			/* register to select */
    ULONG 	value			/* value to write */
    )
    {
    ULONG *dv = (ULONG *)pDrvCtrl->devAdrs;

    /* select CSR */

    *(dv+RAPOffset) = PCISWAP(((ULONG)reg & 0x000000FF));

    /* write val to BCR */

    *(dv+BDPOffset) = PCISWAP(((ULONG)value & 0x0000FFFF));
    }

/******************************************************************************
*
* lnRestartSetup - setup memory descriptors and turn on chip
*
* Initializes all the shared memory structures and turns on the chip.
*/

LOCAL STATUS lnRestartSetup
    (
    DRV_CTRL *pDrvCtrl
    )
    {
    ULONG       stat = 0;
    char        *buf;
    int         loopy;
    int         timeoutCount = 0;
    ln_ib       *ib;
    ln_rmd      *rmd;
    ln_tmd      *tmd;
    void        *pTemp;


    /* Set up the Rx descriptors */

    rmd = pDrvCtrl->rring;                      /* receive ring */
    buf = pDrvCtrl->rmd_ring.r_bufs;
    for (loopy = 0; loopy < pDrvCtrl->rsize; loopy++)
      {
        pTemp = LN_CACHE_VIRT_TO_PHYS (buf);     /* convert to physical addr */
        pTemp = (void *)(MEM_TO_PCI_PHYS((ULONG)pTemp));
        rmd->rbuf_adr = PCISWAP((ULONG) pTemp); /* bits 31:00 of buf addr */
        rmd->rbuf_rmd1 = 0;
        /* neg of buffer byte count */
        rmd->rbuf_rmd1 = PCISWAP((RMD1_BCNT_MSK & -(pDrvCtrl->bufSize)) | 
                                  RMD1_CNST);
        rmd->rbuf_mcnt = 0;                     /* no message byte count yet */
        rmd->rbuf_rmd1 |= PCISWAP(RMD1_OWN);    /* buffer now owned by LANCE */
        rmd++;                                  /* step to next descriptor */
        buf += (pDrvCtrl->bufSize);             /* step to the next buffer */
      }

    pDrvCtrl->rindex = 0;


    /* Setup the Tx descriptors */

    tmd = pDrvCtrl->tring;                 /* transmit ring */
    buf = pDrvCtrl->tmd_ring.t_bufs;

    for (loopy = 0; loopy < pDrvCtrl->tsize; loopy++)
      {
        pTemp = LN_CACHE_VIRT_TO_PHYS (buf);     /* convert to physical addr */
        pTemp = (void *)(MEM_TO_PCI_PHYS((ULONG)pTemp));
        tmd->tbuf_adr = PCISWAP((ULONG) pTemp); /* bits 31:00 of buf addr */
        tmd->tbuf_tmd1 = PCISWAP(TMD1_CNST);    /* no message byte count yet */
        tmd->tbuf_tmd2 = 0;                     /* no error status yet */
        tmd->tbuf_tmd1 |= PCISWAP(TMD1_ENP);    /* buffer is end of packet */
        tmd->tbuf_tmd1 |= PCISWAP(TMD1_STP);    /* buffer is start of packet */
        tmd++;                                  /* step to next descriptor */
        buf += (pDrvCtrl->bufSize);             /* step to next buffer */
      }

    pDrvCtrl->tindex =
        pDrvCtrl->dindex = 0;

    /* Setup the initialization block */

    ib = pDrvCtrl->ib;
    ib->lnIBMode = 0;       /* chip will be in normal receive mode */

#if (_BYTE_ORDER == _BIG_ENDIAN)
    bcopy ((char *)pDrvCtrl->idr.ac_enaddr, ib->lnIBPadr, 6);
#else
    swab ((char *)pDrvCtrl->idr.ac_enaddr, ib->lnIBPadr, 4);
    swab ((char *)&pDrvCtrl->idr.ac_enaddr[4], &ib->lnIBPadr[6], 2);
#endif /* _BYTE_ORDER == _BIG_ENDIAN */

    pTemp = LN_CACHE_VIRT_TO_PHYS ( pDrvCtrl->rring ); /* point to Rx ring */
    pTemp = (void *)(MEM_TO_PCI_PHYS((ULONG)pTemp));
    ib->lnIBRdra = PCISWAP((ULONG)pTemp);
    ib->lnIBMode |=  PCISWAP((IB_MODE_RLEN_MSK & (pDrvCtrl->rpo2 << 20)));

    pTemp = LN_CACHE_VIRT_TO_PHYS ( pDrvCtrl->tring ); /* point to Tx ring */
    pTemp = (void *)(MEM_TO_PCI_PHYS((ULONG)pTemp));
    ib->lnIBTdra = PCISWAP((ULONG)pTemp);
    ib->lnIBMode |= PCISWAP((IB_MODE_TLEN_MSK & (pDrvCtrl->tpo2 << 28)));

    /* Flush the write pipe */

    CACHE_PIPE_FLUSH ();


    lnCsrWrite (pDrvCtrl, 0, CSR0_STOP);        /* set the stop bit */

    /* set the bus mode to little endian */

    lnCsrWrite (pDrvCtrl, 3, lnPciCSR_3B);

    /* Point the device to the initialization block */

    pTemp = LN_CACHE_VIRT_TO_PHYS ( pDrvCtrl->ib );
    pTemp = (void *)(MEM_TO_PCI_PHYS((ULONG)pTemp));
    lnCsrWrite (pDrvCtrl, 2, (((ULONG)pTemp >> 16) & 0x0000ffff));
    lnCsrWrite (pDrvCtrl, 1, ((ULONG)pTemp & 0x0000ffff));
    lnCsrWrite (pDrvCtrl, 0, CSR0_INIT);            /* init chip (read IB) */

    while (((stat = lnCsrRead (pDrvCtrl, 0)) & (CSR0_IDON | CSR0_ERR)) == 0)
      {
        if (timeoutCount++ > 0x10000) break;
        taskDelay(100);
      }

    if ((stat & CSR0_ERR) == CSR0_ERR)
      {
        if ((stat & CSR0_MERR)  == CSR0_MERR)
	  {
            logMsg ("lnPci: Lance memory error while initialization:\t0x%x\n",
                    stat, 0,0,0,0,0);
	  }
        else
	  {
            logMsg ("lnPci: Lance error while initialization:\t0x%x\n",
                    stat, 0,0,0,0,0);
	  }
      }

    /* log chip initialization failure */

    if ( (stat & CSR0_ERR) || (timeoutCount >= 0x10000) )
      {
        logMsg ("lnPci%d: Device initialization failed\n",
                    pDrvCtrl->idr.ac_if.if_unit, 0,0,0,0,0);
        return (ERROR);
      }

    /* Device is initialized.  Start transmitter and receiver.  The device
     * RAP register is left selecting CSR 0.
     */

    lnCsrWrite(pDrvCtrl, 0, CSR0_IDON | CSR0_INEA | CSR0_STRT);

    sysLanIntEnable (pDrvCtrl->ilevel);           /* enable LANCE interrupts */
 
    pDrvCtrl->idr.ac_if.if_flags |= (IFF_UP | IFF_RUNNING | IFF_NOTRAILERS);

    return (OK);
    }

/*******************************************************************************
*
* lnRestart - restart the device after a fatal error
*
* This routine takes care of all the messy details of a restart.  The device
* is reset and re-initialized.  The driver state is re-synchronized.
*/

static void lnRestart
    (
    DRV_CTRL *pDrvCtrl
    )
    {
    printf ("lnPci: device reset\n");

    if (lnChipReset (pDrvCtrl) == ERROR)        /* reset lance device */
        {
        logMsg ("lnPci: Device failed to reset\n", 0,0,0,0,0,0);
        return;
        }

    lnRestartSetup (pDrvCtrl);

    return;
    }

#ifdef BSD43_DRIVER

#include "sys/socket.h"

/*******************************************************************************
*
* convertDestAddr - converts socket addr into enet addr and packet type
*
*/

static BOOL convertDestAddr
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
    ENET_HDR *pEnetHdr;

    if (pDestSktAddr->sa_family == AF_UNSPEC)
        {
        pEnetHdr = (ENET_HDR *) pDestSktAddr->sa_data;      /* ptr to hdr */
        bcopy (pEnetHdr->dst, pDestEnetAddr, 6);            /* copy dst addr */
        *pPacketType = pEnetHdr->type;                      /* copy type */
        return (TRUE);
        }
    }


    /* Unsupported family */

    return (FALSE);

    } /* End of convertDestAddr() */

/* END OF FILE */

#endif
