/* if_ene.c - Novell/Eagle NE2000 network interface driver */

/* Copyright 1994-1998 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01l,25feb99,jmb  fix type of whirlGig arg (for SH7729)
01j,15jul97,spm  removed driver initialization from ioctl support (SPR #8831)
01i,15may97,spm  reverted to bcopy routines for mbufs in BSD 4.4
01h,07apr97,spm  code cleanup, corrected statistics, and upgraded to BSD 4.4
01g,11sep98,jmb  Manta mods:  Use pciIntConnect, only 2 slots on Manta,
                 add whirlyGig.
01f,25oct96,dat  SPR 7352, did not support IRQ 12. Now supports all IRQs.
		 fixed some compiler warnings.
01e,14jun95,hdn  removed function declarations defined in sysLib.h.
01d,01feb95,jdi  doc cleanup.
01c,12nov94,tmk  removed some IMPORTed function decs and included unixLib.h
01a,01jan94,bcs  written.
*/

/*
DESCRIPTION
This module implements the Novell/Eagle NE2000 network interface driver.
There is one user-callable routine, eneattach().


BOARD LAYOUT
The diagram below shows the relevant jumpers for VxWorks configuration.
Other compatible boards will be jumpered differently; many are jumperless.

.bS
        _________________________________________________________
        |                                                       |
        |                                                       |
        |                                           WWWWWWWW    |
        |                       WWWW  WWW           87654321    ||
        |                       1111   11         1 ........    ||
        |                       5432  901         2 ........    ||
        |                       ....  ...         3 ........    ||
        |                       ....  ...                       ||
        |               W                                       |
        |               1                                       |
        |               6                                       |___
        |               .                                       |___|
        |               .                                       |
        |________               ___                         ____|
                |               | |                         |
                |_______________| |_________________________|

    W1..W8  1-2 position selects AUI ("DIX") connector
	    2-3 position selects BNC (10BASE2) connector
    W9..W11 YYN  I/O address 300h, no boot ROM
	    NYN  I/O address 320h, no boot ROM
	    YNN  I/O address 340h, no boot ROM
	    NNN  I/O address 360h, no boot ROM
	    YYY  I/O address 300h, boot ROM at paragraph 0c800h
	    NYY  I/O address 320h, boot ROM at paragraph 0cc00h
	    YNY  I/O address 340h, boot ROM at paragraph 0d000h
	    NNY  I/O address 360h, boot ROM at ??? (invalid configuration?)
    W12     Y    IRQ 2 (or 9 if you prefer)
    W13     Y    IRQ 3
    W14     Y    IRQ 4
    W15     Y    IRQ 5 (note that only one of W12..W15 may be installed)
    W16     Y    normal ISA bus timing
	    N    timing for COMPAQ 286 portable, PS/2 Model 30-286, C&T chipset
.bE


EXTERNAL INTERFACE
There are two user-callable routines:
.iP eneattach() 14
publishes the `ene' interface and initializes the driver and device.
.iP eneShow()
displays statistics that are collected in the interrupt handler.
.LP
See the manual entries for these routines for more detail.


SYSTEM RESOURCE USAGE
    - one interrupt vector
    - 16 bytes in the uninitialized data section (bss)
    - 1752 bytes (plus overhead) of malloc'ed memory per unit attached


CAVEAT
This driver does not enable the twisted-pair connector on the Taiwanese
ETHER-16 compatible board.

*/

/***** includes *****/

#include "vxWorks.h"
#include "net/mbuf.h"
#include "net/unixLib.h"
#include "net/protosw.h"
#include "ioctl.h"
#include "socket.h"
#include "errnoLib.h"
#include "net/uio.h"
#include "net/if.h"
#include "net/route.h"
#include "netinet/in.h"
#include "netinet/in_systm.h"
#include "netinet/ip.h"
#include "netinet/ip_var.h"
#include "netinet/in_var.h"
#include "netinet/if_ether.h"
#include "etherLib.h"
#include "vme.h"
#include "iv.h"
#include "iosLib.h"
#include "ioLib.h"
#include "memLib.h"
#include "net/systm.h"
#include "net/if_subr.h"
#include "sysLib.h"
#include "vxLib.h"
#include "stdio.h"
#include "intLib.h"
#include "logLib.h"
#include "string.h"
#include "netLib.h"
#include "stdlib.h"
#include "semLib.h"
#include "drv/intrCtl/i8259a.h"
#include "drv/netif/if_ene.h"


/***** externals *****/

/* whirlyGig writes to the first or second alphanumeric LED on the
 * board whenever the netcard's interrupt handler is called
 */
void whirlyGig (unsigned int position, int counter); 

/***** defines *****/

#if CPU==SH7729
#define NENE    2                       /* number of boards supported */

#define ENE_INTCONNECT pciIntConnect
#else

#define NENE    4                       /* number of boards supported */

#undef ENE_DEBUG          /* define to include error/status messages. */
#endif

#ifndef ENE_INTCONNECT
#define ENE_INTCONNECT intConnect
#endif

/***** typedefs *****/

/* Typedefs for external structures that are not typedef'd in their .h files */

typedef struct mbuf MBUF;
typedef struct arpcom IDR;                  /* Interface Data Record wrapper */
typedef struct ifnet IFNET;                 /* real Interface Data Record */
typedef struct sockaddr SOCK;

