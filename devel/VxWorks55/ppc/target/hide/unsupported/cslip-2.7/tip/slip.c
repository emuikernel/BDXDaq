/*
 * Copyright (c) 1989 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that: (1) source code distributions
 * retain the above copyright notice and this paragraph in its entirety, (2)
 * distributions including binary code include the above copyright notice and
 * this paragraph in its entirety in the documentation or other materials
 * provided with the distribution, and (3) all advertising materials mentioning
 * features or use of this software display the following acknowledgement:
 * ``This product includes software developed by the University of California,
 * Lawrence Berkeley Laboratory and its contributors.'' Neither the name of
 * the University nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior
 * written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */
#ifndef lint
static char copyright[] =
    "Copyright (c) 1990 Lawrence Berkeley Laboratory\nAll rights reserved.\n";
static char rcsid[] =
    "@(#) $Header: slip.c,v 1.18 91/06/25 19:17:05 leres Exp $ (LBL)";
#endif

/*
 *  tip/slip.c
 *
 *  This file contains the SLIP specific startup code.  It is designed
 *  with sliplogin in mind.  Normally it is used with a CC similar to:
 *
 *	:cc=/etc/sliplogin Sremote:
 */

#include "tip.h"
#include <sys/wait.h>

static jmp_buf jmpbuf;
static sigfunc_t slipabort();

extern char *strerror();

runslip()
{
	register char *cp;
	register int i, j;
	union wait status;
	char *argv[16];
#define MAXARGS (sizeof(argv) / sizeof(*argv))

	if (vflag) {
		fprintf(stderr, "[Invoking slip...]\r\n");
		fflush(stderr);
	}
	if (CC == 0 || *CC == '\0') {
		fprintf(stderr, "[slip: misconfigured, a CC is required]\r\n");
		return(1);
	}

	if (debug) {
		fprintf(stderr, "[\"%s\"]\n", CC);
		fflush(stderr);
	}

	/* Save CC string */
	if ((cp = malloc(strlen(CC) + 1)) == 0) {
		fprintf(stderr, "[slip: malloc failed]\r\n");
		return(1);
	}
	strcpy(cp, CC);

	i = 0;
	for (;;) {
		if (i >= MAXARGS) {
			fprintf(stderr, "[slip: CC has more than %d args]\r\n",
			    MAXARGS);
			return(1);
		}
		argv[i++] = cp;
		if ((cp = index(cp, ' ')) == 0)
			break;
		*cp++ = '\0';
	}
	argv[i] = (char *)0;

	if ((pid = fork()) < 0) {
		fprintf(stderr, "tip: slip fork: ");
		return(1);
	}
	if (pid == 0) {
		/* Child; move device FD to stdin */
		if (FD != 0) {
			close(0);
			if (dup2(FD, 0) < 0) {
				perror("slip dup2");
				exit(1);
			}
			close(FD);
		}

		/* Exec ho! */
		execv(argv[0], argv);
		fprintf(stderr, "tip: slip exec: ");
		perror(argv[0]);
		exit(1);
		/* NOTREACHED */
	}

	/* Catch signal so we can return to caller and clean up */
	signal(SIGHUP, slipabort);
	signal(SIGINT, slipabort);
	signal(SIGQUIT, slipabort);
	if (setjmp(jmpbuf)) {
		fprintf(stderr, "\r\n[Shutting down slip...]\r\n");
		fflush(stderr);
		i = kill(pid, SIGTERM);
		if (i < 0 && errno == EPERM) {
			/* Give sliplogin a chance to change its uid */
			if (debug) {
				fprintf(stderr,
				    "[slip: child kill: %s, retrying",
				    strerror(errno));
				fflush(stderr);
			}
			j = 8;
			do {
				sleep(1);
				i = kill(pid, SIGTERM);
				if (debug) {
					fputs(".", stderr);
					fflush(stderr);
				}
			} while (i < 0 && --j > 0);
			if (debug) {
				if (i >= 0)
					fputs(" success!", stderr);
				fputs("]\r\n", stderr);
				fflush(stderr);
			}
		}
		pid = 0;
		if (i < 0 && errno != ESRCH) {
			fprintf(stderr, "[slip: child kill: %s]\r\n",
			    strerror(errno));
			/* No point in waiting for child if the kill() failed */
			return(1);
		}
	}

	/* Wait for child to exit (or die) */
	while ((i = wait(&status)) >= 0)
		if (WIFEXITED(status))
			return(status.w_retcode);
	if (errno != ECHILD)
		perror("tip: slip wait");
	return(1);
}

static sigfunc_t
slipabort(s)
{

	signal(s, SIG_IGN);
	longjmp(jmpbuf, 1);
}
