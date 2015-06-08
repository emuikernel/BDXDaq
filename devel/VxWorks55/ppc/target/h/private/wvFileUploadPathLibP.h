/* wvFileUploadPathLibP.h - file upload path header */

/* Copyright 1997 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,27jan98,cth  added openFlags arg to create call, fileUpPathDefaultPerm
01b,18dec97,cth  renamed to wvFileUploadPathLibP.h from wvFileUploadPathP.h,
		 added fileUploadPathLibInit prototype, updated include
01a,16nov97,cth  written, taken from evtSockLibP.h
*/


#ifndef __INCwvfileuploadpathlibph
#define __INCwvfileuploadpathlibph

#ifdef __cplusplus
extern "C" {
#endif

#include "private/wvUploadPathP.h"


/* variable declarations */

extern int fileUpPathDefaultPerm;


/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern STATUS 	 fileUploadPathLibInit (void);
extern UPLOAD_ID fileUploadPathCreate (char *fname, int openFlags);
extern void      fileUploadPathClose (UPLOAD_ID upId);
extern int     fileUploadPathWrite (UPLOAD_ID upId, char * pData, size_t size);

#else   /* __STDC__ */

extern STATUS 	 fileUploadPathLibInit ();
extern UPLOAD_ID fileUploadPathCreate ();
extern void      fileUploadPathClose ();
extern int       fileUploadPathWrite ();

#endif  /* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCwvfileuploadpathlibph*/

