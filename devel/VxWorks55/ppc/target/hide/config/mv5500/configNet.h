/* configNet.h - network configuration header */

/* Copyright 1984-1999 Wind River Systems, Inc. */
/* Copyright 1999-2002 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01a,28oct02,scb Adapted from hxeb100 base (version 01d).
*/
 
#ifndef __INCnetConfigh
#define __INCnetConfigh

#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */

#include "vxWorks.h"
#include "end.h"

#define END_BUFF_LOAN   1

#ifdef INCLUDE_GEI_END
#   define GEI_LOAD_FUNC gei82543EndLoad
#   define GEI_STRING "\0                                                 "

IMPORT END_OBJ* GEI_LOAD_FUNC (char*, void*);
#endif /* INCLUDE_GEI_END */

#ifdef INCLUDE_WANCOM_END
#   define  WANCOM_LOAD_FUNC               sysWancomEndLoad
#   define  WANCOM0_STRING ""

IMPORT END_OBJ* WANCOM_LOAD_FUNC (char*, void*);
#endif /* INCLUDE_WANCOM_END */

END_TBL_ENTRY endDevTbl [] =
{

#ifdef INCLUDE_GEI_END
    { 0, GEI_LOAD_FUNC, GEI_STRING, END_BUFF_LOAN, NULL, FALSE},
#endif /* INCLUDE_GEI_END */

#ifdef INCLUDE_WANCOM_END
    { 0, WANCOM_LOAD_FUNC, WANCOM0_STRING, 0, NULL, FALSE},
#endif /* INCLUDE_WANCOM_END */
    { 0, END_TBL_END, NULL, 0, NULL, FALSE},
};

/* define IP_MAX_UNITS to the actual number in the table. */

#ifndef IP_MAX_UNITS 
#   define IP_MAX_UNITS            (NELEMENTS(endDevTbl) - 1)
#endif  /* ifndef IP_MAX_UNITS */

#ifdef __cplusplus
    }
#endif /* __cplusplus */

#endif /* __INCnetConfigh */
