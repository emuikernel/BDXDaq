/* if_med.c - Matrix DB-ETH Ethernet network interface driver */

/* "Copyright 1989, Matrix Corporation" */

/* Copyright 1984-1997 Wind River Systems, Inc. */


/*
modification history
--------------------
02m,15jul97,spm  removed driver initialization from ioctl support (SPR #8831); 
                 cleaned up handling of promiscuous mode
02l,07apr97,spm  code cleanup, corrected statistics, and upgraded to BSD 4.4
02k,16dec96,dat  fixed SPR 3026, incorrect test in medNIC_AwaitStop. 
		 Removed compiler warnings.
02j,05may93,caf  tweaked for ansi.
02i,11aug93,jmm  Changed ioctl.h and socket.h to sys/ioctl.h and sys/socket.h
02h,09sep92,gae  documentation tweaks.
02g,26may92,rrr  the tree shuffle
		  -changed includes to have absolute path from h/
02f,04oct91,rrr  passed through the ansification filter
                  -changed functions to ansi style
		  -changed includes to have absolute path from h/
		  -fixed #else and #endif
		  -changed VOID to void
		  -changed copyright notice
02e,20sep90,dab  made medInit() return int.
02d,10aug90,dnw  added include of if_subr.h.
02c,11jul90,hjb  changed references to do_protocol() to use
		 do_protocol_with_type().  de-linted.
02b,26jun90,hjb  upgrade to use cluster and if_subr routines;
	   +dab  integrated in the new distribution from Matrix.
02a,05may90,dab  cleanup of Matrix version 01t.
01a,25sep89,srm  written by modifying if_ln.c.
*/

/*
DESCRIPTION
This module implements the
Matrix DB-ETH Ethernet network interface driver.
There is one user-callable routine: medattach().

BOARD LAYOUT
This device is onboard.  No jumpering diagram required.

SEE ALSO: ifLib
*/

#include "vxWorks.h"
#include "net/mbuf.h"
#include "net/protosw.h"
#include "sys/ioctl.h"
#include "sys/socket.h"
#include "errno.h"
#include "net/uio.h"
#include "net/if.h"
#include "net/route.h"
#include "netinet/in.h"
#include "netinet/in_systm.h"
#include "netinet/ip.h"
#include "netinet/ip_var.h"
#include "netinet/in_var.h"
#include "netinet/if_ether.h"
#include "net/if_subr.h"
#include "etherLib.h"
#include "vme.h"
#include "iv.h"
#include "iosLib.h"
#include "ioLib.h"
#include "memLib.h"
#include "net/systm.h"
#include "sys/types.h"
#include "drv/netif/if_med.h"
#include "intLib.h"
#include "logLib.h"
#include "stdio.h"
#include "sysLib.h"
#include "netLib.h"
#include "net/if_subr.h"

IMPORT void   sysDbusEtherIntEnable ();
IMPORT ULONG  tickGet ();


#define  NIC_INTERRUPTS_HANDLED   (nic_PKT_RECV_NO_ERRORS \
                                 | nic_PKT_RECV_ERROR     \
                                 | nic_PKT_XMIT_NO_ERRORS \
                                 | nic_PKT_XMIT_ERROR     \
                                 | nic_RECV_BFR_OVF_WARN  \
                                 | nic_NET_STATS_CNTR_OVF)

/* Actual arguments for medSetNIC_IMR() */

#define  MASK_NONE               0
#define  UNMASK_NONE             0

/* DB-ETH addresses as seen from the CPU */

#define  ETH_ADRS_PROM_ADRS    ((u_long)dbeth_ETH_ADDR_ROM           \
                                + ((u_long)es->nicAddr & 0xfff00000))
#define  NIC_XMIT_BFR_ADRS     ((u_long)dbeth_NIC_XMIT_BFR           \
                                + ((u_long)es->nicAddr & 0xfff00000))
#define  NIC_RECV_BFR_ADRS(R)  ((u_long)dbeth_NIC_RECV_BFR           \
                                + ((u_long)es->nicAddr & 0xfff00000)    \
                                + ((u_long)(R) << 8))
#define  END_OF_RECV_BFR       ((u_long)dbeth_NIC_RECV_BFR_END       \
                                + ((u_long)es->nicAddr & 0xfff00000))

/* NIC register initial values */

#define PSTART  (u_char) (dbeth_NIC_RECV_BFR >> 8)
#define PSTOP   (u_char) ((dbeth_NIC_RECV_BFR + dbeth_NIC_RECV_BFR_SIZE) >> 8)
#define	BNRY    (u_char) (PSTART)
#define CURR    (u_char) (BNRY + 1)

/* kludge, to force compilation of IP interface code */

#ifndef INET
#define INET
#endif	/* INET */

/* globals */

u_int medLogCount = 50;  /* log every medLogCount number of errors */

/* locals */

LOCAL med_Stats_s       med_Stats;
LOCAL struct med_softc *med_softc [NNIC];


/* forward static functions */

static void medRecv (struct med_softc *es, struct ether_header *eh, int len);
static nic_RecvHdr_s *medGetRecvPacket (struct med_softc *es);
static void medRecvIntrHandler (struct med_softc *es);
static int medIoctl (struct ifnet *ifp, int cmd, caddr_t data);
static void medXmitIntrHandler (struct med_softc *es, u_char xmitStatus);
static void medIntr (int unit);
static void medRecordRecvError (struct med_softc *es);
static void medIntEnable (BOOL bEnable);
static void medReset (int unit);
static int medInit (int unit);
static u_char bitReverse (u_char in);
static void medEthAddrGet (int unit);
static void medConfigNIC (int unit);
#ifdef BSD43_DRIVER
static int medOutput (struct ifnet *ifp, struct mbuf *m0, struct sockaddr
		*dst);