typedef struct 
    {
    IDR         es_ac;
#define es_if           es_ac.ac_if
#define es_enaddr       es_ac.ac_enaddr
    BOOL        attached;
    USHORT      eneAddr;
    UINT        memAddr;
    UINT        memSize;
    int         intLevel;
    int         intVec;
    int         config;
    ENE_STAT    eneStat;
    volatile UCHAR current;             /* current-page register at interrupt */
    UCHAR       nextPacket;             /* where the next received packet is */
    UCHAR       rxFilter;               /* receiver configuration */
    UCHAR       imask;
    int         flags;
    UINT        txreq;
    char        packetBuf [MAX_FRAME_SIZE];/* shared at task level between */
                                        /* eneGet and enePut */
    } ENE_SOFTC;


/***** locals *****/

/*
 * Ethernet software status per interface.
 *
 * Each interface is referenced by a network interface structure,
 * es_if, which the routing code uses to locate the interface.
 * This structure contains the output queue for the interface, its address, ...
 */

LOCAL ENE_SOFTC * pEneSoftc [NENE];


/***** forward declarations *****/

static void     enereset (int unit);
static int      eneinit (int unit);
static void     eneintr (int unit);
static int      eneioctl (IFNET * ifp, int cmd, caddr_t data);
#ifdef BSD43_DRIVER
static int      eneoutput (IFNET * ifp, MBUF * m0, SOCK * dst);
#endif
static void     eneGet (int unit);
#ifdef BSD43_DRIVER
static void     enePut (int unit);
#else
static void     enePut (ENE_SOFTC * pSoftc);
#endif
static void     eneReset (int unit);
static void     eneBoardReset (int unit);
static void     eneGetAddr (int unit);
static void     eneInit (int unit);
static void     eneDataOut (int unit, char * pData, UINT16 len,
                            UINT16 eneAddress);
static void     eneDataIn (int unit, UINT16 eneAddress, UINT16 len,
                           char * pData);
static UCHAR    eneGetCurr (int unit);
static void     eneIntEnable (int unit);
static void     eneOverwriteRecover (int unit, UCHAR cmdStatus);
       void     eneShow (int unit, BOOL zap);
       void     enedetach (int unit);


/*******************************************************************************
*
* eneattach - publish the `ene' network interface and initialize the driver and device
*
* This routine attaches an `ene' Ethernet interface to the network if the
* device exists.  It makes the interface available by filling in the network
* interface record.  The system will initialize the interface when it is
* ready to accept packets.
*
* RETURNS: OK or ERROR.
*
* SEE ALSO: ifLib, netShow
*/

STATUS eneattach 
    (
    int unit,           /* unit number */
    int ioAddr,         /* address of ene's shared memory */
    int ivec,           /* interrupt vector to connect to */
    int ilevel          /* interrupt level */
    )
    {
    FAST ENE_SOFTC * pSoftc;

    if ((UINT)unit >= NENE)
        {
        errnoSet (S_iosLib_CONTROLLER_NOT_PRESENT);
        return (ERROR);
        }

    if (pEneSoftc [unit] == NULL)
        {
        pEneSoftc [unit] = malloc (sizeof (ENE_SOFTC));
        if (pEneSoftc [unit] == NULL)
            return (ERROR);
        bzero ( (char *) pEneSoftc [unit], sizeof (ENE_SOFTC));
        }
    pSoftc = pEneSoftc [unit];
    if (pSoftc->attached)
        return (ERROR);

    pSoftc->eneAddr     = ioAddr;
    if (ilevel < 0 || ilevel > 14)	/* Valid int Level ? */
        ilevel = 3;                     /* default is IRQ3 */
    pSoftc->intLevel    = ilevel;
    pSoftc->intVec      = ivec;

    pSoftc->rxFilter = RCON_BROAD;

    eneBoardReset (unit);               /* power-on initialization */

    eneInit (unit);                     /* initialize ENE */

    (void) ENE_INTCONNECT ((VOIDFUNCPTR *) INUM_TO_IVEC (ivec), 
        (VOIDFUNCPTR) eneintr, unit);

    /* enable interrupt */

    eneIntEnable (unit);

    sysIntEnablePIC (ilevel);

#ifdef BSD43_DRIVER
    ether_attach (&pSoftc->es_if, unit, "ene",
                  (FUNCPTR)eneinit, (FUNCPTR)eneioctl,
                  (FUNCPTR)eneoutput, (FUNCPTR)enereset);
#else
    ether_attach (
                 &pSoftc->es_if, 
                 unit, 
                 "ene",
                 (FUNCPTR)eneinit, 
                 (FUNCPTR)eneioctl,
                 (FUNCPTR)ether_output, 
                 (FUNCPTR)enereset
                 );
     pSoftc->es_if.if_start = (FUNCPTR)enePut;
#endif

    eneinit (unit);

    pSoftc->attached = TRUE;

    return (OK);
    }

/*******************************************************************************
*
* enereset - reset the interface
*/

static void enereset 
    (
    int unit 
    )
    {
    eneReset (unit);
    }

/*******************************************************************************
*
* eneinit - initialize ene
*
* Restart the link level software on the board and mark the interface up.
*
* RETURNS: 0
*/

static int eneinit 
    (
    int unit 
    )
    {
    FAST ENE_SOFTC * pSoftc = pEneSoftc [unit];

    pSoftc->es_if.if_flags |= IFF_UP|IFF_RUNNING; /* open for business*/

    return (0);
    }

/*******************************************************************************
*
* eneintr - Ethernet interface interrupt
*
* Hand off reading of packets from the interface to task level.
* Tabulate any transmit errors and cue task level to start next output.
*/

static void eneintr 
    (
    int unit 
    )
    {
    FAST ENE_SOFTC * pSoftc = pEneSoftc [unit];
    UCHAR intStat;
    UCHAR txStat;
    UCHAR rxStat;
    UCHAR cmdStat;

    pSoftc->eneStat.interrupts++;
#if CPU==SH7729
    whirlyGig ((unsigned int) unit, pSoftc->eneStat.interrupts);
#endif
    sysBusIntAck (pSoftc->intLevel);                    /* acknowledge int */

    /* acknowledge interrupt, get Transmit/Receive status */

    intStat = sysInByte (ENE_INTSTAT (pSoftc->eneAddr));
    sysOutByte (ENE_INTSTAT (pSoftc->eneAddr), intStat);
    txStat = sysInByte (ENE_TSTAT (pSoftc->eneAddr));
    rxStat = sysInByte (ENE_RSTAT (pSoftc->eneAddr));

    /* get xxxCount Reg that is cleared after read access */

    pSoftc->eneStat.collisions += sysInByte (ENE_COLCNT (pSoftc->eneAddr));
    pSoftc->eneStat.aligns += sysInByte (ENE_ALICNT (pSoftc->eneAddr));
    pSoftc->eneStat.crcs += sysInByte (ENE_CRCCNT (pSoftc->eneAddr));
    pSoftc->eneStat.missed += sysInByte (ENE_MPCNT (pSoftc->eneAddr));


    if (intStat & ISTAT_OVW) /* Overwrite */
        {
        pSoftc->es_if.if_ierrors++;
        pSoftc->eneStat.overwrite++;
        cmdStat = sysInByte (ENE_CMD (pSoftc->eneAddr));
        sysOutByte (ENE_INTMASK (pSoftc->eneAddr), 0);/* disable board ints.*/
        sysOutByte (ENE_CMD (pSoftc->eneAddr),
                    CMD_NODMA | CMD_PAGE0 | CMD_STOP);
        (void) netJobAdd ( (FUNCPTR)eneOverwriteRecover, unit, cmdStat, 0, 0,0);
        }
    if (intStat & ISTAT_RXE) /* Receive-error */
        {
        pSoftc->es_if.if_ierrors++;
        pSoftc->eneStat.rerror++;
        if (rxStat & RSTAT_OVER)
            pSoftc->eneStat.overruns++;
        if (rxStat & RSTAT_DIS)
            pSoftc->eneStat.disabled++;
        if (rxStat & RSTAT_DFR)
            pSoftc->eneStat.deferring++;
        }
    if (intStat & ISTAT_PRX) /* Receive */
        {
        pSoftc->current = eneGetCurr (unit);
        if ((pSoftc->flags & RXING) == 0)
            {
            pSoftc->flags |= RXING;
            (void) netJobAdd ((FUNCPTR)eneGet, unit, 0,0,0,0);
            }
        }

    if ( (intStat & ISTAT_TXE) != 0)    /* Transmit error-packet not sent */
        {
        pSoftc->es_if.if_oerrors++;
        pSoftc->es_if.if_opackets--;
        pSoftc->eneStat.terror++;
        if (txStat & TSTAT_ABORT)
            {
            pSoftc->eneStat.aborts++;
            pSoftc->eneStat.collisions += 16;
            }
        if (txStat & TSTAT_UNDER)
            pSoftc->eneStat.underruns++;
        }
    if ( (intStat & ISTAT_PTX) != 0)    /* Transmit-packet sent */
        {
        if (txStat & TSTAT_CDH)
            pSoftc->eneStat.heartbeats++;
        if (txStat & TSTAT_OWC)
            pSoftc->eneStat.outofwindow++;
        if (txStat & TSTAT_PTX)
            pSoftc->eneStat.tnoerror++;
        }
    if ( (intStat & (ISTAT_TXE | ISTAT_PTX)) != 0) /* Transmit complete */
        if (pSoftc->es_if.if_snd.ifq_head != NULL)
#ifdef BSD43_DRIVER
            (void) netJobAdd ((FUNCPTR)enePut, unit, 0,0,0,0);
#else
            (void) netJobAdd ((FUNCPTR)enePut, (int)pSoftc, 0,0,0,0);
#endif
    }

/*******************************************************************************
*
* eneGet - read a packet off the interface
*
* Copy packets from the buffer ring into mbufs and hand them to the next
* higher layer.
*/

static void eneGet 
    (
    int unit
    )
    {
    FAST ENE_SOFTC * pSoftc = pEneSoftc [unit];
    FAST struct ether_header * eh;
    FAST MBUF * m;
#ifdef BSD43_DRIVER
    int         off;
#endif
    UINT        packetSize;
    UCHAR *     pData;
    static UCHAR uppByteCnt;
    UINT8       tempPage;               /* used in buffer validation */
    UINT8       pageCount;              /* used in buffer validation */
    ENE_HEADER  h;

unlucky:

    while (TRUE)
        {
        if (pSoftc->nextPacket == pSoftc->current)
            break;                      /* done all complete frames */

        eneDataIn (unit, (((UINT)pSoftc->nextPacket << 8) & 0x0000ffff),
                   sizeof (ENE_HEADER), (char *) &h);
#ifdef ENE_DEBUG
printf ("ene: Get: next=%02x, uppByteCnt=%02x, lowByteCnt=%02x\n",
        h.next, h.uppByteCnt, h.lowByteCnt);
#endif

        /* Calculate upper byte count in case it's corrupted,
         * though this supposedly happens only in StarLAN applications
         * with bus clock frequence greater than 4 mHz.
         */

        if (h.next > pSoftc->nextPacket)
            uppByteCnt = h.next - pSoftc->nextPacket;
        else
            uppByteCnt = (NE2000_PSTOP - pSoftc->nextPacket) +
                         (h.next - NE2000_PSTART);
        if (h.lowByteCnt > 0xfc)
            uppByteCnt -= 2;
        else
            uppByteCnt -= 1;
        h.uppByteCnt = uppByteCnt;

        /* compute packet size excluding Ethernet checksum bytes */

        packetSize = ( ( (UINT)h.uppByteCnt << 8) + h.lowByteCnt) - 4;

        /* Check for packet (and header) shifted in memory (by heavy load).
         * The method and solution are recommended by 3Com in their
         * EtherLink II Adapter Technical Manual, with the addition of
         * a reasonableness check on the next-page link.
         */

        pageCount = (packetSize + 4 + sizeof (ENE_HEADER) + (ENE_PAGESIZE - 1))
                    / ENE_PAGESIZE;
        tempPage = pSoftc->nextPacket + pageCount;
        if (tempPage >= NE2000_PSTOP)
            tempPage -= (NE2000_PSTOP - NE2000_PSTART);
        if ((h.next != tempPage) ||
            (h.next < NE2000_PSTART) || (h.next >= NE2000_PSTOP))
            {
            pSoftc->eneStat.badPacket++; /* can't trust anything now */
            pSoftc->eneStat.rerror++;
            eneInit (unit);             /* stop and restart the interface */
            pSoftc->flags &= ~RXING;
            eneIntEnable (unit);
            return;
            }

        /* reject runts, although we are configured to reject them */

        if (packetSize < 60)
            {
            pSoftc->eneStat.shortPacket++;
            goto doneGet;
            }

        /* reject packets larger than our scratch buffer */

        if (packetSize > MAX_FRAME_SIZE)
            goto doneGet;

        if (h.rstat & RSTAT_PRX)
            {

            /* 3Com says this status marks a packet bit-shifted in memory;
             * the data cannot be trusted but the NIC header is OK.
             */

            if ( (h.rstat & (RSTAT_DFR | RSTAT_DIS)) != 0)
                {
                pSoftc->eneStat.badPacket++;
                pSoftc->eneStat.rerror++;
                goto doneGet;
                }
            pSoftc->eneStat.rnoerror++;
            }
        else
            {
            if ( (h.rstat & RSTAT_DFR) != 0)
                pSoftc->eneStat.jabber++;
            pSoftc->eneStat.rerror++;
            goto doneGet;
            }

        /* copy separated frame to a temporary buffer */

        eneDataIn (unit,
                   ((UINT)pSoftc->nextPacket << 8) + sizeof (ENE_HEADER),
                   packetSize,
                   pSoftc->packetBuf);

        eh = (struct ether_header *)(pSoftc->packetBuf);

        pSoftc->es_if.if_ipackets++;        /* bump statistic. */

        /* call input hook if any */

        if ((etherInputHookRtn != NULL) &&
            (* etherInputHookRtn) (&pSoftc->es_if, (char *) eh, packetSize))
            {
            goto doneGet;
            }

        if (packetSize >= SIZEOF_ETHERHEADER)
            packetSize -= SIZEOF_ETHERHEADER;
        else
            goto doneGet;

        pData = ((unsigned char *) eh) + SIZEOF_ETHERHEADER;

#ifdef BSD43_DRIVER
        check_trailer (eh, pData, (int *) &packetSize, &off, &pSoftc->es_if);

        /* copy data from the scratch buffer to mbuf a long-word at a time */

        m = bcopy_to_mbufs (pData, packetSize, off,
                            (IFNET *) &pSoftc->es_if, 2);
        if (m == NULL)
            {
            pSoftc->es_if.if_ierrors++;         /* bump error statistic. */
            goto doneGet;
            }

        do_protocol_with_type (eh->ether_type, m, &pSoftc->es_ac, packetSize);
#else
        m = bcopy_to_mbufs (pData, packetSize, 0, (IFNET *)&pSoftc->es_if, 2);

        if (m == NULL)
            {
            pSoftc->es_if.if_ierrors++;         /* bump error statistic. */
            goto doneGet;
            }

        do_protocol (eh, m, &pSoftc->es_ac, packetSize);
#endif

doneGet:
        pSoftc->nextPacket = h.next;
        sysOutByte (ENE_BOUND (pSoftc->eneAddr),
                    (pSoftc->nextPacket != NE2000_PSTART ?
                     pSoftc->nextPacket - 1 : NE2000_PSTOP - 1));

        }

    pSoftc->flags &= ~RXING;

    /* go back if we received an interrupt and a new packet */

    if (pSoftc->nextPacket != pSoftc->current)
        goto unlucky;
    }

#ifdef BSD43_DRIVER
/*******************************************************************************
*
* eneoutput - ethernet output routine
*
* Encapsulate a packet of type family for the local net.
* Use trailer local net encapsulation if enough data in first
* packet leaves a multiple of 512 bytes of data in remainder.
* If destination is this address or broadcast, send packet to
* loop device to kludge around the fact that 3com interfaces can't
* talk to themselves.
*
* RETURNS: ?
*/

