/* if_eex.c - Intel EtherExpress 16 network interface driver */

/* Copyright 1989-1997 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01k,15jul97,spm  added ARP request to SIOCSIFADDR ioctl handler
01j,07apr97,spm  corrected statistics counter for incoming packets
01i,17mar97,spm  upgraded to BSD 4.4 and corrected statistics counters.
01h,14jun95,hdn  removed function declarations defined in sysLib.h.
01g,01feb95,jdi  doc cleanup.
01f,17jan95,jag  only send connector select command if NOT auto-detect
01e,12nov94,tmk  removed some IMPORTed function decs and included unixLib.h
01d,20feb94,bcs  add attachment type selection, stub for auto-detect
01c,13dec93,bcs  rearranged initialization, fixed comments
01b,04dec93,bcs  made functional, added AL_LOC compile-time option
01a,28nov93,bcs  derived from if_ei.c (rev. 03c,15jan93)
*/

/*
DESCRIPTION
This module implements the Intel EtherExpress 16 PC network interface card
driver.  It is specific to that board as used in PC 386/486 hosts.
This driver is written using the device's I/O registers exclusively.


SIMPLIFYING ASSUMPTIONS
This module assumes a little-endian host (80x86); thus, no endian
adjustments are needed to manipulate the 82586 data structures (little-endian).

The on-board memory is assumed to be sufficient; thus, no provision is made
for additional buffering in system memory.

The "frame descriptor" and "buffer descriptor" structures can be bound
into permanent pairs by pointing each FD at a "chain" of one BD of MTU size.
The 82586 receive algorithm fills exactly one BD for each FD; it looks to
the NEXT FD in line for the next BD.

The transmit and receive descriptor lists are permanently linked into
circular queues partitioned into sublists designated by the EEX_LIST
headers in the driver control structure.  Empty partitions have NULL pointer
fields.  EL bits are set as needed to tell the 82586 where a partition ends.
The lists are managed in strict FIFO fashion; thus the link fields are never
modified, just ignored if a descriptor is at the end of a list partition.


BOARD LAYOUT
This device is soft-configured.  No jumpering diagram is required.


EXTERNAL INTERFACE
This driver provides the standard external interface with the following
exceptions.  All initialization is performed within the attach routine and
there is no separate initialization routine.  Therefore, in the global interface
structure, the function pointer to the init() routine is NULL.

There is one user-callable routine, eexattach().  For details on usage,
see the manual entry for this routine.


EXTERNAL SUPPORT REQUIREMENTS
None.


SYSTEM RESOURCE USAGE
    - one mutual exclusion semaphore
    - one interrupt vector
    - one watchdog timer.
    - 8 bytes in the initialized data section (data)
    - 912 bytes in the uninitialized data section (bss)

The data and bss sections are quoted for the MC68020 architecture and may vary
for other architectures.  The code size (text) will vary widely between
architectures, and is thus not quoted here.

The device contains on-board buffer memory; no system memory is required
for buffering.


TUNING HINTS
The only adjustable parameter is the number of TFDs to create in adapter buffer
memory.  The total number of TFDs and RFDs is 21, given full-frame buffering
and the sizes of the auxiliary structures.  eexattach() requires at least
MIN_NUM_RFDS RFDs to exist.  More than ten TFDs is not sensible
in typical circumstances.


SEE ALSO: ifLib
*/

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
#include "net/unixLib.h"
#include "sys/times.h"
#include "drv/netif/if_eex.h"
#include "net/if_subr.h"


/***** LOCAL DEFINITIONS *****/

#undef EEX_DEBUG                             /* Compiles debug output */
#undef EEX_DEBUG1  /* dhe 10/24/94 */

#define MAX_UNITS       4                   /* maximum units to support */
#define DEF_NUM_TFDS    8                   /* default number of TFDs */
#define MIN_NUM_RFDS    4                   /* minimum number of RFDs */

#define MAX_IRQ         15              /* Highest PC IRQ number */

#define EEX_MEMSIZE     0x8000          /* Size of EtherExpress memory */
                                        /* Production units are 32K */
#define SCP_OFFSET      0xfff6          /* At top of 64K address space */
                                        /* (aliased in a 32K board) */
#define ISCP_OFFSET     0x0000          /* EtherExpress convention */
#define SCB_OFFSET      0x0008          /* EtherExpress convention */
                                        /* Supported by hardware feature */
#define CFD_OFFSET      ( SCB_OFFSET + sizeof (SCB) )
#define TFD_OFFSET      ( CFD_OFFSET + sizeof (CFD) )

/* These should be in a common file but which one? */
/* Ethernet attachment type selections */

#define ATTACHMENT_DEFAULT      0       /* use card as configured */
#define ATTACHMENT_AUI          1       /* AUI  (thick, DIX, DB-15) */
#define ATTACHMENT_BNC          2       /* BNC  (thin, 10BASE-2) */
#define ATTACHMENT_RJ45         3       /* RJ-45 (twisted pair, TPE, 10BASE-T)*/

/* Typedefs for external structures that are not typedef'd in their .h files */

typedef struct mbuf MBUF;
typedef struct arpcom IDR;                  /* Interface Data Record wrapper */
typedef struct ifnet IFNET;                 /* real Interface Data Record */
typedef struct sockaddr SOCK;

/* The definition of our linked list management structure */

typedef struct eex_list                     /* EEX_LIST - 82586 queue head */
    {
    volatile EEX_SHORTLINK  head;       /* header of list */
    volatile EEX_SHORTLINK  tail;       /* tail of list */
    } EEX_LIST;

/* Macro to follow TFRAME and RFRAME (TFD/RFD) lists */

#define SUCC_FD(fd)     ( \
                sysOutWord (pDrvCtrl->port + RDPTR, (fd) + RF_LINK), \
                sysInWord (pDrvCtrl->port + DXREG) )

/* The definition of the driver control structure */

typedef struct drv_ctrl
    {
    IDR                 idr;        /* interface data record */

    BOOL                attached;   /* indicates attach() called */
    CFD                 *pCfd;      /* Scratch CFD to copy to board mem.*/
    EEX_SHORTLINK       rfdBase;    /* RFD pool starts here */
    EEX_SHORTLINK       lastFd;     /* last RFD dhe 10/26/94 */
    volatile EEX_LIST   rxQueue;    /* receive queue */
    volatile EEX_SHORTLINK freeRfd; /* first free receive frame */
    volatile EEX_LIST   tfdQueue;   /* free transmit frame descriptors */
    volatile EEX_LIST   txQueue;    /* to be sent queue */
    volatile EEX_LIST   cblQueue;   /* actual chip transmit queue */
    volatile EEX_LIST   cleanQueue; /* queue of TFDs to cleanup */
    volatile BOOL       rcvHandling;/* flag, indicates netTask active */
    volatile BOOL       txCleaning; /* flag, indicates netTask active */
    volatile BOOL       txIdle;     /* flag, indicates idle transmitter */
    int                 nTFDs;      /* how many TFDs to create */
    int                 nRFDs;      /* how many RFDs to create */
    int                 port;       /* base I/O port */
    int                 ivec;       /* interrupt vector */
    int                 intLevel;   /* interrupt number */
    int                 irqCode;    /* encoded value written to IRQ port */
    int                 attachment; /* connector type: BNC, AUI, or TPE */
    UINT16              setup;      /* base, boot ROM, I/O, AUI/BNC, IRQ */
    UINT16              tpeBit;     /* bit 0 = 1 && setup AUI bit = 0 for TPE */
    MEM_SETUP           memSetup;   /* everything to set up memory mapping */
    WDOG_ID             wid;        /* watchdog timer for transmit */
    long                transLocks; /* count for transmit lockup failures */
    } DRV_CTRL;

#define DRV_CTRL_SIZ  sizeof(DRV_CTRL)


/***** GLOBALS *****/


/***** LOCALS *****/

/* The array of driver control structs */

LOCAL DRV_CTRL drvCtrl [MAX_UNITS];

/* Encoded IRQ selection numbers for EtherExpress 16 IRQ Control port
 * Entries correspond to PC IRQ numbers 0-15.  A zero entry means the
 * corresponding IRQ cannot be used by the board; a non-zero entry is
 * the value needed in the board's interrupt control register to program
 * the board for the corresponding IRQ.
 */

LOCAL int irqTable [MAX_IRQ + 1] =
    {
    0, 0, 1, 2, 3, 4, 0, 0, 0, 1, 5, 6, 0, 0, 0, 0
    };

/* following are used only in netTask() context so can be common */

LOCAL CFD       Cfd;                    /* expedient for code porting */
                                        /* build commands here then copy */
                                        /* to board memory */
LOCAL char      buffer [FRAME_SIZE];    /* Copy frames through here */

/* forward function declarations */

static void eexReset (int unit);
static int eexIoctl (IDR *pIDR, int cmd, caddr_t data);
#ifdef BSD43_DRIVER
static int eexOutput (IDR *pIDR, MBUF *pMbuf, SOCK *pDestAddr);
static void eexTxStartup (int unit);
#else
static void eexTxStartup (DRV_CTRL *pDrvCtrl);
#endif
static void eexInt (DRV_CTRL *pDrvCtrl);
static void eexTxCleanQ (DRV_CTRL *pDrvCtrl);
static void eexHandleRecvInt (DRV_CTRL *pDrvCtrl);
static STATUS eexReceive (DRV_CTRL *pDrvCtrl, EEX_SHORTLINK rfdOffset);
static void eexDiag (int unit);
static void eexConfig (int unit);
static void eexIASetup (int unit);
static void eexRxStartup (DRV_CTRL *pDrvCtrl);
static void eexAction (int unit, UINT16 action);
static STATUS eexCommand (DRV_CTRL *pDrvCtrl, UINT16 cmd);

static void eexTxQPut (DRV_CTRL *pDrvCtrl, EEX_SHORTLINK tfdOffset);
static void eexTxQFlush (DRV_CTRL *pDrvCtrl);
static void eexRxQPut (DRV_CTRL *pDrvCtrl, EEX_SHORTLINK rfdOffset);
static EEX_SHORTLINK eexRxQGet (DRV_CTRL *pDrvCtrl);
static BOOL eexRxQFull (DRV_CTRL *pDrvCtrl);
static void eexQInit (EEX_LIST *pHead);
static EEX_SHORTLINK eexQGet (DRV_CTRL *pDrvCtrl, EEX_LIST *pQueue);
static void eexQPut (DRV_CTRL *pDrvCtrl, EEX_LIST *pQueue, EEX_SHORTLINK pNode);
static STATUS eexDeviceStart (int unit);
static void eexWatchDog(int unit);
static STATUS eexEnetAddrGet (DRV_CTRL *pDrvCtrl, char addr[]);
static STATUS eex586Init (int unit);
static void   eex586IntAck (int unit);
static void   eex586IntEnable (int unit);
static void   eex586IntDisable (int unit);
static void   eex586SetReset (int unit);
static void   eex586ClearReset (int unit);
static void   eex586ChanAtn (DRV_CTRL *pDrvCtrl);
static UINT16 eexReadEEPROM (DRV_CTRL *pDRvCtrl, UINT16 address);
static void eexOutBitsEEPROM (int port, int count, UINT16 bits);
static UINT16 eexInBitsEEPROM (int port, int count);
static void   eexDelayEEPROM (void);

static void DUMMY ()
{}


/*******************************************************************************
*
* eexattach - publish the `eex' network interface and initialize the driver and device
*
* The routine publishes the `eex' interface by filling in a network interface
* record and adding this record to the system list.  This routine also
* initializes the driver and the device to the operational state.
*
* RETURNS: OK or ERROR.
*
* SEE ALSO: ifLib
*/

STATUS eexattach
    (
    int         unit,                   /* unit number */
    int         port,                   /* base I/O address */
    int         ivec,                   /* interrupt vector number */
    int         ilevel,                 /* interrupt level */
    int         nTfds,                  /* # of transmit frames (0=default) */
    int         attachment              /* 0=default, 1=AUI, 2=BNC, 3=TPE */
    )
    {
    DRV_CTRL    *pDrvCtrl;
    long        size;                   /* temporary size holder */

    /* Sanity check the unit number */

    if (unit < 0 || unit >= MAX_UNITS)
        return (ERROR);

    /* Ensure single invocation per system life */

    pDrvCtrl = & drvCtrl [unit];

#ifdef EEX_DEBUG /* dhe 10/20/94 */
    logMsg ("eex: eeattach: pDrvCtrl=%x\n", pDrvCtrl, 0, 0, 0, 0, 0);
#endif

    if (pDrvCtrl->attached)
        return (OK);

    /* Check interrupt number for sanity and usability */
    if (ilevel < 0 || ilevel > MAX_IRQ)
        return (ERROR);

    pDrvCtrl->intLevel = ilevel;
    pDrvCtrl->irqCode = irqTable [ilevel];
    if (pDrvCtrl->irqCode == 0)
        return (ERROR);

    /* Determine number of Tx and Rx descriptors to use */

    pDrvCtrl->nTFDs = (nTfds != 0) ? nTfds : DEF_NUM_TFDS;

    /* Determine which connector is desired */

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
    /* Publish the interface record */

#ifdef BSD43_DRIVER
    ether_attach    ( (IFNET *) & pDrvCtrl->idr, unit, "eex", (FUNCPTR) NULL,
                     (FUNCPTR) eexIoctl, (FUNCPTR) eexOutput,
                     (FUNCPTR) eexReset);
#else
    ether_attach    (
                    (IFNET *) & pDrvCtrl->idr,
                    unit,
                    "eex",
                    (FUNCPTR) NULL,
                    (FUNCPTR) eexIoctl,
                    (FUNCPTR) ether_output,
                    (FUNCPTR) eexReset
                    );
    pDrvCtrl->idr.ac_if.if_start = (FUNCPTR)eexTxStartup;
#endif

    /* Calculate the total size of 82586 memory pool.
     * Evaluate as a long so it can force nRFDs negative if way too many
     * TFDs were asked for.
     */

    size =
            sizeof (SCP) +
            sizeof (ISCP) +
            sizeof (SCB) +
            sizeof (CFD) +                      /* synch'ed command frame */
            (sizeof (TFRAME) * pDrvCtrl->nTFDs);   /* pool of transmit frames */
    pDrvCtrl->nRFDs = ( (long)EEX_MEMSIZE - size ) / sizeof (RFRAME);

#ifdef EEX_DEBUG /* dhe 10/20/94 */
    logMsg ("eex: eeattach: nRFDS=%x\n", pDrvCtrl->nRFDs, 0, 0, 0, 0, 0);
#endif

    if (pDrvCtrl->nRFDs < MIN_NUM_RFDS)
        return (ERROR);

    /* Save other passed-in parameters */

    pDrvCtrl->port = port;                      /* base I/O address */
    pDrvCtrl->ivec = ivec;                      /* interrupt vector */

    pDrvCtrl->pCfd      = &Cfd;

    /* Init the watchdog that will patrol for device lockups */

    pDrvCtrl->transLocks = 0;
    pDrvCtrl->wid = wdCreate ();                      /* create watchdog */
    if (pDrvCtrl->wid == NULL)                        /* no resource */
        goto error;

    pDrvCtrl->rcvHandling   = FALSE;  /* netTask not running our receive job */
    pDrvCtrl->txCleaning    = FALSE;  /* netTask not running our clean job */
    pDrvCtrl->txIdle        = TRUE;         /* transmitter idle */

    /* Perform device initialization */

    if (eex586Init (unit) != OK)
        goto error;

    sysIntEnablePIC (ilevel);

    /* Set our flag */

    pDrvCtrl->attached = TRUE;

    /* Set status flags in IDR */

    pDrvCtrl->idr.ac_if.if_flags |= (IFF_UP | IFF_RUNNING | IFF_NOTRAILERS);

    /* Successful return */

    return (OK);

    /***** Handle error cases *****/

    error:

        {
        /* Release watchdog */

        if (pDrvCtrl->wid != NULL)
            wdDelete (pDrvCtrl->wid);

        return (ERROR);
        }
    }

/*******************************************************************************
*
* eexReset - reset the eex interface
*
* Mark interface as inactive and reset the chip.
*/

static void eexReset
    (
    int unit
    )
    {
    DRV_CTRL *pDrvCtrl;

    pDrvCtrl = & drvCtrl [unit];

    pDrvCtrl->idr.ac_if.if_flags = 0;
    eex586IntDisable (unit);                    /* disable ints from the dev */
    eex586SetReset (unit);                      /* put the 586 chip in reset */

    }

/*******************************************************************************
*
* eexIoctl - interface ioctl procedure
*
* Process an interface ioctl request.
*/

