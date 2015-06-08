/* interface.h - common definitions for RIP interface routines */

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
 *	@(#)interface.h	8.1 (Berkeley) 6/5/93
 */

/*
modification history
--------------------
01k,28apr02,rae  added extern C jazz (SPR #76303)
01j,22mar02,niq  Merged from Synth view, tor3_x.synth branch, ver 01l
01i,12oct01,rae  merge from truestack ver 01j base 01h
01h,11sep98,spm  removed references to bloated trace commands (SPR #22350)
01g,06oct97,gnn  added sendHook function pointer.
01f,16may97,gnn  added leakHook function pointer.
01e,17apr97,gnn  changed MIB-II names to follow conventions.
01d,14apr97,gnn  added the pointer for the authentication hook.
01c,07apr97,gnn  cleared up some of the more egregious warnings.
                 added MIB-II interfaces and options.
01b,24feb97,gnn  removed multicast stuff
                 changed the routine names.
01a,26nov96,gnn  created from BSD4.4 routed
*/

#ifndef __INCinterfaceh
#define __INCinterfaceh

#ifdef __cplusplus
extern "C" {
#endif
#include "m2RipLib.h"

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif  /* CPU_FAMILY==I960 */

/* defines */

#define AUTH_KEY_LEN        16 /* must be 20 if SHA-1 will be used */
#define RIP_AUTH_PROTO_MD5  1
#define RIP_AUTH_PROTO_SHA1 2


/*
 * Routing table management daemon.
 */

struct interface;
typedef struct ripAuthKey RIP_AUTH_KEY;

struct ripAuthKey
    {
    UINT16             keyId;               /* the key id */
    UINT               authProto;           /* the authentication protocol */
    UCHAR              pKey [AUTH_KEY_LEN]; /* the secret authentication key */
    ULONG              timeStart;           /* time key was entered */
    ULONG              timeEnd;             /* time when key expires */
    ULONG              lastRcvSequence;     /* last sequence number received */
    ULONG              lastSntSequence;     /* last sequence number sent */
    struct interface * ifp;                 /* interface where key is valid */
    RIP_AUTH_KEY *     pNext;               /* next auth key in the list */
    };

/*
 * An ``interface'' is similar to an ifnet structure,
 * except it doesn't contain q'ing info, and it also
 * handles ``logical'' interfaces (remote gateways
 * that we want to keep polling even if they go down).
 * The list of interfaces which we maintain is used
 * in supplying the gratuitous routing table updates.
 */
struct interface {
	struct	interface *int_next;
	struct	sockaddr int_addr;		/* address on this host */
	union {
             struct	sockaddr intu_broadaddr;
             struct	sockaddr intu_dstaddr;
	} int_intu;
#define	int_broadaddr	int_intu.intu_broadaddr	/* broadcast address */
#define	int_dstaddr	int_intu.intu_dstaddr	/* other end of p-to-p link */
	int	int_metric;			/* init's routing entry */
	int	int_flags;			/* see below */
	/* START INTERNET SPECIFIC */
	u_long	int_net;			/* network # */
	u_long	int_netmask;			/* net mask for addr */
	u_long	int_subnet;			/* subnet # */
	u_long	int_subnetmask;			/* subnet mask for addr */
	/* END INTERNET SPECIFIC */
	char	*int_name;			/* from kernel if structure */
        u_short	int_index;			/* from kernel if structure */
	u_short	int_transitions;		/* times gone up-down */

        M2_RIP2_IFSTAT_ENTRY ifStat;            /* MIB-II statistics. */
        M2_RIP2_IFCONF_ENTRY ifConf;            /* MIB-II configuration. */

        FUNCPTR authHook;                /* per interface authentication */
        FUNCPTR leakHook;                /* per interface leak hook. */
        BOOL (*sendHook) ();             /* per interface validation hook */
        RIP_AUTH_KEY * pAuthKeys;        /* list of valid authe keys */
};

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 0                 /* turn off alignment requirement */
#endif  /* CPU_FAMILY==I960 */

/*
 * 0x1 to 0x10 are reused from the kernel's ifnet definitions,
 * the others agree with the RTS_ flags defined elsewhere.
 */
#define	IFF_UP		0x1		/* interface is up */
#define	IFF_BROADCAST	0x2		/* broadcast address valid */
#define	IFF_DEBUG	0x4		/* turn on debugging */
#define	IFF_LOOPBACK	0x8		/* software loopback net */
#define	IFF_POINTOPOINT	0x10		/* interface is point-to-point link */

#define	IFF_SUBNET	0x100000	/* interface on subnetted network */
#define	IFF_PASSIVE	0x200000	/* can't tell if up/down */
#define	IFF_INTERFACE	0x400000	/* hardware interface */
#define	IFF_REMOTE	0x800000	/* interface isn't on this machine */
#define  IFF_ROUTES_DELETED 0x1000000    /* interface routes have been deleted*/

struct	interface *ripIfWithAddr();
struct	interface *ripIfWithAddrAndIndex();
struct	interface *ripIfWithDstAddr();
struct	interface *ripIfWithDstAddrAndIndex();
struct	interface *ripIfWithNet();
struct	interface *ripIfLookup();

#ifdef __cplusplus
}
#endif
#endif /* __INCinterfaceh */
