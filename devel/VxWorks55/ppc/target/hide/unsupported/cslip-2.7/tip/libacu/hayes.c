#ifndef lint
static char rcsid[] =
    "@(#) $Header: hayes.c,v 1.6 92/12/12 20:58:32 leres Exp $ (LBL)";
#endif
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
static char sccsid[] = "@(#)hayes.c	5.2 (Berkeley) 9/13/88";
#endif /* not lint */

/*
 * Routines for calling up on a Hayes Modem
 * (based on the old VenTel driver).
 * The modem is expected to be strapped for "echo".
 * Also, the switches enabling the DTR and CD lines
 * must be set correctly.
 * NOTICE:
 * The easy way to hang up a modem is always simply to
 * clear the DTR signal. However, if the +++ sequence
 * (which switches the modem back to local mode) is sent
 * before modem is hung up, removal of the DTR signal
 * has no effect (except that it prevents the modem from
 * recognizing commands).
 * (by Helge Skrivervik, Calma Company, Sunnyvale, CA. 1984)
 */
/*
 * TODO:
 * It is probably not a good idea to switch the modem
 * state between 'verbose' and terse (status messages).
 * This should be kicked out and we should use verbose
 * mode only. This would make it consistent with normal
 * interactive use thru the command 'tip dialer'.
 */

#include "tip.h"
#include <sys/time.h>


static char MDM_OK[] = "\nOK\r";
static char RRING[] = "RRING\r\n";
static char CONNECT[] = "CONNECT";	/* generic prefix */

#define	CMD_DELAY	5	/* number of seconds to wait for response
				 * while in command mode */
#define DIAL_DELAY	120	/* number of seconds to wait for response
				 * after sending dial string */
#define WRITE_DELAY	40	/* ms between each character written to modem */
#define	MAXSTR	256

#define	min(a,b)	((a < b) ? a : b)

static	int sigALRM();
static	int timeout = 0;
static	jmp_buf timeoutbuf;
static	char gobble();

#define	DIALING		1
#define IDLE		2
#define CONNECTED	3
#define	FAILED		4
static	int state = IDLE;

static void hywrite();
static void hyerrputc();
static void hyerrputs();
static int hyread();
static char hygetc();

hay_dialer(num, acu)
	register char *num;
	char *acu;
{
	register char *cp;
	register int connected = 0;
	char dummy;
#ifdef ACULOG
	char line[80];
#endif
	if (hay_sync() == 0)		/* make sure we can talk to the modem */
		return(0);
	if (boolean(value(VERBOSE))) {
		(void) printf("dialing... ");
		if (debug)
			putchar('\n');
		(void) fflush(stdout);
	}
	fflush(stdout);
	ioctl(FD, TIOCHPCL, 0);
	ioctl(FD, TIOCFLUSH, 0);	/* get rid of garbage */
	hywrite("ATv0\r");	/* tell modem to use short status codes */
	gobble("\r");
	gobble("\r");
	hywrite("ATTD");	/* send dial command */
	hywrite(num);
	state = DIALING;
	hywrite("\r");
	connected = 0;
	if (gobble("\r")) {
		if ((dummy = gobble("012345")) != '1' && dummy != '5')
			error_rep(dummy);
		else
			connected = 1;
	}
	if (connected)
		state = CONNECTED;
	else {
		state = FAILED;
		return (connected);	/* lets get out of here.. */
	}
	ioctl(FD, TIOCFLUSH, 0);
#ifdef ACULOG
	if (timeout) {
		sprintf(line, "%d second dial timeout",
			number(value(DIALTIMEOUT)));
		logent(value(HOST), num, "hayes", line);
	}
#endif
	if (timeout)
		hay_disconnect();	/* insurance */
	return (connected);
}


hay_disconnect()
{
	char c;
	int len, rlen;

	/* first hang up the modem*/
#ifdef DEBUG
	printf("\rdisconnecting modem....\n\r");
#endif
	ioctl(FD, TIOCCDTR, 0);
	sleep(1);
	ioctl(FD, TIOCSDTR, 0);
	goodbye();
}

hay_abort()
{

	char c;

	hywrite("\r");	/* send anything to abort the call */
	hay_disconnect();
}

static int
sigALRM()
{

	printf("\07timeout waiting for reply\n\r");
	timeout = 1;
	longjmp(timeoutbuf, 1);
}

static char
gobble(match)
	register char *match;
{
	char c;
	int (*f)();
	int i, status = 0;
	int timeout = number(value(DIALTIMEOUT));

	if (debug) {
		fputs("<? \"", stderr);
		hyerrputs(match);
		fputs("\"\n<- \"", stderr);
		(void) fflush(stderr);
	}
	do {
		c = hygetc(timeout);
		if (c == 0)
			break;
		c &= 0177;
		for (i = 0; i < strlen(match); i++)
			if (c == match[i])
				status = c;
	} while (status == 0);
	if (debug) {
		(void) fprintf(stderr, "\"\n");
		(void) fflush(stderr);
	}
	return (status);
}

error_rep(c)
	register char c;
{
	printf("\n\r");
	switch (c) {

	case '0':
		printf("OK");
		break;

	case '1':
		printf("CONNECT");
		break;
	
	case '2':
		printf("RING");
		break;
	
	case '3':
		printf("NO CARRIER");
		break;
	
	case '4':
		printf("ERROR in input");
		break;
	
	case '5':
		printf("CONNECT 1200");
		break;
	
	default:
		printf("Unknown Modem error: %c (0x%x)", c, c);
	}
	printf("\n\r");
	return;
}

/*
 * set modem back to normal verbose status codes.
 */
goodbye()
{
	int len, rlen;
	char c;

	ioctl(FD, TIOCFLUSH, &len);	/* get rid of trash */
/* XXX should really try to make this code work */
#ifdef notdef
	if (hay_sync()) {
		sleep(1);
#ifndef DEBUG
		ioctl(FD, TIOCFLUSH, 0);
#endif
		hywrite("ATH0\r");		/* insurance */
#ifndef DEBUG
		c = gobble("03");
		if (c != '0' && c != '3') {
			printf("cannot hang up modem\n\r");
			printf("please use 'tip dialer' to make sure the line is hung up\n\r");
		}
#endif
		sleep(1);
		ioctl(FD, FIONREAD, &len);
#ifdef DEBUG
		printf("goodbye1: len=%d -- ", len);
		rlen = read(FD, dumbuf, min(len, DUMBUFLEN));
		dumbuf[rlen] = '\0';
		printf("read (%d): %s\r\n", rlen, dumbuf);
#endif
		hywrite("ATv1\r");
		sleep(1);
#ifdef DEBUG
		ioctl(FD, FIONREAD, &len);
		printf("goodbye2: len=%d -- ", len);
		rlen = read(FD, dumbuf, min(len, DUMBUFLEN));
		dumbuf[rlen] = '\0';
		printf("read (%d): %s\r\n", rlen, dumbuf);
#endif
	}
	ioctl(FD, TIOCFLUSH, 0);	/* clear the input buffer */
	ioctl(FD, TIOCCDTR, 0);		/* clear DTR (insurance) */
	close(FD);
#endif
}

#define MAXRETRY	5

hay_sync()
{
	int len, retry = 0;
	char buf[32];

	while (retry++ <= MAXRETRY) {
		hywrite("AT\r");
#ifdef notdef
		sleep(1);
#endif
		if (hyread(buf, CMD_DELAY, sizeof(buf)) &&
		    strcmp(buf, "AT\r") == 0 &&
		    hyread(buf, CMD_DELAY, sizeof(buf))) {
			/* Still in terse mode */
			if (strcmp(buf, "0\r") == 0)
				return(1);
			/* Must be in verbose mode */
			if (strcmp(buf, "\r") == 0 &&
			    hyread(buf, CMD_DELAY, sizeof(buf)) &&
			    strcmp(buf, MDM_OK) == 0)
				return(1);
		}
		ioctl(FD, TIOCCDTR, 0);
		ioctl(FD, TIOCSDTR, 0);
	}
	printf("Cannot synchronize with hayes...\n\r");
	return(0);
}

static void
hyerrputc(c)
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
hyerrputs(s)
	register char *s;
{

	while (*s)
		hyerrputc(*s++);
}


static void
hyputc(c)
	char c;
{
	struct timeval t;

	if (write(FD, &c, 1) != 1) {
		perror("tip: hyputc: write error");
		return;
	}
	if (debug)
		hyerrputc(c);
	t.tv_sec = 0;
	t.tv_usec = WRITE_DELAY * 1000;
	(void) select(32, 0, 0, 0, &t);
}

static void
hywrite(cp)
	register char *cp;
{

	if (debug) {
		(void) fprintf(stderr, "-> \"");
		(void) fflush(stderr);
	}
	while (*cp != '\0')
		hyputc(*cp++);
	if (debug) {
		(void) fprintf(stderr, "\"\n");
		(void) fflush(stderr);
	}
}

static char
hygetc(timeout)
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
			perror("hygetc: select");
		return (0);
	}
	if ((n = read(FD, &c, 1)) <= 0) {
		perror("hygetc: read");
		return (0);
	}
	if (debug)
		hyerrputc(c);
	return (c);
}

/*
 * Read a line from the telebit. We assume a line will end
 * with carriage return.
 */
static int
hyread(buf, timeout, max)
	char *buf;
	unsigned int timeout, max;
{
	char *cp = buf;
	char c;
	int ret = 1;
	static int failed = 0;

	if (debug) {
		(void) fprintf(stderr, "<- \"");
		(void) fflush(stderr);
	}
	*buf = '\0';

	/* grab everything up to the next \n */
	while (cp < &buf[max - 2] && (*cp++ = c = hygetc(timeout)) != '\r')
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
