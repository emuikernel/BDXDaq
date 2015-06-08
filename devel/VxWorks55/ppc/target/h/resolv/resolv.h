/* resolv.h - resolver internal data structures header file */

/* Copyright 1984-2001 Wind River Systems, Inc. */


/*	$NetBSD: resolv.h,v 1.8 1994/10/26 00:56:16 cgd Exp $	*/

/*
 * Copyright (c) 1983, 1987, 1989 The Regents of the University of California.
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
 *	@(#)resolv.h	5.15 (Berkeley) 4/3/91
 */

/*
modification history
--------------------
01c,24sep01,gls  removed definition of __P() macro (SPR #28330)
01b,02apr97,jag  Change types from u_int*_t to uint*_t. Added C++ and function
		 prototypes.
01a,11dec96,jag  created from BSD4.4 release. Deleted constants that are not
		 used by vxWorks.  Changed bit fields to meet ANSI specs.
*/


#ifndef __INCresolvh
#define	__INCresolvh

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * revision information.  this is the release date in YYYYMMDD format.
 * it can change every day so the right thing to do with it is use it
 * in preprocessor commands such as "#if (__RES > 19931104)".  do not
 * compare for equality; rather, use it to determine whether your resolver
 * is new enough to contain a certain feature.
 */

#define	__RES		19931104


/*
 * Global defines and variables for resolver stub.
 */
#define	MAXNS			3	/* max # name servers we'll track */
#define	MAXDFLSRCH		3	/* # default domain levels to try */
#define	MAXDNSRCH		6	/* max # domains in search path */
#define	LOCALDOMAINPARTS	2	/* min levels in name that is "local" */
#define	MAXDNSLUS		4	/* max # of host lookup types */

#define	RES_TIMEOUT		5	/* min. seconds between retries */
#define MAXRESOLVSORT		10	/* number of net to sort on */
#define RES_MAXNDOTS		15	/* should reflect bit field size */

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif  /* CPU_FAMILY==I960 */

struct __res_state {
	int	retrans;	 	/* retransmition time interval */
	int	retry;			/* number of times to retransmit */
	long	options;		/* option flags - see below. */
	int	nscount;		/* number of name servers */
	struct	sockaddr_in nsaddr_list[MAXNS];	/* address of name server */
#define	nsaddr	nsaddr_list[0]		/* for backward compatibility */
	u_short	id;			/* current packet id */
	char	*dnsrch[MAXDNSRCH+1];	/* components of domain to search */
	char	defdname[MAXDNAME];	/* default domain */
	long	pfcode;			/* RES_PRF_ flags - see below. */
	u_int	ndots:4,		/* threshold for initial abs. query */
		nsort:4,		/* number of elements in sort_list[] */
		:0;			/* ANSI padding field */
	struct {
		struct in_addr addr;
		uint32_t mask;
	} sort_list[MAXRESOLVSORT];
	char	lookups[MAXDNSLUS];
};

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 0                 /* turn off alignment requirement */
#endif  /* CPU_FAMILY==I960 */

/*
 * Resolver options
 */
#define RES_INIT	0x0001		/* address initialized */
#define RES_DEBUG	0x0002		/* print debug messages */
#define RES_AAONLY	0x0004		/* authoritative answers only */
#define RES_USEVC	0x0008		/* use virtual circuit */
#define RES_PRIMARY	0x0010		/* query primary server only */
#define RES_IGNTC	0x0020		/* ignore trucation errors */
#define RES_RECURSE	0x0040		/* recursion desired */
#define RES_DEFNAMES	0x0080		/* use default domain name */
#define RES_STAYOPEN	0x0100		/* Keep TCP socket open */
#define RES_DNSRCH	0x0200		/* search up local domain tree */

#define RES_DEFAULT	(RES_RECURSE | RES_DEFNAMES | RES_DNSRCH)

/*
 * Resolver "pfcode" values.  Used by dig.
 */
#define	RES_PRF_STATS	0x0001
/*			0x0002  */
#define	RES_PRF_CLASS	0x0004
#define	RES_PRF_CMD	0x0008
#define	RES_PRF_QUES	0x0010
#define	RES_PRF_ANS	0x0020
#define	RES_PRF_AUTH	0x0040
#define	RES_PRF_ADD	0x0080
#define	RES_PRF_HEAD1	0x0100
#define	RES_PRF_HEAD2	0x0200
#define	RES_PRF_TTLID	0x0400
#define	RES_PRF_HEADX	0x0800
#define	RES_PRF_QUERY	0x1000
#define	RES_PRF_REPLY	0x2000
#define	RES_PRF_INIT	0x4000
/*			0x8000  */

extern struct __res_state _res;


/* Private routines shared between libc/net, named, nslookup and others. */
#define	dn_skipname	__dn_skipname
#define	fp_query	__fp_query
#define	hostalias	__hostalias
#define	putlong		__putlong
#define	putshort	__putshort
#define p_class		__p_class
#define p_time		__p_time
#define p_type		__p_type

int	 __dn_skipname __P((const u_char *, const u_char *));
void	 __fp_query __P((char *, FILE *));
char	*__hostalias __P((const char *));
void	 __putlong __P((uint32_t, u_char *));
void	 __putshort __P((uint16_t, u_char *));
char	*__p_class __P((int));
char	*__p_time __P((uint32_t));
char	*__p_type __P((int));

int	 resQuery __P((char *, int, int, u_char *, int));
int	 resSearch __P((const char *, int, int, u_char *, int));
int	 resMkQuery __P((int, const char *, int, int, const char *, int,
		const char *, char *, int));
int	 resSend __P((const char *, int, char *, int));

#ifdef __cplusplus
}
#endif

#endif /* __INCresolvh */
