/* ppp_vxworks.h - VxWorks dependent PPP routine header */

/* Copyright 1995 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,16jun95,dzb  header file consolidation.
01b,16jan95,dzb  changed to ppp_vxworks.h.  added function prototypes.
01a,22dec94,dzb  written.
*/

#ifndef __INCppp_vxworksh
#define __INCppp_vxworksh

#ifdef __cplusplus
extern "C" {
#endif

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern void	establish_ppp (void);
extern void	disestablish_ppp (void);
extern void	output (int unit, u_char *p, int len);
extern int	read_packet (u_char *buf);
extern void	ppp_send_config (int unit, int mtu, u_long asyncmap, int pcomp,
		    int accomp);
extern void	ppp_set_xaccm (int unit, ext_accm accm);
extern void	ppp_recv_config (int unit, int mru, u_long asyncmap, int pcomp,
		    int accomp);
extern int	sifvjcomp (int u, int vjcomp, int cidcomp, int maxcid);
extern int	sifup (int u);
extern int	sifdown (int u);
extern int	sifaddr (int u, int o, int h, int m);
extern int	cifaddr (int u, int o, int h);
extern int	sifdefaultroute (int u, int g);
extern int	cifdefaultroute (int u, int g);
extern int	sifproxyarp (int unit, u_long hisaddr);
extern int	cifproxyarp (int unit, u_long hisaddr);
extern int	get_ether_addr (u_long ipaddr, struct sockaddr *hwaddr);
extern int	ppp_available (void);
extern char	*stringdup (char *in);

#else	/* __STDC__ */

extern void	establish_ppp ();
extern void	disestablish_ppp ();
extern void	output ();
extern int	read_packet ();
extern void	ppp_send_config ();
extern void	ppp_set_xaccm ();
extern void	ppp_recv_config ();
extern int	sifvjcomp ();
extern int	sifup ();
extern int	sifdown ();
extern int	sifaddr ();
extern int	cifaddr ();
extern int	sifdefaultroute ();
extern int	cifdefaultroute ();
extern int	sifproxyarp ();
extern int	cifproxyarp ();
extern int	get_ether_addr ();
extern int	ppp_available ();
extern char	*stringdup ();

#endif	/* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCppp_vxworksh */
