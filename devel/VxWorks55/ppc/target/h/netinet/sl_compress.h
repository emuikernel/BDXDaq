/* sl_compress.h - Definitions for tcp compression routines */

/*	$NetBSD: slcompress.h,v 1.8 1994/06/29 06:36:51 cgd Exp $	*/

/*
 * Copyright (c) 1989, 1993
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
 *	@(#)slcompress.h	8.1 (Berkeley) 6/10/93
 */

/*
 * Definitions for tcp compression routines.
 *
 * Van Jacobson (van@helios.ee.lbl.gov), Dec 31, 1989:
 *	- Initial distribution.
 */

/*
modification history
--------------------
01d,17nov96,bin  removed MLEN replaced with CL_SIZE_128
01c,01aug96,vin	 integrated with BSD4.4, moved to netinet made universal
		 for CSLIP as well as PPP.
01b,16jun95,dzb  added necessary header file inclusions.
01a,16jan95,dzb  WRS-ize.  added prototype of ppp_sl_uncompress_tcp_part().
                 prepended "ppp_" due to conflict with CSLIP's slcompress.
*/

#ifndef	__INCsl_compressh
#define	__INCsl_compressh

#ifdef  __cplusplus
extern "C" {
#endif

/* includes */

#include "netinet/in_systm.h"
#include "net/mbuf.h"
#include "netinet/ip.h"

#define SL_STATS		/* define to include statistics counters */

#define MAX_STATES 16           /* must be > 2 and < 256 */
#define MAX_HDR    CL_SIZE_128	

/*
 * Compressed packet format:
 *
 * The first octet contains the packet type (top 3 bits), TCP
 * 'push' bit, and flags that indicate which of the 4 TCP sequence
 * numbers have changed (bottom 5 bits).  The next octet is a
 * conversation number that associates a saved IP/TCP header with
 * the compressed packet.  The next two octets are the TCP checksum
 * from the original datagram.  The next 0 to 15 octets are
 * sequence number changes, one change per bit set in the header
 * (there may be no changes and there are two special cases where
 * the receiver implicitly knows what changed -- see below).
 * 
 * There are 5 numbers which can change (they are always inserted
 * in the following order): TCP urgent pointer, window,
 * acknowlegement, sequence number and IP ID.  (The urgent pointer
 * is different from the others in that its value is sent, not the
 * change in value.)  Since typical use of SLIP links is biased
 * toward small packets (see comments on MTU/MSS below), changes
 * use a variable length coding with one octet for numbers in the
 * range 1 - 255 and 3 octets (0, MSB, LSB) for numbers in the
 * range 256 - 65535 or 0.  (If the change in sequence number or
 * ack is more than 65535, an uncompressed packet is sent.)
 */

/*
 * Packet types (must not conflict with IP protocol version)
 *
 * The top nibble of the first octet is the packet type.  There are
 * three possible types: IP (not proto TCP or tcp with one of the
 * control flags set); uncompressed TCP (a normal IP/TCP packet but
 * with the 8-bit protocol field replaced by an 8-bit connection id --
 * this type of packet syncs the sender & receiver); and compressed
 * TCP (described above).
 *
 * LSB of 4-bit field is TCP "PUSH" bit (a worthless anachronism) and
 * is logically part of the 4-bit "changes" field that follows.  Top
 * three bits are actual packet type.  For backward compatibility
 * and in the interest of conserving bits, numbers are chosen so the
 * IP protocol version number (4) which normally appears in this nibble
 * means "IP packet".
 */

/* packet types */
#define TYPE_IP 0x40
#define TYPE_UNCOMPRESSED_TCP 0x70
#define TYPE_COMPRESSED_TCP 0x80
#define TYPE_ERROR 0x00

/* Bits in first octet of compressed packet */
#define NEW_C	0x40	/* flag bits for what changed in a packet */
#define NEW_I	0x20
#define NEW_S	0x08
#define NEW_A	0x04
#define NEW_W	0x02
#define NEW_U	0x01

/* reserved, special-case values of above */
#define SPECIAL_I (NEW_S|NEW_W|NEW_U)		/* echoed interactive traffic */
#define SPECIAL_D (NEW_S|NEW_A|NEW_W|NEW_U)	/* unidirectional data */
#define SPECIALS_MASK (NEW_S|NEW_A|NEW_W|NEW_U)

#define TCP_PUSH_BIT 0x10

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif  /* CPU_FAMILY==I960 */

/*
 * "state" data for each active tcp conversation on the wire.  This is
 * basically a copy of the entire IP/TCP header from the last packet
 * we saw from the conversation together with a small identifier
 * the transmit & receive ends of the line use to locate saved header.
 */
struct cstate {
	struct cstate *cs_next;	/* next most recently used cstate (xmit only) */
	u_short cs_hlen;	/* size of hdr (receive only) */
	u_char cs_id;		/* connection # associated with this state */
	u_char cs_filler;
	union {
		char csu_hdr[MAX_HDR];
		struct ip csu_ip;	/* ip/tcp hdr from most recent packet */
	} slcs_u;
};
#define cs_ip slcs_u.csu_ip
#define cs_hdr slcs_u.csu_hdr

/*
 * all the state data for one serial line (we need one of these
 * per line).
 */
struct slcompress {
	struct cstate *last_cs;	/* most recently used tstate */
	u_char last_recv;	/* last rcvd conn. id */
	u_char last_xmit;	/* last sent conn. id */
	u_short flags;
#ifdef SL_STATS
	u_int sls_packets;	/* outbound packets */
	u_int sls_compressed;	/* outbound compressed packets */
	u_int sls_searches;	/* searches for connection state */
	u_int sls_misses;	/* times couldn't find conn. state */
	u_int sls_uncompressedin;	/* inbound uncompressed packets */
	u_int sls_compressedin;	/* inbound compressed packets */
	u_int sls_errorin;	/* inbound unknown type packets */
	u_int sls_tossed;	/* inbound packets tossed because of error */
#endif
	struct cstate tstate[MAX_STATES];	/* xmit connection states */
	struct cstate rstate[MAX_STATES];	/* receive connection states */
};

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 0                 /* turn off alignment requirement */
#endif  /* CPU_FAMILY==I960 */

/* flag values */
#define SLF_TOSS 1		/* tossing rcvd frames because of input err */

#ifdef SL_STATS
#define INCR(counter) ++comp->counter;
#else
#define INCR(counter)
#endif

#define BCMP(p1, p2, n) bcmp((char *)(p1), (char *)(p2), (int)(n))

#ifdef BCOPY
#undef BCOPY
#endif /* BCOPY */
#define BCOPY(s, d, l)	bcopy((char *) s, (char *) d, (int) l)
#ifndef KERNEL
#define ovbcopy bcopy
#endif

/* ENCODE encodes a number that is known to be non-zero.  ENCODEZ
 * checks for zero (since zero has to be encoded in the long, 3 byte
 * form).
 */
#define ENCODE(n) { \
	if ((u_short)(n) >= 256) { \
		*cp++ = 0; \
		cp[1] = (n); \
		cp[0] = (n) >> 8; \
		cp += 2; \
	} else { \
		*cp++ = (n); \
	} \
}
#define ENCODEZ(n) { \
	if ((u_short)(n) >= 256 || (u_short)(n) == 0) { \
		*cp++ = 0; \
		cp[1] = (n); \
		cp[0] = (n) >> 8; \
		cp += 2; \
	} else { \
		*cp++ = (n); \
	} \
}

