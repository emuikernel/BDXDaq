/*
 * Copyright (c) 1990, 1991, 1992 The Regents of the University of
 * California. All rights reserved.
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
    "@(#) $Header: slinfo.c,v 1.18 92/06/18 16:42:31 leres Exp $ (LBL)";
#endif

#include <stdio.h>
#include <nlist.h>

/* XXX SunOS 4.1 defines this and 3.5 doesn't... */
#ifdef sun
#ifdef _nlist_h
#define SUNOS4
#define NEWBSD
#define STREAMS
#endif
#endif

#ifdef hp300
#define NEWBSD
#endif

#include <ctype.h>

#include <sys/param.h>
#include <sys/mbuf.h>
#include <sys/buf.h>
#include <sys/map.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <strings.h>

#include <sys/time.h>
#include <sys/kernel.h>
#include <sys/ioctl.h>

#include <kvm.h>
#include <fcntl.h>

#include <net/if.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#ifdef NEWBSD
#include <netinet/in_var.h>
#endif
#ifdef STREAMS
#include <sys/stream.h>
#endif

#include <arpa/inet.h>

#ifdef notdef
/* ??? */
#include <sundev/mbvar.h>
#endif

#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <net/slcompress.h>
#include <net/if_slvar.h>

#define satosin(dst) ((struct sockaddr_in *)(dst))

static struct nlist nl[] = {
#define NL_IFNET 0
	{ "_ifnet" },
#define NL_RTHASHSIZE 1
	{ "_rthashsize" },
#define NL_RTHOST 2
	{ "_rthost" },
	{ "" }
};

static char *prog;
static kvm_t *kd;

void kread();
void printb();

main(argc, argv)
	int argc;
	char **argv;
{
	register char *cp;
	struct sl_softc sl_softc;
	register struct sl_softc *sc;
	char *myname = "sl";
	register struct ifnet *ifp;
#ifdef NEWBSD
	union {
		struct ifaddr ifa;
		struct in_ifaddr in;
	} ifaddr;
#endif
	u_long addr;
	char name[16];
	char *vmunix = 0;
	char *kmem = 0;
	int unit = 0;

	if (cp = rindex(argv[0], '/'))
		prog = cp + 1;
	else
		prog = argv[0];

	if (argc > 1 && isdigit(argv[1][0])) {
		unit = atoi(*++argv);
		--argc;
	}
	if (argc > 1)
		vmunix = argv[1];
	if (argc > 2)
		kmem = argv[2];

	if ((kd = kvm_open(vmunix, kmem, (void *)0, O_RDONLY, prog)) == 0) {
		exit(1);
		/* NOTREACHED */
	}
	if (kvm_nlist(kd, nl) < 0) {
		fprintf(stderr, "%s: kvm_nlist\n", prog);
		exit(1);
		/* NOTREACHED */
	}
	if (nl[NL_IFNET].n_type == 0) {
		fprintf(stderr, "%s: no namelist\n", prog);
		exit(1);
		/* NOTREACHED */
	}

	/* Loop through ifnet's looking for ours */
	kread(nl[NL_IFNET].n_value, (char *)&addr, sizeof(addr), "ifnet addr");
	sc = &sl_softc;
	ifp = &sc->sc_if;
	for (; addr != 0; addr = (u_long)ifp->if_next) {
		/* Read ifnet */
		kread(addr, (char *)ifp, sizeof(*ifp), "ifnet");
		if (ifp->if_unit != unit)
			continue;

		/* Read interface name */
		kread((u_long)ifp->if_name, name, sizeof(name), "name");
		name[sizeof(name) - 1] = '\0';
		if (strcmp(myname, name) == 0)
			break;
	}
	if (addr == 0) {
		fprintf(stderr, "%s: Can't find interface %s%d\n",
		    prog, myname, unit);
		exit(1);
		/* NOTREACHED */
	}

	/* Read softc */
	kread(addr, (char *)sc, sizeof(*sc), "softc");

#ifdef NEWBSD
/* SunOS 4.0 code */

	/* Get ifaddr struct */
	/* XXX really should check to see if there is more than one */
	if (addr = (u_long)ifp->if_addrlist)
		kread(addr, (char *)&ifaddr, sizeof(ifaddr), "ifaddr");
#define NONE(s) (addr ? s : "none")

	printf("%15s %15s %15s %15s\n",
	    "if_unit", "if_mtu", "if_addr", "ia_net");
	printf("%15d %15d %15s",
	    ifp->if_unit,
	    ifp->if_mtu,
	    NONE(inet_ntoa(satosin(&ifaddr.ifa.ifa_addr)->sin_addr)));
	printf(" %15s\n",
	    NONE(inet_ntoa(*(struct in_addr *)&ifaddr.in.ia_net)));
	printf("%15s %15s %15s %15s",
	    "ifa_dstaddr", "ia_netmask", "ia_subnet", "ia_subnetmask");
	if (addr && ifaddr.ifa.ifa_next)
		printf(" %15s", "ifa_next");
	putchar('\n');
	printf("%15s",
	    NONE(inet_ntoa(satosin(&ifaddr.ifa.ifa_dstaddr)->sin_addr)));
	printf(" %15s",
	    NONE(inet_ntoa(*(struct in_addr *)&ifaddr.in.ia_netmask)));
	printf(" %15s",
	    NONE(inet_ntoa(*(struct in_addr *)&ifaddr.in.ia_subnet)));
	printf(" %15s",
	    NONE(inet_ntoa(*(struct in_addr *)&ifaddr.in.ia_subnetmask)));
	if (addr && ifaddr.ifa.ifa_next) {
		char buf[32];
		sprintf(buf, "0x%x", ifaddr.ifa.ifa_next);
		printf(" %15s", buf);
	}
	putchar('\n');
	putchar('\n');

#define	IFFBITS \
"\020\1UP\2BROADCAST\3DEBUG\4LOOPBACK\5POINTOPOINT\6NOTRAILERS\7RUNNING\10NOARP\
\11PROMISC\12ALLMULTI\14MULTICAST\15LINK0\16LINK1\17LINK2\20PRIVATE"
#else
/* SunOS 3.5 code */
	printf("%15s %15s %15s %15s %15s\n",
	    "if_unit", "if_mtu", "if_addr", "if_net", "if_host[0]");
	printf("%15d %15d %15s",
	    ifp->if_unit,
	    ifp->if_mtu,
	    inet_ntoa(satosin(&ifp->if_addr)->sin_addr));
	printf(" %15s", inet_ntoa(*(struct in_addr *)&ifp->if_net));
	printf(" %15s\n", inet_ntoa(*(struct in_addr *)&ifp->if_host[0]));
	printf("%15s %15s %15s %15s\n",
	    "if_dstaddr", "if_netmask", "if_subnet", "if_subnetmask");
	printf("%15s", inet_ntoa(satosin(&ifp->if_dstaddr)->sin_addr));
	printf(" %15s", inet_ntoa(*(struct in_addr *)&ifp->if_netmask));
	printf(" %15s", inet_ntoa(*(struct in_addr *)&ifp->if_subnet));
	printf(" %15s", inet_ntoa(*(struct in_addr *)&ifp->if_subnetmask));
	putchar('\n');
	putchar('\n');

#define	IFFBITS \
"\020\1UP\2BROADCAST\3DEBUG\4ROUTE\5POINTOPOINT\6NOTRAILERS\7RUNNING\10NOARP\
\11PROMISC\12LOOPBACK\16LINK2\17LINK1\20LINK0"
#endif
	printf("%15s ", "if_flags");
	printb(ifp->if_flags, IFFBITS);
	putchar('\n');

	printf("%15s %15s %15s %15s %15s\n",
	    "if_ipackets", "if_ierrors",
	    "if_opackets", "if_oerrors",
	    "if_collisions");
	printf("%15d %15d %15d %15d %15d\n",
	    ifp->if_ipackets, ifp->if_ierrors,
	    ifp->if_opackets, ifp->if_oerrors,
	    ifp->if_collisions);

	printf("%15s %15s %15s %15s\n",
	    "if_timer", "ifq_len", "ifq_maxlen", "ifq_drops");
	printf("%15d %15d %15d %15d\n",
	    ifp->if_timer, ifp->if_snd.ifq_len,
	    ifp->if_snd.ifq_maxlen, ifp->if_snd.ifq_drops);
	putchar('\n');

	printf("%15s %15s %15s\n", "escape", "bytessent", "bytesrcvd");
	printf("%15u %15u %15u\n",
	    sc->sc_escape, sc->sc_bytessent, sc->sc_bytesrcvd);
	printf("%15s %15s %15s %15s\n",
		"packets", "compressed", "searches", "misses");
	printf("%15u %15u %15u %15u\n",
	    sc->sc_comp.sls_packets, sc->sc_comp.sls_compressed,
	    sc->sc_comp.sls_searches, sc->sc_comp.sls_misses);
	printf("%15s %15s %15s %15s\n",
		"uncompressedin", "compressedin", "errorin", "tossed");
	printf("%15u %15u %15u %15u\n",
	    sc->sc_comp.sls_uncompressedin, sc->sc_comp.sls_compressedin,
	    sc->sc_comp.sls_errorin, sc->sc_comp.sls_tossed);

#ifdef NEWBSD
	prstuff(satosin(&ifaddr.ifa.ifa_addr),
	    satosin(&ifaddr.ifa.ifa_dstaddr));
#else
	prstuff(satosin(&ifp->if_addr),
	    satosin(&ifp->if_dstaddr));
#endif
}

/*
 * Print a value a la the %b format of the kernel's printf
 */
void
printb(v, bits)
	register char *bits;
	register unsigned short v;
{
	register int i, any = 0;
	register char c;

	if (bits && *bits == 8)
		printf("%o", v);
	else
		printf("%x", v);
	bits++;
	if (bits) {
		printf(" (");
		while (i = *bits++) {
			if (v & (1 << (i-1))) {
				if (any)
					putchar(',');
				any = 1;
				for (; (c = *bits) > 32; bits++)
					putchar(c);
			} else
				for (; *bits > 32; bits++)
					;
		}
		putchar(')');
	}
}

#include <net/route.h>
/* XXX Van's hacks to route.h define this */
#ifdef RTV_LOCKED
#define PRSTUFF
#endif

extern char *malloc();

prstuff(gateway, dst)
	struct sockaddr_in *gateway, *dst;
{
	int hashsize;
	register int i;
	struct mbuf **routehash;
	register struct mbuf *m;
	struct mbuf mb;
	register struct rtentry *rt;

	if (nl[NL_RTHASHSIZE].n_type == 0) {
		fprintf(stderr, "%s: no namelist (rthashsize)\n", prog);
		return;
	}
	if (nl[NL_RTHOST].n_type == 0) {
		fprintf(stderr, "%s: no namelist (rthost)\n", prog);
		return;
	}
#ifndef PRSTUFF
	fprintf(stderr, "\n%s: prstuff() not implemented\n", prog);
#else
	kread(nl[NL_RTHASHSIZE].n_value, (char *)&hashsize,
	    sizeof hashsize, "hashsize");
	routehash = (struct mbuf **)malloc( hashsize*sizeof (struct mbuf *) );
	if (routehash == 0) {
		fprintf(stderr, "routehash malloc failed\n");
		return;
	}
	kread(nl[NL_RTHOST].n_value, (char *)routehash,
	    hashsize * sizeof(struct mbuf *), "routehash");
	for (i = 0; i < hashsize; i++)
		for (m = routehash[i]; m; ) {
			kread((u_long)m, (char *)&mb, sizeof mb, "mb");
			m = mb.m_next;
			rt = mtod(&mb, struct rtentry *);
			if (rt->rt_dst.sa_family != AF_INET)
				continue;
			if (bcmp((char *)gateway, (char *)&rt->rt_gateway,
			    sizeof(struct sockaddr_in)) != 0)
				continue;
			if (bcmp((char *)dst, (char *)&rt->rt_dst,
			    sizeof(struct sockaddr_in)) != 0)
				continue;
			printfullrt(mtod(&mb, struct fullrtentry *));
		}
#endif
}

#ifdef PRSTUFF
printfullrt(fr)
	register struct fullrtentry *fr;
{
	putchar('\n');
	printf("%15s %15s %15s %15s\n",
		"mtu", "maxhops", "recvpipe", "sendpipe");
	printf("%15d %15d %15d %15d\n",
	    fr->rt_mtu, fr->rt_maxhops, fr->rt_recvpipe, fr->rt_sendpipe);
	printf("%15s %15s\n",
		"rtt", "rttvar");
	printf("%15d %15d\n",
	    fr->rt_rtt, fr->rt_rttvar);
}
#endif

void
kread(addr, buf, size, errstr)
	u_long addr;
	char *buf;
	int size;
	char *errstr;
{

	if (kvm_read(kd, addr, buf, size) != size) {
		fprintf(stderr, "%s: kvm_read: ", prog);
		perror(errstr);
		exit(1);
		/* NOTREACHED */
	}
}
