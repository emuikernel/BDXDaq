/* if_nic.c - National Semiconductor SNIC Chip (for HKV30) network interface driver */

/* Copyright 1984-1997 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
02b,15jul97,spm  added ARP request to SIOCSIFADDR ioctl handler
02a,07apr97,spm  code cleanup, corrected statistics, and upgraded to BSD 4.4
01z,11aug93,jmm  Changed ioctl.h and socket.h to sys/ioctl.h and sys/socket.h
01y,19feb93,jdi  documentation cleanup.
01x,15oct92,rfs  Added documentation.
01w,02oct92,rfs  Made multiple attach calls per unit return OK.
01v,09sep92,gae  documentation tweaks.
01u,02sep92,ajm  added conditional compilation for __STDC__ (DEC)
01t,18aug92,rfs  Revised to follow style of other drivers.
                 Added cache support.  Reworked OVW error recovery.
01s,23jun92,ajm  fixed ansi warnings for pktBufRead calls
                 went back and fixed all 68k ansi warnings
01r,26may92,rrr  the tree shuffle
                 -changed includes to have absolute path from h/
01q,26may92,ajm  got rid of HOST_DEC def's (new compiler)
                 updated copyright
01p,10oct91,rfs  modified pktBufRead() & pktBufWrite() to return if len is zero
01o,04oct91,rrr  passed through the ansification filter
                 -changed functions to ansi style
                 -changed includes to have absolute path from h/
                 -changed VOID to void
                 -changed copyright notice
01n,28sep91,ajm  ifdef'd HOST_DEC for compiler problem
01m,02Aug91,rfs  cleanup of transmit design, int handler, NIC command register
                 was being handled incorrectly, DMA operations required
                 semaphoring and error checking, changed names, deleted
                 nicdelay(), deleted excessive include files. Provided correct
                 OVW recovery.  Your basic rewrite.
01l,01may91,elh  fixed transmit buffering problems, added changes from
                 heurikon, removed DMAState, XmtByteCount.
01k,20sep90,dab  made nicInit() return int.
01j,10aug90,dnw  added forward declarations of void routines.
                 added include of if_subr.h.
01i,11jul90,hjb  removed code that uses gotIp.  made nicRead() a void function.
                 changed references to do_protocol() to use
                 do_protocol_with_type().
01h,19apr90,hjb  deleted param.h, de-linted.
01g,18mar90,hjb  reduction of redundant code and addition of cluster support.
                 added error recording (TXE, RXE), removed unnecessary
                 error handler which didn't do anything.  delinted.
01f,18sep89,cwp  added multiple transmit buffer support
01e,14sep89,dab  changed iv68k.h to iv.h.
01d,15jun89,cwp  fixed ring buffer overflow and transmit errors.
01c,04apr89,cwp  first working version.
01b,25feb89,cwp  added changes from unix driver.
01a,12jan89,cwp  written.
*/

/*
This module implements the National Semiconductor 83901 SNIC Ethernet network 
interface driver.

This driver is non-generic and is for use on the Heurikon HKV30 board.  
Only unit number zero is supported.  The driver must be given several 
target-specific parameters.  These parameters, and the mechanisms 
used to communicate them to the driver, are detailed below.

BOARD LAYOUT
This device is on-board.  No jumpering diagram is necessary.

EXTERNAL INTERFACE
This driver provides the standard external interface with the following
exceptions.  All initialization is performed within the attach routine; there
is no separate initialization routine.  Therefore, in the global interface
structure, the function pointer to the initialization routine is NULL.

The only user-callable routine is nicattach(), which publishes the `nic'
interface and initializes the driver and device.

TARGET-SPECIFIC PARAMETERS
.iP "device I/O address"
This parameter is passed to the driver by nicattach().
It specifies the base address of the device's I/O register
set.
.iP "interrupt vector"
This parameter is passed to the driver by nicattach().
It specifies the interrupt vector to be used by the driver
to service an interrupt from the SNIC device.  The driver will connect
the interrupt handler to this vector by calling intConnect().
.LP

SYSTEM RESOURCE USAGE
When implemented, this driver requires the following system resources:

    - one mutual exclusion semaphore
    - one interrupt vector
    - 0 bytes in the initialized data section (data)
    - 1702 bytes in the uninitialized data section (BSS)

The above data and BSS requirements are for the MC68020 architecture 
and may vary for other architectures.  Code size (text) varies greatly between
architectures and is therefore not quoted here.

The HKV30 provides a private buffer for all packets transmitted and received.
Thus, the driver does not require any system memory to share with the
device.  This also eliminates all data cache coherency issues.

SEE ALSO: ifLib
*/

#include "vxWorks.h"
#include "iv.h"
#include "stdlib.h"
#include "intLib.h"
#include "netLib.h"
#include "net/mbuf.h"
#include "sys/ioctl.h"
#include "errno.h"
#include "net/if.h"
#include "net/unixLib.h"
#include "net/if_subr.h"
#include "netinet/in_var.h"
#include "netinet/if_ether.h"
#include "etherLib.h"
#include "semLib.h"
#include "cacheLib.h"
#include "drv/netif/if_nic.h"
#include "drv/dma/we32104.h"


/***** LOCAL DEFINITIONS *****/

#define MAX_UNITS      1                     /* max number of units supported */
#define NIC_DCR_CS      DCR_CS_2                /* device #2 on DMAC PBUS */
#define NIC_DCR_PA      0x60                    /* device addr on DMAC PBUS */

