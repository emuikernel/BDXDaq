/* ftpLib.h - arpa File Transfer Protocol library header */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01n,06jun02,elr  Change return for ftpTransientFatalInstall()
01m,23may02,elr  added temporary global flag to bypass PASSIVE mode
01l,22may02,elr  Changed API of ftpCommandEnhanced() and ftpReplyGetEnhanced()
                 for PASSIVE mode support
                 Added function ftpDataConnInitPassiveMode ()
                 Added ftpLibDebugOptionsSet()
01k,12mar02,elr  Added error return codes
                 Replaced ftpErrorSuppress with ftplDebug (SPR 71496)
01j,22sep92,rrr  added support for c++
01i,11sep92,jmm  added external definition for ftpErrorSupress (for spr #1257)
01h,04jul92,jcf  cleaned up.
01g,26may92,rrr  the tree shuffle
01f,04oct91,rrr  passed through the ansification filter
		  -fixed #else and #endif
		  -changed copyright notice
01e,19oct90,shl  changed ftpCommand() to use variable length argument list.
01d,05oct90,shl  added ANSI function prototypes.
                 made #endif ANSI style.
                 added copyright notice.
01c,07aug90,shl  added INCftpLibh to #endif.
01b,20mar87,dnw  prepended FTP_ to reply codes.
01a,07nov86,dnw  written
*/

#ifndef __INCftpLibh
#define __INCftpLibh

#ifdef __cplusplus
extern "C" {
#endif

/* For debugging options */

#define FTPL_DEBUG_OFF          0 /* No debugging messages */
#define FTPL_DEBUG_INCOMING     1 /* Show all incoming responses */
#define FTPL_DEBUG_OUTGOING     2 /* Show all outgoing commands */
#define FTPL_DEBUG_ERRORS       4 /* Display all errors and warnings that occur */

/* For FTP specification see RFC-765 */

/* Reply codes for ftpReplyGet(). (Major numbers 1xx-5xx) */

#define FTP_PRELIM              1 /* positive preliminary */
#define FTP_COMPLETE            2 /* positive completion */
#define FTP_CONTINUE            3 /* positive intermediate */
#define FTP_TRANSIENT           4 /* transient negative completion */
#define FTP_ERROR               5 /* permanent negative completion */

/* Detailed reply codes for ftpReplyGetEnhanced() */

#define FTP_PASSIVE_REPLY            227 /* Command not supported */
#define FTP_COMMAND_NOT_SUPPORTED    502 /* Command not supported */
#define FTP_NOACTION    550       /* requested action not taken */

/* Type codes */

#define	TYPE_A		1	/* ASCII */
#define	TYPE_E		2	/* EBCDIC */
#define	TYPE_I		3	/* image */
#define	TYPE_L		4	/* local byte size */

/* Form codes */

#define	FORM_N		1	/* non-print */
#define	FORM_T		2	/* telnet format effectors */
#define	FORM_C		3	/* carriage control (ASA) */

/* Structure codes */

#define	STRU_F		1	/* file (no record structure) */
#define	STRU_R		2	/* record structure */
#define	STRU_P		3	/* page structure */

/* Mode types */

#define	MODE_S		1	/* stream */
#define	MODE_B		2	/* block */
#define	MODE_C		3	/* compressed */

/* Record Tokens */

#define	REC_ESC		'\377'	/* Record-mode Escape */
#define	REC_EOR		'\001'	/* Record-mode End-of-Record */
#define REC_EOF		'\002'	/* Record-mode End-of-File */

/* Block Header */

#define	BLK_EOR		0x80	/* Block is End-of-Record */
#define	BLK_EOF		0x40	/* Block is End-of-File */
#define BLK_ERRORS	0x20	/* Block is suspected of containing errors */
#define	BLK_RESTART	0x10	/* Block is Restart Marker */

#define	BLK_BYTECOUNT	2	/* Bytes in this block */

/* error values */

/* 
 * Note that for unexpected responses, the low-order byte of the eror code 
 * contains the reponse code.
 */
#define S_ftpLib_ILLEGAL_VALUE                  (M_ftpLib | 1)
#define S_ftpLib_TRANSIENT_RETRY_LIMIT_EXCEEDED (M_ftpLib | 2)
#define S_ftpLib_FATAL_TRANSIENT_RESPONSE       (M_ftpLib | 3)

#define S_ftpLib_REMOTE_SERVER_STATUS_221       (M_ftpLib | 221)
#define S_ftpLib_REMOTE_SERVER_STATUS_226       (M_ftpLib | 226)
#define S_ftpLib_REMOTE_SERVER_STATUS_257       (M_ftpLib | 257)
#define S_ftpLib_REMOTE_SERVER_ERROR_422        (M_ftpLib | 422)
#define S_ftpLib_REMOTE_SERVER_ERROR_425        (M_ftpLib | 425)
#define S_ftpLib_REMOTE_SERVER_ERROR_450        (M_ftpLib | 450)
#define S_ftpLib_REMOTE_SERVER_ERROR_451        (M_ftpLib | 451)
#define S_ftpLib_REMOTE_SERVER_ERROR_452        (M_ftpLib | 452)
#define S_ftpLib_REMOTE_SERVER_ERROR_500        (M_ftpLib | 500)
#define S_ftpLib_REMOTE_SERVER_ERROR_501        (M_ftpLib | 501)
#define S_ftpLib_REMOTE_SERVER_ERROR_502        (M_ftpLib | 502)
#define S_ftpLib_REMOTE_SERVER_ERROR_503        (M_ftpLib | 503)
#define S_ftpLib_REMOTE_SERVER_ERROR_504        (M_ftpLib | 504)
#define S_ftpLib_REMOTE_SERVER_ERROR_520        (M_ftpLib | 520)
#define S_ftpLib_REMOTE_SERVER_ERROR_521        (M_ftpLib | 521)
#define S_ftpLib_REMOTE_SERVER_ERROR_530        (M_ftpLib | 530)
#define S_ftpLib_REMOTE_SERVER_ERROR_550        (M_ftpLib | 550)
#define S_ftpLib_REMOTE_SERVER_ERROR_551        (M_ftpLib | 551)
#define S_ftpLib_REMOTE_SERVER_ERROR_552        (M_ftpLib | 552)
#define S_ftpLib_REMOTE_SERVER_ERROR_553        (M_ftpLib | 553)
#define S_ftpLib_REMOTE_SERVER_ERROR_554        (M_ftpLib | 554)

/* externals */

extern BOOL ftplDebug;        /*  print error message, defined in funcBind.c */

/* disable passive mode - XXX this will be removed in the next release XXX */

extern BOOL ftplPasvModeDisable; 

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern STATUS ftpLogin (int ctrlSock, char *user, char *passwd, char *account);
extern STATUS ftpXfer (char *host, char *user, char *passwd, char *acct,
                       char *cmd, char *dirname, char *filename,
                       int *pCtrlSock, int *pDataSock);
extern int ftpCommand (int ctrlSock, char *fmt, int arg1, int arg2,
                       int arg3, int arg4, int arg5, int arg6);
extern int ftpCommandEnhanced (int ctrlSock, char *fmt, int arg1, int arg2,
                               int arg3, int arg4, int arg5, int arg6,
                               char *replyString,
                               int replyStringLength);
extern int ftpDataConnGet (int dataSock);
extern int ftpDataConnInit (int ctrlSock);
extern int ftpDataConnInitPassiveMode (int ctrlSock);
extern int ftpHookup (char *host);
extern void ftpLibDebugOptionsSet (UINT32 options);
extern int ftpReplyGet (int ctrlSock, BOOL expecteof);
extern int ftpReplyGetEnhanced (int ctrlSock, BOOL expecteof, char *replyString,
                                int replyStringLength);
extern STATUS ftpTransientConfigSet (UINT32 maxRetryCount, UINT32 retryInterval);
extern STATUS ftpTransientConfigGet (UINT32 *maxRetryCount, UINT32 *retryInterval);
extern STATUS ftpTransientFatalInstall (FUNCPTR configlette);
#else  /* __STDC__ */

extern STATUS ftpLogin ();
extern STATUS ftpXfer ();
extern int ftpCommand ();
extern int ftpCommandEnhanced ();
extern int ftpDataConnGet ();
extern int ftpDataConnInit ();
extern int ftpDataConnInitPassiveMode ();
extern int ftpHookup ();
extern void ftpLibDebugOptionsSet ();
extern int ftpReplyGet ();
extern int ftpReplyGetEnhanced ();
extern STATUS ftpPrelimConfigSet ();
extern STATUS ftpTransientConfigSet ();
extern STATUS ftpLogin ();
extern STATUS ftpXfer ();
extern STATUS ftpTransientFatalInstall ();

#endif /* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCftpLibh */
