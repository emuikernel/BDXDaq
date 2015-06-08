/* wtxAsync.h - WTX library used by the asynchronous notification */

/*
modification history
--------------------
01e,27may99,fle  Added async notification on wtx handle basis SPR 67367
01d,09jun98,jmp  renamed wtxAsyncEventGet() in wtxAsyncEventDescGet().
01c,20may98,jmp  added wtxAsyncEventGet().
01b,23apr98,pcn  Make this file a library used by the asynchronous
		 notification.
01a,05feb98,pcn  written.
*/

#ifndef __INCwtxAsynch
#define __INCwtxAsynch       1

#ifdef __cplusplus
extern "C" {
#endif

/* includes */

#include "host.h"
#include "lstLib.h"
#include "vmutex.h"
#include "tcl.h"
#include "sockUtil.h"
#include "wtx.h"

/* defines */

#define		TOOL_EVT_SIZE		300

/* typedefs */

typedef struct wtx_evt_node
    {
    NODE	node;
    char	event [TOOL_EVT_SIZE];	/* event string */
    int		addlDataLen;		/* additional data length */
    char *	addlData;		/* additional data, eg: VIO data */
    } WTX_EVT_NODE;

typedef struct wtx_async_context
    {
    void *	objId;			/* object adress */
    FUNCPTR	wrapperFunc;		/* user routine */
    LIST	wtxEventList;		/* Global event list */
    SEM_ID	wtxEventListMutex;	/* Guarding Mutex for event list */
    SEM_ID	fullListSem;		/* Guarding semaphore for event list */
    SEM_ID	syncSema;		/* Synchronization semaphore */
    SEM_ID	prodMutex;		/* producer Mutex */
    SEM_ID	consMutex;		/* consumer Mutex */
#ifdef WIN32
    SOCKET	serverSockFd;		/* Socket file descriptor */
    SOCKET	serverNewSockFd;	/* Socket file descriptor */
#else
    int		serverSockFd;		/* Socket file descriptor */
    int		serverNewSockFd;	/* Socket file descriptor */
#endif
    BOOL32	asyncNotifyStarted;	/* Asynchronous notification flag */
    }	WTX_ASYNC_CONTEXT;		/* async. notif context */

typedef WTX_ASYNC_CONTEXT *	WTX_ASYNC_HANDLE;/* async. notif handle */

/* globals */

/* function declarations */

extern STATUS	wtxAsyncInitialize
    (
    SOCK_DESC *		pSockDesc,	/* port number and socket id */
    FUNCPTR		wrapperFunc,	/* function called */
    WTX_ASYNC_HANDLE *	pAsyncHandle	/* where to return the handle */
    );

extern void	wtxAsyncStop
    (
    WTX_ASYNC_HANDLE *	asyncHandle
    );

extern WTX_EVENT_DESC * wtxAsyncEventDescGet
    (
    WTX_ASYNC_HANDLE	asyncHandle
    );

#ifdef __cplusplus
}
#endif

#endif /* __INCwtxAsynch */
