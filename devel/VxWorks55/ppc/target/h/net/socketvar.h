/* socketvar.h - socket header file */

/* Copyright 1984 - 2001 Wind River Systems, Inc. */
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that this notice is preserved and that due credit is given
 * to the University of California at Berkeley. The name of the University
 * may not be used to endorse or promote products derived from this
 * software without specific prior written permission. This software
 * is provided ``as is'' without express or implied warranty.
 *
 *      @(#)socketvar.h 7.3 (Berkeley) 12/30/87
 */

/*
modification history
--------------------
02r,10oct01,rae  merge from truestack v02u base 02q (SPR #34005)
02q,19sep97,vin  changed m_ext.ext_size to m_extSize
02p,28feb96,vin  upgraded to bsd 4.4.
02o,18aug94,dzb  added sb_want to struct sockbuf (SPR #3552).
                 added check of m_len for user clusters in sballoc and sbfree.
		 added so_userArg to struct socket for use w/ sowakeup callback.
02n,22sep92,rrr  added support for c++
02m,26may92,rrr  the tree shuffle
02l,04oct91,rrr  passed through the ansification filter
		  -changed copyright notice
02k,05oct90,shl  added copyright notice.
                 made #endif ANSI style.
02j,26jun90,jcf  changed SEM_BINARY to SEMAPHORE.
02i,19apr90,hjb  removed sb_flagSem, changed MIN to min, deleted
		 sblock and sbunlock which are not needed anymore.
02h,20mar90,jcf  changed semTake to include timeout of WAIT_FOREVER.
02g,16mar90,rdc  added select support.
02f,16apr89,gae  updated to new 4.3BSD.
02e,04may88,jcf  changed SEMAPHOREs to SEM_IDs.
02d,14dec87,gae  removed vxWorks.h include!!!
02c,04nov87,dnw  removed unnecessary KERNEL conditional.
02b,03apr87,ecs  added header and copyright.
02a,02feb87,jlf  removed ifdef CLEAN
01a,19nov86,llk  fixed include file references.
*/

#ifndef __INCsocketvarh
#define __INCsocketvarh

#ifdef __cplusplus
extern "C" {
#endif

#include "vxWorks.h"
#include "semLib.h"
#include "selectLib.h"
#include "net/uio.h"
#include "net/mbuf.h"

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif  /* CPU_FAMILY==I960 */

/*
 * Kernel structure per socket.
 * Contains send and receive buffer queues,
 * handle on protocol and pointer to protocol
 * private data and error information.
 */

struct socket
    {
    short	so_type;		/* generic type, see socket.h */
    short	so_options;		/* from socket call, see socket.h */
    short	so_linger;		/* time to linger while closing */
    short	so_state;		/* internal state flags SS_*, below */
    caddr_t	so_pcb;			/* protocol control block */
    struct	protosw *so_proto;	/* protocol handle */

    /* If user wants to use so_userArg, then public API is required.
     * Because socket{} is located in kernel heap, and user can't access
     * it directly.
     */
    int		so_userArg;		/* WRS extension - user argument */
    SEM_ID	so_timeoSem;		/* WRS extension */
    int		so_fd;			/* WRS extension - file descriptor */

    /*
     * Variables for connection queueing.
     * Socket where accepts occur is so_head in all subsidiary sockets.
     * If so_head is 0, socket is not related to an accept.
     * For head socket so_q0 queues partially completed connections,
     * while so_q is a queue of connections ready to be accepted.
     * If a connection is aborted and it has so_head set, then
     * it has to be pulled out of either so_q0 or so_q.
     * We allow connections to queue up based on current queue lengths
     * and limit on number of queued connections for this socket.
     */

    struct	socket *so_head;	/* back pointer to accept socket */
    struct	socket *so_q0;		/* queue of partial connections */
    struct	socket *so_q;		/* queue of incoming connections */
    short	so_q0len;		/* partials on so_q0 */
    short	so_qlen;		/* number of connections on so_q */
    short	so_qlimit;		/* max number queued connections */
    short	so_timeo;		/* connection timeout */
    u_short	so_error;		/* error affecting connection */
    short	so_pgrp;		/* pgrp for signals */
    u_long	so_oobmark;		/* chars to oob mark */

    /* Variables for socket buffering. */

    struct	sockbuf
	{
	u_long		sb_cc;		/* actual chars in buffer */
	u_long		sb_hiwat;	/* max actual char count */
	u_long		sb_mbcnt;	/* chars of mbufs used */
	u_long		sb_mbmax;	/* max chars of mbufs to use */
	u_long		sb_lowat;	/* low water mark (not used yet) */
	struct		mbuf *sb_mb;	/* the mbuf chain */
	struct		proc *sb_sel;	/* process selecting read/write */
	short		sb_timeo;	/* timeout (not used yet) */
	short		sb_flags;	/* flags, see below */

	SEM_ID		sb_Sem;		/* WRS extension */
	int		sb_want;	/* WRS extension */
	} so_rcv, so_snd;

#define	SB_MAX		(256*1024)	/* default for max chars in sockbuf */
#define	SB_LOCK		0x01		/* lock on data queue */
#define	SB_WANT		0x02		/* someone is waiting to lock */
#define	SB_WAIT		0x04		/* someone is waiting for data/space */
#define	SB_SEL		0x08		/* someone is selecting */
#define	SB_ASYNC	0x10		/* ASYNC I/O, need signals */
#define	SB_NOTIFY	(SB_WAIT|SB_SEL|SB_ASYNC)
#define	SB_NOINTR	0x40		/* operations not interruptible */

    void	(*so_upcall) (struct socket *so, caddr_t arg, int waitf);
    caddr_t	so_upcallarg;		/* Arg for above */
    BOOL 	selectFlag; 		/* Select list available? */
    SEL_WAKEUP_LIST so_selWakeupList; /* WRS extension */
    int         vsid;                 /* WRS extension for virtual stack id */
    };

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 0                 /* turn off alignment requirement */
#endif  /* CPU_FAMILY==I960 */


/*
 * Socket state bits.
 */
#define	SS_NOFDREF		0x001	/* no file table ref any more */
#define	SS_ISCONNECTED		0x002	/* socket connected to a peer */
#define	SS_ISCONNECTING		0x004	/* in process of connecting to peer */
#define	SS_ISDISCONNECTING	0x008	/* in process of disconnecting */
#define	SS_CANTSENDMORE		0x010	/* can't send more data to peer */
#define	SS_CANTRCVMORE		0x020	/* can't receive more data from peer */
#define	SS_RCVATMARK		0x040	/* at mark on input */

#define	SS_PRIV			0x080	/* privileged for broadcast, raw... */
#define	SS_NBIO			0x100	/* non-blocking ops */
#define	SS_ASYNC		0x200	/* async i/o notify */
#define	SS_ISCONFIRMING		0x400	/* deciding to accept connection req */

/*
 * Macros for sockets and socket buffering.
 */

/*
 * How much space is there in a socket buffer (so->so_snd or so->so_rcv)?
 * This is problematical if the fields are unsigned, as the space might
 * still be negative (cc > hiwat or mbcnt > mbmax).  Should detect
 * overflow and return 0.  Should use "lmin" but it doesn't exist now.
 */
#define	sbspace(sb) \
    ((long) min((int)((sb)->sb_hiwat - (sb)->sb_cc), \
	 (int)((sb)->sb_mbmax - (sb)->sb_mbcnt)))

/* do we have to send all at once on a socket? */
#define	sosendallatonce(so) \
    ((so)->so_proto->pr_flags & PR_ATOMIC)

/* can we read something from so? */
#define	soreadable(so) \
    ((so)->so_rcv.sb_cc >= (so)->so_rcv.sb_lowat || \
	((so)->so_state & SS_CANTRCVMORE) || \
	(so)->so_qlen || (so)->so_error)

/* can we write something to so? */
#define	sowriteable(so) \
    ((sbspace(&(so)->so_snd) >= (so)->so_snd.sb_lowat) && \
	(((so)->so_state&SS_ISCONNECTED) || \
	  (((so)->so_proto->pr_flags&PR_CONNREQUIRED)==0) || \
     ((so)->so_state & SS_CANTSENDMORE) || \
     (so)->so_error))

/* adjust counters in sb reflecting allocation of m */
#define	sballoc(sb, m) { \
	(sb)->sb_cc += (m)->m_len; \
	(sb)->sb_mbcnt += MSIZE; \
	if ((m)->m_flags & M_EXT) \
		(sb)->sb_mbcnt += (m)->m_extSize; \
}

/* adjust counters in sb reflecting freeing of m */
#define	sbfree(sb, m) { \
	(sb)->sb_cc -= (m)->m_len; \
	(sb)->sb_mbcnt -= MSIZE; \
	if ((m)->m_flags & M_EXT) \
		(sb)->sb_mbcnt -= (m)->m_extSize; \
}

/*
 * Set lock on sockbuf sb; sleep if lock is already held.
 * Unless SB_NOINTR is set on sockbuf, sleep is interruptible.
 * Returns error without lock if sleep is interrupted.
 */
#define sblock(sb, wf) ((sb)->sb_flags & SB_LOCK ? \
		(((wf) == M_WAITOK) ? sb_lock(sb) : EWOULDBLOCK) : \
		((sb)->sb_flags |= SB_LOCK), 0)

/* release lock on sockbuf sb */
#define	sbunlock(sb) { \
	(sb)->sb_flags &= ~SB_LOCK; \
	if ((sb)->sb_flags & SB_WANT) { \
		(sb)->sb_flags &= ~SB_WANT; \
		wakeup((caddr_t)&(sb)->sb_flags); \
	} \
}

#define	sorwakeup(so)	{ sowakeup((so), &(so)->so_rcv, SELREAD); \
			  if ((so)->so_upcall) \
			    (*((so)->so_upcall))((so), (so)->so_upcallarg, M_DONTWAIT); \
			}

#define	sowwakeup(so)	sowakeup((so), &(so)->so_snd, SELWRITE)

IMPORT VOIDFUNCPTR sowakeupHook;

extern u_long	sb_max;

/* to catch callers missing new second argument to sonewconn: */
#define	sonewconn(head, connstatus)	sonewconn1((head), (connstatus))

/* strings for sleep message: */
extern	char netio[], netcon[], netcls[];

/*
 * File operations on sockets.
 */
extern int soo_ioctl (struct socket * so, int cmd, caddr_t data);
extern int soo_select (struct socket *so, SEL_WAKEUP_NODE *wakeupNode);
extern int soo_unselect (struct socket *so, SEL_WAKEUP_NODE *wakeupNode); 

struct mbuf;
struct sockaddr;

extern void	sbappend (struct sockbuf *sb, struct mbuf *m);
extern int	sbappendaddr (struct sockbuf *sb, struct sockaddr *asa,
	    struct mbuf *m0, struct mbuf *control);
extern int	sbappendcontrol (struct sockbuf *sb, struct mbuf *m0,
	    struct mbuf *control);
extern void	sbappendrecord (struct sockbuf *sb, struct mbuf *m0);
extern void	sbcheck (struct sockbuf *sb);
extern void	sbcompress (struct sockbuf *sb, struct mbuf *m, struct mbuf *n);
extern void	sbdrop (struct sockbuf *sb, int len);
extern void	sbdroprecord (struct sockbuf *sb);
extern void	sbflush (struct sockbuf *sb);
extern void	sbinsertoob (struct sockbuf *sb, struct mbuf *m0);
extern void	sbrelease (struct sockbuf *sb);
extern int	sbreserve (struct sockbuf *sb, u_long cc);
extern void	sbwait (struct sockbuf *sb);
extern void	sbwakeup(struct socket *so, struct sockbuf *sb,
			 SELECT_TYPE wakeupType);
extern int	sb_lock (struct sockbuf *sb);
extern int	soabort (struct socket *so);
extern int	soaccept (struct socket *so, struct mbuf *nam);
extern int	sobind (struct socket *so, struct mbuf *nam);
extern void	socantrcvmore (struct socket *so);
extern void	socantsendmore (struct socket *so);
extern int	soclose (struct socket *so);
extern int	soconnect (struct socket *so, struct mbuf *nam);
extern int	soconnect2 (struct socket *so1, struct socket *so2);
extern int	socreate (int dom, struct socket **aso, int type, int proto);
extern int	sodisconnect (struct socket *so);
extern void	sofree (struct socket *so);
extern int	sogetopt (struct socket *so, int level, int optname,
	    struct mbuf **mp);
extern void	sohasoutofband (struct socket *so);
extern void	soisconnected (struct socket *so);
extern void	soisconnecting (struct socket *so);
extern void	soisdisconnected (struct socket *so);
extern void	soisdisconnecting (struct socket *so);
extern int	solisten (struct socket *so, int backlog);
extern struct socket *
	sonewconn1 (struct socket *head, int connstatus);
extern void	soqinsque (struct socket *head, struct socket *so, int q);
extern int	soqremque (struct socket *so, int q);
extern int	soreceive (struct socket *so, struct mbuf **paddr, 
			   struct uio *pUio, struct mbuf **mp0, struct mbuf **
			   controlp, int *flagsp);
extern int	soreserve (struct socket *so, u_long sndcc, u_long rcvcc);
extern void	sorflush (struct socket *so);
extern int	sosend (struct socket *so, struct mbuf *addr, struct uio *uio,
	    struct mbuf *top, struct mbuf *control, int flags);
extern int	sosetopt (struct socket *so, int level, int optname,
	    struct mbuf *m0);
extern int	soshutdown (struct socket *so, int how);
extern void	sowakeup (struct socket *so, struct sockbuf *sb, 
			  SELECT_TYPE wakeupType);

#ifdef __cplusplus
}
#endif

#endif /* __INCsocketvarh */
