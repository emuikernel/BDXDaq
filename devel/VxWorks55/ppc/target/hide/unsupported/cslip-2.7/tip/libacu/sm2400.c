/*
 * Dialer support for Hayes Smartmodem(tm) 2400 modem.
 *
 * $Log:	sm2400.c,v $
 * Revision 1.11  86/06/24  19:47:12  rayan
 * Changed DIAL_PREFIX to use touch-tone. It seems to be available with
 * new CentrexIII...
 * 
 * Revision 1.10  86/06/24  19:39:16  rayan
 * Doubled DIAL_DELAY. Perhaps necessitated by new CentrexIII...
 * 
 * Revision 1.9  86/03/22  21:38:22  rayan
 * Fiddling...
 * 
 * Revision 1.8  86/03/13  17:25:43  rayan
 * Robustness fix. Sometimes the modem doesn't answer the first time 'round.
 * 
 * Revision 1.7  86/03/06  00:55:23  rayan
 * Cosmetics. Immaterial.
 * 
 * Revision 1.6  86/01/04  20:12:45  rayan
 * Moved the enabling of kernel attention to DCD into acu.c since it
 * applies to all the dialers just as well as this one.
 * 
 * Revision 1.5  85/08/14  16:30:52  rayan
 * Minor cleanups made while debugging a dh driver problem, sigh...
 * 
 * Revision 1.4  85/08/09  18:05:28  rayan
 * Ahem, added phone number to list of sm24set() parameters.
 * 
 * Revision 1.3  85/08/09  18:02:02  rayan
 * Slight cleanup, and want to recompile with ACULOG defined (in Makefile).
 * 
 * Revision 1.2  85/08/08  22:24:01  rayan
 * Also turn off escape code in initialization.
 * 
 * Revision 1.1  85/08/08  21:38:32  rayan
 * Initial revision
 * 
 *
 */

#ifndef lint
static char *RCSid = "$Header: sm2400.c,v 1.11 86/06/24 19:47:12 rayan Exp $";
#endif

#undef DEBUG 1

#ifdef	DEBUG
int debug = 1;
#define STATIC
#else
#define STATIC static
#endif

#include "tip.h"
#include <sys/time.h>

	/* commands */
STATIC char	NAME[] =		"sm2400";
STATIC char	WARMUP[] =		"\rat\r";
STATIC char	RESULT_CODES[] =	"\ratq0\r";
STATIC char	DONT_ANSWER[] =		"\rats0=0\r";
STATIC char	IGN_ESCAPE[] =		"\rats2=128\r";
STATIC char	CODES_4[] =		"\ratx4\r";
STATIC char	DIAL_PREFIX[] =		"\ratdt";

	/* result codes */;
STATIC char	MDM_OK[] =		"\r\nOK\r\n";
STATIC char	RING[] =		"\r\nRING\r\n";
STATIC char	NO_CARRIER[] =		"\r\nNO CARRIER\r\n";
STATIC char	ERROR[] =		"\r\nERROR\r\n";
STATIC char	CONNECT[] =		"\r\nCONNECT\r\n";
STATIC char	CONNECT_600[] =		"\r\nCONNECT 0600\r\n";
STATIC char	CONNECT_1200[] =	"\r\nCONNECT 1200\r\n";
STATIC char	CONNECT_2400[] =	"\r\nCONNECT 2400\r\n";
STATIC char	NO_DIALTONE[] =		"\r\nNO DIALTONE\r\n";
STATIC char	NO_ANSWER[] =		"\r\nNO ANSWER\r\n";
STATIC char	BUSY[] =		"\r\nBUSY\r\n";

STATIC char	*nosm24con[] =	{	BUSY,
					NO_DIALTONE,
					NO_ANSWER,
					ERROR,
					RING,
					NO_CARRIER,
					0
				};

/* The baudrate starts at 300, and doubles for next element of array */
STATIC char	*sm24con[] =	{	CONNECT,
					CONNECT_600,
					CONNECT_1200,
					CONNECT_2400,
					0
				};
#ifdef	USG
STATIC struct termio termio;		/* needed on USG to set HUPCL */
#endif	/* USG */

