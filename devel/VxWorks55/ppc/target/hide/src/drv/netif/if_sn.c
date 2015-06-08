/* if_sn.c - National Semiconductor DP83932B SONIC Ethernet network driver */

/* Copyright 1984-1999 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/* Copyright 1991 Ironics Inc. */

/*
modification history
--------------------
01w,16mar99,elg  add unsupported features in documentation (SPR 9145).
01v,15jul97,map  added code to pullup highly fragmented mbufs [SPR# 20843]
01u,15jul97,spm  added ARP request to SIOCSIFADDR ioctl handler
01t,07apr97,spm  code cleanup, corrected statistics, and upgraded to BSD 4.4
01s,23apr96,wkm  allow extra space for Sonic buffer overrun, zero sequence cnt
o1r,16jan96,kkk  increase number of TX desc and TX frags.
o1q,13feb95,jdi  doc cleanup.
o1p,18oct93,cd   fixed virtual/physical address confusion, added extra
		 debug options, corrected comments.
           +caf  made new snClkEnable() and snClkDisable() routines NOMANUAL.
01q,11aug93,jmm  Changed ioctl.h and socket.h to sys/ioctl.h and sys/socket.h
01p,19feb93,jdi  documentation cleanup.
01o,06feb93,rfs  Fixed SPR #1990.
01n,05feb93,rfs  Fixed SPR #1984.
01m,13oct92,rfs  Added documentation.
01l,02oct92,rfs  Made multiple attach calls per unit return OK.
                 Adapted to new convention for driver control structure.
01k,09sep92,gae  documentation tweaks.
01j,30jul92,dnw  Changed to new cacheLib.
                 Removed cache flushes (except of mbufs) since this driver
                 requires write coherent dma caches.
01i,16jul92,rfs  Consolidated init() into attach().
01h,15jul92,rfs  Enabled input hook mechanism.
01g,14jul92,rfs  Sync'ed up to cache library changes.  Required rewriting of
                 method used to obtain shared memory and aligining it to
                 meet the requirements of the device.
01f,29jun92,jwt  added RFS changes to support copy-back caches.
    18jun92,jwt  prototyped 5.1 cache library with SPARC targets.
    13apr92,rfs  Needed to subtract FCS from len of Rx packets.
    17mar92,rfs  Verified cache scheme. Dealt with Tx FIFO underrun.
    10mar92,rfs  Reworked cache coherency scheme and misc sanitizing.
    20feb92,rfs  Receive side was hogging the spl semaphore.  It is now
                 given up, and only taken when needed.
01e,26may92,rrr  the tree shuffle
                 -changed includes to have absolute path from h/
01d,30jan92,rfs  unANSIized for 5.0.x support.
                 Eliminated many routines and re-ordered routines into
                 logical sections.
01c,05nov91,rfs  WRSized, genericized, sanitized, revitalized, ANSIized
01b,29jul91,kjl  clean up in anticipation of beta release
01a,25mar91,kjl  adapted from WRS if_ln.c 01s,20sep90,dab
*/

/*
DESCRIPTION
This module implements the National Semiconductor DP83932 SONIC Ethernet 
network interface driver.

This driver is designed to be moderately generic, operating unmodified
across the range of architectures and targets supported by VxWorks.  
To achieve this, the driver must be given several target-specific
parameters, and some external support routines must be provided.  These
parameters, and the mechanisms used to communicate them to the driver, are
detailed below.  If any of the assumptions stated below are not true for
your particular hardware, this driver will probably not function correctly
with it.  This driver supports up to four individual units per CPU.

BOARD LAYOUT
This device is on-board.  No jumpering diagram is necessary.

EXTERNAL INTERFACE
This driver provides the standard external interface with the following
exceptions.  All initialization is performed within the attach routine;
there is no separate initialization routine.  Therefore, in the global interface
structure, the function pointer to the initialization routine is NULL.

There is one user-callable routine, snattach();
for details, see the manual entry for this routine.

TARGET-SPECIFIC PARAMETERS
.iP "device I/O address"
This parameter is passed to the driver by snattach().
It specifies the base address of the device's I/O register
set.
.iP "interrupt vector"
This parameter is passed to the driver by snattach().
It specifies the interrupt vector to be used by the driver
to service an interrupt from the SONIC device.  The driver will connect
the interrupt handler to this vector by calling intConnect().
.iP "Ethernet address"
This parameter is obtained by calling an external support routine.
It specifies the unique, six-byte address assigned to the VxWorks target
on the Ethernet.
.LP

EXTERNAL SUPPORT REQUIREMENTS
This driver requires five external support functions:
.iP "void sysEnetInit (int unit)" "" 9 -1
This routine performs any target-specific
operations that must be executed before the SONIC
device is initialized.  The driver calls this routine, once per unit, 
from snattach().
.iP "STATUS sysEnetAddrGet (int unit, char *pCopy)"
This routine provides the six-byte Ethernet address used by <unit>.  It
must copy the six-byte address to the space provided by <pCopy>.  This
routine returns OK, or ERROR if it fails.  The driver calls this routine, 
once per unit, from snattach().
.iP "void sysEnetIntEnable (int unit), void sysEnetIntDisable (int unit)"
These routines enable or disable the interrupt from
the SONIC device for the specified <unit>.  Typically,
this involves interrupt controller hardware,
either internal or external to the CPU.  The driver calls these routines 
only during initialization, from snattach().
.iP "void sysEnetIntAck (int unit)"
This routine performs any interrupt acknowledgement or
clearing that may be required.  This typically
involves an operation to some interrupt control
hardware.  The driver calls this routine from the
interrupt handler.
.LP

DEVICE CONFIGURATION
Two global variables, `snDcr' and `snDcr2', are used to set the SONIC
device configuration registers.  By default, the device is programmed in
32-bit mode with zero wait states.  If these values are not suitable,
the `snDcr' and `snDcr2' variables should be modified before calling
snattach().  See the SONIC manual to change these parameters.

SYSTEM RESOURCE USAGE
When implemented, this driver requires the following system resources:
    
    - one interrupt vector
    - 0 bytes in the initialized data section (data)
    - 696 bytes in the uninitialized data section (BSS)

The above data and BSS requirements are for the MC68020 architecture 
and may vary for other architectures.  Code size (text) varies greatly between
architectures and is therefore not quoted here.

This driver uses cacheDmaMalloc() to allocate the memory to be shared with the
SONIC device.  The size requested is 117,188 bytes.

The SONIC device can only be operated if the shared memory region is
write-coherent with the data cache.  The driver cannot maintain cache
coherency for the device for data that is written by the driver
because fields within the shared structures are asynchronously modified by
the driver and the device, and these fields may share the same cache
line.

NOTE 1
The previous transmit descriptor does not exist until the transmitter
has been asked to send at least one packet.  Unfortunately the test
for this condition must be done every time a new descriptor is to be added,
even though the condition is only true the first time.  However, it
is a valuable test, since we should not use the fragment count field
as an index if it is 0.

NOTE 2
There are some things unsupported in this version:

    a) buffer loaning on receive
    b) output hooks
    c) trailer protocol
    d) promiscuous mode

Also, the receive setup needs work so that the number of RRA descriptors
is not fixed at four.  It would be a nice addition to allow all the
sizes of the shared memory structures to be specified by the runtime
functions that call our init routines.

SEE ALSO: ifLib
*/

/*
INTERNAL
This driver contains the conditional compilation switch SN_DEBUG.
If defined, the debug output routines are included.  Output is further
selectable at run-time via the snDebug global variable.
See also the NOTES section at the end of this file.
*/

