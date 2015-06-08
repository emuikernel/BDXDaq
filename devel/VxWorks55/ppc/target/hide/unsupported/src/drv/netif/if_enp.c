/* if_enp.c - CMC ENP 10/L Ethernet network interface driver */

/* Copyright 1984-1997 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
03d,15jul97,spm  added ARP request to SIOCSIFADDR ioctl handler;
                 added support for promiscuous mode to SIOCSIFFLAGS
03c,25apr97,gnn  added multicast by turning on promiscuous mode.
03b,07apr97,spm  code cleanup, corrected statistics, and upgraded to BSD 4.4
03a,16dec96,map  doc: fixed ENP-10/L jumper diagram [SPR# 3025]
02z,14oct93,wmd  Added htons() for pPacketType assignments (SPR #2060).
02y,11aug93,jmm  Changed ioctl.h and socket.h to sys/ioctl.h and sys/socket.h
02x,19feb93,jdi  documentation cleanup.
02w,02feb93,dzb  updated copyright.  removed HK68/V2F referrence (SPR #1876). 
02v,13oct92,rfs  Added documentation.
02u,16sep92,rfs  Reinstated interrupting of the ENP after putting outgoing
                 packets on the transmit ring, even though the documentation
                 is conflicting on this action, and some boards work fine
                 without it.
02t,09sep92,gae  documentation tweaks.
02s,04sep92,ccc  added address modifier parameter to attach() routine.
02r,06aug92,rfs  Functional changes:
                   Eliminated init() routine.  Added errno setting for all
                   error cases in the attach() routine.  Added rejection of
                   multiple attach() calls per unit.  Eliminated obsolete use
                   of transmit output queue.  Moved driver control structures
                   to BSS.  Corrected the ioctl() routine.  Added Tx semaphore.
                   Added write pipe flushing.  Added rejection of work if the
                   interface flags are down.  Revised Rx event handling.
                 Cosmetic changes:
                   Changed name of driver control structure, changed all
                   references to it, changed name of all pointers to it.  Added
                   many comments.  Typedef'd common structures and changed all
                   usages to the new type.  General cleanup and convention
                   adherence.
02q,27jul92,ccc  used sysBusToLocalAdrs() to compute bus address,
                 fixed warnings.
02p,18jul92,smb  Changed errno.h to errnoLib.h.
02o,26may92,rrr  the tree shuffle
                  -changed includes to have absolute path from h/
02n,05may92,wmd  changed LITTLE_ENDIAN to _LITTLE_ENDIAN, BYTE_ORDER to
                 _BYTE_ORDER, cleaned up ansi warnings.
02m,15jan92,wmd  Added conditional to use ntohl for little Endian in enpput()
                 and initialized pEnpDev->enp_mode with 0x8000 (960 only).
02l,17oct91,yao  added support for CPU32.
02k,04oct91,rrr  passed through the ansification filter
                  -changed functions to ansi style
                  -changed includes to have absolute path from h/
                  -fixed #else and #endif
                  -changed READ, WRITE UPDATE to O_RDONLY O_WRONLY O_RDWR
                  -changed VOID to void
                  -changed copyright notice
02j,22aug91,hdn  fixed bug in enpgetaddr.
02i,19jun91,km   some modifications for 960 -- not verified correct.
                 +gae  compiled in enpShow; added enpDump for debug.
                 fixed some usages of ENPDEVICE_OLD (for old CMC LINK-10 ROMs).
02h,29apr91,hdn  use SIZEOF_ETHERHEADER instead of sizeof(ether_header).
02g,10apr91,jdi  documentation cleanup; doc review by elh.
02f,25oct90,gae  made access of "lost carrier" 32-bit register be two 16-bit
                 access -- some boards generate a bus error.
02e,20oct90,gae  added detection of "lost carrier"; added but ifdef'd out
                 enpShow routine; cleanup.
02d,02oct90,hjb  de-linted.
02c,19sep90,dab  made enpinit() return int.  made bcopy_to_mbufs() call in
                 enpread() to use width of 1 for 680[01]0 cpus.
02b,26aug90,hjb  added a hack to avoid prob caused when copying data from
                 hkv2f a word at a time into ENP memory starting at an odd-byte
                 boundary.
02a,10aug90,dnw  added forward declarations for void functions.
                 added include of if_subr.h.
01z,11jul90,hjb  removed code that uses gotIp which is no longer necessary.
                 made enpread() void.  changed references to do_protocol()
                 to do_protocol_with_type().
01y,26jun90,hjb  copy_from_mbufs() & m_freem() fix; fixed enpattach() error msg
01x,07may90,hjb  changed bcopy_to_mbufs to copy 16 bits at a time.
01w,19apr90,hjb  deleted param.h, modifications for bcopy_from_mbufs().
01v,22mar90,hjb  changed copy_to_mbuf() to bcopy_to_mbuf() with argument
                 bcopyBytes() to fix the problems associated with using
                 ENP with hkv2f caused by bcopy'ing more than a byte at a time
                 over VME.
01u,18mar90,hjb  reduction of redundant code and addition of cluster support.
01t,06aug89,gae  changed iv68k.h to iv.h.
01s,25may89,hjb  enpioctl() SIOCSIFFLAGS, check for IFF_UP before calling
                 enpinit().
01r,24may89,rdc  enpintr no longer does netJobAdd if task level is
                 already processing packets. (fixes netRing overflows)
                 enpoutput now bumps if_opackets.
01q,17apr89,del  fixed comment in enpgetaddr.
01q,13apr89,del  fixed problem introduced by CMC when they changed to
                 version 4.0 Link-10 roms.
01p,05aug88,dnw  fixed oversight in enpreset in 01o.
01o,04aug88,dnw  fixed bug causing occasional initialization failure.
                 simplified reset.
01n,23jun88,rdc  added IFF_BROADCAST to if_flags.
01m,05jun88,dnw  changed enp_softc to array of ptrs to descriptors.
                 increased NENP from 1 to 4.
01l,30may88,dnw  changed to v4 names.
01k,22feb88,jcf  made kernel independent.
01j,20feb88,dnw  cleanup, lint.
01i,10feb88,rdc  re-installed enpreset which i accidently trashed in 01g.
                 optimized packet reception by calling ipintr only once
                 for a bunch of packets.
01h,14jan88,jcf  extended maximum firmware initialization wait to 15 seconds
01g,05jan88,rdc  added include of systm.h
01f,20nov87,ecs  lint
           &dnw  changed enpattach to take int vec num instead of vec adrs.
01e,19nov87,dnw  changed to program interrupt vector.
                 changed initial wait loop to use vxMemProbe.
01d,08nov87,dnw  added output hook code.
                 changed calling sequence to input hook.
01c,21oct87,ecs  delinted
01b,28aug87,dnw  removed unnecessary includes.
                 added input hook code.
                 clean up.
01a,21jul87,rdc  adapted from cmc's 4.2 driver.
*/

