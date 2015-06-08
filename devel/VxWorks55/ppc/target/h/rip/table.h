/* table.h - definitions for managing RIP internal routing table */

/* Copyright 1984 - 2002 Wind River Systems, Inc. */

/*
 * Copyright (c) 1983, 1993
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
 *	@(#)table.h	8.1 (Berkeley) 6/5/93
 */

/*
modification history
--------------------
01i,28apr02,rae  added extern C jazz (SPR #76303)
01h,22mar02,niq  Merged from Synth view, tor3_x.synth branch, ver 01k
01g,24jan02,niq  SPR 72415 - Added support for Route tags
01f,12oct01,rae  merge from truestack ver 01h, base 01e
01e,16mar99,spm  recovered orphaned code from tor1_0_1.sens1_1 (SPR #25770)
01d,29sep98,spm  added support for IP group MIB (SPR #9374); removed unused 
                 entries from rt_entry structure (SPR #22535)
01c,11sep98,spm  updated contents to prevent compilation problems (SPR #22352)
01b,24feb97,gnn  Reworked the rt_entry structure.
01a,26nov96,gnn  created from BSD4.4 routed
*/

#ifndef __INCtableh
#define __INCtableh

#ifdef __cplusplus
extern "C" {
#endif

#include "net/route.h"

/*
 * Routing table management daemon.
 */

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif  /* CPU_FAMILY==I960 */

/*
 * Routing table structure; differs a bit from kernel tables.
 *
 * Note: the union below must agree in the first 4 members
 * so the ioctl's will work.
 */

struct rthash {
	struct	rt_entry *rt_forw;
	struct	rt_entry *rt_back;
};

struct rt_entry
    {
    struct	rt_entry *rt_forw;
    struct	rt_entry *rt_back;
    BOOL		 inKernel;	/* Route has been added to the kernel*/
    int			 rt_tag;	/* Route tag - for RIP v2 only */
    short		 rt_refcnt; 	/* How many additional interfaces */
      					/* need this route */
    short		 rt_subnets_cnt;/* How many subnetted interfaces */
      					/* need an "internal" classful */
      					/* network route. Set only for a */
					/* classful route (internal or  */
      					/* otherwise) when atleast two */
					/* subnetted interfaces or one */
      					/* non-subnetted and atleast one */
					/* subnetted interface exists */
					/* for the network */
    struct rtuentry 
        {
        u_long rtu_hash;
        struct sockaddr rtu_dst;
        struct sockaddr rtu_router;	/* Route's gateway - route supplier */
	  				/* for V1 or V2 with unspecified next*/
	  				/* hop. Set to next hop value for */
	  				/* V2 if specified */

        struct sockaddr rtu_netmask;
	int             rtu_orgrouter;	/* router from whom route was */
	  				/* received. For V1 will be same as */
	  				/* rtu_router. For V2 might be */
	  				/* diffrent from rtu_router */
        int rtu_flags;
        int rtu_state;
        int rtu_timer;			/* Route timer */
        int rtu_metric;			/* Route metric */
	int rtu_proto;			/* Protocol that owns the route */
        struct interface *rtu_ifp;	/* Interface through which the */
	  				/* gateway is reachable */
        } rtu_entry;
};

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 0                 /* turn off alignment requirement */
#endif  /* CPU_FAMILY==I960 */

#define	rt_hash		rtu_entry.rtu_hash	/* for net or host */
#define	rt_dst		rtu_entry.rtu_dst	/* match value */
#define	rt_router	rtu_entry.rtu_router	/* who to forward to */
#define	rt_orgrouter	rtu_entry.rtu_orgrouter	/* who received from */
#define	rt_netmask	rtu_entry.rtu_netmask	/* who to forward to */
#define	rt_flags	rtu_entry.rtu_flags	/* kernel flags */
#define	rt_timer	rtu_entry.rtu_timer	/* for invalidation */
#define	rt_state	rtu_entry.rtu_state	/* see below */
#define	rt_metric	rtu_entry.rtu_metric	/* cost of route */
#define	rt_ifp		rtu_entry.rtu_ifp	/* interface to take */
#define	rt_proto	rtu_entry.rtu_proto	/* protocol that owns this */
						/* route */

#define	ROUTEHASHSIZ	32		/* must be a power of 2 */
#define	ROUTEHASHMASK	(ROUTEHASHSIZ - 1)

/*
 * "State" of routing table entry.
 */
#define	RTS_CHANGED	0x1		/* route has been altered recently */
#define	RTS_EXTERNAL	0x2		/* extern info, not installed or sent */
#define	RTS_INTERNAL	0x4		/* internal route, not installed */
#define	RTS_PASSIVE	IFF_PASSIVE	/* don't time out route */
#define	RTS_INTERFACE	IFF_INTERFACE	/* route is for network interface */
#define	RTS_REMOTE	IFF_REMOTE	/* route is for ``remote'' entity */
#define	RTS_SUBNET	IFF_SUBNET	/* route is for network subnet */
#define	RTS_OTHER	0x1000000	/* route installed by another */
                                        /* protocol */
#define	RTS_PRIMARY	0x2000000	/* This is a primary route */

/*
 * Flags are same as kernel, with this addition for af_rtflags:
 */
#define	RTF_SUBNET	0x80000		/* pseudo: route to subnet */

#ifndef VIRTUAL_STACK
struct	rthash nethash[ROUTEHASHSIZ];
struct	rthash hosthash[ROUTEHASHSIZ];
#endif

struct	rt_entry *rtlookup();
struct	rt_entry *rtfind();

IMPORT STATUS mPrivRouteEntryAdd (long, long, long, int, int, int, 
                                  struct rtentry **);
IMPORT STATUS mPrivRouteEntryDelete (long, long, long, int, int, int, 
                                     struct rtentry *);
#ifdef __cplusplus
}
#endif
#endif /* __INCtableh */