#include "vxWorks.h"
#include "sys/types.h"
#include "taskLib.h"
#include "iv.h"
#include "memLib.h"
#include "sys/ioctl.h"
#include "etherLib.h"
#include "net/mbuf.h"
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
#include "cacheLib.h"
#include "stdio.h"
#include "intLib.h"
#include "logLib.h"
#include "netLib.h"

/* Tunables needed by device header file */

#define NUM_RRA_DESC  (4)             /* number of RRA descriptors */
#define NUM_RX_DESC   (512)           /* number of Rx descriptors */
#define NUM_TX_DESC   (64)            /* num of Tx desc, must be power of 2 */
#define MAX_TX_FRAGS  (64)            /* max transmit frags to support */

#include "drv/netif/if_sn.h"          /* device description */

/* CONDITIONAL SWITCHES */

/* LOCAL DEFINITIONS */

#define MAX_UNITS   4                   /* max number of units to support */
#define RX_BUF_SIZE (0x2000)            /* size of one receive buffer */
                                        /* total size of receive buffer area */
#define RX_BUF_EXTRA (0x200)		/* Extra space needed to accomodate */
					/* Sonic buffer overrun in Rev. C */
					/* parts - found by Algorithmics  */
#define RBA_SIZE    ((RX_BUF_SIZE + RX_BUF_EXTRA) * NUM_RRA_DESC)

/* Typedefs for external structures that are not typedef'd in their .h files */

typedef struct mbuf MBUF;
typedef struct arpcom IDR;                  /* Interface Data Record wrapper */
typedef struct ifnet IFNET;                 /* real Interface Data Record */
typedef struct sockaddr SOCK;

/* Driver control structure.  One per unit supported. */

typedef struct drv_ctrl
    {
    IDR sn_ac;                          /* Ethernet common part */

    SONIC *pDev;                        /* ptr to the device registers */

    BOOL attached;                      /* indicates unit is attached */
    char *pMem;                         /* ptr to allocated chunk */
    char *pShMem;                       /* ptr to area shared with device */
    unsigned long shMemSize;            /* size of the shared area */
    unsigned long RSA;                  /* Start of Resource Area */
    unsigned long REA;                  /* End of Resource Area */
    unsigned long RBA1;                 /* Start of Receive Buffer Area */
    unsigned long RBA2;                 /* Start of Receive Buffer Area */
    unsigned long RBA3;                 /* Start of Receive Buffer Area */
    unsigned long RBA4;                 /* Start of Receive Buffer Area */
    unsigned long RDA;                  /* Start of Receive Desc Area */
    unsigned long CDA;                  /* Start of CAM Desc Area */
    TX_DESC *pTDA;                      /* Start of Transmit Desc Area */

    TX_DESC *pTXDFree;                  /* ptr to next free TXD */
    TX_DESC *pTXDReclaim;               /* ptr to next reclaimable TXD */
    RX_DESC *pRXDNext;                  /* Next Receive descriptor */
    RX_DESC *pRXDLast;                  /* Last one in link */

    int TxErrs;                         /* transmitter errors */

    FUNCPTR timerIntHandler;
    int	 ticks;
    unsigned int imr;
    } DRV_CTRL;

#define DRV_CTRL_SIZ    sizeof(DRV_CTRL)

/* shorthand for some struct members */

#define sn_if       sn_ac.ac_if             /* network-visible interface */
#define sn_enaddr   sn_ac.ac_enaddr         /* hardware Ethernet address */


/* GLOBALS */

unsigned long snDcr = WAIT0 | DW_32;
unsigned long snDcr2 = 0;

#define SN_DEBUG

#ifdef SN_DEBUG
int snDebug;
#define SN_DEBUG_INIT      (snDebug & 0x01)
#define SN_DEBUG_TX        (snDebug & 0x02)
#define SN_DEBUG_RX        (snDebug & 0x04)
#define SN_DEBUG_EVENT     (snDebug & 0x08)
#define SN_DEBUG_VERBOSE   (snDebug & 0x10)
#define SN_DEBUG_TRACE     (snDebug & 0x20)
#endif

/* External function prototypes not defined in any header files.
 * Some of these are the functions required of the BSP modules.
 */

IMPORT STATUS sysEnetAddrGet ();
IMPORT STATUS sysEnetInit ();
IMPORT STATUS sysEnetIntEnable ();
IMPORT STATUS sysEnetIntDisable ();
IMPORT STATUS sysEnetIntAck ();

extern int    splnet ();
extern int    splimp ();
extern void   splx (int x);

/* LOCALS */

/* an array of driver control struct ptrs, one per unit */
LOCAL DRV_CTRL  drvCtrl [MAX_UNITS];


LOCAL void      snIntr ();
LOCAL void      snTxReclaim ();
LOCAL void      snEventHandler ();
LOCAL BOOL      snRxPktReady ();
LOCAL STATUS    snRxPktProcess ();
#ifdef BSD43_DRIVER
LOCAL int       snOutput ();
#endif
LOCAL int       snIoctl ();
LOCAL void      snReset ();
#ifdef BSD43_DRIVER
LOCAL void      snStartOutput ();
#else
LOCAL void 	snStartOutput (DRV_CTRL *);
#endif    /* BSD43_DRIVER */
LOCAL void      snChipReset ();
LOCAL void      snChipInit ();
LOCAL void      snCamInit ();


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * SECTION: Special routines; attach, ioctl, etc.
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*******************************************************************************
*
* snattach - publish the `sn' network interface and initialize the driver and device
*
* This routine publishes the `sn' interface by filling in a network interface
* record and adding this record to the system list.  It also
* initializes the driver and the device to the operational state.
*
* RETURNS: OK or ERROR.
*/

