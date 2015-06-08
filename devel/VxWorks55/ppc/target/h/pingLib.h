/* pingLib.h - Packet InterNet Groper (PING) library header */

/* Copyright 1994 - 2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01d,12mar02,rae  Print stats when task killed (SPR #73570)
01c,10oct01,rae  merge from truestack ver 01e, base 01a (SPR 67440)
01b,30oct00,gnn  Added PING_OPT_NOHOST flag to deal with SPR 22766
01a,25oct94,dzb  written.
*/

#ifndef __INCpingLibh
#define __INCpingLibh

#ifdef __cplusplus
extern "C" {
#endif

/* includes */

#include "vwModNum.h"
#include "hostLib.h"
#include "inetLib.h"
#include "semLib.h"
#include "wdLib.h"

/* defines */

#define PING_MAXPACKET		4096	/* max packet size */
#define PING_MINPACKET          12      /* min packet size */
#define PING_INTERVAL		1	/* default packet interval in seconds */
#define PING_TMO		5	/* default packet timeout in seconds */
#define ICMP_PROTO		1	/* icmp socket proto id */
#define ICMP_TYPENUM		20	/* icmp proto type identifier */
#define PING_TASK_NAME_LEN	20	/* max ping Tx task name length */

/* status codes */

#define	S_pingLib_NOT_INITIALIZED		(M_pingLib | 1)
#define	S_pingLib_TIMEOUT			(M_pingLib | 2)
 
/* flags */

#define	PING_OPT_SILENT		0x1	/* work silently */
#define	PING_OPT_DONTROUTE	0x2	/* dont route option */
#define	PING_OPT_DEBUG		0x4	/* print debugging messages */
#define PING_OPT_NOHOST         0x8     /* dont do host lookup */

/* typedefs */

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif  /* CPU_FAMILY==I960 */

typedef struct pingStat				/* PING_STAT */
    {
    struct pingStat *	statNext;		/* next struct in list */
    int			pingFd;                 /* socket file descriptor */
    char		toHostName [MAXHOSTNAMELEN + 2];/* name to ping - PAD */
    char		toInetName [INET_ADDR_LEN];/* IP addr to ping */
    u_char		bufTx [PING_MAXPACKET];	/* transmit buffer */
    u_char		bufRx [PING_MAXPACKET];	/* receive buffer */
    struct icmp	*	pBufIcmp;		/* ptr to icmp */
    ulong_t *		pBufTime;		/* ptr to time */
    int			dataLen;		/* size of data portion */
    int			numPacket;		/* total # of packets to send */
    int			numTx;			/* number of packets sent */
    int			numRx;			/* number of packets received */
    int			idRx;			/* id of Rx task */
    int			clkTick;		/* sys clock ticks per second */
    int			tMin;			/* min RT time (ms) */
    int			tMax;			/* max RT time (ms) */
    int			tSum;			/* sum of all times */
    int			flags;			/* option flags */
    int                 vsid;                   /* virtual stack being used */
    } PING_STAT;

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 0                 /* turn off alignment requirement */
#endif  /* CPU_FAMILY==I960 */

/* forward declarations */
 
#if defined(__STDC__) || defined(__cplusplus)
 
extern STATUS	 pingLibInit (void);
extern STATUS	 ping (char *host, int numPackets, ulong_t options);

#else   /* __STDC__ */

extern STATUS	pingLibInit ();
extern STATUS	ping ();
 
#endif  /* __STDC__ */
 
#ifdef __cplusplus
}
#endif
 
#endif /* __INCpingLibh */
