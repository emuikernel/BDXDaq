/* mbuf.h - mbuf header file */

/* Copyright 1984 - 2002 Wind River Systems, Inc. */

/*
 * Copyright (c) 1982, 1986, 1988, 1993
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
 *	@(#)mbuf.h	8.3 (Berkeley) 1/21/94
 */

/*
modification history
--------------------
01n,21mar02,vvv  updated doc for copy_from_mbufs (SPR #20787)
01m,10oct01,rae  merge from truestack (VIRTUAL_STACK)
01l,30oct00,ijm  changed M_TRAILINGSPACE from + to - (m)->m_len (SPR# 30996)
01k,11dec97,vin  made m_free and m_freem as macros calling netMblkClFree
		 and netMblkClChainFree functions.
01j,08dec97,vin  changed m_copym to netMblkChainDup macro SPR 9966.
01i,30nov97,vin  added extern declarations _pNetSysPool & relevant stuff.
01h,19sep97,vin  changes to fit in clBlk logic
01g,14aug97,vin  changed MBUF_CONFIG to MBLK_CONFIG moved generic stuff to
		 netBufLib.h
01f,02jul97,vin  moved clIncr in clPool to the end. optimized mbuf structure
		 for long word accesses.
01e,15may97,vin  reworked macro copy_from_mbufs, and bcopy_from_mbufs,
		 cleanup.
01d,31jan96,vin  added default cluster configurations.
01c,03dec96,vin  changed declaration for m_getclr(). 
01b,17nov96,vin  removed mbufs, code works with (mbuf, cluster) pair.
01a,03mar96,vin  created from BSD4.4 stuff,integrated with 02u of mbuf.h
*/

#ifndef __INCmbufh
#define __INCmbufh

#ifdef __cplusplus
extern "C" {
#endif

#include "vxWorks.h"
#include "sys/types.h"
#include "net/unixLib.h"
#include "semLib.h"
#include "memLib.h"
#include "string.h"
#include "netBufLib.h"

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif	/* CPU_FAMILY==I960 */

/* Constants */

#define MCLBYTES        	2048	/* default cluster size */
#define USE_CLUSTER(size)       ((size) >= 512 ? TRUE : FALSE)

#define	M_MAXCOMPRESS	(CL_SIZE_128/2)	/* max amount to copy for compression */
#define mbuf		mBlk
#define	m_next		mBlkHdr.mNext
#define	m_len		mBlkHdr.mLen
#define	m_data		mBlkHdr.mData
#define	m_type		mBlkHdr.mType
#define	m_flags		mBlkHdr.mFlags
#define	m_nextpkt	mBlkHdr.mNextPkt
#define m_act		m_nextpkt
#define	m_pkthdr	mBlkPktHdr
#define	m_ext		pClBlk
#define m_extBuf	m_ext->clNode.pClBuf
#define m_extFreeRtn	m_ext->pClFreeRtn
#define m_extSize	m_ext->clSize
#define m_extRefCnt	m_ext->clRefCnt
#define m_extArg1	m_ext->clFreeArg1
#define m_extArg2	m_ext->clFreeArg2
#define m_extArg3	m_ext->clFreeArg3

/* flags copied when copying m_pkthdr */
#define	M_COPYFLAGS	(M_PKTHDR|M_EOR|M_BCAST|M_MCAST)


/* flags to mBufClGet/mHdrClGet/mBlkGet */


/*
 * Macros for type conversion
 * mtod(m,t) -	convert mbuf pointer to data pointer of correct type
 */
#define	mtod(m,t)	((t)((m)->m_data))

#define MC_NO_CLUSTER	((u_char) 0)
#define MC_CLUSTER	((u_char) 1)
#define MC_LANCE	((u_char) 2)	/* 0x02 - 0x80 for netif */
#define MC_BACKPLANE	((u_char) 3)
#define MC_EI		((u_char) 4)
#define MC_UCLUSTER	((u_char) 0x80)	/* 0x80 - 0xff for network code */


/* m_pullup will pull up additional length if convenient;
 * should be enough to hold headers of second-level and higher protocols.
 */

#define	MPULL_EXTRA	32

/*
 * Set the m_data pointer of a newly-allocated mbuf (m_get/MGET) to place
 * an object of the specified size at the end of the mbuf, longword aligned.
 */
#define	M_ALIGN(m, len) \
    { (m)->m_data += (m->m_extSize - (len)) &~ (sizeof(long) - 1); }
/*
 * As above, for mbufs allocated with m_gethdr/MGETHDR
 * or initialized by M_COPY_PKTHDR.
 */
#define	MH_ALIGN(m, len) M_ALIGN((m),(len))

/*
 * Compute the amount of space available
 * before the current start of data in a cluster.
 */
#define	M_LEADINGSPACE(m) ((m)->m_data - (m)->m_extBuf) 

/*
 * Compute the amount of space available
 * after the end of data in a cluster.
 */

#define	M_TRAILINGSPACE(m) ((m)->m_extBuf + (m)->m_extSize - \
			    (m)->m_data - (m)->m_len)
/*
 * Arrange to prepend space of size plen to mbuf m.
 * If a new mbuf must be allocated, how specifies whether to wait.
 * If how is M_DONTWAIT and allocation fails, the original mbuf chain
 * is freed and m is set to NULL.
 */

#ifndef	STANDALONE_AGENT
#define	M_PREPEND(m, plen, how)					\
    { 								\
	if (M_LEADINGSPACE(m) >= (plen)) { 			\
		(m)->m_data -= (plen); 				\
		(m)->m_len += (plen); 				\
	} else 							\
		(m) = m_prepend((m), (plen), (how)); 		\
	if ((m) && (m)->m_flags & M_PKTHDR) 			\
		(m)->m_pkthdr.len += (plen); 			\
    }
#else	/* STANDALONE_AGENT */
/*
 * for standalone agent, the M_PREPEND routine does no try to allocate
 * a new buffer (there is no available). Anyway, this is not needed since
 * all the WDB packets can be stored in one buffer.
 */

#define	M_PREPEND(m, plen, how)					\
    { 								\
	if (M_LEADINGSPACE(m) >= (plen)) { 			\
		(m)->m_data -= (plen); 				\
		(m)->m_len += (plen); 				\
	} else 							\
		(m) = NULL;			 		\
	if ((m) && (m)->m_flags & M_PKTHDR) 			\
		(m)->m_pkthdr.len += (plen); 			\
    }
#endif	/* STANDALONE_AGENT */

/* change mbuf to new type */
#define MCHTYPE(m, t)						\
    {								\
    int ms = splimp(); 						\
    mbstat.m_mtypes[(m)->m_type]--; 				\
    mbstat.m_mtypes[t]++;					\
    (m)->m_type = t;                                            \
    splx(ms); 							\
    }


/*
 * We have to do this locally so that we don't have circular
 * include dependencies.
 */

#ifdef VIRTUAL_STACK
#define _pNetDpool vsTbl[myStackNum]->pNetDPool
#endif

/* compatiblity with 4.3 */
#define  m_copy(m, o, l)       	m_copym((m), (o), (l), M_DONTWAIT)
#define  m_copym(m, o, l, w) 	netMblkChainDup(_pNetDpool, (m), (o), (l), (w))
#define  m_freem(m)		netMblkClChainFree(m)
#define  m_free(m)		netMblkClFree(m)    

#ifdef VIRTUAL_STACK
#undef _pNetDpool 	/* prevents conflicts with vsData.h definition */
#endif

#define m_mbufs		mNum
#define m_drops		mDrops
#define m_wait		mWait
#define m_drain		mDrain
#define m_mtypes 	mTypes

/* SunOS compatible macros to be used in network interface drivers */

/* copy_from_mbufs copies data from mbuf chain to user supplied buffer area.
 *
 * N.B.: This macro takes an extra argument 'len' which is used to return
 * the number of bytes actually transferred.  SunOS routine copy_from_mbufs
 * returns this value; VxWorks macro copy_from_mbufs "returns" the same
 * value in 'len'.
 *
 * This macro should not be used since bcopy_from_mbufs is now obsolete.
 */

#define copy_from_mbufs(buf0, m, len) 					\
    bcopy_from_mbufs (buf0, m, len, NONE)

/* bcopy_from_mbufs copies dat from mbuf chain to user supplied buffer area
 * by transferring bytes by unit 'width' indicated by the user.
 * This macro is similiar to copy_from_mbufs; the only difference is the
 * extra argument 'width' which is used to accommodate certain hardware
 * restrictions that require copying of data to only occur at certain byte size
 * boundaries.  Calling this macro with 'width' value set to NONE is equivalent
 * to calling copy_from_mbufs macro.
 *
 * This macro should not be used since it calls copyFromMbufs which may cause
 * unaligned memory accesses resulting in target failure. The routine
 * netMblkToBufCopy can be used instead. However, netMblkToBufCopy does not
 * free the mbuf chain after copying.
 *
 * Instead of 
 *    bcopy_from_mbufs (pBuf, pMblk, len, width);
 * we can have
 *    len = netMblkToBufCopy (pMblk, pBuf, NULL);
 *    netMblkClChainFree (pMblk);
 *
 * There is no width argument but the last argument to netMblkToBufCopy 
 * specifies which routine will be used to copy the data. If NULL, bcopy is
 * used.
 */
        
#define bcopy_from_mbufs(buf0, m, len, width)				\
    {									\
    (len) = copyFromMbufs (buf0, m, width);				\
    }
    
/* copy_to_mbufs copies data into mbuf chain and returns the pointer to
 * the first mbuf.
 *
 * N.B.: This macro calls a routine bcopy_to_mbufs with width value set
 * to NONE.  VxWorks routine bcopy_to_mbufs is provided to accommodate
 * hardware restrictions that require copying of data to only
 * occur at certain byte size boundaries.
 */

#define copy_to_mbufs(buf, totlen, off, ifp) 				\
    bcopy_to_mbufs (buf, totlen, off, ifp, NONE)

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 0                 /* turn off alignment requirement */
#endif  /* CPU_FAMILY==I960 */

/* imported stuff */

IMPORT struct mbstat 	mbstat;			/* mBlk statistics */
IMPORT M_CL_CONFIG	mClBlkConfig;		/* mBlk, clBlk configuration */
IMPORT CL_DESC 		clDescTbl [];		/* cluster configuration */
IMPORT int		clDescTblNumEnt; 	/* no. elements in the table */
IMPORT M_CL_CONFIG	sysMclBlkConfig;	/* mBlk, clBlk configuration */
IMPORT CL_DESC 		sysClDescTbl [];	/* cluster configuration */
IMPORT int		sysClDescTblNumEnt; 	/* no. elements in the table */

#ifndef VIRTUAL_STACK
IMPORT NET_POOL_ID	_pNetDpool;		/* network data pool */
IMPORT NET_POOL_ID	_pNetSysPool;		/* network data pool */
#endif

/* These variables are initialized when individual protocols are initialized */

extern int max_linkhdr;		/* largest link-level header */
extern int max_protohdr;	/* largest protocol header */
extern int max_hdr;		/* largest link+protocol header */
extern int max_datalen;		/* CL_SIZE_128 - max_hdr */

extern struct	mbuf *m_free (struct mbuf *);
extern struct	mbuf *m_getclr (int, UCHAR, int, BOOL);
extern struct	mbuf *m_prepend (struct mbuf *, int, int);
extern struct	mbuf *m_pullup (struct mbuf *, int);
extern void	m_adj (struct mbuf *, int);
extern void	m_freem (struct mbuf *);
extern void 	m_cat (struct mbuf *, struct mbuf *);
extern struct 	mbuf *m_devget (char *, int, int, struct ifnet *,
				void (*copy)());
struct mbuf *	mBufClGet (int, UCHAR, FAST int, BOOL); 
struct mbuf * 	mHdrClGet (int, UCHAR, FAST int, BOOL); 

IMPORT struct mbuf * bcopy_to_mbufs (u_char * buf, int totlen, int off0,
                                     struct ifnet * ifp, int width);
IMPORT struct mbuf * build_cluster (u_char * buf, int totlen,
                                    struct ifnet * ifp, u_char ctype,
                                    u_char * pRefcnt, FUNCPTR freeRtn,
                                    int arg1, int arg2, int arg3);
IMPORT int	copyFromMbufs (char * pIobuf, struct mbuf * pMbuf, int width);

#ifdef __cplusplus
}
#endif

#endif /* __INCmbufh */