/*
This module implements the CMC ENP 10/L Ethernet network interface driver.

This driver is designed to be moderately generic, operating unmodified 
across the range of architectures and targets supported by VxWorks.
To achieve this, the driver must be given several target-specific
parameters.  These parameters, and the mechanisms used to communicate them
to the driver, are detailed below.  This driver supports up to four
individual units per CPU.


BOARD LAYOUT
The diagram below shows the relevant jumpers for VxWorks configuration.
Default values are: I/O address 0x00de0000, Standard Addressing (A24),
interrupt level 3.
.bS
______________________________              _______________________________
|             P1             |    ENP-10    |             P2              |
|                            ----------------                             |
|                   ::::X::                L                              |
|                   INT LVL                L INT ACK                      |
|                                          R                              |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
|   "                                                                     |
|   " IO ADRS                                                             |
|   -                                            -------  -------         |
|   "                                           |  ROM  ||  ROM  |        |
|   "                                           |LINK-10||LINK-10|        |
|   "                                           |       ||       |        |
|   "                                           | HIGH  ||  LOW  |        |
|                                               |  U4   ||  U3   |        |
|                                                ---^---  ---^---         |
|_________________________________________________________________________|
.bE
.bS
______________________________              _______________________________
|             P1             |  ENP-10/L    |             P2              |
|  JP17    JP16      JP15    ----------------                ----------   |
|  X:XX=_: ::X::::   ::::X::          " JP14                |  ROM     |  |
|          ADRS BITS INT LVL          " ADRS BITS (EXT)     > LINK-10  |  |
|                                     "                     |  LOW  U63|  |
|    JP12                             "                      ==========   |
|  " STD/                             "                     |  ROM     |  |
|  " EXT                              "                     > LINK-10  |  |
|  "                                  "                     |  HIGH U62|  |
|  "                                  "                      ----------   |
|  -                                  _ JP13 TIMEOUT                      |
|                                     " JP13 TIMEOUT*                     |
|  "                                                                      |
|  " INT ACK                                                              |
|  - JP11                                                                 |
|            _. JP7 SYSCLOCK                                              |
|            ._ JP7 SYSCLOCK*                                             |
|      ._ JP8                                                             |
|                                                                         |
|                                                                         |
|                                                                         |
|_________________________________________________________________________|
.bE


EXTERNAL INTERFACE
This driver provides the standard external interface with the following
exceptions.  All initialization is performed within the attach routine;
there is no separate initialization routine.  Therefore, in the global interface
structure, the function pointer to the initialization routine is NULL.

The only user-callable routine is enpattach(), which publishes the `enp'
interface and initializes the driver and device.

TARGET-SPECIFIC PARAMETERS
.iP "base VME address of ENP memory"
This parameter is passed to the driver by enpattach().
The ENP board presents an area of its memory to the VME bus.  This
address is jumper selectable on the ENP.  This parameter is the same as
the address selected on the ENP.
.iP "VME address modifier code"
This parameter is passed to the driver by enpattach().
It specifies the AM (address modifier) code to use when the driver accesses
the VME address space of the ENP board.
.iP "interrupt vector"
This parameter is passed to the driver by enpattach().
It specifies the interrupt vector to be used by the driver
to service an interrupt from the ENP board.  The driver will connect
the interrupt handler to this vector by calling intConnect().
.iP "interrupt level"
This parameter is passed to the driver by enpattach().
It specifies the interrupt level that is associated with the
interrupt vector.  The driver enables the interrupt from the ENP by
calling sysIntEnable() and passing this parameter.
.LP

SYSTEM RESOURCE USAGE
When implemented, this driver requires the following system resources:

    - one mutual exclusion semaphore
    - one interrupt vector
    - 76 bytes in the initialized data section (data)
    - 808 bytes in the uninitialized data section (BSS)

The above data and BSS requirements are for the MC68020 architecture 
and may vary for other architectures.  Code size (text) varies greatly between
architectures and is therefore not quoted here.

The ENP board provides the buffers for all packets transmitted and received.
Therefore, the driver does not require any system memory to share with the
device.  This also eliminates all data cache coherency issues.

SEE ALSO: ifLib
*/

#include "vxWorks.h"
#include "net/mbuf.h"
#include "net/protosw.h"
#include "sys/ioctl.h"
#include "sys/socket.h"
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
#include "drv/netif/if_enp.h"
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
#include "cacheLib.h"


/***** LOCAL DEFINITIONS *****/

#undef ENP_DEBUG            /* define to display error/status messages */
#define MAX_UNITS    4                  /* max units to support */

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
#endif

#define ENET_HDR_SIZ        sizeof(ENET_HDR)
#define ENET_HDR_REAL_SIZ   14

/* The definition of the driver control structure */

typedef struct drv_ctrl
    {
    IDR         idr;                    /* interface data record */

    BOOL        attached;               /* TRUE if attach() has succeeded */
    ENPDEVICE   *enpAddr;               /* VME address of board */
    ENPSTAT     enpStat;                /* duplicate copy of ENP statisics */
    ULONG       lostCarrier;            /* statistic counter */
    int         enpIntLevel;            /* VME interrupt level */
    int         enpIntVec;              /* VME interrupt vector */
    BOOL        taskLevelActive;        /* netTask processing packets */
#ifdef BSD43_DRIVER
    SEM_ID      TxSem;                  /* transmitter semaphore */
#endif
    } DRV_CTRL;


/***** LOCALS *****/

/* The array of driver control structures, one per unit supported */

LOCAL DRV_CTRL drvCtrl [MAX_UNITS];

/*LOCAL*/ BOOL enpNewROM;       /* TRUE if LINK-10 v4.0 or higher */


/***** FUNCTION DECLARATIONS *****/

/* those not defined in header files */
extern int splimp (void);
extern void splx (int spl);

/* forward static functions */

static int enpIoctl (IDR *pIDR, int cmd, caddr_t data);
#ifdef BSD43_DRIVER
static int enpOutput (IDR *pIDR, MBUF *pMbuf, SOCK *dst);
#else
LOCAL void enpTxStartup (DRV_CTRL * pDrvCtrl);
#endif
static int enpringput (RING *rp, BCB *pBCB);
static int enpringget (RING *rp);
static void enpIntr (int unit);
static void enpRxEvent (int unit);
static void enpgetaddr (int unit);
static BOOL enpringempty (RING *rp);
#ifdef BSD43_DRIVER
static BOOL convertDestAddr
            (
            IDR *pIDR,
            SOCK *pDestSktAddr,
            char *pDestEnetAddr,
            u_short *pPacketType,
            MBUF *pMbuf
            );
#endif
static STATUS enpReset (int unit);
static STATUS enpRxPktProcess (int unit);


/*******************************************************************************
*
* enpattach - publish the `enp' network interface and initialize the driver and device
*
* This routine publishes the `enp' interface by filling in a network 
* interface record and adding this record to the system list.  It also
* initializes the driver and the device to the operational state.
*
* RETURNS: OK or ERROR.
*/

STATUS enpattach
    (
    int unit,           /* unit number                     */
    char *addr,         /* address of enp's shared memory  */
    int ivec,           /* interrupt vector to connect to  */
    int ilevel,         /* interrupt level                 */
    int enpAddrAm       /* address VME address modifier */
    )
    {
    int        delay;
    short      status;
    int        temp;
    ENPDEVICE  *pEnpDev;
    char       *enpBase;
    DRV_CTRL   *pDrvCtrl;

#ifdef ENP_DEBUG
    printf ( "\nenp: attach: unit=%d, addr=%x, ivec=%d, ilevel=%d, Am=%d\n",
             unit, addr, ivec, ilevel, enpAddrAm );
#endif

    /* Safety check unit number */

    if (unit < 0 || unit >= MAX_UNITS)
        {
        errnoSet (S_iosLib_CONTROLLER_NOT_PRESENT);
        return (ERROR);
        }

    /* Get pointer to our driver control structure */

    pDrvCtrl = & drvCtrl [unit];

    /* Check for multiple invocation per unit */

    if (pDrvCtrl->attached)
        return (OK);

    /* Publish the interface data record */

#ifdef BSD43_DRIVER
    ether_attach    ( & pDrvCtrl->idr.ac_if, unit, "enp", (FUNCPTR) NULL,
                      (FUNCPTR) enpIoctl, (FUNCPTR) enpOutput,
                      (FUNCPTR) enpReset);
#else
    ether_attach ( 
                 &pDrvCtrl->idr.ac_if, unit, 
                 "enp", 
                 (FUNCPTR)NULL,
                 (FUNCPTR)enpIoctl, 
                 (FUNCPTR)ether_output,
                 (FUNCPTR)enpReset
                 );
    pDrvCtrl->idr.ac_if.if_start = (FUNCPTR)enpTxStartup;
#endif

#ifdef BSD43_DRIVER
    /* Create the transmit semaphore. */

    pDrvCtrl->TxSem = semMCreate    (
                                    SEM_Q_PRIORITY |
                                    SEM_DELETE_SAFE |
                                    SEM_INVERSION_SAFE
                                    );

    if (pDrvCtrl->TxSem == NULL)
        {
#ifdef ENP_DEBUG
        printf ("enp: error creating transmitter semaphore\n");
#endif
        return (ERROR);
        }
#endif

    /* Determine local address and AM codes to access the controller */

    if (sysBusToLocalAdrs (enpAddrAm, addr, &enpBase) == ERROR)
        {
        errnoSet (EFAULT);
        return (ERROR);
        }

#ifdef ENP_DEBUG
    printf ( "enp: attach: local addr=%x\n", enpBase );
#endif

    pEnpDev = (ENPDEVICE *) enpBase;
    enpBase += 0x1000;  /* memory is offset from board base address */

    /* Connect the interrupt handler */

    (void) intConnect ((VOIDFUNCPTR *)INUM_TO_IVEC (ivec),
                       (VOIDFUNCPTR)enpIntr, unit);

    /* wait for kernel reset to finish */

    delay = 15 * sysClkRateGet ();
    while (1)
        {

        /* Test for response on the buss */

        if  (
            vxMemProbe  (
                        (char *) & pEnpDev->enp_status,
                        O_RDONLY,
                        sizeof (short),
                        (char *) & status
                        )
            == OK
            )
            {
            /* Check status word */
            if ( (ntohs (status) & STS_READY) != 0 )
                break;
            }

        /* Check for timeout */

        if (--delay <= 0)
            {
            (void) errnoSet (S_iosLib_CONTROLLER_NOT_PRESENT);
            return (ERROR);
            }

        taskDelay (sysClkRateGet() / 30);       /* 1/30th of a second */
        }

    /* start link level firmware */

    pEnpDev->enp_intvector = htons (ivec);
    temp = htonl ((u_long) enpBase);
    WRITELONG (&pEnpDev->enp_base, temp);

#if  CPU_FAMILY==I960
    pEnpDev->enp_mode = htons (0x0080);
#endif /* CPU_FAMILY==I960 */

    pEnpDev->enp_go = htons (GOVAL);

    CACHE_PIPE_FLUSH ();

    /* wait for the link level firmware to finish initialization */

    delay = 15 * sysClkRateGet ();
    while ((ntohs (pEnpDev->enp_state) & S_ENPRUN) == 0)
        {
        if (--delay <= 0)
            {
            (void) errnoSet (S_ioLib_DEVICE_ERROR);
            return (ERROR);
            }

        taskDelay (sysClkRateGet() / 30);       /* 1/30th of a second */
        }

    pDrvCtrl->enpAddr     = pEnpDev;
    pDrvCtrl->enpIntLevel = ilevel;
    pDrvCtrl->enpIntVec   = ivec;

    enpgetaddr (unit);

    (void) sysIntEnable (ilevel);

    /* Raise the interface flags */

    pDrvCtrl->idr.ac_if.if_flags |= IFF_UP | IFF_RUNNING | IFF_NOTRAILERS | IFF_MULTICAST;

    /* Set our success indicator */

    pDrvCtrl->attached = TRUE;

    return (OK);
    }

/*******************************************************************************
*
* enpReset - reset the interface
*/

LOCAL STATUS enpReset
    (
    int unit
    )
    {
    DRV_CTRL *pDrvCtrl;
    ENPDEVICE *pEnpDev;

#ifdef ENP_DEBUG
    printf ( "enp: reset: unit=%d\n", unit );
#endif

    /* Safety check unit number */

    if (unit < 0 || unit >= MAX_UNITS)
        {
        errnoSet (S_iosLib_CONTROLLER_NOT_PRESENT);
        return (ERROR);
        }

    pDrvCtrl = & drvCtrl[unit];
    pEnpDev = pDrvCtrl->enpAddr;
    pDrvCtrl->idr.ac_if.if_flags = 0;
    RESET_ENP (pEnpDev);

    /* reset the status word that enpattach watches to know when enp is reset;
     * this is necessary because the enp doesn't clear this status immediately
     * on reset.  Re-initializing the enp after a reset (i.e. in VxWorks
     * init after boot rom reset) could get fooled if we rely on enp to clear
     * the READY bit in this status.
     */
    pEnpDev->enp_status = 0;

    CACHE_PIPE_FLUSH ();

    return (OK);
    }

/*******************************************************************************
*
* enpIntr - the driver's interrupt handler
*
* The ENP board will only interrupt us to notify us that there are packets
* to be processed.  The processing of each packet is done by netTask.  This
* routine simply queues our job for netTask.
*/

LOCAL void enpIntr
    (
    int unit
    )
    {
    DRV_CTRL *pDrvCtrl;

    /* Safety check unit number */

    if (unit < 0 || unit >= MAX_UNITS)
        return;

    pDrvCtrl = & drvCtrl [unit];
    sysBusIntAck (pDrvCtrl->enpIntLevel);

    if (!pDrvCtrl->taskLevelActive)
        {
        /* indicate that enpHandleInt is q'd */
        pDrvCtrl->taskLevelActive = TRUE;
        (void) netJobAdd ((FUNCPTR)enpRxEvent, unit, 0,0,0,0);
        /* see comment in enpHandleInt */
        }
    }

/*******************************************************************************
*
* enpRxEvent - handler for receive events
*/

LOCAL void enpRxEvent
    (
    int unit
    )
    {
    DRV_CTRL  *pDrvCtrl;
    ENPDEVICE *pDev;


    pDrvCtrl = & drvCtrl [unit];
    pDev = pDrvCtrl->enpAddr;

    splx (0);       /* Give up the spl semaphore taken for us by netTask. */

    /* the following loop attempts to pull multiple buffers off of the
     * input queue;  the boolean "taskLevelActive" is used to inform
     * the interrupt level code that we're already processing input
     * buffers, so there's no need to do a netJobAdd, thereby decreasing
     * the possibility of a net task ring buffer overflow.
     */

    do
        {
        pDrvCtrl->taskLevelActive = TRUE;

        while (!enpringempty ((RING*) &pDev->enp_tohost))
            {
            if ( enpRxPktProcess (unit) == ERROR )
                {
                pDrvCtrl->taskLevelActive = FALSE;
                splimp ();      /* Take the spl semaphore for netTask */
                return;
                }
            }

        pDrvCtrl->taskLevelActive = FALSE;

        /* check once more after resetting taskLevelActive to avoid race */

        }
    while (!enpringempty ((RING*) &pDev->enp_tohost));

    splimp ();      /* Take the spl semaphore for netTask */
    }

/*******************************************************************************
*
* enpRxPktProcess - process the next received packet
*
* Copy packets from a BCB into an mbuf and hand it to the next higher layer.
*/

LOCAL STATUS enpRxPktProcess
    (
    int unit
    )
    {
    int             len;
    int             temp;
    DRV_CTRL        *pDrvCtrl;
    ENET_HDR        *pEnetHdr;
    MBUF            *pMbuf;
    BCB             *pBCB;
    ENPDEVICE       *pDev;
    unsigned char   *pData;


    /* Get control pointers */

    pDrvCtrl = & drvCtrl [unit];
    pDev = pDrvCtrl->enpAddr;

    /* Check ifnet flags. Return if incorrect. */

    if  (
        (pDrvCtrl->idr.ac_if.if_flags & (IFF_UP | IFF_RUNNING)) !=
        (IFF_UP | IFF_RUNNING)
        )
        return (ERROR);

    /* Get pointer to next packet */

    pBCB = (BCB*) enpringget ((RING*) &pDev->enp_tohost);
    if (pBCB == NULL)                     /* unlikely error */
        return (ERROR);

    /* Bump the incoming packet statistic */

    pDrvCtrl->idr.ac_if.if_ipackets++;

    /* Get length of packet */

    len = ntohs (pBCB->b_msglen);

    /* Get pointer to packet */

    READLONG ((& pBCB->b_addr), &temp);
    pEnetHdr = (ENET_HDR *) ntohl (temp);

    /* call input hook if any */

    if (etherInputHookRtn != NULL)
        {
        if  (
            (*etherInputHookRtn) (& pDrvCtrl->idr.ac_if, (char *) pEnetHdr, len)
            == 1
            )
            {
            (void) enpringput ((RING *)&pDev->enp_enpfree, pBCB);
            return (OK);
            }
        }

    /* Adjust length; subtract size of header */

    len -= ENET_HDR_REAL_SIZ;

    /* Get pointer to data section of the packet */

    pData = ((unsigned char *) pEnetHdr) + ENET_HDR_REAL_SIZ;

    /* copy data from enp to mbuf a word at a time */

#if (CPU == MC68000) || (CPU == MC68010) || (CPU == CPU32)
    pMbuf = bcopy_to_mbufs (pData, len, 0, (IFNET *) &pDrvCtrl->idr.ac_if, 1);
#else
    pMbuf = bcopy_to_mbufs (pData, len, 0, (IFNET *) &pDrvCtrl->idr.ac_if, 2);
#endif  /* (CPU == MC68000) || (CPU == MC68010) || (CPU == CPU32) */

    if (pMbuf != NULL)
#ifdef BSD43_DRIVER
        do_protocol_with_type   (
                                ntohs (pEnetHdr->type),
                                pMbuf,
                                & pDrvCtrl->idr,
                                len
                                );
#else
        do_protocol (pEnetHdr, pMbuf, &pDrvCtrl->idr, len);
#endif
    else
        ++pDrvCtrl->idr.ac_if.if_ierrors;       /* bump error stat */

    (void) enpringput ((RING *)&pDev->enp_enpfree, pBCB);
    return (OK);
    }

#ifdef BSD43_DRIVER
/*******************************************************************************
*
* enpOutput - the driver's output routine
*
* The destination address is passed to this function as a pointer to a
* generic socket address structure.  This address information is first
* converted to the six byte destination address and type field that we
* need to build a proper Ethernet frame.
*
* The data to be sent out is held in a chain of mbufs.  This function is
* passed a pointer to the head mbuf.  The data in each mbuf fragment must
* be copied to the device's packet buffer.
*/

LOCAL STATUS enpOutput
    (
    IDR *pIDR,
    MBUF *pMbuf,
    SOCK *pDest
    )
    {
    int         lostCarrier;
    int         blen;
    u_short     packetType;
    u_char      *bufStart;
    DRV_CTRL    *pDrvCtrl;
    ENPDEVICE   *pDev;
    MBUF        *pTempMbuf;
    BCB         *pBCB;
    u_char      *pBuf;
    char        destEnetAddr [6];

#ifdef ENP_DEBUG
    printf ( "enp: output: pIDR=%x, pMbuf=%x, pDest=%x\n",
             pIDR, pMbuf, pDest );
#endif

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
    pDev = (ENPDEVICE *) pDrvCtrl->enpAddr;

    /* Obtain exclusive access to transmitter.  This is necessary because
     * certain events can cause netTask to run a job that attempts to transmit
     * a packet.  We can only allow one task here at a time.
     */

    semTake (pDrvCtrl->TxSem, WAIT_FOREVER);

    /* some boards (eg. iv3201a,hkv2f,mv133) get bus errors with 32bit access */
    READLONG (&pDev->enp_stat.e_lost_carrier, &lostCarrier);
    lostCarrier = ntohl (lostCarrier);

    if (lostCarrier > pDrvCtrl->lostCarrier)
        {
        /* The test should probably be "not equal", however, roll-over
         * will take a long time.  After reset pDrvCtrl->lostCarrier won't
         * be in sync -- we should initialize (i.e. read) it elsewhere.
         *
         * Keep a separate count of lost carrier so statics don't
         * get messed up.
         */
        pDrvCtrl->lostCarrier = lostCarrier;
#ifdef ENP_DEBUG
        logMsg  ("enp%d: lost carrier\n",
                pDrvCtrl->idr.ac_if.if_unit, 0, 0, 0, 0, 0);
#endif
        }

    /* If no resource available, discard packet */

    if (enpringempty ((RING *) &pDev->enp_hostfree))
        {
        semGive (pDrvCtrl->TxSem);          /* Release exclusive access. */
        m_freem (pMbuf);
        return (OK);
        }

    pBCB = (BCB *) enpringget ((RING *)&pDev->enp_hostfree);

    /* Get pointer to buffer to fill with outgoing packet */

    READLONG ((&pBCB->b_addr),  &bufStart);
    bufStart = (u_char *) ntohl ((int)bufStart);

    /* First fill in the Ethernet header */

    pBuf = bufStart;
    bcopyBytes (destEnetAddr, (char *) pBuf, 6);
    pBuf += 6;
    bcopyBytes ((char *) pDrvCtrl->idr.ac_enaddr, (char *) pBuf, 6);
    pBuf += 6;
    bcopyBytes ( (char *)&packetType, (char *) pBuf, 2);
    pBuf += 2;

    /* copy data from mbufs to enp a word at a time.
     * most other netif drivers use copy_from_mbufs() to accomplish
     * this task.  we're doing this manually here because we
     * need to take care of a special case (hkv2f + ENP, see below).
     */

    pTempMbuf = pMbuf;                      /* copy ptr */

    while (pTempMbuf != NULL)
        {
        /* if destination adrs starts at odd byte boundary
         * copy a byte at a time.  copying a word from hkv2f into ENP memory
         * starting at an odd byte causes problems.
         */
        if (((int) pBuf & 1) == 1)
            bcopyBytes  (
                        mtod (pTempMbuf, char *),
                        (char *) pBuf,
                        (int) pTempMbuf->m_len
                        );
        else
            {
            blen = pTempMbuf->m_len >> 1;

            /* copy 2 bytes at a time */

            bcopyWords (mtod (pTempMbuf, char *), (char *) pBuf, blen);

            /* copy the remaining byte if any */

            blen <<= 1;
            if (blen != pTempMbuf->m_len)
                {
                *((char *) (pBuf + blen)) =
                    *((char *) (mtod (pTempMbuf, char *) + blen));
                }
            }

        /* Move pointer ahead, and get next mbuf pointer */

        pBuf += pTempMbuf->m_len;
        pTempMbuf = pTempMbuf->m_next;
        }

    /* Done with mbufs, release them to system */

    m_freem (pMbuf);

    /* Set length of packet */

    blen = max ( MINPKTSIZE, ((int) pBuf - (int) bufStart) );
    pBCB->b_len = htons (blen);

    /* Put the BCB on the outgoing ring, and if this is the only entry on
     * the ring, generate an interrupt to the ENP by writing to the special
     * location in the shared memory space.  Some boards require this
     * interrupt, while others work fine without it, and still others perform
     * a reset instead of the interrupt.  Go figure.
     */

    if ( enpringput ((RING *) & pDev->enp_toenp, pBCB) == 1 )
        pDev->enp_iow.hst2enp_interrupt = ENPVAL;

    /* Bump the statistic counter. */

    pIDR->ac_if.if_opackets++;

    /* Release exclusive access. */

    semGive (pDrvCtrl->TxSem);

    return (OK);
    }
#else
/*******************************************************************************
*
* enpTxStartup - kick start the transmitter after generic ether_output
*
*/

LOCAL void enpTxStartup
    (
    DRV_CTRL * pDrvCtrl 	/* pointer to driver control structure */
    )
    {
    int         lostCarrier;
    int         blen;
    u_char      *bufStart;
    ENPDEVICE   *pDev;
    MBUF        *pTempMbuf;
    BCB         *pBCB;
    u_char      *pBuf;
    MBUF *	pMbuf;

    pDev = (ENPDEVICE *) pDrvCtrl->enpAddr;

    /* BSD 4.4 drivers already have exclusive access - no semaphore needed. */

    /* some boards (eg. iv3201a,hkv2f,mv133) get bus errors with 32bit access */
    READLONG (&pDev->enp_stat.e_lost_carrier, &lostCarrier);
    lostCarrier = ntohl (lostCarrier);

    if (lostCarrier > pDrvCtrl->lostCarrier)
        {
        /* The test should probably be "not equal", however, roll-over
         * will take a long time.  After reset pDrvCtrl->lostCarrier won't
         * be in sync -- we should initialize (i.e. read) it elsewhere.
         *
         * Keep a separate count of lost carrier so statics don't
         * get messed up.
         */
        pDrvCtrl->lostCarrier = lostCarrier;
#ifdef ENP_DEBUG
        logMsg  ("enp%d: lost carrier\n",
                pDrvCtrl->idr.ac_if.if_unit, 0, 0, 0, 0, 0);
#endif
        }

    while (pDrvCtrl->idr.ac_if.if_snd.ifq_head)
        {
        IF_DEQUEUE (&pDrvCtrl->idr.ac_if.if_snd, pMbuf);

        /* If no resource available, discard packet */

        if (enpringempty ((RING *) &pDev->enp_hostfree))
            {
            if (pMbuf != NULL)
                m_freem (pMbuf);
            break;
            }

        pBCB = (BCB *) enpringget ((RING *)&pDev->enp_hostfree);

        /* Get pointer to buffer to fill with outgoing packet */

        READLONG ((&pBCB->b_addr),  &bufStart);
        bufStart = (u_char *) ntohl ((int)bufStart);
        pBuf = bufStart;

        /* copy data from mbufs to enp a word at a time.
         * most other netif drivers use copy_from_mbufs() to accomplish
         * this task.  we're doing this manually here because we
         * need to take care of a special case (hkv2f + ENP, see below).
         */

        pTempMbuf = pMbuf;                      /* copy ptr */

        while (pTempMbuf != NULL)
            {
            /* if destination adrs starts at odd byte boundary
             * copy a byte at a time.  copying a word from hkv2f into ENP memory
             * starting at an odd byte causes problems.
             */
            if (((int) pBuf & 1) == 1)
                bcopyBytes  (
                            mtod (pTempMbuf, char *),
                            (char *) pBuf,
                            (int) pTempMbuf->m_len
                            );
            else
                {
                blen = pTempMbuf->m_len >> 1;

                /* copy 2 bytes at a time */

                bcopyWords (mtod (pTempMbuf, char *), (char *) pBuf, blen);

                /* copy the remaining byte if any */

                blen <<= 1;
                if (blen != pTempMbuf->m_len)
                    {
                    *((char *) (pBuf + blen)) =
                        *((char *) (mtod (pTempMbuf, char *) + blen));
                    }
                }

            /* Move pointer ahead, and get next mbuf pointer */

            pBuf += pTempMbuf->m_len;
            pTempMbuf = pTempMbuf->m_next;
            }

        /* Done with mbufs, release them to system */

        m_freem (pMbuf);

        /* Set length of packet */

        blen = max ( MINPKTSIZE, ((int) pBuf - (int) bufStart) );
        pBCB->b_len = htons (blen);

        /* Put the BCB on the outgoing ring, and if this is the only entry on
         * the ring, generate an interrupt to the ENP by writing to the special
         * location in the shared memory space.  Some boards require this
         * interrupt, while others work fine without it, and still others 
         * perform a reset instead of the interrupt.  Go figure.
         */

        if ( enpringput ((RING *) & pDev->enp_toenp, pBCB) == 1 )
            pDev->enp_iow.hst2enp_interrupt = ENPVAL;

        /* Bump the statistic counter. */

        pDrvCtrl->idr.ac_if.if_opackets++;
        }
    return;
    }
#endif

/*******************************************************************************
*
* enpIoctl - ioctl for interface
*
* RETURNS: 0, or EINVAL
*/

