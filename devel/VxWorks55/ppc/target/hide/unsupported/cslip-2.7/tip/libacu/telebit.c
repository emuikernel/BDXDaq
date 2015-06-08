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
static char rcsid[] =
    "@(#) $Header: telebit.c,v 1.19 92/12/12 20:58:28 leres Exp $ (LBL)";
#endif

#include "tip.h"
#include <sys/time.h>

static char NAME[] = "telebit";

/* result codes */ ;
static char RRING[] = "\r\nRRING\r\n";
static char CONNECT[] = "\r\nCONNECT";	/* generic prefix */

#define	CMD_DELAY	5	/* number of seconds to wait for response
				 * while in command mode */
#define DIAL_DELAY	120	/* number of seconds to wait for response
				 * after sending dial string */
#define WRITE_DELAY	40	/* ms between each character written to modem */
#define	MAXSTR	256

void
tb_disconnect()
{

	(void) close(FD);
}

void
tb_abort()
{

	tb_disconnect();
}

static void
tberrputc(c)
	char c;
{

	if (c & ~0177) {
		putc('M', stderr);
		putc('-', stderr);
		c &= 0177;
	}
	switch (c) {

	case '\r':
		putc('\\', stderr);
		putc('r', stderr);
		break;

	case '\n':
		putc('\\', stderr);
		putc('n', stderr);
		break;

	case '\t':
		putc('\\', stderr);
		putc('t', stderr);
		break;

	default:
		if (c < 0x20) {
			putc('^', stderr);
			putc(c + 0x40, stderr);
		} else
			putc(c, stderr);
	}
	(void) fflush(stderr);
}

static void
tberrputs(s)
	register char *s;
{

	while (*s)
		tberrputc(*s++);
}

/* Trim leading and trailing white space */
static char *
tbpretty(s)
	register char *s;
{
	register char *cp;

	cp = s;
	cp += strlen(s) - 1;
	while (cp > s && isspace(*cp & 0177))
		*cp-- = '\0';
	cp = s;
	while (isspace(*cp & 0177))
		cp++;
	return(cp);
}

static void
tbputc(c)
	char c;
{
	struct timeval t;

	if (write(FD, &c, 1) != 1) {
		perror("tip: tbputc: write error");
		return;
	}
	if (debug)
		tberrputc(c);
	t.tv_sec = 0;
	t.tv_usec = WRITE_DELAY * 1000;
	(void) select(32, 0, 0, 0, &t);
}

static void
tbwrite(cp)
	register char *cp;
{

	if (debug) {
		(void) fprintf(stderr, "-> \"");
		(void) fflush(stderr);
	}
	while (*cp != '\0')
		tbputc(*cp++);
	if (debug) {
		(void) fprintf(stderr, "\"\n");
		(void) fflush(stderr);
	}
}

static char
tbgetc(timeout)
	unsigned int timeout;
{
	int n;
	char c;
	struct timeval t;
	int readfds;

	t.tv_sec = timeout;
	t.tv_usec = 0;
	readfds = 1 << FD;
	if ((n = select(FD+1, &readfds, 0, 0, &t)) <= 0) {
		if (n < 0)
			perror("tbgetc: select");
		return (0);
	}
	if ((n = read(FD, &c, 1)) <= 0) {
		perror("tbgetc: read");
		return (0);
	}
	if (debug)
		tberrputc(c);
	return (c);
}

/*
 * Read a line from the telebit. We assume a line will begin
 * with a carriage return/line feed and end with line feed.
 */
static int
tbread(buf, timeout, max)
	char *buf;
	unsigned int timeout, max;
{
	char *cp = buf;
	char c;
	int ret = 1;

	if (debug) {
		(void) fprintf(stderr, "<- \"");
		(void) fflush(stderr);
	}
	*buf = '\0';

	/* get the initial \r \n */
	if ((*cp++ = tbgetc(timeout)) == 0) {
		ret = 0;
		goto bail;
	}
	if ((*cp++ = tbgetc(timeout)) == 0) {
		ret = 0;
		goto bail;
	}

	/* grab everything up to the next \n */
	while (cp < &buf[max - 2] && (*cp++ = c = tbgetc(timeout)) != '\n')
		if (c == 0) {
			ret = 0;
			goto bail;
		}
	*cp = '\0';

bail:
	if (debug) {
		(void) fprintf(stderr, "\"\n");
		(void) fflush(stderr);
	}
	return(ret);
}

