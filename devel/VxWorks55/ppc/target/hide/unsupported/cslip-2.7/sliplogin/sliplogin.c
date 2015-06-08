/*
 * Copyright (c) 1990 The Regents of the University of California.
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
    "@(#) $Header: sliplogin.c,v 1.36 92/06/17 16:20:37 leres Exp $ (LBL)";
#endif

/*
 * sliplogin.c
 * [MUST BE RUN SUID, SLOPEN DOES A SUSER()!]
 *
 * This program initializes its own tty port to be an async TCP/IP interface.
 * It sets the line discipline to slip, invokes a shell script to initialize
 * the network interface, then pauses forever waiting for hangup.
 *
 * It is a remote descendant of several similar programs with incestuous ties:
 * - Kirk Smith's slipconf, modified by Richard Johnsson @ DEC WRL.
 * - slattach, probably by Rick Adams but touched by countless hordes.
 * - the original sliplogin for 4.2bsd, Doug Kingston the mover behind it.
 *
 * There are two forms of usage:
 *
 * "sliplogin"
 * Invoked simply as "sliplogin" and a realuid != 0, the program looks up
 * the uid in /etc/passwd, and then the username in the file /etc/hosts.slip.
 * If and entry is found, the line on fd0 is configured for SLIP operation
 * as specified in the file.
 *
 * "sliplogin IPhost1 </dev/ttyb"
 * Invoked by root with a username, the name is looked up in the
 * /etc/hosts.slip file and if found fd0 is configured as in case 1.
 */

#include <sys/types.h>

/* XXX SunOS 4.1 defines this and 3.5 doesn't... */
#ifdef __sys_types_h
#define TERMIOS
#endif

#ifndef TERMIOS
#include <sys/ioctl.h>
#endif
#include <sys/file.h>
#include <syslog.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <netdb.h>
#include <signal.h>
#include <string.h>
#include <varargs.h>

#ifdef TERMIOS
#include <net/slip.h>
#include <sys/stropts.h>
#include <sys/termios.h>
#include <sys/ttold.h>
#include <sys/sockio.h>
#endif

#ifndef ACCESSFILE
#define ACCESSFILE "/etc/slip.hosts"
#endif
#ifndef LOGINFILE
#define LOGINFILE "/etc/slip.login"
#define LOGOUTFILE "/etc/slip.logout"
#endif

extern	char *getenv();
extern	char *ttyname();
extern	char *strerror();
extern	char *mktemp();

int	unit;
int	speed;
char	loginargs[BUFSIZ];
char	loginfile[BUFSIZ];
char	logoutfile[BUFSIZ];
char	loginname[BUFSIZ];
#ifdef DEBUG
char	tempfile[BUFSIZ];
#endif
char	accessfile[] = ACCESSFILE;
char	null[] = "/dev/null";
char	*prog;
int	uid, suid;

/* VARARGS */
void
log(va_alist)
	va_dcl
{
	register char *cp, *cp2;
	register int pri, i, j;
	register char lch;
	va_list vp;
	char buf[BUFSIZ], fmt[BUFSIZ];

	va_start(vp);
	pri = va_arg(vp, int);
	cp = va_arg(vp, char *);
	(void)strcpy(fmt, cp);
	/* Convert %m's in fmt */
	lch = '\0';
	for (cp = fmt; *cp != '\0'; ++cp) {
		if (cp[0] == '%' && cp[1] == 'm' && lch != '%') {
			cp2 = strerror(errno);
			i = strlen(cp2);
			j = strlen(&cp[2]);
			bcopy(&cp[2], &cp[i], j + 1);
			bcopy(cp2, cp, i);
			cp += i - 1;
		}
		lch = *cp;
	}
	(void)vsprintf(buf, fmt, vp);
	(void)fprintf(stderr, "%s: %s\n", prog, buf);
	syslog(pri, "%s", buf);
	va_end(vp);
}

