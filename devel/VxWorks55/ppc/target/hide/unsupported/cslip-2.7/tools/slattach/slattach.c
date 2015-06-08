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
    "$Header: slattach.c,v 1.6 92/06/18 18:22:17 leres Exp $";
#endif

/*
 * slattach [{+|-}{c|e|i} ...] device src-name dst-name [baudrate]
 *
 * Attach the next free slip line to "device", assign it the IP src
 * address associated with "src-name" and say the other end of the
 * slip link has IP address "dst-name".  If "baudrate" is specified,
 * it is used as the device baudrate.  Otherwise the current baudrate
 * set for the device is used.
 *
 * Examples:
 *	slattach ttya 126.1 126.2 19200 &
 *	slattach +c -i ttyd0 foo.somewhere.edu bar.nowhere.org &
 */
#include <stdio.h>
#include <sys/types.h>

/* XXX SunOS 4.1 defines this and 3.5 doesn't... */
#ifdef __sys_types_h
#define SUNOS4
#endif

#ifndef SUNOS4
#include <sys/ioctl.h>
#include <sgtty.h>
#endif
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <netdb.h>
#include <fcntl.h>
#include <signal.h>
#include <ctype.h>

#ifdef SIOCSETRTINFO
#include <net/route.h>
#endif

#ifdef SUNOS4
#include <net/slip.h>
#include <sys/mbuf.h>
#include <sys/stream.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <net/slcompress.h>
#include <net/if_slvar.h>
#include <sys/stropts.h>
#include <sys/termios.h>
#include <sys/ttold.h>
#include <sys/sockio.h>
#endif

#include <syslog.h>

#ifndef SUNOS4
int slipdisc = SLIPDISC;
int olddisc;
struct sgttyb otty;
#endif

struct	ifreq ifr;
struct	sockaddr_in sin = { AF_INET };

int wantbaud;
int speed;

char devname[256];
char hostname[256];
char *dev;
int fd, s;
int set_flags;		/* interface flag bits to set */
int clear_flags;	/* interface flag bits to clear */

/* todo:
 * verify addresses before diving in
 * hup routine that restores tty state
 */

