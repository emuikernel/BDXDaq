/* m2IfLib.h - VxWorks MIB-II interface to SNMP Agent */

/* Copyright 1984-2003 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01h,27jan03,vvv  added prototypes for m2IfUsrCountersInstall() and 
		 m2IfUsrCountersRemove (SPR #83254)
01g,13jan03,rae  Merged from velocecp branch
01b,21nov02,vvv  added declaration for m2PollStatsIfPoll() function
01a,11feb00,ann  written for RFC 2233 implementation
*/

#ifndef __INCm2IfLibh
#define __INCm2IfLibh

#ifdef __cplusplus
extern "C" {
#endif

#include "m2Lib.h"
#include "end.h"

extern STATUS m2PollStatsIfPoll (END_IFDRVCONF *);
extern STATUS m2IfUsrCountersInstall (M2_ID *, void *);
extern STATUS m2IfUsrCountersRemove (M2_ID *);

#ifdef __cplusplus
}
#endif

#endif /* __INCm2Libh */