LOCAL int enpIoctl
    (
    IDR *pIDR,
    int cmd,
    caddr_t data
    )
    {
    int error;
    ENPDEVICE* pEnpDev;
    DRV_CTRL* pDrvCtrl;
    
    pDrvCtrl = & drvCtrl [pIDR->ac_if.if_unit];

#ifdef ENP_DEBUG
    printf ( "enp: ioctl: pIDR=%x, cmd=%x, data=%x\n", pIDR, cmd, data );
#endif

    error = 0;

    switch (cmd)
        {
        case SIOCSIFADDR:
            ((struct arpcom *)pIDR)->ac_ipaddr = IA_SIN (data)->sin_addr;
            arpwhohas (pIDR, &IA_SIN (data)->sin_addr);
            break;

        case SIOCSIFFLAGS:
            /* set promiscuous bit according to flags */

            if (pDrvCtrl->idr.ac_if.if_flags & IFF_PROMISC) 
                {
                pEnpDev = pDrvCtrl->enpAddr;
                pEnpDev->enp_mode |= 0x1000;
                }
            else
                {
                pEnpDev = pDrvCtrl->enpAddr;
                pEnpDev->enp_mode &= ~0x1000;
                }   
            break;

            /* Just set the thing into promiscuous mode. */
        case SIOCADDMULTI:
        case SIOCDELMULTI:
            pEnpDev = pDrvCtrl->enpAddr;
            pEnpDev->enp_mode |= 0x1000;
            break;
        default:
            error = EINVAL;
        }

    return (error);
    }

/*******************************************************************************
*
* enpgetaddr - get hardwired ethernet address.
*
* Read the ethernet address off the board, one byte at a time.
*
* INTERNAL
* CMC changed the LINK-10 ROMs and did not make the change
* to ENPDEVICE backward compatable with v3.x.
* To determine the proper ENPDEVICE structure (or rather LINK-10
* revision) the Ethernet base address should be 0.
* If it is we assume that the LINK-10 PROMs are v4.0.
* This assumtion is based on the hope that the new LINK-10 ROMs will
* leave the RING32 buffers that were inserted in the ENPDEVICE structure
* set to 0 after a reset. This seems to work. (see if_enp.h for differences
* in the ENPDEVICE structure).
* The fields e_stat, and/or e_netaddr (AKA enp_stat and enp_addr) should
* are dependent on the ROM version.
*/

LOCAL void enpgetaddr
    (
    int unit
    )
    {
    DRV_CTRL *pDrvCtrl          = & drvCtrl[unit];
    ENPDEVICE_OLD *oldaddr = (ENPDEVICE_OLD *) pDrvCtrl->enpAddr;
    ENPDEVICE     *addr    = pDrvCtrl->enpAddr;
    USHORT        *pChk    = (USHORT *) &oldaddr->enp_addr.e_baseaddr;

    /* check version of LINK-10 PROMs */

    enpNewROM = (*pChk == 0x00);        /* TRUE = new ROMs */

    if (enpNewROM)
        {
        bcopyBytes  (
                    (caddr_t) & addr->enp_addr.e_baseaddr,
                    (caddr_t) pDrvCtrl->idr.ac_enaddr,
                    sizeof (pDrvCtrl->idr.ac_enaddr)
                    );
        }
    else
        {
        /* CMC LINK-10 v4.0 PROMs */
        bcopyBytes  (
                    (caddr_t) & oldaddr->enp_addr.e_baseaddr,
                    (caddr_t) pDrvCtrl->idr.ac_enaddr,
                    sizeof (pDrvCtrl->idr.ac_enaddr)
                    );
        }
    }

/*******************************************************************************
*
* enpringempty - check for ring empty.
*
* RETURNS: TRUE or FALSE
*/

LOCAL BOOL enpringempty
    (
    RING *rp
    )
    {
    return (rp->r_rdidx == rp->r_wrtidx);
    }

/*******************************************************************************
*
* enpringput - write to next ring location
*
* RETURNS: Number of entries left in ring or 0 if full.
*/

LOCAL int enpringput
    (
    RING *rp,
    BCB *pBCB
    )
    {
    int idx;
    int wrtidx = ntohs (rp->r_wrtidx);
    int rdidx  = ntohs (rp->r_rdidx);
    int size   = ntohs (rp->r_size);

    idx = (wrtidx + 1) & (size-1);

    if (idx != rdidx)
        {
        pBCB = (BCB *) htonl ((u_long) pBCB);
        WRITELONG (&rp->r_slot[ wrtidx ], pBCB);
        rp->r_wrtidx = htons (idx);

        CACHE_PIPE_FLUSH ();

        if ((idx -= rdidx) < 0)
            idx += size;

        return (idx);                   /* num ring entries */
        }

    return (0);
    }

/*******************************************************************************
*
* enpringget - get the next item from a ring
*
* RETURNS: Pointer to next item or 0 if none.
*/

LOCAL int enpringget
    (
    RING *rp
    )
    {
    int buf = 0;
    int wrtidx = ntohs (rp->r_wrtidx);
    int rdidx  = ntohs (rp->r_rdidx);
    int size   = ntohs (rp->r_size);

    if  (rdidx != wrtidx)
        {
        READLONG (&rp->r_slot [rdidx], &buf);
        buf = ntohl (buf);
        rp->r_rdidx = htons ((++rdidx) & (size-1));
        CACHE_PIPE_FLUSH ();
        }

    return (buf);
    }

/*******************************************************************************
*
* enpShow - show ENP-10 statistics
*
* INTERNAL
* This works only on some versions of the ENP 10/L onboard software
* e.g. LINK-10 4.0 is fine, 3.1 is not.
*
* RETURNS: 0
*
* NOMANUAL
*/

