/* wvLibP.h - WindView lib private header */

/* Copyright 1997 Wind River Systems, Inc. */

/*
modification history
--------------------
01e,20apr98,cth  changed WV_LOG_HEADER def, reverted uploadTaskPriority to 01b
01d,17apr98,cth  added type WV_LOG_HEADER, removed prototype of wvUpload
01c,15apr98,cth  removed wvUploadTaskPriority, added wvUploadTaskPriorityCont
		 and wvUploadPathPriorityOnce
01b,18dec97,cth  updated includes
01a,16nov97,cth  created, derived from wvLib.h
*/

#ifndef __INCwvlibph
#define __INCwvlibph

#include "wvLib.h"

#include "private/wvUploadPathP.h"
#include "private/wvBufferP.h"

#ifdef __cplusplus
extern "C" {
#endif


/* globals */

extern int      wvUploadTaskPriority;	   /* upload task priority */
extern int      wvUploadTaskStackSize;     /* upload task stack size (bytes) */
extern int      wvUploadTaskOptions;       /* upload task options */

/* types */

typedef struct wvUploadTaskDescriptor
    {
    int         uploadTaskId;           /* task id of this upload task */
    SEMAPHORE   uploadCompleteSem;      /* given iff buffers are emptied */
    BOOL        exitWhenEmpty;          /* if true task exits when buf empty */
    UPLOAD_ID	uploadPathId;		/* id of the upload path to host */
    BUFFER_ID	bufferId;		/* id of the buffer to upload from */
    int		status;			/* indicate errors in up task */
    } WV_UPLOADTASK_DESC;


typedef struct wvEvtLogHeader
    {
    PART_ID memPart;			/* where it's located */
    int     len;			/* length of header */
    char   *header; 			/* a packed array of events */
    } WV_LOG_HEADER;


/*
 * See wvLib manual page for a description of the taskname event buffer.
 */

typedef struct tnEvent
    {
    short          eventId;
    int            status;
    int            priority;
    int            taskLockCount;
    int            tid;
    int            nameSize;
    char          *name;
    } TN_EVENT;

typedef struct tnNode
    {
    TN_EVENT      *event;
    int            key;
    struct tnNode *next;
    } TN_NODE;

typedef struct tnHashTable
    {
    int            size;
    PART_ID        memPart;
    TN_NODE      **tbl;		/* *tbl[] */
    } TN_HASH_TBL;

typedef TN_HASH_TBL * TASKBUF_ID;

typedef struct tnIterKey
    {
    int 	   index;
    TN_NODE 	  *pCur;
    } TN_ITER_KEY;


/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

#else   /* __STDC__ */

#endif  /* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCwvlibph*/

