/*
 * Modified by Akihiro Tominaga. (tomy@sfc.wide.ad.jp)
 */
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
#if !defined(sony_news) && !defined(__FreeBSD__) && !defined(__osf__)

#ifdef lint
static char rcsid[] =
    "@(#) $Header: myetheraddr.c,v 1.6 92/03/17 18:43:30 leres Exp $ (LBL)";
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <nlist.h>
#include <syslog.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/proc.h>
#include <kvm.h>
#include <fcntl.h>

#include <sys/socket.h>
#include <net/if.h>

#include <netinet/in.h>
#include <netinet/in_var.h>
#include <netinet/if_ether.h>
#include <arpa/inet.h>

/* Cast a struct sockaddr to a sockaddr_in */
#define SATOSIN(sa) ((struct sockaddr_in *)(sa))

/* Determine if "bits" is set in "flag" */
#define ALLSET(flag, bits) (((flag) & (bits)) == (bits))

static struct nlist nl[] = {
#define N_IFNET 0
	{ "_ifnet" },
	{ 0 }
};


int
getmac(ifname, result)
  char *ifname;
  char *result;
{
  register struct ifnet *ifp = NULL;
  register struct arpcom *ac = NULL;
  register u_char *ep = NULL;
  struct arpcom arpcom;
  register struct ifaddr *ifa = NULL;
  register struct in_ifaddr *in = NULL;
  union {
    struct ifaddr ifa;
    struct in_ifaddr in;
  } ifaddr;
  u_long addr = 0;
  char name[16];
  char fullname[16];
  static kvm_t *kd = NULL;

  bzero(&arpcom, sizeof(arpcom));
  bzero(&ifaddr, sizeof(ifaddr));
  bzero(name, sizeof(name));
  bzero(fullname, sizeof(fullname));

  /* Open kernel memory for reading */
  kd = kvm_open(0, 0, 0, O_RDONLY, "kvm_open");
  if (kd == 0) {
    syslog(LOG_ERR, "kvm_open() error in getmac(): %m");
    return(-1);
  }

  /* Fetch namelist */
  if (kvm_nlist(kd, nl) != 0) {
    syslog(LOG_ERR, "bad nlist in getmac(): %m");
    kvm_close(kd);
    return(-1);
  }

  ac = &arpcom;
  ifp = &arpcom.ac_if;
#ifdef __bsdi__
  ep = arpcom.ac_enaddr;
#else
  ep = arpcom.ac_enaddr.ether_addr_octet;
#endif
  ifa = &ifaddr.ifa;
  in = &ifaddr.in;

  if (kvm_read(kd, nl[N_IFNET].n_value, (char *)&addr, sizeof(addr)) !=
               sizeof(addr)) {
    syslog(LOG_ERR, "kvm_read() error in getmac(): %m");
    kvm_close(kd);
    return(-1);
  }
  for ( ; addr; addr = (u_long)ifp->if_next) {
    if (kvm_read(kd, addr, (char *)ac, sizeof(*ac)) != sizeof(*ac)) {
      syslog(LOG_ERR, "kvm_read() error in getmac(): %m");
      kvm_close(kd);
      return(-1);
    }
    /* Only look at configured, broadcast interfaces */
    if (!ALLSET(ifp->if_flags, IFF_UP | IFF_BROADCAST))
      continue;

    /* Only look at the specified interface */
    if (kvm_read(kd, (u_long)ifp->if_name, (char *)name, sizeof(name)) !=
                 sizeof(name)) {
      syslog(LOG_ERR, "kvm_read() error in getmac(): %m");
      kvm_close(kd);
      return(-1);
    }
    name[sizeof(name) - 1] = '\0';
    sprintf(fullname, "%s%d", name, ifp->if_unit);
    if (strcmp(fullname, ifname) != 0)
      continue;

    kvm_close(kd);
    bcopy(ep, result, 6);
    return(0);
  }

  kvm_close(kd);
  errno = 0;
  syslog(LOG_ERR, "getmac() is failed");
  return(-1);
}
#endif
