/* configNet.h - network configuration header */

/* Copyright 1984-1997 Wind River Systems, Inc. */
/* Copyright 1999 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01e,12jun02,kab  SPR 74987: cplusplus protection
01d,16jul99,rhv  Picking up WRS component/stub approach.
01c,01jul99,rhv  Incorporating WRS code review changes.
01b,29jan99,mdp  Changed 21x40 references to 21x4x.
01a,15dec98,mdp  Written (from version 01g of mv2304/configNet.h).
*/
 
#ifndef INCnetConfigh
#define INCnetConfigh

#ifdef __cplusplus
    extern "C" {
#endif

#include "vxWorks.h"
#include "end.h"

#define DEC_LOAD_FUNC	sysDec21x40EndLoad
#define DEC_BUFF_LOAN   1

#  define   DEC_LOAD_STRING  ""		/* created in sys<device>End.c */

/* define IP_MAX_UNITS to the actual number in the table. */

#ifndef IP_MAX_UNITS 
#define IP_MAX_UNITS            (NELEMENTS(endDevTbl) - 1)
#endif  /* ifndef IP_MAX_UNITS */

IMPORT END_OBJ* DEC_LOAD_FUNC (char*, void*);

END_TBL_ENTRY endDevTbl [] =
{
    { 0, DEC_LOAD_FUNC, DEC_LOAD_STRING, DEC_BUFF_LOAN, NULL, FALSE},
    { 0, END_TBL_END, NULL, 0, NULL, FALSE},
};

#ifdef __cplusplus
    }
#endif

#endif /* INCnetConfigh */
