/* if_sl.c - Serial Line IP (SLIP) network interface driver */

/* Copyright 1989 - 2002 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*	$NetBSD: if_sl.c,v 1.33 1994/12/11 21:39:05 mycroft Exp $	*/

/*
 * Copyright (c) 1987, 1989, 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)if_sl.c	8.6 (Berkeley) 2/1/94
 */

/*
modification history
--------------------
02e,16may02,vvv  replaced schednetisr with direct call to ipintr;
		 fixed compilation warnings
02d,17oct00,spm  merged from version 02e of tor2_0_x branch (base version 02c):
                 updated for new if_attach: reports memory allocation failures
02c,08may97,vin  switched the order of ifDstAddrSet and ifAddrSet, fixed
		 SIOCSIFDSTADDR in slioctl, SPR 8531, added slRtRequest().
02b,01aug97,jmb	 Reduce number of slread netJobs by a factor of two
		 when FRAME_END character used to begin a new frame.
02a,16jul97,jmb	 Added code to discard packets at interrupt level if
		 the input ring buffer is full.
01z,04dec96,vin  replaced MFREE with m_free (..)
01y,18jul96,vin  upgraded to BSD4.4, removed call to in_ifaddr_remove as it 
		 is being done in if_dettach now. 
01x,07mar96,gnn	 Added code to handle user selectable MTUs for the SLIP
	         link.  (SPR #4652)
01w,14mar95,dzb  make sure to flush the wrt task's queue on close (SPR #4111).
01v,13mar95,dzb  added call to in_ifaddr_remove when deleting if (SPR #4109).
01u,13mar95,dzb  changed to use free() instead of cfree() (SPR #4113).
01t,24jan95,jdi  doc cleanup.
01s,08dec94,dzb  return ENOBUFS when dropping packet in sloutput() (SPR #3844).
01r,11nov94,dzb  added CSLIP enhancements.  decreased SLMTU size.  doc tweaks.
		 implemented buffer loaning, and removed extra data copies.
		 replaced calls to sl_btom() with copy_to_mbufs().
01q,22apr93,caf  ansification: added cast to ioctl() parameter.
01p,11aug93,jmm  Changed ioctl.h and socket.h to sys/ioctl.h and sys/socket.h
01o,19feb93,jdi  documentation cleanup.
01n,22sep92,gae  documentation tweaks.
01m,18jul92,smb  Changed errno.h to errnoLib.h.
01l,26may92,rrr  the tree shuffle
		  -changed includes to have absolute path from h/
01k,07nov91,jpb  made slipWrtTask global for task TCB "ENTRY" status.
01j,04oct91,rrr  passed through the ansification filter
                  -changed functions to ansi style
		  -changed includes to have absolute path from h/
		  -changed VOID to void
		  -changed copyright notice
01i,10apr91,jdi  documentation cleanup; removed stray close-quote in example;
		 doc review by elh.
01h,10aug90,dnw  corrected forward declarations for void functions.
01g,26jun90,hjb  SLBUFSIZE increased to fix a bug in ESC handling.
01f,26jun90,jcf  changed semaphore initialization to 5.0.
01e,07may90,hjb  changed MIN,MAX to min,max; de-linted.
01d,19apr90,hjb  deleted param.h, de-linted.
01c,20mar90,jcf  changed semaphores to binary.
01b,18mar90,hjb  minor cleanup.
01a,11apr89,hjb  first vxWorks version.
*/

/*
DESCRIPTION
This module implements the VxWorks Serial Line IP (SLIP) network 
interface driver.  Support for compressed TCP/IP headers (CSLIP) is included.

The SLIP driver enables VxWorks to talk to other machines over serial
connections by encapsulating IP packets into streams of bytes suitable
for serial transmission.

USER-CALLABLE ROUTINES
SLIP devices are initialized using slipInit().  Its parameters specify 
the Internet address for both sides of the SLIP point-to-point link,
the name of the tty device on the local host, and options to enable CSLIP
header compression.  The slipInit() routine calls slattach() to attach the
SLIP interface to the network.  The slipDelete() routine deletes a specified
SLIP interface.

LINK-LEVEL PROTOCOL
SLIP is a simple protocol that uses four token characters
to delimit each packet:

    - END (0300)
    - ESC (0333)
    - TRANS_END (0334)
    - TRANS_ESC (0335)

The END character denotes the end of an IP packet.  The ESC character is used
with TRANS_END and TRANS_ESC to circumvent potential occurrences of END or ESC
within a packet.  If the END character is to be embedded, SLIP sends 
"ESC TRANS_END" to avoid confusion between a SLIP-specific END and actual
data whose value is END.  If the ESC character is to be embedded, then
SLIP sends "ESC TRANS_ESC" to avoid confusion.  (Note that the SLIP ESC is
not the same as the ASCII ESC.)

On the receiving side of the connection, SLIP uses the opposite actions to
decode the SLIP packets.  Whenever an END character is received, SLIP
assumes a full IP packet has been received and sends it up to the IP layer.

TARGET-SPECIFIC PARAMETERS
The global flag slipLoopBack is set to 1 by default. This flag enables
the packets to be sent to the loopback interface if they are destined to
to a local slip interface address. By setting this flag, any packets
sent to a local slip interface address will not be seen on the actual serial
link. Set this flag to 0 to turn off this facility. If this flag is not set
any packets sent to the local slip interface address will actually be sent
out on the link and it is the peer's responsibility to loop the packet back.

IMPLEMENTATION
The write side of a SLIP connection is an independent task.  Each SLIP
interface has its own output task that sends SLIP packets over a
particular tty device channel.  Whenever a packet is ready to be sent out,
the SLIP driver activates this task by giving a semaphore.  When the
semaphore is available, the output task performs packetization
(as explained above) and writes the packet to the tty device.

The receiving side is implemented as a "hook" into the tty driver.  A tty
ioctl() request, FIOPROTOHOOK, informs the tty driver to call the SLIP
interrupt routine every time a character is received from a serial port.
By tracking the number of characters and watching for the END character,
the number of calls to read() and context switching time have been
reduced.  The SLIP interrupt routine will queue a call to the SLIP read
routine only when it knows that a packet is ready in the tty driver's ring
buffer.  The SLIP read routine will read a whole SLIP packet at a time and
process it according to the SLIP framing rules.  When a full IP packet is
decoded out of a SLIP packet, it is queued to IP's input queue.

CSLIP compression is implemented to decrease the size of the TCP/IP
header information, thereby improving the data to header size ratio.
CSLIP manipulates header information just before a
packet is sent and just after a packet is received.  Only TCP/IP 
headers are compressed and uncompressed; other protocol types are
sent and received normally.  A functioning CSLIP driver is required
on the peer (destination) end of the physical link in order to carry
out a CSLIP "conversation."

Multiple units are supported by this driver.  Each individual unit may
have CSLIP support disabled or enabled, independent of the state of
other units.

INTERNAL
The write side of a SLIP connection is an independent task.  Each SLIP
interface will have its own output task that will send SLIP packets over a
particular tty device channel.  Whenever a packet is ready to be sent out,
sloutput() will activate this task by giving a semaphore.  When the
semaphore is available, the output task will perform packetization
(as explained above) and write the packet to the tty device.

The receiving side is implemented as a "hook" into the tty driver.  A tty
ioctl() request, FIOPROTOHOOK, informs the tty driver to call slintr() every
time a character is received from a serial port.  By tracking the number
of characters and watching for the END character, the number of calls to
read() and context switching time have been reduced.  The slintr() routine
will queue a call to slread() only when it knows that a packet is ready in
the tty driver's ring buffer.  slread() will read a whole SLIP packet at a
time and process it according to the SLIP framing rules.  When a full IP
packet is decoded out of a SLIP packet, it is enqueued to IP's input queue.

The CSLIP compressed header format is only implemented for TCP/IP packets.
The slcompress.c:sl_compress_tcp routine is called from sloutput() to
compress the header while the outgoing packet is still in an mbuf chain.
The slcompress.c:sl_uncompress_tcp routine is called from slRecv() to
uncompress the header before the packet is copied to an mbuf chain (i.e.,
the incoming packet is in a contiguous buffer).
The CSLIP header's first octet contains the packet type (top 3 bits), TCP
'push' bit, and flags that indicate which of the 4 TCP sequence
numbers have changed (bottom 5 bits).  The next octet is a
conversation number that associates a saved IP/TCP header with
the compressed packet.  The next two octets are the TCP checksum
from the original datagram.  The next 0 to 15 octets are
sequence number changes, one change per bit set in the header
(there may be no changes and there are two special cases where
the receiver implicitly knows what changed).
There are 5 numbers which can change (they are always inserted
in the following order): TCP urgent pointer, window,
acknowlegement, sequence number and IP ID.  (The urgent pointer
is different from the others in that its value is sent, not the
change in value.)  Since typical use of SLIP links is biased
toward small packets, changes use a variable length coding with
one octet for numbers in the range 1 - 255 and 3 octets (0, MSB,
LSB) for numbers in the range 256 - 65535 or 0.  (If the change
in sequence number or ack is more than 65535, an uncompressed
packet is sent.)

BOARD LAYOUT
No hardware is directly associated with this driver; therefore, a jumpering
diagram is not applicable.

SEE ALSO
ifLib, tyLib,
John Romkey: RFC-1055,
.I "A Nonstandard for Transmission of IP Datagrams Over Serial Lines: SLIP,"
Van Jacobson: RFC-1144, entitled
.I "Compressing TCP/IP Headers for Low-Speed Serial Links"

ACKNOWLEDGEMENT
This program is based on original work done by Rick Adams of The Center for
Seismic Studies and Chris Torek of The University of Maryland.
The CSLIP enhancements are based on work done by Van Jacobson of
University of California, Berkeley for the "cslip-2.7" release.

INTERNAL

.CS
        slipInit   sloutput    slipDelete   slipBaudSet   slintr   slioctl
           /\          |              |                     |
          /  \         |semaphore     |                     |
         /    \        |              |                     |
        /      \       |--|           |                     |
 slattach slipWrtTask <-  |           |                   slread
      |                   |           |                     |
      |          sl_compress_tcp      |  sl_uncompress_tcp  |
      |                               |         |           |
  slSoftcFree <-----------------------+         ---------slRecv
.CE
*/

/* includes */

#include "vxWorks.h"
#include "sys/socket.h"
#include "sys/ioctl.h"
#include "stdio.h"
#include "errnoLib.h"
#include "ioLib.h"
#include "semLib.h"
#include "memLib.h"
#include "taskLib.h"
#include "tickLib.h"
#include "logLib.h"
#include "ifLib.h"
#include "routeLib.h"
#include "netLib.h"
#include "etherLib.h"
#include "net/if.h"
#include "net/if_types.h"
#include "net/if_subr.h"
#include "net/route.h"
#include "net/mbuf.h"
#include "net/unixLib.h"
#include "netinet/in.h"
#include "netinet/in_systm.h"
#include "netinet/in_var.h"
#include "netinet/ip.h"
#include "netinet/ip_var.h"
#include "netinet/sl_compress.h"
#include "if_sl.h"
#include "drv/netif/netifDev.h"
#include "rngLib.h"
#include "tyLib.h"

/* defines */

/*
 * SLMTU is now a hard limit on input packet size.  SLMTU must be <= MCLBYTES
 * to take advantage of buffer loaning.  Additionally, it has been shown
 * that a smaller MTU works well with SLIP, so that echo traffic does
 * not have to wait for larger, bulk data packets.  With an MTU that is
 * 10 to 20 times that TCP/IP header, a reasonable line efficiency is
 * achieved.  For SLIP, this dictates an MTU of 400 to 800 bytes.
 * CSLIP could do well with an MTU as low as 100 to 400 bytes.
 */


#define	SLMTU		576		/* max Tx/Rx size in bytes */
#define L_POOL		6		/* Rx loaner buffer pool */
#define NSLIP		20		/* max. number of SLIP interfaces */
#define	SLBUF_OFF	(sizeof(struct ifnet *) + 128)
#define SLBUF_HI	((SLMTU*2) - 1)	/* Rx character threshold */
#define SLBUFSIZE	((SLMTU*2) + SLBUF_OFF)
                                        /* leave room for CSLIP uncompress */

/* SLIP protocol characters */

#define FRAME_END	 	0300		/* Frame End */
#define FRAME_ESCAPE		0333		/* Frame Esc */
#define TRANS_FRAME_END	 	0334		/* transposed frame end */
#define TRANS_FRAME_ESCAPE 	0335		/* transposed frame esc */

/* SLIP control flags */

#define SL_COMPRESS	0x01	/* enable CSLIP compression */
#define SL_COMPRESS_RX	0x02	/* enable compression on Rx of compress pkt */
#define SL_INTR_PACKET_DISCARD 0x04  /* discard current packet at int. level */

#define SIN(s) ((struct sockaddr_in *)s)
    
/* typedefs */

typedef	struct		/* SL_SOFTC */
    {
    struct ifnet      sc_if;		/* network-visible interface */
    u_char            sc_flags;		/* SLIP interface control flags */
    short	      sc_ilen;		/* chars in current Rx packet */
    short	      sc_qlen;		/* num of chars in tty input queue */
    char *	      sc_buf;		/* input buffer */
    char *            sc_orig;		/* pointer to malloc'ed buffer space */
    int	 	      sc_fd;		/* file descriptor for slip device */
    SEM_ID	      sc_wrtSem;	/* write semaphore */
    int		      sc_wrtTaskId;	/* write task ID */
    struct slcompress sc_comp;		/* TCP/IP compression info */
    int		      sc_nLoanRxBuf;	/* number of Rx buffers left to loan */
    char *            sc_lPool[L_POOL];	/* receive buffer loaner pool */
    UINT8 *           sc_pRefC[L_POOL];	/* stack of reference count pointers */
    UINT8	      sc_refC[L_POOL];	/* actual reference count values */
    } SL_SOFTC;

/* globals */

int slipTaskPriority	= 55;	/* netTask priority +5 */
int slipTaskOptions	= VX_SUPERVISOR_MODE | VX_UNBREAKABLE;
int slipTaskStackSize	= 2000;
int slipLoopBack 	= 1;

IMPORT void if_dettach (struct ifnet *ifp);
IMPORT void if_down (struct ifnet *ifp);

IMPORT struct  ifqueue ipintrq; 
IMPORT struct  ifnet loif;

/* locals */

LOCAL SL_SOFTC *sl_softc [NSLIP];

/* forward declarations */

STATUS		slipDelete ();			/* global for user access */
void		slipWrtTask (SL_SOFTC *sc);	/* global for "ENTRY" status */
STATUS		slattach (int unit, int fd, BOOL compressEnable,
			  BOOL compressAllow, int mtu);

/* static forward declarations */

#ifdef __STDC__

static void slSoftcFree (SL_SOFTC *sc, int unit);
static void slinit (int unit);
static STATUS sloutput (struct ifnet *ifp, struct mbuf *m,
                        struct sockaddr* dst, struct rtentry * rtp);
static BOOL slintr (int unit, int inchar);
static void slread (SL_SOFTC *sc, int count);
static char *slRecv (SL_SOFTC *sc, BOOL firstCluster);
static int slioctl (struct ifnet *ifp, int cmd, caddr_t data);
static int numCharsToMask (u_char mask1, u_char mask2, u_int size, u_char *cp);
static void slLoanFree (SL_SOFTC *sc, char *pRxBuf, UINT8 *pRef);

static void slRtRequest (int cmd, struct rtentry * pRtEntry,
                         struct sockaddr * pSockAddr); 

#else	/* __STDC__ */

static void slSoftcFree ();
static void slinit ();
static STATUS sloutput ();
static BOOL slintr ();
static void slread ();
static char *slRecv ();
static int slioctl ();
static int numCharsToMask ();
static void slLoanFree ();

#endif	/* __STDC__ */

/*******************************************************************************
*
* slipInit - initialize a SLIP interface
*
* This routine initializes a SLIP device.  Its parameters specify the name
* of the tty device, the Internet addresses of both sides of the SLIP
* point-to-point link (i.e., the local and remote sides of the serial line
* connection), and CSLIP options.
*
* The Internet address of the local side of the connection is specified in
* <myAddr> and the name of its tty device is specified in <devName>.
* The Internet address of the remote side is specified in <peerAddr>.
* If <baud> is not zero, the baud rate will be the specified value;
* otherwise, the default baud rate will be the rate set by the tty driver.
* The <unit> parameter specifies the SLIP device unit number.  Up to twenty
* units may be created.
*
* The CLSIP options parameters <compressEnable> and <compressAllow> determine
* support for TCP/IP header compression.  If <compressAllow> is TRUE (1), then
* CSLIP will be enabled only if a CSLIP type packet is received by this
* device.  If <compressEnable> is TRUE (1), then CSLIP compression will be
* enabled explicitly for all transmitted packets, and compressed packets
* can be received.
*
* The MTU option parameter allows the setting of the MTU for the link.
* 
* For example, the following call initializes a SLIP device, using the
* console's second port, where the Internet address of the local host is
* 192.10.1.1 and the address of the remote host is 192.10.1.2.
* The baud rate will be the default rate for /tyCo/1.  CLSIP is enabled
* if a CSLIP type packet is received.  The MTU of the link is 1006.
* .CS
*     slipInit (0, "/tyCo/1", "192.10.1.1", "192.10.1.2", 0, 0, 1, 1006);
* .CE
*
* RETURNS:
* OK, or ERROR if the device cannot be opened, memory is insufficient,
* or the route is invalid.
*/

STATUS slipInit
    (
    int         unit,           /* SLIP device unit number (0 - 19) */
    char        *devName,       /* name of the tty device to be initialized */
    char        *myAddr,        /* address of the SLIP interface */
    char        *peerAddr,      /* address of the remote peer SLIP interface */
    int         baud,           /* baud rate of SLIP device: 0=don't set rate */
    BOOL	compressEnable,	/* explicitly enable CSLIP compression */
    BOOL	compressAllow,	/* enable CSLIP compression on Rx */
    int		mtu		/* user set-able MTU */
    )
    {
    SL_SOFTC	*sc;
    int 	slipFd;
    char 	slipName [10];
    char 	slipWrtName [10];

    /* check and see of unit is in valid range */
    if (unit < 0 || unit >= NSLIP)
	{
	(void)errnoSet (S_if_sl_INVALID_UNIT_NUMBER);
	return (ERROR);
	}

    if ((slipFd = open (devName, O_RDWR, 0)) == ERROR)
	return (ERROR);

    /* set the baud rate only if the 'baud' is not equal to zero */
    if (baud != 0 && ioctl (slipFd, FIOBAUDRATE, baud) == ERROR)
	{
	(void) close (slipFd);
	return (ERROR);
	}

    if (mtu <= 0 || mtu > 2048)
	mtu = SLMTU;
    /*
     * Re-allocate big enough ring buffers for this tty device since
     * the default ring buffer size of tty devices is 512 bytes, which
     * may be smaller than SLMTU.  The ring buffers are 4 times
     * the size of a maximum SLIP packet, for insurance.
     */
    if (ioctl (slipFd, FIORBUFSET, 4 * mtu) == ERROR ||
	ioctl (slipFd, FIOWBUFSET, 4 * mtu) == ERROR)
	{
	(void) close (slipFd);
	return (ERROR);
	}

    if (slattach (unit, slipFd, compressEnable, compressAllow, mtu) == ERROR)
	{
	(void) close (slipFd);
	return (ERROR);
	}

    sc = sl_softc [unit];

    (void) sprintf (slipName, "sl%d", sc->sc_if.if_unit);
    (void) sprintf (slipWrtName, "tSl%dWrt", sc->sc_if.if_unit);

    /* Set own and peer's internet address.
     * Add a routing table entry to tell how to loopback over SLIP.
     * Point to point loopback routing table entry is required to
     * properly talk to myself using the SLIP interface address.
     */
    if (ifDstAddrSet (slipName, peerAddr) == ERROR	||
        ifAddrSet (slipName, myAddr) == ERROR		||
	routeAdd (myAddr, myAddr) == ERROR)
	{
	(void) close (slipFd);
	return (ERROR);
	}

    /* start up the SLIP output task for this interface */

    sc->sc_wrtTaskId = taskSpawn (slipWrtName, slipTaskPriority,
		                  slipTaskOptions, slipTaskStackSize,
		                  (FUNCPTR) slipWrtTask, (int) sc,
				  0, 0, 0, 0, 0, 0, 0, 0, 0);

    if (sc->sc_wrtTaskId == ERROR)
	{
	(void) close (slipFd);
	return (ERROR);
	}

    return (OK);
    }

/*******************************************************************************
*
* slipBaudSet - set the baud rate for a SLIP interface
*
* This routine adjusts the baud rate of a tty device attached to a SLIP
* interface.  It provides a way to modify the baud rate of a tty
* device being used as a SLIP interface.
*
* RETURNS: OK, or ERROR if the unit number is invalid or uninitialized.
*/

STATUS slipBaudSet
    (
    int unit,           /* SLIP device unit number */
    int baud            /* baud rate */
    )
    {
    SL_SOFTC *sc;

    /* check for valid unit number */

    if (unit < 0 || unit >= NSLIP)
	{
	(void)errnoSet (S_if_sl_INVALID_UNIT_NUMBER);
	return (ERROR);
	}

    /* make sure that the sl_softc for the 'unit' is initialized properly */
    if ((sc = sl_softc [unit]) == NULL)
	{
	(void)errnoSet (S_if_sl_UNIT_UNINITIALIZED);
	return (ERROR);
	}

    return (ioctl (sc->sc_fd, FIOBAUDRATE, baud));
    }

/*******************************************************************************
*
* slattach - publish the `sl' network interface and initialize the driver and device
*
* This routine publishes the `sl' interface by filling in a network interface
* record and adding this record to the system list.  It also initializes
* the driver and the device to the operational state.
*
* This routine is usually called by slipInit().
*
* RETURNS: OK or ERROR.
*/

STATUS slattach
    (
    int		unit,		/* SLIP device unit number */
    int		fd,		/* fd of tty device for SLIP interface */
    BOOL	compressEnable,	/* explicitly enable CSLIP compression */
    BOOL	compressAllow,	/* enable CSLIP compression on Rx */
    int		mtu		/* user setable MTU */
    )
    {
    SL_SOFTC		*sc;
    struct ifnet	*ifp;
    int			ix;

    if (sl_softc [unit] != NULL)
	{
	(void)errnoSet (S_if_sl_UNIT_ALREADY_INITIALIZED);
	return (ERROR);
	}

    if ((sc = (SL_SOFTC *) calloc (1, sizeof (SL_SOFTC))) == (SL_SOFTC *) NULL)
	return (ERROR);

    if (mtu <= 0 || mtu > 2048)
	mtu = SLMTU;

    sl_softc [unit] = sc;

    ifp = &sc->sc_if;	/* get a pointer to ifnet structure */

    ifp->if_name 	   = "sl";
    ifp->if_unit 	   = unit;
    ifp->if_mtu 	   = mtu;
    ifp->if_flags 	   = IFF_POINTOPOINT | IFF_MULTICAST;
    sc->sc_if.if_type 	   = IFT_SLIP;
    ifp->if_ioctl 	   = slioctl;
    ifp->if_init	   = (FUNCPTR) slinit;
    ifp->if_output 	   = sloutput;
    ifp->if_reset 	   = slipDelete;
    ifp->if_snd.ifq_maxlen = IFQ_MAXLEN;

    /* allocate SLIP input buffer space, including loaner buffers */

    if ((sc->sc_orig = (char *) calloc (L_POOL, (u_int) SLBUFSIZE)) == NULL)
        {
        free ( (char *)sc);
        return (ERROR);
        }

    /* attach SLIP to linked list of all interfaces */

    if (if_attach (ifp) == ERROR)
        {
        free ( (char *)sc);
        return (ERROR);
        }

    sc->sc_flags	= 0;
    sc->sc_ilen		= 0;
    sc->sc_qlen		= 0;
    sc->sc_nLoanRxBuf	= L_POOL;

    /* set CSLIP options in the SL_SOFTC flags */

    if (compressEnable || compressAllow)
	{
        sl_compress_init (&sc->sc_comp);
        if (compressEnable)
            sc->sc_flags |= SL_COMPRESS;
	else
            sc->sc_flags |= SL_COMPRESS_RX;
        }

    sc->sc_buf = sc->sc_orig + SLBUF_OFF;	/* leave space for uncompress */

    /* assign loaner buffer addresses */

    for (ix = 0; ix < L_POOL; ix++)
	{
	sc->sc_lPool[ix] = (sc->sc_buf + (ix * SLBUFSIZE));
	sc->sc_refC[ix]  = 0;
	sc->sc_pRefC[ix] = &sc->sc_refC[ix];
	}

    if ((sc->sc_wrtSem = semBCreate (SEM_Q_PRIORITY, SEM_EMPTY)) == NULL)
	{
	slSoftcFree (sc, unit);
	return (ERROR);
	}

    /*
     * Make sure that the tty device described by 'fd' is
     * indeed a tty device.
     */
    if (ioctl (fd, FIOISATTY, 0 /*XXX*/) == TRUE)
	sc->sc_fd = fd;
    else
        {
	slSoftcFree (sc, unit);
	return (ERROR);
        }

    /*
     * call ioctl to,
     *   FIOFLUSH     - flush out everything on this tty dev
     *   FIOOPTIONS   - set the tty dev in RAW mode
     *   FIOPROTOHOOK - specify the protocol interface hook routine for SLIP
     *   FIOPROTOARG  - specify the SLIP unit number for this tty dev
     */

    if (ioctl (fd, FIOFLUSH, 0 /*XXX*/)        == ERROR ||
	ioctl (fd, FIOOPTIONS, OPT_RAW)        == ERROR ||
	ioctl (fd, FIOPROTOHOOK, (int) slintr) == ERROR ||
	ioctl (fd, FIOPROTOARG, unit)          == ERROR)
	{
	slSoftcFree (sc, unit);
	return (ERROR);
	}

    return (OK);
    }

/*******************************************************************************
*
* slipDelete - delete a SLIP interface
*
* This routine resets a specified SLIP interface.  It detaches the tty from
* the `sl' unit and deletes the specified SLIP interface from the list of
* network interfaces.  For example, the following call will delete the first
* SLIP interface from the list of network interfaces:
* .CS
*     slipDelete (0);
* .CE
*
* RETURNS: OK, or ERROR if the unit number is invalid or uninitialized.
*/

STATUS slipDelete
    (
    int unit                    /* SLIP unit number */
    )
    {
    SL_SOFTC	*sc;
    struct mbuf	*pMbuf;
    int		s;

    if (unit >= 0 && unit < NSLIP)
    	sc = sl_softc [unit];
    else
	{
	(void)errnoSet (S_if_sl_INVALID_UNIT_NUMBER);
    	return (ERROR);
	}

    if (sc == NULL)
	{
	(void)errnoSet (S_if_sl_UNIT_UNINITIALIZED);
	return (ERROR);
	}

    /* flush write side of the tty ringer buffer */

    (void) ioctl (sc->sc_fd, FIOWFLUSH, 0 /*XXX*/);
    (void) ioctl (sc->sc_fd, FIOPROTOHOOK, (int) NULL);

    s = splimp ();		/* paranoid; splnet probably ok */

    if_down (&sc->sc_if);		/* mark it down */
    (void) close (sc->sc_fd);		/* close the tty device */

    for (;;)			/* flush the wrt task queue and free mbufs */
	{
	IF_DEQUEUE (&sc->sc_if.if_snd, pMbuf);
	if (pMbuf == NULL)
	    break;
	m_freem (pMbuf);
	}

    if (sc->sc_wrtTaskId != ERROR)
        taskDelete (sc->sc_wrtTaskId);

    /*
     * Delete all routes associated with this SLIP interface.
     * This will effectively delete the point-to-point routes for this SLIP.
     */

    (void) ifRouteDelete ("sl", unit);

    slSoftcFree (sc, unit);

    splx (s);
    return (OK);
    }

/*******************************************************************************
*
* slSoftcFree - free the allocated sl_softc structure
*
* This is called by slattach to free the allocated sl_softc structure
* when error occurs within slattach.  The reason for having this routine
* is to get rid of the repetitive code within slattach ().
* This routine is also called by slipDelete ().
*/

LOCAL void slSoftcFree
    (
    SL_SOFTC *sc,
    int       unit
    )
    {
    /* Take the pointer to this SLIP interface out of the linked list
     * of all available network interfaces.  The if_dettach () will un-do
     * the action taken by if_attach ().
     */

    if_dettach (&sc->sc_if);

    if (sc->sc_wrtSem)
        semDelete (sc->sc_wrtSem);		/* delete write semaphore */

    if (sc->sc_buf)
	{
	(void) free (sc->sc_orig);
	sc->sc_buf = NULL;
	}

    (void) free ((char *) sc);
    sl_softc [unit] = (SL_SOFTC *) NULL;
    }

/*******************************************************************************
*
* slinit - initialize SLIP interface
*/

LOCAL void slinit
    (
    int unit
    )
    {
    SL_SOFTC *sc = sl_softc [unit];

    sc->sc_if.if_flags |= IFF_UP|IFF_RUNNING; /* open for business */
    }

/*******************************************************************************
*
* sloutput - output a packet over the SLIP line
*
* Queue a packet and trigger the slipWrtTask if a packet is ready
* by releasing the semaphore.  CSLIP compression for TCP/IP headers is
* supported if (enabled).
*
* RETURNS: OK, or ERROR if the output queue is full.
*/