STATUS snattach
    (
    int    unit,       /* unit number */
    char * pDevRegs,   /* addr of device's regs */
    int    ivec        /* vector number */
    )

    {
    u_long   temp;
    DRV_CTRL *pDrvCtrl;

#ifdef SN_DEBUG
    if (SN_DEBUG_INIT)
        logMsg ("snattach: unit=%d, addr=0x%x, vector=0x%x\n",
                unit, (int)pDevRegs, ivec, 0, 0, 0);
#endif /* SN_DEBUG */

    /* Sanity check the unit number */

    if (unit < 0 || unit >= MAX_UNITS)
        return (ERROR);

    /* Ensure single invocation per system life */

    pDrvCtrl = & drvCtrl [unit];
    if (pDrvCtrl->attached)
        return (OK);

    /* Publish driver existance */

#ifdef BSD43_DRIVER
    ether_attach (&pDrvCtrl->sn_if, unit, "sn", (FUNCPTR) NULL, 
                  (FUNCPTR) snIoctl, (FUNCPTR) snOutput, (FUNCPTR) snReset);
#else
    ether_attach ( 
                 &pDrvCtrl->sn_if,
                 unit,
                 "sn",
                 (FUNCPTR) NULL,
                 (FUNCPTR) snIoctl,
                 (FUNCPTR) ether_output,  /* generic output for Ethernet */
                 (FUNCPTR) snReset
                 );

    pDrvCtrl->sn_if.if_start = (FUNCPTR) snStartOutput;

#endif

    pDrvCtrl->pDev = (SONIC *) pDevRegs;             /* save the device ptr */

    /* get an Ethernet addr in a board specific manner */

    if (sysEnetAddrGet (unit, pDrvCtrl->sn_enaddr) == ERROR)
        return (ERROR);


    {   /***** Establish shared memory region *****/

    /* The driver and the device share areas of memory.  Each area has a
     * specific use.  The device imposes a nasty restriction, in that
     * an area's bounds must lie in the same 64k page.  The entire amount of
     * shared memory that is needed is much less than 64k.  Therefore, we
     * will group all the individual areas into one area, and attempt to
     * ensure that this total area lies in a single 64k page.
     *
     * We must also consider data cache coherency of this shared memory.
     * We therefore use the special system functions that allow us to
     * obtain cache-safe memory, or at least allow us to maintain the
     * coherency of this memory ourselves.
     *
     * The first step is to calculate the size of this shared region.
     */

    pDrvCtrl->shMemSize =
                    CAM_SIZE +      /* the area used for Ethernet addrs */
                    RRA_SIZE +      /* the area that describes Rx buffers */
                    RDA_SIZE +      /* the area that holds Rx descriptors */
                    TDA_SIZE +      /* the area that holds Tx descriptors */
                    RBA_SIZE;       /* the area that holds all Rx buffers */

    /* That was easy.  Now we request some memory from the system.
     * For now, we are taking a simple approach.  We simply request one
     * big region, large enough to ensure that we can position our shared
     * area in a single 64k page.  This approach will waste memory, but
     * blame the board designers who picked the SONIC without realizing
     * that the device has this weird restriction.
     */

    if (!CACHE_DMA_IS_WRITE_COHERENT ())
        return (ERROR);

    pDrvCtrl->pMem = cacheDmaMalloc (pDrvCtrl->shMemSize + 0x00010000);

    if (pDrvCtrl->pMem == NULL)                 /* no memory, so abort */
        return (ERROR);

    /* Find start of 64k page in the region.  This becomes start of our
     * region.
     */

    pDrvCtrl->pShMem = (char *)
                       (((u_long)pDrvCtrl->pMem & 0xffff0000) + 0x00010000);

    } /* Block end */

    { /***** Carve up the shared area into specific areas *****/

    /* Block local variables */
    char *pCur;

    pCur = pDrvCtrl->pShMem;                /* start of shared area */

    pDrvCtrl->CDA = (u_long) pCur;          /* CAM area */
    pCur += CAM_SIZE;                       /* advance the pointer */

    pDrvCtrl->RSA = (u_long) pCur;          /* receive resource table */
    pCur += RRA_SIZE;                       /* advance the pointer */

    pDrvCtrl->REA = (u_long) pCur;          /* save end pointer */
    pDrvCtrl->RDA = (u_long) pCur;          /* receive descriptor area */
    pCur += RDA_SIZE;                       /* advance the pointer */

    pDrvCtrl->pTDA = (TX_DESC *)pCur;       /* transmit descriptor area */
    pCur += TDA_SIZE;                       /* advance the pointer */

    pDrvCtrl->RBA1 = (u_long) pCur;         /* Receive Buffer 1 */
    pCur += RX_BUF_SIZE + RX_BUF_EXTRA;     /* advance the pointer */

    pDrvCtrl->RBA2 = (u_long) pCur;         /* Receive Buffer 2 */
    pCur += RX_BUF_SIZE + RX_BUF_EXTRA;     /* advance the pointer */

    pDrvCtrl->RBA3 = (u_long) pCur;         /* Receive Buffer 3 */
    pCur += RX_BUF_SIZE + RX_BUF_EXTRA;     /* advance the pointer */

    pDrvCtrl->RBA4 = (u_long) pCur;         /* Receive Buffer 4 */

    } /* Block end */

#ifdef SN_DEBUG
    if (SN_DEBUG_INIT)
        {
        /* Dump all the resultant region pointers */
        logMsg ("snattach: pDrvCtrl=%x RRA=%x RDA=%x TDA=%x\n",
                (int)pDrvCtrl, pDrvCtrl->RSA,
                pDrvCtrl->RDA, (int)pDrvCtrl->pTDA, 0, 0);
        logMsg ("snattach: RBA1=%x RBA2=%x RBA3=%x RBA4=%x\n",
                pDrvCtrl->RBA1, pDrvCtrl->RBA2,
		pDrvCtrl->RBA3, pDrvCtrl->RBA4, 0, 0);
        }
#endif /* SN_DEBUG */

    { /***** Perform device initialization *****/

    snChipReset (pDrvCtrl);                 /* reset device */
    sysEnetInit (unit);                     /* do any board specific set up */
    sysEnetIntDisable (unit);               /* board specific int disable */
                                            /* connect interrupt */
    (void) intConnect ((VOIDFUNCPTR *)INUM_TO_IVEC(ivec),snIntr,unit);

    {  /* init the Tx descriptors */
    int loopy;
    TX_DESC *pTXD;

    pTXD = pDrvCtrl->pTDA;                      /* get initial ptr */
    bzero ((char *)pTXD, TDA_SIZE);             /* zero the whole enchilada */

    loopy = NUM_TX_DESC;
    while (loopy--)                             /* loop thru each */
        {
        pTXD->pLink = pTXD + 1;                 /* link to next */
        pTXD++;                                 /* bump to next */
        }

    /* The link field of the last desc is special; it needs to point
     * back to the first desc.  So, we fix it here.
     */

    (--pTXD)->pLink = pDrvCtrl->pTDA;

    pDrvCtrl->pTXDFree = pDrvCtrl->pTDA;              /* initial value */
    pDrvCtrl->pTXDReclaim = pDrvCtrl->pTDA;           /* initial value */
    }

    {  /* init the Rx buffer descriptors */
    RRA_DESC *pRRAD;                            /* RRA working descriptor */

    /* Build the RRA */

    pRRAD = (RRA_DESC *)pDrvCtrl->RSA;           /* get ptr to first entry */

    /* Stuff buffer ptr; least significant 16 bits, then most significant 16 */

    temp = (u_long) CACHE_DMA_VIRT_TO_PHYS (pDrvCtrl->RBA1);
    pRRAD->buff_ptr0 = temp & UMASK;
    pRRAD->buff_ptr1 = temp >> 16;

    /* Stuff word count; least significant 16 bits, then most significant 16 */

    pRRAD->buff_wc0 = RX_BUF_SIZE >> 1 & UMASK;
    pRRAD->buff_wc1 = RX_BUF_SIZE >> 1 >> 16;

    pRRAD++;                                    /* bump to next entry */

    /* Stuff buffer ptr; least significant 16 bits, then most significant 16 */

    temp = (u_long) CACHE_DMA_VIRT_TO_PHYS (pDrvCtrl->RBA2);
    pRRAD->buff_ptr0 = (u_long) temp & UMASK;
    pRRAD->buff_ptr1 = (u_long) temp >> 16;

    /* Stuff word count; least significant 16 bits, then most significant 16 */

    pRRAD->buff_wc0 = RX_BUF_SIZE >> 1 & UMASK;
    pRRAD->buff_wc1 = RX_BUF_SIZE >> 1 >> 16;

    pRRAD++;                                    /* bump to next entry */

    /* Stuff buffer ptr; least significant 16 bits, then most significant 16 */

    temp = (u_long) CACHE_DMA_VIRT_TO_PHYS (pDrvCtrl->RBA3);
    pRRAD->buff_ptr0 = (u_long) temp & UMASK;
    pRRAD->buff_ptr1 = (u_long) temp >> 16;

    /* Stuff word count; least significant 16 bits, then most significant 16 */

    pRRAD->buff_wc0 = RX_BUF_SIZE >> 1 & UMASK;
    pRRAD->buff_wc1 = RX_BUF_SIZE >> 1 >> 16;

    pRRAD++;                                    /* bump to next entry */

    /* Stuff buffer ptr; least significant 16 bits, then most significant 16 */

    temp = (u_long) CACHE_DMA_VIRT_TO_PHYS (pDrvCtrl->RBA4);
    pRRAD->buff_ptr0 = (u_long) temp & UMASK;
    pRRAD->buff_ptr1 = (u_long) temp >> 16;

    /* Stuff word count; least significant 16 bits, then most significant 16 */

    pRRAD->buff_wc0 = RX_BUF_SIZE >> 1 & UMASK;
    pRRAD->buff_wc1 = RX_BUF_SIZE >> 1 >> 16;
    }

    { /* Setup the Rx frame descriptors */
    RX_DESC *pRXD;                              /* Rx working descriptor */

    pRXD = (RX_DESC *) pDrvCtrl->RDA;           /* get initial ptr */
    bzero ((char *)pRXD, RDA_SIZE);             /* zero the whole tomato */
    pDrvCtrl->pRXDNext = pRXD;                  /* start from the start! */
    pDrvCtrl->pRXDLast =                        /* stuff ptr to last */
                    (RX_DESC *) ((u_long)pRXD + RDA_SIZE - RX_DESC_SIZ);

    while (pRXD <= pDrvCtrl->pRXDLast)          /* loop thru each */
        {
        pRXD->in_use = NOT_IN_USE;              /* set NOT_IN_USE */
        pRXD->link =                            /* set link */
            (RX_DESC *) CACHE_DMA_VIRT_TO_PHYS (pRXD + 1);
        pRXD++;                                 /* bump to next */
        }

    /* The link field of the last desc is special; it points nowhere,
     * but must mark the end-of-list.  So, we fix it here.
     */

    pDrvCtrl->pRXDLast->link = RX_EOL;
    }

    /* Enable device interrupts at system level */

    sysEnetIntEnable (unit);

    /* Bring up the device */

    snChipInit (pDrvCtrl);

    } /* Block end */

    /* Set flags */

    pDrvCtrl->attached = TRUE;
    pDrvCtrl->sn_ac.ac_if.if_flags |= (IFF_UP | IFF_RUNNING | IFF_NOTRAILERS);

    return (OK);

    } /* End of snattach() */

/*******************************************************************************
*
* snReset - reset the interface
*
* Mark interface as inactive & reset the chip
* This is one of the routines that can be called from "outside" via
* the interface structure.
* Cannot be called before the attach, since DRV_CTRL ptr would be NULL.
*/

LOCAL void snReset (unit)
    int unit;

    {
    DRV_CTRL *pDrvCtrl;

#ifdef SN_DEBUG
    if (SN_DEBUG_INIT)
        logMsg ("sn: Reset: unit=%d\n", unit, 0, 0, 0, 0, 0);
#endif /* SN_DEBUG */

    pDrvCtrl = & drvCtrl [unit];

    pDrvCtrl->sn_if.if_flags = 0;
    snChipReset (pDrvCtrl);                            /* reset device */
    }

/*******************************************************************************
*
* snChipReset - Place chip in Reset mode
*/

LOCAL void snChipReset (pDrvCtrl)
    DRV_CTRL *pDrvCtrl;

    {
    pDrvCtrl->pDev->cr = RST;
    pDrvCtrl->pDev->rsc = 0;  /* set the sequence counter to zero */
    }

/*******************************************************************************
*
* snChipInit - hardware init of chip (init & start it)
*/

LOCAL void snChipInit (pDrvCtrl)
    DRV_CTRL *pDrvCtrl;

    {
    SONIC *pDev;                            /* ptr to the device regs */
    u_long temp;

    pDev = pDrvCtrl->pDev;

#ifdef SN_DEBUG
    if (SN_DEBUG_INIT)
        logMsg ("sn: Chip init: pDrvCtrl=0x%X\n", (int)pDrvCtrl, 0, 0, 0, 0, 0);
#endif /* SN_DEBUG */

    pDev->cr = RST;                     /* Turn ON RESET */
                                        /* Data Control */
    pDev->dcr = snDcr;
    if (snDcr2)
	pDev->dcr2 = snDcr2;
    pDev->imr = 0;                      /* All Interrupts off */
    pDev->isr = 0x7fff;                 /* Clear ISR */

    temp = (u_long) CACHE_DMA_VIRT_TO_PHYS (pDrvCtrl->RSA);
    pDev->urra = temp >> 16;            /* Upper RSA */
    pDev->rsa = temp & UMASK;           /* Lower RSA */

    temp = (u_long) CACHE_DMA_VIRT_TO_PHYS (pDrvCtrl->REA);
    pDev->rea = temp & UMASK;           /* Lower REA */

    temp = (u_long) CACHE_DMA_VIRT_TO_PHYS (pDrvCtrl->RSA);
    pDev->rrp = temp & UMASK;           /* point to first desc */
    pDev->rwp = temp & UMASK;           /* point to first desc */
    pDev->rsc = 0;

    temp = (u_long) CACHE_DMA_VIRT_TO_PHYS (pDrvCtrl->RDA);
    pDev->urda = temp >> 16;            /* Upper RDA */
    pDev->crda = temp & UMASK;          /* Lower RDA */

    temp = (u_long) CACHE_DMA_VIRT_TO_PHYS (pDrvCtrl->pTDA);
    pDev->utda = temp >> 16;            /* first TXD */
    pDev->ctda = temp & UMASK;

    pDev->cr = RST_OFF;                 /* Turn OFF RESET */
    pDev->cr = RRRA;                    /* prime with RRA read */
    snCamInit (pDrvCtrl);               /* Initialize the CAM */

    pDev->rcr = BRD;

    if (pDev->imr & TCEN)
	{
	pDev->wt0 = pDrvCtrl->ticks & 0xffff;
	pDev->wt1 = pDrvCtrl->ticks >> 16;
	pDev->cr = ST;
	}

    pDev->imr = (pDrvCtrl->imr |= PRXEN); /* Enable these IRQ's */
    pDev->cr = RXEN;                    /* Turn on Receiver */

    }

/*******************************************************************************
*
* snCamInit - put the local Ethernet address in the CAM
*
* First we set up a data area with the enet addr, then we tell the device
* where it is and command the device to read it.  When the device is done,
* it will interrupt us.  We wait for this interrupt with a semaphore that
* will be given by the interrupt handler.
*/

LOCAL void snCamInit (pDrvCtrl)
    DRV_CTRL *pDrvCtrl;

    {
    SONIC *pDev;                         /* ptr to the device regs */
    int jj;
    u_long *CamData;

    CamData = (unsigned long *) pDrvCtrl->CDA;
    pDev = pDrvCtrl->pDev;

    jj = 0;
    CamData [jj++] = 0;                            /* Entry 0 */
    CamData [jj++] = pDrvCtrl->sn_enaddr [1] << 8 | pDrvCtrl->sn_enaddr [0];
    CamData [jj++] = pDrvCtrl->sn_enaddr [3] << 8 | pDrvCtrl->sn_enaddr [2];
    CamData [jj++] = pDrvCtrl->sn_enaddr [5] << 8 | pDrvCtrl->sn_enaddr [4];
    CamData [jj]   = 0x1;                          /* Enable entry 0 */

    pDev->cdc = 1;                                 /* One entry */
    pDev->cdp = (u_long) CACHE_DMA_VIRT_TO_PHYS (pDrvCtrl->CDA) & UMASK;
    pDev->cr = LCAM;                               /* issue "load CAM" cmd */

    /* Wait for operation to complete.  A "dead time" loop here is deemed
     * OK, since we are in initialization phase, and will only do this
     * once.
     */

    while (!(pDev->isr & LCD))
        ;

    pDev->isr = LCD;                        /* clear the event flag */
    }