STATUS enpShow
    (
    int unit,   /* interface unit */
    BOOL zap    /* zero totals */
    )
    {
    int             ix;
    int             status;
    DRV_CTRL        *pDrvCtrl;
    ENPDEVICE       *addr;
    ENPDEVICE_OLD   *oldaddr;

    static char *e_message [] =
        {
        "successful transmissions",
        "multiple retries on xmit",
        "single retries",
        "too many retries",
        "transmission delayed",
        "xmit data chaining failed",
        "transmit data fetch failed",
        "collision after xmit",
        "lost carrier",
        "xmit length > 1518",
        "transceiver mismatch",
        "xmit memory error",
        "good receptions",
        "no recv buff available",
        "checksum failed",
        "crc error AND data length != 0 mod 8",
        "rcv data chain failure",
        "receive data store failed",
        "rcv memory error"
        };

    /* Safety check unit number */

    if (unit < 0 || unit >= MAX_UNITS)
        {
        errnoSet (S_iosLib_CONTROLLER_NOT_PRESENT);
        return (ERROR);
        }

    pDrvCtrl = & drvCtrl [unit];
    addr     = pDrvCtrl->enpAddr;
    oldaddr  = (ENPDEVICE_OLD *) pDrvCtrl->enpAddr;

    printf ("enp%d (v%s):\n", unit, (enpNewROM ? "4.x" : "3.x"));

    for (ix = 0; ix < NELEMENTS(e_message); ix++)
        {
        if (enpNewROM)
            {
            READLONG ((&addr->enp_stat.e_stat [ix]), &status);
            }
        else
            {
            READLONG ((&oldaddr->enp_stat.e_stat [ix]), &status);
            }

        status = ntohl (status);
        printf ("    %-30.30s  %4d\n", e_message [ix],
                status - pDrvCtrl->enpStat.e_stat [ix]);
        if (zap)
            pDrvCtrl->enpStat.e_stat [ix] = status;
        }

    return (OK);
    }

#if     FALSE
/*******************************************************************************
*
* enpDumpRing - dump particular ring
*
* RETURNS: N/A
*/

LOCAL void enpDumpRing (rn, rp)
    char *rn;           /* ring name */
    RING32 *rp;         /* ring pointer */

    {
    int ix;
    int temp;

    printf ("    %s: rdidx = 0x%04.4x, wridx = 0x%04.4x, size = 0x%04.4x\n",
        rn, ntohs (rp->r_rdidx), ntohs (rp->r_wrtidx), ntohs (rp->r_size));

    for (ix = 0; ix < 32;)
        {
        if ((ix % 6) == 0)
            printf ("      ");

        READLONG (&rp->r_slot[ix], &temp);
        printf ("%02.2d:%08.8x ", ix , ntohl (temp));

        if ((++ix % 6) == 0)
            printf ("\n");
        }
    printf ("\n");
    }

/*******************************************************************************
*
* enpDump - dump ring
*
* RETURNS: N/A
*
* NOMANUAL
*/

void enpDump (unit, ring)
    int unit;           /* interface unit */
    BOOL ring;          /* true if dump ring structure */
    {
    DRV_CTRL *pDrvCtrl          = & drvCtrl[unit];
    ENPDEVICE *addr        = pDrvCtrl->enpAddr;
    ENPDEVICE_OLD *oldaddr = (ENPDEVICE_OLD *) pDrvCtrl->enpAddr;
    int temp;

    if (enpNewROM)
        {
        temp = ntohs (addr->enp_state);
        printf ("enp%d: is %s\n", unit, temp & S_ENPRUN ? "running" : "down!");

        if ((temp & S_ENPRUN) == 0x0)
            return;

        printf ("    state = 0x%04.4x, mode  = 0x%04.4x\n",
            ntohs (addr->enp_state), ntohs (addr->enp_mode) );
        READLONG (&addr->enp_base, &temp);
        printf ("    base  = 0x%08.8x\n", ntohl (temp));
        printf ("    run   = 0x%04.4x, ivec  = 0x%04.4x\n\n",
            ntohs (addr->enp_enprun), ntohs (addr->enp_intvector));

        if (ring)
            {
            enpDumpRing ("toenp",    &addr->enp_toenp);
            enpDumpRing ("hostfree", &addr->enp_hostfree);
            enpDumpRing ("tohost",   &addr->enp_tohost);
            enpDumpRing ("enpfree",  &addr->enp_enpfree);
         /* enpDumpRing ("rcvdma",   &addr->enp_rcvdma); */
         /* enpDumpRing ("rcv_d",    &addr->enp_rvc_d); */
            }
        }
    else
        {
        temp = ntohs (oldaddr->enp_state);
        printf ("enp%d: is %s\n", unit, temp & S_ENPRUN ? "running" : "down!");

        if ((temp & S_ENPRUN) == 0x0)
            return;

        printf ("    state = 0x%04.4x, mode  = 0x%04.4x\n",
                ntohs (oldaddr->enp_state), ntohs (oldaddr->enp_mode) );
        READLONG (&oldaddr->enp_base, &temp);
        printf ("    base  = 0x%08.8x\n", ntohl (temp));
        printf ("    run   = 0x%04.4x, ivec  = 0x%04.4x\n\n",
                ntohs (oldaddr->enp_enprun), ntohs (oldaddr->enp_intvector));

        if (ring)
            {
            enpDumpRing ("toenp",    &oldaddr->enp_toenp);
            enpDumpRing ("hostfree", &oldaddr->enp_hostfree);
            enpDumpRing ("tohost",   &oldaddr->enp_tohost);
            enpDumpRing ("enpfree",  &oldaddr->enp_enpfree);
            }
        }
    }
#endif  /* FALSE */

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
    extern BOOL arpresolve();           /* we call this */

    /***** Internet family *****/

    {
    struct in_addr destIPAddr;              /* not used */
    int trailers;                           /* not supported */

     if (pDestSktAddr->sa_family == AF_INET)
        {
        *pPacketType = htons(ETHERTYPE_IP);            /* stuff packet type */
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
        *pPacketType = htons(pEnetHdr->type);                      /* copy type */
        return (TRUE);
        }
    }


    /* Unsupported family */

    return (FALSE);

    } /* End of convertDestAddr() */
#endif

/* END OF FILE */