static int eexIoctl
    (
    IDR     *pIDR,
    int     cmd,
    caddr_t data
    )
    {
    int error;

#ifdef EEX_DEBUG
    printf ("eex: ioctl: pIDR=%x cmd=%x data=%x\n", pIDR, cmd, data);
#endif /* EEX_DEBUG */

    error = 0;

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

#ifdef BSD43_DRIVER
/*******************************************************************************
*
* eexOutput - interface output routine.
*
* This is the entry point for packets arriving from protocols above.  This
* routine merely calls our specific output routines that eventually lead
* to a transmit to the network.
*
* RETURNS: 0 or appropriate errno
*/

static int eexOutput
    (
    IDR *    pIDR,
    MBUF *     pMbuf,
    SOCK * pDestAddr
    )
    {

#ifdef EEX_DEBUG
    printf ("eex: output: pIDR=%x pMbuf=%x pDestAddr=%x\n",
            pIDR, pMbuf, pDestAddr);
#endif /* EEX_DEBUG */

    return (ether_output ( (IFNET *)pIDR, pMbuf, pDestAddr,
            (FUNCPTR) eexTxStartup, pIDR));
    }
#endif

/*******************************************************************************
*
* eexTxStartup - start output on the chip
*
* Looks for any action on the queue, and begins output if there is anything
* there. This routine is called from several possible threads. Each will be
* described below.
*
* The first, and most common thread, is when a user task requests the
* transmission of data. Under BSD 4.3, this will cause eexOutput() to be 
* called, which will cause ether_output() to be called, which will cause 
* this routine to be called (usually). This routine will not be called if 
* ether_output() finds that our interface output queue is full. In this case, 
* the outgoing data will be thrown out. BSD 4.4 uses a slightly different
* model in which the generic ether_output() routine is called directly, 
* followed by a call to this routine.
*
* The second, and most obscure thread, is when the reception of certain
* packets causes an immediate (attempted) response. For example, ICMP echo
* packets (ping), and ICMP "no listener on that port" notifications. All
* functions in this driver that handle the reception side are executed in the
* context of netTask(). Always. So, in the case being discussed, netTask() will
* receive these certain packets, cause IP to be stimulated, and cause the
* generation of a response to be sent. We then find ourselves following the
* thread explained in the second example, with the important distinction that
* the context is that of netTask().
*
* The third thread occurs when this routine runs out of TFDs and returns. If
* this occurs when our output queue is not empty, this routine would typically
* not get called again until new output was requested. Even worse, if the
* output queue was also full, this routine would never get called again and
* we would have a lock state. It DOES happen. To guard against this, the
* transmit clean-up handler detects the out-of-TFDs state and calls this
* function. The clean-up handler also runs from netTask.
*
* Note that this function is ALWAYS called between an splnet() and an splx().
* This is true because netTask(), and ether_output() take care of
* this when calling this function. Therefore, no calls to these spl functions
* are needed anywhere in this output thread.
*/

#ifdef BSD43_DRIVER
static void eexTxStartup
    (
    int unit
    )
    {
    DRV_CTRL * pDrvCtrl;

    pDrvCtrl = & drvCtrl [unit];
#else
static void eexTxStartup
    (
    DRV_CTRL * 	pDrvCtrl
    )
    {
#endif
    MBUF * pMbuf;
    int length;
    EEX_SHORTLINK tfdOffset;

    /*
     * Loop until there are no more packets ready to send or we
     * have insufficient resources left to send another one.
     */

    while (pDrvCtrl->idr.ac_if.if_snd.ifq_head)
        {
        IF_DEQUEUE (&pDrvCtrl->idr.ac_if.if_snd, pMbuf);  /* dequeue a packet */

        copy_from_mbufs (buffer, pMbuf, length);
        length = max (ETHERSMALL, length);

        if ( (etherOutputHookRtn != NULL) &&
            (* etherOutputHookRtn)
            (&pDrvCtrl->idr, buffer, length))
            continue;

        /* get free tfd */
        tfdOffset = eexQGet (pDrvCtrl, (EEX_LIST *)&pDrvCtrl->tfdQueue);

        if (tfdOffset == NULL)
            break;                              /* out of TFD's */

        /* copy header and data to contiguous board memory areas */

        sysOutWord (pDrvCtrl->port + WRPTR, tfdOffset + TF_BUFFER);
        sysOutWordString (pDrvCtrl->port + DXREG, (short *)buffer,
                          (length + 1) / 2);
#ifndef EEX_AL_LOC
        length -= EH_SIZE;              /* compensate for header not in data */
#endif  /* EEX_AL_LOC */
        sysOutWord (pDrvCtrl->port + WRPTR, tfdOffset + TF_ACT_COUNT);
        sysOutWord (pDrvCtrl->port + DXREG,
                    (length & ACT_COUNT_MASK) | TBD_S_EOF);

#ifndef EEX_AL_LOC
        /* Move length/ethertype up to where 82586 expects it following dest. */

        sysOutWord (pDrvCtrl->port + RDPTR, tfdOffset + TF_OLDLENGTH);
        length = sysInWord (pDrvCtrl->port + DXREG);

        /* separate read and write because both pointers increment */
        /* on any access to DXREG */

        sysOutWord (pDrvCtrl->port + WRPTR, tfdOffset + TF_NEWLENGTH);
        sysOutWord (pDrvCtrl->port + DXREG, length);
#endif  /* EEX_AL_LOC */

#ifndef BSD43_DRIVER    /* BSD 4.4 ether_output() doesn't bump statistic. */
        pDrvCtrl->idr.ac_if.if_opackets++;
#endif

        eexTxQPut (pDrvCtrl, tfdOffset);
        }
    }

/*******************************************************************************
*
* eexInt - entry point for handling interrupts from the 82586
*
* The interrupting events are acknowledged to the device, so that the device
* will deassert its interrupt signal. The amount of work done here is kept
* to a minimum; the bulk of the work is defered to the netTask. Several flags
* are used here to synchronize with task level code and eliminate races.
*/

static void eexInt
    (
    DRV_CTRL *pDrvCtrl
    )
    {
    UINT16  event;
    int unit;

    unit = pDrvCtrl->idr.ac_if.if_unit;
    sysBusIntAck (pDrvCtrl->intLevel);

    event = sysInWord (pDrvCtrl->port + SCB_STATUS) & (SCB_S_CX | SCB_S_FR | SCB_S_CNA | SCB_S_RNR);

#ifdef EEX_DEBUG
    logMsg ("eex: interrupt: event=%x\n", event, 0, 0, 0, 0, 0);
#endif /* EEX_DEBUG */

    eexCommand (pDrvCtrl, event);                        /* ack the events */

    eex586IntAck (unit);                                /* ack 586 interrupt */

    /* Handle transmitter interrupt */

    if (event & SCB_S_CNA)                              /* reclaim tx tfds */
        {

        wdCancel(pDrvCtrl->wid);                /* canus delenda est */

        if (pDrvCtrl->cleanQueue.head == NULL)      /* clean queue empty */
            {
            pDrvCtrl->cleanQueue.head = pDrvCtrl->cblQueue.head; /* new head */
            pDrvCtrl->cleanQueue.tail = pDrvCtrl->cblQueue.tail; /* new tail */
            }
        else                                            /* concatenate queues */
            {
            pDrvCtrl->cleanQueue.tail = pDrvCtrl->cblQueue.tail;
            }

        if (!pDrvCtrl->txCleaning)                          /* not cleaning? */
            {
            pDrvCtrl->txCleaning = TRUE;                    /* set flag */
            netJobAdd ( (FUNCPTR) eexTxCleanQ, (int) pDrvCtrl, 0, 0, 0, 0);
                            /* defer cleanup */
            }

        if (pDrvCtrl->txQueue.head != NULL)             /* anything to flush? */
            eexTxQFlush (pDrvCtrl);                      /* flush the tx q */
        else
            pDrvCtrl->txIdle = TRUE;                    /* transmitter idle */
        }

    /* Handle receiver interrupt */

    if ( (event & SCB_S_FR) && !(pDrvCtrl->rcvHandling))
        {
        pDrvCtrl->rcvHandling = TRUE;
        (void) netJobAdd ( (FUNCPTR) eexHandleRecvInt, (int) pDrvCtrl,0, 0, 0, 0);         /* netTask processes */
        }
         

    /* dhe 10/20/94 could lose some packets here */
    if ( (event & SCB_S_RNR) && !(pDrvCtrl->rcvHandling))
        eexRxStartup(pDrvCtrl);

    } 

/*******************************************************************************
*
* eexTxCleanQ - checks errors in completed TFDs and moves TFDs to free queue
*
* This routine is executed by netTask. It "cleans" the TFDs on the clean-up
* queue by checking each one for errors and then returning the TFD to the
* "free TFD" queue. The startup routine is sometimes called here to eliminate
* the lock-out case where the driver input queue is full but there are no
* TFDs available.
*/

static void eexTxCleanQ
    (
    DRV_CTRL *pDrvCtrl
    )
    {
    EEX_SHORTLINK tfdOffset;
    UINT16 status;
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

            eex586IntDisable (unit);
            tfdOffset = eexQGet (pDrvCtrl, (EEX_LIST *)&pDrvCtrl->cleanQueue);
            eex586IntEnable (unit);
            if (tfdOffset == NULL)
                break;

            sysOutWord (pDrvCtrl->port + RDPTR, tfdOffset);
            status = sysInWord (pDrvCtrl->port + DXREG);

            pDrvCtrl->idr.ac_if.if_collisions +=         /* add any colls */
                (status & CFD_S_RETRY) ? 16 :  /* excessive colls */
                (status & CFD_S_COLL_MASK);    /* some colls */

            if (!(status & CFD_S_OK))          /* packet not sent */
                {
                pDrvCtrl->idr.ac_if.if_oerrors++;        /* incr err count */
                pDrvCtrl->idr.ac_if.if_opackets--;       /* decr sent count */
                }

            /* return to tfdQ */
            eexQPut (pDrvCtrl, (EEX_LIST *)&pDrvCtrl->tfdQueue, tfdOffset);
            }

        if (needTxStart)                                  /* check flag */
#ifdef BSD43_DRIVER
            eexTxStartup (unit);
#else
            eexTxStartup (pDrvCtrl);
#endif
        pDrvCtrl->txCleaning = FALSE;
        }
    while (pDrvCtrl->cleanQueue.head != NULL);            /* check again */
    }

/*******************************************************************************
*
* eexHandleRecvInt - task level interrupt service for input packets
*
* This routine is called at task level indirectly by the interrupt
* service routine to do any message received processing.
*/

static void eexHandleRecvInt
    (
    DRV_CTRL *pDrvCtrl
    )
    {
    EEX_SHORTLINK rfdOffset;

    do
        {
        pDrvCtrl->rcvHandling = TRUE;             /* interlock with eexInt() */

        while ( (rfdOffset = eexRxQGet (pDrvCtrl)) != NULL)
            (void) eexReceive (pDrvCtrl, rfdOffset);

        pDrvCtrl->rcvHandling = FALSE;            /* interlock with eexInt() */
        }
    while (eexRxQFull (pDrvCtrl));              /* make sure rx q still empty */
    }

/*******************************************************************************
*
* eexReceive - pass a received frame to the next layer up
*
* Strips the Ethernet header and passes the packet to the appropriate
* protocol.
* Is responsible for proper disposition of frame buffer in all cases.
*/

static STATUS eexReceive
    (
    DRV_CTRL *pDrvCtrl,
    EEX_SHORTLINK rfdOffset
    )
    {
    struct ether_header * 	pEh;
    u_char * 	pData;
    int         len;
#ifdef BSD43_DRIVER
    UINT16      etherType;
#endif
    MBUF * 	m = NULL;
    UINT16      status;


    /* Check packet for errors.  This should be completely unnecessary,
     * but since Intel does not seem capable of explaining the exact
     * functioning of the 'save bad frames' config bit, we will look for
     * errors.
     */

    sysOutWord (pDrvCtrl->port + RDPTR, rfdOffset);
    status = sysInWord (pDrvCtrl->port + DXREG);

    if  (
        ( status & ( RFD_S_OK | RFD_S_COMPLETE ) ) !=
        ( RFD_S_OK | RFD_S_COMPLETE )
        )
        {
        ++pDrvCtrl->idr.ac_if.if_ierrors;            /* bump error counter */
#ifdef EEX_DEBUG1 /* dhe 10/20/94 */
    logMsg ("case 1: status=%x,offset=%x\n", status, rfdOffset, 0, 0, 0, 0);
#endif
        eexRxQPut (pDrvCtrl, rfdOffset);             /* free the RFD */
        return (ERROR);
        }

    /* get frame length */

    sysOutWord (pDrvCtrl->port + RDPTR, rfdOffset + RF_ACT_COUNT);
    len = sysInWord (pDrvCtrl->port + DXREG) & ACT_COUNT_MASK;
#ifndef EEX_AL_LOC
    len += EH_SIZE;
#endif  /* EEX_AL_LOC */

    sysOutWord (pDrvCtrl->port + RDPTR, rfdOffset + RF_BUFFER);
    sysInWordString (pDrvCtrl->port + DXREG, (short *) buffer, (len + 1) / 2);
    pEh = (struct ether_header *)buffer;    /* get ptr to ethernet header */

    /* Bump input packet counter. */

    ++pDrvCtrl->idr.ac_if.if_ipackets;

    /* Service input hook */

    if ( (etherInputHookRtn != NULL) )
        {
        if  ( (* etherInputHookRtn) (&pDrvCtrl->idr, (char *)pEh, len) )
            {
#ifdef EEX_DEBUG1 /* dhe 10/20/94 */
    logMsg ("case 2: status=%x, offset=%x\n", status, rfdOffset, 0, 0, 0, 0);
#endif
            eexRxQPut (pDrvCtrl, rfdOffset);             /* free the RFD */
            return (OK);
            }
        }

    len -= EH_SIZE;
    pData = (u_char *) buffer + EH_SIZE;
#ifdef BSD43_DRIVER
    etherType = ntohs (pEh->ether_type);
#endif

    m = copy_to_mbufs (pData, len, 0, &pDrvCtrl->idr);

    if (m != NULL)
#ifdef BSD43_DRIVER
        do_protocol_with_type (etherType, m, &pDrvCtrl->idr, len);
#else
        do_protocol (pEh, m, &pDrvCtrl->idr, len);
#endif
    else
        ++pDrvCtrl->idr.ac_if.if_ierrors;    /* bump statistic */

#ifdef EEX_DEBUG1 /* dhe 10/20/94 */
    logMsg ("case 3: status=%x, offset=%x\n", status, rfdOffset, 0, 0, 0, 0);
#endif
    eexRxQPut (pDrvCtrl, rfdOffset);             /* free the RFD */

    return (OK);
    }

/*******************************************************************************
*
* eexDeviceStart - reset and start the device
*
* This routine assumes interrupts from the device have been disabled, and
* that the driver control structure has been initialized.
*/

static STATUS eexDeviceStart
    (
    int unit                              /* physical unit number */
    )
    {
    DRV_CTRL *pDrvCtrl;

    /* Get pointers */

    pDrvCtrl = & drvCtrl [unit];

    /* Ensure the 82586 is in its reset state */

    eex586SetReset (unit);

    /* Initialize the SCP */

    sysOutWord (pDrvCtrl->port + WRPTR, SCP_OFFSET);
    sysOutWord (pDrvCtrl->port + DXREG, 0);     /* sysbus is 16-bit */
    sysOutWord (pDrvCtrl->port + DXREG, 0);
    sysOutWord (pDrvCtrl->port + DXREG, 0);
    sysOutWord (pDrvCtrl->port + DXREG, ISCP_OFFSET);
    sysOutWord (pDrvCtrl->port + DXREG, 0);

    /* Initialize the ISCP */

    sysOutWord (pDrvCtrl->port + WRPTR, ISCP_OFFSET);
    sysOutWord (pDrvCtrl->port + DXREG, 1);     /* Set ISCP busy bit */
    sysOutWord (pDrvCtrl->port + DXREG, SCB_OFFSET);
    sysOutWord (pDrvCtrl->port + DXREG, 0);     /* SCB is zero-based */
    sysOutWord (pDrvCtrl->port + DXREG, 0);

    /* Initialize the SCB */

    /* WRPTR is at the SCB following ISCP initialization */
    sysOutWord (pDrvCtrl->port + DXREG, 0);
    sysOutWord (pDrvCtrl->port + DXREG, 0);
    sysOutWord (pDrvCtrl->port + DXREG, 0);
    sysOutWord (pDrvCtrl->port + DXREG, 0);
    sysOutWord (pDrvCtrl->port + DXREG, 0);
    sysOutWord (pDrvCtrl->port + DXREG, 0);
    sysOutWord (pDrvCtrl->port + DXREG, 0);
    sysOutWord (pDrvCtrl->port + DXREG, 0);

    /* Tell the device to read the SCP */

    eex586ClearReset (unit);
    eex586ChanAtn (pDrvCtrl);

    /*
     * The device will now read our SCP and ISCP. It will clear the busy
     * flag in the ISCP.
     */

    taskDelay (50);
    sysOutWord (pDrvCtrl->port + RDPTR, ISCP_OFFSET);
    if ( sysInByte (pDrvCtrl->port + DXREG) == 1 )
        {
        printf ("\neex: device did not initialize\n");
        return (ERROR);
        }

    return (OK);
    }

