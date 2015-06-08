/* if_elt.c - 3Com 3C509 Ethernet network interface driver */

/* Copyright 1993-1998 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01o,24sep98,dat  SPR 21261, discard packets only when complete.
01n,15jul97,spm  changed ioctl handler: added ARP request to SIOCSIFADDR and 
                 cleaned up SIOCSIFFLAGS
01m,17mar97,hdn  added codes to load values in EEPROM to the address config
		 register and resource config register. 
01m,07apr97,spm  code cleanup, corrected statistics, and upgraded to BSD 4.4
01l,06mar96,hdn  supported PCMCIA EtherLink III card.
01k,14jun95,hdn  removed function declarations defined in sysLib.h.
01j,01feb95,jdi  doc cleanup.
01i,12nov94,tmk  removed some IMPORTed function decs and included unixLib.h
01h,28wed94,hdn  fixed a bug for RX incomplete packet.
		 cleaned up, eltFailure(), eltRxDrain(), eltTxCleanup().
01g,10apr94,hdn  made warm start work by clearing the interrupt latch bit.
01f,20feb94,bcs  add attachment (connector) type selection in eltattach(),
                 relegate receive-in-net-task to compile-time selection
01e,07feb94,bcs  fix excessive transmit (and/or receive) net job queuing,
                 add counters (as "ELT_TIMING") for netJobAdd() calls,
                 remove rxInNetTask argument to eltattach(), update comments
01d,23jan94,bcs  call eltTxCleanup() and eltStatFlush() from interrupt handler,
                 remove receive threshold (not obviously helping), add
                 receive and interrupt timing statistics, fix comments
01c,16jan94,bcs  eliminated receive task (didn't help enough), consolidated
                 packet receive routine into eltRxDrain, simplified transmit
                 to handle only one packet at a time but 10% slower
01b,30dec93,bcs  fixed adapter failure int. handling, broke up attach routine
                 added statistics capture
01a,10dec93,bcs  written
*/

/*
DESCRIPTION
This module implements the 3Com 3C509 network adapter driver.

The 3C509 (EtherLink\(rg III) is not well-suited for use in real-time systems.
Its meager on-board buffering (4K total; 2K transmit, 2K receive) forces the
host processor to service the board at a high priority.  3Com makes a virtue
of this necessity by adding fancy lookahead support and adding the label
"Parallel Tasking" to the outside of the box.  Using 3Com's drivers, this board
will look good in benchmarks that measure raw link speed.
The board is greatly simplified by using the host CPU as a DMA controller.


BOARD LAYOUT
This device is soft-configured by a DOS-hosted program supplied by the
manufacturer.  No jumpering diagram is required.


EXTERNAL INTERFACE
This driver provides the standard external interface with the following
exceptions.  All initialization is performed within the attach routine and
there is no separate initialization routine.  Thus, in the global interface
structure, the function pointer to the initialization routine is NULL.

There are two user-callable routines:
.iP eltattach() 14
publishes the `elt' interface and initializes the driver and device.
.iP eltShow()
displays statistics that are collected in the interrupt handler.
.LP
See the manual entries for these routines for more detail.


SYSTEM RESOURCE USAGE
    - one mutual exclusion semaphore
    - one interrupt vector
    - 16 bytes in the uninitialized data section (bss)
    - 180 bytes (plus overhead) of malloc'ed memory per unit
    - 1530 bytes (plus overhead) of malloc'ed memory per frame buffer,
        minimum 5 frame buffers.


SHORTCUTS
The EISA and MCA versions of the board are not supported.

Attachment selection assumes the board is in power-on reset state; a warm
restart will not clear the old attachment selection out of the hardware,
and certain new selections may not clear it either.  For example, if RJ45
was selected, the system is warm-booted, and AUI is selected, the RJ45
connector is still functional.

Attachment type selection is not validated against the board's capabilities,
even though there is a register that describes which connectors exist.

The loaned buffer cluster type is MC_EI; no new type is defined yet.

Although it seems possible to put the transmitter into a non-functioning
state, it is not obvious either how to do this or how to detect the resulting
state.  There is therefore no transmit watchdog timer.

No use is made of the tuning features of the board; it is possible that
proper dynamic tuning would reduce or eliminate the receive overruns
that occur when receiving under task control (instead of in the ISR).


TUNING HINTS
More receive buffers (than the default 20) could help by allowing more
loaning in cases of massive reception; four per receiving TCP connection plus
four extras should be considered a minimum.


INTERNAL
A driver implementation for this board faces two conflicting network
interface design goals:  (1) minimizing the time spent at interrupt level,
and (2) servicing the board's input without dropping packets.  This
implementation can be switched at run-time to meet either goal.

Received data is drained from the board's FIFO in the interrupt handler.
Many common TCP/IP occurrences, such as filling a 4K TCP window or
fragmenting a 4K packet, can cause data to arrive faster than it can be
drained at the task level.  This causes receive overruns and if the data
stream continues for very long, the resulting retries may cause the transmitter
to give up completely.

One possible compromise has been tried and discarded: to receive at the
task level in a high-priority task.  It turns out that the worst-case
task-switching latency is too great to prevent the board from overrunning
its receive FIFO often enough to be unacceptable.  Another possible compromise
is to add code to switch between task-level and interrupt-level reception
as necessary depending on the packet arrival rate.  Yet another possibility
is to use the receive threshold feature of the board to receive a packet
in chunks rather than all-or-nothing could make the interrupt-level reception
more palatable in some applications; the threshold would be advanced by perhaps
200 bytes per interrupt until the whole packet has been read.

The timing measurements represent an attempt to understand why the receiver
performance is what it is, both interrupt-level and task-level.  The most
important statistics are "timer overflows" and "max rx latency", where
overflow is a latency greater than 255 timer ticks.  "Timer invalids" are
not in themselves bad; it means that another interrupt (presumably transmit)
occurred before the receiver was serviced and caused the timer to be reset
(this is a hardware feature).


SEE ALSO: ifLib
*/

#define ELT_TIMING                          /* Compiles rx timing measurement */

#include "vxWorks.h"
#include "wdLib.h"
#include "iv.h"
#include "vme.h"
#include "net/mbuf.h"
#include "net/protosw.h"
#include "socket.h"
#include "ioctl.h"
#include "errno.h"
#include "memLib.h"
#include "intLib.h"
#include "net/if.h"
#include "net/unixLib.h"
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
#include "net/if_subr.h"
#include "drv/netif/if_elt.h"


/* defines */

#undef ELT_DEBUG                             /* Compiles debug output */

#define MAX_UNITS		4	/* maximum units to support */
#define DEF_NUM_RX_FRAMES	20	/* default number of rx buffers */
#define MIN_NUM_RX_FRAMES	4	/* number we reserve from loaning */

#define PARTIAL_TRANSFER_MASK   0xfffe  /* when pulling partial frames out */
                                        /* of receive FIFO, transfer even */
                                        /* words only */

/* These should be in a common file but which one? */
/* Ethernet attachment type selections */

