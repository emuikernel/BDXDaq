
/* usrFtp.c - Support for FTP client */

/* Copyright 2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,23may02,elr  ftpLibDebugLevelSet (FTP_DEBUG_LEVEL) is now
                 ftpLibDebugOptionsSet (FTP_DEBUG_OPTIONS)
01a,20dec06,elr  created
*/

/*
DESCRIPTION
This file is used to include support for the ftp client.

NOMANUAL
*/

IMPORT UINT32 ftplTransientMaxRetryCount;
IMPORT UINT32 ftplTransientRetryInterval;
IMPORT STATUS ftpTransientFatalInstall();
IMPORT STATUS ftplDebugOptionsSet();

/*******************************************************************************
*
* ftpTransientFatal - example applette to terminate FTP transient host responses
*
* ftpXfer will normally retry a command if the host responds with a 4xx
*     reply.   If this applette is installed,  it can immediately terminate
*     the retry sequence.
*
*
* RETURNS : TRUE - Terminate retry attempts
*           FALSE - Continue retry attempts
*
*
* SEE ALSO : ftpTransientFatalInstall(), ftpTransientConfigSet()
*
*/

LOCAL BOOL ftpTransientFatal 
    (
    UINT32 reply /* Three digit code defined in RFC #640 */
    )
    {
    switch (reply)
        {
        case (421): /* Service not available */
        case (450): /* File unavailable */
        case (451): /* error in processing */
        case (452): /* insufficient storage */
            { 
            /* yes, these are actually non-recoverable replies */
            return (TRUE); 
            break;
            }
            /* attempt to retry the last command */
        default:
        return (FALSE); 
        }
    }

/******************************************************************************
*
* usrFtpInit - initialize FTP parameters
*
* This routine configures ftp client parameters.
* defined by the user via the project facility.
*
* RETURNS: OK or ERROR
*
* NOMANUAL
*/

STATUS usrFtpInit (void)
    {
    ftplTransientMaxRetryCount = FTP_TRANSIENT_MAX_RETRY_COUNT;
    ftplTransientRetryInterval = FTP_TRANSIENT_RETRY_INTERVAL;
    ftpTransientFatalInstall ( FTP_TRANSIENT_FATAL );
    ftpLibDebugOptionsSet ( FTP_DEBUG_OPTIONS );

    return OK;
    }
