/* raw_cb.h - raw protocol interface control block header file */

/* Copyright 1984 - 2001 Wind River Systems, Inc. */

/*
 * Copyright (c) 1980, 1986, 1993
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
 *	@(#)raw_cb.h	8.1 (Berkeley) 6/10/93
 */

/*
modification history
--------------------
03c,10oct01,rae  merge from truestack (VIRTUAL_STACK)
03b,31jan97,vin  changed declaration according to prototype decl in protosw.h
03a,03mar96,vin  created from BSD4.4 stuff,integrated with 02i of raw_cb.h
02i,22sep92,rrr  added support for c++
02h,26may92,rrr  the tree shuffle
02g,28feb92,wmd  changed last pragma to turn off alignment 1.
02f,04oct91,rrr  passed through the ansification filter
		  -fixed #else and #endif
		  -changed copyright notice
02e,10jun91.del  added pragma for gnu960 alignment.
02d,05oct90,shl  added copyright notice.
                 made #endif ANSI style.
02c,16apr89,gae  updated to new 4.3BSD.
02b,04nov87,dnw  moved rawcb definition to raw_cb.c.
02a,03apr87,ecs  added header and copyright.
*/

#ifndef __INCraw_cbh
#define __INCraw_cbh

#ifdef __cplusplus
extern "C" {
#endif

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif	/* CPU_FAMILY==I960 */

/*
 * Raw protocol interface control block.  Used
 * to tie a socket to the generic raw interface.
 */
struct rawcb {
	struct	rawcb *rcb_next;	/* doubly linked list */
	struct	rawcb *rcb_prev;
	struct	socket *rcb_socket;	/* back pointer to socket */
	struct	sockaddr *rcb_faddr;	/* destination address */
	struct	sockaddr *rcb_laddr;	/* socket's address */
	struct	sockproto rcb_proto;	/* protocol family, protocol */
};

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 0                 /* turn off alignment requirement */
#endif  /* CPU_FAMILY==I960 */

#define	sotorawcb(so)		((struct rawcb *)(so)->so_pcb)

/*
 * Nominal space allocated to a raw socket.
 */
#define	RAWSNDQ		8192
#define	RAWRCVQ		8192

#ifndef VIRTUAL_STACK
extern 	struct rawcb rawcb;			/* head of list */
#endif

extern	int	 raw_attach (struct socket *, int);
extern	void	 raw_ctlinput (int, struct sockaddr *);
extern	void	 raw_detach (struct rawcb *);
extern	void	 raw_disconnect (struct rawcb *);
extern	void	 raw_init (void);
extern	void	 raw_input (struct mbuf *, struct sockproto *, 
			    struct sockaddr *, struct sockaddr *);
extern	int	 raw_usrreq (struct socket *, int, struct mbuf *, 
			     struct mbuf *, struct mbuf *);

#ifdef __cplusplus
}
#endif

#endif /* __INCraw_cbh */
