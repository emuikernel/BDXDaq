/* configNet.h - network configuration header */

/* Copyright 1984-2001 Wind River Systems, Inc. */
/* Copyright 1999-2001 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01e,12jun02,kab  SPR 74987: cplusplus protection
01d,28sep01,srr  Cleaned up support for CARRIER_TYPE and removed sysEnd.c.
01c,31jul01,srr  Added PrPMC-G Gigabit Ethernet and PrPMC Carrier 1 support.
01b,06jul01,srr  Added Gigabit Ethernet driver support.
01a,31aug00,dmw  Written (from version 01a of mv5100/configNet.h).
*/
 
#ifndef INCnetConfigh
#define INCnetConfigh

#ifdef __cplusplus
    extern "C" {
#endif

#include "vxWorks.h"
#include "end.h"

#define FEI_END_LOAD_FUNC sysFei82557EndLoad
#define DEC_END_LOAD_FUNC sysDec21x40EndLoad

#define END_BUFF_LOAN   1

#define END_LOAD_STRING  ""		/* created in sys<Driver>.c */

#ifdef INCLUDE_FEI_END
IMPORT END_OBJ* FEI_END_LOAD_FUNC (char*, void*);
#endif /* INCLUDE_FEI_END */

#ifdef INCLUDE_DEC_END
IMPORT END_OBJ* DEC_END_LOAD_FUNC (char*, void*);
#endif /* INCLUDE_DEC_END */

#if (CARRIER_TYPE == PRPMC_G)
#   define GEI82543_LOAD_FUNC     gei82543EndLoad  /* external interface */
#   define GEI82543_BUFF_LOAN     1                /* enable buffer loan */
#   define GEI_MAX_UNITS          4                /* Maximum device units */
#   define GEI82543_LOAD_STRING_0 geiEndLoadStr[0]
#   define GEI82543_LOAD_STRING_1 geiEndLoadStr[1]
#   define GEI82543_LOAD_STRING_2 geiEndLoadStr[2]
#   define GEI82543_LOAD_STRING_3 geiEndLoadStr[3]

char geiEndLoadStr [GEI_MAX_UNITS] [END_DESC_MAX] = {{0},{0},{0},{0}};

IMPORT END_OBJ* GEI82543_LOAD_FUNC (char*, void*);
#endif /* (CARRIER_TYPE == PRPMC_G) */


END_TBL_ENTRY endDevTbl [] =
{
#if (CARRIER_TYPE == PRPMC_BASE)
#   ifdef INCLUDE_FEI_END
        { 0, FEI_END_LOAD_FUNC, END_LOAD_STRING, END_BUFF_LOAN, NULL, FALSE},
#   elif defined (INCLUDE_DEC_END)
	{ 0, DEC_END_LOAD_FUNC, END_LOAD_STRING, END_BUFF_LOAN, NULL,FALSE},
#   endif /* INCLUDE_FEI_END */

#   ifdef INCLUDE_SECONDARY_ENET
#       ifdef INCLUDE_DEC_END
	    { 1, DEC_END_LOAD_FUNC, END_LOAD_STRING, END_BUFF_LOAN, NULL,FALSE},
#       endif /* INCLUDE_DEC_END */
#   endif /* INCLUDE_SECONDARY_ENET */

#elif (CARRIER_TYPE == PRPMC_G)
    { 0, GEI82543_LOAD_FUNC, GEI82543_LOAD_STRING_0, GEI82543_BUFF_LOAN,
      NULL, FALSE},
#   ifdef INCLUDE_SECONDARY_ENET
#       ifdef INCLUDE_FEI_END
            { 0, FEI_END_LOAD_FUNC, END_LOAD_STRING, END_BUFF_LOAN, NULL,FALSE},
#       endif /* INCLUDE_FEI_END */
#   endif /* INCLUDE_SECONDARY_ENET */

#elif (CARRIER_TYPE == PRPMC_CARRIER_1)
#   ifdef INCLUDE_FEI_END
        { 0, FEI_END_LOAD_FUNC, END_LOAD_STRING, END_BUFF_LOAN, NULL, FALSE},
#   ifdef INCLUDE_SECONDARY_ENET
	{ 1, FEI_END_LOAD_FUNC, END_LOAD_STRING, END_BUFF_LOAN, NULL,FALSE},
#       ifdef INCLUDE_TERTIARY_ENET
	    { 2, FEI_END_LOAD_FUNC, END_LOAD_STRING, END_BUFF_LOAN, NULL,FALSE},
#       endif /* INCLUDE_TERTIARY_ENET */
#   endif /* INCLUDE_SECONDARY_ENET */
#   endif /* INCLUDE_FEI_END */

#endif /* (CARRIER_TYPE == PRPMC_BASE) */

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
