/* if_loop.c - software loopback network interface driver */

/* Copyright 1984 - 2002 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*	$NetBSD: if_loop.c,v 1.13 1994/10/30 21:48:50 cgd Exp $	*/

/*
 * Copyright (c) 1982, 1986, 1993
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
 *
 *	@(#)if_loop.c	8.1 (Berkeley) 6/10/93
 */

/*
modification history
--------------------
02d,07may02,wap  Need to use netJobAdd() again (at least for now) to avoid
                 infinite recursion problem (SPR #76568)
02c,15apr02,wap  remove netJobAdd() in keeping with new behavior in if_subr.c
02b,12oct01,rae  merge from truestack ver. 02e, base 02a (SPR #69112)
02a,19mar96,vin  upgraded to BSD4.4, integrated with 01o of if_loop.c.
01o,11aug93,jmm  Changed ioctl.h and socket.h to sys/ioctl.h and sys/socket.h
01n,22feb93,jdi  documentation cleanup.
01m,09sep92,gae  documentation tweaks.
01l,26may92,rrr  the tree shuffle
		  -changed includes to have absolute path from h/
01k,04oct91,rrr  passed through the ansification filter
                  -changed functions to ansi style
		  -changed includes to have absolute path from h/
		  -changed copyright notice
01j,02oct90,hjb  we now always turn IFF_NOTRAILERS on for loopback device
		   and turn IFF_RUNNING on when needed.
02i,19sep90,dab  made loattach() return STATUS.
02h,11jul90,hjb  removed references to ipintrPending(), netTaskSemId and
		 added a call to schednetisr().
02g,19apr90,hjb  deletd param.h, de-linted
02f,18mar90,hjb  reduction of redundant code, performance enhancement
02f,16apr89,gae  updated to new 4.3BSD.
02e,30may88,dnw  changed to v4 names.
02d,05jan88,rdc  added include of systm.h
02c,02may87,dnw  clean up.
02b,03apr87,ecs  added copyright.
02a,02feb87,jlf  Removed CLEAN ifdefs.
*/

/*
DESCRIPTION
This module implements the software loopback network interface driver.
The only user-callable routine is loattach(), which publishes the `lo'
interface and initializes the driver and device.

This interface is used for protocol testing and timing.
By default, the loopback interface is accessible at Internet
address 127.0.0.1.

To use this feature, include the following component:
INCLUDE_LOOPBACK 

BOARD LAYOUT
This device is "software only."  A jumpering diagram is not applicable.

SEE ALSO: ifLib
*/

#include "vxWorks.h"
#include "tickLib.h"
#include "netLib.h"
#include "logLib.h"
#include "net/mbuf.h"
#include "sys/socket.h"
#include "errno.h"
#include "sys/ioctl.h"

#include "net/if.h"
#include "net/route.h"
#include "net/systm.h"
#include "net/if_types.h"

#ifdef	INET
#include "netinet/in.h"
#include "netinet/in_systm.h"
#include "netinet/in_var.h"
#include "netinet/ip_var.h"    /* for ipintr() prototype */
#include "netinet/ip.h"
#endif

#ifdef NS
#include "../netns/ns.h"
#include "../netns/ns_if.h"
#endif

#ifdef VIRTUAL_STACK
#include "netinet/vsLib.h"
#endif

#ifndef VIRTUAL_STACK
#define NLOOP	1      /* currently only one */
#endif /* VIRTUAL_STACK */

#define	LOMTU	(1024*32)	/* maximum packet size */


#ifndef VIRTUAL_STACK
struct ifnet loif [NLOOP];
#endif /* VIRTUAL_STACK */

/* forward declarations */

int looutput ();
static int loioctl (struct ifnet *ifp, int cmd, caddr_t data);
static void lortrequest (int cmd, struct rtentry * rt, struct sockaddr * sa);

/*******************************************************************************
*
* loattach - publish the `lo' network interface and initialize the driver and pseudo-device
*
* This routine attaches an `lo' Ethernet interface to the network, if the
* interface exists.  It makes the interface available by filling in
* the network interface record.  The system initializes the interface
* when it is ready to accept packets.
*
* RETURNS: OK.
*/

STATUS loattach (void)
    {
    FAST struct ifnet *	ifp;
    FAST int		ix;

    for (ix = 0; ix < NLOOP; ix++) 
	{
	ifp = &loif[ix];
	bzero ((char *)ifp, sizeof (struct ifnet));
	ifp->if_unit = ix;
	ifp->if_name = "lo";
	ifp->if_mtu = LOMTU;
	ifp->if_flags = IFF_LOOPBACK | IFF_MULTICAST;
	ifp->if_ioctl = loioctl;
	ifp->if_output = looutput;
	ifp->if_type = IFT_LOOP;
	ifp->if_hdrlen = 0;
	ifp->if_addrlen = 0;
        ifp->pCookie = NULL;
#ifdef VIRTUAL_STACK
        ifp->vsNum = myStackNum; /* Assign the VS Num to the IFNET for */
                                 /* processing by tNetTask on reception */
                                 /* of packets. */
#endif
	if_attach(ifp);
	}
    return (OK);
    }

/*******************************************************************************
*
* looutput - output packet on loopback interface
*
* NOMANUAL
*/

int looutput
    (
    struct ifnet *	ifp,
    FAST struct mbuf *	m,
    struct sockaddr *	dst,
    FAST struct rtentry *rt
    )
    {
    int 		s;

    if ((m->m_flags & M_PKTHDR) == 0)
	panic("looutput no HDR");

    ifp->if_lastchange = tickGet();

#if NBPFILTER > 0
    if (ifp->if_bpf) 
	{
	/*
	 * We need to prepend the address family as
	 * a four byte field.  Cons up a dummy header
	 * to pacify bpf.  This is safe because bpf
	 * will only read from the mbuf (i.e., it won't
	 * try to free it or keep a pointer to it).
	 */
	struct mbuf m0;
	u_int af = dst->sa_family;

	m0.m_next = m;
	m0.m_len = 4;
	m0.m_data = (char *)&af;

	bpf_mtap(ifp->if_bpf, &m0);
	}
#endif
    m->m_pkthdr.rcvif = ifp;

    if (rt && rt->rt_flags & (RTF_REJECT|RTF_BLACKHOLE)) 
	{
	m_freem(m);
	return (rt->rt_flags & RTF_BLACKHOLE ? 0 :
		rt->rt_flags & RTF_HOST ? EHOSTUNREACH : ENETUNREACH);
	}
    ifp->if_opackets++;
    ifp->if_obytes += m->m_pkthdr.len;
    switch (dst->sa_family) 
	{

#ifdef INET
	case AF_INET:
	    netJobAdd ((FUNCPTR)ipintr, (int)m, 0, 0, 0, 0);
	    break;
#endif
	default:
	    logMsg("lo%d: can't handle af%d\n", (int)ifp->if_unit,
		   (int)dst->sa_family,0,0,0,0);
	    m_freem(m);
	    return (EAFNOSUPPORT);
	}

    s = splimp();
    ifp->if_ipackets++;
    ifp->if_ibytes += m->m_pkthdr.len;
    splx(s);
    return (0);
    }

/*******************************************************************************
*
* lortrequest - handle route request for loop back
*
* NOMANUAL
*/

void lortrequest
    (
    int 		cmd,
    struct rtentry *	rt,
    struct sockaddr *	sa
    )
    {
    if (rt)
	rt->rt_rmx.rmx_mtu = LOMTU;
    }

/*******************************************************************************
*
* loioctl - handle loop ioctl request
*/

LOCAL int loioctl
    (
    FAST struct ifnet *	ifp,
    int 		cmd,
    caddr_t 		data
    )
    {
    FAST struct ifaddr *ifa;
    FAST struct ifreq *	ifr;
    FAST int 		error = 0;

    switch (cmd)
	{
	case SIOCSIFADDR:
	    ifp->if_flags |= (IFF_UP | IFF_RUNNING);

	    ifa = (struct ifaddr *)data;

	    if (ifa != 0 && ifa->ifa_addr->sa_family == AF_ISO)
		ifa->ifa_rtrequest = lortrequest;

	    /* Everything else is done at a higher level. */
	    break;
	case SIOCADDMULTI:
	case SIOCDELMULTI:
	    ifr = (struct ifreq *)data;
	    if (ifr == 0)
		{
		error = EAFNOSUPPORT;		/* XXX */
		break;
		}

	    switch (ifr->ifr_addr.sa_family)
		{
#ifdef INET
		case AF_INET:
		    break;
#endif
		default:
		    error = EAFNOSUPPORT;
		    break;
		}
	    break;

	default:
	    error = EINVAL;
	}
    return (error);
    }
