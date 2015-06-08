/* wvLib.h - event log header */

/* Copyright 1994-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01p,13may98,cth  added protype for wvUploadTaskConfig
01o,20apr98,cth  added protypes for wvLogHeaderCreate/Upload, modified 
		 wvEvtLogInit definition, changed wvUploadTaskPriority to 01l
01n,17apr98,cth  changed prototype of wvEvtLogInit
01m,15apr98,cth  added prototypes for task name storage, and wvEvtBufferGet,
		 renamed wvContinuousUploadStop to wvUploadStop
01l,17mar98,pr   added memory instrumentation support
01k,27jan98,cth  added wvEvtLogInit function declaration, updated copyright
01j,18dec97,cth  removed temporary references used for WV2.0 integration, 
		 updated include files, removed wvOn/Off, fixed prototypes
01i,16nov97,cth  more work for WindView 2.0
01h,30jul97,nps  major rework for WindView 2.0.
01g,08mar94,smb  changed prototype for OSE functionality
		 removed PASSIVE_MODE
01f,04mar94,smb  changed prototype for wvHostInfoInit (SPR #3089)
01e,22feb94,smb  changed typedef EVENT_TYPE to event_t (SPR #3064)
01d,15feb94,smb  renamed collection modes
01c,19jan94,smb  removed wvEvtLog, added wvEvtLogEnable and wvEvtLogEnable
		 removed ALL_LEVELS and changed OBJECT_STATE to OBJECT_STATUS
		 added wvOn(), wvOff(), and wvEvtLogStop().
01b,30dec93,c_s  changed prototype of wvServerInit ().  SPR #2790.
01a,10dec93,smb  written.
*/

#ifndef __INCwvlibh
#define __INCwvlibh

#include "vxWorks.h"

#include "private/wvBufferP.h"
#include "private/wvUploadPathP.h"
#include "private/wvLibP.h"
#include "private/eventP.h"


#ifdef __cplusplus
extern "C" {
#endif

/* defines */

#define OBJ_TASK        1	/* task objects */
#define OBJ_SEM         2	/* semaphore objects */
#define OBJ_MSG         3	/* message queue objects */
#define OBJ_WD          4	/* watch-dog timer objects */
#define OBJ_MEM         5	/* memory objects */

#define INSTRUMENT_ON   1	/* create objects with instrumentation on */
#define INSTRUMENT_OFF  2	/* create objects with instrumentation off */


/* types */

typedef struct wvUploadTaskDescriptor * WV_UPLOADTASK_ID;
typedef struct wvEvtLogHeader *WV_LOG_HEADER_ID;


/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern void   		wvLibInit (void);
extern void   		wvLibInit2 (void);
extern void		wvEvtLogInit (BUFFER_ID bufId);

extern void 		wvEvtLogStart (void);
extern void   		wvEvtLogStop (void);

extern void   		wvEvtClassSet (UINT32 classDescription);
extern UINT32  		wvEvtClassGet (void);
extern void   		wvEvtClassClear (UINT32 classDescription);
extern void   		wvEvtClassClearAll (void);

extern BUFFER_ID	wvEvtBufferGet (void);
extern void		wvUploadTaskConfig (int stackSize, int priority);
extern STATUS 		wvUploadStop   (WV_UPLOADTASK_ID upTaskId);
extern WV_UPLOADTASK_ID wvUploadStart  (BUFFER_ID bufId, UPLOAD_ID pathId, 
			                BOOL uploadContinuously);

extern WV_LOG_HEADER_ID wvLogHeaderCreate (PART_ID memPart);
extern STATUS		wvLogHeaderUpload (WV_LOG_HEADER *pH, UPLOAD_ID pathId);

extern STATUS 		wvObjInstModeSet (int mode);
extern STATUS 		wvObjInst (int objType, void *objId, BOOL evtLoggingOn);
extern STATUS 		wvSigInst (int mode);
extern STATUS 		wvEvent (event_t usrEventId, char *buffer, 
				 size_t bufSize);

extern TASKBUF_ID	wvTaskNamesPreserve (PART_ID memPart, int size);
extern STATUS		wvTaskNamesUpload  (TASKBUF_ID bufId, UPLOAD_ID pathId);
extern STATUS		wvTaskNamesBufAdd  (short eventId, int status, 
					    int priority, int taskLockCount,
					    int tid, char *name);


#else   /* __STDC__ */

extern void   		wvLibInit ();
extern void   		wvLibInit2 ();
extern void		wvEvtLogInit ();

extern void 		wvEvtLogStart ();
extern void   		wvEvtLogStop ();

extern void   		wvEvtClassSet ();
extern UINT32  		wvEvtClassGet ();
extern void   		wvEvtClassClear ();
extern void   		wvEvtClassClearAll ();

extern BUFFER_ID	wvEvtBufferGet ();
extern void		wvUploadTaskConfig ();
extern STATUS 		wvUploadStop ();
extern WV_UPLOADTASK_ID wvUploadStart ();

extern WV_LOG_HEADER_ID wvLogHeaderCreate ();
extern STATUS		wvLogHeaderUpload ();

extern STATUS wvObjInstModeSet ();
extern STATUS wvObjInst ();
extern STATUS wvSigInst ();
extern STATUS wvEvent ();

extern TASKBUF_ID	wvTaskNamesPreserve ();
extern STATUS		wvTaskNamesUpload ();
extern STATUS		wvTaskNamesBufAdd ();

#endif  /* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCwvlibh*/

