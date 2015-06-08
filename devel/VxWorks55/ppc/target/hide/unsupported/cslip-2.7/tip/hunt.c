/*
 * Copyright (c) 1983 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that: (1) source distributions retain this entire copyright
 * notice and comment, and (2) distributions including binaries display
 * the following acknowledgement:  ``This product includes software
 * developed by the University of California, Berkeley and its contributors''
 * in the documentation or other materials provided with the distribution
 * and in all advertising materials mentioning features or use of this
 * software. Neither the name of the University nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef lint
static char sccsid[] = "@(#)hunt.c	5.4 (Berkeley) 9/2/88";
#endif /* not lint */

#include "tip.h"

extern char *getremote();
extern char *rindex();

static	jmp_buf deadline;
static	int deadfl;

char uucplock[BUFSIZ];

dead()
{

	deadfl = 1;
	longjmp(deadline, 1);
}

hunt(name)
	char *name;
{
	register char *cp, *cf, *ct;
	sigfunc_t (*f)();

	f = signal(SIGALRM, dead);
	while (cp = getremote(name)) {
		/*
		 * The name of the lock is pretty important. We don't
		 * want to introduce the following security problems:
		 * 1. Namecollision in lock file between different devices
		 * 2. uucplock is used to decide whether or not to call
		 *    acucntrl. Thus, non-/dev devices should have a different
		 *    lock name than devices in /dev.
		 */
		if (*cp != '/')	/* device name must be absolute path */
			continue;
		while (*cp == '/') cp++;
		cp--;
		if (strncmp("/dev/", cp, 5) == 0) {
			cf = cp + 5;
			while (*cf == '/') cf++;
		} else
			cf = cp;
		ct = uucplock;
		while (*cf) {
			if (*cf == '/') {
				if (ct == uucplock
				    || (ct > uucplock && *(ct-1) != '_'))
					*ct++ = '_';
			} else
				*ct++ = *cf;
			cf++;
		}
		*ct = '\0';
		/*
		 * At this point we are guranteed that any devices outside
		 * /dev will have a lock name starting with underscore (_),
		 * and that any slashes (/) in the pathname have been changed
		 * to underscores, and that multiple slashes are only made
		 * into a single underscore. Thus, any way of expressing
		 * a pathname will lead to a unique lock file name.
		 */
		deadfl = 0;
		if (uu_lock(uucplock) < 0)
			continue;
		/*
		 * Straight through call units, such as the BIZCOMP,
		 * VADIC and the DF, must indicate they're hardwired in
		 *  order to get an open file descriptor placed in FD.
		 * Otherwise, as for a DN-11, the open will have to
		 *  be done in the "open" routine.
		 */
		if (!HW)
			break;
		if (setjmp(deadline) == 0) {
			alarm(10);
			FD = open(cp, O_RDWR);
		}
		alarm(0);
		if (FD < 0) {
			perror(cp);
			deadfl = 1;
		}
		if (!deadfl) {
			if (!slip)
				ioctl(FD, TIOCEXCL, 0);
			ioctl(FD, TIOCHPCL, 0);
			signal(SIGALRM, SIG_DFL);
			return ((int)cp);
		}
		(void)uu_unlock(uucplock);
	}
	signal(SIGALRM, f);
	return (deadfl ? -1 : (int)cp);
}
