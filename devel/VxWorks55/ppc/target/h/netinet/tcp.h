/* tcp.h - tcp header file */

/* Copyright 1984-1996 Wind River Systems, Inc. */

/*
 * Copyright (c) 1982, 1986, 1993
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
 *	@(#)tcp.h	8.1 (Berkeley) 6/10/93
 */

/*
modification history
--------------------
01a,03mar96,vin  created from BSD4.4 stuff. integrated with 02k of tcp.h
		 Corrected bit fields according to ansi spec.
*/

#ifndef __INCtcph
#define __INCtcph

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _BYTE_ORDER
/*
 * Definitions for byte order,
 * according to byte significance from low address to high.
 */
#define _LITTLE_ENDIAN   1234    /* least-significant byte first (vax) */
#define _BIG_ENDIAN      4321    /* most-significant byte first (IBM, net) */
#define _PDP_ENDIAN      3412    /* LSB first in word, MSW first in long (pdp) */
#ifdef vax
#define _BYTE_ORDER      _LITTLE_ENDIAN
#else
#define _BYTE_ORDER      _BIG_ENDIAN      /* mc68000, tahoe, most others */
#endif
#endif	/* _BYTE_ORDER */

typedef	u_long	tcp_seq;

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif	/* CPU_FAMILY==I960 */

/*
 * TCP header.
 * Per RFC 793, September, 1981.
 */
struct tcphdr {
	u_short	th_sport;		/* source port */
	u_short	th_dport;		/* destination port */
	tcp_seq	th_seq;			/* sequence number */
	tcp_seq	th_ack;			/* acknowledgement number */
#if _BYTE_ORDER == _LITTLE_ENDIAN
	u_int	th_x2:4,		/* (unused) */
		th_off:4,		/* data offset */
#endif
#if _BYTE_ORDER == _BIG_ENDIAN
	u_int	th_off:4,		/* data offset */
		th_x2:4,		/* (unused) */
#endif
		th_flags:8,
#define	TH_FIN	0x01
#define	TH_SYN	0x02
#define	TH_RST	0x04
#define	TH_PUSH	0x08
#define	TH_ACK	0x10
#define	TH_URG	0x20
		th_win:16;		/* window */
	u_short	th_sum;			/* checksum */
	u_short	th_urp;			/* urgent pointer */
};

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 0                 /* turn off alignment requirement */
#endif	/* CPU_FAMILY==I960 */

#define	TCPOPT_EOL			0
#define	TCPOPT_NOP			1
#define	TCPOPT_MAXSEG			2
#define TCPOLEN_MAXSEG			4
#define TCPOPT_WINDOW			3
#define TCPOLEN_WINDOW			3
#define TCPOPT_SACK_PERMITTED		4		/* Experimental */
#define TCPOLEN_SACK_PERMITTED		2
#define TCPOPT_SACK			5		/* Experimental */
#define TCPOPT_TIMESTAMP		8
#define TCPOLEN_TIMESTAMP		10
#define TCPOLEN_TSTAMP_APPA		(TCPOLEN_TIMESTAMP+2) /* appendix A */

#define TCPOPT_TSTAMP_HDR	\
    (TCPOPT_NOP<<24|TCPOPT_NOP<<16|TCPOPT_TIMESTAMP<<8|TCPOLEN_TIMESTAMP)

/*
 * Default maximum segment size for TCP.
 * With an IP MSS of 576, this is 536,
 * but 512 is probably more convenient.
 * This should be defined as MIN(512, IP_MSS - sizeof (struct tcpiphdr)).
 */
#define	TCP_MSS		512

#define	TCP_MAXWIN	65535	/* largest value for (unscaled) window */

#define	TCP_MAX_WINSHIFT	14	/* maximum window shift */

/*
 * User-settable options (used with setsockopt).
 */
#define	TCP_NODELAY	0x01	/* don't delay send to coalesce packets */
#define	TCP_MAXSEG	0x02	/* set maximum segment size */

#ifdef __cplusplus
}
#endif

#endif /* __INCtcph */