#define ATTACHMENT_DEFAULT      0       /* use card as configured */
#define ATTACHMENT_AUI          1       /* AUI  (thick, DIX, DB-15) */
#define ATTACHMENT_BNC          2       /* BNC  (thin, 10BASE-2) */
#define ATTACHMENT_RJ45         3       /* RJ-45 (twisted pair, TPE, 10BASE-T)*/

#define ELT_PRODUCTID_3C589	0x9058	/* product ID for PCMCIA 3C589 */


/* globals */

ELT_CTRL * pEltCtrl [MAX_UNITS];


/* locals */



/* forward function declarations */

LOCAL void	eltBoardInit	(int unit);
LOCAL STATUS	eltMemoryInit	(int unit);
LOCAL void	eltReset	(int unit);
LOCAL int	eltIoctl	(IDR * pIDR, int cmd, caddr_t data);
#ifdef BSD43_DRIVER
LOCAL int	eltOutput	(IDR * pIDR, MBUF * pMbuf, SOCK * pDestAddr);
LOCAL void	eltTxOutputStart (int unit);
#else
LOCAL void	eltTxOutputStart (ELT_CTRL * pDrvCtrl);
#endif
LOCAL void	eltInt		(ELT_CTRL * pDrvCtrl);
LOCAL void	eltTxFlush	(ELT_CTRL * pDrvCtrl, BOOL netJob);

LOCAL void	eltRxDeliver	(ELT_CTRL * pDrvCtrl);
LOCAL void	eltLoanFree	(ELT_CTRL * pDrvCtrl, ELT_FRAME * pFrame);
LOCAL void	eltRxFree	(ELT_CTRL * pDrvCtrl, ELT_FRAME * pFrame);
LOCAL void	eltStatFlush	(ELT_CTRL * pDrvCtrl);
LOCAL void	eltIntMaskSet	(ELT_CTRL * pDrvCtrl, int maskBits);
LOCAL void	eltIntDisable	(ELT_CTRL * pDrvCtrl);
LOCAL void	eltIntEnable	(ELT_CTRL * pDrvCtrl);
LOCAL void	eltRxStart	(ELT_CTRL * pDrvCtrl);
LOCAL void	eltTxStart	(ELT_CTRL * pDrvCtrl);
LOCAL STATUS	eltActivate	(ELT_CTRL * pDrvCtrl);
LOCAL void	eltIdCommand	(int idPort);
LOCAL UINT16	eltIdEepromRead	(int address);
LOCAL int	eltEepromRead	(int port, int offset);


/*******************************************************************************
*
* eltEepromRead - read one 16-bit adapter EEPROM register.
*
* RETURNS: Value from an offset of the EEPROM.
*/

LOCAL int eltEepromRead
    (
    int port,
    int offset
    )

    {
    int ix;

    sysOutWord (port + EEPROM_CONTROL, 0x80 + offset);

    /* wait 162 us minimum */

    for (ix = 0; ix < 300; ix++)
	sysDelay ();
    
    return (sysInWord (port + EEPROM_DATA));
    }

/*******************************************************************************
*
* eltattach - publish the `elt' interface and initialize the driver and device
*
* The routine publishes the `elt' interface by filling in a network interface
* record and adding this record to the system list.  This routine also
* initializes the driver and the device to the operational state.
*
* RETURNS: OK or ERROR.
*
* SEE ALSO: ifLib
*/

STATUS eltattach
    (
    int         unit,                   /* unit number */
    int         port,                   /* base I/O address */
    int         ivec,                   /* interrupt vector number */
    int         intLevel,               /* interrupt level */
    int         nRxFrames,              /* # of receive frames (0=default) */
    int         attachment,             /* Ethernet connector to use */
    char	*ifName			/* interface name */
    )
    {
    ELT_CTRL *  pDrvCtrl;
    int productID;
    short *p;

    /* Sanity check the unit number */

    if (unit < 0 || unit >= MAX_UNITS)
        return (ERROR);

    /* allocate this unit's control structure the first time attach is called */

    if (pEltCtrl [unit] == NULL)
        {
        pEltCtrl [unit] = malloc (sizeof (ELT_CTRL));
        if (pEltCtrl [unit] == NULL)
            return (ERROR);
        bzero ( (char *) pEltCtrl [unit], sizeof (ELT_CTRL));
        }

    /* Ensure single invocation per system life */

    pDrvCtrl = pEltCtrl [unit];
    if (pDrvCtrl->attached)
        return (OK);

    /* it would be a good idea to validate these parameters.... */

    pDrvCtrl->intLevel	= intLevel;	/* PC IRQ number */
    pDrvCtrl->ivec	= ivec;		/* interrupt vector */

    /* Determine which attachment is desired */

    switch (attachment)
        {
        case ATTACHMENT_DEFAULT:
        case ATTACHMENT_AUI:
        case ATTACHMENT_BNC:
        case ATTACHMENT_RJ45:
            pDrvCtrl->attachment = attachment;
            break;
        default:
            return (ERROR);             /* unknown value */
        }

    /* Find and activate the adapter */

    pDrvCtrl->port = port;
    productID = eltEepromRead (pDrvCtrl->port, EE_A_PRODUCT_ID);
    if (productID == ELT_PRODUCTID_3C589)
	{
        p = (short *)pDrvCtrl->idr.ac_enaddr;
	*p++ = htons (eltEepromRead (port, EE_A_OEM_NODE_0));
	*p++ = htons (eltEepromRead (port, EE_A_OEM_NODE_1));
	*p++ = htons (eltEepromRead (port, EE_A_OEM_NODE_2));
	sysOutWord (port + ELT_COMMAND, SELECT_WINDOW | WIN_CONFIG);
	sysOutWord (port + ADDRESS_CONFIG, eltEepromRead (port, EE_A_ADDRESS));
	sysOutWord (port + RESOURCE_CONFIG, eltEepromRead (port,EE_A_RESOURCE));
	}
    else
        if (eltActivate (pDrvCtrl) != OK)
            return (ERROR);

    eltBoardInit (unit);

    /* Determine number of Rx descriptors to use */

    pDrvCtrl->nRxFrames = nRxFrames ? nRxFrames : DEF_NUM_RX_FRAMES;

    /* Publish the interface record */

    if (strncmp (ifName, "pcmcia", 6) != 0)
	ifName = "elt";

#ifdef BSD43_DRIVER
    ether_attach    ( (IFNET *) & pDrvCtrl->idr, unit, ifName, (FUNCPTR) NULL,
                    (FUNCPTR) eltIoctl, (FUNCPTR) eltOutput, (FUNCPTR) eltReset
                    );
#else
    ether_attach    ( 
                    (IFNET *) & pDrvCtrl->idr, 
                    unit, 
                    ifName, 
                    (FUNCPTR) NULL,
                    (FUNCPTR) eltIoctl, 
                    (FUNCPTR) ether_output, 
                    (FUNCPTR) eltReset
                    );
    pDrvCtrl->idr.ac_if.if_start = (FUNCPTR)eltTxOutputStart;
#endif

    if (eltMemoryInit (unit) != OK)
        return (ERROR);

    /* keep operating registers in window except in critical sections */

    sysOutWord (port + ELT_COMMAND, SELECT_WINDOW | WIN_OPERATING);

    /* Connect the interrupt handler */

    if (intConnect ( (VOIDFUNCPTR *)INUM_TO_IVEC (pDrvCtrl->ivec),
                    eltInt, (int)pDrvCtrl) == ERROR)
        return (ERROR);
    sysIntEnablePIC (intLevel);
    eltIntEnable (pDrvCtrl);

    /* Set our flag */

    pDrvCtrl->attached = TRUE;

    /* Set status flags in IDR */

    pDrvCtrl->idr.ac_if.if_flags |= (IFF_UP | IFF_RUNNING | IFF_NOTRAILERS);

    /* Successful return */

    return (OK);
    }

/*******************************************************************************
*
* eltBoardInit - prepare the board for operation
*
*/

static void eltBoardInit
    (
    int unit
    )
    {
    ELT_CTRL * pDrvCtrl;
    int port;
    int index;
    UINT16 transceiver;

    pDrvCtrl = pEltCtrl [unit];
    port = pDrvCtrl->port;

    eltIntDisable (pDrvCtrl);   /* make it OK to change register window */

    sysOutWord (port + ELT_COMMAND, SELECT_WINDOW | WIN_ADDRESS);
    for (index = 0; index < EA_SIZE; index++)
        sysOutByte (port + index, pDrvCtrl->idr.ac_enaddr [index]);

    /* Select the transceiver hardware (attachment) then do whatever is
     * necessary to activate the selected attachment.
     * A truly complete implementation would probably check to see
     * if the selected attachment were present.
     */

    sysOutWord (port + ELT_COMMAND, SELECT_WINDOW | WIN_CONFIG);
    transceiver = sysInWord (port + ADDRESS_CONFIG);
    if (pDrvCtrl->attachment == ATTACHMENT_DEFAULT)
        {
        switch (transceiver & AC_XCVR_MASK)
            {
            case AC_XCVR_TPE:
                pDrvCtrl->attachment = ATTACHMENT_RJ45;
                break;
            case AC_XCVR_BNC:
                pDrvCtrl->attachment = ATTACHMENT_BNC;
                break;
            case AC_XCVR_AUI:
                pDrvCtrl->attachment = ATTACHMENT_AUI;
                break;
            default:    /* there's only one other value; it's "reserved" */
                pDrvCtrl->attachment = ATTACHMENT_AUI;  /* good enough */
                break;
            }
        }

    /* Now set the selected attachment type, even if it was already selected */

    transceiver &= ~AC_XCVR_MASK;
    switch (pDrvCtrl->attachment)
        {
        case ATTACHMENT_RJ45:
            sysOutWord (port + ADDRESS_CONFIG, transceiver | AC_XCVR_TPE);
            sysOutWord (port + ELT_COMMAND, SELECT_WINDOW | WIN_DIAGNOSTIC);
            sysOutByte (port + MEDIA_STATUS,
                        sysInByte (port + MEDIA_STATUS) |
                        MT_S_JABBER_ENABLE | MT_S_LINK_BEAT_ENABLE);
            break;
        case ATTACHMENT_BNC:
            sysOutWord (port + ADDRESS_CONFIG, transceiver | AC_XCVR_BNC);
            sysOutWord (port + ELT_COMMAND, START_COAX);
            break;
        case ATTACHMENT_AUI:
        default:
            sysOutWord (port + ADDRESS_CONFIG, transceiver | AC_XCVR_AUI);
            break;
        }

    /* enable normal Ethernet address recognition */

    pDrvCtrl->rxFilter = RX_F_NORMAL;

    /*
     * Define the set of status bits that could cause interrupts.
     * There is no interrupt cause indicator separate from the board status.
     * To keep the ISR from seeing status conditions we didn't want to be
     * interrrupted for, we must mask the STATUS off, not the interrupt.
     * This prevents the condition from interrupting and also prevents
     * the ISR from seeing the condition even if it is true.
     * The interrupt mask is set only once to the set of all conditions
     * we might want to be interrupted by; the status mask is set and
     * cleared according to which conditions we want at any particular
     * time.  The intMask field in the control structure
     * is named for its effect; it is really used in the status mask
     * command (3Com calls it the read zero mask).
     */

    /* enable the status bits we currently want to cause interrupts */

    eltIntMaskSet (pDrvCtrl, ADAPTER_FAILURE | TX_COMPLETE |
                             RX_COMPLETE | RX_EARLY | UPDATE_STATS);

    /* enable the collection of statistics */

    sysOutWord (port + ELT_COMMAND, STATS_ENABLE);

    /* enable the hardware to generate interrupt requests */

    sysOutWord (port + ELT_COMMAND, SELECT_WINDOW | WIN_CONFIG);
    sysOutWord (port + CONFIG_CONTROL, CC_ENABLE);

    sysOutWord (port + ELT_COMMAND, SELECT_WINDOW | WIN_OPERATING);

    eltIntEnable (pDrvCtrl);

    eltRxStart (pDrvCtrl);
    eltTxStart (pDrvCtrl);
    }

/*******************************************************************************
*
* eltMemoryInit - initialize memory structures
*
* The only thing we allocate from system meory besides the driver control
* structure is the frame buffer pool.  Here we malloc it as one big region
* and divide it up into receive and transmit buffers.
*/

static STATUS eltMemoryInit
    (
    int unit
    )
    {
    ELT_CTRL * pDrvCtrl;
    ELT_FRAME * pFrame;
    int         index;

    pDrvCtrl = pEltCtrl [unit];

    /* Initialize our data structures */

    pDrvCtrl->pFrameArea = malloc ( (1 + pDrvCtrl->nRxFrames)
                                    * sizeof (ELT_FRAME));
    if (pDrvCtrl->pFrameArea == NULL)
        return (ERROR);

    pFrame = pDrvCtrl->pFrameArea;
    pDrvCtrl->pTxFrame = pFrame++;              /* first allocate tx frame */

    pDrvCtrl->pRxHead = pFrame;                 /* first rx frame */
    for (index = pDrvCtrl->nRxFrames; index > 0; index--)
        {
        pFrame->count = 0;
        pFrame->nextByte = pFrame->header;
        pFrame->length = 0;
        pFrame->lNext = pFrame + 1;
        pFrame++;
        }
    pDrvCtrl->pRxTail = pFrame - 1;             /* last rx frame */
    pDrvCtrl->pRxTail->lNext = NULL;

    pDrvCtrl->pRxCurrent = pDrvCtrl->pRxHead;
    pDrvCtrl->nLoanFrames = pDrvCtrl->nRxFrames - MIN_NUM_RX_FRAMES;

    return (OK);
    }

/*******************************************************************************
*
* eltReset - reset the elt interface
*
* Mark interface as inactive and reset the adapter.
*/

static void eltReset
    (
    int unit
    )
    {
    ELT_CTRL * pDrvCtrl;

    pDrvCtrl = pEltCtrl [unit];

    eltIntDisable (pDrvCtrl);           /* prevent device from interrupting */
    sysOutWord (pDrvCtrl->port + ELT_COMMAND, RX_RESET);
    sysOutWord (pDrvCtrl->port + ELT_COMMAND, TX_RESET);

    }

/*******************************************************************************
*
* eltIoctl - interface ioctl procedure
*
* Process an interface ioctl request.
*/

static int eltIoctl
    (
    IDR     *pIDR,
    int     cmd,
    caddr_t data
    )
    {
    int unit;
    ELT_CTRL * pDrvCtrl;
    int error;

#ifdef ELT_DEBUG
    printf ("elt: ioctl: pIDR=%x cmd=%x data=%x\n", pIDR, cmd, data);
#endif /* ELT_DEBUG */

    unit = pIDR->ac_if.if_unit;
    pDrvCtrl = pEltCtrl [unit];
    error = 0;

    switch (cmd)
        {
        case SIOCSIFADDR:
            ((struct arpcom *)pIDR)->ac_ipaddr = IA_SIN (data)->sin_addr;
            arpwhohas (pIDR, &IA_SIN (data)->sin_addr);
            break;

        case SIOCSIFFLAGS:

            /* Set promiscuous mode according to current interface flags */

            pDrvCtrl->rxFilter = ( (pIDR->ac_if.if_flags & IFF_PROMISC) != 0) ?
                                   RX_F_PROMISCUOUS : RX_F_NORMAL;
            sysOutWord (pDrvCtrl->port + ELT_COMMAND,
                        SET_RX_FILTER | pDrvCtrl->rxFilter);
            break;

        default:
            error = EINVAL;
        }

    return (error);
    }

#ifdef BSD43_DRIVER
/*******************************************************************************
*
* eltOutput - interface output routine.
*
* This is the entry point for packets arriving from protocols above.  This
* routine merely calls our specific output routines that eventually lead
* to a transmit to the network.
*
* RETURNS: 0 or appropriate errno
*/

static int eltOutput
    (
    IDR *    pIDR,
    MBUF *     pMbuf,
    SOCK * pDestAddr
    )
    {

#ifdef ELT_DEBUG
    printf ("elt: output: pIDR=%x pMbuf=%x pDestAddr=%x\n",
            pIDR, pMbuf, pDestAddr);
#endif /* ELT_DEBUG */

    return (ether_output ( (IFNET *)pIDR, pMbuf, pDestAddr,
            (FUNCPTR) eltTxOutputStart, pIDR));
    }
#endif

/*******************************************************************************
*
* eltTxOutputStart - start output on the board
*
* This routine is called from ether_output() when a new packet is enqueued
* in the interface mbuf queue.
*
* Note that this function is ALWAYS called between an splnet() and an splx().
* This is true because netTask(), and ether_output() take care of
* this when calling this function. Therefore, no calls to these spl functions
* are needed anywhere in this output thread.
*/

