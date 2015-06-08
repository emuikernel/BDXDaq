/* wvTsfsUploadPathLibP.h - tsfs event-upload mechanism library header */

/* Copyright 1997 Wind River Systems, Inc. */

/*
modification history
--------------------
01e,27jan98,cth  removed tsfsUploadPathError prototypes, SOSENDBUFSIZE
01d,18dec97,cth  renamed again to wvTsfsUploadPathLibP.h from, updated include,
		 wvTsfsUploadPathP.h, added tsfsUploadPathLibInit prototype 
01c,16nov97,cth  renamed again to wvTsfsUploadPathP.h from tsfsUploadPathP.h
01b,16nov97,cth  changed prototypes to match new WV2.0 upload-path model
                 renamed to tsfsUploadPathP.h from evtTsfsSockLibP.h
01a,21aug97,cth  created, modified from evtSockLibP.h
*/


#ifndef __INCwvtsfsuploadpathlibph
#define __INCwvtsfsuploadpathlibph

#ifdef __cplusplus
extern "C" {
#endif


#include "private/wvUploadPathP.h"


#if defined(__STDC__) || defined(__cplusplus)

extern STATUS	 tsfsUploadPathLibInit (void);
extern UPLOAD_ID tsfsUploadPathCreate (char *ipAddress, short port);
extern void      tsfsUploadPathClose (UPLOAD_ID upId);
extern int  tsfsUploadPathWrite (UPLOAD_ID upId, char * buffer, size_t bufSize);

#else   /* __STDC__ */

extern STATUS	 tsfsUploadPathLibInit ();
extern UPLOAD_ID tsfsUploadPathCreate ();
extern void      tsfsUploadPathClose ();
extern int       tsfsUploadPathWrite ();

#endif  /* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCwvtsfsuploadpathlibph*/

