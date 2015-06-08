/* asyncloadlib.h - host-based asynchronous object-module loader header file */

/* Copyright 1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,19jun98,c_c Created
*/

#ifndef __INCasyncloadlibh
#define __INCasyncloadlibh

#include "vmutex.h"
#include "loadlib.h"

#ifdef __cplusplus
extern "C" {
#endif

/* defines */

#define LOAD_IN_PROGRESS	0	/* load in progress*/
#define LOAD_DONE		1	/* load finished (good or bad) */

#define LOAD_PENDING		0	/* load in work queue */
#define LOAD_INITIALIZED	1	/* load initialized */
#define LOAD_RELOCATING		2	/* relocation in progress */
#define LOAD_DOWNLOADING	3	/* downloading to the target */
#define LOAD_COMPLETE		4	/* load complete */
#define LOAD_ABORTED		5	/* load canceled at user request */

/* typedefs */

/* load progress informations */

typedef struct loadprogressinfo
    {
    int		loadState;	/* load phase */
    int		maxValue;	/* value to be reached (i.e. bytes to load) */
    int		actualValue;	/* currently portion done (i.e. byte sent)  */
    }	LOAD_PROGRESS_INFO;

/* asynchonous load context definition */

typedef struct loadcontext
    {
    SEM_ID	completionEvent;/* semaphore to wait (synchronous operations) */
    LOAD_PROGRESS_INFO loadProgress;/* current load phase and progress infos  */
    char *	fileInMem;	/* object module file resident in memory      */
    int		loadFlags;	/* Control of loader's behavior               */
    char *	pText;		/* text segment                               */
    char *	pData;		/* data segment                               */
    char *	pBss;		/* bss segment                                */
    SEG_INFO 	Seg;		/* segment informations                       */
    MODULE_ID	module;		/* created module                             */
    STATUS	loadStatus;	/* operation status upon completion or abort  */
    int		loadError;	/* operation error code                       */
    }	LOAD_CONTEXT;

/* Progress Information for asynchronous loads */

typedef struct loadreportinfo
    {
    int		loadState;	/*
    				 * two value :
    				 *  LOAD_IN_PROGRESS : 
				 *    - informations available in
				 *	PROGRESSINFO structure.
    				 *  LOAD_DONE : 
				 *    - informations available in
				 *       MODULEINFO structure.
				 */
    union	stateinfo
	{
	struct moduleinfo
	    {
	    STATUS	loadStatus;	/* operation status               */
	    int		loadError;	/* operation error code           */
	    MODULE_ID	module;		/* loaded module Id               */
	    char *	pText;		/* where the text has been loaded */
	    char *	pData;		/* where the data has been loaded */
	    char *	pBss;		/* where the Bss  has been loaded */
	    }	MODULEINFO;

	struct progressinfo
	    {
	    int		state;		/* Current State                  */
	    int		maxValue;	/* Maximum value for this state   */
	    int		currentValue;	/* current value for this state   */
	    }	PROGRESSINFO;
	}	STATEINFO;
    }	LOADREPORTINFO;

/* externals */

extern STATUS asyncLoadLibInit 	/* Initialize the library */
    (
    FUNCPTR	loaderRoutine	/* routine to call to perform the load */
    );

extern BOOL asyncLoadContextVerify 	/* verify a context */
    (
    LOAD_CONTEXT *	pLoadContext
    );

extern void asyncLoadContextDelete	/* delete a context */
    (
    LOAD_CONTEXT *	pLoadContext
    );

extern LOAD_CONTEXT *	asyncLoadModuleAt	/* submit a load */
    (
    int		fd,	    /* file descriptor                               */
    char *	filename,   /* Name of the module file                       */
    char *	fileInMem,  /* Base of file buffer                           */
    int		loadFlag,   /* Control of loader's behavior                  */
    void **	ppText,	    /* Load text segment at addr. pointed to by this */
			    /* Ptr, return load addr. via this ptr           */
    void **	ppData,     /* Load data segment at addr. pointed to by this */
			    /* Pointer, return load addr. via this ptr       */
    void **	ppBss       /* Load BSS segment at addr. pointed to by this  */
			    /* Pointer, return load addr. via this ptr       */
    );

extern STATUS asyncLoadWait	/* wait for a load completion */
    (
    LOAD_CONTEXT *	pLoadContext	/* load to wait for */
    );
 
extern STATUS asyncLoadCancel		/* cancel a load operation */
    (
    LOAD_CONTEXT *	pLoadContext 	/* load to cancel */
    );

extern STATUS asyncLoadReport
    (
    LOAD_CONTEXT *	pLoadContext,	/* load context to examine */
    LOADREPORTINFO *	pLoadReportInfo	/* where to report informations */
    );

#ifdef __cplusplus
}
#endif

#endif /* __INCasyncloadlibh */
