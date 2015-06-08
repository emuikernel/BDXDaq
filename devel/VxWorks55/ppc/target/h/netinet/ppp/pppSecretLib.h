/* pppSecretLib.h - PPP authentication secrets library header */

/* Copyright 1995 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,09may95,dzb  written.
*/

#ifndef __INCpppSecretLibh
#define __INCpppSecretLibh

#ifdef __cplusplus
extern "C" {
#endif

/* includes */

#include "vwModNum.h"
#include "netinet/ppp/pppd.h"

/* typedefs */

typedef struct ppp_secret		/* PPP_SECRET */
    {
    struct ppp_secret *	secretNext;
    char		client [MAXNAMELEN];
    char		server [MAXNAMELEN];
    char		secret [MAXSECRETLEN];
    char		addrs [MAXNAMELEN];
    } PPP_SECRET;

/* defines */

#define	S_pppSecretLib_NOT_INITIALIZED		(M_pppSecretLib | 1)
#define	S_pppSecretLib_SECRET_DOES_NOT_EXIST	(M_pppSecretLib | 2)
#define	S_pppSecretLib_SECRET_EXISTS		(M_pppSecretLib | 3)

/* globals */

extern PPP_SECRET *	pppSecretHead;

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern STATUS	pppSecretLibInit (void);
extern STATUS	pppSecretAdd (char * client, char * server, char * secret,
		    char * addrs);
extern STATUS	pppSecretDelete (char * client, char * server, char * secret);
extern int	pppSecretFind (char * client, char * server, char * secret,
		    struct wordlist ** ppAddrs);

#else	/* __STDC__ */

extern STATUS	pppSecretLibInit ();
extern STATUS	pppSecretAdd ();
extern STATUS	pppSecretDelete ();
extern int	pppSecretFind ();

#endif	/* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCpppSecretLibh */
