/* usrWindview.c - configuration file for Windview on the target */

/* Copyright 1997 Wind River Systems, Inc. */

/*
modification history
--------------------
01i,03mar99,dgp  update to reference project facility instead of config.h
01h,28aug98,dgp  FCS man page edit
01g,13may98,cth  changed minimum buffers, added wvUploadStop to error handling
01f,06may98,dgp  clean up man pages for WV 2.0 beta release
01e,20apr98,cth  changed rbuff options, moved wvEvtLogInit, added 
		 wvLogHeaderCreate and Upload
01d,17apr98,cth  added partition id to wvEvtLogInit
01c,15apr98,cth  more clean up
01b,27jan98,cth  cleaned up and made work
01a,18dec97,cth  written
*/

/*
DESCRIPTION
This file is included in usrConfig.c.

This file contains routines that configure and initialize Windview's target
components based on directives in the configuration files.  The routines
wvOn() and wvOff() are also defined here. They provide an easy way to
start and stop a typical instance of Windview from a shell, rather than
using the Windview-host user interface.  wvOn() and wvOff() simplify the 
process of starting and stopping Windview by bundling the calls necessary 
to do so into a single routine.  The Windview host GUI does not call wvOn()
or wvOff() explicitly, so it is possible to modify them to meet users'
specific requirements.

INTERNAL
The typical instance of Windview includes using rBuff buffers; one of file, 
socket, or TSFS-socket upload paths (determined by the value of 
WV_DEFAULT_UPLOAD_TYPE set below); and uploading events continuously.  
There are several upload paths that can be configured into the vxWorks
image.  The upload paths are included by defining INCLUDE_WVUPLOAD_<type>
in configAll.h.  It is common to include more than one type of path,
allowing the host user interface to be used to select between the types
without recompiling the target.  If no upload path is explicitly included
within configAll.h, then INCLUDE_WVUPLOAD_FILE is added by default.  See 
usrDepend.c for other dependencies.

SEE ALSO
.I WindView User's Guide
*/

#ifndef __INCusrWindviewc
#define __INCusrWindviewc


#include "vxWorks.h"
#include "inetLib.h"
#include "logLib.h"
#include "wvLib.h"
#include "wvTmrLib.h"
#include "rBuffLib.h"

#include "private/wvBufferP.h"
#include "private/wvUploadPathP.h"
#include "private/seqDrvP.h"
#include "private/funcBindP.h"
#include "private/wvFileUploadPathLibP.h"
#include "private/wvSockUploadPathLibP.h"
#include "private/wvTsfsUploadPathLibP.h"

/* defines */

#define WV_FILE         1               /* file upload-path type */
#define WV_SOCK         2               /* socket upload-path type */
#define WV_TSFSSOCK     3               /* tsfs socket upload-path type */


#define WV_DEFAULT_BUF_MIN              4
#define WV_DEFAULT_BUF_MAX              10
#define WV_DEFAULT_BUF_SIZE             0x8000
#define WV_DEFAULT_BUF_THRESH           0x4000
#define WV_DEFAULT_BUF_OPTIONS          0x0


/* 
 * Choose an upload path created/destroyed by wvOn/wvOff, based on the 
 * included libraries.  The order of choice is File, then TSFS socket, 
 * then socket, etc.
 */

#ifdef INCLUDE_WVUPLOAD_FILE
#define WV_DEFAULT_UPLOAD_TYPE          WV_FILE
#endif

#ifdef INCLUDE_WVUPLOAD_TSFSSOCK
#if    !defined (WV_DEFAULT_UPLOAD_TYPE)
#define WV_DEFAULT_UPLOAD_TYPE		WV_TSFSSOCK
#endif
#endif

#ifdef INCLUDE_WVUPLOAD_SOCK
#if    !defined (WV_DEFAULT_UPLOAD_TYPE)
#define WV_DEFAULT_UPLOAD_TYPE		WV_SOCK
#endif
#endif


/* globals */

int wvDefaultBufSize;		/* default size of a buffer */
int wvDefaultBufMax;		/* default max number of buffers */
int wvDefaultBufMin;		/* default min number of buffers */
int wvDefaultBufThresh;		/* default threshold for uploading, in bytes */
int wvDefaultBufOptions;	/* default rBuff options */

rBuffCreateParamsType   wvDefaultRBuffParams;  /* contains the above values */

BUFFER_ID	  wvBufId;	/* event-buffer id used by wvOn/Off */
UPLOAD_ID	  wvUpPathId;	/* upload-path id used by wvOn/Off */
WV_UPLOADTASK_ID  wvUpTaskId;	/* upload-task id used by wvOn/Off */
WV_LOG_HEADER_ID  wvLogHeader;	/* the event-log header */



/* forward declarations */

int wvRBuffErrorHandler (void);


/******************************************************************************
*
* windviewConfig - configure and initialize Windview components
*
* NOMANUAL
*
*/

STATUS windviewConfig (void)
    {

    /*
     * This is not mandatory at this point, but by setting these global 
     * variables to the default values here, they can be changed from the
     * shell before wvOn/wvOff use them.
     */

    wvDefaultBufSize     	= WV_DEFAULT_BUF_SIZE;
    wvDefaultBufMax	 	= WV_DEFAULT_BUF_MAX;
    wvDefaultBufMin	 	= WV_DEFAULT_BUF_MIN;
    wvDefaultBufThresh	 	= WV_DEFAULT_BUF_THRESH;
    wvDefaultBufOptions	 	= WV_DEFAULT_BUF_OPTIONS;

    wvDefaultRBuffParams.minimum	  = wvDefaultBufMin;
    wvDefaultRBuffParams.maximum	  = wvDefaultBufMax;
    wvDefaultRBuffParams.buffSize	  = wvDefaultBufSize;
    wvDefaultRBuffParams.threshold	  = wvDefaultBufThresh;
    wvDefaultRBuffParams.options	  = wvDefaultBufOptions;

    wvDefaultRBuffParams.sourcePartition  = memSysPartId;
    wvDefaultRBuffParams.errorHandler	  = wvRBuffErrorHandler;

    /* 
     * Initialize timestamp support with user defined, system timer, or
     * sequential, depending on configAll directives.  Sequential will
     * be the default if user or system is not requested.
     */

#ifdef  INCLUDE_TIMESTAMP
#ifdef  INCLUDE_USER_TIMESTAMP
    wvTmrRegister ((UINTFUNCPTR) USER_TIMESTAMP,
                   (UINTFUNCPTR) USER_TIMESTAMPLOCK,
                   (FUNCPTR)     USER_TIMEENABLE,
                   (FUNCPTR)     USER_TIMEDISABLE,
                   (FUNCPTR)     USER_TIMECONNECT,
                   (UINTFUNCPTR) USER_TIMEPERIOD,
                   (UINTFUNCPTR) USER_TIMEFREQ);
#else   /* INCLUDE_USER_TIMESTAMP */
    wvTmrRegister (sysTimestamp,
                   sysTimestampLock,
                   sysTimestampEnable,
                   sysTimestampDisable,
                   sysTimestampConnect,
                   sysTimestampPeriod,
                   sysTimestampFreq);
#endif  /* INCLUDE_USER_TIMESTAMP */
#else   /* INCLUDE_TIMESTAMP */
    wvTmrRegister (seqStamp,
                   seqStampLock,
                   seqEnable,
                   seqDisable,
                   seqConnect,
                   seqPeriod,
                   seqFreq);
#endif  /* INCLUDE_TIMESTAMP */

    /*
     * Include and complete initialization for wvLib.
     */

    wvLibInit2 ();

    /*
     * Include and initialize rBuffLib as the event buffer.
     */

    rBuffLibInit ();

    /* 
     * Pull in upload-path functions. Any or all of the types of upload
     * paths can be included in the final vxWorks in order that the
     * Windview host GUI can create whichever type the user requests.
     */

#ifdef INCLUDE_WVUPLOAD_FILE
    fileUploadPathLibInit ();
#endif
#ifdef INCLUDE_WVUPLOAD_SOCK
    sockUploadPathLibInit ();
#endif
#ifdef INCLUDE_WVUPLOAD_TSFSSOCK
    tsfsUploadPathLibInit ();
#endif

    return (OK);
    }

/******************************************************************************
*
* wvOn - start a typical instance of event logging and upload (Windview)
*
* This routine starts a typical instance of WindView event logging and event
* upload.  rBuff buffers are used to store events.  The file upload path is
* TSFS-socket, socket, or file.  Event data is uploaded to the path 
* continuously.
*
* Event logging is started for events belonging to <class>.  The rBuff buffer
* is created using the values in 'wvDefaultRBuffParams', which are taken from
* the globals 'wvDefaultBufxxx', configurable from usrWindview.c.  The upload
* path used to transfer events away from the event buffer is determined by
* the value of WV_DEFAULT_UPLOAD_TYPE, also set in usrWindview.c to one 
* of WV_FILE, WV_TSFSSOCK or WV_SOCK.  The upload path is opened using the 
* remaining parameters as follows:
*
* .TS
* tab(|)
* lf3 lf3 lf3
* l l l.
*         | WV_FILE        | WV_TSFSOCK / WV_SOCK
* _
* arg2    | char *filename | char *hostIp
* arg3    | openFlags      | short port
* arg4    | not used       | not used
* .TE
*
* The arguments are of generic type and name to allow the addition of new
* types of upload paths or customization of wvOn().
*
* RETURNS: OK, or ERROR if the upload path can not be created, the upload task 
* can not be created, or the rBuff buffer can not be created.
*
* SEE ALSO: wvOff()
*
*/

STATUS wvOn 
    (
    int class,		/* class of events to be logged */
    int arg2,		/* filename, hostIp, etc. */
    int arg3,		/* port number, openFlags etc. */
    int arg4		/* for future expansion */
    )
    {

    /* Create a buffer. */

    if ((wvBufId = rBuffCreate (& wvDefaultRBuffParams)) == NULL)
        {
        logMsg ("wvOn: error creating buffer.\n",0,0,0,0,0,0);
        return (ERROR);
        }

    /* 
     * Create a path for events to be uploaded to the host.  If more than
     * one UploadPath library is compiled into vxWorks, the fileUploadPath
     * will be the one used.
     */

#if (WV_DEFAULT_UPLOAD_TYPE == WV_SOCK)

    if ((wvUpPathId = sockUploadPathCreate ((char *) arg2, 
				            (short)(htons (arg3)))) == NULL)
        {
        logMsg ("wvOn: error creating upload path(ip=%s port=%d).\n",
                arg2, arg3,0,0,0,0);
        rBuffDestroy (wvBufId);
        return (ERROR);
        }
#endif
#if (WV_DEFAULT_UPLOAD_TYPE == WV_TSFSSOCK)

    if ((wvUpPathId = tsfsUploadPathCreate ((char *) arg2, 
					    (short)(htons (arg3)))) == NULL)
        {
        logMsg ("wvOn: error creating upload path(ip=%s port=%d).\n",
                arg2, arg3,0,0,0,0);
        rBuffDestroy (wvBufId);
        return (ERROR);
        }
#endif
#if (WV_DEFAULT_UPLOAD_TYPE == WV_FILE)

    if ((wvUpPathId = fileUploadPathCreate ((char *) arg2, arg3)) == NULL)
        {
        logMsg ("wvOn: error creating upload path(fname=%s).\n",
                arg2,0,0,0,0,0);
        rBuffDestroy (wvBufId);
        return (ERROR);
        }
#endif

    /*
     * Initialize the event log, and let event logging routines know which
     * buffer to log events to.
     */

    wvEvtLogInit (wvBufId);

    /*
     * Start the upload task.  Uploading will be continuous after this
     * is started.
     */

    if ((wvUpTaskId = wvUploadStart (wvBufId, wvUpPathId, TRUE)) == NULL)
        {
        logMsg ("wvOn: error launching upload.\n",0,0,0,0,0,0);
        rBuffDestroy (wvBufId);

#if (WV_DEFAULT_UPLOAD_TYPE == WV_SOCK)
        sockUploadPathClose (wvUpPathId);
#endif
#if (WV_DEFAULT_UPLOAD_TYPE == WV_TSFSSOCK)
        tsfsUploadPathClose (wvUpPathId);
#endif
#if (WV_DEFAULT_UPLOAD_TYPE == WV_FILE)
        fileUploadPathClose (wvUpPathId);
#endif
        return (ERROR);
        }

    /*
     * Capture a log header, including task names active in the system.
     */

    if ((wvLogHeader = wvLogHeaderCreate (memSysPartId)) == NULL)
        {
        logMsg ("wvOn: error creating log header.\n",0,0,0,0,0,0);
	wvUploadStop (wvUpTaskId);
        rBuffDestroy (wvBufId);

#if (WV_DEFAULT_UPLOAD_TYPE == WV_SOCK)
        sockUploadPathClose (wvUpPathId);
#endif
#if (WV_DEFAULT_UPLOAD_TYPE == WV_TSFSSOCK)
        tsfsUploadPathClose (wvUpPathId);
#endif
#if (WV_DEFAULT_UPLOAD_TYPE == WV_FILE)
        fileUploadPathClose (wvUpPathId);
#endif
        return (ERROR);
        }

    /*
     * Upload the header immediately, because we are uploading continuously.
     */

    if (wvLogHeaderUpload (wvLogHeader, wvUpPathId) != OK)
        {
        logMsg ("wvOn: error uploading log header.\n",0,0,0,0,0,0);
	wvUploadStop (wvUpTaskId);
        rBuffDestroy (wvBufId);

#if (WV_DEFAULT_UPLOAD_TYPE == WV_SOCK)
        sockUploadPathClose (wvUpPathId);
#endif
#if (WV_DEFAULT_UPLOAD_TYPE == WV_TSFSSOCK)
        tsfsUploadPathClose (wvUpPathId);
#endif
#if (WV_DEFAULT_UPLOAD_TYPE == WV_FILE)
        fileUploadPathClose (wvUpPathId);
#endif
        return (ERROR);
	}

    /*
     * Turn on event logging.
     */

    wvEvtClassSet (class);
    wvEvtLogStart ();

    return (OK);
    }

/******************************************************************************
*
* wvOff - stop a typical instance of event logging and upload (Windview)
*
* This routine stops event logging and uploading of events that was started
* with wvOn().  First, event logging is stopped.  Next the event upload task 
* flushes the buffers and is then deleted.  Finally, the upload path is 
* closed and the event buffer is deleted.
*
* The type of upload path is determined by the value of WV_DEFUALT_UPLOAD_TYPE,
* set in usrWindview.c.  The upload task ID, buffer ID, and upload path ID are 
* stored in the 'wvUpTaskId', 'wvBufId', and 'wvUpPathId' globals, respectively.
*
* RETURNS: N/A 
*
* SEE ALSO: wvOn()
*
*/

void wvOff (void)
    {

    /* Stop event logging. */

    wvEvtLogStop ();

    /* 
     * Stop continuous upload of events.  This will block until the buffer
     * is emptied.
     */

    if (wvUploadStop (wvUpTaskId) == ERROR)
	{
	logMsg ("wvOff: error killing upload task.\n", 0,0,0,0,0,0);
	}

    /* Close the upload path. */

#if (WV_DEFAULT_UPLOAD_TYPE == WV_SOCK)
        sockUploadPathClose (wvUpPathId);
#endif
#if (WV_DEFAULT_UPLOAD_TYPE == WV_TSFSSOCK)
        tsfsUploadPathClose (wvUpPathId);
#endif
#if (WV_DEFAULT_UPLOAD_TYPE == WV_FILE)
        fileUploadPathClose (wvUpPathId);
#endif

    /* Destroy the event buffer. */

    if (wvBufId != NULL)
        rBuffDestroy (wvBufId);
    }

/*******************************************************************************
* wvRBuffErrorHandler - error handler for the rBuffer created by  wvOn,wvOff
*
* NOMANUAL
*
*/
int wvRBuffErrorHandler (void)
    {
    logMsg ("wvRBuffErrorHandler: error occurred.\n", 0,0,0,0,0,0);
    return OK;
    }


#endif /* __INCusrWindviewc */