#ifdef BSD43_DRIVER
static void eltTxOutputStart
    (
    int unit
    )
    {
    ELT_CTRL * pDrvCtrl;

    pDrvCtrl = pEltCtrl [unit];
#else
static void eltTxOutputStart
    (
    ELT_CTRL * pDrvCtrl
    )
    {
#endif
    if (sysInWord (pDrvCtrl->port + TX_FREE_BYTES) >= TX_IDLE_COUNT)
        {
#ifdef ELT_TIMING

        /* pretend we queued eltTxFlush, to keep the counts straight */

        if ( (int)++pDrvCtrl->eltStat.taskQTxOuts >
             (int)pDrvCtrl->eltStat.maxTxTaskQ)
            pDrvCtrl->eltStat.maxTxTaskQ = pDrvCtrl->eltStat.taskQTxOuts;
#endif /* ELT_TIMING */
        eltTxFlush (pDrvCtrl, FALSE);   /* not called via netJobAdd() */
        }
    }

/*******************************************************************************
*
* eltInt - entry point for handling interrupts from the EtherLink III
*
* Most of the board's interrupting conditions must be actually handled to
* clear the interrupting condition; the ACK command does nothing for them.
* Where we defer handling to the task level, we mask the status conditions
* to prevent a repeated interrupt.  This works everywhere except when there
* are received packets waiting to be delivered to the protocols; this is
* synchronized by a flag.
*
* Note that the board manual says to acknowledge the board interrupts before
* acknowledging the processor interrupt, presumably because of level-sensitive
* logic somewhere.
*/

static void eltInt
    (
    ELT_CTRL * pDrvCtrl
    )
    {
    UINT16 status;
    UINT16 statusDiag;
    UINT16 statusRx;
    UINT16 statusRxNew;
    UINT8  statusTx;
    UINT16 length;
    int	   ix = 0;
    int    port = pDrvCtrl->port;
    BOOL   needTxStart = FALSE;
    volatile ELT_FRAME * pFrame;
    STATUS result;

#ifdef ELT_TIMING
    {
        int time = (int) sysInByte (pDrvCtrl->port + TIMER);

        if (time> pDrvCtrl->eltStat.maxIntLatency)
            pDrvCtrl->eltStat.maxIntLatency = time; 
        if (pDrvCtrl->interruptTime == -1)
            pDrvCtrl->interruptTime = time;
        else if (pDrvCtrl->interruptTime >= 0)
            pDrvCtrl->interruptTime = -1;
    }
#endif /* ELT_TIMING */

    status = sysInByte (pDrvCtrl->port + ELT_STATUS) & 0x00ff;

    if ( (status & INTERRUPT_LATCH) == 0)
        {
        ++pDrvCtrl->eltStat.strayint;
        return;
        }
    else
        ++pDrvCtrl->eltStat.interrupts;

    /* Handle adapter failure first in case other conditions mask it */

    if ( (status & ADAPTER_FAILURE) != 0)
        {
        sysOutWord (port + ELT_COMMAND, SELECT_WINDOW | WIN_DIAGNOSTIC);
        statusDiag = sysInWord (port + FIFO_DIAGNOSTIC);
        sysOutWord (port + ELT_COMMAND, SELECT_WINDOW | WIN_OPERATING);

        if ( (statusDiag & FD_TX_OVERRUN) != 0)
            {
            ++pDrvCtrl->eltStat.txoverruns;
            sysOutWord (port + ELT_COMMAND, TX_RESET);
            eltTxStart (pDrvCtrl);
            }

        if ( (status & FD_RX_UNDERRUN) != 0)
            {
            ++pDrvCtrl->eltStat.rxunderruns;
            sysOutWord (port + ELT_COMMAND, RX_RESET);
            eltRxStart (pDrvCtrl);
            }
        }


    /* Handle receive complete interrupt */

    if ( (status & (RX_COMPLETE | RX_EARLY)) != 0)
        {

        if ( (status & RX_EARLY) != 0)
            ++pDrvCtrl->eltStat.rxearly;
        if (pDrvCtrl->pRxCurrent == NULL)
            ++pDrvCtrl->eltStat.rxnobuffers;

#ifdef ELT_TIMING
        {
        int timeNow;
        int timeDelta;

        if (pDrvCtrl->interruptTime >= 0)
            {
            timeNow = (int)sysInByte (port + TIMER);
            if (timeNow == 0xff)
                ++pDrvCtrl->eltStat.timerOverflow;
            else
                {
                ++pDrvCtrl->eltStat.timerUpdates;
                timeDelta = timeNow - pDrvCtrl->interruptTime;
                if (timeDelta > pDrvCtrl->eltStat.maxRxLatency)
                    pDrvCtrl->eltStat.maxRxLatency = timeDelta; 
                if ( (pDrvCtrl->eltStat.minRxLatency == 0) ||
                    (timeDelta < pDrvCtrl->eltStat.minRxLatency))
                    pDrvCtrl->eltStat.minRxLatency = timeDelta;
                }
            }
        else if (pDrvCtrl->interruptTime == -1)
            ++pDrvCtrl->eltStat.timerInvalid;
        pDrvCtrl->interruptTime = -2;
        }
#endif /* ELT_TIMING */

        while ( (pFrame = pDrvCtrl->pRxCurrent) != NULL)
            {
            statusRx = sysInWord(port + RX_STATUS);

            if (statusRx & RX_S_INCOMPLETE)	/* incomplete */
	        {
                if (statusRx == RX_S_INCOMPLETE)	/* no bytes available */
		    break;

                while ((statusRx != (statusRxNew = sysInWord(port + RX_STATUS)))
		       && (ix < 10000))
		    {
		    statusRx = statusRxNew;   
		    ix++;	/* XXX */
		    }
	        }

#ifdef ELT_DEBUG
            logMsg ("eltInt: statusRx=%04x ix=%d\n", statusRx, ix, 0, 0, 0, 0);
#endif /* ELT_DEBUG */
    
            /* Copy the FIFO data into the buffer */

            length = statusRx & RX_S_CNT_MASK;

            /* Prevent buffer overflow.
             * should we discard the packet? that's hard to do safely here.
	     * so truncate packet
	     */

            if ( (pFrame->count + length) > MAX_FRAME_SIZE)
                length = MAX_FRAME_SIZE - pFrame->count;

            /* transfer only an even number of bytes to leave the pointer
             * on an even boundary until we reach the end of the frame
             */

            if ( (statusRx & RX_S_INCOMPLETE) != 0)
                length &= PARTIAL_TRANSFER_MASK;

            sysInWordString (port + DATA_REGISTER, (short *) pFrame->nextByte,
                             (length + 1) / 2);
            pFrame->count += length;
            pFrame->nextByte += length;

	    /* partial packet, wait for complete packet */

            if ( (statusRx & RX_S_INCOMPLETE) != 0)
                break;

	    /*
	     * Packet is now complete. Use 'Discard' command to complete the 
	     * transfer
	     */

            sysOutWord (port + ELT_COMMAND, RX_DISCARD); /* discard packet */
            while ( (sysInWord (port + ELT_STATUS) & COMMAND_IN_PROGRESS) != 0)
                ;                           /* wait for command to finish */

            /* Check for errors in completed packet */

            if ( (statusRx & RX_S_ERROR) != 0)
                {
                switch (statusRx & RX_S_CODE_MASK)
                    {
                    case RX_S_OVERRUN:	/* handled by statistics registers */
                        break;
                    case RX_S_RUNT:
                        ++pDrvCtrl->eltStat.shortPacket;
                        break;
                    case RX_S_ALIGN:
                        ++pDrvCtrl->eltStat.aligns;
                        break;
                    case RX_S_CRC:
                        ++pDrvCtrl->eltStat.crcs;
                        break;
                    case RX_S_OVERSIZE:
                        ++pDrvCtrl->eltStat.badPacket;
                        break;
                    default:		/* no other codes are defined */
                        break;	
                    }
                ++pDrvCtrl->eltStat.rxerror;

                ++pDrvCtrl->idr.ac_if.if_ierrors;   /* bump error counter */
                pFrame->count = 0;
                pFrame->nextByte = pFrame->header;
                pFrame->length = 0;
                continue;
                }

            /* Got a complete packet, count it. */

            ++pDrvCtrl->idr.ac_if.if_ipackets;

            pFrame->length = statusRx;
            pDrvCtrl->pRxCurrent = pDrvCtrl->pRxCurrent->lNext;
            }
#ifdef ELT_TIMING
        pDrvCtrl->interruptTime = -1;
#endif /* ELT_TIMING */

        /* receive no more for now */

        if (pDrvCtrl->pRxCurrent == NULL)
            pDrvCtrl->intMask &= ~(RX_COMPLETE | RX_EARLY);

        if (!pDrvCtrl->rxHandling && (pDrvCtrl->pRxHead->length != 0))
            {
            result = netJobAdd ( (FUNCPTR) eltRxDeliver, (int) pDrvCtrl,
                                 0, 0, 0, 0);
#ifdef ELT_DEBUG
            if (result == ERROR)
                logMsg ("elt: netJobAdd (eltRxDeliver) failed\n",
                        0, 0, 0, 0, 0, 0);
#endif /* ELT_DEBUG */

#ifdef ELT_TIMING
            if (result != ERROR)
                if ( (int)++pDrvCtrl->eltStat.taskQRxOuts >
                          (int)pDrvCtrl->eltStat.maxRxTaskQ)
                     pDrvCtrl->eltStat.maxRxTaskQ = 
                              pDrvCtrl->eltStat.taskQRxOuts;
#endif /* ELT_TIMING */
            pDrvCtrl->rxHandling = TRUE;
            }
        }


    /* Handle transmitter interrupts */

    if ( (status & TX_COMPLETE) != 0)
        {
        if ( ( (statusTx = sysInByte (port + TX_STATUS)) & TX_S_COMPLETE) != 0)
            {
            sysOutByte (port + TX_STATUS, 0);       /* clear old status */

            /* other errors are tabulated by reading the statistics registers */

            if ( (statusTx & TX_S_MAX_COLL) != 0)
                pDrvCtrl->idr.ac_if.if_collisions += 16;
            if ( (statusTx & TX_S_JABBER) != 0)
                ++pDrvCtrl->eltStat.jabbers;
            
            if ((statusTx & (TX_S_JABBER | TX_S_MAX_COLL | TX_S_UNDERRUN)) != 0)
                {
                needTxStart = TRUE;                 /* restart transmitter */

                /* packet not sent */

                pDrvCtrl->idr.ac_if.if_oerrors++;   /* incr err count */
                pDrvCtrl->idr.ac_if.if_opackets--;  /* decr sent count */
                }
            if ( (statusTx & (TX_S_JABBER | TX_S_UNDERRUN)) != 0)
                {

                /* Must reset transmitter; this clears the tx FIFO */

                sysOutWord (port + ELT_COMMAND, TX_RESET);
                }
            }

        if (needTxStart)                            /* check flag */
            eltTxStart (pDrvCtrl);

        if (pDrvCtrl->idr.ac_if.if_snd.ifq_head != NULL)
            {
            if (!pDrvCtrl->txHandling)
                {
                if (netJobAdd ( (FUNCPTR) eltTxFlush, (int) pDrvCtrl, (int) TRUE,
                                0, 0, 0) == ERROR)
                    {
#ifdef ELT_DEBUG
                    logMsg ("elt: netJobAdd (eltTxFlush) failed\n",
                            0, 0, 0, 0, 0, 0);
#endif /* ELT_DEBUG */
                    }
                else
                    pDrvCtrl->txHandling = TRUE;
#ifdef ELT_TIMING
                if ( (int)++pDrvCtrl->eltStat.taskQTxOuts >
                     (int)pDrvCtrl->eltStat.maxTxTaskQ)
                    pDrvCtrl->eltStat.maxTxTaskQ = pDrvCtrl->eltStat.taskQTxOuts;
#endif /* ELT_TIMING */
                }
            }
        }


    /* Handle update statistics interrupt */

    if ( (status & UPDATE_STATS) != 0)
        eltStatFlush (pDrvCtrl);


    /* mask and ack the events we've just handled or queued handlers for */

    sysOutWord (pDrvCtrl->port + ELT_COMMAND, MASK_STATUS | pDrvCtrl->intMask);
    sysOutWord (pDrvCtrl->port + ELT_COMMAND, ACK_INTERRUPT | status);
    sysBusIntAck (pDrvCtrl->intLevel);
    }

/*******************************************************************************
*
* eltTxFlush - transmit a packet from the output queue
*
* This routine, if the transmitter is currently idle, and if there is a
* packet in the output queue, dequeues a packet and transmits it.  The
* next transmit activity will be a completion interrupt, which will cause
* this routine to be scheduled again if there is a packet in the output
* queue.
*/

static void eltTxFlush
    (
    ELT_CTRL *  pDrvCtrl,
    BOOL        netJob
    )
    {
    int port;
    ELT_FRAME * pFrame;
    UINT16 outputCount;
    MBUF * m;

    port = pDrvCtrl->port;
    pFrame = pDrvCtrl->pTxFrame;

    do
        {
        if (netJob)
            pDrvCtrl->txHandling = TRUE;

        /* Don't dequeue if currently sending */

        while ( (sysInWord (pDrvCtrl->port + TX_FREE_BYTES) >= TX_IDLE_COUNT) &&
                (pDrvCtrl->idr.ac_if.if_snd.ifq_head != NULL))
            {

            IF_DEQUEUE (&pDrvCtrl->idr.ac_if.if_snd, m);

            copy_from_mbufs (pFrame->header, m, outputCount);

            outputCount = max (ETHERSMALL, outputCount);

            if ( (etherOutputHookRtn != NULL) &&
                 (* etherOutputHookRtn)
                 (&pDrvCtrl->idr, pFrame->header, outputCount))
                continue;

            pFrame->length = outputCount | TX_F_INTERRUPT;
            outputCount = (outputCount + TX_F_PREAMBLE_SIZE + 3) &
                          TX_F_DWORD_MASK;

            sysOutWordString (port + DATA_REGISTER, (short *) & pFrame->length,
                              outputCount / 2);

            /* Bump the statistic counter. */
#ifdef ELT_DEBUG
            printf ("Packet sent.\n");
            fflush (stdout);
#endif /* ELT_DEBUG */

#ifndef BSD43_DRIVER
            pDrvCtrl->idr.ac_if.if_opackets++;
#endif

            break;                      /* ISR must call eltTxCleanup() */
            }
        if (netJob)
            pDrvCtrl->txHandling = FALSE;
        }
    while ( (sysInWord (pDrvCtrl->port + TX_FREE_BYTES) >= TX_IDLE_COUNT) &&
            (pDrvCtrl->idr.ac_if.if_snd.ifq_head != NULL));
#ifdef ELT_TIMING
    --pDrvCtrl->eltStat.taskQTxOuts;
#endif /* ELT_TIMING */
    }

/*******************************************************************************
*
* eltRxDeliver - pass received frames to the next layer up
*
* Strips the Ethernet header and passes the packet to the appropriate
* protocol.  If there are enough received frame buffers, the packet may
* be passed to the protocol in the frame buffer.
* In this latter case, the frame buffer will
* eventually be returned by the protocol, via a call to our eltLoanFree().
*/

static void eltRxDeliver
    (
    ELT_CTRL * pDrvCtrl
    )
    {
    volatile ELT_FRAME * pFrame;
    UINT16      length;
    struct ether_header *   pEh;
    u_char *    pData;
    UINT16      etherType;
    MBUF *      pMbuf;

#ifdef ELT_DEBUG
    printf ("Elt: Handling received packet.\n");
    fflush (stdout);
#endif /* ELT_DEBUG */

    do
        {
        pDrvCtrl->rxHandling = TRUE;
        while ( ( (pFrame = pDrvCtrl->pRxHead) != NULL) &&
                (pFrame->length != 0))
            {
            pDrvCtrl->pRxHead = pFrame->lNext;

            length = pFrame->count - EH_SIZE;
            pEh = (struct ether_header *)pFrame->header;
            pData = (u_char *) pFrame->data;
            etherType = ntohs (pEh->ether_type);

            /* Service input hook */

            if (etherInputHookRtn != NULL)
                {
                if ( (* etherInputHookRtn) (&pDrvCtrl->idr, (char *)pEh,
                                            length) )
                    {
                    eltRxFree (pDrvCtrl, (ELT_FRAME *)pFrame);
                    continue;
                    }
                }

#ifdef ELT_DEBUG
        printf ("elt: rxDeliver: frame at %08lx, type %04x, length %d, count %d\n",
                (long) pFrame, etherType, pFrame->length, pFrame->count);
#endif /* ELT_DEBUG */

            /* we can loan out receive frames from our pool if:
             *
             * 1) we have enough (nLoanFrames > 0) and
             * 2) size of the input ethernet frame is large enough to be used
             *    with clustering.
             */

            pFrame->refCnt = 0;

            if ( (pDrvCtrl->nLoanFrames > 0) && (USE_CLUSTER (length)) &&
                 ( (pMbuf = build_cluster (pData, length, &pDrvCtrl->idr, MC_EI,
                                     &pFrame->refCnt, eltLoanFree,
                                     (int) pDrvCtrl,
                                     (int) pFrame, NULL)) != NULL))
                {
                pDrvCtrl->nLoanFrames -= 1;             /* one less to loan */
                }
            else
                {
                pMbuf = copy_to_mbufs (pData, length, 0, &pDrvCtrl->idr);
                eltRxFree (pDrvCtrl, (ELT_FRAME *)pFrame);
                }

            /* deliver mbufs to protocol */

#ifdef BSD43_DRIVER
            if (pMbuf != NULL)
                do_protocol_with_type (etherType, pMbuf, &pDrvCtrl->idr,length);
#else
            if (pMbuf != NULL)
                do_protocol (pEh, pMbuf, &pDrvCtrl->idr, length);
#endif
            else
                ++pDrvCtrl->idr.ac_if.if_ierrors;
            }
        pDrvCtrl->rxHandling = FALSE;   /* allow self to be rescheduled */
        }
    while ( ( (pFrame = pDrvCtrl->pRxHead) != NULL) && (pFrame->length != 0));
#ifdef ELT_TIMING
    --pDrvCtrl->eltStat.taskQRxOuts;
#endif /* ELT_TIMING */
    }

/*******************************************************************************
*
* eltLoanFree - return a loaned receive frame buffer
*
* This routine is called by the protocol code when it has completed use of
* a frame buffer that we loaned to it.
*/

static void eltLoanFree
    (
    ELT_CTRL * pDrvCtrl,
    ELT_FRAME * pFrame
    )
    {
    eltRxFree (pDrvCtrl, pFrame);

    pDrvCtrl->nLoanFrames += 1;         /* one more to loan */

    }

/*******************************************************************************
*
* eltRxFree - free a received frame buffer
*
* Reinitialize necessary fields in the frame structure and link it to the
* tail of the free frames list.  If there was no current frame (the receive
* routine had run out of empty buffers), make this the current frame and
* unmask receive interrupts in case they were waiting for a buffer.
*/

static void eltRxFree
    (
    ELT_CTRL * pDrvCtrl,
    ELT_FRAME * pFrame
    )
    {
    pFrame->lNext = NULL;
    pFrame->count = 0;
    pFrame->nextByte = pFrame->header;
    pFrame->length = 0;

    eltIntDisable (pDrvCtrl);

    if (pDrvCtrl->pRxHead == NULL)
        pDrvCtrl->pRxHead = pFrame;
    else
        pDrvCtrl->pRxTail->lNext = pFrame;
    pDrvCtrl->pRxTail = pFrame;

    if (pDrvCtrl->pRxCurrent == NULL)
        {
        pDrvCtrl->pRxCurrent = pFrame;

        eltIntMaskSet (pDrvCtrl, RX_COMPLETE | RX_EARLY);
        }

    eltIntEnable (pDrvCtrl);
    }

/*******************************************************************************
*
* eltStatFlush - flush the board's statistics registers to statistics block
*
* Called when the board reports that its statistics registers are getting
* full, or when someone wants to see the current statistics (to fully update
* the statistics block).
*
* Note that reading a statistics register zeroes it in the hardware.
* Note also that zeroing all the registers is necessary and sufficient
* to clear the interrupt condition.
*
* Must be called with board or system interrupts disabled.
*/

static void eltStatFlush
    (
    ELT_CTRL * pDrvCtrl
    )
    {
    int         port;

    port = pDrvCtrl->port;

    sysOutWord (port + ELT_COMMAND, SELECT_WINDOW | WIN_STATISTICS);
    sysOutWord (port + ELT_COMMAND, STATS_DISABLE);

    pDrvCtrl->eltStat.nocarriers +=     sysInByte (port + CARRIER_LOSTS) & 0x0f;
    pDrvCtrl->eltStat.heartbeats +=     sysInByte (port + SQE_FAILURES) & 0x0f;
    pDrvCtrl->eltStat.multcollisions += sysInByte (port + MULT_COLLISIONS)
                                        & 0x3f;
    pDrvCtrl->eltStat.collisions +=     sysInByte (port + ONE_COLLISIONS)
                                        & 0x3f;
    pDrvCtrl->eltStat.latecollisions += sysInByte (port + LATE_COLLISIONS);
    pDrvCtrl->eltStat.rxoverruns +=     sysInByte (port + RECV_OVERRUNS);
    pDrvCtrl->eltStat.txnoerror +=      sysInByte (port + GOOD_TRANSMITS);
    pDrvCtrl->eltStat.rxnoerror +=      sysInByte (port + GOOD_RECEIVES);
    pDrvCtrl->eltStat.deferring +=      sysInByte (port + TX_DEFERRALS);

    /* Must read all the registers to be sure to clear the interrupt */

    (void) sysInWord (port + BYTES_RECEIVED);
    (void) sysInWord (port + BYTES_TRANSMITTED);

    sysOutWord (port + ELT_COMMAND, STATS_ENABLE);
    sysOutWord (port + ELT_COMMAND, SELECT_WINDOW | WIN_OPERATING);
    }

/*******************************************************************************
*
* eltIntMaskSet - enable specific status conditions to cause interrupts
*
* Sets bit(s) in the intMask field of the device control structure and in
* the board's "read zero mask" where a one bit enables the corresponding
* status condition to be read and to cause an interrupt.
*/

static void eltIntMaskSet
    (
    ELT_CTRL * pDrvCtrl,
    int maskBits
    )
    {
    eltIntDisable (pDrvCtrl);

    pDrvCtrl->intMask |= maskBits;
    sysOutWord (pDrvCtrl->port + ELT_COMMAND, MASK_STATUS | pDrvCtrl->intMask);

    eltIntEnable (pDrvCtrl);
    }

/*******************************************************************************
*
* eltIntEnable - enable board to cause interrupts
*
* Because the board has maskable status, this routine can simply set the
* mask to all ones.  We set all the bits symbolically; the effect is the
* same.  Note that the interrupt latch is not maskable; if none of the other
* mask bits are set, no interrupts will occur at all.  Only those interrupts
* whose status bits are enabled will actually occur.  Note that the "intMask"
* field in the device control structure is really the status mask.
*/

static void eltIntEnable
    (
    ELT_CTRL * pDrvCtrl
    )
    {
    UINT16  status;

    status = sysInByte (pDrvCtrl->port + ELT_STATUS) & 0x00ff;
    sysOutWord (pDrvCtrl->port + ELT_COMMAND, ACK_INTERRUPT | status);
    sysOutWord (pDrvCtrl->port + ELT_COMMAND, MASK_INTERRUPT |
                ADAPTER_FAILURE | TX_COMPLETE | TX_AVAILABLE | RX_COMPLETE |
                RX_EARLY | INTERRUPT_REQ | UPDATE_STATS);
    }

/*******************************************************************************
*
* eltIntDisable - prevent board from causing interrupts
*
* This routine simply sets all the interrupt mask bits to zero.
* It is intended for guarding board-critical sections.
*/

static void eltIntDisable
    (
    ELT_CTRL * pDrvCtrl
    )
    {
    UINT16  status;

    sysOutWord (pDrvCtrl->port + ELT_COMMAND, MASK_INTERRUPT | 0);
    status = sysInByte (pDrvCtrl->port + ELT_STATUS) & 0x00ff;
    sysOutWord (pDrvCtrl->port + ELT_COMMAND, ACK_INTERRUPT | status);
    }

/*******************************************************************************
*
* eltTxStart - turn on board's transmit function
*/

static void eltTxStart
    (
    ELT_CTRL *  pDrvCtrl
    )
    {
    sysOutWord (pDrvCtrl->port + ELT_COMMAND, TX_ENABLE);
    }

/*******************************************************************************
*
* eltRxStart - enable board to start receiving
*/

static void eltRxStart
    (
    ELT_CTRL *  pDrvCtrl
    )
    {
    sysOutWord (pDrvCtrl->port + ELT_COMMAND, SET_RX_FILTER |
                                              pDrvCtrl->rxFilter);
    sysOutWord (pDrvCtrl->port + ELT_COMMAND, RX_ENABLE);
    }

/*******************************************************************************
*
* eltActivate - attempt to find and activate the adapter with given address
*
* The 3Com 3C509 ISA adapter does not enable itself at power-on.  This is
* left to the driver, which presumably knows which board it wants.  This
* we do know, from the port field in the driver control structure.
*
* As a helpful side effect, this routine stores the OEM Ethernet address
* of the selected adapter into the driver control structure.
*
* Note that this procedure will activate only one board of the given I/O
* address; this is presumably designed in by 3Com as a helpful feature.
*/

static STATUS eltActivate
    (
    ELT_CTRL * pDrvCtrl
    )
    {
    int adapterPort;    /* I/O address of adapter we're to look for */
    int selectedPort;   /* I/O address of adapter we've found */
    int addressConfig;  /* adapter's address configuration register */
    int resourceConfig; /* adapter's resource configuration register */
    char nodeAddress [EA_SIZE];
    STATUS status = OK; /* presume OK, change if there's a problem */

    adapterPort = pDrvCtrl->port;

    eltIdCommand (ID_PORT);             /* wake up all adapters */
    sysOutByte (ID_PORT, ID_SET_TAG);   /* clear all tags */

    /* first see if there's a 3Com 3C5xx board out there at all */

    if (eltIdEepromRead (EE_A_MANUFACTURER) != MANUFACTURER_ID)
        return (ERROR);

    /* Identify adapters one by one until we find the right one;
     * as we eliminate adapters, we tag them so they don't participate
     * in the next round of contention eliminations.  Along the way,
     * as part of the adapter contention process, we read out the
     * station address and resource configuration.
     */

    do
        {
        eltIdCommand (ID_PORT);         /* prepare for contention */

        /* Now read all untagged adapters' addresses from EEPROM
         * a bit at a time.  Tagged adapters ignore the reads therefore
         * won't be found; we find the next untagged adapter.
         */

        * (UINT16 *) &nodeAddress [0] = eltIdEepromRead (EE_A_OEM_NODE_0);
        * (UINT16 *) &nodeAddress [2] = eltIdEepromRead (EE_A_OEM_NODE_1);
        * (UINT16 *) &nodeAddress [4] = eltIdEepromRead (EE_A_OEM_NODE_2);
        resourceConfig = eltIdEepromRead (EE_A_RESOURCE);
        addressConfig = eltIdEepromRead (EE_A_ADDRESS);
        if ( (addressConfig & AC_IO_BASE_MASK) == AC_IO_BASE_EISA)
            {

            /* the EISA base address is the last possible one; if we hit
             * this value without finding the adapter we want, we're done.
             */

            status = ERROR;
            break;
            }
        selectedPort = (addressConfig & AC_IO_BASE_MASK) * AC_IO_BASE_FACTOR +
                       AC_IO_BASE_ZERO;

#ifdef ELT_DEBUG
        printf ("elt: activate: adapter at 0x%04x\n", selectedPort);
#endif /* ELT_DEBUG */

        /* tag this adapter so if we don't want it it won't contend again */

        sysOutByte (ID_PORT, ID_SET_TAG + 1);
        }
    while (selectedPort != adapterPort);

    if (status != ERROR)
        {
        sysOutByte (ID_PORT, ID_ACTIVATE);
        uswab (nodeAddress, (char *)pDrvCtrl->idr.ac_enaddr, EA_SIZE);
        }

    return (status);
    }

/*******************************************************************************
*
* eltIdCommand - put all adapters into ID command state
*
* This procedure synchronizes the ID state machines of all installed 3Com
* adapters in preparation for contending among them.
*/

static void eltIdCommand
    (
    int idPort          /* I/O address to use as ID port (1x0 hex) */
    )
    {
    int idValue;        /* data read or written to ID port */
    int count;

    /* We should guard this routine since the ID procedure touches
     * all unactivated adapters.  In fact the first three writes should
     * be guarded against any possible intervening write to any port
     * 0x100, 0x110, 0x120, ... , 0x1f0.
     */

    sysOutByte (idPort, ID_RESET);      /* select the ID port */
    sysOutByte (idPort, ID_RESET);      /* put adapters in ID-WAIT state */
    idValue = ID_SEQUENCE_INITIAL;
    for (count = ID_SEQUENCE_LENGTH; count > 0; count--)
        {
        sysOutByte (idPort, idValue);
        idValue <<= 1;
        if ( (idValue & ID_CARRY_BIT) != 0)
            idValue ^= ID_POLYNOMIAL;
        }
    }

/*******************************************************************************
*
* eltIdEepromRead - read one 16-bit adapter EEPROM register
*
* Read an EEPROM register bitwise by way of the ID port.  Used for adapter
* contention process and to find out what's in the EEPROM.  Addresses are
* masked to a valid size; invalid addresses are simply truncated.
*/

static UINT16 eltIdEepromRead
    (
    int address                         /* EEPROM register address */
    )
    {
    int bitCount;
    UINT16 value;

    sysOutByte (ID_PORT, ID_EEPROM_READ | (address & ID_EEPROM_MASK));
    taskDelay (2);                      /* must wait for 162 uS read cycle */
    value = 0;
    for (bitCount = ID_REGISTER_SIZE; bitCount > 0; bitCount--)
        {
        value <<= 1;
        value |= sysInByte (ID_PORT) & 1;
        }
    return (value);
    }

/*******************************************************************************
*
* eltShow - display statistics for the 3C509 `elt' network interface
*
* This routine displays statistics about the `elt' Ethernet network interface.
* It has two parameters: 
* .iP <unit>
* interface unit; should be 0.
* .iP <zap>
* if 1, all collected statistics are cleared to zero.
* .LP
*
* RETURNS: N/A
*/

void eltShow 
    (
    int unit,   /* interface unit */
    BOOL zap    /* 1 = zero totals */
    )
    {
    ELT_CTRL * pDrvCtrl;
    FAST int ix;
    static char *e_message [] = {
        "collisions",
        "crcs",
        "aligns",
        "rx no buffers",
        "rx over-runs",
        "disabled",
        "deferring",
        "tx under-run",
        "aborts",
        "out-of-window",
        "heart-beats",
        "bad-packet",
        "short-packet",
        "tx-no-error",
        "rx-no-error",
        "tx-error",
        "rx-error",
        "over-write",
        "wrapped",
        "interrupts",
        "reset",
        "stray-int",
        "multiple-collisions",
        "late-collisions",
        "no-carriers",
        "jabbers",
        "tx over-run",
        "rx under-run",
#ifdef ELT_TIMING
        "rx early",
        "timer updates",
        "timer overflows",
        "timer invalids",
        "max rx latency",
        "min rx latency",
        "max int latency",
        "current outstanding net rx tasks",
        "max outstanding rx net tasks",
        "current outstanding net tx tasks",
        "max outstanding tx net tasks"
#else
        "rx early"
#endif /* ELT_TIMING */
	};

    pDrvCtrl = pEltCtrl [unit];

    /* Disable board interrupts because eltStatFlush() changes reg. window */

    eltIntDisable (pDrvCtrl);
    eltStatFlush (pDrvCtrl);            /* get the latest statistics */
    eltIntEnable (pDrvCtrl);

    for (ix = 0; ix < NELEMENTS(e_message); ix++)
        {
        printf ("    %-30.30s  %4d\n", e_message [ix],
                pDrvCtrl->eltStat.stat [ix]);
        if (zap)
            pDrvCtrl->eltStat.stat [ix] = 0;
        }
    }