/* Typedefs for external structures that are not typedef'd in their .h files */

typedef struct mbuf MBUF;
typedef struct arpcom IDR;                  /* Interface Data Record wrapper */
typedef struct ifnet IFNET;                 /* real Interface Data Record */
typedef struct sockaddr SOCK;
typedef struct ether_header ETH_HDR;

#ifdef BSD43_DRIVER
#define ENET_HDR_REAL_SIZ   14
#endif

typedef struct rx_hdr
    {
    u_char status;                              /* status of packet */
    u_char nextRxPage;                          /* page next pkt starts at */
    u_char cntL;                                /* low byte of frame length */
    u_char cntH;                                /* high byte of frame length */
    } RX_HDR;
#define RX_HDR_SIZ      sizeof(RX_HDR)

typedef struct rx_frame
    {
    RX_HDR rxHdr;                               /* the receive status header */
    ETH_HDR enetHdr;                            /* the Ethernet header */
    u_char data [ETHERMTU];                     /* the data */
    u_long fcs;
    } RX_FRAME;
#define RX_FRAME_SIZ        sizeof(RX_FRAME)

typedef struct tx_desc
    {
    volatile BOOL ready;                        /* ready for transmission */
    unsigned short addr;                        /* addr in NIC buffer */
    unsigned short len;                         /* length of packet */
    struct tx_desc *next;                       /* link to next descriptor */
    } TX_DESC;
#define TX_DESC_SIZ     sizeof(TX_DESC)

/* The driver control structure. */

typedef struct drv_ctrl
    {
    IDR                 idr;                /* interface data record */

    BOOL                attached;           /* indicates unit is attached */
    int                 nicIntVec;          /* interrupt vector */
    SEM_ID              dmaSem;             /* exclusive use of DMA units */
    u_char              NextPkt;            /* NIC buf page for next pkt */
    volatile BOOL       dmaWait;            /* waiting for DMA completion */
    volatile BOOL       taskRcvActive;      /* indicates netTask in use */
    NIC_DEVICE          *nicAddr;           /* address of NIC chip */
    volatile TX_DESC    *pTxActive;         /* ptr to Tx desc in use */
    TX_DESC             *pTxNext;           /* ptr to next Tx desc to use */
    TX_DESC             txDesc [MAXXMT];    /* array of Tx descriptors */
    RX_FRAME            rxFrame;            /* storage for one Rx frame */
    } DRV_CTRL;

#define DRV_CTRL_SIZ       sizeof(DRV_CTRL)

/* array of driver control structs, one per unit supported */
LOCAL DRV_CTRL drvCtrl [MAX_UNITS];


/* forward static functions */

static void nicReset (int unit);
static void nicIntr (int unit);
static void nicRestart (int unit);
static void nicHandleInt (DRV_CTRL *pDrvCtrl);
static BOOL nicRead (DRV_CTRL *pDrvCtrl);
#ifdef BSD43_DRIVER
static int nicOutput (IDR *pIDR, MBUF *pMbuf, SOCK *pDest);
#else
static int nicTxStartup (DRV_CTRL *pDrvCtrl);
#endif
static int nicIoctl (IDR *pIDR, int cmd, caddr_t data);
static void nicEnetAddrGet (int unit);
static void nicConfig (int unit);
static STATUS pktBufWrite (DRV_CTRL *pDrvCtrl, u_long nicBufAddr,
                           u_long len, char *pData);
static STATUS pktBufRead (DRV_CTRL *pDrvCtrl, u_long nicBufAddr,
                          u_long len, char *pData);
#ifdef BSD43_DRIVER
static BOOL convertDestAddr (IDR *pIDR, SOCK *pDestSktAddr,
                             char *pDestEnetAddr, u_short *pPacketType,
                             MBUF *pMbuf);
#endif


/*******************************************************************************
*
* nicattach - publish the `nic' network interface and initialize the driver and device
*
* This routine publishes the `nic' interface by filling in a network interface
* record and adding this record to the system list.  It also
* initializes the driver and the device to the operational state.
*
* RETURNS: OK or ERROR.
*/

STATUS nicattach
    (
    int unit,                                 /* unit number */
    NIC_DEVICE *pNic,                         /* address of NIC chip */
    int ivec                                  /* interrupt vector to use */
    )
    {
    int         temp;
    int         loopy;
    WE32104     *pDmac;
    DRV_CTRL    *pDrvCtrl;


    /* Sanity check the unit number */

    if (unit < 0 || unit >= MAX_UNITS)
        return (ERROR);

    /* Ensure single invocation per system life */

    pDrvCtrl = & drvCtrl [unit];
    if (pDrvCtrl->attached)
        return (OK);

    pDrvCtrl->dmaSem = semBCreate(SEM_Q_FIFO, SEM_FULL);
    pDrvCtrl->nicAddr = pNic;
    pDrvCtrl->nicIntVec = ivec;

    /* Publish the interface data record */

#ifdef BSD43_DRIVER
    ether_attach    (
                    &pDrvCtrl->idr.ac_if,
                    unit,
                    "nic",
                    (FUNCPTR) NULL,
                    (FUNCPTR) nicIoctl,
                    (FUNCPTR) nicOutput,
                    (FUNCPTR) nicReset
                    );
#else
    ether_attach (
                 &pDrvCtrl->idr.ac_if,
                 unit,
                 "nic",
                 (FUNCPTR) NULL,
                 (FUNCPTR) nicIoctl,
                 (FUNCPTR) ether_output, 
                 (FUNCPTR) nicReset
                 );
    pDrvCtrl->idr.ac_if.if_start = (FUNCPTR)nicTxStartup;
#endif

    /*
     * Fill in the Tx descriptors.
     * Note that the address in the NIC buffer must be on a 256 byte boundary,
     * since only the upper 8 bits are fed to the device when telling it
     * to transmit.
     */

    for (loopy = 0; loopy < MAXXMT; loopy++)
        {
        /* addr in NIC buf */
        pDrvCtrl->txDesc[loopy].addr = XMTBUF + (loopy * 0x600);
        /* point to next */
        pDrvCtrl->txDesc[loopy].next = & pDrvCtrl->txDesc [loopy+1];
        pDrvCtrl->txDesc[loopy].ready = FALSE;
        }

    pDrvCtrl->txDesc[MAXXMT-1].next = pDrvCtrl->txDesc;    /* point to first */

    /* Connect the interrupt handler */

    (void) intConnect (INUM_TO_IVEC (ivec), nicIntr, 0);

    pDrvCtrl->pTxNext = pDrvCtrl->txDesc;          /* reset "next Tx" ptr */
    pDrvCtrl->pTxActive = NULL;                    /* no active Tx desc */
    pDrvCtrl->taskRcvActive = FALSE;               /* reset flag */
    pDrvCtrl->NextPkt = CURR;                      /* reset to initial value */

    /* The WE32104 controls all access to the peripheral buss. It allows the
     * CPU "transparent" access to the devices on that buss. Before this
     * access is allowed, the WE32104 must be told device control info. The
     * following line sets up the channel 2 device control register to
     * correctly access the NIC. These values are obtained from the HKV30
     * manual.
     */

    pDmac = (WE32104 *) DMA_BASE;
    pDmac->Ch2.DevControl = NIC_DCR_CS | NIC_DCR_PA;

    pDmac->Mask = pDmac->Mask & ~MASK_CH2;      /* allow channel 2 requests */

    temp = RESET_ON;                            /* Issue a hardware reset */
    temp = RESET_OFF;                           /* release the reset */
    temp = LED_OFF;                             /* the 'U' LED on XE module */

    while (!(pNic->Isr & RST))                  /* wait for reset done */
        ;

    nicConfig (unit);                              /* configure the device */

    /* Raise the interface flags */

    pDrvCtrl->idr.ac_if.if_flags |= IFF_UP | IFF_RUNNING | IFF_NOTRAILERS;

    /* Set our flag */

    pDrvCtrl->attached = TRUE;

    temp = LED_ON;                      /* turn XE module LED on */

    return (OK);
    }

/*******************************************************************************
*
* nicReset - reset of interface
*/

LOCAL void nicReset
    (
    int unit
    )
    {
    int         temp;
    DRV_CTRL    *pDrvCtrl;

    pDrvCtrl = & drvCtrl [unit];
    pDrvCtrl->idr.ac_if.if_flags = 0;
    temp = RESET_ON;                            /* Issue a hardware reset */
    temp = LED_OFF;                             /* the 'U' LED on XE module */

    }

/*******************************************************************************
*
* nicIntr - The driver's interrupt handler
*
* This function clears the cause of the device interrupt(s) and then acts
* on the individual possible causes.  The primary goal of this routine is to
* minimize the time spent in it.  This is accomplished by deferring processing
* to the netTask via the netJobAdd() function.
*
* A secondary goal is to keep the transmitter working, if there are more
* frames ready to be sent.  This is done examining flags set by the task level
* code that indicates ready frames.  The transmitter is flagged as "idle" if
* it appears that there are no frames ready to send.
*
* Note that in case the receiver overruns, we promptly mark the interface as
* "down" and then leave only the RDC interrupt enabled.  This is in case netTask
* is in the midst of DMA activity, we must allow it to complete.  The receive
* handler will give up when it discovers the interface is down, which will
* then allow netTask to run our OVW handler.  This provides a nice orderly
* error recovery, even though the crummy NIC requires complete re-init.
*/

