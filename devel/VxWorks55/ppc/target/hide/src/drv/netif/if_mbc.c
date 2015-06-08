/* if_mbc.c - Motorola 68EN302 network-interface driver */

/* Copyright 1996-1997 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01h,15jul97,spm  removed driver initialization from ioctl support (SPR #8831)
01g,07apr97,spm  code cleanup, corrected statistics, and upgraded to BSD 4.4
01h,06jun97,map  renamed mbcRestartTx to mbcDeviceRestart,
                 code to retart device on rx FIFO overflow problem [SPR# 8670]
01g,07may97,map  corrected rx buffer initialization loop [SPR# 8481]
01f,06nov96,dgp  doc: final formatting
01e,20aug96,map  doc updates
01d,12aug96,map  made mbcIntr global
01c,09aug96,map  clean compiler warnings in mbcattach()
01b,08aug96,map  modified for en302 ethernet
01a,09may96,dzb  derived from v01a of src/drv/netif/if_cpm.c.
*/

/*
DESCRIPTION
This is a driver for the Ethernet controller on the 68EN302 chip.  The device
supports a 16-bit interface, data rates up to 10 Mbps, a dual-ported RAM, and
transparent DMA.  The dual-ported RAM is used for a 64-entry CAM table, and a
128-entry buffer descriptor table.  The CAM table is used to set the Ethernet
address of the Ethernet device or to program multicast addresses.  The buffer
descriptor table is partitioned into fixed-size transmit and receive tables.
The DMA operation is transparent and transfers data between the internal FIFOs
and external buffers pointed to by the receive- and transmit-buffer descriptors
during transmits and receives.

The driver currently supports one Ethernet module controller, but it can be
extended to support multiple controllers when needed. An Ethernet module
is initialized by calling mbcattach().

The driver supports buffer loaning for performance and input/output hook
routines.  It does not support multicast addresses.

The driver requires that the memory used for transmit and receive buffers be
allocated in cache-safe RAM area.  

A glitch in the EN302 Rev 0.1 device causes the Ethernet transmitter to
lock up from time to time. The driver uses a watchdog timer to reset the
Ethernet device when the device runs out of transmit buffers and cannot
recover within 20 clock ticks.

BOARD LAYOUT
This device is on-chip.  No jumpering diagram is necessary.  


EXTERNAL INTERFACE
This driver presents the standard WRS network driver API: first the device
unit must be attached with the mbcattach() routine, then it must be
initialized with the mbcInit() routine.

The only user-callable routine is mbcattach(), which publishes the `mbc'
interface and initializes the driver structures.


TARGET-SPECIFIC PARAMETERS
.iP "Ethernet module base address"
This parameter is passed to the driver via mbcattach().

This parameter is the base address of the Ethernet module. The driver
addresses all other Ethernet device registers as offsets from this address.

.iP "interrupt vector number"
This parameter is passed to the driver via mbcattach().

The driver configures the Ethernet device to use this parameter while
generating interrupt ack cycles.  The interrupt service routine mbcIntr() is
expected to be attached to the corresponding interrupt vector externally,
typically in sysHwInit2().

.iP "number of transmit and receive buffer descriptors"
These parameters are passed to the driver via mbcattach().

The number of transmit and receive buffer descriptors (BDs) used is
configurable by the user while attaching the driver.  Each BD is 8 bytes in
size and resides in the chip's dual-ported memory, while its associated
buffer, 1520 bytes in size, resides in cache-safe conventional RAM. A minimum
of 2 receive and 2 transmit BDs should be allocated.  If this parameter is
NULL, a default of 32 BDs will be used.  The maximum number of BDs depends on
how the dual-ported BD RAM is partitioned.  The 128 BDs in the dual-ported BD
RAM can partitioned into transmit and receive BD regions with 8, 16, 32, or 64
transmit BDs and corresponding 120, 112, 96, or 64 receive BDs.

.iP "Ethernet DMA parameters"
This parameter is passed  to the driver via mbcattach().

This parameter is used to specify the settings of burst limit, water-mark, and
transmit early, which control the Ethernet DMA, and is used to set the EDMA
register.

.iP "base address of the buffer pool"
This parameter is passed to the driver via mbcattach().

This parameter is used to notify the driver that space for the transmit and
receive buffers need not be allocated, but should be taken from a
cache-coherent private memory space provided by the user at the given address.
The user should be aware that memory used for buffers must be 4-byte aligned
and non-cacheable.  All the buffers must fit in the given memory space; no
checking will be performed.  This includes all transmit and receive buffers
(see above) and an additional 16 receive loaner buffers, unless the number of
receive BDs is less than 16, in which case that number of loaner buffers will
be used.  Each buffer is 1520 bytes.  If this parameter is "NONE", space for
buffers will be obtained by calling cacheDmaMalloc() in cpmattach().

.LP


EXTERNAL SUPPORT REQUIREMENTS

The driver requires the following support functions:

.iP "STATUS sysEnetAddrGet (int unit, UINT8 * addr)" "" 9 -1
The driver expects this routine to provide the six-byte Ethernet hardware
address that will be used by <unit>.  This routine must copy the six-byte
address to the space provided by <addr>.  This routine is expected to return
OK on success, or ERROR.  The driver calls this routine, during device
initialization, from the cpmInit() routine.
.LP

SYSTEM RESOURCE USAGE
The driver requires the following system resource:

    - one mutual exclusion semaphore
    - one interrupt vector
    - one watchdog timer
    - 0 bytes in the initialized data section (data)
    - 296 bytes in the uninitialized data section (bss)

The data and BSS sections are quoted for the CPU32 architecture.

If the driver allocates the memory shared with the Ethernet device unit,
it does so by calling the cacheDmaMalloc() routine.  For the default case
of 32 transmit buffers, 32 receive buffers, and 16 loaner buffers, the total
size requested is 121,600 bytes.  If a non-cacheable memory region is provided
by the user, the size of this region should be this amount, unless the
user has specified a different number of transmit or receive BDs.

This driver can only operate if the shared memory region is non-cacheable,
or if the hardware implements bus snooping.  The driver cannot maintain
cache coherency for the device because the buffers are asynchronously
modified by both the driver and the device, and these fields may share the
same cache line.  Additionally, the chip's dual-ported RAM must be
declared as non-cacheable memory where applicable.

SEE ALSO: ifLib,
.I "Motorola MC68EN302 User's Manual",
.I "Motorola MC68EN302 Device Errata, May 30, 1996"

*/

/* includes */

#include "vxWorks.h"
#include "iv.h"
#include "taskLib.h"
#include "memLib.h"
#include "ioctl.h"
#include "etherLib.h"
#include "net/mbuf.h"
#include "net/protosw.h"
#include "socket.h"
#include "errno.h"
#include "net/if.h"
#include "net/unixLib.h"
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
#include "netLib.h"
#include "drv/netif/netifDev.h"
#include "drv/netif/if_mbc.h"
#include "wdLib.h"

/* defines */

#define MAX_UNITS	1	/* max ethernet units */
#define ENET_ADDR_SIZE	0x6	/* size of ethernet src/dest addresses */
#define TX_BD_MIN	2	/* minimum number of Tx buffer descriptors */
#define RX_BD_MIN	2	/* minimum number of Rx buffer descriptors */
#define TX_BD_DEFAULT	0x20	/* default number of Tx buffer descriptors */
#define RX_BD_DEFAULT	0x20	/* default number of Rx buffer descriptors */
#define FRAME_MAX	0x05ee	/* maximum frame size */
#define TX_BUF_SIZE	FRAME_MAX /* size of a transmit buffer */
#define RX_BUF_SIZE	FRAME_MAX /* size of a receive buffer */
#define FRAME_MIN	0x0040	/* minimum frame size */
#define L_POOL		0x10	/* number of Rx loaner buffers in pool */
#define WD_TIMEOUT	20      /* number of clock ticks for reset timeout*/

/* register access macros */

#ifndef MBC_REG_READ
#   define MBC_REG_READ(b, r)		(*(UINT16 *)((b) + (r)))
#endif /* MBC_REG_READ */

#ifndef MBC_REG_WRITE
#   define MBC_REG_WRITE(b, r, v)	(*(UINT16 *)((b) + (r)) = (v))
#endif /* MBC_REG_WRITE */

#define MBC_REG_UPDATE(b, r, v)	MBC_REG_WRITE((b), (r), 		    \
                                              MBC_REG_READ((b), (r)) |(v))
#define MBC_REG_RESET(b, r, v)	MBC_REG_WRITE((b), (r),		            \
                                              MBC_REG_READ((b), (r)) & ~(v))

#define MBC_IEVNT_READ(b)	MBC_REG_READ((b), MBC_IEVNT)
#define MBC_IMASK_READ(b)	MBC_REG_READ((b), MBC_IMASK)
#define MBC_ECNTL_READ(b)	MBC_REG_READ((b), MBC_ECNTL)

#define MBC_INTR_ACK(b, v)	MBC_REG_WRITE((b), MBC_IEVNT, (v))
#define MBC_INTR_DISABLE(b, v)	MBC_REG_RESET((b), MBC_IMASK, (v))
#define MBC_INTR_ENABLE(b, v)	MBC_REG_UPDATE((b), MBC_IMASK, (v))
                                         
/* typedefs */

typedef struct mbuf MBUF;
typedef struct arpcom IDR;	/* Interface Data Record */
typedef struct ifnet IFNET;
typedef struct ifreq IFREQ;
typedef struct sockaddr SOCK;
typedef struct drv_ctrl /* DRV_CTRL */
    {
    IDR		idr;            /* mbc interface data record */
    BOOL	initialized;    /* inidicates that the unit is initialized */
    int		nLoan;          /* number of buffers in the loaner pool */
    UINT8 *	lPool[L_POOL];  /* loaner pool */
    UINT8 	refCnt[L_POOL]; /* loaner buffer reference counters */
    UINT8 *	pRefCnt[L_POOL];/* pointer to the reference counters */
    MBC_DEV	ether;          /* mbc device */
    UINT16	resetCounter;	/* number of times the device was reset */
    WDOG_ID	wdId;           /* watchdog id */
    UINT16	inum;		/* interrupt number */
    UINT16	dmaParms;	/* DMA parameters */
    } DRV_CTRL;


/* globals */

IMPORT STATUS	sysEnetAddrGet (int unit, UINT8 * addr);
IMPORT void	sysMbcDmaParmGet (int unit, UINT16 * pParm);

/* locals */

LOCAL DRV_CTRL drvCtrl[MAX_UNITS]; 

/* forward declarations */

LOCAL STATUS	mbcInit (int uint);
LOCAL int	mbcIoctl (IFNET * pIfNet, int cmd, caddr_t data);
#ifdef BSD43_DRIVER
LOCAL int	mbcOutput (IFNET * pIfNet, MBUF * pMbuf, SOCK * pSock);
#endif
LOCAL void	mbcReset (int uint);
#ifdef BSD43_DRIVER
LOCAL void	mbcStartOutput (int unit);
#else
LOCAL void	mbcStartOutput (DRV_CTRL *pDrvCtrl);
#endif
LOCAL void      mbcHandleInt (DRV_CTRL * pDrvCtrl);
LOCAL void	mbcDeviceRestart (int unit);
LOCAL void	mbcRecv (DRV_CTRL * pDrvCtrl, MBC_BD * pRxBd);
LOCAL void	mbcLoanFree (DRV_CTRL * pDrvCtrl, UINT8 * pRxBuf,
                             UINT8 * pRefCnt);
void		mbcIntr (int unit);


/*******************************************************************************
*
* mbcattach - publish the `mbc' network interface and initialize the driver
*
* The routine publishes the `mbc' interface by adding an `mbc' Interface Data
* Record (IDR) to the global network interface list.
*
* The Ethernet controller uses buffer descriptors from an on-chip dual-ported
* RAM region, while the buffers are allocated in RAM external to the
* controller. The buffer memory pool can be allocated in a non-cacheable RAM
* region and passed as parameter <bufBase>.  Otherwise <bufBase> is NULL and
* the buffer memory pool is allocated by the routine using cacheDmaMalloc().
* The driver uses this buffer pool to allocate the specified number of
* 1518-byte buffers for transmit, receive, and loaner pools.
*
* The parameters <txBdNum> and <rxBdNum> specify the number of buffers
* to allocate for transmit and receive. If either of these parameters
* is NULL, the default value of 2 is used. The number of loaner buffers
* allocated is the lesser of <rxBdNum> and 16.
*
* The on-chip dual ported RAM can only be partitioned so that the maximum
* receive and maximum transmit BDs are:
* .iP
* Transmit BDs:  8, Receive BDs: 120
* .iP
* Transmit BDs: 16, Receive BDs: 112
* .iP
* Transmit BDs: 32, Receive BDs:  96
* .iP
* Transmit BDs: 64, Receive BDs:  64
* .LP
*
* RETURNS: ERROR, if <unit> is out of range> or non-cacheable memory cannot be 
* allocated; otherwise TRUE.
*
* SEE ALSO: ifLib,
* .I "Motorola MC68EN302 User's Manual"
*/

STATUS mbcattach
    (
    int		unit,           /* unit number */
    void *	pEmBase,        /* ethernet module base address */
    int		inum,           /* interrupt vector number */
    int		txBdNum,        /* number of transmit buffer descriptors */
    int		rxBdNum,        /* number of receive buffer descriptors */
    int		dmaParms,	/* DMA parameters */
    UINT8 *	bufBase         /* address of memory pool; NONE = malloc it */
    )
    {
    DRV_CTRL *	pDrvCtrl;
    MBC_BD *	pBd;
    UINT8	bdSize;
    UINT8	txBdMax;
    UINT8	rxBdMax;
    UINT8	nLoan;
    UINT8 *	txBuf;
    UINT8 *	rxBuf;
    UINT8 *	loanBuf;
    int		counter;
    

    /* sanity check - unit number */

    if (unit < 0 || unit >= MAX_UNITS)
        return (ERROR);

    pDrvCtrl = & drvCtrl[unit];

    pDrvCtrl->initialized = FALSE;

    /* Use default values if the parameters are NULL */

    if (txBdNum == NULL)
        txBdNum = TX_BD_DEFAULT;
    if (rxBdNum == NULL)
        rxBdNum = RX_BD_DEFAULT;

    /* Use the default minimum value if the given value is smaller */

    txBdNum = max (TX_BD_MIN, txBdNum);
    rxBdNum = max (RX_BD_MIN, rxBdNum);
    
    /* Ensure that txBdNum, and rxBdNum are within range */
    
    if (txBdNum > 64)
        txBdNum = 64;

    for (bdSize = MBC_EDMA_BDS_8; bdSize <= MBC_EDMA_BDS_64; bdSize++) 
       {
        txBdMax = 1 << (bdSize + 3);   /* set txBdMax to (8, 16, 32, 64) */
        rxBdMax = MBC_BD_MAX - txBdMax;
        
        if (txBdNum <= txBdMax)
            {
            if (rxBdNum > rxBdMax)
                rxBdNum = rxBdMax;
            break;
            }
        }

    /* set the number of receive loaner buffers */

    nLoan  = min (rxBdNum, L_POOL);

    /* fill in the driver control parameters */
    
    pDrvCtrl->nLoan		= nLoan;
    pDrvCtrl->ether.mbcNum	= unit;
    pDrvCtrl->ether.bdSize	= bdSize;
    pDrvCtrl->ether.txBdNum	= txBdNum;
    pDrvCtrl->ether.rxBdNum	= rxBdNum;
    pDrvCtrl->ether.txBdBase	= (MBC_BD *) ((int) pEmBase + MBC_EBD);
    pDrvCtrl->ether.rxBdBase	= (MBC_BD *) ((int) pEmBase + MBC_EBD +
                                   sizeof (MBC_BD) * (1 << (bdSize + 3)));
    pDrvCtrl->ether.txBdNext	= 0;
    pDrvCtrl->ether.rxBdNext	= 0;
    pDrvCtrl->ether.pDevBase	= pEmBase;
    pDrvCtrl->resetCounter	= 0;
    pDrvCtrl->inum		= inum;
    pDrvCtrl->dmaParms		= dmaParms & (MBC_EDMA_BLIM_MSK |
                                              MBC_EDMA_WMRK_MSK |
                                              MBC_EDMA_TSRLY);

    /* create a watchdog timer */

    pDrvCtrl->wdId = wdCreate();

    if (pDrvCtrl->wdId == NULL)
        return (ERROR);

    /* set up the transmit and receive buffers */

    if (bufBase == (UINT8 *) NONE)
        {
        /* allocate the memory pool */

        txBuf = cacheDmaMalloc (txBdNum * TX_BUF_SIZE);
        if (txBuf == NULL)
            {
            wdDelete (pDrvCtrl->wdId);
            return (ERROR);
            }

        rxBuf = cacheDmaMalloc (rxBdNum * RX_BUF_SIZE);
        if (rxBuf == NULL)
            {
            wdDelete (pDrvCtrl->wdId);
            cacheDmaFree (txBuf);
            return (ERROR);
            }

        loanBuf = cacheDmaMalloc (nLoan * RX_BUF_SIZE);
        if (loanBuf == NULL)
            pDrvCtrl->nLoan = nLoan = 0;
        }
    else
        {
        /* partition memory pool for transmit/receive/loaner buffers */

        txBuf = bufBase;
        rxBuf = txBuf + (txBdNum * TX_BUF_SIZE);
        loanBuf = rxBuf + (rxBdNum * RX_BUF_SIZE);
        }

    /* take the device out of reset state */

    MBC_REG_WRITE ((int)pEmBase, MBC_ECNTL, MBC_ECNTL_RES_OFF);

    /* Associate a buffer with each buffer descriptor */

    pBd = pDrvCtrl->ether.txBdBase;
    
    for (counter = 0; counter < txBdNum; counter++, pBd++)
        pBd->dataPointer = txBuf + (counter * TX_BUF_SIZE);

    pBd = pDrvCtrl->ether.rxBdBase;
    for (counter = 0; counter < rxBdNum; counter++, pBd++)
        pBd->dataPointer = rxBuf + (counter * RX_BUF_SIZE);

    /* Initialize the loaner buffer pool */
    
    for (counter = 0; counter < nLoan; counter++)
        {
        pDrvCtrl->lPool[counter] = loanBuf + (counter * RX_BUF_SIZE);
        pDrvCtrl->refCnt[counter] = 0;
        pDrvCtrl->pRefCnt[counter] = & pDrvCtrl->refCnt[counter];
        }

    /* reset the chip */
    
    mbcReset (unit);

    /* publish the Interface Data Record */

#ifdef BSD43_DRIVER    
    ether_attach (& pDrvCtrl->idr.ac_if, unit, MBC_IFNAME,
                  (FUNCPTR) mbcInit, (FUNCPTR) mbcIoctl,
                  (FUNCPTR) mbcOutput, (FUNCPTR) mbcReset);
#else
    ether_attach (
                 &pDrvCtrl->idr.ac_if, 
                 unit,
                 MBC_IFNAME,
                 (FUNCPTR) mbcInit, 
                 (FUNCPTR) mbcIoctl,
                 (FUNCPTR) ether_output, 
                 (FUNCPTR) mbcReset
                 );
    pDrvCtrl->idr.ac_if.if_start = (FUNCPTR)mbcStartOutput;
#endif

    mbcInit (unit);

    return (OK);
    }

/*******************************************************************************
*
* mbcInit - initialize the MBC network device
*
* This routine marks the interface as down, then configures and initializes
* the chip.  It sets up the transmit and receive buffer descriptor (BD)
* rings, initializes registers, and enables interrupts.  Finally, it marks
* the interface as up and running.  It does not touch the receive loaner
* buffer stack.
*
* This routine is called from the driver attach routine.
*
* The complement of this routine is mbcReset().  Once a unit is reset by
* mbcReset(), it may be re-initialized to a running state by this routine.
*
* RETURNS: OK if successful, otherwise ERROR.
*
* SEE ALSO: mbcIoctl(), mbcReset()
*
*/

LOCAL STATUS mbcInit
    (
    int			unit           /* unit number */
    )
    {
    DRV_CTRL *		pDrvCtrl;
    MBC_BD *		pBd;
    volatile char *	pDevBase;
    UINT8		enetAddr[ENET_ADDR_SIZE];
    UINT16		regValue;
    int			counter;

    
    /* sanity check the unit number */
    
    if (unit < 0 || unit >= MAX_UNITS)
        return (ERROR);

    pDrvCtrl = & drvCtrl[unit];

    /* ensure single invocation */
    
    if (pDrvCtrl->initialized)
        return (OK);

    pDevBase = pDrvCtrl->ether.pDevBase;

    /* mark the device as down */

    pDrvCtrl->idr.ac_if.if_flags &= ~(IFF_UP | IFF_RUNNING);

    /* take the device out of reset state */

    MBC_REG_WRITE (pDevBase, MBC_ECNTL, MBC_ECNTL_RES_OFF);

    /* set up transmit buffer descriptors */
    
    pDrvCtrl->ether.txBdNext = 0;
    pBd = pDrvCtrl->ether.txBdBase;

    for (counter = pDrvCtrl->ether.txBdNum; counter; counter--, pBd++)
        pBd->statusMode = (MBC_TXBD_I | MBC_TXBD_L | MBC_TXBD_TC);

    /* turn on the `wrap' bit on the last buffer descriptor */

    pBd--;
    pBd->statusMode |= MBC_TXBD_W;


    /* set up the receive buffer descriptors */

    pDrvCtrl->ether.rxBdNext = 0;
    pBd = pDrvCtrl->ether.rxBdBase;

    for (counter = pDrvCtrl->ether.rxBdNum; counter; counter--, pBd++)
        pBd->statusMode = (MBC_RXBD_E | MBC_RXBD_I);

    /* turn on the `wrap' bit on the last buffer descriptor */
    
    pBd--;
    pBd->statusMode |= MBC_RXBD_W;

    /* set the interrupt vector number */

    MBC_REG_WRITE (pDevBase, MBC_IVEC, pDrvCtrl->inum);

    /* set BD size and other DMA parameters */

    regValue = pDrvCtrl->dmaParms;

    regValue |= (pDrvCtrl->ether.bdSize << MBC_EDMA_BDS_SHFT);

    MBC_REG_WRITE (pDevBase, MBC_EDMA, regValue);

    /*
     * set the operating mode to, no internal loopback, no full duplex, and
     * no hearbeat control.
     */
    
    MBC_REG_WRITE (pDevBase, MBC_ECFG, 0);

    /*
     * set address control to, no hash enable, no index enable, no multicast,
     * no physical address rejection, and promiscuous mode, if needed.
     */
    
    regValue = 0;
    if (pDrvCtrl->idr.ac_if.if_flags & IFF_PROMISC)
        regValue |= MBC_AR_PROM;
    
    MBC_REG_WRITE (pDevBase, MBC_AR, regValue);

    /* set max receive frame size */

    MBC_REG_WRITE (pDevBase, MBC_EMRBLR, FRAME_MAX);
        
    /* get ethernet address for this device */
    
    if (sysEnetAddrGet (unit, (UINT8 *) enetAddr) == ERROR)
        return (ERROR);

    /* copy in correct byte sequence */
    
    for (counter=0; counter < ENET_ADDR_SIZE; counter++)
        pDrvCtrl->idr.ac_enaddr[ENET_ADDR_SIZE - counter - 1] =
            enetAddr[counter];

    /* install the ethernet address */
    
    bcopy ((char *) pDrvCtrl->idr.ac_enaddr,
           (char *) (pDrvCtrl->ether.pDevBase + MBC_ARTAB),
           ENET_ADDR_SIZE);
    
    /* set rest of the AR entries to broadcast address */

    regValue=0xffff;
    for (counter = 1; counter < MBC_ARTAB_SIZE; counter++)
        {
        MBC_REG_WRITE (pDevBase, MBC_ARTAB + (counter * 8), regValue);
        MBC_REG_WRITE (pDevBase, MBC_ARTAB + (counter * 8) + 2, regValue);
        MBC_REG_WRITE (pDevBase, MBC_ARTAB + (counter * 8) + 4, regValue);
        }
    
    /* enable interrupts - clear events and set mask */

    MBC_REG_WRITE (pDevBase, MBC_IEVNT, 0xffff);
    MBC_REG_WRITE (pDevBase, MBC_IMASK, MBC_IMASK_RXF | MBC_IMASK_EBE);

    /* set interface flags - mark device as `up' */

    pDrvCtrl->idr.ac_if.if_flags |= IFF_UP | IFF_RUNNING | IFF_NOTRAILERS;
    
    /* mark device as initialized */

    pDrvCtrl->initialized = TRUE;

    /* enable the device */

    MBC_REG_UPDATE (pDevBase, MBC_ECNTL, MBC_ECNTL_ENBL);
    
    /* start sending enqueued packets */

#ifdef BSD43_DRIVER
    (void) netJobAdd ((FUNCPTR) mbcStartOutput, unit, 0, 0, 0, 0); 
#else
    (void) netJobAdd ((FUNCPTR) mbcStartOutput, (int)pDrvCtrl, 0, 0, 0, 0); 
#endif

    return (OK);
    }

#ifdef BSD43_DRIVER
/*******************************************************************************
*
* mbcOutput - network interface output routine
*
* This routine simply calls ether_output(). ether_output() resolves 
* the hardware addresses and calls mbcStartOutput() with the unit number
* passed as an argument.
*
* RETURNS: OK if successful, otherwise errno.
*/

LOCAL int mbcOutput
    (
    IFNET *	pIfNet,		/* pointer to IFNET structure */
    MBUF *	pMbuf,		/* mbuf chain for output */
    SOCK *	pSock		/* sock ptr for destination */  
    )
    {
    DRV_CTRL * pDrvCtrl = & drvCtrl[pIfNet->if_unit];

    return (ether_output (pIfNet, pMbuf, pSock, (FUNCPTR) mbcStartOutput,
	    & pDrvCtrl->idr));
    }
#endif

/*******************************************************************************
*
* mbcStartOutput - output packet to network interface device
*
* mbcStartOutput() takes a packet from the network interface output queue,
* copies the mbuf chain into an interface buffer, and sends the packet over
* the interface.  etherOutputHookRtns are supported.
*
* Collision stats are collected in this routine from previously sent BDs.
* These BDs will not be examined until after the transmitter has cycled the
* ring, coming upon the BD after it has been sent. Thus, collision stat
* collection will be delayed a full cycle through the Tx ring.
*
* This routine is called under several possible scenarios.  Each one will be
* described below.
*
* The first, and most common, is when a user task requests the transmission of
* data. Under BSD 4.3, this results in a call to mbcOutput(), which in turn
* calls ether_output(). The routine, ether_output(), will make a call to
* mbcStartOutput() if our interface output queue is not full, otherwise, the
* outgoing data is discarded. BSD 4.4 uses a slightly different model, in
* which the generic ether_output() routine is called directly, followed by
* a call to this routine.
*
* The second scenario is when this routine, while executing runs out of free
* Tx BDs, turns on transmit interrupts and exits.  When the next BD is
* transmitted, an interrupt occurs and the ISR does a netJobAdd of the routine
* which executes in the context of netTask() and continues sending packets
* from the interface output queue.  
*
* The third scenario is when the device is reset, typically when the 
* promiscuous mode is altered; which results in a call to mbcInit().  This 
* resets the device, does a netJobAdd() of this routine to enable transmitting 
* queued packets.
*
* RETURNS: N/A
*/