/*******************************************************************************
*
* eexDiag - format and issue a diagnostic command
*/

static void eexDiag
    (
    int unit
    )
    {
    DRV_CTRL *pDrvCtrl;

    pDrvCtrl = & drvCtrl [unit];

    bzero ( (char *)pDrvCtrl->pCfd, sizeof (CFD));      /* zero command frame */

    eexAction (unit, CFD_C_DIAG);                /* run diagnostics */

    if (!(pDrvCtrl->pCfd->cfdStatus & CFD_S_OK))
        printErr ("eexDiag: i82586 diagnostics failed.\n");
    }

/*******************************************************************************
*
* eexConfig - format and issue a config command
*/

static void eexConfig
    (
    int unit
    )
    {
    DRV_CTRL *pDrvCtrl;

    pDrvCtrl = & drvCtrl [unit];

    bzero ( (char *)pDrvCtrl->pCfd, sizeof (CFD));      /* zero command frame */

    /* Recommended i82586 User's Manual configuration values. */

    pDrvCtrl->pCfd->cfdConfig.byteCount         = 12;
    pDrvCtrl->pCfd->cfdConfig.fifoLimit         = 0x08;
    pDrvCtrl->pCfd->cfdConfig.srdy_saveBad      = 0x40;

#ifdef  EEX_AL_LOC
    pDrvCtrl->pCfd->cfdConfig.addrLen_loopback  = 0x2e;
#else
    pDrvCtrl->pCfd->cfdConfig.addrLen_loopback  = 0x26;
#endif  /* EEX_AL_LOC */
    pDrvCtrl->pCfd->cfdConfig.backoff           = 0x00;
    pDrvCtrl->pCfd->cfdConfig.interframe        = 0x60;
    pDrvCtrl->pCfd->cfdConfig.slotTimeLow       = 0x00;
    pDrvCtrl->pCfd->cfdConfig.slotTimeHi_retry  = 0xf2;
    pDrvCtrl->pCfd->cfdConfig.promiscuous       = 0x00; /* promiscuous off */
    pDrvCtrl->pCfd->cfdConfig.carrier_collision = 0x00;
    pDrvCtrl->pCfd->cfdConfig.minFrame          = 0x40;

    eexAction (unit, CFD_C_CONFIG);          /* configure the chip */
    }

/*******************************************************************************
*
* eexIASetup - format and issue an interface address command
*/

static void eexIASetup
    (
    int unit
    )
    {
    DRV_CTRL *pDrvCtrl;

    pDrvCtrl = & drvCtrl [unit];

    bzero ( (char *)pDrvCtrl->pCfd, sizeof (CFD));      /* zero command frame */

    bcopy   (
            (char *)pDrvCtrl->idr.ac_enaddr,
            (char *)pDrvCtrl->pCfd->cfdIASetup.ciAddress,
            6
            );

    eexAction (unit, CFD_C_IASETUP);         /* set up the address */
    }

/*******************************************************************************
*
* eexRxStartup - start up the Receive Unit
*
* Starts up the Receive Unit.  Assumes that the receive structures are set up.
*/

static void eexRxStartup
    (
    DRV_CTRL *pDrvCtrl
    )
    {

    if (sysInWord (pDrvCtrl->port + SCB_STATUS) & SCB_S_RUREADY)
        /* already running */
        return;

    /* zero the status and EL bit of the current queue tail dhe 10/26/94 */
    sysOutWord(pDrvCtrl->port + WRPTR, pDrvCtrl->rxQueue.tail);
    sysOutWord(pDrvCtrl->port + DXREG, 0);
    sysOutWord(pDrvCtrl->port + DXREG, 0);

    /* set up head and tail to point to original list  dhe 10/26/94 */
    pDrvCtrl->rxQueue.head = pDrvCtrl->rfdBase;
    pDrvCtrl->rxQueue.tail = pDrvCtrl->lastFd;

    /* Point receive area to list of free frames */
    sysOutWord (pDrvCtrl->port + SCB_RFA, pDrvCtrl->freeRfd);
    
    /*
     * need to rebuild the rfd list here--copy code from initialization
     * routine.  set queue.head and queue.tail.
     */


    eex586IntDisable (pDrvCtrl->idr.ac_if.if_unit);

    if ( (sysInWord (pDrvCtrl->port + SCB_STATUS) & SCB_S_RUMASK)
        != SCB_S_RUIDLE)
        eexCommand (pDrvCtrl, SCB_C_RUABORT);            /* abort if not idle */

    eexCommand (pDrvCtrl, SCB_C_RUSTART);              /* start receive unit */
    eex586IntEnable (pDrvCtrl->idr.ac_if.if_unit);

    }

/*******************************************************************************
*
* eexAction - execute the specified action with the CFD pointed to in pDrvCtrl
*
* Do the command contained in the CFD synchronously, so that we know
* it's complete upon return.  This routine assumes that interrupts from the
* device have been disabled.
*/

static void eexAction
    (
    int    unit,
    UINT16 action
    )
    {
    CFD      *pCfd;
    DRV_CTRL *pDrvCtrl;

    pDrvCtrl = & drvCtrl [unit];
    pCfd = pDrvCtrl->pCfd;      /* Building command in scratch area */

    while (1)                   /* wait for idle command unit */
        {
        if ( (sysInWord (pDrvCtrl->port + SCB_STATUS) & SCB_S_CUMASK)
            == SCB_S_CUIDLE)
            break;
        }

    { /* Prepare and issue the command to the device */

    /* fill in CFD */

    pCfd->cfdStatus  = 0;                       /* clear status */
    pCfd->cfdCommand = CFD_C_EL | action;       /* fill in action */

    /* and the SCB */

    sysOutWord (pDrvCtrl->port + SCB_COMMAND,
                        SCB_S_CX |              /* ack any events */
                        SCB_S_FR |
                        SCB_S_CNA |
                        SCB_S_RNR |
                        SCB_C_CUSTART);         /* start command unit */

    /* Point SCB to command area in board memory */

    sysOutWord (pDrvCtrl->port + SCB_CBL, CFD_OFFSET);

    /* Copy CFD to board memory */

    sysOutWord (pDrvCtrl->port + WRPTR, CFD_OFFSET);
    sysOutWordString (pDrvCtrl->port + DXREG, (short *) pCfd, sizeof (CFD) / 2);

    eex586ChanAtn (pDrvCtrl);               /* notify device of new command */
    }

    while (1)               /* wait for command acceptance and interrupt */
        {
        if ( (sysInWord (pDrvCtrl->port + SCB_COMMAND) == 0)
            && (sysInWord (pDrvCtrl->port + SCB_STATUS) & SCB_S_CNA))
            break;
        }

    /* Copy updated CFD back to system memory */

    sysOutWord (pDrvCtrl->port + RDPTR, CFD_OFFSET);
    sysInWordString (pDrvCtrl->port + DXREG, (short *) pCfd, sizeof (CFD) / 2);

    /* Acknowledge the event to the device */

    sysOutWord(pDrvCtrl->port + SCB_COMMAND, SCB_S_CX | SCB_S_CNA);
    eex586ChanAtn (pDrvCtrl);

    while (1)               /* wait for acknowledge acceptance */
        {
        if (sysInWord (pDrvCtrl->port + SCB_COMMAND) == 0)
            break;
        }
    }

/*******************************************************************************
*
* eexCommand - deliver a command to the 82586 via SCB
*
* This function causes the device to execute a command. It should be called
* with interrupts from the device disabled. An error status is returned if
* the command field does not return to zero, from a previous command, in a
* reasonable amount of time.
*/

static STATUS eexCommand
    (
    DRV_CTRL *pDrvCtrl,
    UINT16    cmd
    )
    {
    int loopy;

    for (loopy = 0x8000; loopy--;)
        {
        /* wait for cmd zero */
        if (sysInWord (pDrvCtrl->port + SCB_COMMAND) == 0)
            break;
        }

    if (loopy > 0)
        {
        /* fill in command */
        sysOutWord (pDrvCtrl->port + SCB_COMMAND, cmd);
        eex586ChanAtn (pDrvCtrl);    /* channel attention */
        return (OK);
        }
    else
        {
        logMsg("eex driver: command field frozen\n", 0, 0, 0, 0, 0, 0);
        return (ERROR);
        }
    }

/*******************************************************************************
*
* eexTxQPut - place a transmit frame on the transmit queue
*
* The TFD has been filled in with the network pertinent data. This
* routine will enqueue the TFD for transmission and attempt to feed
* the queue to the device.
*/

static void eexTxQPut
    (
    DRV_CTRL *pDrvCtrl,
    EEX_SHORTLINK tfdOffset
    )
    {
    int unit;

    unit = pDrvCtrl->idr.ac_if.if_unit;

    eex586IntDisable (pDrvCtrl->idr.ac_if.if_unit);  /* disable dev ints */

    if (pDrvCtrl->txQueue.head != NULL)
        {
        /* Clear EL bit on previous tail frame */

        sysOutWord (pDrvCtrl->port + WRPTR,
                    pDrvCtrl->txQueue.tail + TF_COMMAND);
        sysOutWord (pDrvCtrl->port + DXREG, CFD_C_XMIT);
        }
        
    /* enqueue the TFD */

    eexQPut (pDrvCtrl, (EEX_LIST *)&pDrvCtrl->txQueue, tfdOffset);

    sysOutWord (pDrvCtrl->port + WRPTR, pDrvCtrl->txQueue.tail + TF_COMMAND);
    sysOutWord (pDrvCtrl->port + DXREG, CFD_C_XMIT | CFD_C_EL);

    if (pDrvCtrl->txIdle)                             /* transmitter idle */
        eexTxQFlush (pDrvCtrl);                        /* flush txQueue */

    eex586IntEnable (pDrvCtrl->idr.ac_if.if_unit);  /* enable dev ints */
    }

/*******************************************************************************
*
* eexTxQFlush - make cmd unit of device start processing cmds
*
* This routine flushes the contents of the txQ to the cblQ and starts the
* device transmitting the cblQ. Called only if transmit queue is not empty.
* Sometimes called from interrupt handler.
*/

static void eexTxQFlush
    (
    DRV_CTRL *pDrvCtrl
    )
    {
    extern int sysClkRateGet();     /* we call this */

    pDrvCtrl->cblQueue.head = pDrvCtrl->txQueue.head;   /* remember cbl head */
    pDrvCtrl->cblQueue.tail = pDrvCtrl->txQueue.tail;   /* remember cbl tail */
    eexQInit ( (EEX_LIST *)&pDrvCtrl->txQueue);         /* tx queue now empty */

    sysOutWord(pDrvCtrl->port + SCB_CBL, pDrvCtrl->cblQueue.head);

    pDrvCtrl->txIdle = FALSE;                   /* transmitter busy */

    /* start watchdog */

    wdStart (
            pDrvCtrl->wid,
            (int) (sysClkRateGet() >> 1),
            (FUNCPTR) eexWatchDog,
            pDrvCtrl->idr.ac_if.if_unit
            );

    /* start command unit */

    eexCommand (pDrvCtrl, SCB_C_CUSTART);
    }

/*******************************************************************************
*
* eexRxQPut - return a RFD to the receive queue for use by the device
*/

static void eexRxQPut
    (
    DRV_CTRL *pDrvCtrl,
    EEX_SHORTLINK rfdOffset
    )
    {
    EEX_SHORTLINK rxTail;
    UINT16 status;
    EEX_SHORTLINK rbdOffset;    /* dhe 10/24/94 */


    sysOutWord (pDrvCtrl->port + WRPTR, rfdOffset);
    sysOutWord (pDrvCtrl->port + DXREG, 0);     /* clear status */
    sysOutWord (pDrvCtrl->port + DXREG, CFD_C_EL); /* new end of list */

    /* dhe 10/24/94 get the buffer pointer */
    sysOutWord(pDrvCtrl->port + RDPTR, rfdOffset + RB_OFFSET);
    rbdOffset = sysInWord(pDrvCtrl->port + DXREG);

    /* dhe 10/24/94 TEST TEST zero the actual count of the buffer */
    sysOutWord(pDrvCtrl->port + WRPTR, rbdOffset);
    sysOutWord(pDrvCtrl->port + DXREG, 0);


    rxTail = pDrvCtrl->rxQueue.tail;         /* remember tail */

    /* Put the RFD on the list */

    eexQPut (pDrvCtrl, (EEX_LIST *) & pDrvCtrl->rxQueue, rfdOffset);

    if (rxTail != NULL)
        {
        sysOutWord (pDrvCtrl->port + WRPTR, rxTail + RF_COMMAND);
        sysOutWord (pDrvCtrl->port + DXREG, 0); /* clear old tail EL */

        sysOutWord (pDrvCtrl->port + RDPTR, rxTail);
        status = sysInWord (pDrvCtrl->port + DXREG);
        if (status & (CFD_S_COMPLETE | CFD_S_BUSY))
            {
            pDrvCtrl->freeRfd = rfdOffset;       /* link questionable */
            }
        else if (!(sysInWord (pDrvCtrl->port + SCB_STATUS) & SCB_S_RUREADY))
            /* receiver dormant */
            {
            eexRxStartup (pDrvCtrl);             /* start receive unit */
            }
        }
    else
        {
        pDrvCtrl->freeRfd = rfdOffset;           /* first free RFD */
        }
    }

/*******************************************************************************
*
* eexRxQGet - get a successfully received frame from the receive queue
*
* RETURNS: ptr to valid RFD, or NULL if none available
*/

static EEX_SHORTLINK eexRxQGet
    (
    DRV_CTRL *pDrvCtrl
    )
    {
    EEX_SHORTLINK rfdOffset = NULL;

    if (eexRxQFull (pDrvCtrl))
        rfdOffset = eexQGet (pDrvCtrl, (EEX_LIST *)&pDrvCtrl->rxQueue);

    return (rfdOffset);
    }

/*******************************************************************************
*
* eexRxQFull - boolean function to determine fullness of receive queue
*
* RETURNS: TRUE if completely received frame is available, FALSE otherwise.
*/

static BOOL eexRxQFull
    (
    DRV_CTRL *pDrvCtrl
    )
    {

    return (
        (pDrvCtrl->rxQueue.head != NULL) &&
        (
        sysOutWord (pDrvCtrl->port + RDPTR, pDrvCtrl->rxQueue.head),
        sysInWord (pDrvCtrl->port + DXREG) & CFD_S_COMPLETE
        )
        );
    }

/*******************************************************************************
*
* eexQInit - initialize a singly linked node queue
*/

static void eexQInit
    (
    EEX_LIST *pQueue
    )
    {
    pQueue->head = pQueue->tail = NULL;         /* init head & tail */
    }

/*******************************************************************************
*
* eexQGet - get a node from the head of a node queue
*
* RETURNS: ptr to useable node, or NULL ptr if none available
* Since we never alter links, we are taking the last node when head==tail.
*/

static EEX_SHORTLINK eexQGet
    (
    DRV_CTRL *pDrvCtrl,
    EEX_LIST *pQueue
    )
    {
    EEX_SHORTLINK pNode;

    if ( (pNode = (EEX_SHORTLINK) pQueue->head) != NULL) /* if list not empty */
        {
        if (pQueue->head == pQueue->tail)
            pQueue->head =                              /* list now empty */
            pQueue->tail = NULL;
        else
            pQueue->head = SUCC_FD (pNode);             /* advance ptr */
        }

    return (pNode);
    }

/*******************************************************************************
*
* eexQPut - put a node on the tail of a node queue
*/

static void eexQPut
    (
    DRV_CTRL *pDrvCtrl,
    EEX_LIST *pQueue,
    EEX_SHORTLINK pNode
    )
    {
    /* assert (SUCC_FD (pQueue->tail) == pNode); */

    if (pQueue->head == NULL)                   /* if list empty */
        pQueue->head = pNode;                   /* new element is head */

    pQueue->tail = pNode;                       /* update tail ptr */
    }

/*******************************************************************************
*
* eexWatchDog - if the watchdog timer fired off, we've hung during a transmit
*
* Check the scb command to verify and if so, reinit.
*/

static void eexWatchDog
    (
    int unit        /* unit number */
    )

    {
    DRV_CTRL *pDrvCtrl;

    pDrvCtrl = & drvCtrl [unit];

    /* sanity check.
     * If the scb status indicates that CU (transmit) is active
     * It might make sense to loop through the cfd's to look for
     * a complete bit as a sanity check , but given that we are
     * here and that transmit was active, we will go ahead and do
     * a reset.
     */

    if (sysInWord (pDrvCtrl->port + SCB_STATUS) & SCB_S_CUACTIVE)
        {
        pDrvCtrl->transLocks++;                     /* local failure count */
        pDrvCtrl->idr.ac_if.if_oerrors++;           /* incr err count */
        pDrvCtrl->idr.ac_if.if_opackets--;          /* decr sent count */
        DUMMY(unit);
        }
    }