static void medStartOutput (int unit);
#else
static void medStartOutput (struct med_softc *es);
#endif
static xmitBCB_s *medXmitBfrAlloc (struct med_softc *es);
static STATUS medXmitPacket (struct med_softc *es, char *pPacket, u_short
		packetLen);
static void medReadTallyCntrs (struct med_softc *es);
static void medMoveWrappedPacket (struct med_softc *es, nic_RecvHdr_s
		*pRecvPacket, u_long pktSize);
static void medSetNIC_IMR (struct med_softc *es, u_char unmaskBits, u_char
		maskBits);
static void medNIC_AwaitStop (NIC_DEVICE *pNIC);
static void medXmitBfrDealloc (xmitQCB_s *pXmitQCB);
static STATUS medXmtrLock (lockID_t *pLock);
static void medXmtrUnlock (lockID_t *pLock);


/*******************************************************************************
*
* medattach - publish the interface, and initialize the driver and device
*
* The routine publishes `med' the interface by filling in a network interface
* record and adding this record to the system list.  This routine also
* initializes the driver and the device to the operational state.
*
* RETURNS: OK or ERROR
*/

STATUS medattach
    (
    int unit,           /* unit number */
    char *addr,         /* address of Dbus Ethernet board */
    int ivec,           /* interrupt vector to connect to */
    int ilevel          /* interrupt level */
    )
    {
    FAST struct med_softc *es;
    FAST NIC_DEVICE *pNicDev = (NIC_DEVICE *) (addr + (u_long) dbeth_NIC_ADDR);

    /* allocate and initialize med descriptor (med_softc) */

    if ((es = (struct med_softc *) calloc (1, sizeof (struct med_softc)))
        == NULL)
        {
        printErr ("med%d: ERROR - malloc failed.\n", es->es_if.if_unit);
	return (ERROR);
        }

    med_softc [unit] = es;	/* set ptr to med descriptor in array */

    /* allocate and initialize overflow buffer for wrapped receive packets */

    if ((es->pOvfRecvBfr = (nic_RecvHdr_s *)calloc (1, OVERFLOW_BFR_SIZE)) == 0)
        {
        printErr ("med%d: ERROR - malloc failed.\n", es->es_if.if_unit);
	return (ERROR);
        }

    es->bFirstInit  = TRUE;            /* Unit 'unit' not yet initialized */
    es->nicAddr     = pNicDev;
    es->nicIntLevel = ilevel;
    es->nicIntVec   = ivec;

    (void) intConnect (INUM_TO_IVEC (ivec), medIntr, unit);

    medReset (unit);

#ifdef BSD43_DRIVER
    ether_attach ( (struct ifnet *)&es->es_if, unit, "med",
		  medInit, medIoctl, medOutput, (FUNCPTR) medReset);
#else
    ether_attach (
                 &es->es_if,
                 unit, 
                 "med",
		 (FUNCPTR) medInit, 
                 (FUNCPTR) medIoctl, 
                 (FUNCPTR) ether_output, 
                 (FUNCPTR) medReset
                 );
    es->es_if.if_start = (FUNCPTR)medStartOutput;
#endif

    medIntEnable (TRUE);      /* enable the DB-ETH interrupt */

    medInit (unit);

    return (OK);
    }

/*******************************************************************************
*
* medRecv - receive packet processing
*
* Process Ethernet receive completion:
* If input error, just drop packet. (This is handled in medRecvIntrHandler.)
* Otherwise purge input buffered data path and examine
* packet to determine type.  If can't determine length
* from type, then have to drop packet.  Otherwise decapsulate
* packet based on type and pass to type-specific higher-level
* input routine.
*/

LOCAL void medRecv
    (
    FAST struct med_softc *es,
    FAST struct ether_header *eh,   /* ether header followed by packet data */
    int len
    )
    {
    FAST struct mbuf	      *m;
    FAST u_char		      *pData;
    int		       	       off;


    es->es_if.if_ipackets++;

    /* call input hook if any */

    if (etherInputHookRtn != NULL &&
	(*etherInputHookRtn) (&es->es_if, (char *) eh, len))
	{
	return;		/* input hook has already processed this packet */
	}

#ifdef BSD43_DRIVER

    /* This legacy code is not correct for the BSD 4.4 stack. It would
     * also treat multicast addresses like alien packets, and not send
     * them up the stack. The higher-level protocols in the new stack
     * can handle these addresses, and will discard them if they do not
     * match an existing multicast group.
     */

    /* do software filter if controller is in promiscuous mode */

    if (es->bPromisc)
	{
	if ((bcmp ((char *) eh->ether_dhost,	/* not our adrs? */
		     (char *) es->es_enaddr,
		     sizeof (eh->ether_dhost)) != 0) &&
	    (bcmp ((char *) eh->ether_dhost,	/* not broadcast? */
		     (char *) etherbroadcastaddr,
		     sizeof (eh->ether_dhost)) != 0))
	    {
	    return;				/* not for us */
	    }
	}
#endif

    if (len >= sizeof (struct ether_header))
        len -= sizeof (struct ether_header);
    else
        len = 0;

    pData = ((u_char *) eh) + (sizeof (struct ether_header));

#ifdef BSD43_DRIVER
    /*
     * Deal with trailer protocol: if type is trailer
     * get true type from first 16-bit word past data.
     * Remember that type was trailer by setting off.
     */

    check_trailer (eh, pData, &len, &off, &es->es_if);

    if (len == 0)
        return;					/* sanity */
#endif

    m = copy_to_mbufs (pData, len, off, (struct ifnet *) &es->es_if);

    if (m == NULL)
        {
        es->es_if.if_ierrors++;
	return;
        }

#ifdef BSD43_DRIVER
    do_protocol_with_type (eh->ether_type, m, &es->es_ac, len);
#else
    do_protocol (eh, m, &es->es_ac, len);
#endif

    return;
    }

/*******************************************************************************
*
* medGetRecvPacket - return the address of the next packet received
*
* If a packet is available in the NIC receive buffer, return its address.
* Otherwise, return NULL.
*/