LOCAL void nicIntr
    (
    int unit
    )
    {
    DRV_CTRL *pDrvCtrl;
    NIC_DEVICE *pNic;
    TX_DESC *pTxDesc;
    unsigned char isr;                            /* copy of ISR */
    unsigned char tsr;                            /* copy of TSR */

    pDrvCtrl = & drvCtrl [unit];
    pNic = pDrvCtrl->nicAddr;

    isr = pNic->Isr;                              /* copy the ISR */
    pNic->Isr = isr;                              /* ACK all the ints */

    /* handle receiver overrun */

    if  (
        (isr & OVW) &&
        (pDrvCtrl->idr.ac_if.if_flags & (IFF_UP | IFF_RUNNING))
        )
        {
        pNic->Imr = RDCE;                       /* disable all but RDC intrs */
        pDrvCtrl->idr.ac_if.if_flags &=                 /* flag as down */
            ~(IFF_UP | IFF_RUNNING | IFF_PROMISC);
        /* Place our restart routine on the netTask queue */
        netJobAdd ((FUNCPTR) nicRestart, unit, 0, 0, 0, 0);
        return;                     /* no sense continuing */
        }

    /* handle packet Tx error */

    if (isr & TXE)
        {
        pDrvCtrl->idr.ac_if.if_opackets--;
        pDrvCtrl->idr.ac_if.if_oerrors++;
        }

    /* handle packet received */

    if  ((isr & PRX) &&
        (pDrvCtrl->idr.ac_if.if_flags & (IFF_UP | IFF_RUNNING)) &&
        (!pDrvCtrl->taskRcvActive))
        {
        pDrvCtrl->taskRcvActive = TRUE;                 /* set flag */
        /* defer to netTask */
        netJobAdd ((FUNCPTR) nicHandleInt, (int) pDrvCtrl, 0, 0, 0, 0);
        }

    /* handle packet transmitted */

    if  ((isr & PTX) &&
        (pDrvCtrl->idr.ac_if.if_flags & (IFF_UP | IFF_RUNNING)) &&
                                                /* copy ptr, safety check it */
        ((pTxDesc = (TX_DESC *) pDrvCtrl->pTxActive) != NULL))
        {
        tsr = pNic->Tsr;                          /* read Tx status reg */
        pTxDesc->ready = FALSE;                   /* active pkt completed */

        if (pTxDesc->next->ready)                 /* if next pkt ready to go */
            {
            pTxDesc = pTxDesc->next;              /* use next desc */
            pDrvCtrl->pTxActive = pTxDesc;        /* update active ptr */
            pNic->Tpsr = pTxDesc->addr >> 8;
            pNic->Tbcr0 = pTxDesc->len & 0xff;
            pNic->Tbcr1 = pTxDesc->len >> 8;

            /* Start NIC transmitting. Note that since we could be doing
             * a NIC remote DMA transfer at this time, we must maintain the
             * state of these bits.
             */
            pNic->Cr = TXP | (pNic->Cr & (RREAD | RWRITE));
            }
        else
            pDrvCtrl->pTxActive = NULL;             /* transmitter idle */

        if (tsr & COL)                              /* collision */
            pDrvCtrl->idr.ac_if.if_collisions++;
        }

    /* handle DMA complete */

    if (isr & RDC)
        pDrvCtrl->dmaWait = FALSE;                  /* clr the waiting flag */
    }

/*******************************************************************************
*
* nicRestart - restart the device after fatal error
*/

LOCAL void nicRestart
    (
    int unit
    )
    {
    int         temp;
    int         loopy;
    DRV_CTRL    *pDrvCtrl;
    NIC_DEVICE  *pNic;                         /* address of device */

    pDrvCtrl = & drvCtrl [unit];
    pNic = pDrvCtrl->nicAddr;

    /* The device has all interrupts disabled and the interface flags are down.
     * Our job is to reset the device and our dynamic driver variables,
     * bring the device back to life, and put the interface online.
     */

    temp = RESET_ON;                            /* Issue a hardware reset */
    temp = RESET_OFF;                           /* release the reset */

    /* Wait for device reset indication */

    while ( ! (pNic->Isr & RST) )
        ;

    pNic->Cr = RPAGE0 | STP;                    /* select page 0 and stop */
    pNic->Dcr = NOTLS | FIFO8;

    pNic->Rbcr0 = 0;                /* clear remote DMA byte count registers */
    pNic->Rbcr1 = 0;

    for (loopy = 0; loopy < MAXXMT; loopy++)
        pDrvCtrl->txDesc[loopy].ready = FALSE;

    pDrvCtrl->pTxNext = pDrvCtrl->txDesc;          /* reset "next Tx" ptr */
    pDrvCtrl->pTxActive = NULL;                    /* no active Tx desc */
    pDrvCtrl->taskRcvActive = FALSE;               /* reset flag */
    pDrvCtrl->NextPkt = CURR;                      /* reset to initial value */

    pNic->Rcr = AB;         /* accept broadcast, but not runt or multicast */
    pNic->Tcr = MODE1;                          /* internal loopback mode */

    pNic->Pstart = PSTART;
    pNic->Pstop  = PSTOP;
    pNic->Bnry   = BNRY;

    pNic->Cr   = RPAGE1;                        /* map in page 1 */
    pNic->Par0 = pDrvCtrl->idr.ac_enaddr [0];
    pNic->Par1 = pDrvCtrl->idr.ac_enaddr [1];
    pNic->Par2 = pDrvCtrl->idr.ac_enaddr [2];
    pNic->Par3 = pDrvCtrl->idr.ac_enaddr [3];
    pNic->Par4 = pDrvCtrl->idr.ac_enaddr [4];
    pNic->Par5 = pDrvCtrl->idr.ac_enaddr [5];
    pNic->nic_pg1.mar0 = 0xff;
    pNic->nic_pg1.mar1 = 0xff;
    pNic->nic_pg1.mar2 = 0xff;
    pNic->nic_pg1.mar3 = 0xff;
    pNic->nic_pg1.mar4 = 0xff;
    pNic->nic_pg1.mar5 = 0xff;
    pNic->nic_pg1.mar6 = 0xff;
    pNic->nic_pg1.mar7 = 0xff;
    pNic->Curr = CURR;
    pNic->Cr = RPAGE0;                          /* back to page 0 */

    pNic->Tcr = MODE0;                          /* put Tx into normal mode */
    pNic->Cr = STA;                             /* start the beast running */
    pNic->Isr = 0xff;                           /* clr any pending ints */
    pNic->Imr = PRXE|PTXE|TXEE|OVWE|RDCE;       /* enable these ints */

    /* Raise the interface flags */

    pDrvCtrl->idr.ac_if.if_flags |= IFF_UP | IFF_RUNNING | IFF_NOTRAILERS;

    }

/*******************************************************************************
*
* nicHandleInt - defered recv interrupt handler
*
* This function handles the received frames from the device.  It runs in the
* context of the netTask, which was triggered by a received packet interrupt.
*/

LOCAL void nicHandleInt
    (
    DRV_CTRL *pDrvCtrl
    )
    {

    do
        {
        pDrvCtrl->taskRcvActive = TRUE;

        while (nicRead (pDrvCtrl))            /* process a received frame */
            ;

        pDrvCtrl->taskRcvActive = FALSE;

        } while (nicRead (pDrvCtrl));
    }

/*******************************************************************************
*
* nicRead - read a packet off the interface
*
* nicRead copies packets from local memory into an mbuf and hands it to
* the next higher layer.
*/

LOCAL BOOL nicRead
    (
    DRV_CTRL *pDrvCtrl
    )
    {
    MBUF *m;
    NIC_DEVICE *pNic;                      /* ptr to the NIC registers */
    RX_FRAME *pRxFrame;                    /* ptr to our frame storage */
    ETH_HDR *eh;                                /* ptr to enet hdr */
    u_int len;                                  /* length of the enet pkt */
    u_short curr;                               /* current page NIC using */
    int oldLevel;

    if (!(pDrvCtrl->idr.ac_if.if_flags & (IFF_UP | IFF_RUNNING)))
        return (FALSE);

    pNic = pDrvCtrl->nicAddr;
    pRxFrame = &pDrvCtrl->rxFrame;
    eh = &pRxFrame->enetHdr;

    /*
     * See if any unprocessed frames in NIC buffer. Unfortunately, there
     * is no reliable way of doing this other than reading the CURR register.
     * The NIC designers, in their infinite wisdom, put this register in
     * page 1. To access page 1, we need to temporarily disable our interrupt
     * handler.
     */

    oldLevel = intLock ();
    pNic->Cr = RPAGE1;                              /* select page 1 */
    curr = pNic->Curr;                              /* get current page */
    pNic->Cr = RPAGE0;                              /* restore page 0 */
    intUnlock (oldLevel);

    if (pDrvCtrl->NextPkt == curr)             /* if NIC has not moved ahead */
        return (FALSE);

    pDrvCtrl->idr.ac_if.if_ipackets++;

    /*
     * OK, there is work to be done.
     * First we copy the NIC receive status header from the NIC buffer
     * into our local area. This is done so that we can obtain the length
     * of the packet before copying out the rest of it. Note that the length
     * field in the NIC header includes the Ethernet header, the data, and
     * the 4 byte FCS field.
     */

    while   (
            pktBufRead  (
                        pDrvCtrl,
                        pDrvCtrl->NextPkt << 8,
                        RX_HDR_SIZ,
                        (char *) pRxFrame
                        )
            == ERROR
            )
        ;

    len = pRxFrame->rxHdr.cntL + (pRxFrame->rxHdr.cntH << 8) - 4 /* FCS */;

#if 0   /* valid frame checks, for paranoia */
    if  (
        ((pRxFrame->rxHdr.status & ~PHY) != PRX) ||
        (pRxFrame->rxHdr.nextRxPage < PSTART) ||
        (pRxFrame->rxHdr.nextRxPage >= PSTOP) ||
        (len < 60) || (len > 1514)
        )
        return (FALSE);
#endif

    /* copy Ethernet packet section of the frame */

    while   (
            pktBufRead  (
                        pDrvCtrl,
                        (pDrvCtrl->NextPkt << 8) + RX_HDR_SIZ,
                        len,
                        (char *) eh
                        )
            == ERROR
            )
        ;

    /* done with frame in NIC buf, so update vars */

    pDrvCtrl->NextPkt = pRxFrame->rxHdr.nextRxPage;/* where dev will use next */
    pNic->Bnry = (pDrvCtrl->NextPkt == PSTART) ?   /* if dev is at beginning */
                (PSTOP - 1) :                     /* boundary is 1 before end */
                (pDrvCtrl->NextPkt - 1);           /* else it's 1 before next */

    /* call input hook if any */

    if (etherInputHookRtn != NULL)
        if  ( (* etherInputHookRtn) (&pDrvCtrl->idr.ac_if, (char *)eh, len))
            return (TRUE);

    len -= sizeof (ETH_HDR);            /* now equals length of data only */

    m = copy_to_mbufs   (
                        pRxFrame->data,
                        len,
                        0,
                        (IFNET *) & pDrvCtrl->idr.ac_if
                        );

    if (m != NULL)
#ifdef BSD43_DRIVER
        do_protocol_with_type (eh->ether_type, m, &pDrvCtrl->idr, len);
#else
        do_protocol (eh, m, &pDrvCtrl->idr, len);
#endif
    else
        pDrvCtrl->idr.ac_if.if_ierrors++;

    return (TRUE);
    }

#ifdef BSD43_DRIVER
/*******************************************************************************
*
* nicOutput - the driver's output routine
*
* This function attempts to copy the data from the mbuf chain to the buffer
* that NIC transmits from.  If successfull, and the NIC is not currently busy
* transmitting, it then attempts to start the NIC transmitting.
*/

