/* usrWindview.c - configuration file for Windview on the target */

/* Copyright 1997 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,13may98,pr   taken from src/config/usrWindview.c version 01g
*/

/*
DESCRIPTION
*/

/* defines */

#define WV_FILE         1               /* file upload-path type */
#define WV_SOCK         2               /* socket upload-path type */
#define WV_TSFSSOCK     3               /* tsfs socket upload-path type */

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

FUNCPTR	_func_wvOnUploadPathCreate;
FUNCPTR	_func_wvOnUploadPathClose;

int wvArgHtons;
char * wvLogMsgString1;
char * wvLogMsgString2;

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
     * Include and complete initialization for wvLib.
     */

    wvLibInit2 ();

    return (OK);
    }

/******************************************************************************
*
* wvOn - start typical instance of event logging and upload (Windview)
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

    short pathArg;

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

    if (wvArgHtons == 1)
        pathArg = (short)(htons (arg3));
    else
        pathArg = arg3;

    if (_func_wvOnUploadPathCreate != NULL)
	if ((wvUpPathId = (UPLOAD_ID)_func_wvOnUploadPathCreate ((char *) arg2, 
					               (short)pathArg)) == NULL)
        {
        logMsg ("wvOn: error creating upload path(%s%d %s%d).\n",
                (int)wvLogMsgString1,arg2, (int)wvLogMsgString2, pathArg,0,0);
        rBuffDestroy (wvBufId);
        return (ERROR);
        }

    /*
     * Initialize the event log, and let event logging routines know which
     * buffer to log events to.
     */

    wvEvtLogInit (wvBufId);

    /*
     * Start the upload task.  Uploading will be continuous after this
     * is started.
     */

    logMsg ("wvOn: wvBufId %d wvUpPathId %d.\n",(int)wvBufId, \
		(int)wvUpPathId, 0,0,0,0);
    if ((wvUpTaskId = wvUploadStart (wvBufId, wvUpPathId, TRUE)) == NULL)
        {
        logMsg ("wvOn: error launching upload.\n",0,0,0,0,0,0);
        rBuffDestroy (wvBufId);

    if (_func_wvOnUploadPathClose != NULL)
        _func_wvOnUploadPathClose (wvUpPathId);

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

    if (_func_wvOnUploadPathClose != NULL)
        _func_wvOnUploadPathClose (wvUpPathId);

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

    if (_func_wvOnUploadPathClose != NULL)
        _func_wvOnUploadPathClose (wvUpPathId);

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

    if (_func_wvOnUploadPathClose != NULL)
        _func_wvOnUploadPathClose (wvUpPathId);

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

