/* bkendlib.h - backend library header */

/* Copyright 1994-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01j,05mar98,c_c  Added DSA related routines.
01i,02mar98,pcn  WTX 2: changed bkendLogInit parameters.
01h,22jan98,c_c Implemented backend V2 APIs.
		Added new structure for bkEndInit_2().
		Added APIs to get backend infos.
01g,23feb96,elp added WIN32 functions prototype (suppress warnings)
01f,12may95,tpr changed bckndLog() prototype.
		changed bcknd by bkend.
		moved bkendLogXXX prototype in bkendlog.h
01e,05mar95,tpr added bckndDebugClose() prototype.
		renamed bckndDebugInit() and bckndDebug() by bckndLogInit() and
		bckndLog().
01d,24jan95,tpr changed bckndDebug() prototype.
		added bckndDebugInit() prototype.
01c,14dec94,trp added #include <sys/time.h>.
01b,22nov94,tpr added WDB_TIMEOUT and WDB_MAX_RESEND.
		added bckndDebug() prototype.
01a,03oct94,tpr written.
*/

#ifndef __INCbkendlibh
#define __INCbkendlibh

#include "flagutil.h"

#ifdef __cplusplus
extern "C" {
#endif

/* defines */

#define BKEND_TIMEOUT		1	/* default client call timeout in sec */
#define BKEND_RESEND		3	/* max number of request re-send */

#define POLL_NONE_MODE		0	/* no async event notification */
#define POLL_SELECT_MODE	1	/* selectable file desc */
#define POLL_BKROUTINE_MODE	2	/* back end own select routine */

#define BKEND_VERSION_1		0x010000	/* backend V1 define */
#define BKEND_VERSION_2		0x020000	/* backend V2 define */

/* typedefs */

/* 
 * This structure reflects the version and associated informations exchanged
 * during the new xxxInit_2() routine provided by the backend V2 and after 
 * compliant. Note that the union Info is present only for future expansion,
 * as the structure can evolve with the new versions. the tgtBkInfoSize 
 * field is present only to avoid memory out of bounds. This field should be 
 * tested by the backend before beginning to fill the fields.
*/

typedef struct _bkEndInfo          
    {
    int tgtBkInfoSize;	/* size of this structure */
    int tgtBkVersion;	/* Version of BackEnd Hiword = Major; Loword = Minor*/

    union
	{
	struct _pollingMethod
	    {
	    int bkEndPollingMethod;	/* 
					 * polling method used :
					 *	POLL_NONE_MODE
					 *	POLL_SELECT_MODE
					 *	POLL_BKROUTINE_MODE
					 */
	    union _bkEndNotifMethod
		{
		int pollingFd;	/* polling file desc for POLL_SELECT_MODE */
		int (*bkEndSelectRtn)(int timeout);   /* bkEnd select routine */
		} BKEND_NOTIF_METHOD;

	    } POLLING_METHOD;

	} INFO;

    } BKEND_INFO;

/* function declarations */

extern STATUS  bkendAttach 
    (
    char *	pBkendName		/* backend short name */
    );

/* set the DSA capability */

extern void bkendDsaEnable
    (
    STATUS      (* svcAddRtn)(u_int)	/* WDB service add routine */
    );
    
extern FLAG_DESC * bkendFlagGet (void); /* get the optionnal flag list */

/* init the backend log (if provided by the backend) */

extern STATUS   bkendLogInit	
    (
    char *	pWdbLogFileName,	/* file name to save info in */
    u_int       wdbLogMaxSize   	/* Max size for the log file */
    );

/* close the backend log (if provided by the backend) */

extern void bkendLogClose (void);

extern BKEND_INFO * bkendInfoGet (void); /* get the event management strategy */
    
extern FUNCPTR bkendInitFuncGet(void);	/* get the backend init routine */

#ifdef WIN32

/* functions to get some info from the target server */

extern char * tgtNameGet (void);
extern void targetServerRestart (void);

#endif /* WIN32 */


#ifdef __cplusplus
}
#endif

#endif /* __INCbkendlibh */