#ifdef BSD43_DRIVER
LOCAL void mbcStartOutput
    (
    int		unit		/* unit number */
    )
    {
    DRV_CTRL *		pDrvCtrl = & drvCtrl[unit];
#else
LOCAL void mbcStartOutput
    (
    DRV_CTRL * 	pDrvCtrl 	/* driver control structure */
    )
    {
    int unit = pDrvCtrl->idr.ac_if.if_unit;
#endif
    volatile char *	pDevBase = pDrvCtrl->ether.pDevBase;
    MBUF *		pMbuf;		/* mbuf pointer */
    int			length;
    MBC_BD *		pTxBd;
    UINT8 *		pad;
    UINT8		txBdMax = pDrvCtrl->ether.txBdNum - 2;


    if (txBdMax == 0)
        txBdMax++;

    /*
     * Loop until:
     *     a) there are no more packets ready to send
     *     b) we have insufficient resources left to send another one
     *     c) txBdMax packets have been sent, so that the
     *        transmitter on the rev 0.1 chip is not choked.
     */

    while (pDrvCtrl->idr.ac_if.if_snd.ifq_head != NULL)
        {
	pTxBd = & pDrvCtrl->ether.txBdBase[pDrvCtrl->ether.txBdNext];
        
	/* check if a transmit buffer descriptor is available */
        
        if ((pTxBd->statusMode & MBC_TXBD_R) || (txBdMax == 0))
	    {
	    /* if no BD available, enable interrupt to see when one is free */

            wdStart (pDrvCtrl->wdId, WD_TIMEOUT, (FUNCPTR) mbcDeviceRestart,
                     unit);

            MBC_INTR_ENABLE (pDevBase, MBC_IEVNT_TXF);

	    break;
	    }

        IF_DEQUEUE (& pDrvCtrl->idr.ac_if.if_snd, pMbuf); 

        copy_from_mbufs (pTxBd->dataPointer, pMbuf, length);

        /* initial hardware version does not support padding */

        if (length < FRAME_MIN)
            {
            pad = pTxBd->dataPointer + length;
            for (; length != FRAME_MIN; length++, pad++)
                *pad = 0x88;
            }

        /* call hook if one is connected */

        if ((etherOutputHookRtn == NULL) || ((* etherOutputHookRtn)
	    (& pDrvCtrl->idr.ac_if, pTxBd->dataPointer, length) == 0))
            {
	    /* add in collision stats from previously sent BDs */

            pDrvCtrl->idr.ac_if.if_collisions += ((pTxBd->statusMode &
                                                   MBC_TXBD_RC) >>
                                                  MBC_TXBD_RC_SHFT);

            /* add in transmission failures */
            
            if (pTxBd->statusMode & MBC_TXBD_ERRS)
                {
                pDrvCtrl->idr.ac_if.if_oerrors++;
                pDrvCtrl->idr.ac_if.if_opackets--;
                }

	    /* since hook did not grab the frame, send packet */

            pTxBd->dataLength = length;
            pTxBd->statusMode &= ~(MBC_TXBD_ERRS|MBC_TXBD_RC);
            pTxBd->statusMode |= MBC_TXBD_R;

            /* incr BD count */

	    pDrvCtrl->ether.txBdNext = (pDrvCtrl->ether.txBdNext + 1) %
				        pDrvCtrl->ether.txBdNum;

#ifndef BSD43_DRIVER    /* BSD 4.4 ether_output() doesn't bump statistic. */
            pDrvCtrl->idr.ac_if.if_opackets++;
#endif
            }
        txBdMax --;
        }
    }

/*******************************************************************************
*
* mbcIoctl - network interface control routine
*
* This routine implements the network interface control functions.
* It handles SIOCSIFADDR and SIOCSIFFLAGS commands.
*
* RETURNS: OK if successful, otherwise EINVAL.
*/

LOCAL int mbcIoctl
    (
    IFNET *	pIfNet,		/* pointer to IFNET structure */
    int		cmd,		/* command to process */
    caddr_t	data		/* pointer to data */ 
    )
    {
    DRV_CTRL *	pDrvCtrl = & drvCtrl[pIfNet->if_unit];
    int         error   = 0;            /* error value */
    int         s       = splimp ();    /* sem not taken in calling routines */
    BOOL	promiscReq;		/* requested promisc state */
    BOOL	promiscCur;     	/* current promisc state */

    switch (cmd)
        {
        case SIOCSIFADDR:       /* set interface address */
            ((struct arpcom *)pIfNet)->ac_ipaddr = IA_SIN (data)->sin_addr;
            arpwhohas (pIfNet, &IA_SIN (data)->sin_addr);
            break;
    
        case SIOCSIFFLAGS:
            /* process request, if promisc mode needs change */

            promiscReq = ((pDrvCtrl->idr.ac_if.if_flags & IFF_PROMISC) ?
                          TRUE : FALSE);
            promiscCur = ((MBC_REG_READ (pDrvCtrl->ether.pDevBase, MBC_AR) &
                           MBC_AR_PROM) ? TRUE : FALSE);

            if ((promiscReq && (!promiscCur)) || ((!promiscReq) && promiscCur))
                {
                mbcDeviceRestart (pIfNet->if_unit);
                }

            break;

        default:
            error = EINVAL;
        }

    splx (s);
    return (error);
    }


/*******************************************************************************
*
* mbcReset - network interface reset routine
*
* This routine marks the interface as down and resets the device.  This
* includes disabling interrupts, stopping the transmitter and receiver.
*
* The complement of this rotuine is mbcInit().  Once a unit is reset by this
* routine, it can be re-initialized to a  running state by mbcInit().
*
* RETURNS: N/A
*/

LOCAL void mbcReset
    (
    int		unit		/* unit number */
    )
    {
    DRV_CTRL * pDrvCtrl;
    int counter = 0xffff;

    if (unit < 0 || unit >= MAX_UNITS)	/* sanity check the unit number */
        return;
		  
    pDrvCtrl = & drvCtrl[unit];

    /* Disable interrupts */
    
    MBC_REG_WRITE (pDrvCtrl->ether.pDevBase, MBC_IMASK, 0x0);

    /* clear pending `graceful stop complete' event, and start one */

    MBC_REG_WRITE (pDrvCtrl->ether.pDevBase, MBC_IEVNT, MBC_IEVNT_GRA);
    MBC_REG_UPDATE (pDrvCtrl->ether.pDevBase, MBC_ECNTL, MBC_ECNTL_GTS);

    /* wait for graceful stop to register */
    
    while ((counter--) &&
           (! (MBC_IEVNT_READ (pDrvCtrl->ether.pDevBase) & MBC_IEVNT_GRA)));

    /* disable the receiver and transmitter. */

    MBC_REG_RESET (pDrvCtrl->ether.pDevBase, MBC_ECNTL, MBC_ECNTL_ENBL);

    /* mark the driver as down */

    pDrvCtrl->idr.ac_if.if_flags &= ~(IFF_UP | IFF_RUNNING);

    /* mark the driver as no longer initialized */

    pDrvCtrl->initialized = FALSE;
    pDrvCtrl->resetCounter++;
    
    }


/*******************************************************************************
*
* mbcIntr - network interface interrupt handler
*
* This routine is called at interrupt level. It handles work that 
* requires minimal processing. Interrupt processing that is more 
* extensive gets handled at task level. The network task, netTask(), is 
* provided for this function. Routines get added to the netTask() work 
* queue via the netJobAdd() command.
*
* RETURNS: N/A
*/

void mbcIntr
    (
    int		unit		/* unit number */
    )
    {
    DRV_CTRL * 		pDrvCtrl = & drvCtrl[unit];
    volatile char *	pDevBase = pDrvCtrl->ether.pDevBase;
    int 		events;

    /* identify and acknowledge all interrupt events */

    events = (MBC_IEVNT_READ (pDevBase) &
              (MBC_IMASK_READ (pDevBase) | MBC_IMASK_BSY));
    
    MBC_INTR_ACK (pDevBase, events);

    /* check for spurious interrupt -> driver initialized ? */

    if (! pDrvCtrl->initialized)
	{
	MBC_INTR_ACK (pDevBase, 0xffff);

	return;
	}

    /* handle receive events */

    if (events & MBC_IEVNT_RXF)
	{
        (void) netJobAdd ((FUNCPTR) mbcHandleInt, (int) pDrvCtrl, 0,0,0,0); 

	/* turn off receive interrupts - mbcHandleInt turns back on */

	MBC_INTR_DISABLE (pDevBase, MBC_IEVNT_RXF);
        }

    /* handle transmitter events - BD full condition -> ever happen ? */

    if (events & MBC_IEVNT_TXF)
	{

        wdCancel (pDrvCtrl->wdId);

#ifdef BSD43_DRIVER        
        (void) netJobAdd ((FUNCPTR) mbcStartOutput, unit, 0, 0, 0, 0); 
#else
        (void) netJobAdd ((FUNCPTR) mbcStartOutput, (int)pDrvCtrl, 0, 0, 0, 0); 
#endif

	MBC_INTR_DISABLE (pDevBase, MBC_IEVNT_TXF);
	}

    /*
     * check for input busy condition, we don't enable this interrupt
     * but we check for it with each interrupt.
     */

    if (events & MBC_IEVNT_BSY)
	{
        /* count discarded frames as errors */

        pDrvCtrl->idr.ac_if.if_ierrors += 1;
	}

    /* restart the transmitter on a ethernet bus error */
    
    if (events & MBC_IEVNT_EBE)
        {
        (void) netJobAdd ((FUNCPTR) mbcDeviceRestart, unit, 0,0,0,0);
        
        
        MBC_INTR_ACK (pDevBase, MBC_IEVNT_EBE);
        }

    /* ignore and reset all other events */

    MBC_INTR_ACK (pDevBase, ~(MBC_IMASK_READ(pDevBase)));

    return;
    }


/*******************************************************************************
*
* mbcHandleInt - task-level interrupt handler
*
* This is the task-level interrupt handler, which is called from 
* netTask(). mbcHandleInt() gets input frames from the device and then calls 
* mbcRecv() to process each frame.  mbcRecv() only gets called if no error
* stats were reported in the buffer descriptor.  Data chaining is not
* supported.
*
* This routine should be called with MBC receive interrupts masked so that
* more netJobAdds of this routine are not performed by mbcIntr().
* Receive interrupts are turned back on by this routine before exiting.
*
* RETURNS: N/A
*/

LOCAL void mbcHandleInt
    (
    DRV_CTRL *	pDrvCtrl	/* pointer to DRV_CTRL structure */
    )
    {
    volatile char *  pDevBase = pDrvCtrl->ether.pDevBase;
    MBC_BD *	     pRxBd;

    pRxBd = & pDrvCtrl->ether.rxBdBase[pDrvCtrl->ether.rxBdNext];
    
    while (!(pRxBd->statusMode & MBC_RXBD_E))
        {
        /* data chaining is not supported - check all error conditions */

        if (((pRxBd->statusMode & (MBC_RXBD_F  | MBC_RXBD_L))
	                       == (MBC_RXBD_F  | MBC_RXBD_L)) &&
            !(pRxBd->statusMode & (MBC_RXBD_CL | MBC_RXBD_OV |
                                   MBC_RXBD_CR | MBC_RXBD_SH |
                                   MBC_RXBD_NO | MBC_RXBD_LG)))
            mbcRecv (pDrvCtrl, pRxBd);
        else
            {
            pDrvCtrl->idr.ac_if.if_ierrors++;

            /*
             * Fix for errata #9 -- Device Errata, Feb 20, 1997.
             * Ethernet frames may be incorrectly received after a rx FIFO
             * overflow.
             */
            
            if ((pRxBd->statusMode & MBC_RXBD_L) &&
                ! (pRxBd->statusMode & MBC_RXBD_F))
                {
                mbcDeviceRestart (pDrvCtrl->ether.mbcNum);
                return;
                }
            }


        /* reset buffer descriptor as empty */

        pRxBd->statusMode |= MBC_RXBD_E;
        
        /* incr BD count */

        pDrvCtrl->ether.rxBdNext = (pDrvCtrl->ether.rxBdNext + 1) %
				    pDrvCtrl->ether.rxBdNum;

        pRxBd = & pDrvCtrl->ether.rxBdBase[pDrvCtrl->ether.rxBdNext];

        /* clear Rx events */

	MBC_INTR_ACK (pDevBase, MBC_IEVNT_RXF);
        }

    /* re-enable Rx interrupts */

    MBC_INTR_ENABLE (pDevBase, MBC_IEVNT_RXF);
    }

/*******************************************************************************
*
* mbcRecv - process an input frame
*
* This routine processes an input frame, then passes it up to the higher 
* level in a form it expects.  Buffer loaning, promiscuous mode, and
* etherInputHookRtns are all supported.  Trailer protocols is not supported.
*
* RETURNS: N/A
*/

LOCAL void mbcRecv
    (
    DRV_CTRL *	pDrvCtrl,	/* pointer to DRV_CTRL structure */
    MBC_BD *	pRxBd		/* receive buffer descriptor */
    )
    { 
    MBUF *	pMbuf = NULL;	/* MBUF pointer */
    UINT8 *	pData;
    int		length;
#ifdef BSD43_DRIVER
    UINT16	type;
#else
    struct ether_header * 	pEh;
#endif

    /* bump input packet counter */

    pDrvCtrl->idr.ac_if.if_ipackets++;

    /* call hook if one is connected */

    if ((etherInputHookRtn != NULL) && ((*etherInputHookRtn)
	(& pDrvCtrl->idr.ac_if, pRxBd->dataPointer, pRxBd->dataLength) != 0))
        return; 

    /* check the M bit, to filter frames not destined for this station. */

    if (pRxBd->statusMode & MBC_RXBD_M)
        return;

    /* adjust length to data only */

    length = pRxBd->dataLength - SIZEOF_ETHERHEADER;

    if (length <= 0)
	{ 
        pDrvCtrl->idr.ac_if.if_ierrors++;
	return;
	}

    /* point to data */

    pData = pRxBd->dataPointer + SIZEOF_ETHERHEADER;

#ifdef BSD43_DRIVER
    /* save type - build_cluster trashes the type field */

    type = ((ETH_HDR *) pRxBd->dataPointer)->ether_type;
#else
    /* BSD 4.4 uses Ethernet header */
   
    pEh = (struct ether_header *) pRxBd->dataPointer;
#endif

    /* OK to loan out buffer ? -> build a mbuf cluster */

    if ((pDrvCtrl->nLoan > 0) && (USE_CLUSTER (length)))
	pMbuf = build_cluster (pData, length, & pDrvCtrl->idr, MC_MBC,
	    pDrvCtrl->pRefCnt[(pDrvCtrl->nLoan - 1)], mbcLoanFree,
	    (int) pDrvCtrl, (int) pRxBd->dataPointer,
	    (int) pDrvCtrl->pRefCnt[(pDrvCtrl->nLoan - 1)]);

    /* if buffer was successfully turned into mbuf cluster */

    if (pMbuf != NULL)
	pRxBd->dataPointer = pDrvCtrl->lPool[--pDrvCtrl->nLoan];
    else
        {
	/* else do same ol' copy to mbuf */

        pMbuf = copy_to_mbufs (pData, length, 0, & pDrvCtrl->idr.ac_if);

        if (pMbuf == NULL)
	    {
            pDrvCtrl->idr.ac_if.if_ierrors++;
            return;
	    }
        }

    /* send up to protocol */

#ifdef BSD43_DRIVER
    do_protocol_with_type (type, pMbuf, & pDrvCtrl->idr, length);
#else
    do_protocol (pEh, pMbuf, & pDrvCtrl->idr, length);
#endif
    }

/*******************************************************************************
*
* mbcDeviceRestart - restart transmits
*
* This routine re-resets the Ethernet device.  It is called by the watchdog
* timer which expires when transmit BDs are all full, and a transmit frame
* interrupt does not occur with in 20 clock ticks.  Since on a transmit lock
* up, the transmitter can be restarted only by resetting the device, this
* routine resets, and reinitializes the device.  The device reset counter will
* be updated on each reset, and can be examined with mbcShow().
*
* RETURNS: N/A
*
* SEE ALSO: mbcIntr(), mbcShow(), mbcStartOutput()
*/

LOCAL void mbcDeviceRestart
    (
    int 	unit
    )
    {
    mbcReset (unit);
    mbcInit (unit);
    }

/*******************************************************************************
*
* mbcLoanFree - return the given buffer to loaner pool
*
* This routine returns <pRxBuf> to the pool of available loaner buffers.
* It also returns <pRef> to the pool of available loaner reference counters,
* then zeroes the reference count.
*
* RETURNS: N/A
*/

LOCAL void mbcLoanFree
    (
    DRV_CTRL *	pDrvCtrl,
    UINT8 *	pRxBuf,
    UINT8 *	pRefCnt
    )
    {
    /* return loaned buffer to pool */

    pDrvCtrl->lPool[pDrvCtrl->nLoan] = pRxBuf;

    /* return loaned reference count to pool */

    pDrvCtrl->pRefCnt[pDrvCtrl->nLoan++] = pRefCnt;

    /* reset reference count - should have been done from above, but... */

    *pRefCnt = 0;
    }

/*******************************************************************************
*
* mbcShow - displays the internal state of an Ethernet unit
*
* This routine prints the following information about the device unit,
* .iP
* - txBdNum, the number of transmit buffer descriptors 
* .iP
* - rxBdNum, the number of receive buffer descriptors
* .iP
* - txBdNext, the next transmit buffer descriptor index
* .iP
* - rxBdNext, the next receive buffer descriptor index
* .iP
* - all the Ethernet configuration registers
* .LP
*
* RETURNS: N/A.
*
* NOMANUAL
*/

void mbcShow
    (
    int unit                    /* unit number */
    )
    {
    DRV_CTRL *		pDrvCtrl;
    volatile char *	pDevBase;
 
    
    if (unit < 0 || unit >= MAX_UNITS)
        return;

    pDrvCtrl = & drvCtrl[unit];
    pDevBase = pDrvCtrl->ether.pDevBase;

    printf ("mbc (unit %d):\n", unit);

    printf ("    0x%x Resets\n",
            pDrvCtrl->resetCounter);
    
    printf ("    %d transmit BDs, %d receive BDs, %d loaner buffers\n",
            pDrvCtrl->ether.txBdNum, pDrvCtrl->ether.rxBdNum,
            pDrvCtrl->nLoan);

    printf ("    Next transmit BD is %d,  Next receive BD is %d\n",
            pDrvCtrl->ether.txBdNext, pDrvCtrl->ether.rxBdNext);

    printf ("    Registers: ectrl (0x%x), edma (0x%x), mrbl (0x%x)\n",
            MBC_REG_READ (pDevBase, MBC_ECNTL),
            MBC_REG_READ (pDevBase, MBC_EDMA),
            MBC_REG_READ (pDevBase, MBC_EMRBLR));
    
    printf ("               ivec (0x%x), ievent (0x%x), imask (0x%x)\n",
            MBC_REG_READ (pDevBase, MBC_IVEC),
            MBC_REG_READ (pDevBase, MBC_IEVNT),
            MBC_REG_READ (pDevBase, MBC_IMASK));
                
    printf ("               ecnfg (0x%x), etest (0x%x), ar(0x%x)\n",
            MBC_REG_READ (pDevBase, MBC_ECFG),
            MBC_REG_READ (pDevBase, MBC_TEST),
            MBC_REG_READ (pDevBase, MBC_AR));
    }

/*******************************************************************************
*
* mbcBdShow - display buffer descriptors
*
* This routine displays the flags, len, and the buffer pointer fields of each
* buffer descriptor in the requested range. The parameter <start> is the
* beginning buffer descriptor, and <count> is number of buffer descriptors to
* display.
*
* RETURNS: N/A.
*
* NOMANUAL
*/

void mbcBdShow
    (
    int unit,                   /* unit number */
    int start,                  /* starting BD */
    int count                   /* number of BDs */
    )
    {
    DRV_CTRL *		pDrvCtrl;
    volatile MBC_BD *	pBd;    /* buffer descriptor pointer */
    
    
    if (unit < 0 || unit >= MAX_UNITS)
        return;

    pDrvCtrl = & drvCtrl[unit];
    pBd      = pDrvCtrl->ether.txBdBase;

    while (start < 128 && count)
        {
        printf ("BD %d: flags=0x%x, len=0x%x, pBuf=0x%xn",
                start, pBd[start].statusMode,
                pBd[start].dataLength,
                (UINT32) pBd[start].dataPointer);
 
        start++;
        count--;
        }
    }
