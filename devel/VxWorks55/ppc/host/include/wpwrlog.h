/* wpwrlog.h - wpwr log utilities library */

/* Copyright 1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,19mar98,c_c  Written.
*/

#ifndef __INCwpwrlogh
#define __INCwpwrlogh

/* includes */

#include <stdarg.h>

/* defines */

#define WPWRLOG_RAW	0   /* print a message if Verbose Mode is set */
#define WPWRLOG_INFO	1   /* print a message if Verbose Mode is set */
#define WPWRLOG_WARNING	2   /* print a warning message if Verbose Mode is set */
#define WPWRLOG_ERROR	3   /* print an error if Verbose Mode is set */
#define WPWRLOG_DEBUG	4   /* print a message if Debug Mode is set */
#define WPWRLOG_ALWAYS	5   /* print message even when option '-V' is not set */
#define WPWRLOG_ACK  	6   /* print message and ask user to acknowledge */

extern void     wpwrLogHeaderSet (void);

extern void     wpwrLogRaw
(
char *fmt,
...
);

extern void     wpwrLogMsg
(
char *fmt,
...
);

extern void     wpwrLogErr
(
char *fmt,
...
);

extern void     wpwrLogWarn
(
char *fmt, ...
);

extern void     wpwrDebug
(
char *fmt,
...
);

extern void wpwrLog
    (
    int logtype,        /* what to print : Error, Warning, ... */
    char * format,      /* message format */
    ...                 /* variable arguments */
    );

extern void wpwrvLog
    (
    int flags,          /* what to print : Error, Warning, ... */
    char * format,      /* message format */
    va_list pArgs       /* variable arguments */
    );

extern void wpwrLogAck
    (
    char *	message		/* message to be acknowledged */
    );

extern void wpwrLogAckRtnSet
    (
    void (*ackRtn) (char *	message)	/* user supplied routine */
    );

extern void wpwrLogPrintRtnSet
    (
    int (*printRoutine) (char *)	/* user supplied print routine */
    );

extern BOOL wpwrDebugModeGet (void);

extern void wpwrDebugModeSet
    (
    BOOL value
    );

extern void wpwrVerboseModeSet
    (
    BOOL value
    );

extern BOOL wpwrVerboseModeGet (void);


#endif /* __INCwpwrlogh */
