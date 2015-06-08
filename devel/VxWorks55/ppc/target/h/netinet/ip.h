/* ip.h - internet protocol header file */

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
 *	@(#)ip.h	8.2 (Berkeley) 6/1/94
 */

/*
modification history
--------------------
02o,31aug00,rae  added Router Alert Option (IPOPT_RA)
02n,15jul97,spm  made IP checksum calculation configurable (SPR #7836)
02m,24feb97,bjl  moved struct ipt_ta declatation outside of union 
		 ipt_timestamp to make struct ipt_ta globally visible for c++
01b,11apr97,rjc  added min cost tos.
01a,03mar96,vin  created from BSD4.4 stuff,integrated with 02l.
		 Corrected bitfields according to the ansi spec.
*/

#ifndef __INCiph
#define __INCiph

#ifdef __cplusplus
extern "C" {
#endif

#include "vxWorks.h"

/* Definitions for selecting IP checksum calculations (also see netLib.h). */

#define IP_CKSUM_SEND_MASK      0x1
#define IP_CKSUM_RECV_MASK      0x2

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

/*
 * Definitions for internet protocol version 4.
 * Per RFC 791, September 1981.
 */
#define	IPVERSION	4

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif  /* CPU_FAMILY==I960 */

/*
 * Structure of an internet header, naked of options.
 *
 * We declare ip_len and ip_off to be short, rather than u_short
 * pragmatically since otherwise unsigned comparisons can result
 * against negative integers quite easily, and fail in subtle ways.
 */
struct ip {
#if _BYTE_ORDER == _LITTLE_ENDIAN
	u_int	ip_hl:4,		/* header length */
		ip_v:4,			/* version */
#endif
#if _BYTE_ORDER == _BIG_ENDIAN
	u_int	ip_v:4,			/* version */
		ip_hl:4,		/* header length */
#endif
		ip_tos:8,		/* type of service */
		ip_len:16;		/* total length */
	u_short	ip_id;			/* identification */
	short	ip_off;			/* fragment offset field */
#define	IP_DF 0x4000			/* dont fragment flag */
#define	IP_MF 0x2000			/* more fragments flag */
#define	IP_OFFMASK 0x1fff		/* mask for fragmenting bits */
	u_char	ip_ttl;			/* time to live */
	u_char	ip_p;			/* protocol */
	u_short	ip_sum;			/* checksum */
	struct	in_addr ip_src,ip_dst;	/* source and dest address */
};

#define	IP_MAXPACKET	65535		/* maximum packet size */

/*
 * Definitions for IP type of service (ip_tos)
 */
#define	IPTOS_LOWDELAY		0x10
#define	IPTOS_THROUGHPUT	0x08
#define	IPTOS_RELIABILITY	0x04
#define	IPTOS_MINCOST		0x02

/*
 * Definitions for IP precedence (also in ip_tos) (hopefully unused)
 */
#define	IPTOS_PREC_NETCONTROL		0xe0
#define	IPTOS_PREC_INTERNETCONTROL	0xc0
#define	IPTOS_PREC_CRITIC_ECP		0xa0
#define	IPTOS_PREC_FLASHOVERRIDE	0x80
#define	IPTOS_PREC_FLASH		0x60
#define	IPTOS_PREC_IMMEDIATE		0x40
#define	IPTOS_PREC_PRIORITY		0x20
#define	IPTOS_PREC_ROUTINE		0x00

/*
 * Definitions for options.
 */
#define	IPOPT_COPIED(o)		((o)&0x80)
#define	IPOPT_CLASS(o)		((o)&0x60)
#define	IPOPT_NUMBER(o)		((o)&0x1f)

#define	IPOPT_CONTROL		0x00
#define	IPOPT_RESERVED1		0x20
#define	IPOPT_DEBMEAS		0x40
#define	IPOPT_RESERVED2		0x60

#define	IPOPT_EOL		0		/* end of option list */
#define	IPOPT_NOP		1		/* no operation */

#define	IPOPT_RR		7		/* record packet route */
#define	IPOPT_TS		68		/* timestamp */
#define	IPOPT_SECURITY		130		/* provide s,c,h,tcc */
#define	IPOPT_LSRR		131		/* loose source route */
#define	IPOPT_SATID		136		/* satnet id */
#define	IPOPT_SSRR		137		/* strict source route */
#define IPOPT_RA		148		/* router alert */

/*
 * Offsets to fields in options other than EOL and NOP.
 */
#define	IPOPT_OPTVAL		0		/* option ID */
#define	IPOPT_OLEN		1		/* option length */
#define IPOPT_OFFSET		2		/* offset within option */
#define	IPOPT_MINOFF		4		/* min value of above */


struct	ipt_ta {
	struct in_addr ipt_addr;
	u_long ipt_time;
};

/*
 * Time stamp option structure.
 */
struct	ip_timestamp {
        u_int	ipt_code:8,		/* IPOPT_TS */
		ipt_len:8,		/* size of structure (variable) */
		ipt_ptr:8,		/* index of current entry */
#if _BYTE_ORDER == _LITTLE_ENDIAN
		ipt_flg:4,		/* flags, see below */
		ipt_oflw:4;		/* overflow counter */
#endif
#if _BYTE_ORDER == _BIG_ENDIAN
		ipt_oflw:4,		/* overflow counter */
		ipt_flg:4;		/* flags, see below */
#endif
	union ipt_timestamp {
		u_long	ipt_time[1];
		struct	ipt_ta ipt_ta[1];
	} ipt_timestamp;
};

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 0                 /* turn off alignment requirement */
#endif  /* CPU_FAMILY==I960 */

/* flag bits for ipt_flg */
#define	IPOPT_TS_TSONLY		0		/* timestamps only */
#define	IPOPT_TS_TSANDADDR	1		/* timestamps and addresses */
#define	IPOPT_TS_PRESPEC	3		/* specified modules only */

/* bits for security (not byte swapped) */
#define	IPOPT_SECUR_UNCLASS	0x0000
#define	IPOPT_SECUR_CONFID	0xf135
#define	IPOPT_SECUR_EFTO	0x789a
#define	IPOPT_SECUR_MMMM	0xbc4d
#define	IPOPT_SECUR_RESTR	0xaf13
#define	IPOPT_SECUR_SECRET	0xd788
#define	IPOPT_SECUR_TOPSECRET	0x6bc5

/*
 * Internet implementation parameters.
 */
#define	MAXTTL		255		/* maximum time to live (seconds) */
#define	IPDEFTTL	64		/* default ttl, from RFC 1340 */
#define	IPFRAGTTL	60		/* time to live for frags, slowhz */
#define	IPTTLDEC	1		/* subtracted when forwarding */

#define	IP_MSS		576		/* default maximum segment size */

#define IPFORWARDING    1		/* Set IP to always forward */

#ifdef __cplusplus
}
#endif

#endif /* __INCiph */