static int eneoutput 
    (
    IFNET * ifp,
    MBUF * m0,
    SOCK * dst 
    )
    {
    return (ether_output (ifp, m0, dst, (FUNCPTR)enePut,
                          & pEneSoftc [ifp->if_unit]->es_ac));
    }
#endif

/*******************************************************************************
*
* enePut - copy a packet to the interface.
*
* Copy from mbuf chain to transmitter buffer in shared memory.
*
*/

#ifdef BSD43_DRIVER
static void enePut 
    (
    int unit
    )
    {
    FAST ENE_SOFTC * pSoftc   = pEneSoftc [unit];
#else
static void enePut 
    (
    ENE_SOFTC * pSoftc
    )
    {
    int unit = pSoftc->es_if.if_unit;
#endif
    FAST MBUF * m;
    FAST int len;
    long qdtimer;
    
    int s = splnet ();

    if (pSoftc->es_if.if_snd.ifq_head != NULL)
        {
        qdtimer = 10000L;
        while ( ( (sysInByte (ENE_CMD (pSoftc->eneAddr)) & CMD_TXP) != 0)
               && (--qdtimer > 0))
            ;
        if (qdtimer == 0)
            eneInit (unit);

        IF_DEQUEUE (&pSoftc->es_if.if_snd, m);


        /* we assume no packet will be larger than MAX_FRAME_SIZE,
         * based on our advertised MTU.
         */

        copy_from_mbufs (pSoftc->packetBuf, m, len);
        len = max (ETHERSMALL, len);

        /* call output hook if there is one, else send the packet */

        if ( (etherOutputHookRtn != NULL) &&
             (* etherOutputHookRtn) (&pSoftc->es_if, pSoftc->packetBuf, len))
            {
            }
        else
            {
            /* put packet in board's output buffer */

            eneDataOut (unit, pSoftc->packetBuf, len, (NE2000_TSTART << 8));

            /* kick Transmitter */

            sysOutByte (ENE_INTMASK (pSoftc->eneAddr), 0x00);
            sysOutByte (ENE_TSTART (pSoftc->eneAddr), NE2000_TSTART);
            sysOutByte (ENE_TCNTH (pSoftc->eneAddr), len >> 8);
            sysOutByte (ENE_TCNTL (pSoftc->eneAddr), len & 0xff);
            sysOutByte (ENE_CMD (pSoftc->eneAddr), CMD_TXP);
            eneIntEnable (unit);
            }
        }

    splx (s);
    }

/*******************************************************************************
*
* eneBoardReset - reset the whole board
*
* The NE2000 has an I/O port dedicated to hardware reset; you READ the port
* to initiate the reset then WRITE 0 to the port.
*/

static void eneBoardReset
    (
    int unit
    )
    {
    FAST ENE_SOFTC * pSoftc   = pEneSoftc [unit];

    (void) sysInByte (ENE_RESET (pSoftc->eneAddr));
    taskDelay (sysClkRateGet () >> 1);
    sysOutByte (ENE_RESET (pSoftc->eneAddr), 0x00);
    taskDelay (sysClkRateGet () >> 1);

    sysOutByte (ENE_CMD (pSoftc->eneAddr), CMD_NODMA | CMD_PAGE0 | CMD_STOP);
    while ((sysInByte (ENE_INTSTAT (pSoftc->eneAddr)) & ISTAT_RST) != ISTAT_RST)
        ;
    }

/*******************************************************************************
*
* eneReset - reset the chip
*
* Reset the chip.
*
*/

static void eneReset 
    (
    int unit
    )
    {
    FAST ENE_SOFTC * pSoftc   = pEneSoftc [unit];
    int s = splnet ();

    /* mask interrupt */
    sysOutByte (ENE_INTMASK (pSoftc->eneAddr), 0x00);

    eneInit (unit);

    eneIntEnable (unit);

    pSoftc->eneStat.reset++;
    splx (s);
    }

/*******************************************************************************
*
* eneioctl - ioctl for interface
*
* RETURNS: ?
*/

static int eneioctl 
    (
    FAST IFNET * ifp,
    int cmd,
    caddr_t data 
    )
    {
    int unit;
    ENE_SOFTC * pSoftc;
    int error = 0;
    int s;

    unit = ifp->if_unit;
    pSoftc = pEneSoftc [unit];

    s = splimp();

    switch (cmd)
        {
        case SIOCSIFADDR:
            ((struct arpcom *)ifp)->ac_ipaddr = IA_SIN (data)->sin_addr;
            arpwhohas (ifp, &IA_SIN (data)->sin_addr);
            break;

        case SIOCSIFFLAGS:
            /* Set promiscuous mode according to current interface flags */

            pSoftc->rxFilter = ( (ifp->if_flags & IFF_PROMISC) != 0) ?
                                   RCON_BROAD | RCON_PROM : RCON_BROAD;
            sysOutByte (ENE_RCON (pSoftc->eneAddr), pSoftc->rxFilter);

            break;

        default:
            error = EINVAL;
        }

    splx (s);
    return (error);
    }

/*******************************************************************************
*
* eneInit - initialize SMC8390 ENE (Ethernet LAN Controller)
*
* RETURNS: N/A
*
* NOMANUAL
*/

static void eneInit 
    (
    int unit
    )
    {
    FAST ENE_SOFTC * pSoftc          = pEneSoftc [unit];
    UCHAR * pAddr               = (UCHAR *)pSoftc->es_enaddr;

    /* 1. program Command Register for page 0 and for no DMA */

    sysOutByte (ENE_CMD (pSoftc->eneAddr), CMD_NODMA | CMD_PAGE0 | CMD_STOP);
#ifdef OLDSTUFF
    while ((sysInByte (ENE_INTSTAT (pSoftc->eneAddr)) & ISTAT_RST) != ISTAT_RST)
        ;
#endif /* OLDSTUFF */

    /* 2. initialize Data Configuration Register:
     *    16-bit bus, burst mode, 8-deep FIFO.
     */

    sysOutByte (ENE_DCON (pSoftc->eneAddr), DCON_BSIZE1 | DCON_BUS16 | 0x08);

    eneGetAddr (unit);                  /* get ethernet address */

    /* 3. clear Remote Byte Count Register */

    sysOutByte (ENE_RBCR0 (pSoftc->eneAddr), 0x00);
    sysOutByte (ENE_RBCR1 (pSoftc->eneAddr), 0x00);

    /* 4. initialize Receive Configuration Register */

    sysOutByte (ENE_RCON (pSoftc->eneAddr), pSoftc->rxFilter);

    /* 5. place the ENE in LOOPBACK mode 1 or 2 */

    sysOutByte (ENE_TCON (pSoftc->eneAddr), TCON_LB1);

    /* 6. initialize Receive Buffer Ring */

    sysOutByte (ENE_RSTART (pSoftc->eneAddr), NE2000_PSTART);
    sysOutByte (ENE_RSTOP (pSoftc->eneAddr), (char)NE2000_PSTOP);
    sysOutByte (ENE_BOUND (pSoftc->eneAddr), (char)NE2000_PSTART);

    /* 7. clear Interrupt Status Register */

    sysOutByte (ENE_INTSTAT (pSoftc->eneAddr), (char)0xff);

    /* 8. initialize Interrupt Mask Register */

    sysOutByte (ENE_INTMASK (pSoftc->eneAddr), 0x00);

    /* 9. program Command Register for page 1 */

    sysOutByte (ENE_CMD (pSoftc->eneAddr), CMD_PAGE1 | CMD_STOP);

    /* i) initialize physical address registers */

    sysOutByte (ENE_STA0 (pSoftc->eneAddr), *pAddr++);
    sysOutByte (ENE_STA1 (pSoftc->eneAddr), *pAddr++);
    sysOutByte (ENE_STA2 (pSoftc->eneAddr), *pAddr++);
    sysOutByte (ENE_STA3 (pSoftc->eneAddr), *pAddr++);
    sysOutByte (ENE_STA4 (pSoftc->eneAddr), *pAddr++);
    sysOutByte (ENE_STA5 (pSoftc->eneAddr), *pAddr);

    /* ii) initialize multicast address registers */

    sysOutByte (ENE_MAR0 (pSoftc->eneAddr), 0);
    sysOutByte (ENE_MAR1 (pSoftc->eneAddr), 0);
    sysOutByte (ENE_MAR2 (pSoftc->eneAddr), 0);
    sysOutByte (ENE_MAR3 (pSoftc->eneAddr), 0);
    sysOutByte (ENE_MAR4 (pSoftc->eneAddr), 0);
    sysOutByte (ENE_MAR5 (pSoftc->eneAddr), 0);
    sysOutByte (ENE_MAR6 (pSoftc->eneAddr), 0);
    sysOutByte (ENE_MAR7 (pSoftc->eneAddr), 0);

    /* iii) initialize current page pointer */

    sysOutByte (ENE_CURR (pSoftc->eneAddr), NE2000_PSTART + 1);
    pSoftc->nextPacket = NE2000_PSTART + 1;

    /* set memory parameters */

    sysOutByte (ENE_CMD (pSoftc->eneAddr), 
			 (char)(CMD_NODMA | CMD_PAGE2 | CMD_STOP));
    sysOutByte (ENE_ENH (pSoftc->eneAddr), 0x00);       /* 0 wait states */
    sysOutByte (ENE_BLOCK (pSoftc->eneAddr), 0x00);     /* 0x00xxxx */

    /* 10. put the ENE in START mode */

    sysOutByte (ENE_CMD (pSoftc->eneAddr), CMD_NODMA | CMD_PAGE0 | CMD_START);

    /* 11. initialize Transmit Configuration Register */

    sysOutByte (ENE_TCON (pSoftc->eneAddr), 0x00);

    }

/*******************************************************************************
*
* eneGetAddr - get hardwired ethernet address.
*
* Read the ethernet address from the board's byte-wide configuration memory,
* one byte at a time.  If we could read it in word-wide cycles, we could use
* eneDataIn(), below.  The on-board logic requires double the byte count!
*
*/

static void eneGetAddr 
    (
    int unit 
    )
    {
    FAST ENE_SOFTC * pSoftc          = pEneSoftc [unit];
    UCHAR * pAddr               = (UCHAR *)pSoftc->es_enaddr;

    sysOutByte (ENE_CMD (pSoftc->eneAddr), CMD_NODMA | CMD_PAGE0 | CMD_START);
    sysOutByte (ENE_RSAR0 (pSoftc->eneAddr), NE2000_EADDR_LOC);
    sysOutByte (ENE_RSAR1 (pSoftc->eneAddr), NE2000_CONFIG_PAGE);
    sysOutByte (ENE_RBCR0 (pSoftc->eneAddr), (EA_SIZE * 2) & 0xff);
    sysOutByte (ENE_RBCR1 (pSoftc->eneAddr), (EA_SIZE * 2) >> 8);
    sysOutByte (ENE_CMD (pSoftc->eneAddr), CMD_RREAD | CMD_START);

    *pAddr++ = sysInByte (ENE_DATA (pSoftc->eneAddr));
    *pAddr++ = sysInByte (ENE_DATA (pSoftc->eneAddr));
    *pAddr++ = sysInByte (ENE_DATA (pSoftc->eneAddr));
    *pAddr++ = sysInByte (ENE_DATA (pSoftc->eneAddr));
    *pAddr++ = sysInByte (ENE_DATA (pSoftc->eneAddr));
    *pAddr   = sysInByte (ENE_DATA (pSoftc->eneAddr));
    }

/*******************************************************************************
*
* eneDataOut - output bytes to NE2000 memory
*
* NOMANUAL
*/

static void eneDataOut
    (
    int         unit,
    char *      pData,
    UINT16      len,
    UINT16      eneAddress
    )
    {
    FAST ENE_SOFTC * pSoftc          = pEneSoftc [unit];

    if (len == 0)
        return;                         /* nothing to do */

#ifndef BSD43_DRIVER
    /* Bump the statistic counter. */

    pSoftc->es_if.if_opackets++;
#endif

    sysOutByte (ENE_INTMASK (pSoftc->eneAddr), 0);      /* disable board ints.*/
    sysOutByte (ENE_RBCR0 (pSoftc->eneAddr), (char)0xff);
    sysOutByte (ENE_CMD (pSoftc->eneAddr), CMD_NODMA | CMD_PAGE0 | CMD_START);
    sysOutByte (ENE_RSAR0 (pSoftc->eneAddr), eneAddress & 0xff);
    sysOutByte (ENE_RSAR1 (pSoftc->eneAddr), eneAddress >> 8);
    sysOutByte (ENE_RBCR0 (pSoftc->eneAddr), len & 0xff);
    sysOutByte (ENE_RBCR1 (pSoftc->eneAddr), len >> 8);
    sysOutByte (ENE_CMD (pSoftc->eneAddr), CMD_RWRITE | CMD_START);

    sysOutWordString (ENE_DATA (pSoftc->eneAddr), (short *)pData, (len / 2));
    if (len & 1)
        sysOutByte (ENE_DATA (pSoftc->eneAddr), *(pData + (len - 1)));
    eneIntEnable (unit);
    }

/*******************************************************************************
*
* eneDataIn - input bytes from NE2000 memory
*
* NOMANUAL
*/

static void eneDataIn
    (
    int         unit,
    UINT16      eneAddress,
    UINT16      len,
    char *      pData
    )
    {
    FAST ENE_SOFTC * pSoftc          = pEneSoftc [unit];

    sysOutByte (ENE_INTMASK (pSoftc->eneAddr), 0);
    sysOutByte (ENE_CMD (pSoftc->eneAddr), CMD_NODMA | CMD_PAGE0 | CMD_START);
    sysOutByte (ENE_RSAR0 (pSoftc->eneAddr), eneAddress & 0xff);
    sysOutByte (ENE_RSAR1 (pSoftc->eneAddr), eneAddress >> 8);
    sysOutByte (ENE_RBCR0 (pSoftc->eneAddr), len & 0xff);
    sysOutByte (ENE_RBCR1 (pSoftc->eneAddr), len >> 8);
    sysOutByte (ENE_CMD (pSoftc->eneAddr), CMD_RREAD | CMD_START);

    sysInWordString (ENE_DATA (pSoftc->eneAddr), (short *)pData, (len / 2));
    if (len & 1)
         *(pData + (len - 1)) = sysInByte (ENE_DATA (pSoftc->eneAddr));
    eneIntEnable (unit);
    }

/*******************************************************************************
*
* eneGetCurr - get current page
*
* RETURNS: current page that ENE is working on
*
* NOMANUAL
*/

static UCHAR eneGetCurr 
    (
    int unit
    )
    {
    FAST ENE_SOFTC * pSoftc          = pEneSoftc [unit];
    UCHAR curr;

    /* get CURR register */

    sysOutByte (ENE_CMD (pSoftc->eneAddr), CMD_PAGE1);
    curr = sysInByte (ENE_CURR (pSoftc->eneAddr));
    sysOutByte (ENE_CMD (pSoftc->eneAddr), CMD_PAGE0);

    return (curr);
    }

/*******************************************************************************
*
* eneIntEnable - enable NE2000 interrupt conditions
*/

static void eneIntEnable
    (
    int unit
    )
    {
    FAST ENE_SOFTC * pSoftc          = pEneSoftc [unit];

    sysOutByte (ENE_INTMASK (pSoftc->eneAddr), 
                IM_OVWE | IM_TXEE | IM_RXEE | IM_PTXE | IM_PRXE);
    }

/*******************************************************************************
*
* eneOverwriteRecover - recover from receive buffer ring overflow
*
* This procedure is mandated by National Semiconductor as the only safe
* way to recover from an Overwrite Warning Error.  The first two steps
* of their procedure, reading the Command Register and stopping the NIC,
* were accomplished in the interrupt handler.  The rest occurs here.
*
* This routine is scheduled to run in the net task since it must delay
* to allow the STOP command to take effect.
*/

static void eneOverwriteRecover
    (
    int unit,
    UCHAR cmdStatus
    )
    {
    FAST ENE_SOFTC * pSoftc          = pEneSoftc [unit];
    BOOL        reSend;

    /* 1. read the TXP bit in the command register (already in cmdStatus) */

    /* 2. issue the STOP command (done in the interrupt handler) */

    /* 3. delay at least 1.6 milliseconds (1/625 second) */

    taskDelay ((sysClkRateGet () / 625) + 1);

    /* we are now supposedly sure the NIC is stopped */

    /* 4. clear Remote Byte Count Register */

    sysOutByte (ENE_RBCR0 (pSoftc->eneAddr), 0x00);
    sysOutByte (ENE_RBCR1 (pSoftc->eneAddr), 0x00);

    /* 5. read the stored value of the TXP bit (in cmdStatus) */

    if ( (cmdStatus & CMD_TXP) == 0)
        reSend = FALSE;
    else
        {
        if ((sysInByte (ENE_INTSTAT (pSoftc->eneAddr)) &
              (ISTAT_PTX | ISTAT_TXE)) == 0)

            /* transmit was in progress but probably deferring */

            reSend = TRUE;
        else

            /* transmit was completed (probably), don't repeat it */

            reSend = FALSE;
        }

    /* 6. place the NIC in loopback mode */

    sysOutByte (ENE_TCON (pSoftc->eneAddr), TCON_LB1);

    /* 7. issue the START command to the NIC */

    sysOutByte (ENE_CMD (pSoftc->eneAddr), CMD_NODMA | CMD_PAGE0 | CMD_START);

    /* 8. remove one or more packets from the receive buffer ring */

    pSoftc->current = eneGetCurr (unit);
    eneGet (unit);                      /* get all the packets there are */

    /* 9. reset the overwrite warning bit in the ISR */

    /* reset all active interrupt conditions */

    sysOutByte (ENE_INTSTAT (pSoftc->eneAddr),
                sysInByte (ENE_INTSTAT (pSoftc->eneAddr)));

    /* 10. take the NIC out of loopback */

    sysOutByte (ENE_TCON (pSoftc->eneAddr), 0x00);

    /* 11. if need to resend, do so, otherwise if anything queued, send that */

    if (reSend)
        sysOutByte (ENE_CMD (pSoftc->eneAddr), CMD_TXP);
    else if (pSoftc->es_if.if_snd.ifq_head != NULL)
#ifdef BSD43_DRIVER
        enePut (unit);
#else
        enePut (pSoftc);
#endif
    }

/*******************************************************************************
*
* eneShow - display statistics for the NE2000 `ene' network interface
*
* This routine displays statistics about the `ene' Ethernet network interface.
* It has two parameters:
* .iP <unit>
* interface unit; should be 0.
* .iP <zap>
* if 1, all collected statistics are cleared to zero.
* .LP
*
* RETURNS: N/A
*/

void eneShow 
    (
    int unit,   /* interface unit */
    BOOL zap    /* 1 = zero totals */
    )
    {
    FAST ENE_SOFTC * pSoftc;
    FAST int ix;
    static char * e_message [] = {
        "collisions",
        "crcs",
        "aligns",
        "missed",
        "over-runs",
        "disabled",
        "deferring",
        "under-run",
        "aborts",
        "out-of-window coll",
        "heart-beats",
        "bad-packet",
        "short-packet",
        "t-no-error",
        "r-no-error",
        "t-error",
        "r-error",
        "over-write",
        "wrapped",
        "interrupts",
        "reset",
        "stray-int",
        "jabbers"};

    if ((UINT)unit >= NENE)
        {
        printf ("eneShow: invalid unit number %d; valid range 0..%d\n",
                unit, NENE);
        return;
        }

    pSoftc = pEneSoftc [unit];
    if ((pSoftc == NULL) || ! pSoftc->attached)
        {
        printf ("eneShow: unit %d not attached\n", unit);
        return;
        }

    for (ix = 0; ix < NELEMENTS(e_message); ix++)
        {
        printf ("    %-30.30s  %4d\n", e_message [ix], pSoftc->eneStat.stat [ix]);
        if (zap)
            pSoftc->eneStat.stat [ix] = 0;
        }
    printf ("    %-30.30s  0x%02x\n", "flags", pSoftc->flags);
    }

/*******************************************************************************
*
* enedetach - detach the card from the bus.
*
* Dettach the card from the bus for reset.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void enedetach 
    (
    int unit
    )
    {
    FAST ENE_SOFTC * pSoftc     = pEneSoftc [unit];

    if (pSoftc->attached)
        {
        sysOutByte (ENE_INTMASK (pSoftc->eneAddr), 0x00);
        sysOutByte (ENE_CMD (pSoftc->eneAddr), CMD_NODMA | CMD_STOP);
        while ((sysInByte (ENE_INTSTAT (pSoftc->eneAddr)) & ISTAT_RST) != ISTAT_RST)
            ;
        sysOutByte (ENE_RCON (pSoftc->eneAddr), 0x00);
        sysOutByte (ENE_TCON (pSoftc->eneAddr), TCON_LB1);
        sysIntDisablePIC (pSoftc->intLevel);
        eneBoardReset (unit);
        }
    pSoftc->attached = FALSE;           /* allow a new attach */
    }