/*******************************************************************************
*
* eex586Init - Initialize board features
*
* Does pretty much everything to initialize the 82586 and its memory
* structures, either in this routine or in called routines.
*/

static STATUS eex586Init
    (
    int unit
    )
    {
    DRV_CTRL *pDrvCtrl;
    int ix;
    int port;
    EEX_SHORTLINK currentFd;
    /**** dhe 10/26/94 moved to global for reassigning during restart
    EEX_SHORTLINK lastFd;
    ****/
    int connValue;

    pDrvCtrl = & drvCtrl [unit];

    port = pDrvCtrl->port;
    eex586IntDisable (unit);            /* disable device interrupts */

    /* extract setup information from the board's EEPROM */
    /* Remember that readEEPROM() sets the 82586 reset condition. */

    pDrvCtrl->setup = eexReadEEPROM (pDrvCtrl, EEX_EEPROM_SETUP);
    pDrvCtrl->tpeBit = eexReadEEPROM (pDrvCtrl, EEX_EEPROM_TPE_BIT);
    pDrvCtrl->memSetup.wordView.memPage =
                eexReadEEPROM (pDrvCtrl, EEX_EEPROM_MEMPAGE);
    pDrvCtrl->memSetup.wordView.memDecode =
                eexReadEEPROM (pDrvCtrl, EEX_EEPROM_MEMDECODE);

    if (pDrvCtrl->attachment == ATTACHMENT_DEFAULT)
        if ( (pDrvCtrl->memSetup.wordView.memPage & MEMPAGE_AUTODETECT) != 0)
            {

            /* Here we would figure out which connector is in use;
             * "auto-detect" relies on software to do the actual detection.
             */

            }
        else
            {
            if ( (pDrvCtrl->setup & SETUP_BNC) == 0)
                pDrvCtrl->attachment = ATTACHMENT_AUI;
            else if ( (pDrvCtrl->tpeBit & TPE_BIT) == 0)
                pDrvCtrl->attachment = ATTACHMENT_BNC;
            else
                pDrvCtrl->attachment = ATTACHMENT_RJ45;
            }

    /* Tell the board hardware which connector we're using */

    connValue = sysInByte (port + ECR1) & ~(CONN_INTEGRITY | CONN_TRANSCEIVER);
    switch (pDrvCtrl->attachment)
        {
        case ATTACHMENT_AUI:
            break;
        case ATTACHMENT_BNC:
            connValue |= CONN_TRANSCEIVER;
            break;
        case ATTACHMENT_RJ45:
            connValue |= CONN_TRANSCEIVER | CONN_INTEGRITY;
            break;
        }

    /* 17jan95  jag
     * Only send the connector selection command if the board is
     * NOT in "auto-detect" mode.  (If the board is in
     * "auto-detect" mode, another command might disrupt the
     * auto-detected mode).
     */
    if ( (pDrvCtrl->memSetup.wordView.memPage & MEMPAGE_AUTODETECT) == 0)
        sysOutByte (port + ECR1, connValue);

    /* get our enet addr */

    if (eexEnetAddrGet (pDrvCtrl, (char *)pDrvCtrl->idr.ac_enaddr) == ERROR)
        {
        errnoSet (S_iosLib_INVALID_ETHERNET_ADDRESS);
        return (ERROR);
        }

    /* Connect the interrupt handler */

    if (intConnect ( (VOIDFUNCPTR *)INUM_TO_IVEC (pDrvCtrl->ivec),
                    eexInt, (int)pDrvCtrl) == ERROR)
        return (ERROR);

    /* Start the device */

    if ( eexDeviceStart (unit) == ERROR )
        return (ERROR);

    eexDiag (unit);                             /* run diagnostics */
    eexConfig (unit);                           /* configure chip */
    eexIASetup (unit);                          /* set up address */

    /* Carve up the shared memory region into the specific sections */

    pDrvCtrl->rfdBase =
            (TFD_OFFSET + (sizeof (TFRAME) * pDrvCtrl->nTFDs));

    /* create the free tfd queue; nTFDs is non-zero */

    for (ix = 0, currentFd = TFD_OFFSET;
         ix < pDrvCtrl->nTFDs;
         ix ++, pDrvCtrl->lastFd = currentFd, currentFd += sizeof (TFRAME))
        {

        /* Initialize the TFD to link to next TFD, point to adjacent TBD */

        sysOutWord (port + WRPTR, currentFd);
        sysOutWord (port + DXREG, 0);                           /* status */
        sysOutWord (port + DXREG, CFD_C_XMIT);                  /* command */
        sysOutWord (port + DXREG, currentFd + sizeof (TFRAME)); /* TFD link */
#ifdef  EEX_AL_LOC
        sysOutWord (port + DXREG, currentFd + sizeof (TFD));    /* TBD ptr */
#else
        sysOutWord (port + DXREG, currentFd + sizeof (TFD) +
                                  FRAME_SIZE);                  /* TBD ptr */
        sysOutWord (port + WRPTR, currentFd + sizeof (TFD) +
                                  FRAME_SIZE);          /* point to TBD */
#endif  /* EEX_AL_LOC */

        /* Initialize the TBD following the above TFD */

        sysOutWord (port + DXREG, 0);                   /* status */
        sysOutWord (port + DXREG, 0xffff);              /* next TBD */
#ifdef  EEX_AL_LOC
        sysOutWord (port + DXREG,                       /* buffer address */
                    currentFd + sizeof (TFD) + sizeof (TBD));
#else
        sysOutWord (port + DXREG,                       /* buffer address */
                    currentFd + sizeof (TFD) + EH_SIZE);
#endif  /* EEX_AL_LOC */
        sysOutWord (port + DXREG, 0);                   /* buffer add. hi */
        }

    /* Fix the link field of the last TFD to point to the first */

    sysOutWord (port + WRPTR, pDrvCtrl->lastFd + TF_LINK);
    sysOutWord (port + DXREG, TFD_OFFSET);

    pDrvCtrl->tfdQueue.head = TFD_OFFSET;       /* Initialize queues */
    pDrvCtrl->tfdQueue.tail = pDrvCtrl->lastFd;
    eexQInit ( (EEX_LIST *)&pDrvCtrl->txQueue);    /* to be sent queue */
    eexQInit ( (EEX_LIST *)&pDrvCtrl->cblQueue);   /* actively sending queue */
    eexQInit ( (EEX_LIST *)&pDrvCtrl->cleanQueue); /* queue of TFDs to clean */

    /* create all the receive frame descriptors */

    for (ix = 0, currentFd = pDrvCtrl->rfdBase;
         ix < pDrvCtrl->nRFDs;
         ix ++, pDrvCtrl->lastFd = currentFd, currentFd += sizeof (RFRAME))
        {

        /* Initialize the RFD to link to next RFD, point to following RBD */

        sysOutWord (port + WRPTR, currentFd);
        sysOutWord (port + DXREG, 0);                           /* status */
        sysOutWord (port + DXREG, 0);                           /* command */
        sysOutWord (port + DXREG, currentFd + sizeof (RFRAME)); /* RFD link */
#ifdef  EEX_AL_LOC
        sysOutWord (port + DXREG, currentFd + sizeof (RFD));    /* RBD ptr */
#else
        sysOutWord (port + DXREG, currentFd + sizeof (RFD) +
                                  FRAME_SIZE);                  /* RBD ptr */
        sysOutWord (port + WRPTR, currentFd + sizeof (RFD) +
                                  FRAME_SIZE);          /* point to RBD */
#endif  /* EEX_AL_LOC */

        /* Initialize the RBD following the above RFD */

        sysOutWord (port + DXREG, 0);                   /* act. count */
        sysOutWord (port + DXREG, 0xffff);              /* next RBD */
#ifdef  EEX_AL_LOC
        sysOutWord (port + DXREG,                       /* buffer address */
                    currentFd + sizeof (RFD) + sizeof (RBD));
#else
        sysOutWord (port + DXREG,                       /* buffer address */
                    currentFd + sizeof (RFD) + EH_SIZE);
#endif  /* EEX_AL_LOC */
        sysOutWord (port + DXREG, 0);                   /* buffer add. hi */
        sysOutWord (port + DXREG, RBD_M_EL + FRAME_SIZE); /* buffer size */
        }

    /* Fix the link field of the last RFD to point to the first */

    sysOutWord (port + WRPTR, pDrvCtrl->lastFd + RF_LINK);
    sysOutWord (port + DXREG, pDrvCtrl->rfdBase);

    pDrvCtrl->rxQueue.head = pDrvCtrl->rfdBase; /* Initialize queue */
    pDrvCtrl->rxQueue.tail = pDrvCtrl->lastFd;
    pDrvCtrl->freeRfd = pDrvCtrl->rxQueue.head;

    eex586IntAck (unit);                     /* clear any pended dev ints */
    eex586IntEnable (unit);                  /* enable interrupts at system */
    eexRxStartup (pDrvCtrl);                 /* start receive unit */

    return (OK);
    }