#define DECODEL(f) { \
	if (*cp == 0) {\
		(f) = htonl(ntohl(f) + ((cp[1] << 8) | cp[2])); \
		cp += 3; \
	} else { \
		(f) = htonl(ntohl(f) + (u_long)*cp); \
		cp++; \
	} \
}

#define DECODES(f) { \
	if (*cp == 0) {\
		(f) = htons(ntohs(f) + ((cp[1] << 8) | cp[2])); \
		cp += 3; \
	} else { \
		(f) = htons(ntohs(f) + (u_long)*cp); \
		cp++; \
	} \
}

#define DECODEU(f) { \
	if (*cp == 0) {\
		(f) = htons((cp[1] << 8) | cp[2]); \
		cp += 3; \
	} else { \
		(f) = htons((u_long)*cp); \
		cp++; \
	} \
}

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern void	sl_compress_init (struct slcompress *comp);
extern u_int	sl_compress_tcp (struct mbuf *m, struct ip *ip,
		    struct slcompress *comp, int compress_cid);
extern int	sl_uncompress_tcp (u_char** bufp, int len, u_int type,
		    struct slcompress *comp);
#else	/* __STDC__ */

extern void	ppp_sl_compress_init ();
extern void	ppp_sl_compress_setup ();
extern u_char	ppp_sl_compress_tcp  ();
extern int	ppp_sl_uncompress_tcp ();
extern int	ppp_sl_uncompress_tcp_part ();

#endif	/* __STDC__ */

#ifdef  __cplusplus
}
#endif

#endif /* __INCsl_compressh */