LOCAL nic_RecvHdr_s *medGetRecvPacket
    (
    FAST struct med_softc  *es
    )
    {
    FAST u_char          currNIC_WritePage;
    FAST NIC_DEVICE     *pNIC = es->nicAddr;
    FAST int             oldLevel;

    /*
     * Select NIC register page 1, and retrieve the CURR register,
     * re-select NIC register page 0.
     */

    oldLevel = intLock();
    pNIC->Cr = (u_char) (RPAGE1 | ABORT | STA);

    currNIC_WritePage = pNIC->Curr;

    pNIC->Cr = (u_char) (RPAGE0 | ABORT | STA);
    intUnlock (oldLevel);

    if (es->pktNextToRead == currNIC_WritePage)  /* No new packets. */
        {
        return ((nic_RecvHdr_s *) NULL);
        }

    return ((nic_RecvHdr_s *) NIC_RECV_BFR_ADRS (es->pktNextToRead));
    }

/*******************************************************************************
*
* medRecvIntrHandler - task level interrupt service for input packets
*
* This routine is called at task level indirectly by the interrupt
* service routine to do any message received processing.
*/

LOCAL void medRecvIntrHandler
    (
    FAST struct med_softc  *es
    )
    {
    FAST nic_RecvHdr_s  *pRecvPacket;
    FAST u_char          recvStatus;
    FAST u_long          pktSize;
    FAST NIC_DEVICE     *pNIC = es->nicAddr;

    if (es->bRecvBfrOvfl)  /* Ignore packets on receive buffer overflow; */
        return;            /* they all will be dropped.                  */

    while (TRUE)
        {
        while ((pRecvPacket = medGetRecvPacket (es)) != (nic_RecvHdr_s *) NULL)
            {
            if (es->bRecvBfrOvfl)
                return;

            recvStatus = pRecvPacket->recvStatus;
            if  ((recvStatus & PRX) == nic_RECV_NO_ERROR)
                {
                pktSize = (u_long) pRecvPacket->byteCntL
                        | (u_long) (pRecvPacket->byteCntH << 8);

                /*
                 * If the packet is wrapped around in the receive buffer,
                 * move it to a contiguous buffer.
                 */

                if (((u_long) pRecvPacket + pktSize) > (u_long) END_OF_RECV_BFR)
                    {
                    medMoveWrappedPacket (es, pRecvPacket, pktSize);
                    medRecv (es, &(es->pOvfRecvBfr->eh), (int) pktSize);
                    }
                else
                    medRecv (es, &(pRecvPacket->eh), (int) pktSize);

                es->pktNextToRead = pRecvPacket->nextPkt;

                /*
                 * Ensure that the "boundry" register is maintained one page
                 * behind the "current"  register.
                 */

                if (es->pktNextToRead ==
		    ((u_char) ((u_short) dbeth_NIC_RECV_BFR) >> 8))
		    {
                    pNIC->Bnry = PSTOP - 1;
		    }
                else
                    pNIC->Bnry = es->pktNextToRead - 1;
                }
            else
                {
                if ((recvStatus & DIS) == nic_RCVR_DISABLED)
                   logMsg("med%d: receiver disabled\n",
			es->es_if.if_unit, 0,0,0,0,0);

                if ((recvStatus & DFR) == nic_RCVR_DEFERRING)
                   logMsg("med%d: receiver deferring\n", 
			es->es_if.if_unit, 0,0,0,0,0);
                }

            }   /* while ((pRecvPacket ... */

        if (pNIC->Isr & PRX)           /* pending recv interrupt ?   */
            {
            pNIC->Isr = PRX;           /* clear interrupt flag       */
            med_Stats.PRX_intrCount++; /* update statistics monitor  */
            }
        else if (pNIC->Isr & RXE)      /* pending recv error interrupt ? */
            {
            pNIC->Isr = RXE;           /* clear interrupt flag       */
            medRecordRecvError (es);
            }
        else
            {                          /* no recv interrupt          */
            /* Re-enable PRX and RXE interrupts. */
            medSetNIC_IMR (es,  nic_PKT_RECV_NO_ERRORS
                              | nic_PKT_RECV_ERROR, MASK_NONE);
            break; /* break from while (TRUE) */
            }

        }       /* while (TRUE)            */
    }

/*******************************************************************************
*
* medIoctl - process an ioctl request.
*/

LOCAL int medIoctl
    (
    FAST struct ifnet *ifp,
    int cmd,
    caddr_t data
    )
    {
    int 			unit  = ifp->if_unit;
    FAST struct med_softc * 	es = med_softc [unit];
    int 			s     = splimp ();
    int 			error = 0;
    short 			flags;

    switch (cmd)
	{
	case SIOCSIFADDR:
            ((struct arpcom *)ifp)->ac_ipaddr = IA_SIN (data)->sin_addr;
            arpwhohas (ifp, &IA_SIN (data)->sin_addr);
	    break;

	case SIOCGIFADDR:
	    bcopy((caddr_t) es->es_enaddr,
		  (caddr_t) ((struct ifreq *) data)->ifr_addr.sa_data, 6);
	    break;

	case SIOCGIFFLAGS:
	    *(short *) data = ifp->if_flags;
	    break;

	case SIOCSIFFLAGS:
	    {
            flags = ifp->if_flags;

#ifdef BSD43_DRIVER
	    if (ifp->if_flags & IFF_PROMISC)
	        es->bPromisc = TRUE;
	    else
	        es->bPromisc = FALSE;
#endif
	    if (ifp->if_flags & IFF_UP)
	        flags |= (IFF_UP | IFF_RUNNING);
	    else
	        flags &= ~(IFF_UP | IFF_RUNNING);

	    ifp->if_flags = flags;
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
* medXmitIntrHandler - handler for transmit interrupts
*
*/

LOCAL void medXmitIntrHandler
    (
    FAST struct med_softc *es,
    FAST u_char xmitStatus
    )
    {
    FAST xmitQCB_s  * pXmitQCB = &(es->xmitQCB);
    FAST XBIdx_t      headPkt;

    /*
     * The following flags indicate packet transmission error:
     *
     *   ABT - excess collisions, transmission was aborted
     *   FU  - NIC FIFO underrun, transmission was aborted (should never occur)
     *
     * Retransmit the packet.
     *
     * The following flags indicate a collision occurred but the packet
     * was eventually transmitted.
     *
     *   CRS - carrier lost (lost due to a collision)
     *   OWC - out-of-window-collision
     *
     */
    if (xmitStatus & (ABT | FU))
        {
        es->es_if.if_oerrors++;            /* log transmit error */
        if (xmitStatus & ABT)
            {
            med_Stats.excessColl++;        /* update statistics monitor */
	    logMsg("med%d: ERROR - excessive collisions, transmission aborted\n"
                   , es->es_if.if_unit, 0,0,0,0,0);
            }
        if (xmitStatus & FU)
            {
            med_Stats.FIFO_UdrRun++;       /* update statistics monitor */
            logMsg("med%d: ERROR - NIC FIFO underrun, transmission aborted\n",
                   es->es_if.if_unit, 0,0,0,0,0);
            }

        headPkt = pXmitQCB->head;
        if (medXmitPacket (es,
                           pXmitQCB->xmitBCB [headPkt].pPktBfr,
                           pXmitQCB->xmitBCB [headPkt].pktLen) == ERROR)
            logMsg("med%d: ERROR - NIC transmitter active, should be off.\n",
                   es->es_if.if_unit, 0,0,0,0,0);
        }
    else  /* no transmission error */
        {
        if (xmitStatus & CRS)
            med_Stats.carrierLost++;      /* update statistics monitor */

        if (xmitStatus & OWC)
            med_Stats.outOfWndColl++;     /* update statistics monitor */

        es->es_if.if_opackets++;          /* log packet output      */

        pXmitQCB->xmitBCB [pXmitQCB->head].bPktRdy = FALSE;
        medXmitBfrDealloc (pXmitQCB);     /* could be a macro        */
        headPkt = pXmitQCB->head;         /* new head index after buf dealloc */

        if (pXmitQCB->xmitBCB [headPkt].bPktRdy)
            {
            if (medXmitPacket (es,
                               pXmitQCB->xmitBCB [headPkt].pPktBfr,
                               pXmitQCB->xmitBCB [headPkt].pktLen) == ERROR)
                logMsg("med%d: ERROR - NIC transmitter active, should be off.\n"
		       , es->es_if.if_unit, 0,0,0,0,0);
            }
        else
            {
            medXmtrUnlock (&(pXmitQCB->lockNIC_Xmtr));
            if (es->es_if.if_snd.ifq_head != NULL) /* if packet(s) on queue   */
#ifdef BSD43_DRIVER
                netJobAdd ( (FUNCPTR)medStartOutput, es->es_if.if_unit,
                           0, 0, 0, 0);
#else
                netJobAdd ( (FUNCPTR)medStartOutput, (int)es, 0, 0, 0, 0);
#endif
            }
        }
    }

/*******************************************************************************
*
* medIntr - top level DB-ETH interrupt service routine
*/

LOCAL void medIntr
    (
    FAST int unit
    )
    {
    FAST u_char            intrStatus;
    FAST struct med_softc *es   = med_softc [unit];
    FAST NIC_DEVICE       *pNIC = es->nicAddr;

    med_Stats.intrCount++;

    /* retrieve interrupt status */

    intrStatus = pNIC->Isr;

    if (intrStatus & OVW)              /* overflow warning, no recv buffers */
        {
        medSetNIC_IMR (es, UNMASK_NONE,  nic_RECV_BFR_OVF_WARN
                                       | nic_PKT_RECV_NO_ERRORS
                                       | nic_PKT_RECV_ERROR);

        es->bRecvBfrOvfl = TRUE;           /* flag for medRecvIntrHandler() */
        intrStatus &= ~(OVW | PRX | RXE);  /* ignore these interrupts */
        logMsg ("med%d: receive buffer overflow, all buffers cleared.\n",
		unit, 0,0,0,0,0);
        med_Stats.OVW_intrCount++;         /* update statistics monitor */
        (void) netJobAdd (medInit, unit, 0,0,0,0);
        }

    if (intrStatus & PRX)              /* packet reception, no error */
        {
        /*
         * Handle packet reception interrupts (with and without errors)
         * at task level, until none remaining.
         * Mask further PRX (nic_PKT_RECV_NO_ERRORS) interrupts.
         * Mask further RXE (nic_PKT_RECV_ERROR) interrupts.
         */

        medSetNIC_IMR (es, UNMASK_NONE,  nic_PKT_RECV_NO_ERRORS
                                       | nic_PKT_RECV_ERROR);
        pNIC->Isr = PRX;               /* clear interrupt flag */
        med_Stats.PRX_intrCount++;     /* update statistics monitor */
        (void) netJobAdd ((FUNCPTR)medRecvIntrHandler, (int)es, 0,0,0,0);
        }

    if (intrStatus & PTX)              /* packet transmission, not aborted */
        {
        pNIC->Isr = PTX;               /* clear interrupt flag */
        med_Stats.PTX_intrCount++;     /* update statistics monitor */
        medXmitIntrHandler(es, pNIC->Tsr);
        }

    if (intrStatus & TXE)              /* packet transmission aborted */
        {
        pNIC->Isr = TXE;               /* clear interrupt flag       */
        med_Stats.TXE_intrCount++;     /* update statistics monitor  */
        medXmitIntrHandler(es, pNIC->Tsr);
        }

    if (intrStatus & RXE)              /* packet received with error(s) */
        {
        pNIC->Isr = RXE;               /* clear interrupt flag       */
        medRecordRecvError (es);
        }

    if (intrStatus & CNT)              /* tally counter MSB set      */
        {
        pNIC->Isr = CNT;               /* clear interrupt flag       */
        med_Stats.CNT_intrCount++;     /* update statistics monitor  */
        (void) netJobAdd ((FUNCPTR)medReadTallyCntrs, (int)es,0,0,0,0);
        }
    }

/*******************************************************************************
*
* medRecordRecvError - record packet reception errors
*/

LOCAL void medRecordRecvError
    (
    FAST struct med_softc *es
    )
    {
    med_Stats.RXE_intrCount++;     /* update statistics monitor  */

    /*
     * Packets received with errors are not saved. Therefore,
     * es->pktNextToRead is not updated.
     */

    es->es_if.if_ierrors++;

    /* log every medLogCount errors */

    if (medLogCount > 0 && (es->es_if.if_ierrors % medLogCount) == 0)
        logMsg ("med%d: receive error\n",
		es->es_if.if_unit,0,0,0,0,0);
    }

/*******************************************************************************
*
* medIntEnable - enables/disables the DB-ETH interrupt
*
*/

LOCAL void medIntEnable
    (
    BOOL bEnable
    )
    {
    int oldLevel;

    /*
     * In an attempt to avoid spurious interrupts
     * on Dbus interrupt disable...
     */

    if (!bEnable)
        {
        oldLevel = intLock ();
        sysDbusEtherIntEnable (bEnable);
        intUnlock (oldLevel);
        }
    else
        sysDbusEtherIntEnable (bEnable);
    }

/*******************************************************************************
*
* medReset - reset the interface
*
* Mark interface as inactive and reset the NIC.
*/

LOCAL void medReset
    (
    int unit
    )
    {
    struct med_softc  *es   = med_softc [unit];
    NIC_DEVICE        *pNIC = es->nicAddr;

    es->es_if.if_flags &= ~IFF_RUNNING;

    /*
     * The following sequence is required when disabling the NIC from
     * an active network. (This sequence was derived from the DP8390
     * Datasheet Addendum, dated December, 1988, section 2.1.)
     */

    pNIC->Cr = RPAGE0 | STP | ABORT;

    /* clear remote DMA byte count registers */

    pNIC->Rbcr0 = 0;
    pNIC->Rbcr1 = 0;

    /* if the NIC not yet stopped, wait for approximately 2ms */

    if ((pNIC->Isr & RST) != nic_STOP_MODE)
        medNIC_AwaitStop (pNIC);
    }

/*******************************************************************************
*
* medInit - initialize NIC.
*
* Restart the NIC chip and mark the interface as up.
*
*/

LOCAL int medInit
    (
    int unit
    )
    {
    struct med_softc *es  = med_softc [unit];
    struct ifnet     *ifp = &es->es_if;

    medIntEnable (FALSE);     /* disable DB-ETH interrupt */
    ifp->if_flags &= ~(IFF_UP | IFF_RUNNING | IFF_PROMISC);

    /* stop all operations, reset the NIC */

    medReset (unit);

    /* Clear all driver statistics on first initialization, but
     * not on subsequent calls of medInit(), e.g. after receive
     * buffer overflow.
     */

    if (es->bFirstInit)
        {
        bzero((char *) &med_Stats, sizeof (med_Stats));
        es->bFirstInit = FALSE;
        }

    /* configure and start NIC operations */

    medConfigNIC (unit);

    /* initialize the NIC IMR shadow register */

    es->currentIMR = NIC_INTERRUPTS_HANDLED;

    /* flag for medRecvIntrHandler(), no receive buffer overflow */

    es->bRecvBfrOvfl = FALSE;

    /*
     * Initialize the "next-packet-to-read" pointer.
     * The NIC's remote DMA capability is not supported by the DB-ETH.
     * Therefore, the BNRY register is not maintained by the NIC, and
     * it must be maintained by software. However, BNRY must never
     * equal CURR.
     *
     * pktNextToRead: This page pointer is required to determine where
     * to retrieve the next received packet.
     */

    es->pktNextToRead = CURR;

    /*
     * Initialize the xmitQCB and xmitBfrCB structures.
     */
    {
    xmitQCB_s *pXmitQCB = &(es->xmitQCB);
    int        idx;

    pXmitQCB->head   = 0;
    pXmitQCB->tail   = 0;
    pXmitQCB->bEmpty = TRUE;
    pXmitQCB->bFull  = FALSE;

    for (idx = 0; idx < NUM_XMIT_BFRS; idx++)
        {
        pXmitQCB->xmitBCB [idx].bPktRdy = FALSE;
        pXmitQCB->xmitBCB [idx].pPktBfr
             = (char *) (NIC_XMIT_BFR_ADRS + XMIT_BFR_SIZE * idx);
        }

    for (idx = 1; idx < NUM_XMIT_BFRS; idx++)
        pXmitQCB->xmitBCB [idx - 1].next = idx;

    /*
     * Close the ring. Assign the next index parameter of the last buffer
     * control block to the index of the first buffer control block in
     * the ring.
     */
    pXmitQCB->xmitBCB [NUM_XMIT_BFRS - 1].next = 0;

    /* allow access to the NIC transmitter */

    medXmtrUnlock (&(pXmitQCB->lockNIC_Xmtr));
    }

    ifp->if_flags |= (IFF_UP | IFF_RUNNING);

#ifdef BSD43_DRIVER
    if (es->bPromisc)
	ifp->if_flags |= (IFF_PROMISC);
#endif

    medIntEnable (TRUE);

    return (0);
    }

/*******************************************************************************
*
* bitReverse - reverse the position of the bits in a byte
*
*/

LOCAL u_char  bitReverse
    (
   u_char in
    )
    {
    int    bit;
    u_char fromBitMask;
    u_char toBitMask;
    u_char out = 0;

    fromBitMask = 0x01;
    toBitMask   = 0x80;

    for (bit = 0; bit < 8; bit++)
	{
        if (in & fromBitMask)
            out |= toBitMask;
        fromBitMask = fromBitMask << 1;
        toBitMask   = toBitMask   >> 1;
	}

    return (out);
    }

/*******************************************************************************
*
* medEthAddrGet - Get hardwired ethernet address from DB-ETH.
*
* Read the ethernet address out of the ROM, one byte at a time.
* put it in med_softc
*
* INTERNAL
* The DB-ETH prototype boards have the 8 data lines reversed to the
* Ethernet address PROM. Using the assigned Ethernet address (1st 3
* octets), this function determines if the data lines are reversed on
* the DB-ETH board it is controling and makes the appropriate
* conversion if necessary. Note that this function will fail if each
* of the 3 octets are made-up of nybbles which are mirror images of
* each other. For example, the following bytes (in hex):
* 00, 81, 66, 99, E7, ... The third octet assigned to Matrix Corp.
* is 0x0b. This octet in PROM will be determine whether to reverse
* the data values.
*/

LOCAL void medEthAddrGet
    (
    int unit
    )
    {
    u_char             octet;
    u_int              octetSel;
    BOOL               bReversed;
    struct  med_softc *es = med_softc [unit];
    u_char            *pEthOctet = (u_char *) (ETH_ADRS_PROM_ADRS);

    /* XXX optimize by saving result in static variable then just test var. ? */

    bReversed = FALSE;
    if (*(pEthOctet + 2) != 0x0b)
        bReversed = TRUE;

    for (octetSel = 0; octetSel < 6; octetSel++ )
        {
        octet = *pEthOctet;
        if (bReversed)
            octet = bitReverse (octet);
        es->es_enaddr [octetSel] = octet;
        pEthOctet++;
        }
    }

/*******************************************************************************
*
* medConfigNIC - Configure the NIC for operation
*
* INTERNAL
* This NIC initialization/configuration sequence was derived from the
* DP8390 Datasheet Addendum, dated December, 1988, section 2.0. This
* configuration enables the NIC onto an active network.
*/

LOCAL void medConfigNIC
    (
    int unit
    )
    {
    struct med_softc  *es   = med_softc [unit];
    NIC_DEVICE        *pNIC = es->nicAddr;

    /* get Ethernet address from ROM */

    medEthAddrGet (unit);

    /* stop/abort all NIC operations */

    pNIC->Cr  = RPAGE0 | STP | ABORT;
    pNIC->Dcr = FT2 | BMS | BOS | WTS;

    /* clear remote DMA byte count registers */

    pNIC->Rbcr0  = 0;
    pNIC->Rbcr1  = 0;

    /* accept broadcast, but not runt and multicast packets */

    pNIC->Rcr = AB;

    /* configure for internal loopback mode */

    pNIC->Tcr = MODE1;

    /* initialize receive buffer pointers */

    pNIC->Bnry   = BNRY;
    pNIC->Pstart = PSTART;
    pNIC->Pstop  = PSTOP;

    /* clear the interrupt status */

    pNIC->Isr = RST | RDC | CNT | OVW | TXE | RXE | PTX | PRX;

    /* enable selected interrupts */

    pNIC->Imr = NIC_INTERRUPTS_HANDLED;

    /* select NIC page 1 registers */
    pNIC->Cr  = RPAGE1 | STP | ABORT;

    /* initialize the physical address, e.g. the Ethernet address */

    pNIC->Par0 = es->es_enaddr [0];
    pNIC->Par1 = es->es_enaddr [1];
    pNIC->Par2 = es->es_enaddr [2];
    pNIC->Par3 = es->es_enaddr [3];
    pNIC->Par4 = es->es_enaddr [4];
    pNIC->Par5 = es->es_enaddr [5];

    /*
     * Initialize the multicast address registers.
     *    CURRENTLY NOT SUPPORTED
     */

    /* initialize the next packet receive buffer pointer */

    pNIC->Curr = CURR;

    /* start NIC */

    pNIC->Cr = RPAGE0 | STA | ABORT;

    /* enable packet transmission */

    pNIC->Tcr = nic_NORMAL_XMIT_CTRL;
    }

#ifdef BSD43_DRIVER
/*******************************************************************************
*
* medOutput -
*
* Ethernet output routine.
* Encapsulate a packet of type family for the local net.
* Use trailer local net encapsulation if enough data in first
* packet leaves a multiple of 512 bytes of data in remainder.
*/

LOCAL int medOutput
    (
    FAST struct ifnet   *ifp,
    FAST struct mbuf    *m0,
    struct sockaddr     *dst
    )
    {
    return (ether_output (ifp, m0, dst, (FUNCPTR) medStartOutput,
			  &med_softc [ifp->if_unit]->es_ac));
    }
#endif

/*******************************************************************************
*
* medStartOutput - start pending output
*
* Start output to NIC.
* Queue up all pending datagrams for which transmit buffers are available.
* It is very important that this routine be executed with splimp set.
* If this is not done, another task could allocate the same transmit buffer.
*/
#ifdef BSD43_DRIVER
LOCAL void medStartOutput
    (
    int unit
    )
    {
    FAST struct med_softc * 	es = med_softc [unit];
#else
LOCAL void medStartOutput
    (
    struct med_softc * 	es
    )
    {
#endif
    FAST struct mbuf	  *m;
    FAST char		  *buf;
    int			   len;
    FAST xmitBCB_s        *pXmitBCB;
    int			   s = splimp ();
    FAST xmitQCB_s  * pXmitQCB = &(es->xmitQCB);
    FAST XBIdx_t      headPkt  = pXmitQCB->head;

    while (es->es_if.if_snd.ifq_head != NULL)
        {
	/* there is something to send */

        if ((pXmitBCB = medXmitBfrAlloc (es)) == (xmitBCB_s *) NULL)
            {
            med_Stats.noXmitBfrs++;
            es->es_if.if_oerrors++;
	    break;			/* no transmit buffers available */
	    }

        buf  = pXmitBCB->pPktBfr;

	/* get message to send */

	IF_DEQUEUE (&es->es_if.if_snd, m); /* get head of next mbuf chain */
	copy_from_mbufs (buf, m, len);

	len = max (ETHERSMALL, len);

        pXmitBCB->pktLen  = (u_short)len;
        pXmitBCB->bPktRdy = TRUE;

	/* call output hook if any */

	if (etherOutputHookRtn != NULL &&
	     (* etherOutputHookRtn) (&es->es_if, buf, len))
            continue;

        if (medXmtrLock (&(pXmitQCB->lockNIC_Xmtr)) != ERROR)
            {
            /*
             * Start transmission of the next packet on the xmitQ
             * unless the transmit interrupt handler has already
             * transmitted it.
             *
             * Lock mechanism  pXmitQCB->lockNIC_Xmtr is unlocked by
             * the transmit interrupt handler when all packets on
             * the xmitQ have been transmitted.
             */
            if (pXmitQCB->xmitBCB [headPkt].bPktRdy)
                {
                if (medXmitPacket (es, pXmitQCB->xmitBCB [headPkt].pPktBfr,
                                   pXmitQCB->xmitBCB [headPkt].pktLen)
                    == ERROR)
                    {
                    logMsg ("med%d: ERROR - NIC transmitter active, should \
                             be off.\n", es->es_if.if_unit, 0, 0, 0, 0, 0);
                    }
                }
            else
                medXmtrUnlock (&(pXmitQCB->lockNIC_Xmtr));
            }
        /* opackets statistic is incremented elsewhere after interrupt */
        }
    splx (s);
    }

/*******************************************************************************
*
* medXmitBfrAlloc - allocate a NIC transmit buffer
*
* RETURNS: address of the NIC transmit buffer or NULL if none available
*/

LOCAL xmitBCB_s   *medXmitBfrAlloc
    (
    struct med_softc  *es
    )
    {
    FAST xmitQCB_s *pXmitQCB = &(es->xmitQCB);
    FAST xmitBCB_s *pTailXBCB;
    int             oldLevel;

    if (pXmitQCB->bFull)
        return (xmitBCB_s *)NULL;

    /* block interrupt service routine access to xmitQCB */

    oldLevel = intLock();

    pXmitQCB->bEmpty = FALSE;
    pTailXBCB        = &(pXmitQCB->xmitBCB [pXmitQCB->tail]);
    pXmitQCB->tail   = pTailXBCB->next;
    if (pXmitQCB->head == pXmitQCB->tail)
        pXmitQCB->bFull = TRUE;

    intUnlock (oldLevel);
    return (pTailXBCB);
    }

/*******************************************************************************
*
* medXmitPacket - NIC packet transmission
*
* RETURNS: ERROR if NIC transmitter busy, otherwise OK
*/

LOCAL STATUS   medXmitPacket
    (
    struct med_softc *es,
    char *pPacket,
    u_short packetLen
    )
    {
    NIC_DEVICE *pNIC = es->nicAddr;

#if	defined (med_SAVE_XMIT_PKTS)
    static char *pXmitPktSaveBfr = (char *) 0xffd10000;

    /* check for possible buffer overrun */

    if (((u_long) pXmitPktSaveBfr + 1600) > 0xffd20000)
	pXmitPktSaveBfr = (char *) 0xffd10000;

    bcopy (pPacket, pXmitPktSaveBfr, (int) packetLen);
    pXmitPktSaveBfr = pXmitPktSaveBfr + packetLen;
    bcopy ("$$$$$$$$$$$$$$$$", pXmitPktSaveBfr, 16);
    pXmitPktSaveBfr = pXmitPktSaveBfr + 16;
#endif	/* med_SAVE_XMIT_PKTS */

    if ((pNIC->Cr & TXP) == nic_XMTR_BUSY)
        return (ERROR);

    pNIC->Tpsr  = nic_CPU_TO_NIC_ADRS (pPacket);
    pNIC->Tbcr1 = (u_char) (packetLen >> 8);
    pNIC->Tbcr0 = (u_char) packetLen;
    pNIC->Cr    = STA | TXP | ABORT;

    return (OK);
    }

/*******************************************************************************
*
* medStatsGet - output to stdout the med statictics
*
* NOMANUAL
*/

void medStatsGet
    (
    int unit
    )
    {
    struct med_softc *es = med_softc [unit];

    medReadTallyCntrs (es);

    printf("med Statistics:\n");
    printf("   packets received            : %ld\n", es->es_if.if_ipackets);
    printf("   input errors                : %ld\n", es->es_if.if_ierrors);
    printf("   packets transmitted         : %ld\n", es->es_if.if_opackets);
    printf("   output errors               : %ld\n", es->es_if.if_oerrors);
    printf("   packets on send queue       : %d\n", es->es_if.if_snd.ifq_len);
    printf("   packets dropped (not xmit)  : %d\n", es->es_if.if_snd.ifq_drops);
    printf("   xmit bufs full, xmit delayed: %ld\n", med_Stats.noXmitBfrs);
    printf("   NIC total interrupts        : %ld\n", med_Stats.intrCount);
    printf("   NIC PRX interrupts          : %ld\n", med_Stats.PRX_intrCount);
    printf("   NIC PTX interrupts          : %ld\n", med_Stats.PTX_intrCount);
    printf("   NIC RXE interrupts          : %ld\n", med_Stats.RXE_intrCount);
    printf("   NIC TXE interrupts          : %ld\n", med_Stats.TXE_intrCount);
    printf("   NIC OVW interrupts          : %ld\n", med_Stats.OVW_intrCount);
    printf("   NIC CNT interrupts          : %ld\n", med_Stats.CNT_intrCount);
    printf("   carrier lost on xmit        : %ld\n", med_Stats.carrierLost);
    printf("   out of window collision     : %ld\n", med_Stats.outOfWndColl);
    printf("   FIFO underrun               : %ld\n", med_Stats.FIFO_UdrRun);
    printf("   excess collision            : %ld\n", med_Stats.excessColl);
    printf("   frame alignment             : %ld\n", med_Stats.frameAlign);
    printf("   CRC mismatch                : %ld\n", med_Stats.misMatchCRC);
    printf("   missed packet               : %ld\n", med_Stats.missedPkt);
    }

/*******************************************************************************
*
* medReadTallyCntrs - update local counters from NIC tally counters
*
*/

LOCAL void medReadTallyCntrs
    (
    FAST struct med_softc *es
    )
    {
    NIC_DEVICE *pNIC = es->nicAddr;

    med_Stats.frameAlign  += pNIC->FaeErr;
    med_Stats.misMatchCRC += pNIC->CrcErr;
    med_Stats.missedPkt   += pNIC->MspErr;
    }

/*******************************************************************************
*
* medMoveWrappedPacket - move a wrapped packet in the NIC receive buffer
*
*/

LOCAL void medMoveWrappedPacket
    (
    FAST struct med_softc  *es, /* for END_OF_RECV_BFR, NIC_RECV_BFR_ADRS */
    FAST nic_RecvHdr_s *pRecvPacket,
    FAST u_long pktSize
    )
    {
    FAST int    byteCount;
    FAST char  *pOvfRecvBfrMiddle;

    /* copy from start of packet to end of NIC receive buffer */

    byteCount = (int) ((u_long) END_OF_RECV_BFR - (u_long) pRecvPacket);
    bcopy ((char *) pRecvPacket, (char *) (es->pOvfRecvBfr), byteCount);

    /*
     * Copy remainder of packet from the base address of the NIC
     * receive buffer to the current address in the buffer at
     * es->pOvfRecvBfr.
     */

    pOvfRecvBfrMiddle = (char *) ((u_long) (es->pOvfRecvBfr)
			+ (u_long) byteCount);
    byteCount = (int) pktSize - byteCount;

    bcopy ((char *) NIC_RECV_BFR_ADRS (0), pOvfRecvBfrMiddle, byteCount);
    }

/*******************************************************************************
*
* medSetNIC_IMR - mask/unmask interrupts in the NIC Interrupt Mask Register
*
* INTERNAL
* This NIC Interrupt Mask Register (IMR) is essentially write-only.
* The IMR in register page 2 cannot be accessed during normal
* operation.
*/

LOCAL void medSetNIC_IMR
    (
    FAST struct med_softc *es,
    FAST u_char unmaskBits,
    FAST u_char maskBits
    )
    {
    FAST int          oldLevel;
    FAST NIC_DEVICE  *pNIC       = es->nicAddr;
    FAST u_char       currentIMR = es->currentIMR;

    oldLevel = intLock ();

    /*
     * This kludge is to ensure that the PRX and RXE interrupts are not
     * enabled (in task medRecvIntrHandler) after an OVW interrupt.
     */

    if (es->bRecvBfrOvfl)
        unmaskBits &= ~(nic_PKT_RECV_NO_ERRORS | nic_PKT_RECV_ERROR);

    currentIMR     = (currentIMR | unmaskBits) & (~maskBits);
    es->currentIMR = currentIMR;
    pNIC->Imr      = currentIMR;

    intUnlock (oldLevel);
    }

/*******************************************************************************
*
* medNIC_AwaitStop - busy-wait 2ms or until the NIC stops
*/

LOCAL void medNIC_AwaitStop
    (
    FAST NIC_DEVICE *pNIC
    )
    {
    FAST int   ticksIn2ms;
    FAST ULONG tickStartCount = tickGet ();

    /*
     * Wait for NIC to complete current operation.
     * Determine the number of clock ticks in 2ms...
     * Divide the ticks/second value returned by sysClkRateGet ()
     * by 512. (Note that 512 is approximately 500. Thus, shifting
     * by 9 is as if multiplying by approximately 0.002)
     */

    ticksIn2ms = sysClkRateGet () >> 9;
    if (ticksIn2ms == 0)
        ticksIn2ms = 1;

    while ((pNIC->Isr & RST) != nic_STOP_MODE)
        if ((tickStartCount + ticksIn2ms) < tickGet ())
            break;
    }

/*******************************************************************************
*
* medXmitBfrDealloc - deallocate emptied transmit buffer(s)
*
* This routine must execute at interrupt level only.
*/
LOCAL void medXmitBfrDealloc
    (
    FAST xmitQCB_s  *pXmitQCB
    )
    {

    if (pXmitQCB->bEmpty)
        {
        logMsg("med: ERROR - transmit buffer deallocation failed.\n",
		0,0,0,0,0,0);
        return;
        }

    pXmitQCB->bFull = FALSE;
    pXmitQCB->head  = pXmitQCB->xmitBCB [pXmitQCB->head].next;
    if (pXmitQCB->head == pXmitQCB->tail)
        pXmitQCB->bEmpty = TRUE;
    }

/*******************************************************************************
*
* medXmtrLock - lock access to the NIC transmitter
*
* This function must only be invoked by tasks executing with splimp.
* Otherwise, a race condition is possible during the LOCKED test.
*
* RETURNS: ERROR if already locked, OK otherwise
*/
LOCAL STATUS medXmtrLock
    (
    FAST lockID_t *pLock
    )
    {
    FAST int oldLevel;

    if (*pLock == LOCKED)
        return ERROR;

    oldLevel = intLock ();
    *pLock = LOCKED;
    intUnlock (oldLevel);

    return (OK);
    }

/*******************************************************************************
*
* medXmtrUnlock - unlock access to the NIC transmitter
*
*/
LOCAL void medXmtrUnlock
    (
    FAST lockID_t *pLock
    )
    {
    *pLock = UNLOCKED;
    }
