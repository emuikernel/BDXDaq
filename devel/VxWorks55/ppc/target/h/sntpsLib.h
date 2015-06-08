/* sntpsLib.h - Simple Network Time Protocol (SNTP) server include file */

/* Copyright 1984-2001 Wind River Systems, Inc. */

/*
Modification history 
--------------------
01c,30aug01,vvv  added extern "C" definition (SPR #21825)
01b,15jul97,spm  code cleanup, documentation, and integration; entered in
                 source code control
01a,20apr97,kyc  written

*/

#ifndef __INCsntpsh
#define __INCsntpsh

/* includes */

#include "sntp.h"

/* defines */

#define S_sntpsLib_INVALID_PARAMETER         (M_sntpsLib | 1)
#define S_sntpsLib_INVALID_ADDRESS           (M_sntpsLib | 2)

/* Valid settings for SNTPS_MODE in configAll.h */

#define  SNTP_ACTIVE 		1
#define  SNTP_PASSIVE 		2

/* Values for "request" parameter to clock hook routine. */

#define SNTPS_ID 		1
#define SNTPS_RESOLUTION 	2
#define SNTPS_TIME 		3

/* Values for "setting" parameter to sntpsConfigSet() routine. */

#define SNTPS_ADDRESS 		1
#define SNTPS_DELAY 		2

#if CPU_FAMILY==I960
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif  /* CPU_FAMILY==I960 */

typedef struct sntpsTimeData
    {
    ULONG seconds;
    ULONG fraction;
    } SNTP_TIMESTAMP;

#if CPU_FAMILY==I960
#pragma align 0                 /* turn off alignment requirement */
#endif  /* CPU_FAMILY==I960 */

#ifdef __cplusplus
extern "C" {
#endif

IMPORT STATUS sntpsInit (char *, u_char, char *, short, u_short, FUNCPTR);
IMPORT STATUS sntpsClockSet (FUNCPTR);
IMPORT ULONG  sntpsNsecToFraction (ULONG);
IMPORT STATUS sntpsConfigSet (int, void *);

#ifdef __cplusplus
}
#endif

#endif /* __INCsntpsh */