/*******************************************************************************
*
* snIoctl - the driver I/O control function
*
* Process an ioctl request.
* This is one of the routines that can be called from "outside" via
* the interface structure.
* Cannot be called before the attach, since DRV_CTRL ptr would be NULL.
*/

LOCAL int snIoctl (IDR *pIDR, int cmd, caddr_t data)
    {
    int error;

    error = 0;

#ifdef SN_DEBUG
    if (SN_DEBUG_INIT)
	logMsg ("sn: Ioctl: pIDR=0x%x, cmd=0x%x, data=0x%x\n",
               (int)pIDR, (int)cmd, (int)data, 0, 0, 0);
#endif /* SN_DEBUG */

    switch (cmd)
        {
        case SIOCSIFADDR:
            ((struct arpcom *)pIDR)->ac_ipaddr = IA_SIN (data)->sin_addr;
            arpwhohas (pIDR, &IA_SIN (data)->sin_addr);
            break;

        case SIOCSIFFLAGS:
            /* Flags are set outside this module. No additional work to do. */
            break;

        default:
            error = EINVAL;
        }

    return (error);
    }


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * SECTION: Transmit related routines
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef BSD43_DRIVER

/*******************************************************************************
*
* snOutput - adds new output to the driver's queue and calls output routine
*
* This is the driver output rotuine.  This is one of the routines that is
* called from "outside" via the interface structure, ifnet.
*/

LOCAL int snOutput (pIfnet, pMbuf, pDest)
    IFNET *pIfnet;
    MBUF *pMbuf;
    SOCK *pDest;

    {
    DRV_CTRL *pDrvCtrl;
    int spl;

    pDrvCtrl = & drvCtrl [pIfnet->if_unit];

    spl = splnet ();
    if (IF_QFULL (&pIfnet->if_snd))                /* If the queue is full */
        snStartOutput (pIfnet->if_unit);
    splx (spl);

    return (ether_output (pIfnet, pMbuf, pDest, (FUNCPTR) snStartOutput,
              &pDrvCtrl->sn_ac));
    }

/*******************************************************************************
*
* snStartOutput - start pending output
*
* This routine removes mbuf chains from the driver queue and attempts to
* set up a SONIC transmit descriptor for each chain.  The transmit descriptors
* are fed to the SONIC and subsequently transmitted onto the media.
*/

LOCAL void snStartOutput (unit)
    int unit;

    {
    DRV_CTRL       *pDrvCtrl;
    MBUF           *pMbuf;
    MBUF           *pMbufTemp;
    TX_DESC        *pTXD;
    int            len;
    int            adj_len;
    int            fragIndex;
    int            nUseableFrags;
    char           *m_data;
    u_long	   temp;

    pDrvCtrl = & drvCtrl [unit];

    /* Loop as long as our incoming queue is not empty */

    while (pDrvCtrl->sn_if.if_snd.ifq_head != NULL)
        {

        IF_DEQUEUE (&pDrvCtrl->sn_if.if_snd, pMbuf);

        /* call output hook if any */

        /* etherOutputHookRtn not supported.
         * This would require copying mbufs to contiguous buf.
         *  if ((etherOutputHookRtn != NULL) &&
         *      (* etherOutputHookRtn) (&pDrvCtrl->sn_if, buf, len))
         *      continue;
         */

        /* Calculate length  & number of fragments */

#ifdef SN_DEBUG
	if (SN_DEBUG_TRACE)
	    {
	    static char buf[ETHERMTU];
	    struct ether_header *pPktHdr;        /* ptr to packet header */
	    FAST char *p = buf;
	    FAST struct mbuf *m = pMbuf;
	    for (; m != NULL; p += m->m_len, m = m->m_next)
		bcopy (mtod (m, char *), p, m->m_len);
	    pPktHdr = (struct ether_header *)&buf;
	    logMsg ("sn: Tx: packet type 0x%x from %s",
		    pPktHdr->ether_type,
		    (int)ether_sprintf (&pPktHdr->ether_shost), 0, 0, 0, 0);
	    logMsg (" to %s\n", (int)ether_sprintf (&pPktHdr->ether_dhost), 0, 0, 0, 0, 0);
	    }
#endif	
        pMbufTemp     = pMbuf;
        len           =
        nUseableFrags = 0;

        while (pMbufTemp != NULL)            /* while valid pointer */
            {
            if (pMbufTemp->m_len)        /* only count non-zero length frags */
                {
                len += pMbufTemp->m_len;     /* add length of this fragment */
                nUseableFrags++;             /* bump fragment counter */
                }
            pMbufTemp = pMbufTemp->m_next;   /* advance to next mbuf fragment */
            }

        /* Bail out if too many frags */

        if (nUseableFrags >= MAX_TX_FRAGS)
            {
            logMsg ("sn: Tx: too many mbuf fragments\n",0,0,0,0,0,0);
            m_freem (pMbuf);                      /* packet is thrown away */
            pDrvCtrl->sn_if.if_opackets--;        /* correct the stats */
            pDrvCtrl->sn_if.if_oerrors++;
            continue;
            }

        /* See if next transmit descriptor is available */

        pTXD = pDrvCtrl->pTXDFree;

        if (pTXD->flag)                           /* still in use */
            {
            m_freem (pMbuf);                      /* packet is thrown away */
            pDrvCtrl->sn_if.if_opackets--;        /* correct the stats */
            pDrvCtrl->sn_if.if_oerrors++;
#ifdef SN_DEBUG
            if (SN_DEBUG_TX)
                logMsg ("sn: Tx, no descriptors", 0, 0, 0, 0, 0, 0);
#endif /* SN_DEBUG */
            break;
            }

        /* Mark this descriptor as "in use" */

        pTXD->flag   = TRUE;
        pTXD->status = 0;

	/* remember the mbuf associated with this packet */
	pTXD->pMbuf = pMbuf;

        /* Ensure we send a minimum sized packet */

        adj_len = max (ETHERSMALL, len);

        /* Build a transmit descriptor.
         * Loop thru each fragment in the mbuf chain and stuff our info.
         */

        for (
            fragIndex = 0, pMbufTemp = pMbuf;
            pMbufTemp != NULL;                  /* while valid mbuf pointer */
            pMbufTemp = pMbufTemp->m_next       /* advance to next mbuf frag */
            )
            {

            /* ignore zero-length mbufs */

            if ( pMbufTemp->m_len == 0 )
                continue;

            m_data = mtod(pMbufTemp, char *);   /* get pointer to mbuf data */

            /* ensure data is flushed */

            CACHE_USER_FLUSH (m_data, pMbufTemp->m_len);

#ifdef notdef
	    /* This is what we should be able to do... */
	    temp = (u_long) CACHE_DRV_VIRT_TO_PHYS (&cacheUserFuncs, m_data);
#else
	    temp = (u_long) CACHE_DMA_VIRT_TO_PHYS (m_data);
#endif
            pTXD->frag [fragIndex].frag_ptr0 = (u_long) temp & UMASK;
            pTXD->frag [fragIndex].frag_ptr1 = (u_long) temp >> 16;
            pTXD->frag [fragIndex].frag_size = pMbufTemp->m_len;
            fragIndex++;
            }

        pTXD->pkt_size   = adj_len;                    /* set packet length */
        pTXD->frag_count = fragIndex;                  /* set fragment count */

        if (adj_len != len)                            /* pad pkt size to min */
            pTXD->frag [fragIndex-1].frag_size += (adj_len-len);

        /* copy link field to where device will expect it */

        pTXD->frag [fragIndex].frag_ptr0 =
            (u_long) CACHE_DMA_VIRT_TO_PHYS (pTXD->pLink) | TX_EOL;

#ifdef SN_DEBUG
        if (SN_DEBUG_TX && SN_DEBUG_VERBOSE)
            logMsg ("sn: Tx: pTXD=%x\n", (int)pTXD, 0, 0, 0, 0, 0);
#endif /* SN_DEBUG */

        /* Find previous desc in chain. (see NOTE #1)
         * If prev desc has been used, then get a ptr to the actual
         * link field within the desc, and clear the EOL bit.
         */

        {
        TX_DESC *pPrevTXD;
        u_long *pLink;

        if (pTXD == pDrvCtrl->pTDA)
            pPrevTXD = ((TX_DESC *)((char *)pDrvCtrl->pTDA + TDA_SIZE)) - 1;
        else
            pPrevTXD = pTXD - 1;

        if (pPrevTXD->frag_count)
            {
            pLink = & pPrevTXD->frag [pPrevTXD->frag_count].frag_ptr0;
            *pLink &= ~TX_EOL;
            }
        }

        
        /* start the transmitter */

        pDrvCtrl->pDev->cr = TXP;

        /* And finally, adjust pointer to next TXD. */

        pDrvCtrl->pTXDFree = pTXD->pLink;
        }
    }