LOCAL STATUS sloutput
    (
    FAST struct ifnet   *ifp,           /* SLIP interface pointer */
    FAST struct mbuf    *m,             /* SLIP packet to be sent out */
    struct sockaddr     *dst,           /* address of the destination */
    struct rtentry	*rtp		/* pointer to the route entry */
    )
    {
    FAST SL_SOFTC *sc;
    FAST struct ip *ip;
    FAST int 	   s;
    FAST int 	   unit;

    if ((ifp->if_flags & (IFF_UP|IFF_RUNNING)) != (IFF_UP|IFF_RUNNING))
	{
        m_freem (m);
        return (ENETDOWN);
	}

    /*
     * Check and see if protocol family is equal to AF_INET.
     * Currently only AF_INET is support for SLIP.
     */
    if (dst->sa_family != AF_INET)
        {
        m_freem (m);
	return (EAFNOSUPPORT);
        }

    unit = ifp->if_unit;

    sc = sl_softc [unit];

    /*
     * n.b.-- the `40' below checks that we have a least a min length
     * tcp/ip header contiguous in an mbuf.  We don't do `sizeof' on
     * some struct because too many compilers add random padding.
     */

    if (sc->sc_flags & SL_COMPRESS)		/* compress enabled ? */
	{
        ip = mtod(m, struct ip *);

        if ((ip->ip_p == IPPROTO_TCP) && (m->m_len >= 40))
            *mtod(m, u_char *) |= sl_compress_tcp (m, ip, &sc->sc_comp, 1);
        }

    s = splimp ();

    /*
     * If send queue is full drop the packet.
     */
    if (IF_QFULL (&ifp->if_snd))
        {
        IF_DROP (&ifp->if_snd);
        m_freem (m);
        ++sc->sc_if.if_oerrors;
        splx (s);

        return (ENOBUFS);
        }

    /* enqueue the packet for IP module to read later */

    IF_ENQUEUE (&ifp->if_snd, m);
    ifp->if_lastchange = tickGet (); 

    splx (s);

    /* wake the SLIP ouput task to let it send out this packet */

    semGive (sc->sc_wrtSem);

    return (OK);
    }

/*******************************************************************************
*
* slipWrtTask - SLIP writer task
*
* This is our write side task.  It waits for a semaphore and
* when sloutput() yields the semaphore, it will try to send out
* the packet that's been queue up so far.  Each SLIP interface
* has its own slipWrtTask which is spawned in slipInit().
* Start output on interface.  Get another datagram
* to send from the interface queue and map it to
* the interface before starting output.
*
* NOMANUAL
*/

void slipWrtTask
    (
    FAST SL_SOFTC *sc           /* SLIP softc pointer */
    )
    {
    FAST struct mbuf 	*m;
    FAST u_char 	*cp;
    FAST int 		 len;
    int 	 	 fd;
    struct mbuf	 	*m2;
    int 		 rdCount;
    int 		 numch;
    char 		 ch;
    int 		 s;

    fd = sc->sc_fd;

    FOREVER
	{
	semTake (sc->sc_wrtSem, WAIT_FOREVER); /* sync w/ sloutput */

	FOREVER
	    {
	    /* get a packet off of the queue */

	    s = splimp ();
	    IF_DEQUEUE (&sc->sc_if.if_snd, m);
	    splx (s);

	    if (m == NULL)
		break;

	    /*
	     * Check if the serial line has been idle for awhile.
	     * If it has been idle for awhile, we send out a FRAME_END
	     * character to clear out any garbage in transit and start
	     * fresh with a new packet.  We find out whether or not
	     * the line has been idle for awhile by looking at the
	     * read side of the tty ring buffer queue.  It is assumed
	     * that if there is nothing to read from the queue, we
	     * must have been idle for awhile.  If our assumption
	     * is incorrect, we only have wasted the amount of work
	     * and bandwidth it takes to send a character over SLIP.
	     * More than likely, though, this is better than sending
	     * out a large packet and clobbering the whole packet
	     * because of the bad data.
	     */

	    (void) ioctl (fd, FIONREAD, (int) &rdCount);
	    if (rdCount == 0)
		{
		ch = FRAME_END;
		++sc->sc_if.if_obytes;
		(void) write (fd, &ch, 1);
		}

	    /*
	     * send the packet.
	     */
	    while (m)
		{
		cp  = mtod (m, u_char *);
		len = m->m_len;

		while (len > 0)
		    {
		    /*
		     * Find out how many bytes in the string we can
		     * handle without doing something special.
		     */

		    if ((numch = numCharsToMask (FRAME_ESCAPE, FRAME_END,
			(u_int) len, cp)) > 0)
			{
			/*
			 * Put numch characters at once
			 * into the tty output queue.
			 */
			if (write (fd, (char *)cp, numch) != numch)
			    break;

			sc->sc_if.if_obytes += numch;
			len -= numch;
			cp  += numch;
			}

		    /*
		     * If there are characters left in the mbuf,
		     * the first one must be special..
		     * Put it out in a different form.
		     */
		    if (len)
			{
			ch = FRAME_ESCAPE;

			if (write (fd, &ch, 1) != 1)
			    break;

			++sc->sc_if.if_obytes;

			ch = (*cp == FRAME_ESCAPE) ?
			     TRANS_FRAME_ESCAPE :
			     TRANS_FRAME_END;

			if (write (fd, &ch, 1) != 1)
			    break;

			++sc->sc_if.if_obytes;
			++cp;
			--len;
			}
		    }

		m2 = m_free (m); 
		m = m2;
		}

	   /*
	    * indicate the end-of-packet.
	    */

	    ch = FRAME_END;
	    if (write (fd, &ch, 1) != 1)
		{
		/*
		 * This shouldn't happen but if it does we try one
		 * more time (just for the hack of it) and increment
		 * the if_collisions and if_oerrors.
		 */
		ch = FRAME_END;
		(void) write (fd, &ch, 1);
		sc->sc_if.if_collisions++;
		sc->sc_if.if_oerrors++;
		}
	    else
		{
		++sc->sc_if.if_obytes;
		sc->sc_if.if_opackets++;
		}
	    }
	}
    }

/*******************************************************************************
*
* slintr - SLIP tty protocol hook routine
*
* Called at interrupt level by the tty driver input nterrupt routine.
* It is our "protocol hook" routine set by an FIOPROTOHOOK ioctl() call.
* slintr() examines each character coming in and keeps a count
* of input characters in the ring buffer of the tty driver until
* a full packet is indicated by FRAME_END.
* When detected, enqueue a call to slread() via netJobAdd() to
* be processed later at task level.
* A net job will not be added if the first character of the packet
* won't fit into the ring buffer.  In that case the entire packet
* must be discarded at interrupt level.
*
* This routine will return FALSE to tyIRd() to indicate
* that normal processing should continue within tyIRd() when
* this routine returns.
*
* RETURNS: FALSE so tyIRd continues processing
*/

LOCAL BOOL slintr
    (
    int unit,   /* SLIP unit number */
    int inchar  /* incoming character */
    )
    {
    FAST SL_SOFTC *sc = sl_softc [unit];
    TY_DEV * pTyDev;
    RING_ID ringId;

    /* Set discard flag if this is a new packet and ring buffer full */

    if ( sc->sc_qlen == 0 )
	{
        pTyDev = (TY_DEV *) iosFdDevFind(sc->sc_fd);
        ringId = pTyDev->rdBuf;
        if (rngIsFull (ringId))
	    sc->sc_flags |= SL_INTR_PACKET_DISCARD;
        else
	    sc->sc_flags &= ~SL_INTR_PACKET_DISCARD;
        }

    ++sc->sc_qlen;

    if ((inchar & 0xff) == FRAME_END || sc->sc_qlen == SLBUF_HI)
	{
	/* schedule a read for this packet */

        if (sc->sc_flags & SL_INTR_PACKET_DISCARD)
	    sc->sc_if.if_ierrors++;
        else
	    {
	    if (sc->sc_qlen == 1)	/* Single FRAME_END character */
		{
		sc->sc_qlen = 0;
		return (TRUE);          /* Don't put it in ring buffer */
		}
            else
	        (void) netJobAdd ((FUNCPTR) slread, (int) sc, sc->sc_qlen, 
				  0, 0, 0);
            }
	sc->sc_qlen = 0;
	}

    return (FALSE);	/* continue ty processing */
    }

/*******************************************************************************
*
* slread - read a whole SLIP packet and call slinput to process each char
*
* slread() is called only when FRAME_END is encountered when
* we have a SLIP packet waiting for us to read in the tty's
* input ring buffer.
*/

LOCAL void slread
    (
    FAST SL_SOFTC *sc,  /* SLIP interface */
    FAST int count      /* number of characters to be read */
    )
    {
    BOOL firstCluster = TRUE;
    char *builtCluster;
    char *nextBuf = sc->sc_buf;
    FAST u_char *c1 = (u_char *) sc->sc_buf;
    FAST u_char *c2 = c1;
    FAST u_char *end;
    int len;

    if ((len = read (sc->sc_fd, sc->sc_buf, count)) == 0)
	return;
    end = (u_char *) (sc->sc_buf + len);

    while (c1 < end)
	{
	++sc->sc_if.if_ibytes;	/* increment the recieved bytes */
        
	if (sc->sc_ilen > sc->sc_if.if_mtu)
            {
	    sc->sc_if.if_ierrors++;
	    break;
	    }

	switch (*c1)
	    {
	    case FRAME_END:
	        {
                if (sc->sc_ilen > 0)
		    {
	            if ((builtCluster = slRecv (sc, firstCluster)) != NULL)
		        {
		        nextBuf = builtCluster;
		        firstCluster = FALSE;
			}
                    sc->sc_ilen = 0;
		    }

	        sc->sc_buf = (char *) ++c1;
		c2 = c1;
		break;
	        }
	    case FRAME_ESCAPE:
		{
		c1++;
	        if (*c1 == TRANS_FRAME_ESCAPE)
	            *c1 = FRAME_ESCAPE;
	        else if (*c1 == TRANS_FRAME_END)
		    *c1 = FRAME_END;
	        else 
		    {
	            sc->sc_if.if_ierrors++;
                    sc->sc_ilen = 0;
	            sc->sc_buf = (char *) c1;
		    c2 = c1;
		    break;
		    }
		}
            default:
		{
		sc->sc_ilen++;
                *c2++ = *c1++;
		}
            }
	}

    sc->sc_buf = nextBuf;
    sc->sc_ilen = 0;
    }

/*******************************************************************************
*
* slRecv - process an input frame
*
* This routine processes an input frame, then passes it up to the higher
* level in a form it expects.  Buffer loaning, etherInputHookRtns, and
* CSLIP uncompression are all supported.  Trailer protocols are not supported.
*
* RETURNS: N/A
*/

LOCAL char *slRecv
    (
    FAST SL_SOFTC *sc,		/* SLIP interface */
    BOOL firstCluster
    )
    {
    FAST  struct mbuf *m = NULL;
    FAST  char type;
    char  *nextBuf = NULL;
    char  *origBuf = sc->sc_buf;
    UINT8 *origRef = sc->sc_pRefC[(sc->sc_nLoanRxBuf - 1)];

    type = *sc->sc_buf & 0xf0;
    if (type != (IPVERSION << 4))
        {
        if (type & 0x80)
	    type = TYPE_COMPRESSED_TCP;
        else if (type == TYPE_UNCOMPRESSED_TCP)
            *sc->sc_buf &= 0x4f;

        /*
         * we've got something that's not an IP packet.
         * If compression is enabled, try to uncompress
         * it.  Otherwise, if `auto-enable' compression
         * is on and it's a reasonable packet,
         * uncompress it then enable compression.
         * Otherwise, drop it.
         */

        if ((sc->sc_flags & SL_COMPRESS) || ((sc->sc_flags & SL_COMPRESS_RX) &&
             (type == TYPE_UNCOMPRESSED_TCP) && (sc->sc_ilen >= 40)))
	    {
            if ((sc->sc_ilen = sl_uncompress_tcp ((u_char **)&sc->sc_buf, 
						  sc->sc_ilen, (u_int) type,
						  &sc->sc_comp)) <= 0)
                {
	        sc->sc_if.if_ierrors++;
	        return (NULL);
                }

            sc->sc_flags |= SL_COMPRESS;
            }
        else
            {
	    sc->sc_if.if_ierrors++;
	    return (NULL);
            }
        }

    if ((etherInputHookRtn != NULL) && ((*etherInputHookRtn)
        (&sc->sc_if, sc->sc_buf, sc->sc_ilen) != 0))
        return (NULL);

    if ((firstCluster) && (sc->sc_nLoanRxBuf > 0) &&
	(USE_CLUSTER (sc->sc_ilen)))
        m = build_cluster (sc->sc_buf, sc->sc_ilen, &sc->sc_if,
            MC_SL, sc->sc_pRefC[(sc->sc_nLoanRxBuf - 1)], 
	    (FUNCPTR) slLoanFree, (int) sc, (int) origBuf, (int) origRef);

    /* if buffer was successfully turned into mbuf cluster */
 
    if (m != NULL)
        nextBuf = sc->sc_lPool[--sc->sc_nLoanRxBuf];
    else
        {
        /* else do same ol' copy to mbuf */

        m = copy_to_mbufs (sc->sc_buf, sc->sc_ilen, 0, &sc->sc_if);
	if (m == NULL)
	    {
	    sc->sc_if.if_ierrors++;
	    return (NULL);
            }
        }

    /* send up to protocol */

    sc->sc_if.if_ipackets++;
    sc->sc_if.if_lastchange = tickGet(); 

    ipintr (m);

    return (nextBuf);
    }

/*******************************************************************************
*
* slioctl - SLIP ioctl routine
*
* Process an ioctl request.
*
* RETURNS: 0 or errno
*/

LOCAL int slioctl
    (
    FAST struct ifnet *ifp,     /* pointer to SLIP interface */
    int cmd,                    /* command */
    caddr_t data                /* data */
    )
    {
    FAST struct ifaddr *	ifa = (struct ifaddr *)data;
    FAST struct ifreq *		ifr;
    int				error = 0;
    int 			s;

    s = splimp ();

    switch (cmd)
        {
        case SIOCGIFFLAGS:
            *(short *)data = ifp->if_flags;
            break;

        case SIOCSIFADDR:
	    if (ifa->ifa_addr->sa_family == AF_INET)
                {
		slinit (ifp->if_unit);
                ifa->ifa_rtrequest = slRtRequest;
                }
	    else
		error = EAFNOSUPPORT;
	    break;

	case SIOCSIFDSTADDR:
            if (ifa->ifa_dstaddr->sa_family != AF_INET)
		error = EAFNOSUPPORT;
   	    break;

	case SIOCADDMULTI:
	case SIOCDELMULTI:
	    ifr = (struct ifreq *)data;
	    if (ifr == 0) 
		{
		error = EAFNOSUPPORT;		/* XXX */
		break;
		}
	    switch (ifr->ifr_addr.sa_family)
		{
#ifdef INET
		case AF_INET:
		    break;
#endif
		default:
		    error = EAFNOSUPPORT;
		    break;
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
* numCharsToMask - scan the buffer for a mask
*
* numcharsToMask finds out how many characters are in a string
* before a character that matches the 'mask'.
*
* RETURNS: number of chars up to the mask
*/

LOCAL int numCharsToMask
    (
    FAST u_char mask1,
    FAST u_char mask2,
    u_int size,
    FAST u_char *cp
    )
    {
    FAST u_char *end = &cp [size];
    u_char *start = cp;

    while ((cp < end) && (*cp != mask1) && (*cp != mask2))
    	cp++;

    return (cp - start);
    }

/********************************************************************************
* slLoanFree - return the given buffer to loaner pool
*
* This routine returns <pRxBuf> to the pool of available loaner buffers.
* It also returns <pRef> to the pool of available loaner reference counters,
* then zeroes the reference count.
*
* RETURNS: N/A
*/
 
LOCAL void slLoanFree
    (
    SL_SOFTC *sc,
    char *pRxBuf,
    UINT8 *pRef
    )
    {
    /* return loaned buffer to pool */

    sc->sc_lPool[sc->sc_nLoanRxBuf] = pRxBuf;
 
    /* return loaned reference count to pool */
 
    sc->sc_pRefC[sc->sc_nLoanRxBuf++] = pRef;
 
    /* reset reference count - should have been done from above, but... */
 
    *pRef = 0;
    }

/*******************************************************************************
*
* slRtRequest - perform special routing.
*
* This function performs special processing when adding a route to itself
* on a slip interface. It intializes the interface pointer to the loop back
* so that the packet can be rerouted through the loopback instead of actually
* sending the packet out on the wire and have the other end loop back the
* packet. The capability can be turned off by intializing the global flag
* slipLoopBack to 0. By default it is turned on. 
*
* NOMANUAL
*
* RETURNS: N/A
*/

static void slRtRequest
    (
    int 		cmd,		/* route command */
    struct rtentry * 	pRtEntry,	/* pointer to the route entry */
    struct sockaddr * 	pSockAddr	/* pointer to the sock addr */
    )
    {
    /* only when adding a route to itself */

    if (cmd == RTM_ADD)
        {
        if (slipLoopBack && (pRtEntry->rt_flags & RTF_HOST) &&
            (SIN(rt_key(pRtEntry))->sin_addr.s_addr ==
             (IA_SIN(pRtEntry->rt_ifa))->sin_addr.s_addr))
            {
            pRtEntry->rt_rmx.rmx_expire = 0;
            pRtEntry->rt_ifp = &loif;	/* initialize to the loop back */ 
            }
        }
    }
