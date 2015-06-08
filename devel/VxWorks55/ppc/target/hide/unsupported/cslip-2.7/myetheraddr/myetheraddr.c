/*
 * Copyright (c) 1990, 1992 The Regents of the University of California.
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
    "Copyright (c) 1990, 1992 Lawrence Berkeley Laboratory\nAll rights reserved.\n";
static char rcsid[] =
    "@(#) $Header: myetheraddr.c,v 1.6 92/03/17 18:43:30 leres Exp $ (LBL)";
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
 * Note: This program takes advantage of the fact that the ifnet struct
 * is really part of the arpcom struct.
 *
 */

#include <stdio.h>
#include <nlist.h>

/* XXX SunOS 4.1 defines this and 3.5 doesn't... */
#ifdef _nlist_h
#define SUNOS4
#endif

#include <sys/types.h>
#include <kvm.h>
#include <fcntl.h>
#include <arpa/inet.h>

#include <sys/socket.h>
#include <net/if.h>

#include <netinet/in.h>
#ifdef SUNOS4
#include <netinet/in_var.h>
#endif
#include <netinet/if_ether.h>

#include <netdb.h>

/* Cast a struct sockaddr to a structaddr_in */
#define SATOSIN(sa) ((struct sockaddr_in *)(sa))

/* Determine if "bits" is set in "flag" */
#define ALLSET(flag, bits) (((flag) & (bits)) == (bits))

static struct nlist nl[] = {
#define N_IFNET 0
	{ "_ifnet" },
	0
};

void kread();

static char *prog;
static kvm_t *kd;

extern char *optarg;
extern int optind, opterr;

main(argc, argv)
	int argc;
	char **argv;
{
	register char *cp;
	register struct ifnet *ifp;
	register struct arpcom *ac;
	register u_char *ep;
	register int op;
	struct arpcom arpcom;
	struct in_addr *inp;
	struct hostent *hp;
#ifdef SUNOS4
	register struct ifaddr *ifa;
	register struct in_ifaddr *in;
	union {
		struct ifaddr ifa;
		struct in_ifaddr in;
	} ifaddr;
#endif
	u_long addr, hostip, mask;
	char name[16];
	char *host;
	int lflag, num;
	char *usage = "usage: %s [-l] [host]\n";

	/* Determine simple program name for error messages */
	if (cp = (char *)rindex(argv[0], '/'))
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
			(void)fprintf(stderr, usage, prog);
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
		if ((long)hostip == -1) {
			hp = gethostbyname(host);
			if (hp == 0) {
				fprintf(stderr, "%s: bad host \"%s\"\n",
				    prog, host);
				exit(1);
			}
			/*
			 * XXX The host might have more than one ip
			 * address. If so, we just blindly use the
			 * first one. Note that a modern named will
			 * order multiple addresses so that the first
			 * one is on a local interface so this isn't as
			 * bad as it seems.
			 */
			bcopy(hp->h_addr, (char *)&hostip, sizeof(hostip));
		}
		break;

	default:
		(void)fprintf(stderr, usage, prog);
		exit(1);
	}

	/* Open kernel memory for reading */
	kd = kvm_open(0, 0, 0, O_RDONLY, prog);
	if (kd == 0) {
		perror("kvm_open");
		exit(1);
	}

	/* Fetch namelist */
	if (kvm_nlist(kd, nl) != 0) {
		fprintf(stderr, "%s: bad nlist\n", prog);
		exit(1);
	}

	ac = &arpcom;
	ifp = &arpcom.ac_if;
	ep = arpcom.ac_enaddr.ether_addr_octet;
#ifdef SUNOS4
	ifa = &ifaddr.ifa;
	in = &ifaddr.in;
#endif
	num = 0;
	kread(nl[N_IFNET].n_value, (char *)&addr, sizeof(addr), "ifnet addr");
	for ( ; addr; addr = (u_long)ifp->if_next) {
		kread(addr, (char *)ac, sizeof(*ac), "ifnet");
		/* Only look at configured, broadcast interfaces */
		if (!ALLSET(ifp->if_flags, IFF_UP | IFF_BROADCAST))
			continue;
#ifdef SUNOS4
		/* This probably can't happen... */
		if (ifp->if_addrlist == 0)
			continue;
#endif
		/* Get interface ip address if necessary */
		if (hostip || lflag) {
#ifdef SUNOS4
			kread((u_long)ifp->if_addrlist, (char *)&ifaddr,
			    sizeof(ifaddr), "ifaddr");
			inp = &SATOSIN(&ifa->ifa_addr)->sin_addr;
#else
			inp = &SATOSIN(&ifp->if_addr)->sin_addr;
#endif
			/* Check if this interface on the right subnet */
			if (hostip) {
#ifdef SUNOS4
				mask = in->ia_subnetmask;
#else
				mask = ifp->if_subnetmask;
#endif
				/* Skip if interface is not on correct subnet */
				if ((hostip & mask) != (inp->s_addr & mask))
					continue;
			}
		}

		/* Display local ethernet address */
		printf("%x:%x:%x:%x:%x:%x",
		    ep[0], ep[1], ep[2], ep[3], ep[4], ep[5], ep[6]);

		/* Optionally display local ip address and interface name */
		if (lflag) {
			kread((u_long)ifp->if_name,
			    (char *)name, sizeof(name), "name");
			name[sizeof(name) - 1] = '\0';
			printf("\t%s\t%s%d",
			    inet_ntoa(*inp), name, ifp->if_unit);
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

void
kread(addr, buf, size, errmsg)
	u_long addr;
	char *buf;
	u_int size;
	char *errmsg;
{
	if (kvm_read(kd, addr, buf, size) != size) {
		fprintf(stderr, "%s: %s read\n", prog, errmsg);
		exit(1);
		/*NOTREACHED*/
	}
}