#else

/*******************************************************************************
*
* snStartOutput - start pending output
*
* This routine removes mbuf chains from the driver queue and attempts to
* set up a SONIC transmit descriptor for each chain.  The transmit descriptors
* are fed to the SONIC and subsequently transmitted onto the media.
*/

LOCAL void snStartOutput 
    (
    DRV_CTRL * 	pDrvCtrl
    )
    {
    MBUF           *pMbuf;
    MBUF           *pMbufTemp;
    TX_DESC        *pTXD;
    int            len;
    int            adj_len;
    int            fragIndex;
    int            nUseableFrags;
    char           *mbuf_data;
    u_long	   temp;

    /* Loop as long as our incoming queue is not empty */

    while (pDrvCtrl->sn_ac.ac_if.if_snd.ifq_head != NULL)
        {

        IF_DEQUEUE (&pDrvCtrl->sn_if.if_snd, pMbuf);

        /* call output hook if any */

        /* etherOutputHookRtn not supported.
         * This would require copying mbufs to contiguous buf.
         *  if ((etherOutputHookRtn != NULL) &&
         *      (* etherOutputHookRtn) (&pDrvCtrl->sn_if, buf, len))
         *      continue;
         */

        /* Calculate length  & number of fragments */

#ifdef SN_DEBUG
	if (SN_DEBUG_TRACE)
	    {
	    static char buf[ETHERMTU];
	    struct ether_header *pPktHdr;        /* ptr to packet header */
	    FAST char *p = buf;
	    FAST struct mbuf *m = pMbuf;
	    for (; m != NULL; p += m->m_len, m = m->m_next)
		bcopy (mtod (m, char *), p, m->m_len);
	    pPktHdr = (struct ether_header *)&buf;
	    logMsg ("sn: Tx: packet type 0x%x from %s",
		    pPktHdr->ether_type,
		    (int)ether_sprintf (&pPktHdr->ether_shost), 0, 0, 0, 0);
	    logMsg (" to %s\n", (int)ether_sprintf (&pPktHdr->ether_dhost), 0, 0, 0, 0, 0);
	    }
#endif	
        pMbufTemp     = pMbuf;
        len           =
        nUseableFrags = 0;

        while (pMbufTemp != NULL)            /* while valid pointer */
            {
            if (pMbufTemp->m_len)        /* only count non-zero length frags */
                {
                len += pMbufTemp->m_len;     /* add length of this fragment */
                nUseableFrags++;             /* bump fragment counter */
                }
            pMbufTemp = pMbufTemp->m_next;   /* advance to next mbuf fragment */
            }

        /* Pull up the mbuf, if too many frags */

        if (nUseableFrags >= MAX_TX_FRAGS)
            {
             pMbufTemp = pMbuf;
             pMbuf = m_pullup (pMbufTemp, len);
             m_freem (pMbufTemp);
             if (pMbuf == NULL)
                 continue;
            }

        /* See if next transmit descriptor is available */

        pTXD = pDrvCtrl->pTXDFree;

        if (pTXD->flag)                           /* still in use */
            {
            m_freem (pMbuf);                      /* packet is thrown away */
            pDrvCtrl->sn_if.if_oerrors++;
#ifdef SN_DEBUG
            if (SN_DEBUG_TX)
                logMsg ("sn: Tx, no descriptors", 0, 0, 0, 0, 0, 0);
#endif /* SN_DEBUG */
            break;
            }

        /* Mark this descriptor as "in use" */

        pTXD->flag   = TRUE;
        pTXD->status = 0;

	/* remember the mbuf associated with this packet */
	pTXD->pMbuf = pMbuf;

        /* Ensure we send a minimum sized packet */

        adj_len = max (ETHERSMALL, len);

        /* Build a transmit descriptor.
         * Loop thru each fragment in the mbuf chain and stuff our info.
         */

        fragIndex = 0;
        for (pMbufTemp = pMbuf; pMbufTemp != NULL; 
             pMbufTemp = pMbufTemp->m_next)
            {

            /* ignore zero-length mbufs */

            if ( pMbufTemp->m_len == 0 )
                continue;

            mbuf_data = mtod(pMbufTemp, char *); /* get pointer to mbuf data */

            /* ensure data is flushed */

            CACHE_USER_FLUSH (mbuf_data, pMbufTemp->m_len);

#ifdef notdef
	    /* This is what we should be able to do... */
	    temp = (u_long) CACHE_DRV_VIRT_TO_PHYS (&cacheUserFuncs, mbuf_data);
#else
	    temp = (u_long) CACHE_DMA_VIRT_TO_PHYS (mbuf_data);
#endif
            pTXD->frag [fragIndex].frag_ptr0 = (u_long) temp & UMASK;
            pTXD->frag [fragIndex].frag_ptr1 = (u_long) temp >> 16;
            pTXD->frag [fragIndex].frag_size = pMbufTemp->m_len;
            fragIndex++;
            }

        pTXD->pkt_size   = adj_len;                    /* set packet length */
        pTXD->frag_count = fragIndex;                  /* set fragment count */

        if (adj_len != len)                            /* pad pkt size to min */
            pTXD->frag [fragIndex-1].frag_size += (adj_len-len);

        /* copy link field to where device will expect it */

        pTXD->frag [fragIndex].frag_ptr0 =
            (u_long) CACHE_DMA_VIRT_TO_PHYS (pTXD->pLink) | TX_EOL;

#ifdef SN_DEBUG
        if (SN_DEBUG_TX && SN_DEBUG_VERBOSE)
            logMsg ("sn: Tx: pTXD=%x\n", (int)pTXD, 0, 0, 0, 0, 0);
#endif /* SN_DEBUG */

        /* Find previous desc in chain. (see NOTE #1)
         * If prev desc has been used, then get a ptr to the actual
         * link field within the desc, and clear the EOL bit.
         */

        {
        TX_DESC *pPrevTXD;
        u_long *pLink;

        if (pTXD == pDrvCtrl->pTDA)
            pPrevTXD = ((TX_DESC *)((char *)pDrvCtrl->pTDA + TDA_SIZE)) - 1;
        else
            pPrevTXD = pTXD - 1;

        if (pPrevTXD->frag_count)
            {
            pLink = & pPrevTXD->frag [pPrevTXD->frag_count].frag_ptr0;
            *pLink &= ~TX_EOL;
            }
        }

        pDrvCtrl->sn_if.if_opackets++;        /* bump the statistic */

        /* start the transmitter */

        pDrvCtrl->pDev->cr = TXP;

        /* And finally, adjust pointer to next TXD. */

        pDrvCtrl->pTXDFree = pTXD->pLink;
        }
    }

