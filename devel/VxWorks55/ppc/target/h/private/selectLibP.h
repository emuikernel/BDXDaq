/* selectLibP.h - private select library header */

/* Copyright 2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,18sep01,aeg  extracted from version 02c of selectLib.h; added selContext.
*/

#ifndef __INCselectLibP
#define __INCselectLibP

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ASMLANGUAGE
#include "lstLib.h"
#include "private/semLibP.h"

typedef struct selWkNode
    {
    NODE		linkedListHooks;/* hooks for wakeup list */
    BOOL		dontFree;	/* first in free list isn't malloc'ed */
    int			taskId;		/* taskId of select'ed task */
    int			fd;		/* fd to set in fd_set on activity */
    SELECT_TYPE		type;		/* activity task is interested in */
    } SEL_WAKEUP_NODE;

typedef struct
    {
    SEMAPHORE		listMutex;	/* mutex semaphore for list */
    SEL_WAKEUP_NODE	firstNode;	/* usually one deep, stash first one */
    LIST		wakeupList;	/* list of SEL_WAKEUP_NODE's */
    } SEL_WAKEUP_LIST;

typedef struct selContext
    {
    SEMAPHORE	wakeupSem;		/* wakeup semaphore */
    BOOL     	pendedOnSelect;		/* task pended on select? */

    fd_set     *pReadFds;		/* select'ed task's read fd_set */
    fd_set     *pWriteFds;		/* select'ed task's write fd_set */

    /* the following are needed for safe task deletion */

    fd_set     *pOrigReadFds;		/* task's original read fd_set */
    fd_set     *pOrigWriteFds;		/* task's original write fd_set */
    int		width;			/* width parm passed to select() */

    /* 
     * The remaining memory in the SEL_CONTEXT dynamically allocated by
     * the select task create hook is used to store the various fd_set's
     * referenced in the above fd_set pointers.  Note that these structures
     * are not really fd_set's since their size is based on maxFiles (iosLib) 
     * instead of FD_SETSIZE.
     *
     *
     * fd_set  origReadFds;	/@ memory for task's original read fd_set @/
     * fd_set  origWriteFds;	/@ memory for task's original write fd_set @/
     */

    } SEL_CONTEXT;


/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern void	selectInit		(int numFiles);
extern void	selTaskDeleteHookAdd 	(void);

#else	/* __STDC__ */

extern void	selectInit		();
extern void	selTaskDeleteHookAdd 	();

#endif	/* __STDC__ */

#endif /* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCselectLibP */
