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
    "@(#) $Header: login.c,v 1.29 93/08/14 15:21:35 leres Exp $ (LBL)";
#endif

#include "tip.h"
#include <sys/time.h>
#include <ctype.h>
#include <strings.h>

static FILE *fp;
static int line;

static int largc;
static char *largv[10];		/* loginargs()/loginparse() assume args 0-9 */
#define MAXARGS (sizeof(largv) / sizeof(*largv))

static char *loginargs(), *dosend(), *dorecv(), *dogoto();
static int loginparse(), lread(), lputc();
extern char *getpass();
static void errputc(), errputs();

#define LOGIN_DELAY 0x81	/* pause for one second */
#define LOGIN_BREAK 0x82	/* send break */

#define WRITE_DELAY 40		/* ms between each character written to modem */

char *
login()
{
	register int n;
	register char *cp;
	char *cmd, *arg1, *arg2, *arg3;
	static char errmsg[128];

	if (LS == NOSTR)
		return(0);

	/* Split up arguments */
	if (cp = loginargs(LS))
		return(cp);

	if ((fp = fopen(largv[0], "r")) == NULL) {
		(void)sprintf(errmsg, "login: %s: %s",
		    largv[0], strerror(errno));
		return(errmsg);
	}

	line = 0;
	while (n = loginparse(&cmd, &arg1, &arg2, &arg3)) {
		if (equal(cmd, "done")) {
			fclose(fp);
			return(0);
		} else if (equal(cmd, "fail")) {
			fclose(fp);
			return("login failed");
		} else if (equal(cmd, "send")) {
			if (n != 2)
				break;
			if (cp = dosend(arg1, 0)) {
				fclose(fp);
				return(cp);
			}
		} else if (equal(cmd, "sendpass")) {
			if (n != 2)
				break;
			if (cp = dosend(arg1, 1)) {
				fclose(fp);
				return(cp);
			}
		} else if (equal(cmd, "recv")) {
			if (n == 3) {
				/* No label (fall through on timeout) */
				if (cp = dorecv(arg1, "", arg2)) {
					fclose(fp);
					return(cp);
				}
			} else if (n == 4) {
				if (cp = dorecv(arg1, arg2, arg3)) {
					fclose(fp);
					return(cp);
				}
			} else
				break;
		} else if (equal(cmd, "pass")) {
			if (n != 2)
				break;
			if (cp = dosend(getpass(arg1), 1)) {
				fclose(fp);
				return(cp);
			}
		} else if (equal(cmd, "label"))
			continue;
		else if (equal(cmd, "goto")) {
			if (n != 2)
				break;
			if (cp = dogoto(arg1)) {
				fclose(fp);
				return(cp);
			}
		} else
			break;
	}
	if (ferror(fp)) {
		(void)sprintf(errmsg, "login: %s", strerror(errno));
		fclose(fp);
		return(errmsg);
	}
	if (feof(fp)) {
		(void)sprintf(errmsg, "login: %s: premature EOF", LS);
		fclose(fp);
		return(errmsg);
	}

	/* Must be a format error */
	(void)sprintf(errmsg, "login: %s: format error at line %d", LS, line);
	fclose(fp);
	return(errmsg);
}

static char *
dosend(cp, ispass)
	register char *cp;
	int ispass;
{
	register char *cp2, *ret = 0, c;
	struct timeval t;
	static char errmsg[128];

	if (debug) {
		(void)fputs("=> \"", stderr);
		(void)fflush(stderr);
	}
	if (ispass) {
		/* Auto-carriage return for passwords */
		cp2 = cp + strlen(cp) - 1;
		if (cp2 >= cp && *cp2 != '\r') {
			*++cp2 = '\r';
			*++cp2 = '\0';
		}
	}
	t.tv_sec = 0;
	t.tv_usec = WRITE_DELAY * 1000;
	while ((c = *cp) != '\0') {
		if (ispass)
			*cp = '\0';
		++cp;
		if ((unsigned char)c == LOGIN_DELAY)
			sleep(1);
		else if ((unsigned char)c == LOGIN_BREAK)
			genbrk();
		else if (lputc(c) != 1) {
			(void) sprintf(errmsg, "dosend: lputc: %s",
			    strerror(errno));
			ret = errmsg;
			break;
		}
		if (debug) {
			if (ispass && (c != '\r' || *cp != '\0'))
				c = '?';
			errputc(c);
		}
		(void) select(32, 0, 0, 0, &t);
	}
	if (debug) {
		(void)fputs("\"\n", stderr);
		(void)fflush(stderr);
	}
	return(ret);
}

