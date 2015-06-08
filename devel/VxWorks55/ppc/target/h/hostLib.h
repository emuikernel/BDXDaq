/* hostLib.h - header for hostLib.c */

/* Copyright 1984-1997 Wind River Systems, Inc. */

/*
modification history
--------------------
01k,20aug97,jag  added function declations for hostTblSearchByAddr(),
                 hostTblSearchByName() (SPR #9175)
01j,19may97,spm  added S_hostLib_INVALID_PARAMETER error value
01i,22sep92,rrr  added support for c++
01h,27jul92,elh  Moved hostShow to netShow.
01g,04jul92,jcf  cleaned up.
01f,26may92,rrr  the tree shuffle
01e,04oct91,rrr  passed through the ansification filter
		  -fixed #else and #endif
		  -changed VOID to void
		  -changed copyright notice
01d,05oct90,shl  added ANSI function prototypes.
                 made #endif ANSI style.
                 added copyright notice.
01c,13apr89,shl  added MAXHOSTNAMELEN.
01b,04jun88,gae  changed S_remLib_* to S_hostLib_*.
01a,28may88,dnw  extracted from remLib.h
*/

#ifndef __INChostLibh
#define __INChostLibh

#ifdef __cplusplus
extern "C" {
#endif

#include "lstLib.h"
#include "netinet/in.h"

#define MAXHOSTNAMELEN 	64

/* status messages */

#define S_hostLib_UNKNOWN_HOST			(M_hostLib | 1)
#define S_hostLib_HOST_ALREADY_ENTERED		(M_hostLib | 2)
#define S_hostLib_INVALID_PARAMETER             (M_hostLib | 3)

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif  /* CPU_FAMILY==I960 */

typedef struct hostname_struct
    {
    struct hostname_struct *link;
    char *name;
    } HOSTNAME;

typedef struct
    {
    NODE node;
    HOSTNAME hostName;
    struct in_addr netAddr;
    } HOSTENTRY;

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 0                 /* turn off alignment requirement */
#endif  /* CPU_FAMILY==I960 */

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern STATUS 	hostAdd (char *hostName, char *hostAddr);
extern STATUS 	hostDelete (char *name, char *addr);
extern STATUS 	hostGetByAddr (int addr, char *name);
extern int 	gethostname (char *name, int nameLen);
extern int 	hostGetByName (char *name);
extern int 	sethostname (char *name, int nameLen);
extern void 	hostTblInit (void);
extern int      hostTblSearchByName (char *name);
extern STATUS   hostTblSearchByAddr (int addr, char *name);



#else	/* __STDC__ */

extern STATUS 	hostAdd ();
extern STATUS 	hostDelete ();
extern STATUS 	hostGetByAddr ();
extern int 	gethostname ();
extern int 	hostGetByName ();
extern int 	sethostname ();
extern void 	hostTblInit ();
extern int      hostTblSearchByName ();
extern STATUS   hostTblSearchByAddr ();

#endif	/* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INChostLibh */
