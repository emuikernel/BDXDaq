/*
 * Copyright (c) 1992 The Regents of the University of California.
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
    "Copyright (c) 1992 Lawrence Berkeley Laboratory\nAll rights reserved.\n";
static char rcsid[] =
    "@(#) $Header: myetheraddr.c,v 1.3 93/07/18 19:05:10 leres Exp $ (LBL)";
#endif

/*
 * myetheraddr - display ethernet local address(es)
 *
 * usage:
 *
 *	myetheraddr [-l] [host]
 *
 *		-l - long output (also display ip address and interface name)
 *		host - want ethernet address of interface on same subnet as host
 *
 */

#include <sys/param.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/cdefs.h>

#include <net/if.h>
#include <net/if_dl.h>
#include <net/if_types.h>

#include <netinet/in.h>

#include <arpa/inet.h>

#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>

#define NUMINTERFACES 16

struct list {
	char *name;		/* interface name */
	struct in_addr *inp;	/* ip address */
	u_char *ea;		/* ethernet address */
} list[NUMINTERFACES];

/* Cast a struct sockaddr to a structaddr_in */
#define SATOSIN(sa) ((struct sockaddr_in *)(sa))

/* Cast a struct sockaddr to a structaddr_dl */
#define SATOSDL(sa) ((struct sockaddr_dl *)(sa))

static char *prog;

extern char *optarg;
extern int optind, opterr;

int
main(argc, argv)
	int argc;
	char **argv;
{
	register char *cp;
	register struct ifreq *ifrp, *ifend;
	register struct list *lp, *ep;
	register u_char *ea;
	register u_long mask;
	register int op, fd;
	register struct hostent *hp;
	register char *host;
	register int lflag, num;
	register int n;
	u_long hostip;
	struct ifreq ibuf[NUMINTERFACES], ifr;
	struct ifconf ifc;
	char *usage = "usage: %s [-l] [host]\n";

	/* Determine simple program name for error messages */
	if ((cp = (char *)rindex(argv[0], '/')) != NULL)
		prog = cp + 1;
	else
		prog = argv[0];

	/* Parse flags */
	opterr = 0;
	lflag = 0;
	while ((op = getopt(argc, argv, "l")) != EOF)
		switch (op) {

		case 'l':
			++lflag;
			break;
		default:
			(void) fprintf(stderr, usage, prog);
			exit(1);
		}

	/* Parse optional argument */
	switch (argc - optind) {

	case 0:
		hostip = 0;
		break;

	case 1:
		host = argv[optind];
		hostip = inet_addr(host);
		if ((long)hostip != -1)
			break;
		if ((hp = gethostbyname(host)) == 0) {
			fprintf(stderr, "%s: bad host \"%s\"\n", prog, host);
			exit(1);
		}
		/*
		 * XXX The host might have more than one ip address. If
		 * so, we just blindly use the first one. Note that a
		 * modern named will order multiple addresses so that
		 * the first one is on a local interface so this isn't
		 * as bad as it seems.
		 */
		bcopy(hp->h_addr, (char *)&hostip, sizeof(hostip));
		break;

	default:
		(void) fprintf(stderr, usage, prog);
		exit(1);
	}

	/* Fetch the interface configuration */
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0) {
		fprintf(stderr, "%s: ", prog);
		perror("socket");
		exit(1);
	}
	ifc.ifc_len = sizeof(ibuf);
	ifc.ifc_buf = (caddr_t)ibuf;
	if (ioctl(fd, SIOCGIFCONF, (char *)&ifc) < 0 ||
	    ifc.ifc_len < sizeof(struct ifreq)) {
		fprintf(stderr, "%s: ", prog);
		perror("SIOCGIFCONF");
		exit(1);
	}

	/* Spin through interface configuration list, setup internal list */
	ifrp = ibuf;
	ifend = (struct ifreq *)((char *)ibuf + ifc.ifc_len);
	ep = list;
	while (ifrp < ifend) {
		/* Look for interface */
		for (lp = list; lp < ep; ++lp)
			if (strncmp(lp->name,
			    ifrp->ifr_name, sizeof(ifrp->ifr_name)) == 0)
				break;
		if (ep <= lp) {
			/* First time we've see this one */
			ep = lp + 1;
			lp->name = ifrp->ifr_name;
		}

		/* Fill in any relevant info we learned this time */
		switch (ifrp->ifr_addr.sa_family) {

		case AF_INET:
			lp->inp = &SATOSIN(&ifrp->ifr_addr)->sin_addr;
			break;

		case AF_LINK:
			/* We're only interested in ethernet interfaces */
			if (SATOSDL(&ifrp->ifr_addr)->sdl_type == IFT_ETHER)
				lp->ea =
				    (u_char *)LLADDR(SATOSDL(&ifrp->ifr_addr));
			break;
		}

		/* Bump interface config pointer */
		n = ifrp->ifr_addr.sa_len + sizeof(ifrp->ifr_name);
		if (n < sizeof(*ifrp))
			n = sizeof(*ifrp);
		ifrp = (struct ifreq *)((char *)ifrp + n);
	}

	/* Spin through internal list */
	for (lp = list; lp < ep; ++lp) {
		/* We need both the ip and ethernet addresses */
		if (lp->inp == 0 || lp->ea == 0)
			continue;

		/* Optionally check if for interface on the specified subnet */
		if (hostip) {
			/* Get the netmask */
			bcopy(lp->name, ifr.ifr_name, sizeof(ifr.ifr_name));
			if (ioctl(fd, SIOCGIFNETMASK, (char *)&ifr) < 0) {
				fprintf(stderr, "%s: SIOCGIFNETMASK: ", prog);
				perror(lp->name);
				exit(1);
			}
			mask = SATOSIN(&ifr.ifr_addr)->sin_addr.s_addr;
			/* Skip if interface is not on correct subnet */
			if ((hostip & mask) != (lp->inp->s_addr & mask))
				continue;
		}

		/* Display local ethernet address */
		ea = lp->ea;
		printf("%x:%x:%x:%x:%x:%x",
		    ea[0], ea[1], ea[2], ea[3], ea[4], ea[5]);

		/* Optionally display local ip address and interface name */
		if (lflag) {
			printf("\t%s\t%.*s", inet_ntoa(*lp->inp),
			    (int)sizeof(ifrp->ifr_name), lp->name);
		}
		putchar('\n');
		num++;
	}

	/* We're only supposed to display one if a host was specified */
	if (hostip && num > 1) {
		fprintf(stderr,
		    "%s: More than one interface on same subnet as %s\n",
		    prog, host);
		exit(1);
	}

	/* If we displayed any, exit normally */
	if (num)
		exit(0);

	/* Complain and exit with a bad status if we didn't find any */
	if (hostip)
		fprintf(stderr,
		    "%s: Can't find interface on same subnet as %s\n",
		    prog, host);
	else
		fprintf(stderr, "%s: no network interfaces found\n", prog);
	exit(1);
	/*NOTREACHED*/
}
