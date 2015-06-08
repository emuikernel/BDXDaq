/*
 * Copyright (c) 1989 The Regents of the University of California.
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
 */

#ifndef lint
static char rcsid[] =
    "@(#) $Header: page.c,v 1.9 92/12/12 20:57:48 leres Exp $ (LBL)";
#endif

/*
 *	tip/page.c
 *
 *	This file contains code to send alphanumeric pages via
 *	dialup paging services that support "Remote Entry Device
 *	Protocol" (aka "IXO").  These services typically send to
 *	devices like the Motorola PMR-2000 pager.
 *
 *	This module is designed to use the ACK/NAK packet protocol
 *	to ensure the highest likelyhood of correct delivery of the
 *	message.  The packets are checksummed and all messages are
 *	acknowledged by the receiver.
 *
 *	To use this facility, tip must be called with the -p option,
 *	a system name (what system to dial up) and finally a data
 *	string which contains 3 items:  A login code/password, a list
 *	pager id's to receive the message, and the message text.
 *	The three fields are separated by tabs.  The pager id field
 *	is further subdivided into a list of pager ids by commas.
 *
 *	For example:
 *	tip -p metro "12345678\t20001,20002\tPlease call the office."
 *
 *	Would call the system called "metro" as listed in /etc/remote
 *	and login with the ID 12345678.  The message "Please call
 *	the office." would be sent to pagers 20001 and 20002.
 *
 *	This code has been tested only on the Metromedia Paging Services
 *	system around New York City, though it should work on other
 *	similiar systems.
 */

#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <sys/time.h>
#include "tip.h"

#define STX	002	/* ^B */
#define	ETX	003	/* ^C */
#define	EOT	004	/* ^D */
#define	ACK	006	/* ^F */
#define	NAK	025	/* ^U */
#define	ETB	027	/* ^W */
#define	ESC	033	/* ^[ */
#define	RS	036	/* ^^ */

#define WRITE_DELAY 40		/* ms between each character written to modem */

#ifdef notdef
extern jmp_buf	jmpbuf;
#endif

extern char *strchr();
extern int recvtimeout();

char	inbuf[270];
#ifdef notdef
char	mbuf[32];
#endif
char	packet[270];

static char *addstr();

static int pgrecv();
static int pgsend();
static int lread();
static int lputc();
static void errputs();
static void errputc();

sendpage(data)
char *data;
{
	int ret = 1;
	int trys, i;
	char *cp, *pager, *message;
	char *cp2;
	char buf[65 + 2];		/* 65 char max sized msg + CR + EOS */

	if (boolean(value(VERBOSE)))
		vflag++;
#ifdef notdef
#endif
	if (vflag)
		fputs("[Connected]\n", stderr);

	cp = data;
	if (pager = strchr(data, '\t')) {
		*pager = 0;
		pager++;
	} else {
		fprintf(stderr, "Bad paging data: '%s'\n", data);
		return(1);
	}

	trys = 0;
	do {
		if (trys++ > 3) {
			fprintf(stderr, "ID= retries failed\n");
			goto done;
		}
		if (pgsend("\r"))
			return(1);
	} while (pgrecv("ID=", 5) < 3);

	trys = 0;
login_again:

	(void)sprintf(buf, "\033%s\r", cp);
	if (pgsend(buf))
		return(1);

	/* Check what's echoed back */
	cp2 = buf;
	++cp2;		/* step over escape */
	strcat(cp2, "\n\r");
	if (pgrecv(cp2, 10) <= 0) {
		if (trys++ < 4)
			goto login_again;
		else
			goto done;
	}

	/* Wait for ACK, NAK, EOT or Ready(?) */
	if (pgrecv("\r", 10) <= 0) {
		fprintf(stderr, "login timeout\n");
		goto done;
	}

	switch(inbuf[0]) {

	default:	if (trys++ < 4) goto login_again; else goto done;
	case ACK:	break;
	case NAK:	if (trys++ < 4) goto login_again; else goto done;
	case RS:	fprintf(stderr, "login got RS\n"); goto done;
	case ESC:
		switch(inbuf[1]) {
		case EOT:	goto done;
		case '[':	if(inbuf[2] == 'p') break;
		default:	fprintf(stderr, "got unknown ESC code\n");
				goto login_again;
		}
	}
	pgrecv("\r", 10);	/* Swallow version greeting */
	pgrecv("\r", 10);	/* Swallow Go Ahead <ESC>[p */

	if (message = strchr(pager, '\t')) {
		*message = 0;
		message++;
	} else
		message = "[Message not specified]";
next_pager:
	if (cp = strchr(pager, ','))
		*cp++ = '\0';
	buildpkt(pager, message);

	trys = 0;
resend:
	if (pgsend(packet))
		return(1);

	pgrecv("\r", 10);	/* Swallow page id echo */

#ifdef notdef
	/* ??? */
	pgrecv("^C\r\n", 10);	/* ^C\r\n */
#endif

	/* Wait for ACK, NAK, EOT or Ready(?) */
again:
	if ((i = pgrecv("\r", 10)) <= 0) {
		fprintf(stderr, "page send no reply\n");
		goto done;
	}
	switch (inbuf[0]) {

	default:
		fprintf(stderr, "page send reply error\n");
		goto done;

	case ACK:	
		ret = 0;
		pgrecv("\r", 10);	/* Swallow page placed message */
		pgrecv("\r", 10);	/* Swallow thank you */
		goto again;
		/* goto sent; */

	case NAK:
		if (trys++ < 4)
			goto resend;
		else
			goto done;
	case RS:
		fprintf(stderr, "page send got RS\n");
		goto done;

	case ESC:
		switch (inbuf[1]) {

		case EOT:
			goto done;

		case '[':
			if(inbuf[2] == 'p') {
				ret = 1;
				goto sent;
			}
			/* Fall through */

		default:
			fprintf(stderr, "got unknown ESC code (2) \"");
			errputs(inbuf);
			fprintf(stderr, "\"\n");
			goto done;
		}
	}
sent:
	if (cp) {
		pager = cp;
		goto next_pager;
	}

done:
	/* Logout: EOT CR */
	if (pgsend("\004\r"))
		return(ret);
	pgrecv("\r", 3);
	fputs(ret ? "[Error in delivery]\n" : inbuf, stderr);
	pgrecv("\r", 3);
	return(ret);
}

static int
pgsend(cp)
	register char *cp;
{
	register char c;
	struct timeval t;

	if (debug)
		fputs("-> \"", stderr);
	t.tv_sec = 0;
	t.tv_usec = WRITE_DELAY * 1000;
	while (*cp) {
		c = *cp++;
		if (lputc(c) != 1)
			return(-1);
		if (debug)
			errputc(c);
		(void) select(32, 0, 0, 0, &t);
	}
	if (debug)
		fputs("\"\n", stderr);
	return(0);
}

static int
pgrecv(want, secs)
	char *want;
	int secs;
{
	register int i, j, n, size;
int cnt;
char *inbufp = inbuf;
char buf[sizeof(inbuf)];
	char *endstr = "\"\n";
	cnt = 0;
	
	size = strlen(want);
	if (size >= sizeof(buf)) {
		(void)fprintf(stderr, "pgrecv: size too big  (%d >= %d)",
		    size, sizeof(buf));
		return(-1);
	}
	if (debug) {
		fputs("<? \"", stderr);
		errputs(want);
		fputs("\"\n<- \"", stderr);
	}
	for (i = 0; i < size; ) {
		/* Read as much as we can, handle timeout and errors */
		if ((n = lread(FD, &buf[i], size - i, (u_int)secs)) < 0) {
			if (debug)
				fputs(endstr, stderr);
			(void)fprintf(stderr, "pgrecv: lread(1): %s\n",
			    strerror(errno));
			return(-1);
		} else if (n == 0) {
			if (debug)
				fputs(endstr, stderr);
#ifdef notdef
			fprintf(stderr, "pgrecv: timeout (1)");
#endif
			return(0);
		}
		cnt += n;

		/* Gag, ANYP is all it's cracked up to be */
		for (j = i; j < i + n; ++j) {
			buf[j] &= 0177;
*inbufp++ = buf[j];
		}
		if (debug)
			for (j = i; j < i + n; ++j)
				errputc(buf[j]);
		i += n;
		buf[i] = '\0';
*inbufp = '\0';
	}
	i = size - 1;
	while (strncmp(buf, want, size)) {
		bcopy(buf + 1, buf, size);
		if ((n = lread(FD, &buf[i], 1, (u_int)secs)) < 0) {
			if (debug)
				fputs(endstr, stderr);
			(void)fprintf(stderr, "pgrecv: lread(2): %s\n",
			    strerror(errno));
			return(-1);
		} else if (n == 0) {
			if (debug)
				fputs(endstr, stderr);
#ifdef notdef
			fprintf(stderr, "pgrecv: timeout (2)");
#endif
			return(0);
		}
		cnt += n;
		buf[i] &= 0177;
*inbufp++ = buf[i];
		if (debug)
			errputc(buf[i]);
	}
	if (debug)
		fputs(endstr, stderr);

*inbufp++ = '\0';
#ifdef notdef
fprintf(stderr, "\rpgrecv: cnt is %d, size is %d, inbuf '", cnt, size);
errputs(inbuf);
fprintf(stderr, "'\n");
#endif
	return(cnt);
}


buildpkt(pager, message)
char *pager, *message;
{
	char	*cp = &packet[0];
	int	sum;

	*cp++ = STX;
	sum = STX;

	cp = addstr(pager, &sum, cp);
	cp = addstr(message, &sum, cp);

	*cp++ = ETX;
	sum += ETX;
	*cp++ = '0' + ((sum>>8)&0xf);
	*cp++ = '0' + ((sum>>4)&0xf);
	*cp++ = '0' + (sum&0xf);
	*cp++ = '\r';
	*cp = '\0';
}

static char *
addstr(s, sump, cp)
char *s, *cp;
int *sump;
{
	char c;
	int sum = *sump;

	while(c = *s++) {
		*cp = (isprint(c) ? c : '?');
		sum += *cp;
		cp++;
	}
	*cp++ = '\r';
	sum += '\r';

	*sump = sum;
	return(cp);
}

static int
lread(f, buf, len, secs)
	int f;
	char *buf;
	int len;
	u_int secs;
{
	register int n;
	struct timeval t;
	int readfds;

	t.tv_sec = secs;
	t.tv_usec = 0;
	readfds = 1 << FD;
	if ((n = select(f + 1, &readfds, 0, 0, &t)) <= 0)
		return(n);
	return(read(f, buf, len));
}

static int
lputc(c)
	char c;
{
	struct timeval t;

	if (write(FD, &c, 1) != 1)
		return(-1);
	t.tv_sec = 0;
	t.tv_usec = WRITE_DELAY * 1000;
	(void) select(32, 0, 0, 0, &t);
	return(1);
}

static void
errputs(s)
	register char *s;
{
	while (*s)
		errputc(*s++);
}

static void
errputc(c)
	char c;
{
	if (c & ~ 0177) {
		putc('M', stderr);
		putc('-', stderr);
		c &= 0177;
	}
	if (c < 0x20 || c == 0177) {
		if (c == '\b') {
			putc('\\', stderr);
			putc('b', stderr);
		} else if (c == '\f') {
			putc('\\', stderr);
			putc('f', stderr);
		} else if (c == '\n') {
			putc('\\', stderr);
			putc('n', stderr);
		} else if (c == '\r') {
			putc('\\', stderr);
			putc('r', stderr);
		} else if (c == '\t') {
			putc('\\', stderr);
			putc('t', stderr);
		} else {
			putc('^', stderr);
			putc(c ^ 0x40, stderr);	/* DEL to ?, others to alpha */
		}
	} else
		putc(c, stderr);
}