static int
tbset(tosend, buf, num, errmsg)
	char *tosend, *buf, *num, *errmsg;
{
	tbwrite(tosend);
	tbread(buf, CMD_DELAY, MAXSTR);
	if (strcmp(tbpretty(buf), "OK") != 0) {
		(void) printf(" %s modem %s\n", NAME, errmsg);
		(void) fflush(stdout);
#ifdef ACULOG
		logent(value(HOST), num, NAME, errmsg);
#endif
		return (1);
	}
	return (0);
}

tb_dialer(num, acu)
	register char *num;
	char *acu;
{
	int i;
	char buf[MAXSTR];
	register char *cp;
	int verbose = boolean(value(VERBOSE));
	char c;

	/*
	 * Make sure that we get a HUP if anything goes wrong then turn
	 * off echo (in case it's on) to make our response parsing easier.
	 */
	if (verbose) {
		(void) printf("diddling dialer... ");
		if (debug)
			putchar('\n');
		(void) fflush(stdout);
	}
	if (ioctl(FD, TIOCHPCL, 0) < 0)
		perror("tip: TIOCHPCL ioctl");
	tbwrite("\rATE0\r");
	if (debug) {
		(void) fprintf(stderr, "<- \"");
		(void) fflush(stderr);
	}
	while ((c = tbgetc(CMD_DELAY)) != 'O')
		if (c == 0) {
			(void) fprintf(stderr, "\"\ntelebit not responding\n");
			(void) fflush(stderr);
			return (0);
		}
	while ((c = tbgetc(CMD_DELAY)) != '\n')
		if (c == 0) {
			(void) fprintf(stderr,
			    "\"\ntelebit not responding (2)\n");
			(void) fflush(stderr);
			return (0);
		}
	if (debug) {
		(void) fprintf(stderr, "\"\n");
		(void) fflush(stderr);
	}

	/* See if we can figure out what we're dealing with */
	i = -1;
	tbwrite("\rATI\r");
	if (!tbread(buf, CMD_DELAY, sizeof(buf)) ||
	    (cp = tbpretty(buf)) == 0 ||
	    *cp == '\0' ||
	    (i = atoi(cp)) == 0 ||
	    !tbread(buf, CMD_DELAY, sizeof(buf)) ||
	    strcmp(tbpretty(buf), "OK") != 0) {
		(void) fprintf(stderr, "Can't determine telebit model %d\n", i);
		(void) fflush(stderr);
		return(0);
	}
	switch (i) {

	case 965:		/* T1600, T3000, WorldBlazer */
	    cp = "\rATQ0 X1\r";
	    break;

	case 968:		/* T1000 */
	case 971:		/* T2500 */
	default:
	    cp = "\rAT~Q0 X3 W1\r";
	    break;
	}
	if (debug > 1)
		(void) printf("[modem type %d]\n", i);

	/* Setup a few things, bail if any fail */
	if (tbset(cp, buf, num, "telebit result codes not set"))
		return (0);

	/* if the "phone number" doesn't start with a digit, assume it's
	 * a command string to go to the telebit */
	if (verbose) {
		(void) printf("dialing... ");
		if (debug)
			putchar('\n');
		(void) fflush(stdout);
	}
	if (isdigit(*num))
		(void) sprintf(buf, "ATD%s\r", num);
	else
		(void) sprintf(buf, "AT%s\r", num);
	tbwrite(buf);

	while (1) {
		if (tbread(buf, DIAL_DELAY, sizeof(buf)) == 0) {
			(void) strcpy(buf, "timeout on dial");
#ifdef ACULOG
			logent(value(HOST), num, NAME, buf);
#endif
			return (0);
		}
		if (strncmp(buf, RRING, strlen(RRING)))
			break;
		if (verbose) {
			(void) printf("ringing...");
			if (debug)
				putchar('\n');
			(void) fflush(stdout);
		}
	}

	if (strncmp(buf, CONNECT, strlen(CONNECT)) != 0) {
		char buf2[MAXSTR];

		(void) sprintf(buf2, "dial failed (%s)", tbpretty(buf));
		(void) printf("%s", buf2);
		(void) fflush(stdout);
#ifdef ACULOG
		logent(value(HOST), num, NAME, buf2);
#endif
		return (0);
	}
	/*
	 * We assume the modem has lock speed set so we don't care
	 * what the connect speed was.
	 */
	ttysetup(speed(BR));
	if (verbose) {
		(void) printf("(%s)", tbpretty(buf));
		if (debug)
			putchar('\n');
		(void) fflush(stdout);
	}
	i = 2;
	if (ioctl(FD, TIOCFLUSH, &i) < 0)
		perror("tip: TIOCFLUSH ioctl");
	return (1);
}