#define	CMD_DELAY	5	/* number of seconds to wait for response
					while in command mode */
#define DIAL_DELAY	120	/* number of seconds to wait for response
					after sending dial string */
#define	MAXSTR	100

static jmp_buf	jmpbuf;

int 	sm24read(), sm24write();

sm24_dialer(num, acu)
register char	*num;
char	*acu;
{
	int	ret;
	int	i, j;
	char	buf1[MAXSTR], buf2[MAXSTR], buf3[MAXSTR];
#ifdef ACULOG
	char	line[MAXSTR];
#endif

	/*
	 * wake up the modem if it's listening...
	 */
	if (boolean(value(VERBOSE)))
		printf("diddling dialer...");

	sleep(1);	/* Give modem time to settle down after reset */
	if ((!sm24set(RESULT_CODES, buf1, num, "is naughty")
		     && !sm24set(RESULT_CODES, buf1, num, "not listening"))
	    || !sm24set(DONT_ANSWER, buf1, num, "auto-answer not turned off")
	    || !sm24set(IGN_ESCAPE, buf1, num, "escape code not turned off")
	    || !sm24set(CODES_4, buf1, num, "result codes not set"))
		return 0;

	/* insurance ? */
#ifdef	USG
	if (ioctl(FD, TCGETA, &termio) < 0)
		perror("tip: TCGETA ioctl");
	termio.c_cflag |= HUPCL;
	if (ioctl(FD, TCSETA, &termio) < 0)
		perror("tip: TCSETA ioctl");
#else	/* USG */
	if (ioctl(FD, TIOCHPCL, 0) < 0)
		perror("tip: TIOCHPCL ioctl");
#endif	/* USG */

	buf2[0] = '\0';
	for (i = 0, j = 300; sm24con[i] != (char *)0; i++, j*=2) {
		if (BR == j) {
			strcpy(buf2, sm24con[i]);
			break;
		}
	}
	if (buf2[0] == '\0') {
		sprintf(buf2, "unsupported baud rate (%d)", BR);
		goto error;
	}

	/* Is it safe to just say ATD and let the options setting rule? */
	sm24write(DIAL_PREFIX);
	sm24write(num);
	sm24write("\r");
	if (boolean(value(VERBOSE)))
		printf(" dialing... ");
	if (ret = sm24read(buf1, DIAL_DELAY, (u_int) strlen(buf2)) == 0) {
		strcpy(buf2, "TIMEOUT on dial");
		goto error;
	}
	if (ret == -1) {
		strcpy(buf2, "ioctl failed");
		goto error;
	}
	if (strncmp(buf1, buf2, strlen(buf2)) != 0) {
		for (i = 0, j = 300; sm24con[i] != (char *)0; i++, j *= 2) {
			if (!strncmp(buf1, sm24con[i], strlen(sm24con[i]))) {
				/* This should never fail */
				if ((i = speed(j)) != NULL) {
					BR = j;
					ttysetup(i);
				} else {
					printf("speed(%d) failed\n", j);
					return 0;
				}
				goto ok;
			}
		}
		for (i = 0; nosm24con[i] != (char *)0; i++) {
			if (!strncmp(buf1, nosm24con[i], strlen(nosm24con[i]))){
				char	*bp, *cp;
				for (bp = buf2, cp = nosm24con[i]; *cp; cp++) {
					if (*cp != '\r' && *cp != '\n')
						*bp++ = *cp;
				}
				*bp = '\0';
				goto error;
			}
		}
		(void) strcpy(buf2, "UNKNOWN ERROR");
error:
		printf("dial failed (%s)\n", buf2);
		sprintf(buf3, "dial failed (%s)", buf2);
#ifdef ACULOG
		logent(value(HOST), num, NAME, buf3);
#endif
		return 0;
	}
ok:
	if (boolean(value(VERBOSE)))
		printf("(%d baud) ", BR);
	i = 2;
#ifdef	USG
	if (ioctl(FD, TCFLSH, i) < 0)
		perror("tip: TCFLSH ioctl");
#else	/* USG */
	if (ioctl(FD, TIOCFLUSH, &i) < 0)
		perror("tip: TIOCFLUSH ioctl");
#endif	/* USG */
	return 1;
}


sm24_disconnect()
{
#ifdef	USG
	if (ioctl(FD, TCGETA, &termio) < 0)
		perror("tip: TCGETA ioctl");
	termio.c_cflag &= (CBAUD & B0);
	if (ioctl(FD, TCSETA, &termio) < 0)
		perror("tip: TCSETA ioctl");
	sleep(2);
#endif	/* USG */
	(void) close(FD);
}


sm24_abort()
{
	sm24_disconnect();
}

STATIC
sm24write(cp)
register char	*cp;
{
	struct timeval t;

	for (; *cp != '\0' ; cp++) {
		if (write(FD, cp, 1) != 1) {
			perror("tip: sm24write: write error");
			return;
		}
#ifdef	DEBUG
		if (debug) {
			fprintf(stderr, "written '%c'\n", *cp);
		}
#endif
		t.tv_sec = 0;
		t.tv_usec = (10 * 1000000)/BR;
		(void) select(32, 0, 0, 0, &t);
	}
}

STATIC
sm24read(buf, timeout, max)
register char	*buf;
unsigned int timeout, max;
{
	int	alarmtr();
	struct timeval t;
	register char	*rp;
	unsigned int numread, numprev = 0, numpndg = 0;
	unsigned int toread;

#ifdef	DEBUG
	if (debug)
		fprintf(stderr, "\n---------\nentering sm24read(): timeout is: %d, max is: %d\n", timeout, max);
#endif
	rp = buf;
	*rp = '\0';
	(void) signal(SIGALRM, alarmtr);
	if (setjmp(jmpbuf)) {
		*rp = '\0';
		return 0;
	}
	(void) alarm(timeout);
		/* wait till modem says something or until timeout */
	while (numpndg == 0 || numpndg != numprev) {
		numprev = numpndg;
		if (ioctl(FD, FIONREAD, &numpndg) == -1) {
			perror("tip: FIONREAD ioctl");
			return -1;
		}
		t.tv_sec = 0;
		t.tv_usec = (5 * 10 * 1000000)/BR;
		(void) select(32, 0, 0, 0, &t);
	}
	(void) alarm(0);
#ifdef	DEBUG
	if (debug)
		fprintf(stderr, "number of chars waiting is: %d\n", numpndg);
#endif
	toread = ((numpndg > max) ? max : numpndg);
  	if ((numread = read(FD, rp, toread)) < toread)
		perror("read error");
#ifdef	DEBUG
	if (debug) {
		int i; char *p;
		fprintf(stderr, "read %d chars:", numread);
		for (i=0, p=rp; i<numread; i++, p++)
			fprintf(stderr, "%0.2x ", *p);
		fprintf(stderr, "\n");
		fflush(stderr);
	}
#endif
	rp += numread;
	*rp = '\0';
#ifdef	DEBUG
	if (debug) {
		fprintf(stderr, "\nbuf contains: ->%s<-\n", buf);
		fprintf(stderr, "leaving sm24read()\n-----------\n");
		fflush(stderr);
	}
#endif
	return 1;
}


STATIC
accept(baudrate)
int	baudrate;
{
	int i;

	BR = baudrate;
	if ((i = speed(baudrate)) != NULL)
		ttysetup(i);
}


STATIC
alarmtr()
{
#ifdef	DEBUG
	if (debug)
		fprintf(stderr, "alarm went off!\n");
#endif
	longjmp(jmpbuf, 1);
}

STATIC int
sm24set(tosend, buf, num, errmsg)
char	*tosend, *buf, *num, *errmsg;
{
	sm24write(tosend);
	sm24read(buf, CMD_DELAY, (u_int) (sizeof MDM_OK) - 1);
	if (strncmp(buf, MDM_OK, (sizeof MDM_OK) - 1) != 0) {
		printf(" %s modem %s\n", NAME, errmsg);
#ifdef ACULOG
		logent(value(HOST), num, NAME, errmsg);
#endif
		return 0;
	}
	return 1;
}