main(argc,argv)
char *argv[];
{
	int huphandler();
#ifdef SUNOS4
	struct termios tios;
#else
	struct sgttyb ntty;
#endif
	int n;
	struct sockaddr_in addr, dstaddr;
#ifdef SIOCSETRTINFO
	struct fullrtentry froute;
#endif

	while (argc > 1 && (argv[1][0] == '-' || argv[1][0] == '+')) {
		switch (argv[1][1]) {
		case 'c':	/* enable or disable hdr compression */
			if (argv[1][0] == '-')
				clear_flags |= IFF_LINK0;
			else
				set_flags |= IFF_LINK0;
			break;
		case 'e':	/* enable or disable hdr.comp. `auto-enable' */
			if (argv[1][0] == '-')
				clear_flags |= IFF_LINK1;
			else
				set_flags |= IFF_LINK1;
			break;
		case 'i':	/* enable or disable icmp discard */
			if (argv[1][0] == '-')
				clear_flags |= IFF_LINK2;
			else
				set_flags |= IFF_LINK2;
			break;
		default:
			fprintf(stderr,"%s: unknown flag `%s'\n", argv[0],
				argv[1]);
			exit(1);
		}
		--argc;
		++argv;
	}
	dev = argv[1];
	if (argc < 2 || argc > 5) {
		fprintf(stderr,
	"usage: slattach ttyname source-name destination-name [baudrate]\n");
		exit(1);
	}

	openlog("slattach", 0, LOG_DAEMON);
	if (argc == 5) {
		wantbaud = atoi(argv[4]);
		speed = findspeed(wantbaud);
		if (speed == 0) {
			fprintf(stderr, "%s: Unknown speed\n", argv[4]);
			exit(1);
		}
	}
	getaddr(argv[2], &addr);		/* local addr */
	getaddr(argv[3], &dstaddr);		/* dest addr */
	if (strncmp("/dev/", dev, 5) != 0) {
		sprintf(devname,"/dev/%s", dev);
		dev = devname;
	}
	if ((fd = open(dev, O_RDWR|O_NDELAY)) < 0 ) {
		perror(dev);
		exit(1);
	}

#ifndef SUNOS4
	/* Get current tty parameters */
	if (ioctl(fd, TIOCGETP, &otty) < 0) {
		fputs(dev, stderr);
		perror(": get parameters:");
		exit(1);
	}

	/* Save old line discipline */
	if (ioctl(fd, TIOCGETD, &olddisc) < 0) {
		fputs(dev, stderr);
		perror(": get discipline:");
		exit(1);
	}

	/* Enable hangup handler */
	(void) signal(SIGHUP, huphandler);
	(void) signal(SIGINT, huphandler);

	/* Set new tty parameters */
	bcopy((char *)&otty, (char *)&ntty, sizeof(otty));
	if (speed)
		ntty.sg_ispeed = ntty.sg_ospeed = speed;
	else
		wantbaud = findbaud(ntty.sg_ospeed);
	ntty.sg_flags = RAW | ANYP;
	if (ioctl(fd, TIOCSETP, &ntty) < 0) {
		fputs(dev, stderr);
		perror(": set parameters:");
		exit(1);
	}

	/* Set slip line discipline */
	if (ioctl(fd, TIOCSETD, &slipdisc) < 0) {
		fputs(dev, stderr);
		perror(": set discipline:");
		syslog(LOG_INFO, "TIOCSETD: %m");
		exit(1);
	}

	/* Find out which slip line we got? */
	if (ioctl(fd, TIOCGETD, &n) < 0) {
		perror("cant get line discipline");
		cleandeath(1);
	}
#else
	/* pop all streams modules */
	while (ioctl(fd, I_POP, 0) == 0)
		continue;

	/* set up the line parameters */
	if (ioctl(fd, TCGETS, (caddr_t)&tios) < 0) {
		syslog(LOG_ERR, "ioctl (TCGETS): %m");
		exit(1);
	}
	if (speed) {
		tios.c_cflag &= CRTSCTS;
		tios.c_cflag |= speed;
	}
	else {
		/* save the speed and CTSRTS */
		tios.c_cflag &= (CRTSCTS | CBAUD);
		wantbaud = findbaud(tios.c_cflag & CBAUD);
	}
	tios.c_cflag |= CS8|CREAD|HUPCL;
	tios.c_iflag = IGNBRK;
	if (ioctl(fd, TCSETS, (caddr_t)&tios) < 0) {
		syslog(LOG_ERR, "ioctl (TCSETS): %m");
		exit(1);
	}
	/* push the SLIP module */
	if (ioctl(fd, I_PUSH, "slip") < 0) {
		syslog(LOG_ERR, "ioctl (I_PUSH): %m");
		exit(1);
	}
	/* find out what unit number we were assigned */
	if (ioctl(fd, SLIOGUNIT, (caddr_t)&n) < 0) {
		syslog(LOG_ERR, "ioctl (SLIOGUNIT): %m");
		exit(1);
	}
#endif

	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s < 0) {
		perror("ifattach: socket");
		syslog(LOG_INFO, "socket: %m");
		cleandeath(1);
	}
	printf("slattach: sl%d at %d baud,", n, wantbaud); 
	fflush(stdout);
	syslog(LOG_INFO, "sl%d at %d baud", n, wantbaud);

	sprintf(ifr.ifr_name, "sl%d", n);

	bcopy((char *)&dstaddr, (char *)&ifr.ifr_dstaddr, sizeof(dstaddr));
	printf(" remote %s (%s)", argv[3], inet_ntoa(dstaddr.sin_addr));
	fflush(stdout);
	syslog(LOG_INFO, "remote %s (%s)",
	    argv[3], inet_ntoa(dstaddr.sin_addr));
	if (ioctl(s, SIOCSIFDSTADDR, (caddr_t)&ifr) < 0) {
		perror("ioctl (SIOCSIFDSTADDR)");
		syslog(LOG_INFO, "%s: SIOCSIFDSTADDR: %m", ifr.ifr_name);
		cleandeath(1);
	}

	bcopy((char *)&addr, (char *)&ifr.ifr_addr, sizeof(addr));
	printf(" local %s (%s)\n", argv[2], inet_ntoa(addr.sin_addr));
	fflush(stdout);
	syslog(LOG_INFO, "local %s (%s)",
	    argv[2], inet_ntoa(addr.sin_addr));
	if (ioctl(s, SIOCSIFADDR, (caddr_t)&ifr) < 0) {
		perror("ioctl (SIOCSIFADDR)");
		syslog(LOG_INFO, "SIOCSIFADDR: %m");
		cleandeath(1);
	}
	if (set_flags || clear_flags) {
		if (ioctl(s, SIOCGIFFLAGS, (caddr_t)&ifr) < 0) {
			perror("ioctl (SIOCGIFFLAGS)");
			syslog(LOG_INFO, "SIOCGIFFLAGS: %m");
			cleandeath(1);
		}
		ifr.ifr_flags &=~ clear_flags;
		ifr.ifr_flags |=  set_flags;
		if (ioctl(s, SIOCSIFFLAGS, (caddr_t)&ifr) < 0) {
			perror("ioctl (SIOCSIFFLAGS)");
			syslog(LOG_INFO, "SIOCSIFFLAGS: %m");
			cleandeath(1);
		}
	}
#ifdef SIOCSETRTINFO
	/* Tweak up route */
	bzero((char *)&froute, sizeof(froute));
	bcopy((char *)&dstaddr, (char *)&froute.r.rt_dst, sizeof(dstaddr));
	bcopy((char *)&addr, (char *)&froute.r.rt_gateway, sizeof(dstaddr));
	froute.r.rt_flags = RTF_UP | RTF_HOST;
	froute.rt_mtu = 256;
	froute.rt_sendpipe = froute.rt_recvpipe = 1024;
	froute.rt_rtt = (int)(2 * 1.0e6);
	if (ioctl(s, SIOCSETRTINFO, (caddr_t)&froute) < 0) {
		perror("ioctl (SIOCSIFADDR)");
		syslog(LOG_ERR, "ioctl (SIOCSETRTINFO): %m");
	}
#endif
	for (;;)
		pause();
}

struct sg_spds {int sp_val, sp_name;} spds[] = {
#ifdef B50
	{  50,	 B50},
#endif
#ifdef B75
	{  75,	 B75},
#endif
#ifdef B110
	{ 110,	B110},
#endif
#ifdef B150
	{ 150,	B150},
#endif
#ifdef B200
	{ 200,	B200},
#endif
#ifdef B300
	{ 300,  B300},
#endif
#ifdef B600
	{600,	B600},
#endif
#ifdef B1200
	{1200, B1200},
#endif
#ifdef B1800
	{1800, B1800},
#endif
#ifdef B2000
	{2000, B2000},
#endif
#ifdef B2400
	{2400, B2400},
#endif
#ifdef B3600
	{3600, B3600},
#endif
#ifdef B4800
	{4800, B4800},
#endif
#ifdef B7200
	{7200, B7200},
#endif
#ifdef B9600
	{9600, B9600},
#endif
#ifdef EXTA
	{19200,EXTA},
#endif
#ifdef EXTB
	{38400,EXTB},
#endif
	{0, 0}
};

findspeed(wantbaud)
	register int wantbaud;
{
	register struct sg_spds *sp;

	sp = spds;

	while ( sp->sp_val && sp->sp_val != wantbaud)
		sp++;

	return sp->sp_name;
}

findbaud(speed)
	register int speed;
{
	register struct sg_spds *sp;

	sp = spds;

	while ( sp->sp_name && sp->sp_name != speed)
		sp++;

	return sp->sp_val;
}

getaddr(s, sin)
	char *s;
	struct sockaddr_in *sin;
{
	struct hostent *hp;
	int val;

	bzero(sin, sizeof(struct sockaddr_in));
	if (isdigit(*s)) {
		sin->sin_family = AF_INET;
		val = inet_addr(s);
		if (val != -1) {
			sin->sin_addr.s_addr = val;
			return;
		}
	}
	hp = gethostbyname(s);
	if (hp) {
		sin->sin_family = hp->h_addrtype;
		bcopy(hp->h_addr, (char *)&sin->sin_addr, hp->h_length);
		return;
	}
	fprintf(stderr, "%s: bad value\n", s);
	exit(1);
}

cleandeath(error)
{
#ifdef notdef
	if (ioctl(fd, TIOCSETD, &olddisc) < 0) {
		fputs(dev, stderr);
		perror(": restore discipline:");
		syslog(LOG_INFO, "%s: restore discipline: %m", dev);
	} else if (ioctl(fd, TIOCSETP, &otty) < 0) {
		fputs(dev, stderr);
		perror(": restore parameters:");
		syslog(LOG_INFO, "%s: restore parameters: %m", dev);
	}
#endif
	exit(error);
}

huphandler()
{
	cleandeath(0);
}
