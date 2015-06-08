/* pppShow.h - ppp show routine header */

/* Copyright 1995 Wind River Systems, Inc. */

/*
modification history
--------------------
01d,16jun95,dzb  header file consolidation.
01c,09may95,dzb  added pppSecretShow() prototype.
01b,09feb95,dab  removed lcp_echo_fails_reached variable.
01a,13jan95,dab  written as pppInfo.h.
	   +dzb  added: path for ppp header files, WRS copyright.  WRSize.
*/

#ifndef	__INCpppShowh
#define	__INCpppShowh

#ifdef __cplusplus
extern "C" {
#endif

/* includes */

#include "netinet/ppp/pppd.h"

/* typedefs */

typedef struct ppp_info			/* PPP_INFO */
    {
    /* LCP variables */

    lcp_options lcp_wantoptions;	/* Options that we want to request */
    lcp_options lcp_gotoptions;		/* Options that peer ack'd */
    lcp_options lcp_allowoptions;	/* Options we allow peer to request */
    lcp_options lcp_hisoptions;		/* Options that we ack'd */
    fsm lcp_fsm;			/* LCP state structure */

    u_long lcp_echos_pending;		/* Number of outstanding echo msgs */
    u_long lcp_echo_number;		/* ID number of next echo frame */
    u_long lcp_echo_timer_running;	/* TRUE if a timer running */
    u_long lcp_echo_interval;		/* Seconds between intervals */
    u_long lcp_echo_fails;		/* Number of echo failures */

    /* IPCP variables */

    ipcp_options ipcp_wantoptions;	/* Options that we want to request */
    ipcp_options ipcp_gotoptions;	/* Options that peer ack'd */
    ipcp_options ipcp_allowoptions;	/* Options we allow peer to request */
    ipcp_options ipcp_hisoptions;	/* Options that we ack'd */
    fsm ipcp_fsm;			/* IPCP state structure */

    /* authentication variables */

    chap_state chap;			/* CHAP state and variable structure */
    upap_state upap;			/* UPAP state and variable structure */
    } PPP_INFO;

typedef struct ppp_stat			/* PPP_STAT */
    {
					/* RECEVIED: */
    int in_bytes;			/* total number of bytes */
    int in_pkt;				/* total number of packets */
    int in_ip_pkt;			/* number of IP packets */
    int in_vj_compr_pkt;		/* number of VJ compressed packets */
    int in_vj_uncompr_pkt;		/* number of VJ uncompressed packets */
    int in_vj_compr_error;		/* number of VJ compression errors */

                                        /* SENT: */
    int out_bytes;			/* total number of bytes */
    int out_pkt;			/* total number of packets */
    int out_ip_pkt;			/* number of IP packets */
    int out_vj_compr_pkt;		/* number of VJ compressed packets */
    int out_vj_uncompr_pkt;		/* number of VJ uncompressed packets */
    } PPP_STAT;

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern void	pppShowInit (void);
extern void	pppInfoShow (void);
extern STATUS	pppInfoGet (int unit, PPP_INFO *pInfo);
extern void	pppstatShow (void);
extern STATUS	pppstatGet (int unit, PPP_STAT *pStat);
extern void	pppSecretGet (void);

#else	/* __STDC__ */

extern void	pppShowInit ();
extern void	pppInfoShow ();
extern STATUS	pppInfoGet ();
extern void	pppstatShow ();
extern STATUS	pppstatGet ();
extern void	pppSecretGet ();

#endif	/* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif	/* __INCpppShowh */