LOCAL int nicOutput
    (
    IDR  *pIDR,
    MBUF *pMbuf,
    SOCK *pDest
    )
    {
    int         unit;
    int         oldLevel;
    u_long      nicBufAddr;
    MBUF        *pTempMbuf;
    DRV_CTRL    *pDrvCtrl;
    NIC_DEVICE  *pNic;
    TX_DESC     *pTxNext;                   /* ptr to next desc to use */
    ETH_HDR     enetHdr;                    /* space for the packet header */

    /* init ptrs */

    unit = pIDR->ac_if.if_unit;
    pDrvCtrl = & drvCtrl [unit];
    pNic = pDrvCtrl->nicAddr;          /* copy it, due to frequent use */


    /* Check ifnet flags. Return error if incorrect. */

    if  (
        (pIDR->ac_if.if_flags & (IFF_UP | IFF_RUNNING)) !=
        (IFF_UP | IFF_RUNNING)
        )
        {
        m_freem (pMbuf);                /* release MBUFs */
        return (ENETDOWN);
        }

    /* Attempt to convert socket addr into enet addr and packet type.
     * Note that if ARP resolution of the address is required, the ARP
     * module will call our routine again to transmit the ARP request
     * packet.  This means we may actually call ourselves recursively!
     */

    if  (
        convertDestAddr (
                        pIDR,
                        pDest,
                        &enetHdr.ether_dhost,     /* fill our local header */
                        &enetHdr.ether_type, /* fill our local header */
                        pMbuf
                        )
        == FALSE
        )
        return (OK);

    /* Get next transmit resource */

    pTxNext = pDrvCtrl->pTxNext;        /* copy it, due to frequent use */
    if (pTxNext->ready)                 /* if still in use */
        {
        m_freem (pMbuf);                /* release MBUFs */
        return (ENETDOWN);
        }

    /* Fill in the source address of the Ethernet header */

    bcopy ( (char *) pIDR->ac_enaddr, &enetHdr.ether_shost, 6 );

    /* copy Ethernet header to NIC buffer */

    nicBufAddr = pTxNext->addr;

    while   (
            pktBufWrite (
                        pDrvCtrl,
                        nicBufAddr,
                        ENET_HDR_REAL_SIZ,
                        (char *) & enetHdr
                        )
            == ERROR
            )
            ;

    /* copy mbufs to NIC buffer */

    pTempMbuf = pMbuf;                      /* copy head of MBUF chain */

    for (
        nicBufAddr += (u_long) ENET_HDR_REAL_SIZ;
        pTempMbuf != NULL;
        nicBufAddr += (u_long) pTempMbuf->m_len, pTempMbuf = pTempMbuf->m_next)
        {
        while
            (
            pktBufWrite (
                        pDrvCtrl,
                        nicBufAddr,
                        pTempMbuf->m_len,
                        mtod (pTempMbuf, caddr_t)
                        )
            == ERROR
            )
            ;
        }

    pTxNext->len = max (MINPKTSIZE, (nicBufAddr - pTxNext->addr));

    m_freem (pMbuf);                       /* free the mbufs */

    oldLevel = intLock ();                 /* keep our int handler out */

    pTxNext->ready = TRUE;                 /* mark packet as "ready" */

    if (pDrvCtrl->pTxActive == NULL)       /* if transmitter not active */
        {
        pDrvCtrl->pTxActive = pTxNext;           /* set active ptr */
        pNic->Tpsr = pTxNext->addr >> 8;
        pNic->Tbcr0 = pTxNext->len & 0xff;
        pNic->Tbcr1 = pTxNext->len >> 8;
        pNic->Cr = TXP;                    /* start NIC transmitting */
        }

    pDrvCtrl->pTxNext = pTxNext->next;     /* move ptr to next desc */

    intUnlock (oldLevel);                  /* allow interrupts again */

    /* Bump packet counter */

    pIDR->ac_if.if_opackets++;

    return (0);
    }
#else
/*******************************************************************************
*
* nicTxStartup - the driver's output routine
*
* This function attempts to copy the data from the mbuf chain to the buffer
* that NIC transmits from.  If successful, and the NIC is not currently busy
* transmitting, it then attempts to start the NIC transmitting.
*/

LOCAL int nicTxStartup
    (
    DRV_CTRL * 	pDrvCtrl 	/* pointer to driver control structure */
    )
    {
    MBUF * 	pMbuf;
    int         oldLevel;
    u_long      nicBufAddr;
    MBUF        *pTempMbuf;
    NIC_DEVICE  *pNic;
    TX_DESC     *pTxNext;                   /* ptr to next desc to use */

    pNic = pDrvCtrl->nicAddr;          /* copy it, due to frequent use */

    /* Get next transmit resource */

    while (pDrvCtrl->idr.ac_if.if_snd.ifq_head)
        {
        /* Dequeue a packet from the transmit queue. */
 
        IF_DEQUEUE (&pDrvCtrl->idr.ac_if.if_snd, pMbuf);

        pTxNext = pDrvCtrl->pTxNext;        /* copy it, due to frequent use */
        if (pTxNext->ready)                 /* if still in use */
            {
            m_freem (pMbuf);                /* release MBUFs */
            pDrvCtrl->idr.ac_if.if_oerrors++;
            break;
            }

        /* copy mbufs to NIC buffer */

        pTempMbuf = pMbuf;                      /* copy head of MBUF chain */

        nicBufAddr = pTxNext->addr;

        while (pTempMbuf != NULL)
            {
            while (pktBufWrite (pDrvCtrl, nicBufAddr, pTempMbuf->m_len,
                                mtod (pTempMbuf, caddr_t))
                   == ERROR)
                ;
            nicBufAddr += (u_long) pTempMbuf->m_len;
            pTempMbuf = pTempMbuf->m_next;
            }

        pTxNext->len = max (MINPKTSIZE, (nicBufAddr - pTxNext->addr));

        m_freem (pMbuf);                       /* free the mbufs */

        oldLevel = intLock ();                 /* keep our int handler out */

        pTxNext->ready = TRUE;                 /* mark packet as "ready" */

        if (pDrvCtrl->pTxActive == NULL)       /* if transmitter not active */
            {
            pDrvCtrl->pTxActive = pTxNext;           /* set active ptr */
            pNic->Tpsr = pTxNext->addr >> 8;
            pNic->Tbcr0 = pTxNext->len & 0xff;
            pNic->Tbcr1 = pTxNext->len >> 8;
            pNic->Cr = TXP;                    /* start NIC transmitting */
            }

        pDrvCtrl->pTxNext = pTxNext->next;     /* move ptr to next desc */

        intUnlock (oldLevel);                  /* allow interrupts again */

        /* Bump packet counter */

        pDrvCtrl->idr.ac_if.if_opackets++;
        }

    return (0);
    }
#endif

/*******************************************************************************
*
* nicIoctl - the driver's I/O control routine
*
*/

LOCAL int nicIoctl
    (
    IDR *pIDR,
    int cmd,
    caddr_t data
    )
    {
    int error;

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

/*******************************************************************************
*
* nicEnetAddrGet - get hardwired Ethernet address.
*
* Read the Ethernet address out of the ROM
*/

LOCAL void nicEnetAddrGet
    (
    int unit
    )
    {
    DRV_CTRL *pDrvCtrl = & drvCtrl [unit];
    NIC_DEVICE *pNic = pDrvCtrl->nicAddr;
    int i;

    pNic->Rsar0 = 0;                            /* ID ROM starts at 0 */
    pNic->Rsar1 = 0;
    pNic->Rbcr0 = 6;                            /* length of enet addr */
    pNic->Rbcr1 = 0;
    pNic->Cr = RREAD;                           /* Start NIC Remote DMA */

    for (i = 0; i < 6; i++)
        {
        while (pNic->Rsar0 == i)            /* wait for byte available */
            ;
        pDrvCtrl->idr.ac_enaddr [i] = *NIC_BUF_PORT;  /* read byte from port */
        }
    }

/*******************************************************************************
*
* nicConfig - configure the NIC chip for Ethernet and program address
*/

LOCAL void nicConfig
    (
    int unit
    )
    {
    DRV_CTRL *pDrvCtrl = & drvCtrl [unit];
    NIC_DEVICE *pNic = pDrvCtrl->nicAddr;

    pNic->Cr = RPAGE0 | STP;                    /* select page 0 and stop */
    pNic->Dcr = NOTLS | FIFO8;

    pNic->Rbcr0 = 0;                /* clear remote DMA byte count registers */
    pNic->Rbcr1 = 0;

    pNic->Rcr = AB;         /* accept broadcast, but not runt or multicast */
    pNic->Tcr = MODE1;                          /* internal loopback mode */

    pNic->Pstart = PSTART;
    pNic->Pstop  = PSTOP;
    pNic->Bnry   = BNRY;

    nicEnetAddrGet (unit);                      /* Get Ethernet address */

    /* set up page 1 registers */

    pNic->Cr   = RPAGE1;
    pNic->Par0 = pDrvCtrl->idr.ac_enaddr [0];
    pNic->Par1 = pDrvCtrl->idr.ac_enaddr [1];
    pNic->Par2 = pDrvCtrl->idr.ac_enaddr [2];
    pNic->Par3 = pDrvCtrl->idr.ac_enaddr [3];
    pNic->Par4 = pDrvCtrl->idr.ac_enaddr [4];
    pNic->Par5 = pDrvCtrl->idr.ac_enaddr [5];
    pNic->nic_pg1.mar0 = 0xff;
    pNic->nic_pg1.mar1 = 0xff;
    pNic->nic_pg1.mar2 = 0xff;
    pNic->nic_pg1.mar3 = 0xff;
    pNic->nic_pg1.mar4 = 0xff;
    pNic->nic_pg1.mar5 = 0xff;
    pNic->nic_pg1.mar6 = 0xff;
    pNic->nic_pg1.mar7 = 0xff;
    pNic->Curr = CURR;

    pNic->Cr = RPAGE0;                          /* back to page 0 */
    pNic->Tcr = MODE0;                          /* put Tx into normal mode */
    pNic->Cr = STA;                             /* start the beast running */
    pNic->Isr = 0xff;                           /* clr any pending ints */
    pNic->Imr = PRXE|PTXE|TXEE|OVWE|RDCE;       /* enable these ints */

    }

/*******************************************************************************
*
* pktBufWrite - write data to NIC buffer
*/

LOCAL STATUS pktBufWrite
    (
    DRV_CTRL *pDrvCtrl,
    unsigned long nicBufAddr,
    unsigned long len,
    char *pData
    )
    {
    NIC_DEVICE *pNic;
    WE32104 *pDmac;
    STATUS status;

    if (len == 0)                                       /* nothing to do */
        return (OK);

    pNic = pDrvCtrl->nicAddr;
    pDmac = (WE32104 *) DMA_BASE;

    semTake(pDrvCtrl->dmaSem, WAIT_FOREVER);           /* obtain semaphore */
    pDrvCtrl->dmaWait = TRUE;                          /* set our flag */

    /* set up NIC remote DMA transfer */

    pNic->Rsar0 = nicBufAddr;
    pNic->Rsar1 = nicBufAddr >> 8;
    pNic->Rbcr0 = len;
    pNic->Rbcr1 = len >> 8;

    /* set up main DMAC transfer */

    cacheFlush (DATA_CACHE, pData, len);            /* ensure coherency */
    pDmac->Ch2.SourceAdd = (unsigned long) pData;
    pDmac->Ch2.TranCount = len;
    pDmac->Ch2.Mode = MODE_TT_MP | MODE_DS_LONG | MODE_BR;

    /* start both devices doing the transfer */

    pDmac->Ch2.StatCont = SCR_STR;
    pNic->Cr = RWRITE;

    /* wait for interrupt handler to clear our flag */

    while (pDrvCtrl->dmaWait)
        {
        if  (
            (pDrvCtrl->idr.ac_if.if_flags & (IFF_UP | IFF_RUNNING)) !=
            (IFF_UP | IFF_RUNNING)
            )
            {
            pDrvCtrl->dmaWait = FALSE;
            pDmac->Ch2.StatCont = SCR_SA;               /* abort */
            semGive(pDrvCtrl->dmaSem);                  /* release the sem */
            return (OK);                                /* this is correct */
            }
        }

    /* check for DMAC error */

    if(pDmac->Ch2.StatCont & SCR_ACT)
        {
        pDmac->Ch2.StatCont = SCR_SA;               /* abort */
        status = ERROR;
        }
    else
        status = OK;

    semGive(pDrvCtrl->dmaSem);
    return (status);
    }

/*******************************************************************************
*
* pktBufRead - read data from NIC buffer
*/

LOCAL STATUS pktBufRead
    (
    DRV_CTRL *pDrvCtrl,
    unsigned long nicBufAddr,
    unsigned long len,
    char *pData
    )
    {
    NIC_DEVICE *pNic;
    WE32104 *pDmac;
    STATUS status;

    if (len == 0)                                       /* nothing to do */
        return (OK);

    /* Avoid starting DMA if device is down to to fatal error */

    if  (
        (pDrvCtrl->idr.ac_if.if_flags & (IFF_UP | IFF_RUNNING)) !=
        (IFF_UP | IFF_RUNNING)
        )
        return (OK);

    pNic = pDrvCtrl->nicAddr;
    pDmac = (WE32104 *) DMA_BASE;

    semTake(pDrvCtrl->dmaSem, WAIT_FOREVER);            /* obtain semaphore */
    pDrvCtrl->dmaWait = TRUE;                           /* set our flag */

    /* set up NIC remote DMA transfer */

    pNic->Rsar0 = nicBufAddr;
    pNic->Rsar1 = nicBufAddr >> 8;
    pNic->Rbcr0 = len;
    pNic->Rbcr1 = len >> 8;

    /* set up main DMAC transfer */

    pDmac->Ch2.DestAdd = (unsigned long) pData;
    pDmac->Ch2.TranCount = len;
    pDmac->Ch2.Mode = MODE_TT_PM | MODE_DS_LONG | MODE_BR;

    /* start both devices doing the transfer */

    pDmac->Ch2.StatCont = SCR_STR;
    pNic->Cr = RREAD;

    /* wait for interrupt handler to clear our flag */

    while (pDrvCtrl->dmaWait)
        {
        if  (
            (pDrvCtrl->idr.ac_if.if_flags & (IFF_UP | IFF_RUNNING)) !=
            (IFF_UP | IFF_RUNNING)
            )
            {
            pDrvCtrl->dmaWait = FALSE;
            pDmac->Ch2.StatCont = SCR_SA;               /* abort */
            semGive(pDrvCtrl->dmaSem);                  /* release the sem */
            return (OK);                                /* this is correct */
            }
        }

    /* check for DMAC error */

    if(pDmac->Ch2.StatCont & SCR_ACT)
        {
        pDmac->Ch2.StatCont = SCR_SA;               /* abort */
        status = ERROR;
        }
    else
        {
        cacheInvalidate (DATA_CACHE, pData, len);   /* ensure coherency */
        status = OK;
        }

    semGive(pDrvCtrl->dmaSem);
    return (status);
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
#ifdef	__STDC__
    extern arpresolve   (               /* we call this */
                        IDR *,
                        MBUF *,
                        struct in_addr *,
                        char *,
                        int *
                        );
#else	/* __STDC__ */
    extern arpresolve   ();
#endif	/* __STDC__ */

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
        bcopy (pEnetHdr->dst, pDestEnetAddr, 6);            /* copy dst addr */
        *pPacketType = pEnetHdr->ether_type;                /* copy type */
        return (TRUE);
        }
    }

    /* Unsupported family */

    return (FALSE);
    }
#endif