static char *
dorecv(timeout, label, want)
	char *timeout, *label, *want;
{
	register int i, j, n, size, secs;
	register char *ret = 0;
	static char buf[128], lastch;
	char *cstr = "\"\n<= \"";

	size = strlen(want);
	if (size >= sizeof(buf)) {
		(void)sprintf(buf, "dorecv: size too big  (%d >= %d)",
		    size, sizeof(buf));
		ret = buf;
		goto bail;
	}
	if (debug) {
		(void)fputs("<? \"", stderr);
		errputs(want);
		(void)fputs(cstr, stderr);
		(void)fflush(stderr);
	}
	secs = atoi(timeout);
	lastch = -1;
	for (i = 0; i < size; ) {
		/* Read as much as we can, handle timeout and errors */
		if ((n = lread(FD, &buf[i], size - i, (u_int)secs)) < 0) {
			(void)sprintf(buf, "dorecv: lread(1): %s\n",
			    strerror(errno));
			ret = buf;
			goto bail;
		} else if (n == 0) {
			/* Timeout */
			ret = (char *)-1;
			goto bail;
		}

		/* Gag, ANYP is all it's cracked up to be */
		for (j = i; j < i + n; ++j)
			buf[j] &= 0177;
		if (debug)
			for (j = i; j < i + n; ++j) {
				if (lastch == '\n')
					(void)fputs(cstr, stderr);
				errputc(buf[j]);
				lastch = buf[j];
			}
		i += n;
		buf[i] = '\0';
	}
	i = size - 1;
	while (strncmp(buf, want, size)) {
		bcopy(buf + 1, buf, i);
		if ((n = lread(FD, &buf[i], 1, (u_int)secs)) < 0) {
			(void)sprintf(buf, "dorecv: lread(2): %s\n",
			    strerror(errno));
			ret = buf;
			goto bail;
		} else if (n == 0) {
			/* Timeout */
			ret = (char *)-1;
			goto bail;
		}
		buf[i] &= 0177;
		if (debug) {
			if (lastch == '\n')
				(void)fputs(cstr, stderr);
			errputc(buf[i]);
			lastch = buf[i];
		}
	}
bail:
	if (debug) {
		(void)fputs("\"\n", stderr);
		(void)fflush(stderr);
	}
	if (ret == (char *)-1) {
		if (*label) {
			if (debug) {
				(void)fputs("recv: timeout\n", stderr);
				(void)fflush(stderr);
			}
			return(dogoto(label));
		}
		if (debug) {
			(void)fputs("recv: timeout, falling through\n", stderr);
			(void)fflush(stderr);
		}
		ret = 0;
	}
	return(ret);
}

static char *
dogoto(label)
	char *label;
{
	register int n;
	char *cmd, *arg1, *arg2, *arg3;
	static char buf[128];

	if (debug) {
		(void)fprintf(stderr, "goto %s\n", label);
		(void)fflush(stderr);
	}
	rewind(fp);
	line = 0;
	(void)strcpy(buf, label);
	while (n = loginparse(&cmd, &arg1, &arg2, &arg3))
		if (n == 2 && equal(cmd, "label") && equal(arg1, buf))
			break;
	if (ferror(fp)) {
		(void)sprintf(buf, "dogoto: %s", strerror(errno));
		return(buf);
	}
	return(0);
}

/*
 * Split up login script and its arguments
 */
static char *
loginargs(str)
	char *str;
{
	register int i, j;
	register char *cp;
	static char errmsg[128];
	char buf[128];
	char hostname[128];

	/* Get simple hostname */
	(void) gethostname(hostname, sizeof(hostname));
	hostname[sizeof(hostname) - 1] = '\0';
	if (cp = index(hostname, '.'))
		*cp = '\0';

	/* Save string (leave room for hostname expansion) */
	if ((cp = malloc(2 * strlen(str))) == 0)
		return("loginargs(): malloc failed");
	strcpy(cp, str);
	str = cp;

	/* hostname hack */
	cp = str;
	while (cp = index(cp, '%'))
		if (cp[1] == 'h') {
			/* Make room */
			j = strlen(hostname);
			i = strlen(&cp[2]);
			/* Scoot last part over */
			bcopy(&cp[2], &cp[j], i);
			/* Stuff in hostname */
			bcopy(hostname, cp, j);
			/* Step over hostname */
			cp += j;
		}

	/* XXX might want to allow blanks to be escaped */
	i = 0;
	cp = str;
	for (;;) {
		if (i >= MAXARGS) {
			sprintf(errmsg, "LS has more than %d args", MAXARGS);
			return(errmsg);
		}
		largv[i++] = cp;
		if ((cp = index(cp, ' ')) == 0)
			break;
		*cp++ = '\0';
	}
	largc = i;
	return((char *)0);
}


