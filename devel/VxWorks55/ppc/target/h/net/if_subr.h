/* if_subr.h - header for common routines for network interface drivers */

/* Copyright 1984 - 2000 Wind River Systems, Inc. */
/*
modification history
--------------------
01n,17oct00,spm  updated ether_attach to report memory allocation failures
01m,15may97,vin  added additional type declarations.
01l,06sep93,jcf  added prototype for netTypeInit.
01k,22sep92,rrr  added support for c++
01j,23jun92,ajm  added prototype for do_protocol_with_type
01i,06jun92,elh  added includes.
01h,04jun92,ajm  Fixed erroneous ansi prototype.
01g,26may92,rrr  the tree shuffle
01f,01apr92,elh  Added ansi prototypes.  Added functions.
01e,04oct91,rrr  passed through the ansification filter
		  -changed VOID to void
		  -changed copyright notice
01d,19jul91,hdn  moved SIZEOF_ETHERHEADER to if_ether.h.
01c,29apr91,hdn  added defines and macros for TRON architecture.
01b,05oct90,shl  added copyright notice.
                 made #endif ANSI style.
01a,10aug90,dnw  written
*/

#ifndef __INCif_subrh
#define __INCif_subrh

#ifdef __cplusplus
extern "C" {
#endif

/* includes */

#include "net/if.h"
#include "netinet/if_ether.h"
#include "net/mbuf.h"

/* this macro directly calls ether_input function. */

#define do_protocol(eh, m, pArpcom, len)				\
	ether_input((struct ifnet *)pArpcom, eh, m)

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern void do_protocol_with_type (u_short type, struct mbuf *m,
                                   struct arpcom *pArpcom, int len);

extern int set_if_addr (struct ifnet *ifp, char *data, u_char *enaddr);

extern STATUS ether_attach (struct ifnet *ifp, int unit, char *name,
		          FUNCPTR initRtn, FUNCPTR ioctlRtn, FUNCPTR outputRtn,
			  FUNCPTR resetRtn);

extern void ether_ifattach (struct ifnet *);
extern void ether_input (struct ifnet *, struct ether_header *, struct mbuf *);
extern int  ether_output (struct ifnet *, struct mbuf *, struct sockaddr *, 
                          struct rtentry *);
extern char    *ether_sprintf (u_char *);


extern STATUS netTypeAdd (int etherType, FUNCPTR inputRtn);
extern STATUS netTypeDelete (int etherType);
extern void netTypeInit ();

#else

extern void do_protocol ();
extern void do_protocol_with_type ();
extern int ether_output ();
extern int set_if_addr ();
extern void ether_attach ();
extern STATUS netTypeAdd ();
extern STATUS netTypeDelete ();
extern void netTypeInit ();
extern void ether_ifattach ();
extern void ether_input ();
extern char * ether_sprintf ();

#endif	/* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCif_subrh */