/*******************************************************************************
*
* eexEnetAddrGet - get OEM Ethernet address from EtherExpress on-board EEPROM
*
* The "OEM" (i.e. not necessarily assigned by Intel) Ethernet address for the
* board is contained in words 2, 3, and 4 of the on-board EEPROM.  Note that
* EEPROM addresses are WORD not byte addresses!
* The byte swapping arises from the address being stored in big-endian byte
* order, apparently in approximation of the transmitted order (but the bits
* would have to be reversed for true transmitted bit order).
*/

static STATUS eexEnetAddrGet
    (
    DRV_CTRL *pDrvCtrl,
    char addr[]
    )

    {
    UINT16 eepromAddress;
    char *dest = addr;
    UINT16 t;

    for (eepromAddress = EEX_EEPROM_EA_HIGH;
         eepromAddress >= EEX_EEPROM_EA_LOW;
         eepromAddress--)
        {
        t = eexReadEEPROM (pDrvCtrl, eepromAddress);
        *dest++ = t >> 8;
        *dest++ = t & 0xff;
        }
    
    return (OK);
    }

/*******************************************************************************
*
* eex586IntAck - acknowledge EtherExpress 16 interrupt
*
* The board hardware conatains an interrupt latch which is cleared by clearing
* the interrupt enable bit.  We presume we want the interrupts enabled so we
* re-enable the interrupt.
*/

static void eex586IntAck
    (
    int unit
    )
    {
    eex586IntDisable (unit);
    eex586IntEnable (unit);

    }

/*******************************************************************************
*
* eex586IntDisable - disable EtherExpress 16 interrupt
*
* Prevent the board's 82586 from interrupting the system.
* As noted in IntAck above, the board has an interrupt latch which is cleared
* "with the disable-enable interrupt bit".  We will assume that after the
* moment of disabling, pending interrupts will be latched and honored when
* the interrupt is re-enabled.  There is no evidence to support this
* assumption, and it should be tested some day.
*/

static void eex586IntDisable
    (
    int unit
    )
    {
    DRV_CTRL *pDrvCtrl;

    pDrvCtrl = & drvCtrl [unit];

    sysOutByte (pDrvCtrl->port + SEL_IRQ, pDrvCtrl->irqCode);

    }

/*******************************************************************************
*
* eex586IntEnable - enable EtherExpress 16 interrupt
*
* Allow the board's 82586 to interrupt the system.
*/

static void eex586IntEnable
    (
    int unit
    )
    {
    DRV_CTRL *pDrvCtrl;

    pDrvCtrl = & drvCtrl [unit];

    sysOutByte (pDrvCtrl->port + SEL_IRQ, pDrvCtrl->irqCode | IRQ_ENB);

    }

/*******************************************************************************
*
* eex586SetReset - reset the EtherExpress 16's  82586 chip
*
* Set chip into reset state (and hold it there until released elsewhere).
*/

static void eex586SetReset
    (
    int unit
    )
    {
    DRV_CTRL *pDrvCtrl;

    pDrvCtrl = & drvCtrl [unit];

    sysOutByte (pDrvCtrl->port + EE_CTRL, RESET_82586);

    }

/*******************************************************************************
*
* eex586ClearReset - remove the EtherExpress 16's 82586 chip from reset state
*
* Clear the bit holding the chip's reset line enabled, allowing the chip
* to follow its power-on initialization sequence.
*/

static void eex586ClearReset
    (
    int unit
    )
    {
    DRV_CTRL *pDrvCtrl;

    pDrvCtrl = & drvCtrl [unit];

    /* All other bits in this register should be zero when chip runs */
    sysOutByte (pDrvCtrl->port + EE_CTRL, 0);

    }

/*******************************************************************************
*
* eex586ChanAtn - assert Channel Attention to the EtherExpress 16's  82586 chip
*
* Channel Attention tells the 82586 to examine its System Control Block
* for commands and acknowledgements.
* This could really be a macro.
*/

static void eex586ChanAtn
    (
    DRV_CTRL *pDrvCtrl
    )
    {

    /* The act of writing to the port does the job; data is irrelevant */
    sysOutByte (pDrvCtrl->port + CA_CTRL, 0);

    }

/*******************************************************************************
*
* eexReadEEPROM - read one word from the on-board EEPROM
*
* Note that a necessary side effect of accessing the EEPROM is to set the
* RESET_82586 bit, because the 82586's bus signals must be disabled.
* Note that EEPROM addresses are WORD not byte addresses!
*/

static UINT16 eexReadEEPROM
    (
    DRV_CTRL *pDrvCtrl,
    UINT16 address
    )
    {
    int port;
    UCHAR eeCtrl;               /* maintains current port contents */
    UINT16 value;

    port = pDrvCtrl->port + EE_CTRL;
    eeCtrl = sysInByte (port) & (EEPROM_CHIPSEL | RESET_82586);
    sysOutByte (port, eeCtrl | EEPROM_CHIPSEL);

    eexOutBitsEEPROM (port, 3, EEPROM_OP_READ);
    eexOutBitsEEPROM (port, 6, address);
    value = eexInBitsEEPROM (port, 16);

    eeCtrl &= EEPROM_CHIPSEL;           /* be certain chip select is clear */
    sysOutByte (port, eeCtrl);          /* also the data port */
    eexDelayEEPROM();                   /* data setup time */
    sysOutByte (port, eeCtrl | EEPROM_CLOCK);   /* clock with CS low? */
    eexDelayEEPROM();
    sysOutByte (port, eeCtrl);
    eexDelayEEPROM();

    return (value);
    }

/*******************************************************************************
*
* eexOutBitsEEPROM - send a bit string to the on-board EEPROM
*
* Shifts the righmost 'count' bits out, LEFTMOST first.
*/

static void eexOutBitsEEPROM
    (
    int port,
    int count,
    UINT16 bits
    )
    {
    UCHAR eeCtrl;               /* maintains current port contents */

    bits <<= 16 - count;        /* left-justify the bit string */
    eeCtrl = sysInByte (port) & (EEPROM_CHIPSEL | RESET_82586);

    while (count--)
        {
        if (bits & 0x8000)
            eeCtrl |= EEPROM_OUTPUT;
        else
            eeCtrl &= ~EEPROM_OUTPUT;

        sysOutByte (port, eeCtrl);
        eexDelayEEPROM();                       /* data setup time */
        sysOutByte (port, eeCtrl | EEPROM_CLOCK);
        eexDelayEEPROM();
        sysOutByte (port, eeCtrl);
        eexDelayEEPROM();
        bits <<= 1;
        }
    sysOutByte (port, eeCtrl & ~EEPROM_OUTPUT); /* clear output bit */
    }

/*******************************************************************************
*
* eexInBitsEEPROM - receive a bit string from the on-board EEPROM
*
*/

static UINT16 eexInBitsEEPROM
    (
    int port,
    int count
    )
    {
    UCHAR eeCtrl;               /* maintains current port contents */
    UINT16 value;               /* build result a bit at a time */

    eeCtrl = sysInByte (port) & (EEPROM_CHIPSEL | RESET_82586);
    value = 0;

    while (count--)
        {
        value <<= 1;
        sysOutByte (port, eeCtrl | EEPROM_CLOCK);
        eexDelayEEPROM();
        if (sysInByte (port) & EEPROM_INPUT)
            value |= 1;
        sysOutByte (port, eeCtrl);
        eexDelayEEPROM();
        }
    return (value);
    }

/*******************************************************************************
*
* eexDelayEEPROM - delay for EEPROM to recognize clock change
*
* Supposedly 4.0 microseconds for the Hyundai EEPROM.
*/

static void eexDelayEEPROM
    (
    )
    {
    taskDelay (1);              /* gross overkill! */
    }

/******************************************************************************/

/* END OF FILE */