/*
 * Reads input file until a line is parsed
 */
static int
loginparse(cmd, arg1, arg2, arg3)
	char **cmd, **arg1, **arg2, **arg3;
{
	register int i, n;
	register char *cp, *scp;
	char buf[512];
	static char sbuf[512];

	/* Get a non-blank, non-comment line */
	do {
		/* Get next line */
		if (fgets(buf, sizeof(buf), fp) == 0)
			return(0);
		++line;

		/* Nail trailing newline */
		i = strlen(buf) - 1;
		if (i >= 0 && buf[i] == '\n')
			buf[i] = '\0';
	} while (buf[0] == '\0' || buf[0] == '#');

	/* Do argument substitution */
	cp = buf;
	while (cp = index(cp, '$'))
		if (cp[1] == '$') {
			/* Fold doubled up $'s to singles */
			i = strlen(cp) + 1;
			bcopy(&cp[1], cp, i);
			++cp;
		} else if (!isdigit(cp[1])) {
			/* Leave single $'s alone */
			++cp;
		} else {
			/* Substitute */
			n = cp[1] - '0';
			/* Fetch argument */
			if (n >= largc)
				scp = "";
			else
				scp = largv[n];
			n = strlen(scp);
			i = strlen(&cp[2]) + 1;
			/* Scoot last part over */
			bcopy(&cp[2], &cp[n], i);
			/* Stuff in argument */
			bcopy(scp, cp, n);
			/* Step over argument */
			cp += n;
		}

	/* Parse */
	n = 1;
	*cmd = sbuf;
	*arg1 = *arg2 = *arg3 = "";
	for (cp = buf, scp = sbuf; *cp != '\0'; ++cp, ++scp)
		if (*cp == '\\') {
			++cp;
			if (*cp == '\0')
				break;
			if (*cp == 'b')
				*scp = '\b';
			else if (*cp == 'f')
				*scp = '\f';
			else if (*cp == 'n')
				*scp = '\n';
			else if (*cp == 'r')
				*scp = '\r';
			else if (*cp == 't')
				*scp = '\t';
			else if (*cp == 'd')
				*scp = LOGIN_DELAY;
			else if (*cp == 'x')
				*scp = LOGIN_BREAK;
			else if (*cp == '?')
				*scp = 0177;
			else
				*scp = *cp;
		} else if (*cp == ' ' || *cp == '\t') {
			*scp = '\0';
			if (++n == 2)
				*arg1 = scp + 1;
			else if (n == 3)
				*arg2 = scp + 1;
			else if (n == 4)
				*arg3 = scp + 1;
			else
				return(n);	/* too many arguments */
			/* Eat white space */
			while (*cp == ' ' || *cp == '\t')
				++cp;
			--cp;
		} else
			*scp = *cp;
	*scp = '\0';
	if (debug > 1) {
		(void)fprintf(stderr, "loginparse: line %d, n %d,\t\"",
		    line, n);
		errputs(*cmd);
		(void)fputs("\"\t\"", stderr);
		errputs(*arg1);
		(void)fputs("\"\t\"", stderr);
		errputs(*arg2);
		(void)fputs("\"\t\"", stderr);
		errputs(*arg3);
		(void)fputs("\"\n", stderr);
		(void)fflush(stderr);
	}
	return(n);
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
	fd_set readfds;

	t.tv_sec = secs;
	t.tv_usec = 0;
	FD_ZERO(&readfds);
	FD_SET(f, &readfds);
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

	if ((unsigned char) c == LOGIN_DELAY) {
		(void)fputs("\\d", stderr);
		(void)fflush(stderr);
		return;
	} else if ((unsigned char) c == LOGIN_BREAK) {
		(void)fputs("\\x", stderr);
		(void)fflush(stderr);
		return;
	}

	if (c & ~ 0177) {
		(void)fputs("M-", stderr);
		c &= 0177;
	}
	if (c < 0x20 || c == 0177) {
		if (c == '\b')
			(void)fputs("\\b", stderr);
		else if (c == '\f')
			(void)fputs("\\f", stderr);
		else if (c == '\n')
			(void)fputs("\\n", stderr);
		else if (c == '\r')
			(void)fputs("\\r", stderr);
		else if (c == '\t')
			(void)fputs("\\t", stderr);
		else if (c == '"')
			(void)fputs("\\\"", stderr);
		else {
			(void)putc('^', stderr);
			/* DEL to ?, others to alpha */
			(void)putc(c ^ 0x40, stderr);
		}
	} else
		(void)putc(c, stderr);
	(void)fflush(stderr);
}
