/* wvUploadPathP.h - generic upload-path definition for windview */

/* Copyright 1997 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,18dec97,cth  moved to private, renamed to wvUploadPathP.h, added comments
01a,16nov97,cth  written.
*/

/*
This file contains the definition of a structure that should make up the
core of any upload-path id that can be passed to wvLib.  Windview uses a
pointer to this type of structure to access the path's routines.
*/

#ifndef __INCwvuploadpathph
#define __INCwvuploadpathph


#ifdef __cplusplus
extern "C" {
#endif


/* typedefs */

typedef struct uploadPathDesc	/* UPLOAD_DESC */
    {
    int (* writeRtn) ();	/* write to the upload path */
    int (* errorRtn) ();	/* signal upload path that an error occurred */
    } UPLOAD_DESC;

typedef UPLOAD_DESC *UPLOAD_ID;


#ifdef __cplusplus
}
#endif

#endif /* __INCwvuploadpathph*/
