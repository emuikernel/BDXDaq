/* resolvLib.h - VxWorks DNS resolver facilities */

/* Copyright 1984-2001 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01e,12oct01,rae  merge from truestack (S_resolvLib_INVALID_ADDRESS)
01d,24sep01,gls  removed definition of __P() macro (SPR #28330)
01c,19may97,spm  added S_resolvLib_INVALID_PARAMETER error value (SPR #8603),
                 corrected format of modification history
01b,02apr97,jag  removed reference to header file resolv/param.h. 
01a,29jul96,rjc  written. 
*/

#ifndef __INCresolvLibh
#define __INCresolvLibh

#ifdef __cplusplus
extern "C" {
#endif


/* includes */

#include "vxWorks.h"
#include "vwModNum.h"
#include "sockLib.h"
#include "netinet/in.h"
#include "arpa/inet.h"
#include "netdb.h"
#include "resolv/nameser.h"
#include "netinet/in.h"
#include "resolv/resolv.h"


#define MAXIPADDRLEN   20             /* assuming decimal dot notation */
#define MAXALIASES     20
#define MAXADDRS       20

#define  QUERY_LOCAL_FIRST      1     /* Query static host table first */
#define  QUERY_DNS_FIRST        2     /* Query DNS first */
#define  QUERY_DNS_ONLY         3     /* Query DNS only */

#define S_resolvLib_NO_RECOVERY       (M_resolvLib | 1)
#define S_resolvLib_TRY_AGAIN         (M_resolvLib | 2)
#define S_resolvLib_HOST_NOT_FOUND    (M_resolvLib | 3)
#define S_resolvLib_NO_DATA           (M_resolvLib | 4)
#define S_resolvLib_BUFFER_2_SMALL    (M_resolvLib | 5)
#define S_resolvLib_INVALID_PARAMETER (M_resolvLib | 6)
#define S_resolvLib_INVALID_ADDRESS   (M_resolvLib | 7)

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif  /* CPU_FAMILY==I960 */

/* Resolver parameter configuration structures */

typedef struct 
    {
		    /* order in which to query hostLib database and DNS */
    char   queryOrder;   
				   /* maximum domain name based on rfc883 */
    char   domainName [MAXDNAME]; 
    				   /* IP address of name servers */
    char   nameServersAddr [MAXNS][MAXIPADDRLEN];	

    } RESOLV_PARAMS_S;

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 0                 /* turn off alignment requirement */
#endif  /* CPU_FAMILY==I960 */

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

STATUS             resolvInit (char *, char *, FUNCPTR);
struct hostent *   resolvGetHostByName (char *, char *, int);
struct hostent *   resolvGetHostByAddr (const char *, char *, int);
int                resolvDNExpand (const u_char *, const u_char *, 
				   const u_char *, u_char *, int);
int                resolvDNComp (const u_char *, u_char *, int, u_char **, 
				 u_char **);
int                resolvQuery (char *, int, int, u_char *, int);

int                resolvMkQuery (int, const char *, int, int, const char *,
                                  int, const char *, char *, int);
int                resolvSend (const char *, int, char *, int) ;
STATUS               resolvParamsSet (RESOLV_PARAMS_S *); 
void               resolvParamsGet (RESOLV_PARAMS_S *);

#else   /* __STDC__ */

STATUS             resolvInit ();
struct hostent *   resolvGetHostByName ();
struct hostent *   resolvGetHostByAddr (); 
int                resolvDNExpand ();
int                resolvDNComp ();
int                resolvQuery ();
int                resolvMkQuery ();
int                resolvSend ();
void               resolvParamsSet ();
void               resolvParamsGet ();

#endif  /* __STDC__ */


#ifdef __cplusplus
}
#endif


#endif /* __INCresolvLibh */
