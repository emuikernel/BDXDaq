/* inet.h - Internet header */

/* Copyright 1984 - 1999 Wind River Systems, Inc. */

/*
 * Copyright (c) 1983 Regents of the University of California.

 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *      @(#)inet.h      5.2 (Berkeley) 6/27/88
 */

/*
modification history
--------------------
01g,26jul99,spm  added ansi/c++ prototypes for routines (SPR #5307)
01f,22sep92,rrr  added support for c++
01e,26may92,rrr  the tree shuffle
01d,04oct91,rrr  passed through the ansification filter
		  -changed copyright notice
01c,05oct90,shl  added copyright notice.
		 added title.
01b,16apr89,gae  updated to new 4.3BSD.
01a,22dec86,rdc  created.
*/

#ifndef __INCineth
#define __INCineth

#ifdef __cplusplus
extern "C" {
#endif

/*
 * External definitions for
 * functions in inet(3N)
 */

#if defined(__STDC__) || defined(__cplusplus)

extern 	unsigned long 	inet_addr (char *);
extern 	char * 		inet_ntoa (struct in_addr);
extern 	struct in_addr 	inet_makeaddr (int, int);
extern 	unsigned long 	inet_network (char *);

#else

unsigned long inet_addr();
char    *inet_ntoa();
struct  in_addr inet_makeaddr();
unsigned long inet_network();

#endif /* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCafh */
