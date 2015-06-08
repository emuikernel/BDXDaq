/*
 * Copyright (c) 1983, 1989 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
/*
 * Some part are modified by kei@cs.uec.ac.jp and tomy@sfc.wide.ad.jp.
 */

#if !defined(__bsdi__) && !defined(__FreeBSD__)

#include <stdio.h>
#include <nlist.h>
#include <syslog.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/mbuf.h>
#include <sys/ioctl.h>
#include <net/route.h>
#include <netinet/in.h>
#include <rpc/types.h>

#ifdef __osf__
#define rtentry ortentry
#endif

/*
 * from Berkeley's code (route.c 5.6 (Berkeley) 6/5/86)
 */
static struct nlist nl[] = {
#define N_RTHOST        0
        { "_rthost" },
#define N_RTNET         1
        { "_rtnet" },
#define N_RTHASHSIZE    2
        { "_rthashsize" },
        { "" }
};

int
flushroutes()
{
  static int first = 1;
  int s = 0;
  struct mbuf mb;
  register struct rtentry *rt = NULL;
  register struct mbuf *m = NULL;
  struct mbuf **routehash = NULL;
  int rthashsize = 0, i = 0, doinghost = 1, kmem = 0;

  bzero(&mb, sizeof(mb));

  if ((s = socket(AF_INET, SOCK_RAW, 0)) < 0) {
    return(-1);
  }

  if (first) {
    first = 0;
    nlist("/vmunix", nl);
  }
  if (nl[N_RTHOST].n_value == 0) {
    syslog(LOG_LOCAL0|LOG_ERR, "\"rthost\", symbol not in namelist");
    exit(1);
  }
  if (nl[N_RTNET].n_value == 0) {
    syslog(LOG_LOCAL0|LOG_ERR, "\"rtnet\", symbol not in namelist");
    exit(1);
  }
  if (nl[N_RTHASHSIZE].n_value == 0) {
    syslog(LOG_LOCAL0|LOG_ERR,
	   "\"rthashsize\", symbol not in namelist");
    exit(1);
  }
  kmem = open("/dev/kmem", 0);
  if (kmem < 0) {
    syslog(LOG_LOCAL0|LOG_ERR, "/dev/kmem: %m");
    exit(1);
  }
  lseek(kmem, nl[N_RTHASHSIZE].n_value, 0);
  read(kmem, &rthashsize, sizeof (rthashsize));
  routehash = (struct mbuf **)calloc(1, rthashsize*sizeof (struct mbuf *));

  lseek(kmem, nl[N_RTHOST].n_value, 0);
  read(kmem, routehash, rthashsize*sizeof (struct mbuf *));
again:
  for (i = 0; i < rthashsize; i++) {
    if (routehash[i] == 0)
      continue;
    m = routehash[i];
    while (m) {
      lseek(kmem, m, 0);
      read(kmem, &mb, sizeof (mb));
      rt = mtod(&mb, struct rtentry *);
      if (((struct sockaddr_in *)&rt->rt_dst)->sin_addr.s_addr
	  != INADDR_LOOPBACK) {
	ioctl(s, SIOCDELRT, (caddr_t)rt);
      }
      m = mb.m_next;
    }
  }
  if (doinghost) {
    lseek(kmem, nl[N_RTNET].n_value, 0);
    read(kmem, routehash, rthashsize*sizeof (struct mbuf *));
    doinghost = 0;
    goto again;
  }
  close(kmem);
  close(s);
  free(routehash);
  return(0);
}
#else /* __bsdi__ */

#ifndef BSDOS    /* It's BSD/386 1.? */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/kinfo.h>
#include <net/route.h>

/*
 * Purge all entries in the routing tables not
 * associated with network interfaces.
 */
int
flushroutes()
{
  int needed = 0, seqno = 0, rlen = 0, s = 0;
  char *buf = NULL, *next = NULL, *lim = NULL;
  register struct rt_msghdr *rtm = NULL;
  struct sockaddr_in *sa = NULL;
  
  if ((s = socket(PF_ROUTE, SOCK_RAW, 0)) < 0) {
    syslog(LOG_WARNING, "socket() error in flushroutes(): %m");
    return(-1);
  }
  
  shutdown(s, 0); /* Don't want to read back our messages */
  if ((needed = getkerninfo(KINFO_RT_DUMP, 0, 0, 0)) < 0) {
    syslog(LOG_WARNING, "route-getkerninfo-estimate: %m");
    close(s);
    return(-1);
  }
  if ((buf = calloc(1, needed)) == NULL) {
    syslog(LOG_WARNING, "calloc() error in flushroutes(): %m");
    close(s);
    return(-1);
  }
  if ((rlen = getkerninfo(KINFO_RT_DUMP, buf, &needed, 0)) < 0) {
    syslog(LOG_WARNING, "actual retrieval of routing table: %m");
    free(buf);
    close(s);
    return(-1);
  }
  lim = buf + rlen;
  seqno = 0;		/* ??? */
  for (next = buf; next < lim; next += rtm->rtm_msglen) {
    rtm = (struct rt_msghdr *)next;
    if ((rtm->rtm_flags & RTF_GATEWAY) == 0)
      if ((rtm->rtm_flags & RTF_HOST) != 0)
	continue;
    rtm->rtm_type = RTM_DELETE;
    rtm->rtm_seq = seqno;
    rlen = write(s, next, rtm->rtm_msglen);
    if (rlen < (int)rtm->rtm_msglen) {
      syslog(LOG_WARNING, "write() error to routing socket");
      errno = 0;
      syslog(LOG_WARNING, "got only %d for rlen", rlen);
      free(buf);
      close(s);
      return(-1);
    }

    sa = (struct sockaddr_in *)(rtm + 1);
    seqno++;
  }

  free(buf);
  close(s);
  return(0);
}

#else  /* It's BSD/OS 2.0 or later */

/*
 * Modified by tomy@sfc.wide.ad.jp
 */
/*
 *
 * Copyright (c) 1983, 1989, 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef lint
static char sccsid[] = "@(#)route.c	8.3 (Berkeley) 3/19/94";
#endif /* not lint */

#include <sys/param.h> 
#include <sys/socket.h>
#include <sys/sysctl.h>
#include <net/route.h>
#include <unistd.h> 
#include <stdlib.h> 
#include <syslog.h>

/*
 * Purge all entries in the routing tables not
 * associated with network interfaces.
 */

int
flushroutes()
{
  int s;
  size_t needed;
  int mib[6], rlen, seqno;
  char *buf, *next, *lim;
  register struct rt_msghdr *rtm;

  if ((s = socket(PF_ROUTE, SOCK_RAW, 0)) < 0) {
    syslog(LOG_WARNING, "socket() error in flushroutes(): %m");
    return(-1);
  }

  shutdown(s, 0);
  mib[0] = CTL_NET;
  mib[1] = PF_ROUTE;
  mib[2] = 0;             /* protocol */
  mib[3] = 0;             /* wildcard address family */
  mib[4] = NET_RT_DUMP;
  mib[5] = 0;             /* no flags */

  if (sysctl(mib, 6, NULL, &needed, NULL, 0) < 0) {
    syslog(LOG_WARNING, "sysctl() error in flushroutes(): %m");
    close(s);
    return(-1);
  }
  if ((buf = calloc(1, needed)) == NULL) {
    syslog(LOG_WARNING, "calloc() error in flushroutes(): %m");
    close(s);
    return(-1);
  }
  if (sysctl(mib, 6, buf, &needed, NULL, 0) < 0) {
    syslog(LOG_WARNING, "sysctl() error in flushroutes(): %m");
    free(buf);
    close(s);
    return(-1);
  }
  lim = buf + needed;
  seqno = 0;

  for (next = buf; next < lim; next += rtm->rtm_msglen) {
    rtm = (struct rt_msghdr *) next;

    if ((rtm->rtm_flags & (RTF_GATEWAY | RTF_LLINFO)) == 0)
      continue;
    rtm->rtm_type = RTM_DELETE;
    rtm->rtm_seq = seqno;
    rlen = write(s, next, rtm->rtm_msglen);
    if (rlen < (int)rtm->rtm_msglen) {
      break;
    }
    seqno++;
  }

  free(buf);
  close(s);
  return(0);
}
#endif /* BSDOS */
#endif /* __bsdi__ */
