/* wvSockUploadPathLibP.h - socket upload path library header */

/* Copyright 1997 Wind River Systems, Inc. */

/*
modification history
--------------------
01e,27jan98,cth  removed sockUploadPathError prototypes, SOSENDBUFSIZE, added
		 sockUpPathSendBufSize
01d,18dec97,cth  renamed again to wvSockUploadPathLibP.h from, updated include, 
		 wvSockUploadPathP.h, added sockUploadPathLibInit prototype
01c,16nov97,cth  renamed again from sockUploadPathP.h to wvSockUploadPathP.h
01b,16nov97,cth  changed prototypes to match WV2.0 upload-path model
                 renamed from evtSockLibP.h to sockUploadPathP.h
01a,10dec93,smb  written.
*/


#ifndef __INCwvsockuploadpathlibph
#define __INCwvsockuploadpathlibph

#ifdef __cplusplus
extern "C" {
#endif

#include "private/wvUploadPathP.h"


/* variable declarations */

extern int sockUpPathSendBufSize;


/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern STATUS	 sockUploadPathLibInit (void);
extern UPLOAD_ID sockUploadPathCreate (char *ipAddress, short port);
extern void      sockUploadPathClose (UPLOAD_ID upId);
extern int     sockUploadPathWrite (UPLOAD_ID upId, char * pData, size_t size);

#else   /* __STDC__ */

extern STATUS	 sockUploadPathLibInit ();
extern UPLOAD_ID sockUploadPathCreate ();
extern void      sockUploadPathClose ();
extern int       sockUploadPathWrite ();

#endif  /* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCwvsockuploadpathlibph*/

