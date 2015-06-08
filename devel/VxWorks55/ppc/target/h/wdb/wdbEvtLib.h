/* wdbEvtLib.h - header file for remote debug events */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01f,25apr02,jhw  Added C++ support (SPR 76304).
01e,12feb99,dbt  use wdb/ prefix to include wdb.h header file.
01d,04feb98,dbt  moved eventpoint handling to wdbEvtptLib.h
01c,07jun95,ms	added prototype for wdbEvtptDeleteAll
01b,05apr95,ms  new data types.
01a,20sep94,ms  written.
*/

#ifndef __INCwdbEvtLibh
#define __INCwdbEvtLibh

#ifdef __cplusplus
extern "C" {
#endif

/* includes */

#include "wdb/dll.h"
#include "wdb/wdb.h"

/* data types */

typedef struct			/* hidden */
    {
    dll_t	evt_list;
    WDB_EVT_TYPE evt_type;
    UINT32    (*evt_eventAdd) (WDB_EVTPT_ADD_DESC *pEvtpt, UINT32 *pId);
    UINT32    (*evt_eventDel) (TGT_ADDR_T *pId);
    } WDB_EVT_CLASS;

typedef struct
    {
    dll_t	node;
    BOOL	onQueue;
    void        (*getEvent)     (void *arg, WDB_EVT_DATA *pEvtData);
    void        (*deq)          (void *arg);
    void *      arg;
    } WDB_EVT_NODE;

/* function prototypes */

#if defined(__STDC__)

extern void   	wdbEventNodeInit	(WDB_EVT_NODE * pEvtNode,
			void (*getEvent) (void *arg, WDB_EVT_DATA *pEvtData),
			void (*deq) (void *arg),
			void *arg);
extern void   	wdbEventPost		(WDB_EVT_NODE * pEvtNode);
extern STATUS	wdbEventDeq		(WDB_EVT_NODE * pEvtNode);
extern void   	wdbEventClassConnect	(WDB_EVT_CLASS * pEvtList);
extern BOOL 	(*__wdbEventListIsEmpty) (void);
extern void	(*__wdbEvtptDeleteAll)	(void);

#else  /* defined(__STDC__) */

extern void   	wdbEventNodeInit	();
extern void   	wdbEventPost		();
extern STATUS	wdbEventDeq		();
extern void   	wdbEventClassConnect	();
extern BOOL 	(*__wdbEventListIsEmpty) ();
extern void	(*__wdbEvtptDeleteAll)	();

#endif /* defined(__STDC__) */

#define	wdbEventListIsEmpty()	(__wdbEventListIsEmpty == NULL ? TRUE : \
				(*__wdbEventListIsEmpty)())

#define wdbEvtptDeleteAll()	{if (__wdbEvtptDeleteAll != NULL)	\
				     (*__wdbEvtptDeleteAll)();}

#ifdef __cplusplus
}
#endif

#endif  /* __INCwdbEvtLibh */

