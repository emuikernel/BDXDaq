/* tcp_debug.h - tcp debug header file */

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
 *	@(#)tcp_debug.h	8.1 (Berkeley) 6/10/93
 */

/*
modification history
--------------------
01a,03mar96,vin  created from BSD4.4 stuff, integrated with 02l of tcp_debug.h
*/


#ifndef __INCtcp_debugh
#define __INCtcp_debugh

#ifdef __cplusplus
extern "C" {
#endif

/* includes  */

#include "tcp_var.h"
#include "tcpip.h"

#define TCP_NDEBUG		100	/* length of debug info array */
#define TCP_DEBUG_NUM_DEFAULT	20	/* default display number */

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif	/* CPU_FAMILY==I960 */

struct	tcp_debug {
	n_time	td_time;
	short	td_act;
	short	td_ostate;
	caddr_t	td_tcb;
	caddr_t	td_tiphdr;
	struct	tcpiphdr td_ti;
	struct	tcpcb td_cb;
	short	td_req;
};

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 0                 /* turn off alignment requirement */
#endif	/* CPU_FAMILY==I960 */

#define	TA_INPUT 	0
#define	TA_OUTPUT	1
#define	TA_USER		2
#define	TA_RESPOND	3
#define	TA_DROP		4

#ifdef TANAMES
char	*tanames[] =
    { "input", "output", "user", "respond", "drop" };
#endif

IMPORT VOIDFUNCPTR tcpTraceRtn;
IMPORT VOIDFUNCPTR tcpReportRtn;

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern void	tcpTraceInit (void);

#else	/* __STDC__ */

extern void	tcpTraceInit ();

#endif	/* __STDC__ */
 
#ifdef __cplusplus
}
#endif

#endif /* __INCtcp_debugh */
