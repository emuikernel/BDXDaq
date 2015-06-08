/* defs.h - common definitions for routed/rip */

/* Copyright 1984 - 2002 Wind River Systems, Inc. */

/*
 * Copyright (c) 1983, 1988, 1993
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
 *	@(#)defs.h	8.2 (Berkeley) 4/28/95
 */

/*
modification history
--------------------
01o,13jan03,rae  Merged from velocecp branch (SPR #83863)
01n,28apr02,rae  added extern C jazz (SPR #76303)
01m,22mar02,niq  Merged from Synth view, tor3_x.synth branch, ver 01o
01l,24jan02,niq  SPR 72415 - Added support for Route tags
01k,12oct01,rae  merge from truestack ver 01l, base 01j
01j,16mar99,spm  recovered orphaned code from tor1_0_1.sens1_1 (SPR #25770)
01i,05oct98,spm  made task parameters adjustable (SPR #22422)
01h,11sep98,spm  added support for expanded ripShutdown routine and provided 
                 broader access to mutual exclusion semaphore to prevent 
                 collisions between RIP tasks (SPR #22352); removed references
                 to bloated trace commands (SPR #22350); corrected value
                 of MIN_WAITTIME (SPR #22360)
01g,26jun98,spm  changed prototypes to remove compiler warnings; added 
                 prototype for ripClearInterfaces; changed RIP_MCAST_ADDR
                 constant from string to value
01f,06oct97,gnn  added function prototypes and removed ripState
01e,17apr97,gnn  changed the MIB-II variables to follow coding conventions.
01d,07apr97,gnn  cleared up some of the more egregious warnings.
                 added MIB-II interfaces and options.
01c,12mar97,gnn  added multicast support
                 added timer variables
01b,24feb97,gnn  changed some variable names, added some fields to global
                 structure
                 changed location of netdb.h
01a,26nov96,gnn  created from BSD4.4 routed
*/

#ifndef __INCdefsh
#define __INCdefsh

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Internal data structure definitions for
 * user routing process.  Based on Xerox NS
 * protocol specs with mods relevant to more
 * general addressing scheme.
 */
#include "sys/socket.h"
#include "time.h"

#include "net/route.h"
#include "net/af.h"
#include "netinet/in.h"
#include "rip/ripLib.h"

#include "stdio.h"

#include "rip/interface.h"
#include "rip/table.h"
#include "rip/rip2.h"
#include "rip/m2RipLib.h"

#include "wdLib.h"
#include "semLib.h"
#include "netdb.h"

#define equal(a1, a2) \
	(memcmp((a1), (a2), sizeof (struct sockaddr)) == 0)

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif  /* CPU_FAMILY==I960 */

#define S_ADDR(x)       (((struct sockaddr_in *)(x))->sin_addr.s_addr)
#define INFO_DST(I)     ((I)->rti_info[RTAX_DST])
#define INFO_GATE(I)    ((I)->rti_info[RTAX_GATEWAY])
#define INFO_MASK(I)    ((I)->rti_info[RTAX_NETMASK])
#define INFO_IFA(I)     ((I)->rti_info[RTAX_IFA])
#define INFO_IFP(I)     ((I)->rti_info[RTAX_IFP])
#define INFO_AUTHOR(I)  ((I)->rti_info[RTAX_AUTHOR])
#define INFO_BRD(I)     ((I)->rti_info[RTAX_BRD])
  
/* Needed for compatability */
/* XXX */
struct osockaddr {
        u_short sa_family;              /* address family */
        char    sa_data[14];            /* up to 14 bytes of direct address */
}; 

/*
 * This global structure holds what used to be all the global 
 * variables in the rip program.
 */

typedef struct rip_globals
    {
    int version;                /* What RIP version are we running? */
    struct	sockaddr_in addr;	/* address of daemon's socket */
    
    int	s;			/* source and sink of all data */
    int	routeSocket;		/* source for routing messages */

    int ripTaskId; 		/* Identifier for primary RIP task. */
    int ripTimerTaskId; 	/* Identifier for RIP timer task. */

    int	supplier;		/* process should supply updates */
    BOOL lookforinterfaces;	/* if 1 probe kernel for new up interfaces */
    int	externalinterfaces;	/* # of remote and local interfaces */
    int gateway;                /* Are we a gateway? */
    BOOL multicast;             /* Are we using multicast? */
    struct	timeval now;		/* current idea of time */
    struct      timeval then;           /* previous idea of time */
    struct	timeval lastbcast;	/* last time all/changes broadcast */
    struct	timeval lastfullupdate;/* last time full table broadcast */
    struct	timeval nextbcast;     /* time to wait before changes */
                                        /* broadcast */
    int	needupdate;		/* true if we need update at nextbcast */
    
    int timerRate;          /* How often does the timer go off */
    int supplyInterval;     /* How often do we supply routes? */
    int expire;             /* HOw long until we expire a route? */
    int garbage;            /* When is a route really thrown out? */

    char	packet[MAXPACKETSIZE+1];
    struct	rip_pkt *msg;
    
    int         port;
    int         faketime;       /* Used by the timer routine to keep track. */
    WDOG_ID     timerDog;       /* Watch dog for firing off the timer. */
    SEM_ID      timerSem;       /* Semaphore that our user level routine */
                                /* waits on to do timer processing. */
    FUNCPTR 	pRouteHook;   	/* Route propagation hook */

    /*
     * Variables to store MIB-II data.
     */

    M2_RIP2_GLOBAL_GROUP ripGlobal;

    /*
     * The ripConf element contains configuration settings
     * copied for all available interfaces during initialization.
     * Because the configuration may be changed on an interface-specific 
     * basis, this structure should not be consulted for the
     * actual values after routedIfInit() has been called.
     */
    
    M2_RIP2_IFCONF_ENTRY ripConf;
    } RIP;

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 0                 /* turn off alignment requirement */
#endif  /* CPU_FAMILY==I960 */

IMPORT SEM_ID ripLockSem;

/* This is the global structure used by the routing daemon. */

struct	in_addr inet_makeaddr();
int	inet_maskof();
int 	sndmsg();
STATUS	supply();
int	cleanup();

int	rtioctl();
struct rt_entry * rtadd(struct sockaddr *dst, struct sockaddr *gate, int metric, 
			int state, struct sockaddr *netmask, int proto, int tag,
			int from, struct interface *ifp);
STATUS rtchange(struct rt_entry *rt, struct sockaddr *gate, short metric,
                struct sockaddr *netmask, int tag, int from, 
		struct interface *ifp);
STATUS rtdelete(struct rt_entry *rt);

void toall (int (*)(), int, struct interface *);

void ripRouteToAddrs (struct rt_entry* pRoute,
                      struct sockaddr_in** ppDsin,
                      struct sockaddr_in** ppGsin,
                      struct sockaddr_in** ppNsin);
void ifRouteAdd (struct sockaddr * pAddr, int refCnt, int subnetsCnt, 
		 BOOL internalFlag, u_long subnetMask);
void ripBuildPacket (RIP2PKT *, struct rt_entry*, struct interface *, int);
void ripSetInterfaces(INT32 sock, UINT32 mcastAddr);
void ripClearInterfaces(INT32 sock, UINT32 mcastAddr);
STATUS ripRequestSend (u_short ifIndex);

/* Defines created in the port to VxWorks. */

#define RIP_PORT 520
#define NSEC_MAX 1000000000
#define RIP_TASK "tRipTask"
#define RIP_TIMER "tRipTimerTask"
#define RIP_MAX_PACKET 512                   /* The maximum packet size. */
#define RIP_MIN_PACKET 24                    /* A packet with 1 entry. */

#define RIP_MCAST_ADDR 0xe0000009 	/* RIP group address is 224.0.0.9 */

#ifdef __cplusplus
}
#endif
#endif /* __INCdefsh */
