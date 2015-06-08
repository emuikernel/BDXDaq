/* wdbEvtptLib.h - header file for remote debug eventpoints */

/* Copyright 1998-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,25apr02,jhw  Added C++ support (SPR 76304).
01b,12feb99,dbt  use wdb/ prefix to include wdb.h header file.
01a,12feb98,dbt	 written.
*/

#ifndef __INCwdbEvtptLibh
#define __INCwdbEvtptLibh

#ifdef __cplusplus
extern "C" {
#endif

/* includes */

#include "wdb/dll.h"
#include "wdb/wdb.h"

/* data types */

typedef struct			/* hidden */
    {
    dll_t		evtptList;
    WDB_EVT_TYPE	evtptType;
    UINT32		(*evtptAdd) (WDB_EVTPT_ADD_DESC *pEvtpt, UINT32 *pId);
    UINT32		(*evtptDel) (TGT_ADDR_T *pId);
    } WDB_EVTPT_CLASS;

/* function prototypes */

#if defined(__STDC__)

extern void   	wdbEvtptClassConnect	(WDB_EVTPT_CLASS * pEvtList);
extern void	(*__wdbEvtptDeleteAll)	(void);

#else  /* defined(__STDC__) */

extern void   	wdbEvtptClassConnect	();
extern void	(*__wdbEvtptDeleteAll)	();

#endif /* defined(__STDC__) */

#define wdbEvtptDeleteAll()	{if (__wdbEvtptDeleteAll != NULL)	\
				     (*__wdbEvtptDeleteAll)();}

#ifdef __cplusplus
}
#endif

#endif  /* __INCwdbEvtptLibh */
