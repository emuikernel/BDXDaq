/*
 * Copyright (c) 1988 The Regents of the University of California.
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
static char sccsid[] = "@(#)uucplock.c	5.3 (Berkeley) 9/2/88";
#endif /* not lint */

#include <sys/types.h>
#include <sys/file.h>
#include <sys/dir.h>
#include <errno.h>

/* pick the directory naming scheme you are using */

#define LOCKDIRNAME	"/tmp/LCK..%s"	/**/

extern int slip;

/* 
 * uucp style locking routines
 * return: 0 - success
 * 	  -1 - failure
 */

uu_lock(ttyname)
	char *ttyname;
{
#ifndef NOLOCK
	extern int errno;
	int fd, pid;
	char tbuf[sizeof(LOCKDIRNAME) + MAXNAMLEN];
	off_t lseek();
	register int mode;

	(void)sprintf(tbuf, LOCKDIRNAME, ttyname);
	mode = O_RDWR|O_CREAT;
	if (slip)
		mode = O_RDWR|O_CREAT;
	else
		mode = O_RDWR|O_CREAT|O_EXCL;
	fd = open(tbuf, mode, 0660);
	if (fd < 0) {
		/*
		 * file is already locked
		 * check to see if the process holding the lock still exists
		 */
		fd = open(tbuf, O_RDWR, 0);
		if (fd < 0) {
			perror("lock open");
			return(-1);
		}
		if (read(fd, &pid, sizeof(pid)) != sizeof(pid)) {
			(void)close(fd);
			perror("lock read");
			return(-1);
		}

		if (kill(pid, 0) == 0 || errno != ESRCH) {
			(void)close(fd);	/* process is still running */
			return(-1);
		}
		/*
		 * The process that locked the file isn't running, so
		 * we'll lock it ourselves
		 */
		if (lseek(fd, 0L, L_SET) < 0) {
			(void)close(fd);
			perror("lock lseek");
			return(-1);
		}
		/* fall out and finish the locking process */
	}
	pid = getpid();
	if (write(fd, (char *)&pid, sizeof(pid)) != sizeof(pid)) {
		(void)close(fd);
		(void)unlink(tbuf);
		perror("lock write");
		return(-1);
	}
	(void)close(fd);
#endif
	return(0);
}

uu_unlock(ttyname)
	char *ttyname;
{
#ifndef NOLOCK
	char tbuf[sizeof(LOCKDIRNAME) + MAXNAMLEN];

	(void)sprintf(tbuf, LOCKDIRNAME, ttyname);
	return(unlink(tbuf));
#else
	return (0);
#endif
}
