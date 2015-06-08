/* pppd.h - PPP daemon header */

/* Copyright 1995 Wind River Systems, Inc. */

/*
 * Copyright (c) 1989 Carnegie Mellon University.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by Carnegie Mellon University.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

/*
modification history
--------------------
01l,29oct01,gls  removed definition of sleep()
01k,05aug96,vin  included if_types.h.
01j,22jun95,dzb  changed [un]timeout() to ppp_[un]timeout().
                 removed usehostname option.
01i,12jun95,dzb  added SNMP MIBII counters to ppp_task_vars.
                 header file consolidation.  changed to ppp_auth.h.
		 moved PPP_OPTIONS and options flags to options.h.
		 removed device_script() prototype.
		 changed [dis[connect strings to global hook routines.
		 changed [UN]TIMEOUT macros to PPP_[UN]TIMEOUT. 
01h,22may95,dzb  Added OPT_REQUIRE_PAP and OPT_REQUIRE_CHAP.
01g,16may95,dzb  Added uselogin and OPT_LOGIN for login option.
                 removed logged_in and attempts fields from PPP_TASK_VARS.
01f,05may95,dzb  added pap_passwd field to ppp_options structure.
01e,07mar95,dzb  moved pppInit() and pppDelete() prototypes to pppLib.h.
01d,09feb95,dab  removed lcp_echo_fails_reached, uselogin, and OPT_LOGIN.
01c,24jan95,dzb  renamed protoype params for pppInit() and ppp_task().
01b,16jan95,dzb  moved in ARGS macro from args.h.
		 moved in callout from callout.h.
		 moved in patch level macros from patchlevel.h.
01a,13jan95,dab  VxWorks port - first WRS version.
	   +dzb  added: path for ppp header files, WRS copyright.  WRS-ize.
*/

#ifndef	__INCpppdh
#define	__INCpppdh

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __ARGS
#ifdef __STDC__
#define __ARGS(x)       x
#else
#define __ARGS(x)       ()
#endif
#endif

/* includes */

#include "wdLib.h"
#include "semLib.h"
#include "remLib.h"
#include "net/if.h"
#include "net/if_types.h"
#include "time.h"
#include "netinet/ppp/ppp.h"
#include "netinet/ppp/ppp_auth.h"
#include "netinet/ppp/chap.h"
#include "netinet/ppp/fsm.h"
#include "netinet/ppp/if_ppp.h"
#include "netinet/ppp/ipcp.h"
#include "netinet/ppp/lcp.h"
#include "netinet/ppp/magic.h"
#include "netinet/ppp/md5.h"
#include "netinet/ppp/options.h"
#include "netinet/ppp/ppp_vxworks.h"
#include "netinet/ppp/random.h"
#include "netinet/ppp/upap.h"

/* defines */

/*
 * Patch level
 */
#define PATCHLEVEL      2
#define VERSION "2.1"
#define DATE "9 June 94"

/*
 * Limits.
 */
#define MAXWORDLEN      1024    /* max length of word in file (incl null) */
#define MAXARGS         1       /* max # args to a command */
#define MAXNAMELEN      256     /* max length of hostname or name for auth */
#define MAXSECRETLEN    256     /* max length of password or secret */
#define MAXPATHLEN    	1024    /* max length of password or secret */

/*
 * Values for phase.
 */
#define PHASE_DEAD              0
#define PHASE_ESTABLISH         1
#define PHASE_AUTHENTICATE      2
#define PHASE_NETWORK           3
#define PHASE_TERMINATE         4

#ifndef	SIOCSIFMTU
#define SIOCSIFMTU      _IOW('i', 140, int)
#endif	/* SIOCSIFMTU */
#ifndef	SIOCGIFMTU
#define SIOCGIFMTU      _IOR('i', 141, int)
#endif	/* SIOCGIFMTU */

/* Used for storing a sequence of words.  Usually malloced. */
struct wordlist {
	struct wordlist *next;
	char word[1];
};

/*
 * This is a copy of /usr/include/sys/callout.h with the c_func
 * member of struct callout changed from a pointer to a function of type int
 * to a pointer to a function of type void (generic pointer) as per ANSI C
 */

struct  callout {
        int     c_time;         /* incremental time */
        caddr_t c_arg;          /* argument to routine */
        void    (*c_func)();    /* routine (changed to void (*)() */
        struct  callout *c_next;
};

/* PPP task variables structure */
typedef struct ppp_task_vars
{
	PPP_OPTIONS *options;		/* Configuration options */

	int task_id;			/* Task id of pppd task */

	/* interface varibles */
	char ifname[IFNAMSIZ];		/* Interface name */
	int ifunit;			/* Interface unit number */

	/* descriptors */
	int fd;				/* Device descriptor */
	int s;				/* Socket descriptor */

	/* timer variables */
	struct callout *callout;	/* Callout list */
	timer_t timer_id;		/* Id of alarm timer */
	time_t schedtime;		/* Time last timeout was set */

	/* configured variables */
	int debug;			/* Debug flag */
	int inspeed;                    /* Input/Output speed requested */
	u_long netmask;                 /* Netmask to use on PPP interface */
	int auth_required;              /* Require peer to authenticate */
	int defaultroute;               /* Assign default rte through interf */
	int proxyarp;                   /* Set entry in arp table */
	int uselogin;                   /* Check PAP info against login table */
	int phase;                      /* Where the link is at */
	int baud_rate;                  /* Bits/sec currently used */
	char user[MAXNAMELEN];		/* User name */
	char passwd[MAXSECRETLEN];      /* Password for PAP */
	int disable_defaultip;		/* Use hostname IP address */
	int kdebugflag;			/* Enable driver debug */

	char hostname[MAXNAMELEN];	/* Our host name */
	char our_name[MAXNAMELEN];	/* Authentication host name */
	char remote_name[MAXNAMELEN];	/* Remote host name */

	char devname[MAXPATHLEN];	/* Device name */

	int ttystate;			/* Initial TTY state */

	int restore_term;        	/* 1 => we've munged the terminal */

					/* Buffer for outgoing packet */
	u_char outpacket_buf[MTU + DLLHEADERLEN];
					/* Buffer for incoming packet */
	u_char inpacket_buf[MTU + DLLHEADERLEN];

	int hungup;                     /* Terminal has been hung up */

	int peer_mru;			/* Peer MRU */

	/* Records which authentication operations haven't completed yet. */
	int auth_pending;		/* Authentication state */
	struct wordlist *addresses;	/* List of acceptable IP addresses */

	/* lcp echo variables */
	u_long lcp_echos_pending;	/* Number of outstanding echo msgs */
	u_long lcp_echo_number;    	/* ID number of next echo frame */
	u_long lcp_echo_timer_running;	/* TRUE if a timer running */
	u_long lcp_echo_interval;	/* Seconds between intervals */
	u_long lcp_echo_fails;		/* Number of echo failures */

	/* chap variables */
	chap_state chap;		/* CHAP structure */

        /* ipcp variables */
	ipcp_options ipcp_wantoptions;	/* Options we want to request */
	ipcp_options ipcp_gotoptions; 	/* Options that peer ack'd */
	ipcp_options ipcp_allowoptions;	/* Options we allow peer to request */
	ipcp_options ipcp_hisoptions;	/* Options that we ack'd */
	fsm ipcp_fsm;            	/* IPCP fsm structure */
	int cis_received;          	/* # Conf-Reqs received */

	/* lcp variables */
	lcp_options lcp_wantoptions;	/* Options that we want to request */
	lcp_options lcp_gotoptions;	/* Options that peer ack'd */
	lcp_options lcp_allowoptions;	/* Options we allow peer to request */
	lcp_options lcp_hisoptions;	/* Options that we ack'd */
	u_long xmit_accm[8];		/* extended transmit ACCM */
	fsm lcp_fsm;			/* LCP fsm structure */

	/* upap variables */
	upap_state upap;		/* UPAP state; one for each unit */

	/* SNMP MIBII counters */
	u_long	unknownProto;		/* MIBII: ifInUnknownProtos */
} PPP_TASK_VARS;

/* 
 * Externals
 */
extern int ppp_unit;
extern PPP_TASK_VARS *ppp_if[];
extern void syslog(int, char *, ...);

extern FUNCPTR pppConnectHook;		/* user connect hook routine */ 
extern FUNCPTR pppDisconnectHook;	/* user disconnect hook routine */ 

/*
 * Inline versions of get/put char/short/long.
 * Pointer is advanced; we assume that both arguments
 * are lvalues and will already be in registers.
 * cp MUST be u_char *.
 */
#define GETCHAR(c, cp) { \
	(c) = *(cp)++; \
}
#define PUTCHAR(c, cp) { \
	*(cp)++ = (u_char) (c); \
}


#define GETSHORT(s, cp) { \
	(s) = *(cp)++ << 8; \
	(s) |= *(cp)++; \
}
#define PUTSHORT(s, cp) { \
	*(cp)++ = (u_char) ((s) >> 8); \
	*(cp)++ = (u_char) (s); \
}

#define GETLONG(l, cp) { \
	(l) = *(cp)++ << 8; \
	(l) |= *(cp)++; (l) <<= 8; \
	(l) |= *(cp)++; (l) <<= 8; \
	(l) |= *(cp)++; \
}
#define PUTLONG(l, cp) { \
	*(cp)++ = (u_char) ((l) >> 24); \
	*(cp)++ = (u_char) ((l) >> 16); \
	*(cp)++ = (u_char) ((l) >> 8); \
	*(cp)++ = (u_char) ((l)); \
}

#define INCPTR(n, cp)	((cp) += (n))
#define DECPTR(n, cp)	((cp) -= (n))

/*
 * System dependent definitions for user-level 4.3BSD UNIX implementation.
 */

#define DEMUXPROTREJ(u, p) 	demuxprotrej(u, p)

#define PPP_TIMEOUT(r, f, t) 	ppp_timeout((r), (f), (t))
#define PPP_UNTIMEOUT(r, f)	ppp_untimeout((r), (f))

#define BCOPY(s, d, l)		bcopy((char *) s, (char *) d, (int) l)
#define BZERO(s, n)             bzero(s, n)
#define EXIT(u)			die(u, 0)

#define PRINTMSG(m, l)	{ m[l] = '\0'; syslog(LOG_INFO, "Remote message: %s", m); }

/*
 * MAKEHEADER - Add Header fields to a packet.
 */
#define MAKEHEADER(p, t) { \
    PUTCHAR(ALLSTATIONS, p); \
    PUTCHAR(UI, p); \
    PUTSHORT(t, p); }

#ifdef DEBUGALL
#define DEBUGMAIN       1
#define DEBUGFSM        1
#define DEBUGLCP        1
#define DEBUGIPCP       1
#define DEBUGUPAP       1
#define DEBUGCHAP       1
#endif

#ifndef LOG_PPP			/* we use LOG_LOCAL2 for syslog by default */
#if defined(DEBUGMAIN) || defined(DEBUGFSM) || defined(DEBUG) \
  || defined(DEBUGLCP) || defined(DEBUGIPCP) || defined(DEBUGUPAP) \
  || defined(DEBUGCHAP) 
#define LOG_PPP LOG_LOCAL2
#else
#define LOG_PPP LOG_DAEMON
#endif
#endif /* LOG_PPP */

#ifdef DEBUGMAIN
#define MAINDEBUG(x)	if (ppp_if[ppp_unit]->debug) syslog x;
#else
#define MAINDEBUG(x)
#endif

#ifdef DEBUGFSM
#define FSMDEBUG(x)	if (ppp_if[ppp_unit]->debug) syslog x;
#else
#define FSMDEBUG(x)
#endif

#ifdef DEBUGLCP
#define LCPDEBUG(x)	if (ppp_if[ppp_unit]->debug) syslog x;
#else
#define LCPDEBUG(x)
#endif

#ifdef DEBUGIPCP
#define IPCPDEBUG(x)	if (ppp_if[ppp_unit]->debug) syslog x;
#else
#define IPCPDEBUG(x)
#endif

#ifdef DEBUGUPAP
#define UPAPDEBUG(x)	if (ppp_if[ppp_unit]->debug) syslog x;
#else
#define UPAPDEBUG(x)
#endif

#ifdef DEBUGCHAP
#define CHAPDEBUG(x)	if (ppp_if[ppp_unit]->debug) syslog x;
#else
#define CHAPDEBUG(x)
#endif

#ifndef SIGTYPE
#if defined(sun) || defined(SYSV) || defined(POSIX_SOURCE)
#define SIGTYPE void
#else
#define SIGTYPE int
#endif /* defined(sun) || defined(SYSV) || defined(POSIX_SOURCE) */
#endif /* SIGTYPE */

#ifndef MIN
#define MIN(a, b)       ((a) < (b)? (a): (b))
#endif
#ifndef MAX
#define MAX(a, b)       ((a) > (b)? (a): (b))
#endif

#undef  LOG_WARNING
#undef  LOG_ERR
#undef  LOG_INFO
#undef  LOG_NOTICE
#undef  LOG_DEBUG
#undef  LOG_UPTO

#define LOG_WARNING	0
#define LOG_ERR		1
#define LOG_INFO	2
#define LOG_NOTICE	3
#define LOG_DEBUG	4
#define LOG_UPTO	5

#ifndef DEBUGALL
#define _assert(ex)    {if (!(ex)){(void)syslog(LOG_ERR,"Assertion failed: file \"%s\", line %d\n", __FILE__, __LINE__);die(ppp_unit, 1);}}
#define assert(ex)     _assert(ex)
#else
#define _assert(ex)
#define assert(ex)
#endif

void format_packet __ARGS((u_char *, int,
		   void (*) (void *, char *, ...), void *));

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern void	ppp_task (int unit, char *devname, char *local_addr,
		    char *remote_addr, int baud, PPP_OPTIONS *pOptions,
		    char *fOptions);
extern void	die (int unit, int status);
extern void	ppp_timeout (void (*func) (), caddr_t arg, int seconds);
extern void	ppp_untimeout (void (*func) (), caddr_t arg);
extern void	adjtimeout (void);
extern void	demuxprotrej (int unit, u_short protocol);
extern void	log_packet (u_char *p, int len, char *prefix);
extern void	print_string (char *p, int len, void (*printer)
                    (void *, char *, ...), void *arg);
extern void	pr_log (void *arg, char *fmt, ...);
extern void	format_packet (u_char *p, int len,
		   void (*printer) (void *, char *, ...), void *arg);
extern void	novm (char *msg);

#else	/* __STDC__ */
 
extern int	ppp_task ();
extern void	die ();
extern void	ppp_timeout ();
extern void	ppp_untimeout ();
extern void	adjtimeout ();
extern void	demuxprotrej ();
extern void	log_packet ();
extern void	print_string ();
extern void	pr_log ();
extern void	format_packet ();
extern void	novm ();

#endif	/* __STDC__ */
  
#ifdef __cplusplus
}
#endif

#endif	/* __INCpppdh */