#endif    /* BSD43_DRIVER */

/*******************************************************************************
*
* snTxReclaim - reclaims transmit descriptors that the device has serviced
*
*/

LOCAL void snTxReclaim (pDrvCtrl)
    DRV_CTRL *pDrvCtrl;

    {
    TX_DESC *pTXD;
    int unit;

    unit = pDrvCtrl->sn_if.if_unit;                /* get our unit number */
    pTXD = pDrvCtrl->pTXDReclaim;                  /* get ptr to desc */

    CACHE_DMA_INVALIDATE (&pTXD->status, sizeof (u_long));

    /* The device is deemed to be done with the descriptor if the
     * the descriptor had been flagged as "given" to the device,
     * and the descriptor status field is set.
     */

    while (pTXD->flag && pTXD->status)
        {

        if ( pTXD->status & PTX )
            {

            /* Packet was transmitted successfully.  Extract mbuf ptr and
             * free the mbufs.
             */
            m_freem (pTXD->pMbuf);

            /* done with the desc, so clean it up */

            pTXD->flag    = FALSE;
            pTXD->status  = 0;

#ifdef SN_DEBUG
            if (SN_DEBUG_TX && SN_DEBUG_VERBOSE)
                logMsg ("sn: Tx: reclaimed TXD=%x\n", (int)pTXD, 0, 0, 0, 0, 0);
#endif /* SN_DEBUG */

            }
        else                                    /* ERROR! */
            {
            pDrvCtrl->TxErrs++;                 /* bump our counter */
            pDrvCtrl->pDev->cr = TXP;           /* restart transmitter */
            break;
            }

        /* Advance to the next TXD. */

        pTXD = pTXD->pLink;

        /* Update the reclaim pointer for next time. */

        pDrvCtrl->pTXDReclaim = pTXD;

        /* invalidate the cache for this TXD */

        CACHE_DMA_INVALIDATE (&pTXD->status, sizeof (pTXD->status));
        }
    }


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * SECTION: Receive related routines
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/****************************************************************************
*
* snRxPktReady - indicates a received packet is ready for processing
*/

LOCAL BOOL snRxPktReady (pDrvCtrl)
    DRV_CTRL *pDrvCtrl;

    {
    RX_DESC *pRXD;

    pRXD = pDrvCtrl->pRXDNext;

    CACHE_DMA_INVALIDATE (pRXD, RX_DESC_SIZ);

    if (pRXD->in_use == NOT_IN_USE)              /* if not yet filled */
        return (FALSE);

    return (TRUE);
    }

/*******************************************************************************
*
* snRxPktProcess - process one received packet
*
*/

LOCAL STATUS snRxPktProcess (pDrvCtrl)
    DRV_CTRL *pDrvCtrl;

    {
    RX_DESC *pRXD;                       /* ptr to a packet descriptor */
    struct ether_header *pPktHdr;        /* ptr to packet header */
    u_char *pData;                       /* ptr to packet data */
    MBUF *pMbuf;                         /* ptr to mbuf structs */
    SONIC *pDev;                         /* ptr to the device regs */
    int len;
    int ndx;                                  /* index into RRA desc array */
    int status;                               /* return status */
    u_short ether_type;                       /* safe copy of packet type */
    int spl;


    /* The current RX_DESC will already have been made cache coherent
     * by the snRxPktReady() routine.  There is no need to do it here.
     */

    pRXD    = pDrvCtrl->pRXDNext;               /* get ptr to the desc */
    len     = (pRXD->byte_count & UMASK) - 4;   /* get pkt length - FCS */
    pPktHdr = (struct ether_header *)           /* get ptr to pkt header */
                (((pRXD->pkt_ptr1 & UMASK) << 16) | (pRXD->pkt_ptr0 & UMASK));
    pPktHdr = (struct ether_header *) CACHE_DMA_PHYS_TO_VIRT (pPktHdr);

    /* Do some sanity checks on the RXD info */

    if  (
        (len < 60)                      ||
        (len > 1514)                    ||
        (pPktHdr == NULL)               ||
        (!(pRXD->status & PRX))         ||
        (pRXD->link == RX_EOL)          ||
        (pRXD->link == NULL)
        )
        {
        logMsg ("sn: Fatal error.  Receive structures invalid.\n",0,0,0,0,0,0);
        taskSuspend (0);
        return (ERROR);
        }

    pDrvCtrl->sn_if.if_ipackets++;          /* bump statistic */

                                            /* advance our mgmt variable */
    pDrvCtrl->pRXDNext = (RX_DESC *) CACHE_DMA_PHYS_TO_VIRT (pRXD->link);

#ifdef SN_DEBUG
    if (SN_DEBUG_RX && SN_DEBUG_VERBOSE)
        logMsg ("sn: Rx: packet addr=0x%x, len=0x%x, pRXD=0x%x\n",
                (int)pPktHdr, len, (int)pRXD, 0, 0, 0);
    if (SN_DEBUG_TRACE)
	{
        logMsg ("sn: Rx: packet type 0x%x from %s",
		pPktHdr->ether_type, (int)ether_sprintf (&pPktHdr->ether_shost),
		0, 0, 0, 0);
        logMsg (" to %s\n", (int) ether_sprintf (&pPktHdr->ether_dhost),
		0, 0, 0, 0, 0);
	}
#endif /* SN_DEBUG */

    CACHE_DMA_INVALIDATE (pPktHdr, len);

    /* call input hook if any */

    if (etherInputHookRtn != NULL)
        {
        if ((* etherInputHookRtn) (&pDrvCtrl->sn_if, (char *)pPktHdr, len))
            return (OK);
        }

    /* Subtract size of enet header from packet length. */

    len -= 14;

    /* Set pData to begining of packet data. */

    pData = (u_char *)pPktHdr;
    pData += 14;

    /* save the packet type, since build_cluster modifies the packet header */

    ether_type = pPktHdr->ether_type;

    /* convert packet data into mbufs */

    spl = splnet ();
    pMbuf = copy_to_mbufs (pData, len, 0, (IFNET *) &pDrvCtrl->sn_if);
    splx (spl);

    /* We are now done with the packet descriptor and the packet data.
     * First we manage the packet buffer mechanisms, then we manage
     * the packet descriptor mechanisms.
     */

    { /* done with packet buffer */

    if (pRXD->status & LPKT)                 /* if last pkt in this buffer */
        {

        /* form index to RRA descriptor this packet associates with */

        ndx = (((pRXD->seq_no & 0x0000ff00) >> 8) + 1) % NUM_RRA_DESC;

        pDev = pDrvCtrl->pDev;
        pDev->rwp = (unsigned long)(&((RRA_DESC *)pDrvCtrl->RSA)[ndx]) & UMASK;
        if (pDev->isr & RBE)                /* deal with RBE condition */
            pDev->isr = RBE;                /* clear the RBE condition */
        }
    }

    {  /* done with the desc */

    pRXD->in_use = NOT_IN_USE;                     /* Set NOT_IN_USE */
    pRXD->link = RX_EOL;                           /* Last one */

    /* Link into list */

    pDrvCtrl->pRXDLast->link = (RX_DESC *) CACHE_DMA_VIRT_TO_PHYS (pRXD);

    pDrvCtrl->pRXDLast = pRXD;                /* update our mgmt variable */
    }

    if (pMbuf == NULL)
        {
        logMsg ("sn: out of mbufs\n",0,0,0,0,0,0);
        pDrvCtrl->sn_if.if_ierrors++;           /* bump error statistic */
        status = ERROR;
        }
    else
        { /* give the packet to the appropriate protocol */
        spl = splnet ();

#ifdef BSD43_DRIVER
        do_protocol_with_type (ether_type, pMbuf, &pDrvCtrl->sn_ac, len);
#else
        do_protocol (pPktHdr, pMbuf, &pDrvCtrl->sn_ac, len);
#endif

        splx (spl);
        status = OK;
        }

    return (status);
    }


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * SECTION: Interrupt driven routines, and primary event handler
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*******************************************************************************
*
* snIntr - device interrupt handler
*
* This routine is called at interrupt level in response to an interrupt from
* the device.
*
* Only the "received packet" interrupt event is acknowledged and serviced
* in this routine.  Some of the exception conditions are handled elsewhere.
*/