void
findid(name)
	register char *name;
{
	register char *cp;
	register FILE *fp;

	(void)strcpy(loginname, name);
	if ((fp = fopen(accessfile, "r")) == NULL) {
		log(LOG_ERR, "%s: %m", accessfile);
		exit(3);
		/* NOTREACHED */
	}
	while (fgets(loginargs, sizeof(loginargs) - 1, fp)) {
		char c;

		if (ferror(fp))
			break;
		if (loginargs[0] == '#' || isspace(loginargs[0]))
			continue;

		/* Strip trailing newline */
		cp = loginargs + strlen(loginargs) - 1;
		if (cp >= loginargs && *cp == '\n')
			*cp-- = '\0';

		for (cp = loginargs; (c = *cp) && !isspace(c); ++cp)
			;
		*cp = '\0';
		if (strcmp(loginargs, name) != 0)
			continue;
		*cp = c;

		/*
		 * we've found the guy we're looking for -- see if
		 * there's a login file we can use.  First check for
		 * one specific to this host.  If none found, try for
		 * a generic one.
		 */
		(void)sprintf(loginfile, "%s.%s", LOGINFILE, name);
		if (access(loginfile, R_OK|X_OK)) {
			(void)strcpy(loginfile, LOGINFILE);
			(void)strcpy(logoutfile, LOGOUTFILE);
			if (access(loginfile, R_OK|X_OK)) {
				log(LOG_ERR, "access denied for %s - no %s",
				    name, LOGINFILE);
				exit(5);
				/* NOTREACHED */
			}
		} else
			(void)sprintf(logoutfile, "%s.%s", LOGOUTFILE, name);

		(void)fclose(fp);
		return;
	}
	log(LOG_ERR, "SLIP access denied for %s", name);
	exit(4);
	/* NOTREACHED */
}

char *
sigstr(s)
	register int s;
{
	static char buf[32];

	switch (s) {
	case SIGHUP:	return ("HUP");
	case SIGINT:	return ("INT");
	case SIGQUIT:	return ("QUIT");
	case SIGILL:	return ("ILL");
	case SIGTRAP:	return ("TRAP");
	case SIGIOT:	return ("IOT");
	case SIGEMT:	return ("EMT");
	case SIGFPE:	return ("FPE");
	case SIGKILL:	return ("KILL");
	case SIGBUS:	return ("BUS");
	case SIGSEGV:	return ("SEGV");
	case SIGSYS:	return ("SYS");
	case SIGPIPE:	return ("PIPE");
	case SIGALRM:	return ("ALRM");
	case SIGTERM:	return ("TERM");
	case SIGURG:	return ("URG");
	case SIGSTOP:	return ("STOP");
	case SIGTSTP:	return ("TSTP");
	case SIGCONT:	return ("CONT");
	case SIGCHLD:	return ("CHLD");
	case SIGTTIN:	return ("TTIN");
	case SIGTTOU:	return ("TTOU");
	case SIGIO:	return ("IO");
	case SIGXCPU:	return ("XCPU");
	case SIGXFSZ:	return ("XFSZ");
	case SIGVTALRM:	return ("VTALRM");
	case SIGPROF:	return ("PROF");
	case SIGWINCH:	return ("WINCH");
#ifdef SIGLOST
	case SIGLOST:	return ("LOST");
#endif
	case SIGUSR1:	return ("USR1");
	case SIGUSR2:	return ("USR2");
	}
	(void)sprintf(buf, "sig %d", s);
	return (buf);
}

struct sg_spds {
    int sp_val, sp_name;
} spds[] = {
#ifdef B50
	{ 50, B50 },
#endif
#ifdef B75
	{ 75, B75 },
#endif
#ifdef B110
	{ 110, B110 },
#endif
#ifdef B150
	{ 150, B150 },
#endif
#ifdef B200
	{ 200, B200 },
#endif
#ifdef B300
	{ 300, B300 },
#endif
#ifdef B600
	{ 600,B600 },
#endif
#ifdef B1200
	{ 1200, B1200 },
#endif
#ifdef B1800
	{ 1800, B1800 },
#endif
#ifdef B2000
	{ 2000, B2000 },
#endif
#ifdef B2400
	{ 2400, B2400 },
#endif
#ifdef B3600
	{ 3600, B3600 },
#endif
#ifdef B4800
	{ 4800, B4800 },
#endif
#ifdef B7200
	{ 7200, B7200 },
#endif
#ifdef B9600
	{ 9600, B9600 },
#endif
#ifdef EXTA
	{ 19200, EXTA },
#endif
#ifdef EXTB
	{ 38400, EXTB },
#endif
	{ 0, 0 }
};

int
findbaud(wantspeed)
	register int wantspeed;
{
	register struct sg_spds *sp;

	sp = spds;

	while (sp->sp_name && sp->sp_name != wantspeed)
		sp++;

	return (sp->sp_val);
}

/* Open the specified file and re-aim stdout and stderr to it */
void
redirect(file, flags, mode)
	register char *file;
	register int flags, mode;
{
	register int fd;

	(void)close(1);
	if ((fd = open(file, flags, mode)) != 1)
		if (fd < 0 || dup2(fd, 1) < 0 || close(fd) < 0) {
			log(LOG_ERR, "%s: %m", file);
			exit(1);
			/* NOTREACHED */
		}
	if (dup2(1, 2) < 0) {
		log(LOG_ERR, "%s: %m", file);
		exit(1);
		/* NOTREACHED */
	}
}

int
hup_handler(s)
	register int s;
{
	register int err;

	if (setreuid(uid, suid) < 0)
		log(LOG_ERR, "hup setreuid(%d, %d): %m", uid, suid);
#ifdef TERMIOS
	/* Pop the slip module to mark the slip interface down */
	while (ioctl(0, I_POP, 0) == 0)
		continue;
#else
	/* Set the line discipline to make the slip interface down */
	{
		int ldisc;

		ldisc = 0;
		if (ioctl(0, TIOCSETD, (caddr_t)&ldisc) < 0)
			log(LOG_ERR, "restore TIOCSETD: %m");
	}
#endif
	if (access(logoutfile, R_OK|X_OK) == 0) {
		char logincmd[2*BUFSIZ+32];

		(void)sprintf(logincmd, "%s %d %d %s",
		    logoutfile, unit, findbaud(speed), loginargs);
#ifdef DEBUG
		/*
		 * Redirect output to a temp file in case there are errors.
		 */
		(void)strcpy(tempfile, "/tmp/sliplogin.logout.XXXXXX");
		(void)mktemp(tempfile);
		redirect(tempfile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
#endif
		err = system(logincmd);
#ifdef DEBUG
		/*
		 * Only remove temp file if system() was successful.
		 */
		if (err == 0 && unlink(tempfile) < 0)
			log(LOG_ERR, "unlink(%s): %m", tempfile);
#endif
	}
	(void)close(0);
	log(LOG_INFO, "closed %s slip unit %d (%s)",
	    loginname, unit, sigstr(s));
	exit(1);
	/* NOTREACHED */
}

int
main(argc, argv)
	int argc;
	char *argv[];
{
	register int fd, terminal, err;
	register char *name;
	int pgrp;
	char logincmd[2 * BUFSIZ + 32];

	uid = getuid();
	suid = geteuid();
	if (setreuid(suid, uid) < 0)
		log(LOG_ERR, "initial setreuid(%d, %d): %m", suid, uid);

	if (prog = strrchr(argv[0], '/'))
		++prog;
	else
		prog = argv[0];
	if (*prog == '-')
		++prog;
#ifdef notdef
	/* ??? */
	fd = getdtablesize();
	while (--fd > 3)
		(void)close(fd);
#endif

	openlog(prog, LOG_PID, LOG_DAEMON);

	if (argc == 1) {
		if ((name = getenv("USER")) == NULL) {
			log(LOG_ERR, "access denied - no username");
			exit(1);
			/* NOTREACHED */
		}
		findid(name);
		terminal = 0;
	} else if (argc == 2) {
		findid(argv[1]);
		terminal = 1;
	} else {
		(void)fprintf(stderr, "Usage: %s loginname\n", prog);
		exit(1);
		/* NOTREACHED */
	}
	(void) fprintf(stderr, "starting slip login for %s\n", loginname);

	if (setreuid(uid, suid) < 0)
		log(LOG_ERR, "tty setup setreuid(%d, %d): %m", uid, suid);
#ifdef TERMIOS
	{
		struct termios tios;

		/*
		 * Disassociate from current controlling terminal, if any,
		 * and ensure that the slip line is our controlling terminal.
		 */
		if (terminal) {
			if (setsid() < 0)
				log(LOG_ERR, "setsid: %m");
			if (ioctl(0, TIOCSCTTY, (caddr_t)0) < 0)
				log(LOG_ERR, "TIOCSCTTY: %m");
		}

		/* pop all streams modules */
		while (ioctl(0, I_POP, 0) == 0)
			continue;

		/* set up the line parameters */
		if (ioctl(0, TCGETS, (caddr_t)&tios) < 0) {
			log(LOG_ERR, "TCGETS: %m");
			exit(1);
			/* NOTREACHED */
		}
		/* leave the speed unchanged, set everything else */
		tios.c_cflag &= CBAUD;
		tios.c_cflag |= CS8|CREAD|HUPCL;
		tios.c_iflag = IGNBRK;
		if (ioctl(0, TCSETS, (caddr_t)&tios) < 0) {
			log(LOG_ERR, "TCSETS: %m");
			exit(1);
			/* NOTREACHED */
		}
		/* push the SLIP module */
		if (ioctl(0, I_PUSH, "slip") < 0) {
			log(LOG_ERR, "I_PUSH: %m");
			exit(1);
			/* NOTREACHED */
		}
		/* find out what unit number we were assigned */
		if (ioctl(0, SLIOGUNIT, (caddr_t)&unit) < 0) {
			log(LOG_ERR, "SLIOGUNIT: %m");
			exit(1);
			/* NOTREACHED */
		}
	}
#else
	{
		int ldisc;
		struct sgttyb tty;

		/*
		 * Disassociate from current controlling terminal, if any,
		 * and ensure that the slip line is our controlling terminal.
		 */
		if (terminal && (fd = open("/dev/tty", O_RDONLY, 0)) >= 0) {
			(void)ioctl(fd, TIOCNOTTY, (caddr_t)0);
			(void)close(fd);
			/* open slip tty again to acquire as controlling tty? */
			fd = open(ttyname(0), O_RDWR, 0);
			if (fd >= 0)
				(void)close(fd);
		}

		/* We want to be in our own process group */
		pgrp = getpid();
		if (setpgrp(0, pgrp) < 0)
			log(LOG_ERR, "setpgrp(): %m");
		if (ioctl(0, TIOCSPGRP, (caddr_t)&pgrp) < 0)
			log(LOG_ERR, "TIOCSPGRP %m");

		/* set up the line parameters */
		if (ioctl(0, TIOCGETP, (caddr_t)&tty) < 0) {
			log(LOG_ERR, "TIOCGETP: %m");
			exit(1);
			/* NOTREACHED */
		}
		speed = tty.sg_ispeed;
		tty.sg_flags = RAW | ANYP;
		if (ioctl(0, TIOCSETP, (caddr_t)&tty) < 0) {
			log(LOG_ERR, "TIOCSETP: %m");
			exit(1);
		}
		ldisc = SLIPDISC;
		if (ioctl(0, TIOCSETD, (caddr_t)&ldisc) < 0) {
			log(LOG_ERR, "TIOCSETD: %m");
			exit(1);
		}
		/* find out what unit number we were assigned */
		if (ioctl(0, TIOCGETD, (caddr_t)&unit) < 0) {
			log(LOG_ERR, "unit TIOCGETD: %m");
			exit(1);
		}
	}
#endif
	(void)signal(SIGHUP, hup_handler);
	(void)signal(SIGINT, hup_handler);
	(void)signal(SIGTERM, hup_handler);

	log(LOG_INFO, "attaching slip unit %d for %s", unit, loginname);
	(void)sprintf(logincmd, "%s %d %d %s",
	    loginfile, unit, findbaud(speed), loginargs);
#ifdef DEBUG
	/*
	 * Redirect output to a temp file in case there are errors.
	 */
	(void)strcpy(tempfile, "/tmp/sliplogin.login.XXXXXX");
	(void)mktemp(tempfile);
	redirect(tempfile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
#else
	/*
	 * Redirect output to /dev/null so logincmd output won't
	 * babble into the slip tty line.
	 */
	redirect(null, O_WRONLY, 0);
#endif
	err = system(logincmd);
	if (err & 0xff) {
		log(LOG_ERR, "system: %m");
		exit(6);
		/* NOTREACHED */
	} else if (err & 0xff00) {
		log(LOG_ERR, "%s login failed: exit status %d from %s",
		       loginname, err >> 8, loginfile);
		exit(7);
	}
#ifdef DEBUG
	/*
	 * Close debugging file, re-aim output at /dev/null and delete
	 * debugging file.
	 */
	redirect(null, O_WRONLY, 0);
	if (unlink(tempfile) < 0)
		log(LOG_ERR, "unlink(%s): %m", tempfile);
#endif
	/* twiddle thumbs until we get a signal */
	if (setreuid(suid, uid) < 0)
		log(LOG_ERR, "pause setreuid(%d, %d): %m", suid, uid);
	while (1)
		sigpause(0);
}
