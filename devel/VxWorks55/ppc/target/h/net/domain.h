/* domain.h - domain header file */

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
 *	@(#)domain.h	8.1 (Berkeley) 6/2/93
 */

/*
 * Structure per communications domain.
 */

/*
modification history
--------------------
01b,07jul97,vin  added declaration for addDomain() and domaininit().
01a,03mar96,vin  created from BSD4.4 stuff.
*/

#ifndef __INCdomainh
#define __INCdomainh

#ifdef __cplusplus
extern "C" {
#endif

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif  /* CPU_FAMILY==I960 */

/*
 * Structure per communications domain.
 */

struct	domain
    {
    int		dom_family;		/* AF_xxx */
    char	*dom_name;
    int		(*dom_init)();		/* initialize domain data structures */
    int		(*dom_externalize)();	/* externalize access rights */
    int		(*dom_dispose)();	/* dispose of internalized rights */
    struct	protosw *dom_protosw;
    struct	protosw *dom_protoswNPROTOSW;
    struct	domain *dom_next;
    int		(*dom_rtattach)();	/* initialize routing table */
    int		dom_rtoffset;		/* an arg to rtattach, in bits */
    int		dom_maxrtkey;		/* for routing layer */
    };

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 0                 /* turn off alignment requirement */
#endif  /* CPU_FAMILY==I960 */

extern struct domain *domains;

extern void 	domaininit (void);
extern int 	addDomain (struct domain * pDomain);

#ifdef __cplusplus
}
#endif

#endif /* __INCdomainh */