LOCAL void snIntr (int unit)

    {
    DRV_CTRL *pDrvCtrl;                  /* ptr to driver ctrl struct */
    SONIC *pDev;                         /* ptr to the device regs */

    pDrvCtrl = & drvCtrl [unit];
    pDev = pDrvCtrl->pDev;
    if ((pDev->isr & pDev->imr) & TCEN)
	{
	if (pDrvCtrl->timerIntHandler)
	    {
	    pDev->cr = STP;
	    pDev->isr = TCEN;
	    (*pDrvCtrl->timerIntHandler)();
	    pDev->wt0 = pDrvCtrl->ticks & 0xffff;
	    pDev->wt1 = pDrvCtrl->ticks >> 16;
	    pDev->cr = ST;
	    }
	}

    if ((pDev->isr & pDev->imr) & (PKTRX | TXDN | TXER))
	{
        /* disable further interrupts (except timer and bus retry enable). */

	pDev->imr = (pDrvCtrl->imr &= BREN | TCEN);
        (void) netJobAdd (                   /* queue netTask job */
                         (FUNCPTR)snEventHandler,
                         unit,
                         0,0,0,0
                         );
	}

    /* Handle bus retry interrupt. */

    if (pDev->isr & BR)
        pDev->isr = BR;

    sysEnetIntAck (unit);                /* do system int ack */
    }

/*******************************************************************************
*
* snEventHandler - device event handler, a netTask job
*/

LOCAL void snEventHandler (int unit)

    {
    DRV_CTRL *pDrvCtrl;
    SONIC *pDev;                         /* ptr to the device regs */
    int loops;

#ifdef SN_DEBUG
    if (SN_DEBUG_EVENT)
        logMsg ("sn: event handler enter\n", 0, 0, 0, 0, 0, 0);
#endif /* SN_DEBUG */

    /* Give up the spl semaphore. We don't need it yet, even though the
     * netTask thinks we do.
     */

    splx (0);

    pDrvCtrl = & drvCtrl [unit];
    pDev = pDrvCtrl->pDev;
    loops = 0;

    /* Loop, processing events */

    while ( pDev->isr & (PKTRX | TXDN | TXER) )
        {

        /* Process received packets */

        if (pDev->isr & PKTRX)
            {
            while (snRxPktReady (pDrvCtrl))             /* packets ready? */
                {
                pDev->isr = PKTRX;                /* clear this interrupt */
                snRxPktProcess (pDrvCtrl);
                }
            }

        /* Process transmitter events */

        if ( pDev->isr & (TXDN | TXER) )
            {
            pDev->isr = (TXDN | TXER);            /* clear these interrupts */
            snTxReclaim (pDrvCtrl);
            }
 
        loops++;
        }

    /* Take the spl semaphore before we return to the main loop of the
     * netTask.  This is done because the spl semaphore is assumed to be
     * owned by netTask after this routine returns.
     */

    (void) splnet ();

    /* Enable the interrupts */

    pDev->imr = (pDrvCtrl->imr |= PKTRX | TXDN | TXER);

#ifdef SN_DEBUG
    if (SN_DEBUG_EVENT)
        logMsg ("sn: event handler exit %d\n", loops, 0, 0, 0, 0, 0);
#endif /* SN_DEBUG */

    }

/*******************************************************************************
*
* snClkEnable - enable the SONIC timer interrupt
*
* This routine enables the SONIC timer to give a periodic interrupt.
* The <unit> parameter selects the SONIC device.  The <ticksPerSecond>
* parameter specifies the number of interrupts to generate per second.
* When an interrupt occurs, the function in <intHandler> is called.
*
* NOTE:
* The SONIC clock interrupt is dependent on the SONIC interface being
* configured and enabled.  For this reason, this interrupt is unsuitable
* for use as the main system clock.  It is suitable for use as an auxiliary
* clock source for boards that have no other source of clock.
*
* RETURNS: N/A
*
* SEE ALSO: snClkDisable()
*
* NOMANUAL
*/

void snClkEnable
    (
    int unit,			/* unit number */
    int ticksPerSecond,		/* interrupt frequency */
    FUNCPTR intHandler		/* interrupt handler */
    )
    {
    DRV_CTRL *pDrvCtrl;
    SONIC *pDev;
    int key;

    pDrvCtrl = & drvCtrl [ unit ];
    pDev = pDrvCtrl->pDev;

    key = intLock ();

    pDrvCtrl->timerIntHandler = intHandler;
    pDrvCtrl->ticks = 5000000 / ticksPerSecond;
    pDrvCtrl->imr |= TCEN;

    if (pDev)
	{
	pDev->cr = STP;
	pDev->wt0 = pDrvCtrl->ticks & 0xffff;
	pDev->wt1 = pDrvCtrl->ticks >> 16;
	pDev->imr = pDrvCtrl->imr;
	pDev->cr = ST;
	}
    intUnlock (key);
    }

/*******************************************************************************
*
* snClkDisable - disable the SONIC timer interrupt
*
* This routine disables the SONIC timer interrupt.  The <unit> parameter
* selects the SONIC device.
*
* RETURNS: N/A
*
* SEE ALSO: snClkEnable()
*
* NOMANUAL
*/

void snClkDisable
    (
    int unit			/* unit number */
    )
    {
    DRV_CTRL *pDrvCtrl;
    SONIC *pDev;
    int key;

    pDrvCtrl = & drvCtrl [ unit ];
    pDev = pDrvCtrl->pDev;
    if (pDev)
	{
	key = intLock ();
	pDev->imr = (pDrvCtrl->imr &= ~TCEN);
	pDev->cr = STP;
	intUnlock (key);
	}
    }

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * SECTION: NOTES
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* END OF FILE */
