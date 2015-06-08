/* configNet.h - network configuration header */

/* Copyright 1984-1999 Wind River Systems, Inc. */
/* Copyright 1999-2000 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01d,12jun02,kab  SPR 74987: cplusplus protection
01c,03may01,mot  Add FEI driver support
01b,16aug00,dmw  Wind River coding standards review.
01a,13mar00,dmw  Written (from version 01b of mcpn765/configNet.h).
*/
 
#ifndef INCnetConfigh
#define INCnetConfigh

#ifdef __cplusplus
    extern "C" {
#endif

#include "vxWorks.h"
#include "end.h"

/* include fei driver */
#ifdef INCLUDE_FEI_END
#define END_LOAD_FUNC   sysFei82557EndLoad
#endif /* INCLUDE_FEI_END */

#ifdef INCLUDE_MCG_END
#define END_LOAD_FUNC	sysEndLoad
#endif /* INCLUDE_MCG_END */

#define END_BUFF_LOAN   1

#define END_LOAD_STRING  ""		/* created in sysEnd.c */

IMPORT END_OBJ* END_LOAD_FUNC (char*, void*);

END_TBL_ENTRY endDevTbl [] =
    {
        { 0, END_LOAD_FUNC, END_LOAD_STRING, END_BUFF_LOAN, NULL, FALSE},
#ifdef INCLUDE_SECONDARY_ENET
        { 1, END_LOAD_FUNC, END_LOAD_STRING, END_BUFF_LOAN, NULL, FALSE},
#endif /* INCLUDE_SECONDARY_ENET */
        { 0, END_TBL_END, NULL, 0, NULL, FALSE},
    };

/* define IP_MAX_UNITS to the actual number in the table. */

#ifndef IP_MAX_UNITS 
#   define IP_MAX_UNITS            (NELEMENTS(endDevTbl) - 1)
#endif  /* ifndef IP_MAX_UNITS */

#ifdef __cplusplus
    }
#endif

#endif /* INCnetConfigh */
