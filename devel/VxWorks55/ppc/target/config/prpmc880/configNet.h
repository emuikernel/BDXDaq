/* configNet.h - network configuration header */

/* Copyright 1984-1999 Wind River Systems, Inc. */
/* Copyright 1999-2003 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01a,04apr03,simon  Ported. (from ver 01b, mcpm905/configNet.h)
*/
 
#ifndef __INCnetConfigh
#define __INCnetConfigh

#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */

#include "vxWorks.h"
#include "end.h"

#define END_LOAD_FUNC NULL 
#define END_BUFF_LOAN   1
#define END_LOAD_STRING  ""		

#ifdef INCLUDE_GEISC_END
#   define GEISC_LOAD_FUNC     sysMv64360EndLoad
#   define GEISC0_STRING       ""
#   define GEISC1_STRING       ""
IMPORT END_OBJ* GEISC_LOAD_FUNC (char *, void *);
#endif /* INCLUDE_GEISC_END */

END_TBL_ENTRY endDevTbl [] =
    {
#ifdef INCLUDE_GEISC_END
	{ 0, GEISC_LOAD_FUNC, GEISC0_STRING, 0, NULL, FALSE},
	{ 1, GEISC_LOAD_FUNC, GEISC1_STRING, 0, NULL, FALSE},
#endif /* INCLUDE_GEISC_END */
    { 0, END_TBL_END, NULL, 0, NULL, FALSE}
    };

/* define IP_MAX_UNITS to the actual number in the table. */

#ifndef IP_MAX_UNITS 
#   define IP_MAX_UNITS            (NELEMENTS(endDevTbl) - 1)
#endif  /* ifndef IP_MAX_UNITS */

#ifdef __cplusplus
    }
#endif /* __cplusplus */

#endif /* __INCnetConfigh */