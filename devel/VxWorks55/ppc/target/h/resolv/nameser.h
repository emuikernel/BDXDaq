/* nameser.h - Domain Name Server protocol header file */

/* Copyright 1984-1996 Wind River Systems, Inc. */


/*	$NetBSD: nameser.h,v 1.7 1994/10/26 00:56:45 cgd Exp $	*/

/*
 * Copyright (c) 1983, 1989 Regents of the University of California.
 * All rights reserved.
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
 *	@(#)nameser.h	5.25 (Berkeley) 4/3/91
 */

/*
modification history
--------------------
01b,29apr97,jag Changed T_NULL to T_NULL_RR to fix conflict with loadCoffLib.h
01b,02apr97,jag Change types from u_int*_t to uint*_t.
01a,11dec96,jag created from BSD4.4. Corrected bitfields to meet ANSI spec.
                changed STATUS to DNS_STATUS.
*/

#ifndef __INCnameserh
#define	__INCnameserh

#ifdef __cplusplus
extern "C" {
#endif

#define BYTE_ORDER      _BYTE_ORDER
#define LITTLE_ENDIAN   _LITTLE_ENDIAN
#define BIG_ENDIAN      _BIG_ENDIAN


/*
 * Define constants based on rfc883
 */
#define PACKETSZ	512		/* maximum packet size */
#define MAXDNAME	256		/* maximum domain name */
#define MAXCDNAME	255		/* maximum compressed domain name */
#define MAXLABEL	63		/* maximum length of domain label */
	/* Number of bytes of fixed size data in query structure */
#define QFIXEDSZ	4
	/* number of bytes of fixed size data in resource record */
#define RRFIXEDSZ	10

/*
 * Internet nameserver port number
 */
#define NAMESERVER_PORT	53

/*
 * Currently defined opcodes
 */
#define QUERY		0x0		/* standard query */
#define IQUERY		0x1		/* inverse query */
#define DNS_STATUS	0x2		         /* nameserver status query*/
/* #define xxx		0x3		   0x3 reserved */

	/* non standard */
#define UPDATEA		0x9		/* add resource record */
#define UPDATED		0xa		/* delete a specific resource record */
#define UPDATEDA	0xb		/* delete all named resource record */
#define UPDATEM		0xc		/* modify a specific resource record */
#define UPDATEMA	0xd		/* modify all named resource record */

#define ZONEINIT	0xe		/* initial zone transfer */
#define ZONEREF		0xf		/* incremental zone referesh */

/*
 * Currently defined response codes
 */
#define NOERROR		0		/* no error */
#define FORMERR		1		/* format error */
#define SERVFAIL	2		/* server failure */
#define NXDOMAIN	3		/* non existent domain */
#define NOTIMP		4		/* not implemented */
#define REFUSED		5		/* query refused */
	/* non standard */
#define NOCHANGE	0xf		/* update failed to change db */

/*
 * Type values for resources and queries
 */
#define T_A		1		/* host address */
#define T_NS		2		/* authoritative server */
#define T_MD		3		/* mail destination */
#define T_MF		4		/* mail forwarder */
#define T_CNAME		5		/* connonical name */
#define T_SOA		6		/* start of authority zone */
#define T_MB		7		/* mailbox domain name */
#define T_MG		8		/* mail group member */
#define T_MR		9		/* mail rename name */
#define T_NULL_RR	10		/* null resource record */
#define T_WKS		11		/* well known service */
#define T_PTR		12		/* domain name pointer */
#define T_HINFO		13		/* host information */
#define T_MINFO		14		/* mailbox information */
#define T_MX		15		/* mail routing information */
#define T_TXT		16		/* text strings */
#define T_RP		17		/* responsible person */
#define T_AFSDB		18		/* AFS cell database */
#define T_NSAP		22		/* NSAP address */
#define T_NSAP_PTR	23		/* reverse lookup for NSAP */
	/* non standard */
#define T_UINFO		100		/* user (finger) information */
#define T_UID		101		/* user ID */
#define T_GID		102		/* group ID */
#define T_UNSPEC	103		/* Unspecified format (binary data) */
#define T_SA		200		/* shuffle address */
	/* Query type values which do not appear in resource records */
#define T_AXFR		252		/* transfer zone of authority */
#define T_MAILB		253		/* transfer mailbox records */
#define T_MAILA		254		/* transfer mail agent records */
#define T_ANY		255		/* wildcard match */

/*
 * Values for class field
 */
#define C_IN		1		/* the arpa internet */
#define C_CHAOS		3		/* for chaos net at MIT */
#define C_HS		4		/* for Hesiod name server at MIT */
	/* Query class values which do not appear in resource records */
#define C_ANY		255		/* wildcard match */

/*
 * Status return codes for T_UNSPEC conversion routines
 */
#define CONV_SUCCESS     0
#define CONV_OVERFLOW   -1
#define CONV_BADFMT     -2
#define CONV_BADCKSUM   -3
#define CONV_BADBUFLEN -4

/*
 * Structure for query header, the order of the fields is machine and
 * compiler dependent, in our case, the bits within a byte are assignd 
 * least significant first, while the order of transmition is most 
 * significant first.  This requires a somewhat confusing rearrangement.
 */

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif  /* CPU_FAMILY==I960 */

typedef struct 
    {
	u_int	id:16,	/* fields (Bytes 1-2) query identification number */
#if BYTE_ORDER == BIG_ENDIAN
			/* fields in third byte */
	      	qr:1,		/* response flag */
	      	opcode:4,	/* purpose of message */
	      	aa:1,		/* authoritive answer */
	      	tc:1,		/* truncated message */
	      	rd:1,		/* recursion desired */
			/* fields in fourth byte */
	      	ra:1,		/* recursion available */
	      	pr:1,		/* primary server required (non standard) */
	      	unused:2,	/* unused bits */
	      	rcode:4;	/* response code */
#endif
#if BYTE_ORDER == LITTLE_ENDIAN || BYTE_ORDER == PDP_ENDIAN
			/* fields in third byte */
	      	rd:1,		/* recursion desired */
	      	tc:1,		/* truncated message */
	      	aa:1,		/* authoritive answer */
	      	opcode:4,	/* purpose of message */
	      	qr:1,		/* response flag */
			/* fields in fourth byte */
	      	rcode:4,	/* response code */
	      	unused:2,	/* unused bits */
	      	pr:1,		/* primary server required (non standard) */
	      	ra:1;		/* recursion available */
#endif
			/* remaining bytes */
	u_short	qdcount;	/* number of question entries */
	u_short	ancount;	/* number of answer entries */
	u_short	nscount;	/* number of authority entries */
	u_short	arcount;	/* number of resource entries */

    } HEADER;

/*
 * Defines for handling compressed domain names
 */
#define INDIR_MASK	0xc0

/*
 * Structure for passing resource records around.
 */
struct rrec {
	int16_t		r_zone;			/* zone number */
	int16_t		r_class;		/* class number */
	int16_t		r_type;			/* type number */
	uint32_t	r_ttl;			/* time to live */
	int		r_size;			/* size of data area */
	char		*r_data;		/* pointer to data */
};

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 0                 /* turn off alignment requirement */
#endif  /* CPU_FAMILY==I960 */

extern	uint16_t	_getshort();
extern	uint32_t	_getlong();

/*
 * Inline versions of get/put short/long.
 * Pointer is advanced; we assume that both arguments
 * are lvalues and will already be in registers.
 * cp MUST be u_char *.
 */
#define GETSHORT(s, cp) { \
	register u_char *t_cp = (u_char *)(cp); \
	(s) = ((uint16_t)t_cp[0] << 8) \
	    | ((uint16_t)t_cp[1]) ;\
	(cp) += sizeof(uint16_t); \
}

#define GETLONG(l, cp) { \
	register u_char *t_cp = (u_char *)(cp); \
	(l) = ((uint32_t)t_cp[0] << 24) \
	    | ((uint32_t)t_cp[1] << 16) \
	    | ((uint32_t)t_cp[2] << 8) \
	    | ((uint32_t)t_cp[3]) ;\
	(cp) += sizeof(uint32_t); \
}

#define PUTSHORT(s, cp) { \
	register uint16_t t_s = (uint16_t)(s); \
	register u_char *t_cp = (u_char *)(cp); \
	*t_cp++ = t_s >> 8; \
	*t_cp   = t_s; \
	(cp) += sizeof(uint16_t); \
}

/*
 * Warning: PUTLONG --no-longer-- destroys its first argument.  if you
 * were depending on this "feature", you will lose.
 */
#define PUTLONG(l, cp) { \
	register uint32_t t_l = (uint32_t)(l); \
	register u_char *t_cp = (u_char *)(cp); \
	*t_cp++ = t_l >> 24; \
	*t_cp++ = t_l >> 16; \
	*t_cp++ = t_l >> 8; \
	*t_cp   = t_l; \
	(cp) += sizeof(uint32_t); \
}

#ifdef __cplusplus
}
#endif

#endif /* __INCnameserh */
